
#include "FileStrFnc.h"
#include <shellapi.h>
#include <Shlwapi.h>

namespace ns_file_str_ops {

//!从文件读取一行。

//! \param file FILE *文件指针，输出。
//! \param strLine TSTRING 类型 保存结果。
//! \return 读到文件末尾返回false； 否则返回 true 。
bool GetLine(FILE * file, TSTRING &strLine)
{

#ifdef UNICODE
	wint_t ch(0);
#else
	int ch(0);
#endif
	strLine.clear();

	ch = _fgettc(file);

	while (ch != _TEOF && ch != '\r' && ch != '\n') {
		strLine += ch;
		ch = _fgettc(file);
	}
	if ('\r' == ch && (	ch = _fgettc(file) ) != '\n' && ch != _TEOF ) {
			_ungettc(ch, file);
	}
	return (ch != _TEOF);
}


bool ReadLine(FILE *file, TSTRING & strLine) {
	strLine.clear();
	wchar_t ch = fgetwc(file);
	while (ch != WEOF) {
		strLine += ch;

		if ('\r' == ch) {
			ch = fgetwc(file);
			if ('\n' == ch) {
				strLine += static_cast<TString::value_type>('\n');
			}
			else {
				ungetwc(ch, file);
			}
			break;
		}
		else if ('\n' == ch) {
			break;
		}
		ch = fgetwc(file);
	}
	return !strLine.empty();
};


TSTRING & ToLowerCase(TSTRING &str)
{
	TSTRING::size_type size = str.length();
	for (TSTRING::size_type i = 0; i < size; ++i) {
		str[i] = _totlower(str[i]);
	}
	return str;
}

//! 去掉字符串 首尾 的空白。
const TSTRING StripSpaces(const TSTRING & inStr)
{
	TSTRING::size_type iStart(0);
	TSTRING::size_type iEnd(inStr.size());
	while (iStart < iEnd && _istspace(inStr[iStart])) {
		++iStart;
	}
	while (iStart < iEnd && _istspace(inStr[iEnd - 1])){
		--iEnd;
	}
	return inStr.substr(iStart, iEnd - iStart);
}


bool StripCharsAtEnds(TSTRING & str, const TSTRING & chars)
{
	const TSTRING::size_type begin = str.find_first_not_of(chars);
	if (begin == TString::npos) {
		str.clear();
	}
	else {
		str.erase(0, begin);
		const TSTRING::size_type end = str.find_last_not_of(chars);
		assert (end != TString::npos);
		str.erase(end + 1);
	}
	return true;
}

TSTRING & DoubleChar(TSTRING & str, const TSTRING::value_type ch)
{
	TSTRING::size_type pos = 0;
	while((pos = str.find(ch,pos)) != str.npos) {
		str.insert(pos,1,ch);
		pos+=2;
	}
	return str;
}


bool IsStrEndWith(const TSTRING & strSrc, const TSTRING & strMatchThis, bool bMatchCase)
{
	if (strSrc.length() < strMatchThis.length())
		return false;
	TSTRING::size_type dif = strSrc.length() - strMatchThis.length();
	if (bMatchCase) {
		for (TSTRING::size_type i = 0; i < strMatchThis.length(); ++i) {
			if (strMatchThis[i] != strSrc[dif + i]) {
				return false;
			}
		}
	}
	else {
		for (TSTRING::size_type i = 0; i < strMatchThis.length(); ++i) {
			if (_totlower(strMatchThis[i]) != _totlower(strSrc[dif + i])) {
				return false;
			}
		}
	}
	return true;
}


bool GetLastFileTime(const TCHAR * szFN, FILETIME *pSTCreate, FILETIME *pSTAccess,  FILETIME *pSTWrite) {
	bool r = false;
	HANDLE hFile = CreateFile(szFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {
		r = !!GetFileTime(hFile, pSTCreate, pSTAccess, pSTWrite);
		CloseHandle(hFile);
	}
	return r;

}



void GetCmdAndParam(const TSTRING& const_strCmdParam, TSTRING& strCmd, TSTRING& strParam)
{
	TSTRING strCmdParam(const_strCmdParam);
	strCmd.clear();
	strParam.clear();

	//去掉引导空白
	if (strCmdParam.length() && (_istspace(*strCmdParam.begin()) || _istspace(*strCmdParam.rbegin()) )) {
		strCmdParam = ns_file_str_ops::StripSpaces(strCmdParam);
	}

	if( ! strCmdParam.empty()) {
		TSTRING::size_type pos;
		if(strCmdParam[0] == '\"') {
			if ((pos = strCmdParam.find('\"',1)) != TSTRING::npos) {
				strCmd = strCmdParam.substr(1,pos - 1);
				strParam = strCmdParam.substr(pos);
				strParam = strParam.substr(1);
			}
			else if (strCmdParam.length() > 1) {
				strCmd = strCmdParam.substr(1);
			}
		}
		else {
			pos = strCmdParam.find(' ');
			strCmd = strCmdParam.substr(0,pos);
			if(pos != TSTRING::npos) {
				strParam = strCmdParam.substr(pos);
			}
		}

		if (strCmd.length() && (_istspace(*strCmd.begin()) || _istspace(*strCmd.rbegin()) )) {
			strCmd = ns_file_str_ops::StripSpaces(strCmd);
		}
		if (strParam.length() && (_istspace(*strParam.begin()) || _istspace(*strParam.rbegin()) ) ){
			strParam = ns_file_str_ops::StripSpaces(strParam);
		}
	}
}


//! 根据后缀名判断文件是否为可执行文件.
bool IsPathExe(const TSTRING & path)
{

	return  IsStrEndWith(path,_T(".exe"),false) ||
			IsStrEndWith(path,_T(".cmd"),false) ||
			IsStrEndWith(path,_T(".bat"),false) ||
			IsStrEndWith(path,_T(".pif"),false) ||
			IsStrEndWith(path,_T(".scr"),false) ||
			IsStrEndWith(path,_T(".com"),false) ||
			IsStrEndWith(path,_T(".scf"),false);
}


bool FindExe(const TSTRING &strCmd, TSTRING & strFullPath)
{
	bool ret = false;
	TCHAR path[MAX_PATH] ={0};
	DWORD dwSize = MAX_PATH;
	HRESULT hres = AssocQueryString(ASSOCF_OPEN_BYEXENAME,
					 ASSOCSTR_EXECUTABLE,
					 strCmd.c_str(),
					 NULL,
					 path,
					 &dwSize);

	if ((S_OK == hres) || ((ShellSuccess(FindExecutable(strCmd.c_str(),NULL,path))) && *path)) {
		strFullPath = path;
		ret = true;
	}
	return ret;
}

bool ToFullPath(TSTRING & strFile)
{
	bool bRet = false;
	if (strFile.substr(0, 2) == _T(".\\") || strFile.substr(0, 3) == _T("..\\"))
	{
		TCHAR path[MAX_PATH] ={0};

		// 相对路径,获取目录的绝对路径
		int length = GetFullPathName(strFile.c_str(), MAX_PATH,path,NULL);
		if (length > MAX_PATH) {
			std::vector<TCHAR> vEnough(length);
			if (GetFullPathName(strFile.c_str(), length, &vEnough[0], NULL) > 0)
			{
				strFile = &vEnough[0];
				bRet = true;
			}
		}
		else if (length > 0) {
			strFile = path;
			bRet = true;
		}
		else
		{
			// some error happened
		}
	}
	return bRet;
}

//! 运行命令行
bool Execute(const TSTRING & strToBeExecuted, const TCHAR * pOpr, const bool bExpandEnv)
{
	return ExecuteEx(strToBeExecuted, pOpr, NULL, bExpandEnv);
}

//! 运行命令行
bool ExecuteEx(const TSTRING & strToBeExecuted, const TCHAR * pOpr, HWND hwnd, bool bExpandEnv)
{
	// 先展开环境变量
	const int N = 512;
	std::vector<TCHAR> buf(N);
	if (bExpandEnv && ExpandEnvironmentStrings(strToBeExecuted.c_str(), &buf[0], N) && TSTRING(strToBeExecuted) != &buf[0])
	{
		return ExecuteEx(&buf[0], pOpr, hwnd, false);
	}

	TSTRING strCmd,strParam;
	GetCmdAndParam(strToBeExecuted, strCmd, strParam);
	ToFullPath(strCmd);

	TSTRING strDir;
	const TSTRING::size_type posDirEnd = strCmd.find_last_of('\\');
	if (strCmd.npos != posDirEnd) {
		strDir = strCmd.substr(0, posDirEnd);
	}

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.hwnd = hwnd;
	sei.fMask = SEE_MASK_INVOKEIDLIST|SEE_MASK_FLAG_NO_UI;
	sei.lpVerb = pOpr;
	sei.lpFile = strCmd.c_str();
	if (IsPathExe(strCmd)) {
//		// 扩展 \"  - > \"\"\"
//		TSTRING::size_type pos = strParam.find('\"');
//		while (pos != strParam.npos) {
//			strParam.insert(strParam.begin() + pos, '\"');
//			strParam.insert(strParam.begin() + pos, '\"');
//			pos += 3;
//			pos = strParam.find('\"', pos);
//		}
		sei.lpParameters = strParam.c_str();
	}
	else
		sei.lpParameters = NULL;
	sei.lpDirectory = strDir.c_str();
	sei.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&sei);

	if (!ShellSuccess(sei.hInstApp))
	{
		const DWORD err = GetLastError();
		if(ERROR_FILE_NOT_FOUND == err)
		{
			TSTRING strPath;
			if (FindExe(strCmd, strPath))
			{
				strDir.clear();
				TSTRING::size_type pos = strPath.find_last_of('\\');
				if (TSTRING::npos != pos) {
					strDir = strPath.substr(0,pos);
				}
				sei.lpFile = strPath.c_str();
				sei.lpDirectory = strDir.c_str();
				ShellExecuteEx(&sei);
			}
		}
	}
	return ShellSuccess(sei.hInstApp);
}

}
