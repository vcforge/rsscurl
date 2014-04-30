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

protected:
	CURL* m_pCURL;

	struct _tagFeed
	{
		bool bIsLive;
	};
	std::vector<struct _tagFeed> m_vFeeds;

};

#endif // !defined(AFX_RSSCURL_H__CB90B784_F34D_43FB_9DB7_94D53FBCEC9B__INCLUDED_)
