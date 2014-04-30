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

	struct _tagEntry
	{
		bool bIsLive;
	};
	std::vector<struct _tagEntry> m_vEntries;

private:
	std::stringstream m_ssXSL;

	static int s_WriteFunction(void* ptr, size_t size, size_t nmemb, void* stream);
	inline bool _TransformFeed(LPCSTR lpszInput, std::stringstream& ssXSLT, std::string& strResult);
	inline void _CreateEntries(LPCSTR lpszInput);
};

#endif // !defined(AFX_RSSCURL_H__CB90B784_F34D_43FB_9DB7_94D53FBCEC9B__INCLUDED_)
