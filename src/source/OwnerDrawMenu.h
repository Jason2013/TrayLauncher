#ifndef OWNER_DRAW_MENU_H
#define OWNER_DRAW_MENU_H

#include <stdheaders.h>

#include <auto_pointers.h>
#include "MsgMap.h"

typedef unsigned int IDTYPE;
typedef HMENU MENUTYPE;
typedef HICON ICONTYPE;
typedef HWND WINDOWTYPE;

//! 图标 自动指针
class IconTypeDestroyer{
public :
	static void Free(ICONTYPE &hIcon)
	{
		if (hIcon)
		{
			DestroyIcon(hIcon);
			hIcon = NULL;
		}
	}
};
typedef auto_handle<HICON, IconTypeDestroyer, 0, false>  HIcon;

//! 自绘菜单类
class COwnerDrawMenu
{
	AutoHwnd m_hWnd;

	bool m_bUseAble;

public:

	COwnerDrawMenu(ICONTYPE hIconCheck = NULL, const unsigned int iLeftSide = MENUSIDE);
	virtual ~COwnerDrawMenu(void);
	virtual int Display(int x, int y, WINDOWTYPE hWnd = NULL, UINT uFlag = TPM_LEFTALIGN);
	//const UINT_PTR GetCurrentItem() {return SelID();}
	const MENUTYPE Menu() const {return m_hMenu;};
	const TCHAR * Name (const UINT_PTR indexKey) const
	{
		const TCHAR * p = ItemName((IDTYPE)indexKey);
		if (!p && IsMenu((MENUTYPE)indexKey))
			return MenuName((MENUTYPE)indexKey);
		return p;
	}
	const TCHAR * ItemName(const IDTYPE indexKey) const	{return GetStr(m_ItemName,indexKey);};
	const TCHAR * MenuName(const MENUTYPE indexKey)	const {return GetStr(m_MenuName,indexKey);};
	bool SetName(IDTYPE ID, const TSTRING & strNewName);
	bool SetName(MENUTYPE hSubMenu, const TSTRING & strNewName);
	bool SetNameByPos(const int i, const TSTRING & strNewName);
	BOOL Insert(IDTYPE ID,const TCHAR * strName, UINT pos = static_cast<UINT>(-1), ICONTYPE hIcon = NULL);
	BOOL Insert(MENUTYPE hSubMenu,const TCHAR * strName, UINT pos = static_cast<UINT>(-1), ICONTYPE hIcon = NULL);
	BOOL InsertSep(UINT pos = static_cast<UINT>(-1), MENUTYPE hMenu = 0);
	//void AddStaticIcon(IDTYPE ID,ICONTYPE hIcon);
	//void AddStaticIcon(MENUTYPE hSub, ICONTYPE hIcon);
	int Reset();
	UINT SetColor(int index, UINT value);//设置颜色
	void UpdateRoot();

	const ICONTYPE ItemIcon(const int nID) const;
	bool ItemIcon(const int nID, const ICONTYPE hIcon);

	const ICONTYPE MenuIcon(const MENUTYPE hSubMenu) const;
	bool MenuIcon(const MENUTYPE hSubMenu, const ICONTYPE hIcon);

	bool IconByPos(const int iPos, const ICONTYPE hIcon);

protected:
	UINT_PTR SelID() const {return m_selID;}
	void SelID(UINT_PTR id) { m_selID = id; }

	typedef std::map<IDTYPE, TSTRING> IdStrMap;
	typedef IdStrMap :: const_iterator IdStrIter;

	typedef std::map<MENUTYPE, TSTRING> MenuStrMap;
	typedef MenuStrMap :: const_iterator MenuStrIter;

	typedef std::map<IDTYPE,ICONTYPE> IdIconMap;
	typedef IdIconMap::const_iterator IdIconIter;

	typedef std::map<MENUTYPE,ICONTYPE> MenuIconMap;
	typedef MenuIconMap::const_iterator MenuIconIter;

	enum {MENUSIDE = 6, MENUHEIGHT = 22, MENUICON = 16, MENUBLANK = MENUHEIGHT - MENUICON, MENUSEP = 5,MAXMENUWIDTH = 384,NBUF = 1024, SHELL_MAX_ERROR_VALUE = 32};
	IdStrMap & ItemNameMap() {return m_ItemName;};
	MenuStrMap & MenuNameMap() {return m_MenuName;};


	// 模板的成员函数定义

	template <class To,class From> static const To ForceCast(From from)
	{
		return reinterpret_cast<To>(from);
	};

	template<class IdMenu>static bool AddToMap(std::map<IdMenu,TSTRING> &strMap, IdMenu indexKey, const TSTRING & str)
	{
		//if (str.empty())
		//	return false;
		strMap[indexKey] = str;
		return true;
	}

	template<class IdMenu> static const TCHAR * GetStr(const std::map<IdMenu,TSTRING> &strMap, const IdMenu indexKey)
	{
		typename std::map<IdMenu,TSTRING>::const_iterator it = strMap.find(indexKey);
		if (it == strMap.end())
			return 0;
		return it->second.c_str();
	}

	// auto_handle 不适合作为容器元素，所以不存储 HIcon
	// 模板类定义，存放 菜单-图标 的类
	template <class CKey, class CValue>
	class CNoNullIconMap
	{
	public:
		CNoNullIconMap(){}
		~CNoNullIconMap() { Clear(); }
		bool Add(const CKey & key, const CValue & value) {
			// @todo: value == map[key] ???
			Remove(key);
			const bool r = value?true:false;
			if (r) {
				m_map[key] = value;
			}
			return r;
		}
		void Remove(const CKey & key)
		{
			if (m_map.find(key) != m_map.end()) {
				DestroyIcon(m_map[key]);
				m_map.erase(key);
			}
		}
		void Clear()
		{
			for (typename CMap::iterator it = m_map.begin(); it != m_map.end(); ++it) {
				DestroyIcon(it->second);
			}
			m_map.clear();
		}
		CValue operator [] (const CKey & key) const
		{
			typename CMap::const_iterator pos(m_map.find(key));
			if (pos != m_map.end())
				return pos->second;
			return 0;
		}
	private:
		typedef std::map<CKey,CValue> CMap;
		CMap m_map;
		CNoNullIconMap(const CNoNullIconMap &);
		CNoNullIconMap & operator = (const CNoNullIconMap &);
	};
	typedef CNoNullIconMap<IDTYPE,HICON> CIdIconMap;
	typedef CNoNullIconMap<MENUTYPE,HICON> CMenuIconMap;

	void Destroy(void);
	bool AccordingToState(DRAWITEMSTRUCT * pDI);

private:
	COwnerDrawMenu(const COwnerDrawMenu &);//禁止复制构造
	COwnerDrawMenu & operator = (const COwnerDrawMenu &) ;//禁止赋值

	// just for design test
	bool IsParentAndChild(MENUTYPE hMenu,UINT_PTR uItemID);

	int FillRectWithColor(HDC hdc,RECT & rect,COLORREF color);
	void TransformMenu(MENUTYPE hSubMenu);

	MENUTYPE m_hMenu;
	ICONTYPE m_hIconCheck;
	IdStrMap m_ItemName;
	MenuStrMap m_MenuName;
	UINT_PTR m_selID;
	enum e_ClrIndex{ClrIndex_Bk,ClrIndex_BkGrayed,ClrIndex_BkDisabled,
					ClrIndex_Fr,ClrIndex_FrGrayed,ClrIndex_FrDisabled,
					ClrIndex_SelBk, ClrIndex_SelFr,ClrIndex_SelBorder,
					ClrIndex_Side,ClrIndex_Separator,
					ClrIndex_Num};
	std::vector<COLORREF> m_vClrs;

	//store icons
	CIdIconMap m_MenuItemIcons;
	//CIdIconMap m_MenuItemCheckIcons;
	CMenuIconMap m_SubMenuIcons;

	class CWindowClass
	{
		CWindowClass(const CWindowClass &);
		CWindowClass & operator = (const CWindowClass &);
		const TSTRING m_strWndClassName;
		typedef LRESULT (CALLBACK *WindowProcessor)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		CWindowClass(HINSTANCE hInstance, const TCHAR * szWindowClass, WindowProcessor WndProc);
		operator const TCHAR * () const {return m_strWndClassName.c_str();}
	};
	static CWindowClass s_windowClass;

private:
	// 静态成员，处理窗口消息。

	static const TCHAR *szMenuWindowClass;

	static LRESULT  MsgDrawItem(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMeasureItem(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMenuChar(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMenuSelect(HWND, UINT, WPARAM, LPARAM);

	static CMsgMap msgMap;

	static LRESULT CALLBACK MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	typedef std::map<COwnerDrawMenu * , bool> MenuObjects;// count total menus
	static MenuObjects s_Menus;

	void AddThis() { s_Menus[this] = true; }
	void RemoveThis() { s_Menus.erase(this); }

protected:
	MENUTYPE TryGetSubMenu(const DRAWITEMSTRUCT * pDI);
	bool DrawItem(DRAWITEMSTRUCT * pDI);
	int MeasureItem(MEASUREITEMSTRUCT *pMI);
	LRESULT MenuChar(MENUTYPE hMenu,TCHAR ch);
	LRESULT MenuSelect(MENUTYPE hMenu,UINT uItem,UINT uFlags);
	bool DrawMenuIcon(const DRAWITEMSTRUCT * pDI);

	virtual bool DrawItem_impl(DRAWITEMSTRUCT * pDI);
	virtual int MeasureItem_impl(MEASUREITEMSTRUCT *pMI);
	virtual LRESULT MenuChar_impl(MENUTYPE hMenu,TCHAR ch);
	virtual LRESULT MenuSelect_impl(MENUTYPE hMenu,UINT uItem,UINT uFlags);
public:
	typedef HBITMAP BITMAPTYPE;
	void SetSkin(BITMAPTYPE hSide, const BITMAPTYPE (&hBk)[3], const BITMAPTYPE (&hSelBk)[3], const BITMAPTYPE (&hSep)[3], BITMAPTYPE hTitalPic);

	// Set UseActualIconSize
	void UseActualIconSize (const bool newValue) { bUseActualIconSize = newValue; }

	// Get UseActualIconSize
	const bool UseActualIconSize() const { return bUseActualIconSize; }


private:
	// skin about



	MemDC_H m_hSkinDC;

	void DrawSkin(HDC hdc, const RECT & rect,
							  BITMAPTYPE hMid, int iMidW, int iMidH,
							  BITMAPTYPE hLeft, int iLeftW,
							  BITMAPTYPE hRight, int iRightW);
	typedef gdi_ptr<HBITMAP> SkinPic;
	SkinPic m_hSidePic;	//侧边背景

	SkinPic m_hSepPic;	 //分隔符, 中间
	int m_sepPicHeight;
	int m_sepPicWidth;

	SkinPic m_hSepPicLeft;//分隔符, 左边
	int m_sepLeftWidth;
	SkinPic m_hSepPicRight;//分隔符, 右边
	int m_sepRightWidth;

	SkinPic m_hBkPic;   //背景,中间
	int m_BkHeight;
	int m_BkWidth;
	SkinPic m_hBkPicLeft;   //背景,左边
	int m_BkLeftWidth;
	SkinPic m_hBkPicRight;   //背景,右边
	int m_BkRightWidth;

	SkinPic m_hSelBkPic;   //选中的背景,中间
	int m_selBkHeight;
	int m_selBkWidth;

	SkinPic m_hSelBkPicLeft;   //选中的背景,左边
	int m_selBkLeftWidth;
	SkinPic m_hSelBkPicRight;   //选中的背景,右边
	int m_selBkRightWidth;

	SkinPic m_hTitalPic;
	int m_titalPicWidth;

	bool bUseActualIconSize;
protected:
	static const TSTRING szHiddenMenuItem;// = _T("< . >");// normal items should not contain "<"
private:

// @note (lichao#1#): 实现左侧边缘可调，替代MENUSIDE
	int m_iExtraLeftSideWidth;// 左侧边缘图像宽度
};

#endif // OWNER_DRAW_MENU_H
