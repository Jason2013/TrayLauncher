
#include "OwnerDrawMenu.h"

//typedef WINGDIAPI BOOL (WINAPI *DCDeteler)(HDC);
const TSTRING COwnerDrawMenu::szHiddenMenuItem = _T("< . >");// normal items should not contain "<"



COwnerDrawMenu::CWindowClass::CWindowClass(HINSTANCE hInstance, const TCHAR * szWindowClass, WindowProcessor WndProc)
:m_strWndClassName(szWindowClass)
{
	//注册窗口类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= m_strWndClassName.c_str();
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 决定菜单项的大小
LRESULT  COwnerDrawMenu::MsgMeasureItem(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM lParam)
{
	MEASUREITEMSTRUCT *pMI = (MEASUREITEMSTRUCT*)lParam;
	if (wParam != 0 || pMI->CtlType != ODT_MENU)
		return 0;
	for (MenuObjects::iterator iter = s_Menus.begin(); iter != s_Menus.end(); ++iter) {
		if (iter->first->m_hWnd == hWnd) {
			iter->first->MeasureItem(pMI);
		}
	}
	return 0;
}


LRESULT  COwnerDrawMenu::MsgMenuSelect(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM lParam)
{
	UINT uItem = LOWORD(wParam);
	UINT uFlags = HIWORD(wParam);

	if (0xffff == uFlags && NULL == (MENUTYPE)lParam) //system has closed the menu
		return 0;
	for (MenuObjects::iterator iter = s_Menus.begin(); iter != s_Menus.end(); ++iter) {
		if (iter->first->m_hWnd == hWnd) {
			iter->first->MenuSelect((MENUTYPE)lParam,uItem,uFlags);
			break;
		}
	}
	return 0;
}

//!
LRESULT  COwnerDrawMenu::MsgDrawItem(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM lParam)
{
	DRAWITEMSTRUCT * pDI = (DRAWITEMSTRUCT *)lParam;
	if (wParam != 0 || pDI->CtlType != ODT_MENU)
		return 0;
	for (MenuObjects::iterator iter = s_Menus.begin(); iter != s_Menus.end(); ++iter) {
		if (iter->first->m_hWnd == hWnd) {
			return iter->first->DrawItem(pDI);
		}
	}
	return 0;
}


//! 用键盘选择菜单
LRESULT  COwnerDrawMenu::MsgMenuChar(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != MF_POPUP)
		return MAKELRESULT(0,MNC_IGNORE);

	TCHAR ch = LOWORD(wParam);
	const MENUTYPE & hMenu = (MENUTYPE)lParam;
	for (MenuObjects::iterator iter = s_Menus.begin(); iter != s_Menus.end(); ++iter) {
		if (iter->first->m_hWnd == hWnd) {
			return iter->first->MenuChar(hMenu,ch);
		}
	}
	return MAKELRESULT(0,MNC_IGNORE);
}



LRESULT CALLBACK COwnerDrawMenu::MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result(0xDEADBEEF);
	if (msgMap.ProcessMessage(hWnd, message, wParam, lParam, &result)) {
		;//return result;
	}
	else if (WM_CREATE == message) {
		msgMap.Add(WM_DRAWITEM, &MsgDrawItem);
		msgMap.Add(WM_MEASUREITEM, &MsgMeasureItem);
		msgMap.Add(WM_MENUCHAR, &MsgMenuChar);
		msgMap.Add(WM_MENUSELECT, &MsgMenuSelect);
		result = 0;
	}
	else {

//		static std::map<UINT, int> msgRecorder;
//		++msgRecorder[message];

		result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}


CMsgMap COwnerDrawMenu::msgMap;
COwnerDrawMenu::MenuObjects COwnerDrawMenu::s_Menus;
const TCHAR *COwnerDrawMenu::szMenuWindowClass(_T("Lch_Hidden_MenuWindowProcess_Class"));
const HINSTANCE ThisHinstGet();
COwnerDrawMenu::CWindowClass COwnerDrawMenu::s_windowClass(ThisHinstGet(), szMenuWindowClass,MyWndProc);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


COwnerDrawMenu::COwnerDrawMenu(ICONTYPE hIconCheck, const unsigned int iLeftSide )
:m_hWnd(CreateWindow(s_windowClass, NULL, WS_OVERLAPPED, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, ThisHinstGet(), NULL)),
m_bUseAble(!!m_hWnd),
m_hMenu(NULL),
m_hIconCheck(hIconCheck),
m_vClrs(ClrIndex_Num),
m_hSkinDC(CreateCompatibleDC(NULL)),
m_hSepPic(0),
m_sepPicHeight(0),
m_sepPicWidth(0),
m_hSepPicLeft(0),
m_sepLeftWidth(0),
m_hSepPicRight(0),
m_sepRightWidth(0),
m_hBkPic(0),
m_BkHeight(0),
m_BkWidth(0),
m_hBkPicLeft(0),
m_BkLeftWidth(0),
m_hBkPicRight(0),
m_BkRightWidth(0),
m_hSelBkPic(0),
m_selBkHeight(0),
m_selBkWidth(0),
m_hSelBkPicLeft(0),
m_selBkLeftWidth(0),
m_hSelBkPicRight(0),
m_selBkRightWidth(0),
m_hTitalPic(0),
m_titalPicWidth(0),
bUseActualIconSize(false),
m_iExtraLeftSideWidth(iLeftSide)
{
	AddThis();

	m_vClrs[ClrIndex_Bk] = RGB(255,255,255);
	m_vClrs[ClrIndex_Fr] = RGB(0,0,0);
	m_vClrs[ClrIndex_BkGrayed] = RGB(200,200,200);
	m_vClrs[ClrIndex_FrGrayed] = RGB(150,150,150);
	m_vClrs[ClrIndex_BkDisabled] = RGB(128,128,128);
	m_vClrs[ClrIndex_FrDisabled] = RGB(255,255,255);
	m_vClrs[ClrIndex_SelBorder] = RGB(0,0,255);
	m_vClrs[ClrIndex_SelBk] = RGB(235,235,255);
	m_vClrs[ClrIndex_SelFr] = RGB(0, 0, 0);
	m_vClrs[ClrIndex_Side] = RGB(55,109,184);
	m_vClrs[ClrIndex_Separator] = RGB(166,166,166);

	m_hMenu = CreatePopupMenu();
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND;
	mi.hbrBack = (HBRUSH)NULL_BRUSH;
	SetMenuInfo(m_hMenu,&mi);
}


COwnerDrawMenu::~COwnerDrawMenu(void)
{
	RemoveThis();
	Destroy();
}


void COwnerDrawMenu::SetSkin(BITMAPTYPE hSide, const BITMAPTYPE (&hBk)[3], const BITMAPTYPE (&hSelBk)[3], const BITMAPTYPE (&hSep)[3], BITMAPTYPE hTitalPic) {
	m_hSidePic = hSide;
	m_hBkPic = hBk[0];
	m_hBkPicLeft = hBk[1];
	m_hBkPicRight = hBk[2];
	m_hSelBkPic = hSelBk[0];
	m_hSelBkPicLeft = hSelBk[1];
	m_hSelBkPicRight = hSelBk[2];
	m_hSepPic = hSep[0];
	m_hSepPicLeft = hSep[1];
	m_hSepPicRight = hSep[2];
	m_hTitalPic = hTitalPic;

	BITMAP bmp = {0};

	m_BkLeftWidth = 0;
	m_BkRightWidth = 0;
	m_BkHeight = MENUHEIGHT;
	m_BkWidth = 0;
	if (m_hBkPic) {
		GetObject(m_hBkPic,sizeof(bmp),&bmp);
		m_BkHeight = bmp.bmHeight;
		m_BkWidth = bmp.bmWidth;
	}

	if (m_hBkPicLeft) {
		GetObject(m_hBkPicLeft,sizeof(bmp),&bmp);
		if (m_BkHeight == bmp.bmHeight)
			m_BkLeftWidth = bmp.bmWidth;
	}
	if (m_hBkPicRight) {
		GetObject(m_hBkPicRight,sizeof(bmp),&bmp);
		if (m_BkHeight == bmp.bmHeight)
			m_BkRightWidth = bmp.bmWidth;
	}

	if(m_hSelBkPic) {
		GetObject(m_hSelBkPic,sizeof(bmp),&bmp);
		m_selBkHeight = bmp.bmHeight;
		m_selBkWidth = bmp.bmWidth;
	}

	m_selBkLeftWidth = 0;
	m_selBkRightWidth = 0;
	if (m_hSelBkPicLeft) {
		GetObject(m_hSelBkPicLeft,sizeof(bmp),&bmp);
		if (MENUHEIGHT == bmp.bmHeight)
			m_selBkLeftWidth = bmp.bmWidth;
	}
	if (m_hSelBkPicRight) {
		GetObject(m_hSelBkPicRight,sizeof(bmp),&bmp);
		if (MENUHEIGHT == bmp.bmHeight)
			m_selBkRightWidth = bmp.bmWidth;
	}

	if(m_hSepPic) {
		GetObject(m_hSepPic,sizeof(bmp),&bmp);
		m_sepPicHeight = bmp.bmHeight;
		m_sepPicWidth = bmp.bmWidth;
	}
	m_sepLeftWidth = 0;
	m_sepRightWidth = 0;
	if (m_hSepPicLeft) {
		GetObject(m_hSepPicLeft,sizeof(bmp),&bmp);
		if (m_sepPicHeight == bmp.bmHeight)
			m_sepLeftWidth = bmp.bmWidth;
	}
	if (m_hSepPicRight) {
		GetObject(m_hSepPicRight,sizeof(bmp),&bmp);
		if (m_sepPicHeight == bmp.bmHeight)
			m_sepRightWidth = bmp.bmWidth;
	}

	if (m_hTitalPic) {
		GetObject(m_hTitalPic,sizeof(bmp),&bmp);
		if( MENUHEIGHT == bmp.bmHeight)
			m_titalPicWidth = bmp.bmWidth;
	}
}

// rebuild the menu, needed when lang is changed to adjust size;
void COwnerDrawMenu::UpdateRoot()
{
	HMENU hNewMenu = CreatePopupMenu();
	if (IsMenu(hNewMenu)) {
		int n = GetMenuItemCount(Menu());
		for (int i = 0; i < n; ++i) {
			HMENU hSub = GetSubMenu(Menu(),0);
			if (IsMenu(hSub)) {
				InsertMenu(hNewMenu, i, MF_BYPOSITION | MF_OWNERDRAW | MF_POPUP | MF_STRING, reinterpret_cast<UINT_PTR>(hSub),NULL);
			}
			else {
				InsertMenu(hNewMenu, i, MF_BYPOSITION | MF_OWNERDRAW | MF_STRING, GetMenuItemID(Menu(),0),NULL);
			}
			CheckMenuItem(hNewMenu, i, MF_BYPOSITION | (MF_CHECKED & GetMenuState(Menu(),0, MF_BYPOSITION) ) );
			RemoveMenu(Menu(),0,MF_BYPOSITION);
		}
		DestroyMenu(Menu());
		m_hMenu = hNewMenu;
	}
}

//! 显示
int COwnerDrawMenu::Display(int x, int y, WINDOWTYPE hWnd, UINT uFlag)
{
	SelID((UINT)-1);
	int id = 0;
#ifndef TPM_NOANIMATION
#define TPM_NOANIMATION 0x4000L
#endif
	uFlag |= TPM_NOANIMATION;
	HWND hWndMenu(hWnd);
	if (!hWndMenu || !IsWindow(hWndMenu)) {
		hWndMenu = m_hWnd;
		uFlag |= TPM_RETURNCMD | TPM_NONOTIFY;
	}
	AllowSetForegroundWindow(GetCurrentProcessId());
	SetForegroundWindow(hWndMenu);

	id = TrackPopupMenuEx(Menu(), uFlag, x, y, hWndMenu, NULL);
//	if (!id) {
//		DWORD dwErr = GetLastError();//maybe 1446 : Popup menu already active.
//		int a = id;
//	}

	PostMessage(hWndMenu, WM_NULL, 0, 0);
	return id;
}

//! 更改现有 菜单项 的显示名称
bool COwnerDrawMenu::SetName(IDTYPE ID, const TSTRING & strNewName)
{
	if(m_ItemName.find(ID) != m_ItemName.end()) {
		m_ItemName[ID] = strNewName;
		return true;
	}
	return false;
}


//! 更改现有 子菜单 的显示名称
bool COwnerDrawMenu::SetName(MENUTYPE hSubMenu, const TSTRING & strNewName)
{
	if(m_MenuName.find(hSubMenu) != m_MenuName.end()) {
		m_MenuName[hSubMenu] = strNewName;
		return true;
	}
	return false;
}

//! 更改现有菜单条目的名称
bool COwnerDrawMenu::SetNameByPos( const int i, const TSTRING & strNewName)
{
	bool r(false);
	if( 0 <= i && i < GetMenuItemCount(Menu()) ) {
		MENUTYPE hMenu = GetSubMenu(Menu(),i);
		if(IsMenu(hMenu)) {
			r = SetName(hMenu, strNewName);
		}
		else {
			r = SetName(GetMenuItemID(Menu(),i), strNewName);
		}
	}
	return r;
}

bool COwnerDrawMenu::IconByPos(const int iPos, const ICONTYPE hIcon) {
	bool r = false;
	if( 0 <= iPos && iPos < GetMenuItemCount(Menu()) ) {
		//MENUTYPE hMenu = GetSubMenu(Menu(),iPos);
		if(MENUTYPE hMenu = GetSubMenu(Menu(),iPos) ) {//IsMenu(hMenu)) {
			r = MenuIcon(hMenu, hIcon);
		} else {
			IDTYPE id = GetMenuItemID(Menu(),iPos);
			if (id && id != static_cast<IDTYPE>(-1) ) {
				r = ItemIcon(id, hIcon);
			}
		}
	}
	return r;
}

//! 插入一个菜单项
BOOL COwnerDrawMenu::Insert(IDTYPE ID,const TCHAR * strName, UINT pos, ICONTYPE hIcon)
{
	if (IsMenu(ForceCast<MENUTYPE,IDTYPE>(ID))) {
#ifdef _DEBUG
		TrackPopupMenu(ForceCast<MENUTYPE,IDTYPE>(ID), 0, 0, 0,0,m_hWnd,0);
#endif
		DestroyMenu(ForceCast<MENUTYPE,IDTYPE>(ID));
	}

	//assert( ! IsMenu(ForceCast<MENUTYPE,IDTYPE>(ID)));

	if(hIcon) {
		ItemIcon(ID, hIcon);
	}

	if(m_ItemName.find(ID) != m_ItemName.end()) {
		AddToMap(m_ItemName,ID,strName);
		return ModifyMenu(m_hMenu,ID,MF_BYCOMMAND | MF_OWNERDRAW | (0xFF & GetMenuState(m_hMenu,ID,MF_BYCOMMAND) ),ID,NULL);
	}
	AddToMap(m_ItemName,ID,strName);
	return InsertMenu(m_hMenu,pos,MF_BYPOSITION | MF_OWNERDRAW,ID,NULL);//ItemName(ID));
}

 BOOL COwnerDrawMenu::InsertSep(UINT pos, MENUTYPE hMenu) {
	 return InsertMenu(hMenu?hMenu:m_hMenu,pos,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
}

//Get ItemIcon
const ICONTYPE COwnerDrawMenu::ItemIcon(const int nID) const
{
	return m_MenuItemIcons[nID];
}

//Set ItemIcon
bool COwnerDrawMenu::ItemIcon(const int nID, const ICONTYPE hIcon)
{
	return m_MenuItemIcons.Add(nID, hIcon);
}


//Get MenuIcon
const ICONTYPE COwnerDrawMenu::MenuIcon(const MENUTYPE hSubMenu) const
{
	return m_SubMenuIcons[hSubMenu];
}

//Set MenuIcon
bool COwnerDrawMenu::MenuIcon(const MENUTYPE hSubMenu, const ICONTYPE hIcon)
{
	return m_SubMenuIcons.Add(hSubMenu, hIcon);
}


void COwnerDrawMenu::TransformMenu(MENUTYPE hSubMenu)
{
	const int count = GetMenuItemCount(hSubMenu);
	if (count) {
		const int iStrSize = MAX_PATH;
		TCHAR szMenuName[iStrSize];
		for (int i = 0; i < count; ++i) {
			memset(szMenuName, 0, sizeof(szMenuName));
			GetMenuString(hSubMenu, i, szMenuName, iStrSize, MF_BYPOSITION);
			MENUTYPE hSubSub = GetSubMenu(hSubMenu, i);
			if (IsMenu(hSubSub)) {
				AddToMap(m_MenuName, hSubSub, szMenuName);
				TransformMenu(hSubSub);
				ModifyMenu(hSubMenu,i, MF_BYPOSITION | MF_OWNERDRAW | (0xFF & GetMenuState(hSubMenu, i, MF_BYPOSITION) ), reinterpret_cast<UINT_PTR>(hSubSub), NULL);

			}
			else {
				IDTYPE id = GetMenuItemID(hSubMenu, i);
				AddToMap(m_ItemName, id, szMenuName);
				ModifyMenu(hSubMenu,i, MF_BYPOSITION | MF_OWNERDRAW | (0xFF & GetMenuState(hSubMenu, i, MF_BYPOSITION) ), id, NULL);
			}
		}
	}
}

//! 插入一个子菜单
BOOL COwnerDrawMenu::Insert(MENUTYPE hSubMenu,const TCHAR * strName, UINT pos, ICONTYPE hIcon)
{
	assert (IsMenu(hSubMenu)) ;
	assert (m_MenuName.find(hSubMenu) == m_MenuName.end());
	if (m_MenuName.find(hSubMenu) != m_MenuName.end())
		return 0;
	if (hIcon) {
		MenuIcon(hSubMenu, hIcon);
	}
	AddToMap(m_MenuName,hSubMenu, strName);
	TransformMenu(hSubMenu);
	return InsertMenu(m_hMenu, pos, MF_BYPOSITION | MF_POPUP | MF_OWNERDRAW,reinterpret_cast<UINT_PTR>(hSubMenu),NULL);
}


// 销毁菜单内容，仅供 reset() 和 析构函数 调用
void COwnerDrawMenu::Destroy(void)
{
	m_ItemName.clear();	//ClearMap(m_ItemName);
	m_MenuName.clear(); //ClearMap(m_MenuName);
	m_MenuItemIcons.Clear();
	m_SubMenuIcons.Clear();
	if (m_hMenu)
	{
		DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}

}


//! 重新初始化，清空菜单。
int COwnerDrawMenu::Reset(void)
{
	Destroy();
	m_hMenu = CreatePopupMenu();
	return 0;
}


int COwnerDrawMenu::FillRectWithColor(HDC hdc,RECT & rect,COLORREF color)
{
	return FillRect(hdc,&rect,gdi_ptr<HBRUSH>(CreateSolidBrush(color)));
}

#ifdef _DEBUG
bool COwnerDrawMenu::IsParentAndChild(MENUTYPE hMenu, UINT_PTR uItemID)
{
	if(uItemID == (UINT_PTR)-1 || !IsMenu(hMenu))
		return false;
	int count = GetMenuItemCount(hMenu);
	MENUTYPE hSub;
	if (IsMenu((MENUTYPE)uItemID)) {
		for (int i = 0; i < count; ++i)
		{
			hSub = GetSubMenu(hMenu,i);
			if (hSub && ( hSub == (MENUTYPE)uItemID || IsParentAndChild(hSub,uItemID) ) )
				return true;
		}
	}
	else {
		for (int i = 0; i < count; ++i) {
			hSub = GetSubMenu(hMenu,i);
			if(hSub) {
				if(IsParentAndChild(hSub,uItemID))
					return true;
			}
			else if(GetMenuItemID(hMenu,i) == uItemID)
				return true;
		}
	}
	return false;
}
#endif

void COwnerDrawMenu::DrawSkin(HDC hdc, const RECT & rect,
							  BITMAPTYPE hMid, int iMidW, int iMidH,
							  BITMAPTYPE hLeft, int iLeftW,
							  BITMAPTYPE hRight, int iRightW)
{
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;
	BITMAPTYPE hold = (BITMAPTYPE)SelectObject(m_hSkinDC, hMid);
	StretchBlt(hdc, rect.left + iLeftW, rect.top, cx - iLeftW - iRightW, cy,
		m_hSkinDC,0,0,iMidW,iMidH,SRCCOPY);
	if (iLeftW) {
		SelectObject(m_hSkinDC, hLeft);
		BitBlt(hdc, rect.left, rect.top, iLeftW, iMidH, m_hSkinDC, 0,0,SRCCOPY);
	}
	if (iRightW) {
		SelectObject(m_hSkinDC, hRight);
		BitBlt(hdc, rect.right - iRightW, rect.top, iRightW, iMidH, m_hSkinDC, 0,0,SRCCOPY);
	}
	SelectObject(m_hSkinDC, hold);
}

bool COwnerDrawMenu::AccordingToState(DRAWITEMSTRUCT * pDI)
{
	COLORREF bk = m_vClrs[ClrIndex_Bk];
	COLORREF Fr = m_vClrs[ClrIndex_Fr];

	SetBkMode(pDI->hDC, TRANSPARENT);

	RECT rect = pDI->rcItem;

	if (m_hSidePic && m_hSkinDC) {
		BITMAPTYPE hold = (BITMAPTYPE)SelectObject(m_hSkinDC, m_hSidePic);
		BitBlt(pDI->hDC, pDI->rcItem.left,pDI->rcItem.top, m_iExtraLeftSideWidth, MENUHEIGHT, m_hSkinDC, 0,0, SRCCOPY);
		SelectObject(m_hSkinDC, hold);
	}
	else {
		rect.right = m_iExtraLeftSideWidth >> 1;
		FillRectWithColor(pDI->hDC,rect,m_vClrs[ClrIndex_Side]);
		OffsetRect(&rect,rect.right-rect.left,0);
		FillRectWithColor(pDI->hDC,rect,m_vClrs[ClrIndex_Bk]);
	}

	pDI->rcItem.left += m_iExtraLeftSideWidth;
	rect = pDI->rcItem;

	// 设置 菜单

	if (!pDI->itemID) // 分隔线
	{
		if (m_hSepPic && m_hSkinDC) {
			DrawSkin(pDI->hDC, rect, m_hSepPic, m_sepPicWidth, m_sepPicHeight,
				m_hSepPicLeft, m_sepLeftWidth,
				m_hSepPicRight, m_sepRightWidth);
			return true;
		}
		else {
			FillRectWithColor(pDI->hDC,pDI->rcItem, m_vClrs[ClrIndex_Bk]);
			rect.top += MENUSEP / 2;
			rect.bottom = rect.top + 1;
			bk = m_vClrs[ClrIndex_Separator];
		}
	}
	else if (pDI->itemState & ODS_GRAYED) // 无效项目
	{
		SetTextColor(pDI->hDC,m_vClrs[ClrIndex_FrGrayed]);
		bk = m_vClrs[ClrIndex_BkGrayed];
	}
	else if (pDI->itemState & ODS_DISABLED)// 标题
	{
		if (m_hTitalPic && m_hSkinDC) {
			BITMAPTYPE hold = (BITMAPTYPE)SelectObject(m_hSkinDC, m_hTitalPic);
			StretchBlt(pDI->hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
				m_hSkinDC, 0, 0, m_titalPicWidth, MENUHEIGHT, SRCCOPY);
			SelectObject(m_hSkinDC, hold);
			return true;
		}
		else {
			SetTextColor(pDI->hDC,m_vClrs[ClrIndex_FrDisabled]);
			FillRectWithColor(pDI->hDC,rect,m_vClrs[ClrIndex_Bk]);
			++rect.top;
			--rect.bottom;
			bk = m_vClrs[ClrIndex_BkDisabled];
		}
	}
	else if (pDI->itemState & ODS_SELECTED) // 当前选中的
	{
		if(m_hSelBkPic && m_hSkinDC) {
			DrawSkin(pDI->hDC, rect, m_hSelBkPic, m_selBkWidth, m_selBkHeight,
				m_hSelBkPicLeft, m_selBkLeftWidth,
				m_hSelBkPicRight, m_selBkRightWidth);
			return true;
		}
		else {
		//*	// 蓝色边框

			FillRectWithColor(pDI->hDC,rect,m_vClrs[ClrIndex_SelBorder]);
			--rect.right;
			--rect.bottom;
			++rect.left;
			++rect.top;
			bk = m_vClrs[ClrIndex_SelBk];
		/*/
			//三维效果 //todo 如何判断鼠标是否按下
			FillRectWithColor(pDI->hDC,rect,RGB(64,64,64));
			--rect.right;
			--rect.bottom;
			FillRectWithColor(pDI->hDC,rect,RGB(255,255,255));
			++rect.left;
			++rect.top;
			bk = m_vClrs[ClrIndex_Sel];
		// */
		}
	}
	//普通的未选中菜单项

	SetTextColor(pDI->hDC, Fr);

	if(m_hBkPic && m_hSkinDC) {
		DrawSkin(pDI->hDC, rect, m_hBkPic, m_BkWidth, m_BkHeight,
			m_hBkPicLeft, m_BkLeftWidth,
			m_hBkPicRight, m_BkRightWidth);
		return true;
	}
	else {
		SetBkColor(pDI->hDC,bk);
		FillRectWithColor(pDI->hDC,rect,bk);
	}

	return true;
}


MENUTYPE COwnerDrawMenu::TryGetSubMenu(const DRAWITEMSTRUCT * pDI)
{
	HMENU hResult = 0;
	if (ODT_MENU == pDI->CtlType) {
		if (const unsigned int id = pDI->itemID) {
			IdStrIter last = m_ItemName.end();
			if (m_ItemName.empty() || (--last)->first < id) {
				MenuStrMap::const_iterator it = m_MenuName.find(reinterpret_cast<MENUTYPE>(id));
				if (it != m_MenuName.end()) {
					assert (id >= (1 << 16));
					hResult = it->first;
				}
			}
			else {
				//assert(false);
			}
		}
	}
	return hResult;
}


bool COwnerDrawMenu::DrawMenuIcon(const DRAWITEMSTRUCT *pDI) {
	bool bDrawed = false;

	int xBlank = MENUICON / 2 + MENUBLANK;

	//两个可能的类型，菜单与项
	IDTYPE iMaybeID = pDI->itemID;
	MENUTYPE hMaybeMenu = TryGetSubMenu(pDI);
	HICON hIcon = 0;
	if ( hMaybeMenu ) {
		assert( IsMenu(hMaybeMenu) );
		hIcon = MenuIcon(hMaybeMenu);
	} else  {
		hIcon = ItemIcon(iMaybeID);
	}
	// a function to get icon size;
	class CGetIconSize {
	public:
		static bool GetIconSize(ICONTYPE hIconIn, int &x, int &y) {
			bool r = false;
			ICONINFO ii = {0};
			if (GetIconInfo(hIconIn, &ii)) {
				x = ii.xHotspot * 2;
				y = ii.yHotspot * 2;
				DeleteObject(ii.hbmColor);
				DeleteObject(ii.hbmMask);
				r = true;
			}
			return r;
		}
	};

	if (hIcon) {
		int x = MENUICON, y = MENUICON;
		if (UseActualIconSize()) {
			CGetIconSize::GetIconSize(hIcon, x,y);
		}
		DrawIconEx(pDI->hDC,pDI->rcItem.left + xBlank - x/2, pDI->rcItem.top + (MENUHEIGHT - y)/2, hIcon, x, y, 0,NULL,DI_NORMAL);
		bDrawed = true;
		xBlank += x;
	}

	if(m_hIconCheck && (pDI->itemState & ODS_CHECKED)) {
		hIcon = m_hIconCheck;
		int x = MENUICON, y = MENUICON;
		if (UseActualIconSize()) {
			CGetIconSize::GetIconSize(hIcon, x,y);
		}
		DrawIconEx(pDI->hDC,pDI->rcItem.left + xBlank - x/2, pDI->rcItem.top + (MENUHEIGHT - y)/2, hIcon, x, y, 0,NULL,DI_NORMAL);
	}

	return bDrawed;

}

// return true if drawed icon
bool COwnerDrawMenu::DrawItem_impl(DRAWITEMSTRUCT * pDI)
{
	if ((!pDI) || (pDI->rcItem.bottom == pDI->rcItem.top)) {
		return false;
	}

	AccordingToState(pDI);

	IDTYPE iMaybeID = pDI->itemID;
	MENUTYPE hMaybeMenu = TryGetSubMenu(pDI);
	assert(!hMaybeMenu || hMaybeMenu == (HMENU)iMaybeID);

	const bool bDrawedIcon = DrawMenuIcon(pDI);

	const TCHAR *str = hMaybeMenu ? MenuName(hMaybeMenu) : ItemName(iMaybeID);

	if(str && *str) {
		RECT rect = pDI->rcItem;
		rect.left += MENUICON + MENUBLANK * 3;
		DrawText(pDI->hDC,str,-1,&(rect),DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	return bDrawedIcon;
}


bool COwnerDrawMenu::DrawItem(DRAWITEMSTRUCT * pDI) {
	DrawItem_impl(pDI);
	return true;
}

//! 实现，计算并设置菜单项的长宽
int COwnerDrawMenu::MeasureItem_impl(MEASUREITEMSTRUCT *pMI)
{
	if (!pMI || !pMI->itemID) {
		pMI->itemHeight = MENUSEP;
		pMI->itemWidth = 1;//分割线，长度默认很短。
	}
	else {
		if(m_hBkPic) {
			pMI->itemHeight = m_BkHeight;
		}
		else {
			pMI->itemHeight = MENUHEIGHT;
		}
		pMI->itemWidth = 80;// 菜单最小宽度，默认

		const TCHAR * str = Name(pMI->itemID);
		if (!str || !*str)
			return 0;

		if (szHiddenMenuItem == str) {
			pMI->itemHeight = 0;
		}
		else {
			MemDC_H hdc (CreateCompatibleDC(NULL));
			if(hdc) {
				SIZE size = {0};
				GetTextExtentPoint32(hdc,str,static_cast<int>(_tcslen(str)),&size);
				if (size.cy > static_cast<int>(pMI->itemHeight))
					pMI->itemHeight = size.cy;
				pMI->itemWidth = size.cx + MENUICON + m_iExtraLeftSideWidth;

				pMI->itemWidth += MENUBLANK*3;

				if (pMI->itemWidth > MAXMENUWIDTH)
					pMI->itemWidth = MAXMENUWIDTH;
			}
			else {
				DWORD dwError = GetLastError();
				dwError = dwError;
			}
		}
	}
	return pMI->itemWidth;
}

//! 计算并设置菜单项的长宽
int COwnerDrawMenu::MeasureItem(MEASUREITEMSTRUCT *pMI) {
	MeasureItem_impl(pMI);
	return true;
}

// 实现，记录当前选中菜单项ID或子菜单句柄
LRESULT COwnerDrawMenu::MenuSelect_impl(MENUTYPE hMenu,UINT uItem,UINT uFlags)
{
	if((uFlags & MF_GRAYED) || (uFlags & MF_DISABLED))
		return 0;
	if(uFlags & MF_POPUP) {
		m_selID = (UINT_PTR)GetSubMenu(hMenu,uItem);
	}
	else	//菜单项
		m_selID = uItem;
	return 0;
}

// 记录当前选中菜单项ID或子菜单句柄
LRESULT COwnerDrawMenu::MenuSelect(MENUTYPE hMenu,UINT uItem,UINT uFlags) {
	MenuSelect_impl(hMenu, uItem, uFlags);
	return 0;
}

// 实现，处理用户按键选择菜单项
LRESULT COwnerDrawMenu::MenuChar_impl(MENUTYPE hMenu,TCHAR ch)
{
	assert(hMenu == m_hMenu || IsParentAndChild(m_hMenu,(UINT_PTR)hMenu));
	int itemCount = GetMenuItemCount(hMenu);
	std::vector<int> vFound;
	const TCHAR *name;
	UINT_PTR id;
	MENUITEMINFO mi;
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STATE;
	for (int i = 0; i < itemCount; ++i) {
		id = 0;
		if (!GetMenuItemInfo(hMenu,i,TRUE,&mi) ||  (mi.fState & MFS_GRAYED))
			continue;
		if (mi.hSubMenu)
			id = (UINT_PTR)GetSubMenu(hMenu, i);
		else
			id = mi.wID;

		if (!id)
			continue;

		name = Name(id);
		if (!name)
			continue;
		const TCHAR * last = NULL;
		while (*name) {
			if (*name != '&') // not '&'
				++name;
			else if (*(name+1) == '&') // "&&"
				name+=2;
			else	// is "&" not "&&"
				last = name++;
		}

		if (!last) {
			name = Name(id);
			if(_totlower(*name) == _totlower(ch))
				vFound.push_back(i);
		}
		else if (_totlower(*(last+1)) == _totlower(ch))
			vFound.push_back(i);
	}

	if (vFound.empty())
		return MAKELRESULT(0,MNC_IGNORE);
	else if (1 == vFound.size() && !(GetKeyState(VK_SHIFT)&0x8000) )
		return MAKELRESULT(vFound.front(),MNC_EXECUTE);
	else if (SelID() != (UINT_PTR)-1)//有选中项目
		for (UINT j = 0; j + 1 < vFound.size(); ++j) {
			if ( ( GetSubMenu(hMenu,vFound[j]) && GetSubMenu(hMenu,vFound[j]) == (MENUTYPE)SelID() ) ||
				GetMenuItemID(hMenu,vFound[j]) == SelID())
				return MAKELRESULT(vFound[j+1],MNC_SELECT);//当前选中项目符合，选中下一个符合的
		}
	return MAKELRESULT(vFound.front(),MNC_SELECT);
}

// 处理用户按键选择菜单项
LRESULT COwnerDrawMenu::MenuChar(MENUTYPE hMenu,TCHAR ch) {
	return MenuChar_impl(hMenu, ch);
}



