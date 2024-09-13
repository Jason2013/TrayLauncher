#ifndef LCH_FILE_STR_FNC_H
#define LCH_FILE_STR_FNC_H

#include <stdheaders.h>
#include <windows.h>
// an collection of funtions for file and string/wstring


namespace ns_findfile {

	typedef std::pair<TSTRING, TSTRING> StrPair;
	//typedef boost::function<bool (const WIN32_FIND_DATA &)> Cond;

	class StrLowerCmper
	{
	public:
		bool operator() (const TSTRING &str1, const TSTRING &str2) const
		{
			return _tcsicmp(str1.c_str(), str2.c_str()) < 0;
		}
	};

	class NoCaseCmp1st
	{
	public:
		bool operator() (const StrPair & ss1, const StrPair & ss2) const
		{
			return StrLowerCmper()(ss1.first, ss2.first);
		}
	};

	class KeepAll {
	public:
		bool operator () (const WIN32_FIND_DATA &) const { return true; }
	};

	class KeepDir {
	public:
		bool operator () (const WIN32_FIND_DATA & fd) const
		{
			return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(fd.cFileName[0] == '.' && (fd.cFileName[1] == '\0' ||
												(fd.cFileName[1] == '.' && fd.cFileName[2] == '\0')));
		}
	};

	class KeepFile
	{
	public:
		bool operator () (const WIN32_FIND_DATA & fd) const
		{
			return !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}
	};

	class KeepNohiddenDir {
	public:
		bool operator () (const WIN32_FIND_DATA & fd) const
		{
			return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
					!(fd.cFileName[0] == '.' && (fd.cFileName[1] == '\0' ||
												(fd.cFileName[1] == '.' && fd.cFileName[2] == '\0')) );
		}
	};

	template <class Container, class Cond>
	unsigned int BuildNameMap(const TSTRING & strPath, Container &namePaths, Cond allow = KeepAll())
	{
		const TCHAR * pSearch = strPath.c_str();//strPath �������һ��*��������������
		assert(!strPath.empty() && *strPath.rbegin() == '*');
		WIN32_FIND_DATA fd = {0};
		const TCHAR *f = fd.cFileName;
		const int NBUF = 1024;
		TCHAR fullPath[NBUF] = {0};
		const size_t len = _tcslen(pSearch)-1;// len : ��Ŀ¼·��(������б��)����

		memcpy(fullPath,pSearch, len * sizeof(TCHAR));
		unsigned int n = 0;

		HANDLE handle = FindFirstFile(pSearch, &fd);
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				if(!allow(fd)) {
					continue;
				}

				//�ļ�����Ŀ¼��������б�ߣ�������·����
				memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));

				namePaths.push_back(StrPair(f, fullPath));
				++n;

			} while (FindNextFile(handle,&fd));
			FindClose(handle);
		}
		return n;
	}
}

namespace  ns_file_str_ops
{
	//! read a line, \r\n not included
	bool GetLine(FILE * file, TSTRING &strLine);
	//! read a line, \r\n included
	bool ReadLine(FILE *file, TSTRING & strLine);

	inline void WriteStringToFile(const TSTRING & str, FILE * file)
	{
		fwrite(str.c_str(), sizeof(TSTRING::value_type), str.length(), file);
	}
	const TSTRING StripSpaces(const TSTRING & str);
	bool StripCharsAtEnds(TSTRING & str, const TSTRING & chars);

	//! ת����Сд��ĸ��
	TSTRING & ToLowerCase(TSTRING &str);
	inline TSTRING LowerCase(const TSTRING &str) {
		TSTRING s(str);
		return ToLowerCase(s);
	}

	TSTRING & DoubleChar(TSTRING & str, const TSTRING::value_type ch);

	//! �ж��ַ����Ƿ��� �������ַ�����β��
	bool IsStrEndWith(const TSTRING & strSrc, const TSTRING & strMatchThis, bool bMatchCase = true);

	//! ���ݺ�׺���ж��ļ��Ƿ�Ϊ��ִ���ļ�.
	bool IsPathExe(const TSTRING & path);


	bool GetLastFileTime(const TCHAR * szFN, FILETIME *pSTCreate, FILETIME *pSTAccess,  FILETIME *pSTWrite);

	//! ����������Ͳ���
	void GetCmdAndParam(const TSTRING& const_strCmdParam, TSTRING& strCmd, TSTRING& strParam);


	inline bool ShellSuccess(const HINSTANCE hInst)
	{
		return reinterpret_cast<int>(hInst) > 32;
	}

	bool FindExe(const TSTRING &strCmd, TSTRING & strFullPath);

	//! ����������
	bool Execute(const TSTRING & strToBeExecuted, const TCHAR * pOpr = NULL, const bool bExpandEnv = true);

	//! ����������
	bool ExecuteEx(const TSTRING & strToBeExecuted, const TCHAR * pOpr = NULL, HWND hwnd = NULL, bool bExpandEnv = true);

};

#endif //LCH_FILE_STR_FNC_H
