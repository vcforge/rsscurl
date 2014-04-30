// RSSCurl.cpp: implementation of the CRSSCurl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RSSCurl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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

	std::ifstream ifsXSL("xsl/RSS_20_xsl.txt", std::ios_base::binary | std::ios_base::in);

	if( ifsXSL )
	{
		ifsXSL.get(*m_ssXSL.rdbuf(), '\0');
		m_ssXSL.seekg(std::stringstream::beg);
	}

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

		if( _TransformFeed(m_strResponseData.c_str(), m_ssXSL, strResult) )
			_CreateEntries(m_strResponseData.c_str());
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
		return true;
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
}
