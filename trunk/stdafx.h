// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

//#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#define _WIN32_WINNT 0x0501

#include <atlbase.h>
#if _ATL_VER >= 0x0700
	#include <atlcoll.h>
	#include <atlstr.h>
	#include <atltypes.h>
	#define _WTL_NO_CSTRING
	#define END_MSG_MAP_EX END_MSG_MAP
	#define _WTL_NO_WTYPES
#else
	#define _WTL_USE_CSTRING
#endif

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlsplit.h>

#include <Psapi.h>

#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>
#include <map>

#include "fastdelegate.h"
#include "PathUtility.h"

