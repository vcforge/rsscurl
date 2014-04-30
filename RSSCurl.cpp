// RSSCurl.cpp: implementation of the CRSSCurl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RSSCurl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct _tagEntry
{
	_tagEntry(const xercesc_2_8::DOMNode* const pNode);

	std::string strUniqueId;
	bool bIsLive;

private:
	std::map<std::string, std::string> mapstrEntryElements;
};

std::string _XmlCharsToStdString(const XMLCh *const xmlChars)
{
	char* stdChars = xercesc_2_8::XMLString::transcode(xmlChars);
	const std::string strChars = std::string(stdChars);

	xercesc_2_8::XMLString::release(&stdChars);
	return strChars;
}

void _LoadXSLFile(LPCSTR lpszXSLFile, std::stringstream &ssXSL)
{
	std::ifstream ifsXSL(lpszXSLFile, std::ios_base::binary | std::ios_base::in);

	if( ifsXSL )
	{
		ifsXSL.get(*ssXSL.rdbuf(), '\0');
		ssXSL.seekg(std::stringstream::beg);
	}
}

CRSSCurl::CRSSCurl()
{
	m_pCURL = curl_easy_init();

	xercesc_2_8::XMLPlatformUtils::Initialize();
	xalanc_1_10::XalanTransformer::initialize();

	curl_easy_setopt(m_pCURL, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(m_pCURL, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(m_pCURL, CURLOPT_SSL_VERIFYPEER, 0);	// disable certificates
	curl_easy_setopt(m_pCURL, CURLOPT_ERRORBUFFER, m_szErrorBuffer);	// error buffer
	curl_easy_setopt(m_pCURL, CURLOPT_FOLLOWLOCATION, 1);	// follow redirects
	curl_easy_setopt(m_pCURL, CURLOPT_MAXREDIRS, 10);	// number of redirects to follow
	curl_easy_setopt(m_pCURL, CURLOPT_LOW_SPEED_LIMIT, 100);
	curl_easy_setopt(m_pCURL, CURLOPT_LOW_SPEED_TIME, 300);

	curl_easy_setopt(m_pCURL, CURLOPT_WRITEFUNCTION, &s_WriteFunction);
	curl_easy_setopt(m_pCURL, CURLOPT_WRITEDATA, this);

	_LoadXSLFile("xsl/RSS_20_xsl.txt", m_ssRSS20XSL);
	_LoadXSLFile("xsl/Atom_10_xsl.txt", m_ssAtom10XSL);

}

CRSSCurl::~CRSSCurl()
{
	xalanc_1_10::XalanTransformer::terminate();
	xercesc_2_8::XMLPlatformUtils::Terminate();
	if( m_pCURL != NULL )
	{
		curl_easy_cleanup(m_pCURL);
		m_pCURL = NULL;
	}

}

void CRSSCurl::Refresh(LPCSTR lpszUrl)
{
	// Set http request and url
	curl_easy_setopt(m_pCURL, CURLOPT_URL, lpszUrl);

	if( curl_easy_perform(m_pCURL) == CURLE_OK )
	{
		std::string strResult;

		if( _TransformFeed(m_strResponseData.c_str(), m_ssRSS20XSL, strResult) )	// rss 2.0
		{
		}
		else if( _TransformFeed(m_strResponseData.c_str(), m_ssAtom10XSL, strResult) )	// Atom 1.0
		{
		}
	}

}

_tagEntry::_tagEntry(const xercesc_2_8::DOMNode *const pNode)
{
	const xercesc_2_8::DOMNodeList* const pChildNodes = pNode->getChildNodes();
	XMLSize_t cChildNodes = pChildNodes->getLength();

	for( XMLSize_t nNode = 0; nNode < cChildNodes; nNode++ )
	{
		const xercesc_2_8::DOMNode* const pChildNode = pChildNodes->item(nNode);
		const std::string strNodeName = _XmlCharsToStdString(pChildNode->getNodeName());
		const std::string strTextContent = _XmlCharsToStdString(pChildNode->getTextContent());

		if( strNodeName == "#text" )
			continue;
		if( xercesc_2_8::XMLString::equals(strNodeName.c_str(), "entryUniqueId") )
		{
			strUniqueId = strTextContent;
			mapstrEntryElements["entryUniqueId"] = strTextContent;
		}
		else if( xercesc_2_8::XMLString::equals(strNodeName.c_str(), "publishDateTime") )
		{
			mapstrEntryElements["publishDateTime"] = strTextContent;
		}
		else
			mapstrEntryElements[strNodeName] = strTextContent;
		if( strUniqueId.empty() )
			strUniqueId = mapstrEntryElements["url"];
	}

}

int CRSSCurl::s_WriteFunction(void *ptr, size_t size, size_t nmemb, void *stream)
{
	CRSSCurl* pRSSCurl = (CRSSCurl*)stream;
	char* pszBuffer = (char*)ptr;

	pRSSCurl->m_strResponseData.append(pszBuffer, size * nmemb);
	return size * nmemb;

}

bool CRSSCurl::_TransformFeed(LPCSTR lpszInput, std::stringstream &ssXSL, std::string &strResult)
{
	std::stringstream ssResult;
	xalanc_1_10::XalanTransformer xslTransformer;
	std::stringstream ssInput(lpszInput);
	const xalanc_1_10::XSLTInputSource XMLInputSource(ssInput);
	const xalanc_1_10::XSLTInputSource XSLInputSource(ssXSL);
	const xalanc_1_10::XSLTResultTarget XMLResultTarget(ssResult);
	int iResult = -1;

	iResult = xslTransformer.transform(XMLInputSource, XSLInputSource, XMLResultTarget);
	if( iResult == 0 )
	{
		strResult = ssResult.str();
		return !strResult.empty();
	}
	return false;
}

void CRSSCurl::_CreateEntries(LPCSTR lpszInput)
{
	const xercesc_2_8::MemBufInputSource XMLInputSource(reinterpret_cast<const XMLByte*>(lpszInput), strlen(lpszInput), "");
	xercesc_2_8::XercesDOMParser xmlParser;

	xmlParser.setValidationScheme(xercesc_2_8::XercesDOMParser::Val_Never);
	xmlParser.parse(XMLInputSource);
	if( !xmlParser.getDocument()->hasChildNodes() || !xmlParser.getDocument()->getChildNodes()->item(0)->hasChildNodes() )
		return;

	const xercesc_2_8::DOMNodeList* const pChildNodes = xmlParser.getDocument()->getChildNodes()->item(0)->getChildNodes();
	XMLSize_t cChildNodes = pChildNodes->getLength();

	for( XMLSize_t nNode = 0; nNode < cChildNodes; nNode++ )
	{
		const xercesc_2_8::DOMNode* const pChildNode = pChildNodes->item(nNode);
		const std::string strNodeName = _XmlCharsToStdString(pChildNode->getNodeName());

		if( strNodeName == "#text" )
			continue;
		if( xercesc_2_8::XMLString::equals(strNodeName.c_str(), "entry") )
		{
			struct _tagEntry Entry(pChildNode);

			for( int nEntry = 0; nEntry < m_vEntries.size(); nEntry++ )
			{
				if( m_vEntries[nEntry].strUniqueId == Entry.strUniqueId )
					break;
			}
			if( nEntry >= m_vEntries.size() )
				m_vEntries.push_back(Entry);
		}
		else
		{
			const std::string strTextContent = _XmlCharsToStdString(pChildNode->getTextContent());
		}
	}

}
