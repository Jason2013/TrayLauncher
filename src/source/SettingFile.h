#ifndef LCH_SETTING_FILE_H
#define LCH_SETTING_FILE_H

#include <stdheaders.h>
#include "FileStrFnc.h"


class CSettingFile
{
	typedef std::map<TSTRING,TSTRING> StrStrMap;	//一条记录,文件中的一行。
	typedef std::pair<TSTRING,StrStrMap> Section;	//一组多条记录，用 [ ] 开始。
	typedef std::vector<Section> Setting;
	Setting m_Settings; //所有记录。
	bool m_bSettingChanged;
	TSTRING m_strFileName;
	//static const bool GetLine(FILE * file, TSTRING &strLine);
	//static void WriteStringToFile(const TSTRING & str, FILE * file);
	//static const TSTRING StripSpaces(const TSTRING & str);
	const bool Add(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str);

public:
	const bool Read(); //读取文件到 m_Settings。
	CSettingFile(const TSTRING & strFileName, bool bCreateIfNotFound = false);
	CSettingFile();
	~CSettingFile(void);

	int GetSection(const TSTRING & strSection, std::vector<TSTRING> & vStrKey, std::vector<TSTRING> & vStrValue);
	int TryGet(const TSTRING & strIndex, std::vector<TSTRING> & vStr);
	bool Get(const TSTRING & strSection, const TSTRING & strIndex, TSTRING & strDst);
	bool Set(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & strSrc, bool bCreate = true);
	bool AddSection(const TSTRING & strSection);	//添加 Section
	bool ClearSection(const TSTRING & strSection);
	bool Save();
	bool SaveAs(const TSTRING & strFileName);
	bool DisCard();
	void Clear();
};

// SettingFile　关键字
typedef const TCHAR *const CSTR;
CSTR sectionGeneral = _T("general");
CSTR keyConformExit = _T("confirm_exit");
CSTR keyCommand = _T("command");
CSTR keyRunIcon = _T("runicon");
CSTR keyTrayIcon = _T("trayicon");
CSTR keyLanguage = _T("language");
CSTR keyShowHidden = _T("showhidden");
CSTR keyMClick = _T("midclick");
CSTR keySkin = _T("skin");
CSTR keyShowOpenHere = _T("ShowOpenHere");
CSTR keyPriority = _T("priority");
CSTR keyDisableSysMenu = _T("nosysmenu");
CSTR keyRunPosX = _T("runposx");
CSTR keyRunPosY = _T("runposy");

CSTR sectionHistory = _T("history");
CSTR sectionColor = _T("color");

CSTR sectionHotkey = _T("hotkey");
CSTR keyHKMenu = _T("ShowMenu");
CSTR keyHKMenuAtMouse = _T("ShowMenuAtMouse");
CSTR keyHKContextMenu = _T("ShowContextMenu");
CSTR keyHKContextMenu_alt = _T("ShowContextMenu_alt");
CSTR keyHKRunDialog = _T("RunDialog");

#endif
