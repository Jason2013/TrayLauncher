#ifndef TRAYLAUNCHER_RUNDLG_L
#define TRAYLAUNCHER_RUNDLG_L

// @todo (lichao#1#): change run dialog to a class.

icon_ptr & GRunIcon();
HWND & GHdlgRun();

const UINT UM_UPDATEHINT = WM_USER + 2;

HWND CreateRunDialog(HINSTANCE hInst);
bool GetRunPos(int &x, int &y);
bool SetRunPos(const int x, const int y);

using ns_file_str_ops::ShellSuccess;

class CToolTip
{
	CToolTip(HWND hOwner, HWND hTool);
	~CToolTip();

	HWND m_hwndToolTip;
};

//! 设置历史记录
void InitHistory(const std::vector<TSTRING>& vStrHis);

#endif // TRAYLAUNCHER_RUNDLG_L
