#ifndef LCH_STD_HEADRE_H
#define LCH_STD_HEADRE_H
#include <cstdio>
#include <tchar.h>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>

#include <auto_handle.h>
#include <cassert>


typedef wukong::StringType<sizeof(*_T("")) == sizeof(wchar_t)>::ResultType TSTRING;
typedef TSTRING tString, TString;

#endif // LCH_STD_HEADRE_H

