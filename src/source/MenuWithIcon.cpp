
#include <stdexcept>
#include <shellapi.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <algorithm>
#include <direct.h>
#include "resource.h"
#include "language.h"
#include "FileStrFnc.h"
//#include <boost/function.hpp>
#include "MenuWithIcon.h"

using namespace ns_findfile;
using namespace ns_file_str_ops;
//using ns_file_str_ops::ToLowerCase;

//! ����ָ�����ַ�(ch)�ָ������ַ���(inStr)�������ָ������(vStr). ���ַ���Ҳ��Ч��
unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	vStr.clear();
	TSTRING::size_type iStartPos = 0;
	TSTRING::size_type iEndPos = TSTRING::npos;
	/*
	iEndPos = inStr.find(ch, iStartPos);
	while (iEndPos != TSTRING::npos) {
		vStr.push_back(inStr.substr(iStartPos, iEndPos - iStartPos));
		iStartPos = iEndPos + 1;
		iEndPos = inStr.find(ch, iStartPos);
	}
	vStr.push_back(inStr.substr(iStartPos));
	/*/
	do {
		iEndPos = inStr.find(ch, iStartPos);
		vStr.push_back(inStr.substr(iStartPos, iEndPos - iStartPos));
		iStartPos = iEndPos + 1;
	} while (iEndPos != TSTRING::npos);

	//*/
	return vStr.size();
}


//�ж� null ���롣
unsigned int GetSeparatedString(const TSTRING::value_type * inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	if (!inStr) {
		vStr.clear();
		vStr.push_back(_T(""));
		return 1;
	}
	return GetSeparatedString(TSTRING(inStr), ch, vStr);

}

//! ���캯�� CMenuWithIcon
CMenuWithIcon::CMenuWithIcon(ICONTYPE hOpen,ICONTYPE hClose,ICONTYPE hUnknownFile,const TCHAR *szEmpty)
:COwnerDrawMenu(0,0),
m_hIconOpen(hOpen),m_hIconClose(hClose),m_hIconUnknowFile(hUnknownFile),
m_startID(0),m_ID(0),m_strEmpty (_T("Empty")),m_dynamicStartID(0),
m_bShowHidden(false),m_bFilterEmptySubMenus(true),m_menuData ( new CMenuData(_T("root")) ),
m_bOpenDynamicDir(true), m_bHasMyComputer(false)
{
	if (szEmpty && *szEmpty)
		m_strEmpty = szEmpty;
}


CMenuWithIcon::~CMenuWithIcon(void)
{
	Destroy();
}


//! ��ʾ
int CMenuWithIcon::Display(int x, int y, WINDOWTYPE hWnd, UINT uFlag)
{
	int id(Super::Display(x,y, hWnd, uFlag));
	if (!IsWindow(hWnd)) {
		TryProcessCommand(id);
		DestroyDynamic();
	}
	return id;
}

void CMenuWithIcon::DefaultIcons(ICONTYPE hOpen,ICONTYPE hClose,ICONTYPE hUnknown)
{
	m_hIconOpen = hOpen;
	m_hIconClose = hClose;
	m_hIconUnknowFile = hUnknown;
}

LRESULT CMenuWithIcon::MenuSelect_impl(MENUTYPE hMenu,UINT uItem,UINT uFlags)
{
	if((uFlags & MF_GRAYED) || (uFlags & MF_DISABLED)) {
		SelID((UINT)-1);
	} else if (uFlags & MF_POPUP) {
		SelID((UINT_PTR)GetSubMenu(hMenu,uItem));
		BuildDynamic(reinterpret_cast<MENUTYPE>(SelID()));
	} else {
		//�˵���
		SelID(uItem);
	}
	return 0;
}


//! ���Ʋ˵���
bool CMenuWithIcon::DrawItem_impl(DRAWITEMSTRUCT * pDI)
{
	if (!pDI || pDI->rcItem.bottom == pDI->rcItem.top) {
		return false;
	} else if (Super::DrawItem_impl(pDI) || !pDI->itemID) {
		return true;
	}

	//�������ܵ����ͣ��˵�����
	IDTYPE iMaybeID = pDI->itemID;
	MENUTYPE hMaybeMenu = TryGetSubMenu(pDI);
	assert(!hMaybeMenu || hMaybeMenu == (HMENU)iMaybeID);
	if (!hMaybeMenu && (iMaybeID < m_startID || iMaybeID >= m_ID) ) {
		hMaybeMenu = (HMENU)iMaybeID;
	}
	// ���˵� �� �Ӳ˵�

	if (hMaybeMenu ) {
		assert ( IsMenu(hMaybeMenu) );
		// �Ӳ˵�ͼ��
		ICONTYPE hIconSub = NULL;
		bool bDraw = false;//�ɹ����ƶ�̬�Ӳ˵�
		if (IsDynamicMenu(hMaybeMenu)) {
			TSTRING str(m_DynamicPath[hMaybeMenu]);
			str.resize(str.size()-2);
			SHFILEINFO sfi = {0};
			HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo(str.c_str(),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON
					);
			bDraw = (0 != hImgList);
			if (bDraw)
				ImageList_DrawEx(hImgList,sfi.iIcon,pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2,MENUICON,MENUICON,CLR_NONE,CLR_NONE,ILD_NORMAL);

		}
		if (!bDraw) {
			if (pDI->itemState & ODS_GRAYED || pDI->itemState & ODS_DISABLED)
				hIconSub = NULL;
			else if (!(pDI->itemState & ODS_SELECTED)) // û�д�
				hIconSub = m_hIconClose.Get();
			else
				hIconSub = m_hIconOpen.Get();
			if(hIconSub)
				DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,hIconSub,MENUICON,MENUICON, 0,NULL,DI_NORMAL);
		}
	}
	else if(!Cmd(iMaybeID)) {
		//����
		//DrawText(pDI->hDC,Name(iMaybeID),-1,&(pDI->rcItem),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		return true;
	}
	else if ( IsStaticMenu(MENUTYPE(pDI->hwndItem)) || IsDynamicMenu(MENUTYPE(pDI->hwndItem)) || IsExpanedMenu(MENUTYPE(pDI->hwndItem))) {
		//���Զ�̬�˵���ļ���
		SHFILEINFO sfi = {0};
		//memset(m_psfi.Get(),0,sizeof(SHFILEINFO));
		HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
				FILE_ATTRIBUTE_NORMAL,
				&sfi,
				sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON
				//���ļ���Ŀǰ֪�� .ico ͼ���ļ���ͼ����Ҫ���ļ���ȡ����������һЩ�ļ�	��ʧ��
				// | SHGFI_USEFILEATTRIBUTES //�����ļ���ȡͼ�꣬����  Ŀǰ����   ����ʧ��
				);
		if (!hImgList) {
			hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON
					| SHGFI_USEFILEATTRIBUTES //�����ļ���ȡͼ�꣬����  Ŀǰ����   ����ʧ��
					);

		}

		if (hImgList) {
			ImageList_DrawEx(hImgList,sfi.iIcon,pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2,MENUICON,MENUICON,CLR_NONE,CLR_NONE,ILD_NORMAL);
			//DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,sfi.hIcon,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);
		}
	}
	else
		//��ʾĬ��δ֪�ļ�ͼ��
		DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,m_hIconUnknowFile.Get(),MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);

	return true;
}



//! ��ȡ��ݷ�ʽ��.lnk�ļ���ͼ��
ICONTYPE CMenuWithIcon::GetShortCutIcon(LPCTSTR lpszLinkFile, bool bIcon32)
{
	if (!m_comInited && !m_comInited.Init()) {
		return 0;
	}
	ICONTYPE hReturn = NULL;//����ֵ

	IShellLink * psl;
	void *ppsl = &psl;

	// Get a pointer to the IShellLink interface.
	HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
							IID_IShellLink, static_cast<LPVOID*>(ppsl));
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;
		void *pppf = &ppf;

		// Get a pointer to the IPersistFile interface.
		hres = psl->QueryInterface(IID_IPersistFile, static_cast<LPVOID*>(pppf));

		if (SUCCEEDED(hres)) {
		#ifndef UNICODE
			wchar_t szLinkFile[MAXPATH] = {0};
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,lpszLinkFile,-1,szLinkFile,MAX_PATH);
			lpszLinkFile = &szLinkFile;
		#endif
			TCHAR szTgtPath[MAX_PATH];
			TCHAR szIconPath[MAX_PATH];
			int Iindex = 0;
			if (SUCCEEDED(ppf->Load(lpszLinkFile, STGM_READ)) &&
				SUCCEEDED(psl->Resolve(NULL,SLR_NO_UI | SLR_NOSEARCH | SLR_NOTRACK)) &&
				SUCCEEDED(psl->GetPath(szTgtPath, MAX_PATH, NULL, SLGP_SHORTPATH)) &&
				SUCCEEDED(psl->GetIconLocation(szIconPath, MAX_PATH, &Iindex)) )
			{
				const TCHAR * UsedPath = *szIconPath?szIconPath:szTgtPath;
				hReturn = GetIcon(UsedPath, FILEFOLDERICON, Iindex, bIcon32);
			}
			// Release the pointer to the IPersistFile interface.
			ppf->Release();
		}
		// Release the pointer to the IShellLink interface.
		psl->Release();
	}
	return hReturn;
}

//  */


//! ����Ӳ˵���strPath ���ڻ�ȡͼ��
bool CMenuWithIcon::AddSubMenu(MENUTYPE hMenu,MENUTYPE hSubMenu,const tString & strName, const tString & strPath, EICONGETTYPE needIcon)
{
	assert(strName.length());
	AddToMap(MenuNameMap(),hSubMenu, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_POPUP,(UINT_PTR)hSubMenu,NULL);//MenuName(hSubMenu));//

	if (needIcon) {
		if ( ! IsDynamicMenu(hSubMenu) ) {
			//��̬Ŀ¼���ļ��е�ǿ��Ŀ¼���������ͼ��
			MenuIcon(hSubMenu,GetIcon(strPath,needIcon));
		}
		else {
			SHFILEINFO sfi = {0};
			if (!SHGetFileInfo(strPath.c_str(),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON)) {

				//��ϵͳ���Ҳ����������ҵļ�¼��//note: it seems this never happen
				MenuIcon(hSubMenu, GetIcon(strPath,needIcon));
			}
		}
	}
	return true;
}


//! ���һ���˵���,����ȥ�� ǰ���հ�
int CMenuWithIcon::AddMenuItem(MENUTYPE hMenu, const tString & strName, const tString & inStrPath, EICONGETTYPE needIcon, const tString & strIcon)
{
	// ID����ʼλ�� CMDS;
	assert(strName.length());

	// ��һ�˵���
	AddToMap(ItemNameMap(),m_ID, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW,m_ID,NULL);//ItemName(m_ID));//

	if(inStrPath.empty()) {
		// �������
		EnableMenuItem(hMenu,m_ID,MF_BYCOMMAND | MF_DISABLED);
	}
	else {

		// �ж��Ƿ��������в�����
		// �ļ�·��������
		if (inStrPath[0] != '\"')
			AddToMap(m_ItemCmd, m_ID, inStrPath);
		else {
			//const int inLen = inStrPath.length();
			//Arr<TCHAR> strPath(new TCHAR [inLen + 1]);
			//memcpy(strPath.Get(),inStrPath.c_str(), sizeof(TCHAR)*(inLen + 1));
			const TCHAR *strBegin = inStrPath.c_str();
			const TCHAR *strCmd = strBegin;

			while(*(++strCmd) && *strCmd != '\"') ; //�����ڶ���˫����

			if(!*strCmd)
				AddToMap(m_ItemCmd, m_ID, inStrPath);//˫���Ų���ԣ�ԭ������
			else {
				//*strCmd = '\0';
				AddToMap(m_ItemCmd, m_ID, inStrPath.substr(1,strCmd - strBegin - 1));
				while(*++strCmd && _istspace(*strCmd)) ;
				if(*strCmd) {
					AddToMap(m_ItemParam, m_ID, inStrPath.substr(strCmd-strBegin));
				}
			}
		}

		if (needIcon != NOICON) {
			// �Ƕ�̬�˵��Ҫͼ��
			const TCHAR *szIconPath = strIcon.empty()?Cmd(m_ID):strIcon.c_str();
			if (!strIcon.empty())
				AddToMap(m_ItemIconPath, m_ID, strIcon);
			if ( ! IsStaticMenu(hMenu) && ! IsDynamicMenu(hMenu)) {
				ItemIcon(m_ID,GetIcon(szIconPath, needIcon));
			}
			else  {
				//��̬������ϵͳ�б��еĲŴ洢
				SHFILEINFO sfi = {0};
				if (!SHGetFileInfo(szIconPath,
						FILE_ATTRIBUTE_NORMAL,
						&sfi,
						sizeof(SHFILEINFO),
						SHGFI_SYSICONINDEX | SHGFI_SMALLICON
						| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0)
						)) {
					ItemIcon(m_ID,GetIcon(szIconPath,needIcon));
				}
			}
		}
	}

	++m_ID;
	return 1;
}


//! ���ͨ�����ʾ�Ĳ˵���
int CMenuWithIcon::MultiAddMenuItem(MENUTYPE hMenu, const tString & inStrPath,const tString & strName)
{
	//assert (strName.length());
	if(inStrPath.empty())
		return 0;
	TSTRING strPath(inStrPath);
	if(strPath.find('/') != TSTRING::npos) {
		assert (strPath == inStrPath);
		std::replace(strPath.begin(), strPath.end(), '/','\\');
	}
	const tString::size_type len = strPath.length();

	// ĩβ��ʽ����\*������\*.*������\**��,������ʽ������
	// ��ģʽ\*+* :һ����ֱ��չ�������ǲ�����ͼ��

	assert (*strPath.rbegin() == '*');
	EBUILDMODE mode = EFOLDER;
	if(IsStrEndWith(strPath, _T("\\**"), false)){
		strPath = strPath.substr(0, len - 1);
		mode = EEXPAND;
	}
	else if (IsStrEndWith(strPath, _T("\\*+*"), false)){
		mode = EEXPANDNOW;
		strPath = strPath.substr(0, len - 2);
	}
	else if(IsStrEndWith(strPath, _T("\\*.*"), false)){
		mode = EFILE;
		strPath = strPath.substr(0, len - 2);
	}

	if (EFOLDER == mode && strPath[len-2] != '\\') {
		return 0;
	}
	while (!strPath.empty() && _istspace(strPath[0]))
		strPath = strPath.substr(1);

	return MultiModeBuildMenu(hMenu,strPath,strName,mode);
}


//! public ��̬ʵʱ����ָ���Ӳ˵�������,
int CMenuWithIcon::BuildDynamic(MENUTYPE hSubMenu)
{
	int result(0);
	MENUTYPE hSel = (MENUTYPE)SelID();
	if(!IsMenu(hSel) || hSubMenu != hSel) {
		result = 0;
	}
	else if (m_StaticPath.find(hSel) != m_StaticPath.end() ||
			m_DynamicPath.find(hSel) != m_DynamicPath.end()) {
		result = DoBuildDynamic(hSel);
	}
	return result;
}


//! private ��̬ʵʱ����ָ���Ӳ˵�������,

//! \note ֻ�������Ӳ˵��Ͳ˵���Ӳ˵���չ����
//! ��Ҫչ���Ӳ˵����Ը��Ӳ˵����ñ�������
int CMenuWithIcon::DoBuildDynamic(MENUTYPE hMenu)
{
	// ��Ҫ��¼ÿ����̬���Ӳ˵��Ͳ˵��//����̬ ���١�

	//������Ч�˵����Ѿ�������ϵĲ˵���
	if (!IsMenu(hMenu) || GetMenuItemCount(hMenu) > 0)
		return 0;

	const TCHAR *strPath = NULL;
	if (IsStaticMenu(hMenu))
		strPath = GetStr(m_StaticPath,hMenu);
	else {
		assert (IsDynamicMenu(hMenu)) ;
		strPath = GetStr(m_DynamicPath,hMenu);
	}

	if(!strPath || !*strPath)
		return 0;

	const TCHAR *strName = NULL;
	if (IsStaticMenu(hMenu))
		strName = GetWildcard(m_StaticMenuWildcard[hMenu]);
	else {
		assert  (IsDynamicMenu(hMenu)) ;
		strName = GetWildcard(m_DynamicMenuWildcard[hMenu]);
	}

	while (*strPath && _istspace(*strPath)) ++strPath;//ȥ���հ�
	assert (strPath[_tcslen(strPath) - 1] =='*');

	UINT uPreErrMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	int r = MultiModeBuildMenu(hMenu,strPath,(strName && *strName)?strName:_T("*"),EDYNAMIC);
	SetErrorMode(uPreErrMode);

	return r;
}


//! ����˵����������Ӧ��������
bool CMenuWithIcon::TryProcessCommand(unsigned int nSysID)
{
	if (nSysID < m_startID || nSysID >= m_ID)
		return false;
	bool result (true);

	const TCHAR * pCmd = Cmd(nSysID);
	if (pCmd && *pCmd)
	{
		TSTRING strCmdLine(pCmd);
		strCmdLine = _T("\"") + strCmdLine + _T("\"");
		const TCHAR *pParam = Param(nSysID);
		if (pParam && *pParam)
		{
			strCmdLine += _T(" ");
			strCmdLine += pParam;
		}

		const TCHAR *pOpr = NULL;
		if ((GetKeyState(VK_CONTROL)&0x8000) && IsPathExe(pCmd))
		{
			OSVERSIONINFO osvi = {0};
			osvi.dwOSVersionInfoSize = sizeof(osvi);
			if (GetVersionEx(&osvi) && osvi.dwMajorVersion == 6)
			{
				pOpr = _T("runas");
			}
		}

		if (!ns_file_str_ops::Execute(strCmdLine, pOpr))
		{
			//ִ������ʧ��
			//EnableMenuItem(Menu(),nSysID,MF_BYCOMMAND | MF_GRAYED); // maybe a UAC problem, don't disable for now.
			MessageBox(NULL, Cmd(nSysID), _LNG(STR_Failed), MB_ICONERROR);
			result = false;
		}
	}
	return result;
}
/*
const TSTRING CMenuWithIcon::GetCurrentCommandLine(unsigned int nSysID)
{
	if (nSysID < m_startID || nSysID >= m_ID)
		return false;

	const TCHAR * pCmd = Cmd(nSysID);
	if (pCmd && *pCmd) {
		Arr<TCHAR> pEnough;
		TCHAR path[MAX_PATH] ={0};
		TSTRING strWorkPath;

		// ��ȡĿ¼��·��
		GetDirectory(nSysID,strWorkPath);
		if ('.' == *pCmd) {	//���·��
			TCHAR * FinalName;
			int length = GetFullPathName(pCmd,MAX_PATH,path,&FinalName);
			if (length > MAX_PATH) {
				pEnough = new TCHAR[length];
				if (GetFullPathName(pCmd,MAX_PATH,path,&FinalName) > 0)
					pCmd = pEnough.Get();
			}
			else if (length > 0) {
				pCmd = path;
			}
			if (pCmd && *pCmd && '.' != *pCmd) {
				strWorkPath = pCmd;
				strWorkPath = strWorkPath.substr(0,FinalName - pCmd);
			}
		}
	}
	return TSTRING(pCmd);
}
//*/

//! �Ӵ򿪵��ļ�����˵���
int CMenuWithIcon::LoadMenuFromFile(const tString & strFileName, UINT uStartID)
{
	Reset();
	if (!m_menuData.Get())
		return 0;
	m_menuData->Load(strFileName);

	m_startID = uStartID;
	m_ID = m_startID;

	UINT uPreErrMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	const int nStaticMenus = BuildMenuFromMenuData(m_menuData.Get(), Menu());
	SetErrorMode(uPreErrMode);

	m_dynamicStartID = m_ID;

	//���� <����,ID> ӳ��
	TSTRING strName;
	TSTRING::size_type size = 0;
	for (IDTYPE i = m_startID; i < m_dynamicStartID; ++i) {
		strName = ItemName(i);
		ToLowerCase(strName);

		//�Ƴ�ÿ�γ��ֵĵ�һ�� &
		size = strName.length();
		TSTRING::size_type move = 0;
		for (TSTRING::size_type j = 0; j < size; ++j) {
			if(strName[j] == '&') {
				++move;
				++j;
			}
			if(move)
				strName[j-move] = strName[j];
		}
		//�ض�;
		strName.resize(strName.size() - move);

		m_NameIdMap[strName] = i;
	}

	return nStaticMenus;
}

int CMenuWithIcon::BuildMenuFromMenuData(CMenuData * pMenu, MENUTYPE hMenu)
{
	int nItems(0);
	for (unsigned int index = 0; index < pMenu->Count(); ++index) {
		if (pMenu->IsMenu(index)) {
			MENUTYPE hSubMenu = CreateMenu();
			if (!hSubMenu)
				continue;
			nItems += BuildMenuFromMenuData(pMenu->Menu(index), hSubMenu);

			if ( GetMenuItemCount(hSubMenu) <= 0 && m_bFilterEmptySubMenus ) {
				DestroyMenu(hSubMenu);
			}
			else {
				AddSubMenu(hMenu,hSubMenu,pMenu->Item(index)->Name().c_str(), pMenu->Item(index)->Path());
				if (GetMenuItemCount(hSubMenu) <= 0) {
					assert (GetSubMenu(hMenu,GetMenuItemCount(hMenu)-1) == hSubMenu);
					EnableMenuItem(hMenu,GetMenuItemCount(hMenu)-1,MF_BYPOSITION | MF_GRAYED);
					//for(int i = 0; i < GetMenuItemCount(hMenu); ++i)
					//	if(GetSubMenu(hMenu,i) == hSubMenu) {
					//		EnableMenuItem(hMenu,i,MF_BYPOSITION | MF_GRAYED);
					//		break;
					//	}
				}
			}
		}
		else if ( pMenu->Item(index)->Path().length() > 1) {
			// ����ģʽ: \\** , ��ʾ�ҵĵ���
			if (pMenu->Item(index)->Path()[0] == '\\' && pMenu->Item(index)->Path() != _T("\\\\**"))
				continue;// filter begin with '\\' but not "\\**"

			// ����˵���
			const tString & strPath = pMenu->Item(index)->Path();
			const tString strSep(_T("|||"));

			if (!strPath.empty() &&
				*(strPath.rbegin())=='*') {// ƥ��ͨ���
				nItems += MultiAddMenuItem(hMenu,strPath,pMenu->Item(index)->Name());
			}
			else {
				tString::size_type sepPos = strPath.find(strSep) ;
				tString strIcon = tString::npos == sepPos? _T("") : ns_file_str_ops::StripSpaces ( strPath.substr(sepPos + strSep.length()) );
				if (!strIcon.empty()) {
					if ('\"' == strIcon[0]) {
						tString::size_type pos = strIcon.find('\"', 1);
						strIcon = strIcon.substr(1, pos == tString::npos ? pos : pos - 1);

					}
				}
				nItems += AddMenuItem( hMenu,
					pMenu->Item(index)->Name().empty() ? _T("< ??? >") : pMenu->Item(index)->Name() ,
					sepPos == tString::npos ? strPath : ns_file_str_ops::StripSpaces( strPath.substr(0,sepPos ) ),
					FILEFOLDERICON,
					strIcon
					);//ͳ�Ʋ˵�������
			}
		}
		else if ( ! (pMenu->Item(index)->Name().empty()) ) {
			//���ɱ���
			nItems += AddMenuItem(hMenu,pMenu->Item(index)->Name(),_T(""));
		}
		else {
			// empty, separator
			InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
		}

	}
	return nItems;
}


//! ��ȡ�ļ�Ŀ¼����ض������ַ���
bool CMenuWithIcon::GetDirectory(const IDTYPE nID, TSTRING & strWorkPath)
{
	strWorkPath = Cmd(nID);
	TSTRING::size_type pos = strWorkPath.find_last_of('\\');
	bool r = (strWorkPath.npos != pos);
	if (r) {
		strWorkPath.resize(pos);
	}
	return r;
}


//! ���ٶ�̬Ԫ��
void CMenuWithIcon::DestroyDynamic()
{
	if (m_ID == m_dynamicStartID)
		return;

	// 1 ���� ��̬�˵���
	for (IDTYPE i = m_dynamicStartID; i < m_ID; ++i) {
		//����������ƣ� //��̬�˵���û�б���ͼ�꣬��������ͼ��
		m_ItemCmd.erase(i);
		ItemNameMap().erase(i);
	}

	// 2 �����Ӳ˵���¼
	for (MenuStrIter it = m_DynamicPath.begin(); it != m_DynamicPath.end(); ++it) {
		// ����ͼ��
		if(MenuIcon(it->first)) {
			//note: it seems this never happen
			MenuIcon(it->first, 0);//��0ֵ��ʵ��ɾ����
		}
		// �������Ƽ�¼
		MenuNameMap().erase(it->first);
	}
	// ���ٶ�̬�˵�
	for (MenuStrIter it = m_StaticPath.begin(); it != m_StaticPath.end(); ++it) {
		const int n = GetMenuItemCount(it->first);
		for (int j = 0; j < n; ++j)
			DeleteMenu(it->first,0,MF_BYPOSITION);
	}

	// �����̬��ͨ�����¼
	m_DynamicMenuWildcard.clear();
	// �����̬�Ӳ˵���¼
	m_DynamicPath.clear(); //ClearMap(m_DynamicPath);

	// ���¼���
	m_ID = m_dynamicStartID;
}


//! ����һЩ����������Reset() �� �������� ����
void CMenuWithIcon::Destroy(void)
{
	m_ItemIconPath.clear();
	m_ItemCmd.clear();
	m_ItemParam.clear();
	m_StaticPath.clear();
	m_ExpanedMenu.clear();
	m_NameIdMap.clear();

	m_StaticMenuWildcard.clear();
	m_Wildcard.clear();

	m_ID = m_startID;
	m_dynamicStartID = m_ID;
}

namespace
{
;
bool ParseIconIndex(const tString &strPath, tString &path, int &index)
{
	const unsigned pos = strPath.find_last_of(',');
	if (pos == tString::npos)
	{
		return false;
	}
	index = -1;
	for (unsigned int i = pos + 1; i < strPath.size(); ++i)
	{
		if(index == -1)
		{
			index = 0;
		}
		if (isdigit(strPath[i]))
		{
			index *= 10;
			index += strPath[i] - '0';
		}
		else
		{
			return false;
		}
	}
	if (index < 0)
	{
		return false;
	}
	path = strPath.substr(0, pos);
	return true;
}

}


//! ����·����ȡ 16��16 �� 32��32 ͼ��
ICONTYPE CMenuWithIcon::GetIcon(const tString & strPath, EICONGETTYPE needIcon, int iconIndex, bool bIcon32)
{
	const TCHAR *pPath = strPath.c_str();//
	if(!*pPath || NOICON == needIcon)
		return NULL;
	// try to read icon index from strPath.
	if (isdigit(*strPath.rbegin()))
	{
		tString path;
		int index = -1;
		if (ParseIconIndex(strPath, path, index))
		{
			return GetIcon(path, needIcon, index, bIcon32);
		}
	}
	ICONTYPE hIcon = NULL;
/*
	// �·�ʽ������GetFileInfo ,������ͼ����������ܺ�ֱ�ӻ�������ͬ��
	// �Կ�ݷ�ʽ ICONTYPE �Ǵ��м�ͷ�ģ�index������û�м�ͷ��,
	// �������ϰ취ȡ ICONTYPE ��

	if (!m_comInited && !m_comInited.Init())
		return NULL;
	SHFILEINFO sfi = {0};
	SHGetFileInfo(pPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SMALLICON
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // �����Զ��ļ���ֻ�����ļ���չ����ȡ����ͼ�꣬�����ò����ļ��е���ȷͼ�ꡣ
		);//
	hIcon = sfi.hIcon;
	if (!hIcon && FILEFOLDERICON == needIcon)
			return GetIcon(pPath, FILEICON, iconIndex);

/*/
	if (bIcon32)
		ExtractIconEx(pPath,iconIndex, &hIcon,NULL,1); //�����ļ�����ͼ��
	else
		ExtractIconEx(pPath,iconIndex, NULL,&hIcon,1); //�����ļ�����ͼ��
	if(hIcon)
		return hIcon;

	if (IsStrEndWith(pPath, _T(".lnk"), false)) {
		// ��ݷ�ʽ,��Ҫ�ǳ���ȥ����ݷ�ʽ��ͷ,����(often .url files)ֱ���������SHGetFileInfo
		hIcon = GetShortCutIcon(pPath,bIcon32);
		return hIcon;
	}

	////�ļ��Ĺ���ͼ��,�ļ��е�ͼ��
	if (!m_comInited && !m_comInited.Init()) {
		return hIcon;
	}
	SHFILEINFO sfi = {0};
	SHGetFileInfo(pPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON
		| (bIcon32 ? SHGFI_LARGEICON : SHGFI_SMALLICON)
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // �����Զ��ļ���ֻ�����ļ���չ����ȡ����ͼ�꣬�����ò����ļ��е���ȷͼ�ꡣ
		);//
	hIcon = sfi.hIcon;

	if (!hIcon && strPath.find_first_of(_T(":\\")) == tString::npos) {

		TSTRING path;
		if (FindExe(pPath, path)) {
			if (bIcon32)
				ExtractIconEx(path.c_str(), 0, &hIcon,NULL,1);
			else
				ExtractIconEx(path.c_str(), 0, NULL,&hIcon,1);

		}
		// ���ڻ�û��ͼ�꣬���ܣ��ļ�����������
	}
	{
		// ����չ����������
		const int N = 512;
		std::vector<TCHAR> buf(N);
		if (ExpandEnvironmentStrings(strPath.c_str(), &buf[0], N) && strPath != &buf[0])
		{
			hIcon = GetIcon(&buf[0], needIcon, iconIndex, bIcon32);
		}
	}
// */
	return hIcon;

}

ICONTYPE CMenuWithIcon::GetBigIcon(const unsigned int id, int index){
	ICONTYPE result = 0;
	if (m_startID <= id && id < m_dynamicStartID) {
		if (m_ItemIconPath.find(id) != m_ItemIconPath.end())
			result = GetIcon(m_ItemIconPath[id],FILEFOLDERICON,index,true);
		else
			result = GetIcon(Cmd(id),FILEFOLDERICON,index,true);
	}
	return result;
}

//! ������Բ˵���
int CMenuWithIcon::Reset()
{
	Destroy();
	COwnerDrawMenu::Reset(); // 0
	(*m_menuData).Clear();
	HasMyComputer(false);
	return 0;
}


//! ���ͨ�����¼������������
int CMenuWithIcon::AddWildcard(const tString & str)
{
	if ( ! str.empty()) {
		int n = static_cast<int>(m_Wildcard.size());
		for (int i = 0; i < n; ++i)
			if (m_Wildcard[i] == TSTRING(str))
				return i;
		m_Wildcard.push_back(str);
		return static_cast<int>(m_Wildcard.size()) - 1;
	}
	return -1;
}

//! ��������������ͨ�����
const TCHAR * CMenuWithIcon::GetWildcard(int index)
{
	if (index < 0 || index >= static_cast<int>(m_Wildcard.size()))
		return NULL;
	return m_Wildcard[index].c_str();
}


int CMenuWithIcon::MultiModeBuildMenu(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & inStrName, EBUILDMODE mode,bool bNoFileIcon/* = false*/)
{
	tString strName(inStrName);
	if (strName.empty())
		strName = _T("*");
	std::vector<TSTRING> vStrFilter;
	GetSeparatedString(strName, '|', vStrFilter);
	std::sort(vStrFilter.begin(), vStrFilter.end());
	std::unique(vStrFilter.begin(), vStrFilter.end());
	return DoMultiModeBuildMenu(hMenu, inStrPathForSearch, strName, vStrFilter,mode, bNoFileIcon);
}

int CMenuWithIcon::BuildMyComputer(MENUTYPE hMenu, const tString & strName)
{
	int n = 0;
	unsigned long uDriveMask = GetLogicalDrives();
	assert(GetLogicalDrives() == _getdrives());

	if (uDriveMask) {
		tString strDrive(_T("A:\\"));

		// disable a: and b:
		uDriveMask >>= 2;
		strDrive[0] += 2;

		TCHAR szVolName[MAX_PATH+1] = {0};
		TCHAR szFSName[MAX_PATH+1] = {0};
		tString strMenuName;

		MENUTYPE hSubMenu = 0;
		while (uDriveMask) {
			if (uDriveMask & 1) {
				++n;
				hSubMenu = CreatePopupMenu();
				//��¼��̬�˵�

				//��һ����̬�˵�������Ҫ����
				AddToMap(m_StaticPath, hSubMenu, strDrive+_T("*"));//static ��ʾ��ɾ���� dynamic
				m_StaticMenuWildcard[hSubMenu] = AddWildcard(strName);

				tString strVolName;
				if (GetVolumeInformation(strDrive.c_str(), szVolName, MAX_PATH + 1, 0,0,0, szFSName, MAX_PATH + 1) && *szVolName) {
					strVolName = szVolName;
				}
				//else {
					SHFILEINFO sfi;
					SHGetFileInfo(strDrive.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_TYPENAME | SHGFI_DISPLAYNAME);
					strVolName = sfi.szDisplayName;
				//}
				DoubleChar(strVolName, '&');
				strMenuName = strVolName;

				// string replace
				TSTRING strOld(_T("(") + strDrive.substr(0,2) + _T(")"));
				TSTRING strNew(_T("(&") + strDrive.substr(0,2) + _T(")"));
				tString::size_type pos = strMenuName.find(strOld);
				if (pos != tString::npos) {
					strMenuName.replace(pos, strOld.size(), strNew);
				} else {
					strMenuName.append(strNew);
				}

				AddSubMenu(hMenu, hSubMenu, strMenuName, strDrive);
			}

			++strDrive[0];
			uDriveMask >>= 1;
		}
		HasMyComputer(true);
	}
	return n;
}


//! ͨ����˵���캯�������ڶ���ģʽ
//!\param mode ��file��folder��expand��expandnow��ģʽ
//!
int CMenuWithIcon::DoMultiModeBuildMenu(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon/* = false*/)
{
	assert(vStrFilter.size());
	if (_T("\\\\*") == inStrPathForSearch && EEXPAND == mode) {
		return BuildMyComputer(hMenu, strName);
	}

	const TCHAR * pSearch = inStrPathForSearch.c_str();//strPath �������һ��*��������������
	std::vector<TCHAR> path_expanded;
	if(inStrPathForSearch.find('%') != tString::npos)
	{
		path_expanded.resize(inStrPathForSearch.size() + MAX_PATH);
		ExpandEnvironmentStrings(pSearch, &path_expanded[0], path_expanded.size() - 1);
		pSearch = &path_expanded[0];
	}
	int result = 0;

	unsigned int nSubMenus = 0;

	if (EEXPAND == mode || EEXPANDNOW == mode || EDYNAMIC == mode) {

		// ���������Ŀ¼Ϊ�Ӳ˵���

		typedef std::vector<StrPair> NamePathContainer;

		NamePathContainer namePaths;

		if (m_bShowHidden) {
			nSubMenus = BuildNameMap(pSearch, namePaths, KeepDir());
		} else {
			nSubMenus = BuildNameMap(pSearch, namePaths, KeepNohiddenDir());
		}

		//�ļ�����Ϊ�˵���ʱ�����е� '&' ��չ��  '&&'
		for (NamePathContainer::iterator it = namePaths.begin(); it != namePaths.end(); ++it) {
			DoubleChar(it->first, '&');
		}

		// ���Դ�Сд
		sort(namePaths.begin(), namePaths.end(), NoCaseCmp1st() );	// for vector<>
		//namePaths.sort(NoCaseCmp1st()); // for list<>

		// �����������Ӳ˵�
		MENUTYPE hSubMenu = NULL;
		TSTRING strWildCardPath;
		bool bStaticMenu = IsStaticMenu(hMenu);
		switch (mode) {
			case EDYNAMIC:
				if (OpenDynamicDir()) {
					AddMenuItem(hMenu, szHiddenMenuItem, inStrPathForSearch.substr(0, inStrPathForSearch.length()-1), NOICON);
					SetMenuDefaultItem(hMenu, 0, TRUE);
					// empty, separator
					//InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
				}
				for (NamePathContainer::const_iterator it = namePaths.begin(); it != namePaths.end(); ++it) {
					hSubMenu = CreatePopupMenu();
					strWildCardPath = it->second + _T("\\*");
					//��¼��̬�˵�

					AddToMap(m_DynamicPath, hSubMenu, strWildCardPath);

					// ��¼ͨ���
					if (bStaticMenu)
						m_DynamicMenuWildcard[hSubMenu] = m_StaticMenuWildcard[hMenu];
					else {
						assert (IsDynamicMenu(hMenu));
						m_DynamicMenuWildcard[hSubMenu] = m_DynamicMenuWildcard[hMenu];
					}

					AddSubMenu(hMenu, hSubMenu, it->first, it->second,FILEFOLDERICON);

				}
				break;
			case EEXPANDNOW:
				for (NamePathContainer::const_iterator it = namePaths.begin(); it != namePaths.end(); ++it) {
					hSubMenu = CreatePopupMenu();
					strWildCardPath = it->second + _T("\\*");
					//��¼��̬�˵�
					DoMultiModeBuildMenu(hSubMenu,strWildCardPath.c_str(),strName, vStrFilter, EEXPANDNOW,true);

					if (GetMenuItemCount(hSubMenu) > 0) {
						AddSubMenu(hMenu,hSubMenu, it->first, it->second);
						AddToMap(m_ExpanedMenu, hSubMenu, strWildCardPath);
					}
					else {
						DestroyMenu(hSubMenu);
						hSubMenu = NULL;
					}
				}
				break;
			case EEXPAND:
				for (NamePathContainer::const_iterator it = namePaths.begin(); it != namePaths.end(); ++it) {
					hSubMenu = CreatePopupMenu();
					strWildCardPath = it->second + _T("\\*");
					//��¼��̬�˵�

					//��һ����̬�˵�������Ҫ����
					AddToMap(m_StaticPath, hSubMenu, strWildCardPath);//static ��ʾ��ɾ���� dynamic
					m_StaticMenuWildcard[hSubMenu] = AddWildcard(strName);

					AddSubMenu(hMenu, hSubMenu, it->first, it->second);
				}
				break;
			default:
				assert(false);
		}
	}

	// ��strNameΪ�������ҵ�ǰĿ¼�е��ļ�

	WIN32_FIND_DATA fd = {0};
	HANDLE handle = INVALID_HANDLE_VALUE;
	const TCHAR *f = fd.cFileName;
	// NBUF = 1024; �̳�
	TCHAR fullPath[NBUF] = {0};
	size_t len = _tcslen(pSearch)-1;

	memcpy(fullPath,pSearch, len * sizeof(TCHAR));
	fullPath[len] = '\0';	// len : ��Ŀ¼·��(������б��)����

	StrStrMap namePath;
	StrStrMap nameName;
	for (std::vector<TSTRING>::size_type i = 0; i < vStrFilter.size(); ++i) {

		if (vStrFilter[i].empty() && vStrFilter.size() > 1) {
			continue; // ����ж���������������հ�
		} else if ( vStrFilter[i].empty() ) {
			fullPath[len] = '*', fullPath[len+1] = '\0';
		} else {
			memcpy(fullPath + len,vStrFilter[i].c_str(),(1 + vStrFilter[i].length())*sizeof(TCHAR));
		}

		const TSTRING strSearch(fullPath);
		handle = FindFirstFile(strSearch.c_str(),&fd); // ϵͳ�Ỻ������������fullPath���ԸĶ���  Ans��Ӧ���ǵ�. ��Ҫð�գ��½�һ���ɡ�
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				if((f[0] == '.' && f[1] == '\0') || (f[0] == '.' && f[1] == '.' && f[2] == '\0') )
					continue;
				if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && EFOLDER != mode)
					continue;

				if (!m_bShowHidden && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					continue;

				////�ļ�����Ϊ�˵���ʱ�����е� '&' ��չ��  '&&'
				TSTRING strFileName(f);
				if (IsStrEndWith(strFileName, _T(".lnk"), false))
					strFileName.resize(strFileName.size() - 4);
				DoubleChar(strFileName, '&');

				// ������Դ�Сд
				TSTRING strNameLower(strFileName);
				ToLowerCase(strNameLower);
				// ���ʹ��ͨ�������ֹ�ظ��Ĳ��裬��Ȼ�µĻ��滻�ɵġ�
				if (nameName.find(strNameLower) == nameName.end()) {
					nameName[strNameLower] = strFileName;

					//�ļ�����Ŀ¼��������б�ߣ�������·����
					memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));
					namePath[strFileName] = fullPath;
				}

			}while(FindNextFile(handle,&fd));

			FindClose(handle);
		}
	}


	if ( ! nameName.empty()) {

		// �Զ�̬�˵�����ָ���
		if( EDYNAMIC == mode && nSubMenus > 0) {
			InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
		}

		// ���������Ĳ˵���Ŀ
		EICONGETTYPE iconGetType = FILEFOLDERICON;
		if (bNoFileIcon || EDYNAMIC == mode)
			iconGetType = NOICON;//��Ҫ�洢aͼ��
		StrStrMap::iterator itName;
		for (itName = nameName.begin(); itName != nameName.end(); ++itName) {
			result += AddMenuItem(hMenu,itName->second,namePath[itName->second],iconGetType);
		}

		namePath.clear();
		nameName.clear();
	}

	if (EDYNAMIC == mode && 0 == nSubMenus && 0 == result) {
		AddMenuItem(hMenu,m_strEmpty, _T(""));//��̬�˵�����ӱ��⡰�ա�
	}

	return result;
}


//! �ҳ���ȫƥ��,���������������ͼ��,�ɹ�����id��ʧ�ܷ��� 0��
unsigned int CMenuWithIcon::Find(const TSTRING & strName, TSTRING & strPath) const
{
	if(strName.empty() || strName.length() >= MAX_PATH)
		return 0;
	TSTRING strSearch(strName);
	ToLowerCase(strSearch);

	std::map<TSTRING,IDTYPE>::const_iterator
	//auto
	iter = m_NameIdMap.find(strSearch);
	if(iter != m_NameIdMap.end() && Cmd(iter->second)) {
		strPath = Cmd(iter->second);
		if(strPath.length() && strPath[0] != '\"')
			strPath = _T("\"") + strPath + _T("\"");
		if(Param(iter->second))
			strPath = strPath + _T(" ") + Param(iter->second);
		return iter->second;
	}
	return 0;
}

namespace {
	struct MatchBegin
	{
		TSTRING m_str;
		MatchBegin(const TSTRING &str):m_str(str){}
		bool operator()(const TSTRING &dst) {
			return dst.length() >= m_str.length() && dst.substr(0, m_str.length()) == m_str;
		}
	};
	struct IsSubOf
	{
		TSTRING m_str;
		IsSubOf(const TSTRING &str):m_str(str){}
		bool operator()(const TSTRING &dst) {
			return dst.length() >= m_str.length() && dst.find(m_str) != TSTRING::npos;
		}
	};
}

//! �ҳ�����ƥ�䣬���뵽ָ���ַ�������ĩβ��bNoDup = true �Ѵ��ڵ�������
unsigned int CMenuWithIcon::FindAll(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName, bool bAllowDup) const
{
	if(strBeginWith.empty() || strBeginWith.length()>=MAX_PATH)
		return 0;
	TSTRING strSearch(strBeginWith);
	ToLowerCase(strSearch);
	IsSubOf sub(strSearch);
	return FindIf(sub, vStrName, bAllowDup);
}

//! �ҳ�����ƥ�䣬���뵽ָ���ַ�������ĩβ��bNoDup = true �Ѵ��ڵ�������
unsigned int CMenuWithIcon::FindAllBeginWith(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName, bool bAllowDup) const
{
	if(strBeginWith.empty() || strBeginWith.length()>=MAX_PATH)
		return 0;
	TSTRING strSearch(strBeginWith);
	ToLowerCase(strSearch);
	MatchBegin match(strSearch);
	return FindIf(match, vStrName, bAllowDup);
}


//////////////////////////////////////////////////////////////////////////////
