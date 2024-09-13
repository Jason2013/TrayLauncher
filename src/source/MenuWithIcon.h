#ifndef MENU_WITH_ICON_H
#define MENU_WITH_ICON_H

#include "OwnerDrawMenu.h"
#include "MenuData.h"

//! ��ʼ�� com ���࣬����һ������Ϳ����ˡ�Ҳ�����ֶ���ʼ��
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

// ��ͼ�������˵���
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
	//! ���ز˵����Ӧ�������еĲ���
	const TCHAR * Param(const IDTYPE nID) const {return GetStr(m_ItemParam,nID);};
	//! ���ز˵����Ӧ��������(��������)
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
	// ���ͨ�����¼������������ʧ�ܷ��� -1.
	int AddWildcard(const tString & str);
	// ��������������ͨ�����û�з��� NULL
	const TCHAR * GetWildcard(int index);


private :
	enum EBUILDMODE {EFILE,EFOLDER,EEXPAND,EEXPANDNOW,EDYNAMIC};
	int GetFilteredFileList(const TSTRING & inStrPathForSearch, const TCHAR * szFilter, std::vector<TSTRING> & vResult);
	int DoMultiModeBuildMenu(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon);
	int MultiModeBuildMenu(MENUTYPE, const tString & strPath, const tString & strName, EBUILDMODE mode,bool bNoFileIcon = false);

	//! �ҳ�ƥ�䣬���뵽ָ���ַ�������ĩβ��bNoDup = true �Ѵ��ڵ�������
	template <class Cond>
	unsigned int FindIf(Cond cond, std::vector<TSTRING> &vStrName, bool bAllowDup) const
	{
		unsigned int iFound = 0;

		for(std::map<TSTRING,IDTYPE>::const_iterator iter = m_NameIdMap.begin(); iter != m_NameIdMap.end(); ++iter) { //m_NameIdMap�ǰ�����ĸ��˳���
			if(cond(iter->first)) {
				bool bIgnoreThis = false;
				if (!bAllowDup) {
					for (std::vector<TSTRING>::size_type i = 0; i < vStrName.size(); ++i) {
						if (vStrName[i].length() == iter->first.length() && _tcsicmp(vStrName[i].c_str(), iter->first.c_str()) == 0) {
							//�����Ǵ�Сд����ͬ
							bIgnoreThis = true;
							break;
						}
					}
				}
				if (!bIgnoreThis) {
					const TCHAR * pCmd = Cmd(iter->second); //�����ų����⡣
					if (pCmd && *pCmd) {
						++iFound;
						tString strName = ItemName(iter->second);
						//�Ƴ�ÿ�γ��ֵĵ�һ�� &
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
						//�ض�;
						strName.resize(strName.size() - move);
						vStrName.push_back(strName);
					}
				}
			}
		}
		return iFound;
	}

	std::map<TSTRING, IDTYPE> m_NameIdMap;//���ڲ������ƺ�����Ķ�Ӧ��ϵ,ȫ��Сд��ĸ
	HIcon m_hIconOpen;
	HIcon m_hIconClose;
	HIcon m_hIconUnknowFile;

	IdStrMap m_ItemCmd;
	IdStrMap m_ItemParam;
	IdStrMap m_ItemIconPath;


	IDTYPE m_startID;
	IDTYPE m_ID;
	ComIniter m_comInited; //com �Ƿ��Ѿ���ʼ��

	//typedef const std::vector<TSTRING> wildcard_type;
	//std::vector<wildcard_type> m_Wildcard;//�ļ�ͨ�������
	std::vector<TSTRING> m_Wildcard;//�ļ�ͨ�������

	TSTRING m_strEmpty;
	std::map<MENUTYPE,int> m_StaticMenuWildcard;// ��̬Ŀ¼���ļ�ͨ�������
	std::map<MENUTYPE,int> m_DynamicMenuWildcard;// ��̬Ŀ¼���ļ�ͨ�������
	MenuStrMap m_StaticPath;//������� �� ��̬·��
	MenuStrMap m_DynamicPath;// ����ʱ��̬�����·��
	MenuStrMap m_ExpanedMenu;//������ģ����洢ͼ�ꡣ
	IDTYPE m_dynamicStartID;//��¼��̬�˵�����ʼ����������

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
