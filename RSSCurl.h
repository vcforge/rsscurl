// RSSCurl.h: interface for the CRSSCurl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSSCURL_H__CB90B784_F34D_43FB_9DB7_94D53FBCEC9B__INCLUDED_)
#define AFX_RSSCURL_H__CB90B784_F34D_43FB_9DB7_94D53FBCEC9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRSSCurl  
{
public:
	CRSSCurl();
	virtual ~CRSSCurl();

	void Refresh(LPCSTR lpszUrl);

protected:
	CURL* m_pCURL;
	std::string m_strResponseData;
	char m_szErrorBuffer[CURL_ERROR_SIZE];

	struct _tagFeed
	{
		bool bIsLive;
	};
	std::vector<struct _tagFeed> m_vFeeds;

private:
	static int s_WriteFunction(void* ptr, size_t size, size_t nmemb, void* stream);
};

#endif // !defined(AFX_RSSCURL_H__CB90B784_F34D_43FB_9DB7_94D53FBCEC9B__INCLUDED_)
