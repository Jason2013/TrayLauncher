
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "FileStrFnc.h"
#include "language.h"


///////////////////////////////////////////////////////////////////
//////////////
//////////////		Language 类
//////////////
///////////////////////////////////////////////////////////////////
class Language
{
	typedef int IdType;
	typedef std::wstring StringType;
	typedef StringType::value_type CharType;
	typedef std::map<IdType, StringType> IdStrMap;
	typedef std::map<StringType, IdType> StrIdMap;
public:
	Language(void):m_curId(e_idStart) {}
	template <unsigned int N> Language(const CharType * (&szArr)[N]):m_curId(e_idStart) { LoadArray(szArr); }

	unsigned int Size() const { return m_lng.size(); }
	// SetFilter(const StringType & vStr);

	void Clear() { m_lng.clear(); }

	const IdType GetLngId(const StringType & str, const bool bCreateIfNotFound = false)
	{
		IdType id = IdType();
		if (m_id.find(str) != m_id.end())
		{
			id = m_id[str];
		}
		else if (bCreateIfNotFound)
		{
			id = NewId();
			m_id[str] = id;
		}
		return id;
	}

	bool SetLngFile(const StringType & strFileName, const StringType & strSeparator = _T("==>"), const StringType & strLineComment = _T(";"));
/*
	const StringType &GetStr(const IdType & id) {
		IdStrMap::const_iterator pos = m_lng.find(id);
		if (pos != m_lng.end()) {
			return pos->second; 	// found
		}
		return g_strEmpty;	// not found
	}
//*/
	const CharType * GetCStr(const IdType id) {
		IdStrMap::const_iterator pos = m_lng.find(id);
		if (pos != m_lng.end()) {
			return pos->second.c_str(); 	// found
		}
		return GetEmptyString().c_str();	// not found
	}

	unsigned int LoadArray(const unsigned int N, const CharType **strArray, const bool bAddIdIfNotExist = false);

	template <unsigned int N>
	unsigned int LoadArray(const CharType * (&szArr)[N], const bool bAddIdIfNotExist = false) { return LoadArray(N, szArr, bAddIdIfNotExist); }

private:
	inline static const StringType & GetEmptyString()
	{
		static const StringType str;
		return str;
	}
	IdType NewId() { return ++m_curId; }
	IdStrMap m_lng;
	StrIdMap m_id;
	enum {e_idStart = 10000};
	IdType m_curId;

	// non-copyable
	Language(const Language &);
	Language & operator = (const Language &);
};


unsigned int Language::LoadArray(const unsigned int N, const CharType **strArray, const bool bAddIdIfNotExist) {
	Clear();
	const unsigned int n = N/2;
	for (unsigned int i = 0; i < n; ++i) {
		m_lng[GetLngId(strArray[i*2], bAddIdIfNotExist)] = strArray[i*2+1];
	}
	return Size();
}


bool Language::SetLngFile(const StringType & strFileName, const StringType & strSeparator, const StringType & strLineComment) {
	if (strSeparator.empty() ||  (!strLineComment.empty() && _istspace(strLineComment[0])) ) {
		return false;
	}

	file_ptr file(_wfopen(strFileName.c_str(), L"rb"));

	// check unicode le file
	if (!file || fgetwc(file) != 0xfeff) {
		return false;
	}

	const StringType strSpaceChars(L" \t\r\n");
	StringType strLine;
	while (ns_file_str_ops::GetLine(file, strLine)) {
		if (strLine.substr(0, strLineComment.length()) == strLineComment) {
			continue;
		}

		// analyze this line
		StringType::size_type pos = strLine.find(strSeparator);
		if (pos != StringType::npos) {
			// found
			StringType strSrc(strLine.substr(0, pos));
			ns_file_str_ops::StripCharsAtEnds(strSrc, strSpaceChars);

			StringType strDst(strLine.substr(pos + strSeparator.length()));
			ns_file_str_ops::StripCharsAtEnds(strDst, strSpaceChars);

			m_lng[GetLngId(strSrc)] = strDst;
		}
	}
	m_lng.erase(0); // delete the invalid one.
	return true;
}

//// end of Language class


// globla functions
Language & MainLng()
{
	static Language s_mainlng;
	return s_mainlng;
}

inline int MakeLngID(const TSTRING & str)
{
	return MainLng().GetLngId(str, true);
}
const TCHAR * GetLang(const int lngid)
{
	return MainLng().GetCStr(lngid);
}

const TCHAR * GetLang(const TCHAR * strSrc)
{
	return GetLang(MainLng().GetLngId(strSrc));
}


#define DECLARE_DEF_LANG_ID(id,str) const int LANG_ID_ ## id = MakeLngID(L ## #id);
#include "appstrings.h"
#undef DECLARE_DEF_LANG_ID


// default language : English
#define DECLARE_DEF_LANG_ID(id,str) (L ## #id), (str),
const TCHAR * g_strEnglishLngArray[] = {
	//英文界面 English Interface
	#include "appstrings.h"
	_T("")

};
#undef DECLARE_DEF_LANG_ID


bool SetLanguageFile(const TCHAR * szFileName)
{
	// first, reset to default.
	MainLng().LoadArray(g_strEnglishLngArray);

	if (!szFileName || !*szFileName) {
		return true;
	}
	TSTRING strFile(szFileName);
	if(!file_ptr(strFile.c_str(), _T("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}
	return MainLng().SetLngFile(strFile.c_str());
}

const TSTRING GetLngInfo(const TSTRING & strFileName, const TSTRING & strInfoKey)
{
	TSTRING strLngName; // return it

	TSTRING strFile(strFileName);
	if(!file_ptr(strFile.c_str(), _T("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}

	file_ptr file(_wfopen(strFile.c_str(), L"rb"));

	// check unicode le file
	if (!file || fgetwc(file) != 0xfeff) {
		return strLngName;
	}

	const TSTRING strSpaceChars(L" \t\r\n");
	const TSTRING strKey(L";" + strInfoKey);
	const TSTRING strSeparator(L":");
	TSTRING strLine;
	while (ns_file_str_ops::GetLine(file, strLine)) {
		// analyze this line
		TSTRING::size_type pos = strLine.find(strSeparator);
		if (pos != TSTRING::npos) {
			// found
			TSTRING strSrc(strLine.substr(0, pos));
			ns_file_str_ops::StripCharsAtEnds(strSrc, strSpaceChars);
			if (strSrc == strKey)
			{
				strLngName = strLine.substr(pos + strSeparator.length());
				ns_file_str_ops::StripCharsAtEnds(strLngName, strSpaceChars);
				break;
			}
		}
	}
	return strLngName;
}
