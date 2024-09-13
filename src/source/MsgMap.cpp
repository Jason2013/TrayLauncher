
#include <shellapi.h>
#include <shlwapi.h>
#include <dbt.h>
#include "resource.h"
#include "language.h"
#include "MenuWithIcon.h"
#include "GDIWavePic.h"
#include "RunDlg.h"
#include "Hotkey.h"
#include "MsgMap.h"
//#include <gdiplus.h>
#include <cstdio>
#include "SettingFile.h"

std::map<TSTRING, TSTRING> & ExtraSettings();

CSettingFile & Settings()
{
	static CSettingFile s_setting(ExtraSettings()[_T("ini")].empty() ? _T("TL.ini") : ExtraSettings()[_T("ini")], true);
	return s_setting;
}

void SaveRunPos(); // save run dialog position to settings.

HWND g_hWnd;

#define ON_EXIT_SCOPE_EXEC(id, exprs) struct {\
	struct OnExitScope {\
		~OnExitScope() { do{exprs}while(0); }\
	} m_obj_;\
} auto_guard_on_exit_ ## id ## _

#define CALL2(M, P1, P2) M(P1, P2)

#define ON_EXIT_SCOPE(exprs) CALL2(ON_EXIT_SCOPE_EXEC, __LINE__, exprs)

const int UM_MIDCLICK = WM_USER + 4;
const int UM_REFRESH = WM_USER + 5;
const int SKIN_MENU_POS = 2;
const int LNG_MENU_POS = 3;
class CHook
{
	HINSTANCE m_hinstDLL;
public:
	CHook() {
		m_hinstDLL = LoadLibrary(_T("TL.dll"));

		if (m_hinstDLL) {
			typedef bool (*F)(HINSTANCE);
			F OpenHook = (F)GetProcAddress(m_hinstDLL, "OpenHook");

			if (!OpenHook || !OpenHook(m_hinstDLL) ) {
				FreeLibrary(m_hinstDLL);
				m_hinstDLL = 0;
			}
		}
	}
	~CHook() {
		if (m_hinstDLL) {
			typedef void (*F)();
			F CloseHook = (F)GetProcAddress(m_hinstDLL, "CloseHook");

			if (CloseHook) {
				CloseHook();
			}

			FreeLibrary(m_hinstDLL);
			m_hinstDLL = 0;
		}
	}
	operator bool() const {
		return !!m_hinstDLL;
	}
};

bool SwitchHook(bool bSet = false, bool bOn = true)
{
	static Ptr<CHook> g_hook;

	if (bSet) {
		if (bOn) {
			g_hook = new CHook;
		} else {
			g_hook.Reset();
		}
	}

	return g_hook.Get() && static_cast<bool>(*g_hook);
}
const HINSTANCE ThisHinstGet();
int MyProcessCommand(HWND hWnd, int id);

//extern TCHAR g_lpCmdLine[MAX_PATH];
extern const TCHAR *szWindowClass;

const UINT ID_TASKBARICON = 10;
const UINT UM_ICONNOTIFY = WM_USER + 1;


const TSTRING GetLngInfo(const TSTRING & strFileName, const TSTRING &strKey);
const TSTRING GetLngMenuStr(const TSTRING & strFileName)
{
	if (strFileName.empty())
		return strFileName;

	return GetLngInfo(strFileName, _T("Language")) + _T(" ") +
			GetLngInfo(strFileName, _T("Version")) + _T(" @ ") + strFileName;
}

TSTRING g_fileName = TEXT(".\\TLCmd.txt");

icon_ptr & GTrayIcon()
{
	static icon_ptr s_trayIcon;
	return s_trayIcon;
}


void ShowHiddenJudge(CMenuWithIcon *pMenu)
{
	TSTRING strShowHidden(_T("false"));

	if(Settings().Get(sectionGeneral, keyShowHidden, strShowHidden) && (strShowHidden == _T("true") || strShowHidden == _T("1"))) {
		pMenu->ShowHidden() = true;
	} else {
		pMenu->ShowHidden() = false;

		if(strShowHidden != _T("0"))
			Settings().Set(sectionGeneral, keyShowHidden, _T("0"), true);
	}
}



bool MClickJudge()
{
	TSTRING strValue(_T("false"));

	if (!Settings().Get(sectionGeneral, keyMClick, strValue) || (strValue != _T("true") && strValue != _T("1")) ) {
		if (strValue != _T("0")) {
			Settings().Set(sectionGeneral, keyMClick, _T("0"), true);
		}

		return false;
	}

	return true;
}




typedef LRESULT (*MsgProc)(HWND, UINT, WPARAM, LPARAM);
typedef std::map<const UINT,MsgProc> MsgMap;
typedef MsgMap :: const_iterator MsgIter;


enum EHOTKEYS {
	HOTKEYBEGIN = 1,
	HOTKEYPOPMENU = HOTKEYBEGIN,
	HOTKEYPOPSYSMENU,
	HOTKEYPOPEXECUTE,
	HOTKEYMIDCLICK,
	HOTKEYPOPSYSMENU_ALTER,
	HOTKEYEND
};


//UINT_PTR g_lastMenuSel = 0;

Ptr<COwnerDrawMenu> g_pSysTray;
Ptr<CMenuWithIcon> g_pTray;
Ptr<CHotkey> g_pHotkey;


CMsgMap & TheMsgMap()
{
	static CMsgMap msgMap;
	return msgMap;
}

bool & IgnoreUser()
{
	static bool s_bIgnoreUser = false;
	return s_bIgnoreUser;
}

//（消息）函数声明
LRESULT  MsgCreate		(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgIconNotify	(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgDestroy		(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgTaskbarCreated	(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgHotKey(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgNewInstance(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgMidClick(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgEndSession(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgDeviceChange(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgRefresh(HWND, UINT, WPARAM, LPARAM);


BOOL  CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);


enum MENU_ID_TYPE {BASE = 2000,CMDID_START = BASE,AUTOSTART = BASE,SEPRATER,RELOAD,EDITCMDS,EXIT,ABOUT, OPTION, MCLICK,RUNDLG,
                   SKINIDSTART = 2050, SKINIDEND = 3000, LNGIDSTART = SKINIDEND+1, LNGIDEND = 3500, CMDID_END = LNGIDEND, MENUID_START = CMDID_END
                  };
enum AUTORUN {AR_ADD,AR_REMOVE,AR_CHECK};
int AutoStart(AUTORUN);


int SetHotkeys()
{
	if (g_pHotkey.Get()) {
		Settings().AddSection(sectionHotkey);
		TSTRING str;

		if (Settings().Get(sectionHotkey, keyHKMenu, str)) {
			g_pHotkey->Add(HOTKEYPOPMENU,str);
		} else {
			Settings().Set(sectionHotkey, keyHKMenu, _T("Alt + LWin"), true);
		}

		if (Settings().Get(sectionHotkey, keyHKContextMenu, str)) {
			g_pHotkey->Add(HOTKEYPOPSYSMENU,str);
		} else {
			Settings().Set(sectionHotkey, keyHKContextMenu, _T("Alt + RWin"), true);
		}

		if (Settings().Get(sectionHotkey, keyHKMenuAtMouse, str)) {
			g_pHotkey->Add(HOTKEYMIDCLICK,str);
		} else {
			Settings().Set(sectionHotkey, keyHKMenuAtMouse, _T("Shift + LWin"), true);
		}

		if (Settings().Get(sectionHotkey, keyHKRunDialog, str)) {
			g_pHotkey->Add(HOTKEYPOPEXECUTE,str);
		} else {
			Settings().Set(sectionHotkey, keyHKRunDialog, _T("Ctrl + LWin"), true);
		}

		if (Settings().Get(sectionHotkey, keyHKContextMenu_alt, str)) {
			g_pHotkey->Add(HOTKEYPOPSYSMENU_ALTER,str);
		} else {
			Settings().Set(sectionHotkey, keyHKContextMenu_alt, _T("Ctrl + Alt + LWin"), true);
		}
	}

	return 0;
}


int BuildMenuFromFile(const TCHAR * strFile)
{
	file_ptr file(strFile,TEXT("rb"));
	int nItems = 0; //菜单项个数,返回值

	if(!file) {
		nItems = -1; // 打开文件错误
	} else if (!ns_file_str_ops::IsStrEndWith(strFile, _T(".xml"), false) && _fgettc(file) != 0xfeff) {
		MessageBox(NULL, _LNG(STR_cmd_file_not_UNICODE),NULL,MB_OK);
	} else {
		file.Reset();
		nItems = g_pTray->LoadMenuFromFile(strFile, MENUID_START);
		//nItems = g_pTray->BuildMenu(file,MENUID_START); //todo 也做成 非 成员函数??
	}

	return nItems;
}

void SetLanguage(const TSTRING & strFNLng)
{
	bool bDefault = true;

	if (!SetLanguageFile(strFNLng.c_str())) {
		Settings().Set(sectionGeneral, keyLanguage, _T(""),true);
	} else {
		Settings().Set(sectionGeneral, keyLanguage, strFNLng, true);
		bDefault = strFNLng.empty();
	}

	g_pSysTray->SetNameByPos(SKIN_MENU_POS,_LNG(MENU_Select_Skin));
	g_pSysTray->SetNameByPos(LNG_MENU_POS,_LNG(MENU_Language));

	g_pSysTray->SetName(EDITCMDS,_LNG(MENU_Edit_Cmd));
	{
		TSTRING str;
		Settings().Get(sectionHotkey, keyHKRunDialog, str);
		g_pSysTray->SetName(RUNDLG,TSTRING(_LNG(MENU_Run)) + (str.empty() ? _T(" ...") : _T(" ...   ") + str) );
	}

	g_pSysTray->SetName(MCLICK,_LNG(MENU_Use_MClick));
	g_pSysTray->SetName(AUTOSTART,_LNG(MENU_Start_With_OS));
	g_pSysTray->SetName(RELOAD,_LNG(MENU_Refresh));
	g_pSysTray->SetName(ABOUT,_LNG(MENU_About));
	g_pSysTray->SetName(EXIT,_LNG(MENU_Exit));
	g_pSysTray->SetName(LNGIDSTART, _LNG(MENU_Internal));
	g_pSysTray->SetName(SKINIDSTART, _LNG(MENU_Internal));
	g_pSysTray->UpdateRoot();

	HMENU hLngMenu = GetSubMenu(g_pSysTray->Menu(), LNG_MENU_POS);

	if(IsMenu(hLngMenu)) {
		unsigned int count = GetMenuItemCount(hLngMenu);

		for (unsigned int i = 0; i < count; ++i) {
			CheckMenuItem( hLngMenu, i, MF_BYPOSITION | MF_UNCHECKED);
		}

		if ( ! strFNLng.empty()) {
			TSTRING strLngName (GetLngMenuStr(strFNLng));

			for (unsigned int i = 1; i < count; ++i) {
				if (strLngName == g_pSysTray->Name(GetMenuItemID(hLngMenu, i))) {
					CheckMenuItem( hLngMenu, i, MF_BYPOSITION | MF_CHECKED);
					return;
				}
			}
		}

		CheckMenuItem(hLngMenu, 0, MF_BYPOSITION | MF_CHECKED);
	}
}

// Get the count of an array of any type
template <class TItem, unsigned int N>
inline unsigned int ArrayN (const TItem (&)[N])
{
	return N;
}

ICONTYPE MyLoadIcon(const int id)
{
	return id
	       ?(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(id),IMAGE_ICON,0,0,LR_DEFAULTCOLOR)
	       :0;
}
ICONTYPE MyLoadIcon(const TSTRING & strFileName)
{
	return (ICONTYPE)LoadImage(0, strFileName.c_str(), IMAGE_ICON,0,0,LR_LOADFROMFILE);
}

void SetMenuIcons(const TSTRING & iconDir = _T(""))
{
	std::vector<int> vIDs;
	vIDs.push_back(IDI_EDIT);
	vIDs.push_back(IDI_REFRESH);
	vIDs.push_back(IDI_SKIN);
	vIDs.push_back(IDI_LNG);
	vIDs.push_back(0);
	vIDs.push_back(0);
	vIDs.push_back(0);
	vIDs.push_back(0);
	vIDs.push_back(IDI_PENCIL);
	vIDs.push_back(IDI_EXIT);

	const TSTRING & strIconDir(iconDir);

	if (iconDir.empty()) {
		for (unsigned int i = 0; i < vIDs.size(); ++i) {
			g_pSysTray->IconByPos(i,MyLoadIcon(vIDs[i]));
		}
	} else {
		std::vector<TSTRING> vFNs; //filenames
		vFNs.push_back(strIconDir + _T("edit.ico"));
		vFNs.push_back(strIconDir + _T("refresh.ico"));
		vFNs.push_back(strIconDir + _T("skin.ico"));
		vFNs.push_back(strIconDir + _T("language.ico"));
		vFNs.push_back(strIconDir + _T("mclick.ico"));
		vFNs.push_back(strIconDir + _T("autostart.ico"));
		vFNs.push_back(_T(""));// separator
		vFNs.push_back(strIconDir + _T("run.ico"));
		vFNs.push_back(strIconDir + _T("about.ico"));
		vFNs.push_back(strIconDir + _T("exit.ico"));

		for (unsigned int i = 0; i < vFNs.size(); ++i) {
			if (ICONTYPE hIcon = MyLoadIcon(vFNs[i])) {
				g_pSysTray->IconByPos(i, hIcon);
			} else {
				g_pSysTray->IconByPos(i, MyLoadIcon(vIDs[i]));
			}
		}
	}

	vIDs.clear();
	vIDs.push_back(IDI_OPEN);
	vIDs.push_back(IDI_CLOSE);
	vIDs.push_back(IDI_UNKNOWN);

	if (iconDir.empty()) {
		g_pTray->DefaultIcons(MyLoadIcon(vIDs[0]), MyLoadIcon(vIDs[1]), MyLoadIcon(vIDs[2]));
	} else {

		std::vector<TSTRING> vFNs; //filenames
		vFNs.push_back(strIconDir + _T("open.ico"));
		vFNs.push_back(strIconDir + _T("close.ico"));
		vFNs.push_back(strIconDir + _T("unknown.ico"));
		std::vector<ICONTYPE> icons;

		for (unsigned int i = 0; i < vFNs.size(); ++i) {
			if (ICONTYPE hIcon = MyLoadIcon(vFNs[i])) {
				icons.push_back(hIcon);
			} else {
				icons.push_back(MyLoadIcon(vIDs[i]));
			}
		}

		g_pTray->DefaultIcons(icons[0], icons[1], icons[2]);
	}


}
//! 设置菜单皮肤
void SetMenuSkin(const TSTRING & skinSubDir)
{
	const int nPicPerItem = 3;

	if (skinSubDir.empty()) {
		HBITMAP hBit[nPicPerItem] = {0};
		g_pTray->SetSkin(0, hBit, hBit, hBit, 0);
		g_pSysTray->SetSkin(0, hBit, hBit, hBit, 0);
		SetMenuIcons();
	} else {
		const TCHAR * szSkinBk[] = {TEXT("bk.bmp"), TEXT("bkLeft.bmp"), TEXT("bkRight.bmp")};
		const TCHAR * szSkinSelBk[] = {TEXT("selbk.bmp"), TEXT("selbkLeft.bmp"), TEXT("selbkRight.bmp")};
		const TCHAR * szSkinSep[] = {TEXT("sep.bmp"), TEXT("sepLeft.bmp"), TEXT("sepRight.bmp")};
		TSTRING strSkinPath = _T(".\\skin\\") + skinSubDir + _T("\\");
		TSTRING strSkinPicPath;
		HBITMAP hBitBk[nPicPerItem];
		HBITMAP hBitSel[nPicPerItem];
		HBITMAP hSep[nPicPerItem];
		assert(ArrayN(hBitBk) == ArrayN(szSkinBk));
		assert(ArrayN(hSep) == ArrayN(szSkinSep));
		assert(ArrayN(hBitSel) == ArrayN(szSkinSelBk));

		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinBk[i];
			hBitBk[i] = (HBITMAP)LoadImage(0, strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}

		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinSelBk[i];
			hBitSel[i] = (HBITMAP)LoadImage(0, strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}

		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinSep[i];
			hSep[i] = (HBITMAP)LoadImage(0, strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}

		strSkinPicPath = strSkinPath + TEXT("side.bmp");
		HBITMAP hSide = ((HBITMAP)LoadImage(0,strSkinPicPath.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));
		strSkinPicPath = strSkinPath + TEXT("tital.bmp");
		HBITMAP hTital = ((HBITMAP)LoadImage(0,strSkinPicPath.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));

		g_pTray->SetSkin(hSide, hBitBk, hBitSel, hSep, hTital);
		g_pSysTray->SetSkin(hSide, hBitBk, hBitSel, hSep, hTital);
		SetMenuIcons(strSkinPath + _T("icons\\"));
	}

	HMENU hSkinMenu = GetSubMenu(g_pSysTray->Menu(), SKIN_MENU_POS);

	if(IsMenu(hSkinMenu)) {
		unsigned int count = GetMenuItemCount(hSkinMenu);

		for (unsigned int i = 0; i < count; ++i) {
			CheckMenuItem( hSkinMenu, i, MF_BYPOSITION | MF_UNCHECKED);
		}

		if ( ! skinSubDir.empty()) {
			for (unsigned int i = 1; i < count; ++i) {
				if (skinSubDir == g_pSysTray->Name(GetMenuItemID(hSkinMenu, i))) {
					CheckMenuItem( hSkinMenu, i, MF_BYPOSITION | MF_CHECKED);
					Settings().Set(sectionGeneral, keySkin, skinSubDir, true);
					return;
				}
			}
		}

		CheckMenuItem(hSkinMenu, 0, MF_BYPOSITION | MF_CHECKED);
		Settings().Set(sectionGeneral, keySkin, _T(""), true);
	}
}


// 操作托盘图标,添加,更新或删除
void Systray(const HWND hWnd, const DWORD dwMessage, ICONTYPE hIcon = NULL, const TSTRING &strInfo = _T(""))
{
	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = ID_TASKBARICON;
	nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO|NIIF_INFO;
	nid.uCallbackMessage = UM_ICONNOTIFY;
	nid.hIcon = hIcon ? hIcon : LoadIcon(ThisHinstGet(), MAKEINTRESOURCE(IDI_TRAYSTART));
	TSTRING strTip = _T("Tray Launcher");
	TSTRING sid(ExtraSettings()[_T("sid")]);
	if (!sid.empty())
	{
		strTip += _T(" [") + sid + _T("]");
	}
	memcpy(nid.szTip, strTip.c_str(), sizeof(strTip[0]) * (strTip.length() + 1));

	if (strInfo.length()) {
		memcpy(nid.szInfoTitle, strTip.c_str(), sizeof(strTip[0]) * (strTip.length() + 1));
		memcpy(nid.szInfo, strInfo.c_str(), sizeof(TCHAR) * strInfo.length());
		nid.uTimeout = 5000;
	}

	Shell_NotifyIcon(dwMessage, &nid);
}

// refresh automatically if file is changed
void UpdateMenu(const bool bForce = false)
{
	static FILETIME s_tmCreate = {0};
	static FILETIME s_tmWrite = {0};
	bool bBuild(bForce);
	FILETIME tmCreate, tmAccess, tmWrite;

	if (ns_file_str_ops::GetLastFileTime(g_fileName.c_str(), &tmCreate, &tmAccess, &tmWrite) ) {
		if (bForce || CompareFileTime(&tmCreate, &s_tmCreate) || CompareFileTime(&tmWrite , &s_tmWrite) ) {
			s_tmCreate = tmCreate;
			s_tmWrite = tmWrite;
			bBuild = true;
		}
	}

	if (bBuild) {
		Systray(g_hWnd, NIM_MODIFY, GTrayIcon().Get(), _LNG(STR_Refreshing));
		BuildMenuFromFile(g_fileName.c_str());
		Systray(g_hWnd, NIM_MODIFY, GTrayIcon().Get());
	}
}

//! Display menu and process command
void ShowMenu(const POINT * p = NULL, bool bLast = false)
{
	if (!IgnoreUser()) {
		static POINT s_point = {0,0};

		if(p) {
			s_point = *p;
		} else if (!bLast) {
			GetCursorPos(&s_point);
		}

		// refresh
		UpdateMenu();

		if (g_pTray->Display(s_point.x, s_point.y) ) {
			SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
		}
	}
}


void UpdateRunDlgCheck()
{
	if ( GHdlgRun() && IsWindowVisible( GHdlgRun() ) ) {
		CheckMenuItem(g_pSysTray->Menu(),RUNDLG, MF_BYCOMMAND | MF_CHECKED );
	} else {
		CheckMenuItem(g_pSysTray->Menu(),RUNDLG, MF_BYCOMMAND | MF_UNCHECKED );
	}
}

bool StringMeansTrue(const TSTRING &str)
{
	return 	str == _T("true") ||
			str == _T("TRUE") ||
			str == _T("True") ||
			str == _T("1");
}

void ShowSysMenu(HWND hWnd, const POINT * p = NULL)
{
	if (IgnoreUser())
	{
		return;
	}
	TSTRING strValue;
	if (Settings().Get(sectionGeneral, keyDisableSysMenu, strValue) &&
		StringMeansTrue(strValue) )
	{
		return;
	}
	POINT point = {0,0};
	if (p)
	{
		point = *p;
	}
	else
	{
		GetCursorPos(&point);
	}

	UpdateRunDlgCheck();
	MyProcessCommand(hWnd, g_pSysTray->Display(point.x, point.y));
}


//显示运行对话框
void ShowRunDlg()
{
	if (IgnoreUser()) return;

	int nCmdShow = SW_SHOWNORMAL;

	if (!GHdlgRun()) {
		UpdateMenu();
		GHdlgRun() = CreateRunDialog(ThisHinstGet());
	} else if (IsWindowVisible(GHdlgRun())) {
		nCmdShow = SW_HIDE;
	} else {
		nCmdShow = SW_SHOW;
		UpdateMenu();
	}

	ShowWindow(GHdlgRun(), nCmdShow);
}

void ShowAbout()
{
	if (!IgnoreUser())
	{
		IgnoreUser() = true;
		ON_EXIT_SCOPE(IgnoreUser() = false;);
		DialogBox(ThisHinstGet(), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, AboutProc);
	}
}

static std::map<int, TSTRING> s_id2LngFN;

int MyProcessCommand(HWND hWnd, int id)
{
	if(IgnoreUser() || id >= MENUID_START || id < CMDID_START)
		return id;

	switch (id) {
	case ABOUT:

		if (GHdlgRun() && IsWindowVisible(GHdlgRun())) {
			ShowRunDlg();
			ShowAbout();
			//IgnoreUser() = true;
			//DialogBox(ThisHinstGet(), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
			//IgnoreUser() = false;
			ShowRunDlg();
		} else {
			ShowAbout();
		}

		assert(!IgnoreUser());

		break;
	case EXIT: {
			bool bShowExitConformDlg = true;
			TSTRING str;

			if (Settings().Get(sectionGeneral, keyConformExit, str)) {
				bShowExitConformDlg = (str == _T("true") || str == _T("1"));
			} else {
				Settings().Set(sectionGeneral, keyConformExit, _T("1"), true);
			}

			IgnoreUser() = (!bShowExitConformDlg || IDYES == MessageBox(NULL, _LNG(STR_Exit_Ask), _LNG(STR_Confirm), MB_YESNO | MB_TOPMOST));//true;

			if (IgnoreUser()) {
				if (GHdlgRun()) {
					DestroyWindow(GHdlgRun());
					GHdlgRun() = NULL;
				}

				DestroyWindow(hWnd);
			}
		}
		return 0;
		//break;
	case EDITCMDS:
		Settings().Save();
		if(	!ShellSuccess(ShellExecute(NULL, _T("open"), _T(".\\TLMenuCfg.exe"), (_T("\"--ini=") + ExtraSettings()[_T("ini")] + _T("\"")).c_str(), NULL,SW_SHOW)) &&
			!ShellSuccess(ShellExecute(NULL,NULL,g_fileName.c_str(),NULL,NULL,SW_SHOW))) {
			//执行命令失败
			if(ShellSuccess(ShellExecute(NULL, _T("open"), _T("notepad.exe"), g_fileName.c_str(),NULL,SW_SHOW)))
				break;

			MessageBox(NULL,_LNG(STR_Failed_open_create_cmd_file),NULL,MB_ICONERROR);
		}

		break;
	case RELOAD:
		//if (Settings().Read())
		//	ShowHiddenJudge(g_pTray.Get());
		UpdateMenu(true);
		ShowMenu(0,true);
		break;
	case RUNDLG:
		ShowRunDlg();
		break;
	case AUTOSTART:

		if (GetMenuState(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND) & MF_CHECKED) {
			if (0 == AutoStart(AR_REMOVE))
				CheckMenuItem(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND | MF_UNCHECKED);
		} else {
			if (1 == AutoStart(AR_ADD))
				CheckMenuItem(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND | MF_CHECKED);
		}

		break;
	case OPTION:

		//todo Now it's for test only
		if(!ShellSuccess(ShellExecute(NULL,NULL, _T(".\\TL.ini"),NULL,NULL,SW_SHOW))) {
			//执行命令失败
			ShellSuccess(ShellExecute(NULL,_T("open"),_T("notepad.exe"), _T(".\\TL.ini"), NULL,SW_SHOW));
		}

		break;
	case MCLICK:

		//
		if ( SwitchHook(true, !SwitchHook() ) ) {
			CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_CHECKED);
			Settings().Set(sectionGeneral, keyMClick, _T("1"), true);
		} else {
			CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_UNCHECKED);
			Settings().Set(sectionGeneral, keyMClick, _T("0"), true);
		}

		break;
	case SKINIDSTART:
		SetMenuSkin(_T(""));
		break;
	case LNGIDSTART:
		SetLanguage(_T(""));
		break;
	default:

		if (id > SKINIDSTART && id < SKINIDEND) {
			const TCHAR * pSkinDir = g_pSysTray->Name(id);

			if (pSkinDir && *pSkinDir)
				SetMenuSkin(pSkinDir);
		} else if (id > LNGIDSTART && id < LNGIDEND) {
			assert(s_id2LngFN.find(id) != s_id2LngFN.end());
			const TCHAR * //pLngFile = g_pSysTray->Name(id);
			pLngFile = s_id2LngFN[id].c_str();

			if (pLngFile && *pLngFile)
				SetLanguage(pLngFile);
		}

		break;
	}

	SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
	return id;
}


// 初始化，添加消息映射
void InitMsgMap()
{
	TheMsgMap().Add(WM_CREATE,		&MsgCreate);
	TheMsgMap().Add(WM_DESTROY,		&MsgDestroy);
	TheMsgMap().Add(UM_ICONNOTIFY,	&MsgIconNotify);
	TheMsgMap().Add(WM_HOTKEY,		&MsgHotKey);
	TheMsgMap().Add(WM_CLOSE,		&MsgClose);
	TheMsgMap().Add(UM_NEWINSTANCE, &MsgNewInstance);
	TheMsgMap().Add(UM_MIDCLICK,	&MsgMidClick);
	TheMsgMap().Add(UM_REFRESH,	&MsgRefresh);
	TheMsgMap().Add(WM_ENDSESSION,	&MsgEndSession);
	TheMsgMap().Add(WM_DEVICECHANGE, &MsgDeviceChange);
}


//! 处理来自另一个进程的通知
LRESULT  MsgNewInstance(HWND hWnd, UINT, WPARAM, LPARAM)
{
	Sleep(100);//等待发送方退出。
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());
	ShowMenu();
	return 0;
}


//! 处理刷新通知
LRESULT  MsgRefresh(HWND, UINT, WPARAM, LPARAM)
{
	if (g_pTray.Get() && g_pTray->HasMyComputer()) {
		UpdateMenu(true);
	}

	return 0;
}
//! 处理鼠标中键点击的通知
LRESULT  MsgMidClick(HWND hWnd, UINT, WPARAM bDown, LPARAM)
{
	if(bDown || (GetKeyState(VK_LBUTTON)&0x8000) )
	{
		ShowSysMenu(hWnd);
	}
	else
	{
		Sleep(100);
		ShowMenu();
	}

	return 0;
}


//! 处理关机，注销等 通知
LRESULT  MsgEndSession(HWND, UINT, WPARAM wParam, LPARAM)   // WM_ENDSESSION
{
	if (wParam) {
		// 即将关闭会话
		SaveRunPos();
		Settings().Save();
	}

	return 0;
}

//! 处理关机，注销等 通知
LRESULT  MsgDeviceChange(HWND hWnd, UINT, WPARAM wParam, LPARAM lParam)   // WM_DEVICECHANGE
{
	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

	switch (wParam) {
	case DBT_DEVICEARRIVAL:
	case DBT_DEVICEREMOVECOMPLETE :

		// Check whether a CD or DVD or USB stick was inserted or removed.
		if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

			if (lpdbv->dbcv_flags & DBTF_MEDIA) {
				// refresh my computer
				PostMessage(hWnd, UM_REFRESH,0,0);
			}
		}

		break;

	default:
		break;

	}

	return 0;
}
//
//  函数: ProcMessage(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
// 分配消息
LRESULT CALLBACK ProcMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result(0xdeadbeef);

	if (TheMsgMap().ProcessMessage(hWnd, message, wParam, lParam, &result))
		return result;
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}


bool AddHotkey(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
	return ( 0 != RegisterHotKey(hWnd, id, fsModifiers, vk) );

	//if (!RegisterHotKey(hWnd, id, fsModifiers, vk)) {
	//	MessageBox(NULL,_T("Register Hot Key Failed!"),NULL,MB_OK);
	//	return false;
	//}
	//return true;
}


const TSTRING TryUpdateMenuFileToXml(const TSTRING & strFileName)
{
	TSTRING new_name(strFileName);
	// update to xml format.
	if (!ns_file_str_ops::IsStrEndWith(strFileName, _T(".xml"), false)) {
		if (IDYES == MessageBox(NULL, _LNG(STR_ASK_UPDATE_MENU_FILE_TO_XML), _T("Tray Launcher"), MB_YESNO)) {
			CMenuData tmp(_T("root"));
			tmp.Load(strFileName);
			const TSTRING prefix = strFileName.substr(0, strFileName.find_last_of('.'));
			new_name = prefix + _T(".xml");
			// get new xml file name.
			int n = 1;
			while (file_ptr(new_name.c_str(), _T("rb"))) {
				TCHAR num[64] = {0};
				_stprintf(num, _T("%d"), n++);
				new_name = prefix + _T("_") + num + _T(".xml");
			}
			tmp.SaveAs(new_name);
		}
	}
	return new_name;
}


// 以下是不同消息相应的处理函数 : Msg....(HWND, UINT, WPARAM, LPARAM) ;
// WM_CREATE
LRESULT  MsgCreate(HWND hWnd, UINT /*message*/, WPARAM /* wParam */, LPARAM /* lParam */)
{
	g_hWnd = hWnd;

	//for (int i = CMDID_START; i < CMDID_END; ++i) {
	//	if (IsMenu( (HMENU)i ) ) {
	//		//TrackPopupMenuEx((HMENU)i, 0, 0, 0, hWnd, NULL);
	//		DestroyMenu( (HMENU)i );

	//	}
	//}
	UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated")); // 获取托盘重建消息，恢复托盘图标。

	if (WM_TASKBARCREATED != 0)
		TheMsgMap().Add(WM_TASKBARCREATED, &MsgTaskbarCreated);

	ICONTYPE hIconCheck = (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_RIGHT),IMAGE_ICON,0,0,LR_DEFAULTCOLOR);

	g_pSysTray = new COwnerDrawMenu(hIconCheck);
	g_pSysTray->UseActualIconSize(true);
	g_pSysTray->Insert(EDITCMDS,_LNG(MENU_Edit_Cmd));
	g_pSysTray->Insert(RELOAD,_LNG(MENU_Refresh));
	g_pSysTray->Insert(MCLICK,_LNG(MENU_Use_MClick));
	g_pSysTray->Insert(AUTOSTART,_LNG(MENU_Start_With_OS));
	g_pSysTray->InsertSep();
	g_pSysTray->Insert(RUNDLG,(TSTRING(_LNG(MENU_Run)) + _T("\t  Ctrl+Win")).c_str());
	g_pSysTray->Insert(ABOUT,_LNG(MENU_About));
	g_pSysTray->Insert(EXIT,_LNG(MENU_Exit));
	//*

	//构造skin选项
	HMENU hSkinMenu = CreatePopupMenu();

	if (IsMenu(hSkinMenu) ) {
		InsertMenu(hSkinMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, SKINIDSTART, _LNG(MENU_Internal));

		WIN32_FIND_DATA fd = {0};
		HANDLE handle = INVALID_HANDLE_VALUE;
		const TCHAR * const pSearch = _T(".\\skin\\*");
		handle = FindFirstFile(pSearch,&fd);

		if (handle != INVALID_HANDLE_VALUE) {
			std::map<TSTRING,TSTRING> nameName;

			do {
				//只处理文件夹， //跳过 "." 和 ".." 目录
				if((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
				        fd.cFileName[0] == '.' )
					continue;

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(fd.cFileName);
				TCHAR ch('&');
				TSTRING & str = strFileName;
				TSTRING::size_type pos = 0;

				while((pos = str.find(ch,pos)) != str.npos) {
					str.insert(pos,1,ch);
					pos+=2;
				}

				// 忽略大小写
				TSTRING strNameLower(strFileName);
				TSTRING::size_type size = strNameLower.length();

				for (TSTRING::size_type i = 0; i < size; ++i) {
					strNameLower[i] = _totlower(strNameLower[i]);
				}

				nameName[strNameLower] = strFileName;

			} while (FindNextFile(handle,&fd));

			FindClose(handle);


			int id = SKINIDSTART;
			std::map<TSTRING,TSTRING>::iterator itStr,itName;

			for (itName = nameName.begin(); itName != nameName.end() && ++id < SKINIDEND; ++itName) {
				InsertMenu(hSkinMenu,static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, id, itName->second.c_str());
			}
		}

		g_pSysTray->Insert(hSkinMenu, _LNG(MENU_Select_Skin), SKIN_MENU_POS,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_SKIN),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
	}


	//构造language
	HMENU hLngMenu = CreatePopupMenu();

	if (IsMenu(hLngMenu) ) {
		InsertMenu(hLngMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, LNGIDSTART, _LNG(MENU_Internal));

		WIN32_FIND_DATA fd = {0};
		HANDLE handle = INVALID_HANDLE_VALUE;
		const TCHAR * const pSearch = _T(".\\Lng\\*.lng");
		handle = FindFirstFile(pSearch,&fd);

		if (handle != INVALID_HANDLE_VALUE) {
			std::map<TSTRING,TSTRING> nameName;

			do {
				//跳过目录
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(fd.cFileName);
				TCHAR ch('&');
				TSTRING & str = strFileName;
				TSTRING::size_type pos = 0;

				while((pos = str.find(ch,pos)) != str.npos) {
					str.insert(pos,1,ch);
					pos+=2;
				}

				// 忽略大小写
				TSTRING strNameLower(strFileName);
				TSTRING::size_type size = strNameLower.length();

				for (TSTRING::size_type i = 0; i < size; ++i) {
					strNameLower[i] = _totlower(strNameLower[i]);
				}

				nameName[strNameLower] = strFileName;

			} while (FindNextFile(handle,&fd));

			FindClose(handle);

			int id = LNGIDSTART;
			std::map<TSTRING,TSTRING>::iterator itStr,itName;

			for (itName = nameName.begin(); itName != nameName.end() && ++id < LNGIDEND; ++itName) {
				s_id2LngFN[id] = itName->second;

				TSTRING strLngName(GetLngMenuStr(itName->second));
				const TCHAR *sz = strLngName.c_str();
				InsertMenu(hLngMenu,static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, id, sz);//itName->second.c_str());
			}

			//g_pSysTray->Insert(hLngMenu, _LNG(MENU_Language), 4,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_LNG),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
		}

		g_pSysTray->Insert(hLngMenu, _LNG(MENU_Language), LNG_MENU_POS,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_LNG),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));

	}

//#ifdef _DEBUG
//		InsertMenu(hOptionMenu, 0, MF_BYPOSITION | MF_STRING, OPTION, _LNG(STR_Settings));
//#endif

	//g_pSysTray->Insert(hOptionMenu, _LNG(MENU_Option), 3,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPTION),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));

//#ifdef _DEBUG
//		g_pSysTray->AddStaticIcon(OPTION,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPTION),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
//#endif

	//EnableMenuItem(hOptionMenu,0,MF_BYPOSITION | MF_ENABLED);
	// */

	g_pTray = new CMenuWithIcon(
	    (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_CLOSE),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),
	    (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPEN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),
	    (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_UNKNOWN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),
	    _LNG(STR_Empty));

//	SetMenuIcons();
	// set skin
	TSTRING strValue;

	if(!Settings().Get(sectionGeneral, keySkin, strValue)) {
		strValue.clear();
	}

	SetMenuSkin(strValue);

	// set Lng
	if(!Settings().Get(sectionGeneral, keyLanguage, strValue)) {
		strValue.clear();
	}

	SetLanguage(strValue);

	ShowHiddenJudge(g_pTray.Get());

	TSTRING strFileName(g_fileName);

	if(Settings().Get(sectionGeneral, keyCommand, strFileName)) {
		g_fileName = strFileName;
	} else {
		const BOOL no_over_write = TRUE;
		CopyFile(_T(".\\tlcmd.example"), g_fileName.c_str(), no_over_write);
	}

	g_fileName = TryUpdateMenuFileToXml(g_fileName);
	Settings().Set(sectionGeneral, keyCommand, g_fileName, true);
	//BuildMenuFromFile(g_fileName.c_str());
	UpdateMenu();

	if (AutoStart(AR_CHECK) == 1)
		CheckMenuItem(g_pSysTray->Menu(), AUTOSTART,MF_BYCOMMAND | MF_CHECKED);
	else
		CheckMenuItem(g_pSysTray->Menu(), AUTOSTART,MF_BYCOMMAND | MF_UNCHECKED);

//*
	g_pHotkey.Reset(new CHotkey(hWnd));
	SetHotkeys();

	/*/
	AddHotkey(hWnd,HOTKEYPOPMENU,MOD_ALT | MOD_WIN,VK_LWIN);
	AddHotkey(hWnd,HOTKEYPOPSYSMENU,MOD_ALT | MOD_WIN,VK_RWIN);
	AddHotkey(hWnd,HOTKEYPOPEXECUTE,MOD_WIN | MOD_CONTROL, VK_LWIN);
	AddHotkey(hWnd,HOTKEYMIDCLICK,MOD_SHIFT | MOD_WIN, VK_LWIN);
	AddHotkey(hWnd,HOTKEYPOPSYSMENU_ALTER,MOD_ALT | MOD_CONTROL | MOD_WIN, VK_LWIN);
	//*/

	//尝试读取用户自定义图标
	TSTRING strIcon;

	if (!Settings().Get(sectionGeneral, keyRunIcon, strIcon)) {
		strIcon = TEXT(".\\TLRun.ico");
		Settings().Set(sectionGeneral, keyRunIcon, strIcon,true);
	}

	GRunIcon() = (ICONTYPE)LoadImage(0,strIcon.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);

	if (!Settings().Get(sectionGeneral, keyTrayIcon, strIcon)) {
		strIcon = TEXT(".\\TLTray.ico");
		Settings().Set(sectionGeneral, keyTrayIcon, strIcon,true);
	}

	GTrayIcon() = (ICONTYPE)LoadImage(0,strIcon.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());

	// 读取历史记录
	std::vector<TSTRING> vHisKey, vHisValue;
	Settings().GetSection(sectionHistory, vHisKey, vHisValue);
	InitHistory(vHisValue);


	if ( SwitchHook(true, MClickJudge()) ) {
		CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_CHECKED);
	} else {
		CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_UNCHECKED);
	}

	// init run dialog pos

	TSTRING strPos;
	int run_pos_x = 300;
	int run_pos_y = 400;
	if (Settings().Get(sectionGeneral, keyRunPosX, strPos)) {
		run_pos_x = _ttoi(strPos.c_str());
	}
	if (Settings().Get(sectionGeneral, keyRunPosY, strPos)) {
		run_pos_y = _ttoi(strPos.c_str());
	}
	SetRunPos(run_pos_x, run_pos_y);

	SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));

	return 0;
}


//! 关闭消息
LRESULT  MsgClose(HWND, UINT, WPARAM, LPARAM)
{
	// 屏蔽关闭消息
	return 0;
}

//! 全局快捷键
LRESULT MsgHotKey(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM /*lParam*/)
{
	if(IgnoreUser() || wParam < HOTKEYBEGIN || wParam >= HOTKEYEND) {
		;
	} else {
		POINT point = {0,0};

		switch (wParam) {
		case HOTKEYMIDCLICK:
			ShowMenu();
			break;
		case HOTKEYPOPMENU://左键菜单
			ShowMenu(&point);
			break;
		case HOTKEYPOPSYSMENU://右键菜单
		case HOTKEYPOPSYSMENU_ALTER:
			ShowSysMenu(hWnd, &point);
			break;
		case HOTKEYPOPEXECUTE://显示运行对话框
			ShowRunDlg();
			break;
		default:
			break;
		}
	}

	return 0;
}


//! explorer 重启后恢复托盘图标。
LRESULT  MsgTaskbarCreated(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM /* lParam */)
{
	Sleep(2000);
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());
	return 0;
}

void SaveRunPos()
{
	int x = 0;
	int y = 0;
	GetRunPos(x, y);
	TCHAR sz[64] = {0};
	snwprintf(sz, sizeof(sz), _T("%d"), x);
	Settings().Set(sectionGeneral, keyRunPosX, sz);
	memset(sz, 0, sizeof(sz));
	snwprintf(sz, sizeof(sz), _T("%d"), y);
	Settings().Set(sectionGeneral, keyRunPosY, sz);
}


//! 退出程序
LRESULT  MsgDestroy(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	IgnoreUser() = true;
	Systray(hWnd,NIM_DELETE,GTrayIcon().Get());
	//GTrayIcon().Free();
	//GRunIcon().Free();

	for (int i = HOTKEYBEGIN; i < HOTKEYEND; ++i)
		UnregisterHotKey(hWnd,i);

	g_pSysTray.Reset();
	g_pTray.Reset();
	SaveRunPos();
	Settings().Save();
	PostQuitMessage(0);
	return 0;
}


//! 托盘图标操作。
LRESULT  MsgIconNotify(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM lParam)
{
	if (!IgnoreUser()) {
		static bool bLBtnDown = false;
		static bool bRBtnDown = false;
		static POINT ptLast;

		switch(lParam) {
		case WM_LBUTTONDOWN:
			bLBtnDown = true;
			bRBtnDown = false;
			GetCursorPos(&ptLast);
			break;
		case WM_RBUTTONDOWN:
			bRBtnDown = true;
			bLBtnDown = false;
			GetCursorPos(&ptLast);
			break;
		case WM_MOUSEMOVE:

			if (bLBtnDown || bRBtnDown) {
				POINT point;
				GetCursorPos(&point);

				if (point.x != ptLast.x || point.y != ptLast.y)
					bLBtnDown = bRBtnDown = false;
			}

			break;
		case WM_LBUTTONUP:

			if (bLBtnDown) {
				bLBtnDown = false;
				ShowMenu();
			}

			break;
		case WM_RBUTTONUP:

			if (bRBtnDown) {
				bRBtnDown = false;

				ShowSysMenu(hWnd);
			}

			break;
		default:
			break;
		}
	}

	return 0;
}


//! 将指定窗口移动到屏幕中央
void CentralWindow(const HWND hWnd,const HWND hParentWnd = NULL)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int ww = rect.right - rect.left;
	int wh = rect.bottom - rect.top;
	GetWindowRect(hParentWnd ? hParentWnd : GetDesktopWindow(), &rect);
	int dw = rect.right - rect.left;
	int dh = rect.bottom - rect.top;

	MoveWindow(hWnd,rect.left + ((dw - ww)>>1), rect.top + ((dh - wh)>>1), ww, wh, TRUE);
	// 总在最上。
//	SetWindowPos(hWnd, HWND_TOPMOST, rect.left + ((dw - ww)>>1), rect.top + ((dh - wh)>>1), ww, wh,SWP_SHOWWINDOW);
}

int QuoteString(TSTRING &str, const TSTRING::value_type ch = '\"', bool bProcessEmpty = false);
//! 切换开机启动
int AutoStart(AUTORUN action)
{
	int result = 0;
	HKEY hKeyRun;
	TCHAR strSubKey[] = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,strSubKey,0,KEY_ALL_ACCESS,&hKeyRun)) {
		result = -1;
	} else {
		TCHAR name[] = TEXT("Tray Launcher");
		TCHAR path[MAX_PATH] = {0};
		DWORD len = MAX_PATH;

		switch (action) {
		case AR_ADD:
			len = GetModuleFileName(NULL,path,len);

			if (len > 0) {
				TSTRING str (path);
				QuoteString(str);
				result = ( ERROR_SUCCESS == RegSetValueEx(hKeyRun,name,0,REG_SZ,(const BYTE *)str.c_str(),sizeof(TCHAR)*(int)(str.size()+1) ) );
			}

			break;
		case AR_REMOVE:
			RegDeleteValue(hKeyRun,name);
			break;
		case AR_CHECK:

			if (ERROR_SUCCESS == RegQueryValueEx(hKeyRun,name,NULL,NULL, reinterpret_cast<LPBYTE>(path), &len)) {

				TCHAR path2[MAX_PATH] = {0};
				GetModuleFileName(NULL,path2,len);
				TSTRING str (path2);
				QuoteString(str);

				if (str == path) {
					result = 1;
				} else
					result = 2;
			}

			break;
		default:
			break;
		}

		RegCloseKey(hKeyRun);
	}

	return result;
}



////////////////////////////////////////////////////////////////////////////////
//////////
//////////						"关于" 对话框部分
//////////
////////////////////////////////////////////////////////////////////////////////

int GetRand(const int to_range)
{
	return static_cast<int>(rand() * to_range / static_cast<double>(RAND_MAX));
}

// “关于”框的消息处理程序。
BOOL  CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const UINT uTimerID = 1;
	static bool bCloseWindow = true;
	static bool bUserLogo = false;
	static bool bRain = true;
	static Ptr<CGDIWavePic> pWavepic;

	const int wLimit = 82;
	const int hLimit = 110;
	const int iTimerInterval = 19;
	static int iTimerEnter = 0;
	static int iTimerDrawEvery = 0;
	static short DropDepth = 16;
	static RECT rcPic = {0,0,1,1};
	static int rgbDst[3] = {160,160,240};

	HDC hdc = NULL;
	RECT rectClient = {0,0,1,1};
	bool bResult = false;

	switch (message) {
	case WM_TIMER:

		if (// bUserLogo &&
		    bRain && (++iTimerEnter > iTimerDrawEvery)) {
			iTimerEnter = 0;
			pWavepic->Drop( rcPic.left + GetRand(rcPic.right - rcPic.left + 1),
			                rcPic.top + GetRand(rcPic.bottom - rcPic.top + 1),
			                GetRand(DropDepth << 3), GetRand(3) + 1);
		}

		hdc = GetDC(hDlg);

		GetClientRect(hDlg,&rectClient);

		pWavepic->DrawWave(hdc);

		ReleaseDC(hDlg,hdc);
		bResult = TRUE;
		break;

	case WM_INITDIALOG:
		SetWindowLong(hDlg,GWL_STYLE,WS_POPUP);

		CentralWindow(hDlg,NULL);
		GetClientRect(hDlg,&rectClient);
		SetWindowPos(hDlg,HWND_TOPMOST,0,0,	((rectClient.right-rectClient.left) >> 1 ) << 1,
		             ((rectClient.bottom-rectClient.top) >> 1 ) << 1, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_FRAMECHANGED);

		bCloseWindow = false;
		SetTimer(hDlg, uTimerID, iTimerInterval, NULL);
		{
			DWORD dwTime = GetTickCount() & 0xffffff;
			srand(dwTime*31);
		}

		//if (pWavepic) {
		//	bResult = TRUE;
		//	break;
		//}
		//else
		if (NULL != (hdc = GetWindowDC(hDlg))) {
			SetBkMode(hdc, TRANSPARENT);
			// 允许 TLLogo.bmp 自定义关于对话框 logo
			gdi_ptr<HBITMAP> hBitmap ((HBITMAP)LoadImage(ThisHinstGet(),TEXT("TLLogo.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));

			if (hBitmap) {
				BITMAP bmp = {0};
				GetObject(hBitmap,sizeof(bmp),&bmp);
				DropDepth = 16;
				bUserLogo = true;

				if(bmp.bmHeight > hLimit - 2 || bmp.bmWidth > wLimit - 2) {
					hBitmap = NULL;
				}
			}

			if (!hBitmap) {

				bUserLogo = false;
				MemDC_H hMemDC(CreateCompatibleDC(hdc));
				hBitmap = CreateCompatibleBitmap(hdc,32,32);
				HBITMAP hold = (HBITMAP)SelectObject(hMemDC,hBitmap);
				ICONTYPE icon = LoadIcon(ThisHinstGet(), MAKEINTRESOURCE(IDI_TRAYSTART));
				DrawIcon(hMemDC,0,0,icon);

				SelectObject(hMemDC,hold);
				DropDepth = 8;
			}

			if (hBitmap) {
				BITMAP bmp = {0};
				GetObject(hBitmap,sizeof(bmp),&bmp);
				RECT rect = {0,0,bmp.bmWidth,bmp.bmHeight};
				POINT point = {(wLimit - bmp.bmWidth) >> 1, (hLimit - bmp.bmHeight) >> 1};
				iTimerDrawEvery = (32*32*32*16) / (iTimerInterval * bmp.bmHeight * bmp.bmWidth + 1);
				SetRect(&rcPic, point.x,point.y, point.x + rect.right - rect.left, point.y + rect.bottom - rect.top);
				pWavepic = new CGDIWavePic(hdc,hBitmap,&rect);
				pWavepic->SetDrawPos(point.x, point.y);
			}

			ReleaseDC(hDlg,hdc);
		}

		bResult = TRUE;
		break;

	case WM_LBUTTONDOWN:
		pWavepic->Drop(LOWORD(lParam),HIWORD(lParam),DropDepth << 3, 2);
		bResult = TRUE;
		break;

	case WM_LBUTTONDBLCLK:
		//switch raining
		bRain ^= ( LOWORD(lParam) >= rcPic.left && LOWORD(lParam) < rcPic.right &&
		           HIWORD(lParam) >= rcPic.top && HIWORD(lParam) < rcPic.bottom );
		bResult = TRUE;
		break;

	case WM_RBUTTONDOWN:
		bCloseWindow = true;
		bResult = TRUE;
		break;

	case WM_MOUSEMOVE:
		bCloseWindow = false;
		//if (wParam & MK_LBUTTON)
		//	pWavepic->Drop(LOWORD(lParam),HIWORD(lParam),DropDepth << 2, 2);
		pWavepic->Drop(LOWORD(lParam),HIWORD(lParam),DropDepth << (2*static_cast<bool>(wParam & MK_LBUTTON) + 1), 2);
		bResult = TRUE;
		break;

	case WM_RBUTTONUP:

		if(!bCloseWindow) {
			bResult = TRUE;
			break;
		}

		//else go on , no break here
	case WM_COMMAND:
		EndDialog(hDlg, LOWORD(wParam));
		if (LOWORD(wParam) == IDC_WEB && STN_CLICKED == HIWORD(wParam))
		{
			const int maxchar = 256;
			TCHAR sz[maxchar] = {0};
			GetDlgItemText(hDlg, IDC_WEB, sz, maxchar);
			TSTRING str(sz);
			ns_file_str_ops::ExecuteEx(str.substr(str.find(_T("http://"))));
		}
		bResult = TRUE;
		break;
	case WM_ERASEBKGND: {
			GetClientRect(hDlg,&rectClient);

			// do NOT use min MACRO in Visual C++, use std::min
			#ifdef min
			#undef min
			#endif

			int iSmallEdge = std::min(rectClient.right, rectClient.bottom);
			iSmallEdge -= (iSmallEdge >> 1);

			if (iSmallEdge > 1) {
				const int rsrc(255),gsrc(255),bsrc(255);
				COLORREF clr = 0;
				RECT rc = {0};

				for (int i = 0; i < iSmallEdge; ++i) {
					SetRect(&rc,i,i,rectClient.right - i, rectClient.bottom - i);
					clr = RGB(
					          (rsrc * (iSmallEdge - 1 - i) + rgbDst[0] * i)/(iSmallEdge - 1),
					          (gsrc * (iSmallEdge - 1 - i) + rgbDst[1] * i)/(iSmallEdge - 1),
					          (bsrc * (iSmallEdge - 1 - i) + rgbDst[2] * i)/(iSmallEdge - 1)
					      );

					FillRect(reinterpret_cast<HDC>(wParam),&rc, gdi_ptr<HBRUSH>(CreateSolidBrush(clr)));
				}
			} else {
				FillRect(reinterpret_cast<HDC>(wParam),&rectClient, gdi_ptr<HBRUSH>(CreateSolidBrush(RGB(0xD0,0xE0,0xF0))));
			}

			bResult = TRUE;//已经消除
			break;
		}
	case WM_CTLCOLORSTATIC:

		SetBkMode(reinterpret_cast<HDC>(wParam),TRANSPARENT);
		return reinterpret_cast<BOOL>(GetStockObject(NULL_BRUSH));

	case WM_DESTROY:
		KillTimer(hDlg,uTimerID);

		pWavepic.Reset();

		//std::rotate(rgbDst,rgbDst+2,rgbDst+3);
		bResult = TRUE;
		break;
	default:
		bResult = FALSE;
		break;
	}

	return bResult;
}
