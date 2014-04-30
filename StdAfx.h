// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9EA8C0D1_EA1F_444D_ABD5_2DA1A350F28A__INCLUDED_)
#define AFX_STDAFX_H__9EA8C0D1_EA1F_444D_ABD5_2DA1A350F28A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// TODO: reference additional headers your program requires here
#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9EA8C0D1_EA1F_444D_ABD5_2DA1A350F28A__INCLUDED_)
