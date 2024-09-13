#ifndef MENU_WITH_ICON_H
#define MENU_WITH_ICON_H

#include "OwnerDrawMenu.h"
#include "MenuData.h"

//! 初始化 com 的类，声明一个对象就可以了。也可以手动初始化
class ComIniter
{
	bool m_bSuccess;
public:
	ComIniter(bool bInit = true):m_bSuccess(false){
		if (bInit) {
			Init();
		}
	}
	~ComIniter() { UnInit(); }
	operator bool() const { return m_bSuccess; }
	bool Init() {
		return m_bSuccess = m_bSuccess || (SUCCEEDED(CoInitializeEx(NULL,COINIT_APARTMENTTHREADED)));
	}
	void UnInit() {
		if (m_bSuccess) {
			CoUninitialize();
			m_bSuccess = false;
		}
	}
};

// 带图标的命令菜单。
class CMenuWithIcon : public COwnerDrawMenu
{
	typedef COwnerDrawMenu Super;

public:
	CMenuWithIcon(ICONTYPE hOpen = NULL,ICONTYPE hClose = NULL,ICONTYPE hUnknownFile = NULL,const TCHAR * szEmpty = NULL);
	~CMenuWithIcon(void);
	void DefaultIcons(ICONTYPE hOpen,ICONTYPE hClose,ICONTYPE hUnknown);

	virtual int Display(int x, int y, WINDOWTYPE hWnd = NULL, UINT uFlag = TPM_LEFTALIGN);
private:
	bool DrawItem_impl(DRAWITEMSTRUCT * pDI);
	//int MeasureItem_impl(MEASUREITEMSTRUCT *pMI);
	LRESULT MenuSelect_impl(MENUTYPE hMenu,UINT uItem,UINT uFlags);
public:
	ICONTYPE GetBigIcon(const unsigned int id, int index = 0);
	ICONTYPE GetBigIcon(const tString & path, const int index = 0) {return GetIcon(path,FILEFOLDERICON,index,true);}
	//! 返回菜单项对应的命令行的参数
	const TCHAR * Param(const IDTYPE nID) const {return GetStr(m_ItemParam,nID);};
	//! 返回菜单项对应的命令行(不含参数)
	const TCHAR * Cmd(const IDTYPE nID) const {return GetStr(m_ItemCmd,nID);};
	unsigned int Find(const TSTRING& strName, TSTRING& strPath) const;
	unsigned int FindAll(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName, bool bAllowDup = false) const;
	unsigned int FindAllBeginWith(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName, bool bAllowDup = false) const;
	bool TryProcessCommand(unsigned int id);
	// const TSTRING GetCurrentCommandLine(unsigned int nSysID);
	// int ItemIDCount(){return m_ID - m_startID;};
	void DestroyDynamic(void);
	int BuildDynamic(MENUTYPE hSubMenu);
	bool & ShowHidden() {return m_bShowHidden;};

	int LoadMenuFromFile(const tString & strFileName, UINT uStartID);
	//int BuildMenu(FILE *pFile,UINT uStartID);

private:
	int Reset(void);
	CMenuWithIcon(const CMenuWithIcon &);

	enum EICONGETTYPE{NOICON, FILEFOLDERICON, FILEICON};

	//typedef std::map<TSTRING, MENUTYPE> StrMenuMap;
	typedef std::map<TSTRING, TSTRING> StrStrMap;

	bool AddSubMenu(MENUTYPE hMenu,MENUTYPE hSubMenu,const tString & strName, const tString & strIconPath, EICONGETTYPE needIcon = FILEFOLDERICON);
	int AddMenuItem(MENUTYPE hMenu, const tString & strName, const tString & inStrPath, EICONGETTYPE needIcon = FILEFOLDERICON, const tString & strIcon = _T(""));
	int MultiAddMenuItem(MENUTYPE hMenu, const tString & inStrPath,const tString & strName);
	int DoBuildDynamic(MENUTYPE hMenu);
	void Destroy(void);
	ICONTYPE GetIcon(const tString & strPath, EICONGETTYPE needIcon = FILEFOLDERICON,int iconIndex = 0, bool bIcon32 = false);
	ICONTYPE GetShortCutIcon(LPCTSTR lpszLinkFile, bool bIcon32 = true);

	bool GetDirectory(const IDTYPE nID, TSTRING & strWorkPath);


	bool IsStaticMenu(MENUTYPE hMenu)
	{
		return m_StaticPath.find(hMenu) != m_StaticPath.end();
	}
	bool IsDynamicMenu(MENUTYPE hMenu)
	{
		return m_DynamicPath.find(hMenu) != m_DynamicPath.end();
	}
	bool IsExpanedMenu(MENUTYPE hMenu)
	{
		return m_ExpanedMenu.find(hMenu) != m_ExpanedMenu.end();
	}
	// 添加通配符记录，返回索引，失败返回 -1.
	int AddWildcard(const tString & str);
	// 根据索引，返回通配符，没有返回 NULL
	const TCHAR * GetWildcard(int index);


private :
	enum EBUILDMODE {EFILE,EFOLDER,EEXPAND,EEXPANDNOW,EDYNAMIC};
	int GetFilteredFileList(const TSTRING & inStrPathForSearch, const TCHAR * szFilter, std::vector<TSTRING> & vResult);
	int DoMultiModeBuildMenu(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon);
	int MultiModeBuildMenu(MENUTYPE, const tString & strPath, const tString & strName, EBUILDMODE mode,bool bNoFileIcon = false);

	//! 找出匹配，加入到指定字符串向量末尾，bNoDup = true 已存在的跳过。
	template <class Cond>
	unsigned int FindIf(Cond cond, std::vector<TSTRING> &vStrName, bool bAllowDup) const
	{
		unsigned int iFound = 0;

		for(std::map<TSTRING,IDTYPE>::const_iterator iter = m_NameIdMap.begin(); iter != m_NameIdMap.end(); ++iter) { //m_NameIdMap是按照字母表顺序的
			if(cond(iter->first)) {
				bool bIgnoreThis = false;
				if (!bAllowDup) {
					for (std::vector<TSTRING>::size_type i = 0; i < vStrName.size(); ++i) {
						if (vStrName[i].length() == iter->first.length() && _tcsicmp(vStrName[i].c_str(), iter->first.c_str()) == 0) {
							//不考虑大小写，相同
							bIgnoreThis = true;
							break;
						}
					}
				}
				if (!bIgnoreThis) {
					const TCHAR * pCmd = Cmd(iter->second); //用于排除标题。
					if (pCmd && *pCmd) {
						++iFound;
						tString strName = ItemName(iter->second);
						//移除每次出现的第一个 &
						TSTRING::size_type len = strName.length();
						TSTRING::size_type move = 0;
						for (TSTRING::size_type j = 0; j < len; ++j) {
							if(strName[j] == '&') {
								++move;
								++j;
							}
							if(move)
								strName[j-move] = strName[j];
						}
						//截断;
						strName.resize(strName.size() - move);
						vStrName.push_back(strName);
					}
				}
			}
		}
		return iFound;
	}

	std::map<TSTRING, IDTYPE> m_NameIdMap;//用于查找名称和命令的对应关系,全部小写字母
	HIcon m_hIconOpen;
	HIcon m_hIconClose;
	HIcon m_hIconUnknowFile;

	IdStrMap m_ItemCmd;
	IdStrMap m_ItemParam;
	IdStrMap m_ItemIconPath;


	IDTYPE m_startID;
	IDTYPE m_ID;
	ComIniter m_comInited; //com 是否已经初始化

	//typedef const std::vector<TSTRING> wildcard_type;
	//std::vector<wildcard_type> m_Wildcard;//文件通配符条件
	std::vector<TSTRING> m_Wildcard;//文件通配符条件

	TSTRING m_strEmpty;
	std::map<MENUTYPE,int> m_StaticMenuWildcard;// 静态目录的文件通配符索引
	std::map<MENUTYPE,int> m_DynamicMenuWildcard;// 动态目录的文件通配符索引
	MenuStrMap m_StaticPath;//不清理的 根 动态路径
	MenuStrMap m_DynamicPath;// 运行时动态清理的路径
	MenuStrMap m_ExpanedMenu;//不清理的，不存储图标。
	IDTYPE m_dynamicStartID;//记录动态菜单项起始，用于销毁

	bool m_bShowHidden;
	bool m_bFilterEmptySubMenus;

	Ptr<CMenuData> m_menuData;
	//
	int BuildMenuFromMenuData(CMenuData *pMenu, MENUTYPE hMenu);
	int BuildMyComputer(MENUTYPE hMenu, const tString & strName);

public:
	bool OpenDynamicDir() { return m_bOpenDynamicDir; }
	void OpenDynamicDir(const bool newValue) { m_bOpenDynamicDir = newValue; }
	bool HasMyComputer() const { return m_bHasMyComputer; }
private:
	void HasMyComputer(const bool newValue)  { m_bHasMyComputer = newValue; }
	bool m_bOpenDynamicDir;//option to open dir on double click
	bool m_bHasMyComputer;// contain \\** mode

};

#endif //MENU_WITH_ICON_H
