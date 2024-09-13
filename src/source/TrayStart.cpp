// TrayStart.cpp : ����Ӧ�ó������ڵ㡣
//


#ifdef USE_GDIPLUS
	#include <objbase.h>
	#include <gdiplus.h>
#endif
#include <shellapi.h>
#include "MsgMap.h"
#include "TrayStart.h"
#include "language.h"
#include "SettingFile.h"
//#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE & GhInstRef()
{
	static HINSTANCE g_hInst;
	return g_hInst;
}							// ��ǰʵ��
void ThisHinstSet(HINSTANCE hInst)
{
	GhInstRef() = hInst;
}
const HINSTANCE ThisHinstGet()
{
	return GhInstRef();
}

//TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
const TCHAR *szWindowClass = _T("TrayLauncher_WindowClass");			// ����������
//TCHAR g_lpCmdLine[MAX_PATH]={0};
HWND & GHdlgRun();	//���жԻ���


// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK ProcMessage(HWND, UINT, WPARAM, LPARAM);

CSettingFile & Settings();

//*
//! ExtraSettings: �����в�����������Ŀ

//! @return std::map< TSTRING, TSTRING >& :����������
//! @author lichao
//! @date 2009-10-13
//! @note һ�����ڵ���Ŀ��, �����û�ͨ��������.
std::map<TSTRING, TSTRING> & ExtraSettings()
{
	static std::map<TSTRING, TSTRING> setting;
	return setting;
}



//! ProcessArgSettings: ��ȡ�����в�������

//! @param argc : ��������
//! @param argv : �����ַ�������
//! @return void :
//! @author lichao
//! @date 2009-10-13
//! @note [��]
void ProcessArgSettings(int argc, TCHAR ** argv)
{
	TSTRING str;
	for (int i = 1; i < argc; ++i) {
		str = argv[i];
		int n = str.find('=');
		if (TSTRING::npos != static_cast<unsigned int>(n) && n > 2) {
			if (str.substr(0,2) == _T("--"))
			{
				ExtraSettings()[str.substr(2,n-2)] = str.substr(n+1);
			}
		}
	}
#ifdef _DEBUG
	TSTRING strall;
	for (std::map<TSTRING, TSTRING>::const_iterator it = ExtraSettings().begin(); it != ExtraSettings().end(); ++it)
	{
		strall += it->first;
		strall += _T(": ");
		strall += it->second;
		strall += '\n';
	}
#endif
}



//! ProcessArgs: ���������в���

//! @return bool :
//! @author lichao
//! @date 2009-10-13
//! @note [��]
bool ProcessArgs()
{
	LPWSTR *szArglist = 0;
	int nArgs = 0;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if( NULL == szArglist )
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return false;
	}

	ProcessArgSettings(nArgs, szArglist);

	LocalFree(szArglist);

	return true;

}
//*/

void InitLanguage()
{
	Settings().AddSection(sectionGeneral);
	TSTRING strLanguage;
	if (! Settings().Get(sectionGeneral, keyLanguage, strLanguage)) {
		strLanguage.clear();
		Settings().Set(sectionGeneral, keyLanguage, strLanguage,true);
	}
	SetLanguageFile(strLanguage.c_str());
}

void MustBeFirstInstance(const TCHAR * pTitle = NULL)
{
	HANDLE  hMutex;
	TCHAR Name[MAX_PATH] = TEXT("Lch_TestIfFirstInstance_");
	if (pTitle)
		memcpy(Name + _tcslen(Name),pTitle,_tcslen(pTitle) < MAX_PATH - 1 - _tcslen(Name) ?_tcslen(pTitle) : MAX_PATH - 1 - _tcslen(Name));
	hMutex = CreateMutex(
				NULL,			// No security descriptor
				FALSE,			// Mutex object not owned
				Name);			// Object name

	if (NULL == hMutex || ERROR_ALREADY_EXISTS == GetLastError())
	{
		HWND hPre = ::FindWindow(szWindowClass, ExtraSettings()[_T("sid")].c_str());
		if (hPre) {
			SetForegroundWindow(hPre);
			SendNotifyMessage(hPre,UM_NEWINSTANCE, 0,0);
			SetForegroundWindow(hPre);
		}
		else {
			MessageBox(NULL, _LNG(STR_Single_Instance_Only), pTitle, MB_ICONERROR);
		}
		exit(0);
	}
}
#ifdef USE_GDIPLUS
class GDIplusIniter{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR		   gdiplusToken;
public:
	GDIplusIniter() {
		// Initialize GDI+.
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}
	~GDIplusIniter() {
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
};
#endif
//! WinMain �������
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE /*hPrevInstance*/,
					 LPSTR	/*lpCmdLine*/,
					 int	   nCmdShow)
{
#ifdef USE_GDIPLUS
	const GDIplusIniter init;
#endif
	TCHAR path[MAX_PATH] = {0};
	int n = GetModuleFileName(NULL,path,MAX_PATH);
	while(n > 0 && path[n] != '\\') --n;
	path[n] = '\0';
	SetCurrentDirectory(path);

	ProcessArgs();

	InitLanguage();
// @fixme (lichao#1#): ���������д���
	if (ExtraSettings()[_T("single-instance")] != _T("false")) {
		TSTRING sid = ExtraSettings()[_T("sid")];
		MustBeFirstInstance(sid.empty() ? _T("Tray Launcher") : sid.c_str());
	}

	MSG msg;

	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	{
		// check to set priority
		TSTRING strValue;
		if (Settings().Get(sectionGeneral, keyPriority, strValue) && !strValue.empty() )
		{
			ns_file_str_ops::ToLowerCase(strValue);
			std::map<TSTRING, int> mapPriorityClass;
			mapPriorityClass[_T("idle")] = IDLE_PRIORITY_CLASS;
			mapPriorityClass[_T("below_normal")] = BELOW_NORMAL_PRIORITY_CLASS;
			mapPriorityClass[_T("normal")] = NORMAL_PRIORITY_CLASS;
			mapPriorityClass[_T("above_normal")] = ABOVE_NORMAL_PRIORITY_CLASS;
			mapPriorityClass[_T("high")] = HIGH_PRIORITY_CLASS;
			//mapPriorityClass[_T("realtime")] = REALTIME_PRIORITY_CLASS;

			if (mapPriorityClass.find(strValue) != mapPriorityClass.end())
			{
				SetPriorityClass(GetCurrentProcess(), mapPriorityClass[strValue]);
			}
		}
	}


	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRAYSTART));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if ((!GHdlgRun() || !IsDialogMessage(GHdlgRun(), &msg)) //&& !TranslateAccelerator(msg.hwnd, hAccelTable, &msg)
			) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//	����ϣ��
//	�˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//	����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//	����Ӧ�ó���Ϳ��Ի�ù�����
//	����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= ProcMessage;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYSTART));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


void InitMsgMap();


//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
   HWND hWnd;

   ThisHinstSet(hInstance); // ��ʵ������洢��ȫ�ֱ�����

   InitMsgMap();

   hWnd = CreateWindow(szWindowClass, ExtraSettings()[_T("sid")].c_str(), WS_OVERLAPPED,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
	  return FALSE;
   }
   return TRUE;
}
