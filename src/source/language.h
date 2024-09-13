#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //Ŀǰǿ��Unicodeʵ��
#define UNICODE
#endif

#include <stdheaders.h>


//ȫ��������ĺ�������Ψһ�����������
const TCHAR * const g_strEmpty = _T("");
const TCHAR * GetLang(const wchar_t * strSrc);
const TCHAR * GetLang(const int lngid);
bool SetLanguageFile(const TCHAR * );

#define DECLARE_DEF_LANG_ID(id, str) extern const int LANG_ID_ ## id;
#include "appstrings.h"
#undef DECLARE_DEF_LANG_ID

#undef _LNG
//#define _LNG(id) GetLang(L ## #id)
#define _LNG(id) GetLang(LANG_ID_ ## id)
//inline const TCHAR * _LNG(const int id) { return GetLngStr(id); }

#endif //TRAYSTARTLANGUAGEHEADER
