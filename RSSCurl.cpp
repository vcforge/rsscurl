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

	std::ifstream ifsXSLT("xsl/RSS_20_xsl.txt", std::ios_base::binary | std::ios_base::in);

	if( ifsXSLT )
	{
		ifsXSLT.get(*m_ssXSLT.rdbuf(), '\0');
		m_ssXSLT.seekg(std::stringstream::beg);
	}

}

CRSSCurl::~CRSSCurl()
{
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

		_TransformFeed(m_ssXSLT, strResult);
	}

}

int CRSSCurl::s_WriteFunction(void *ptr, size_t size, size_t nmemb, void *stream)
{
	CRSSCurl* pRSSCurl = (CRSSCurl*)stream;
	char* pszBuffer = (char*)ptr;

	pRSSCurl->m_strResponseData.append(pszBuffer, size * nmemb);
	return size * nmemb;

}

void CRSSCurl::_TransformFeed(std::stringstream &ssXSLT, std::string &strResult)
{
}
