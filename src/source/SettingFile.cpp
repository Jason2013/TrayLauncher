
#include <assert.h>
#include <auto_pointers.h>
#include "SettingFile.h"


using namespace ns_file_str_ops;

CSettingFile::CSettingFile()
: m_bSettingChanged(false)
{
}


CSettingFile::CSettingFile(const TSTRING & strFileName, bool bCreateIfNotFound)
: m_bSettingChanged(false), m_strFileName(strFileName)
{
	if( !file_ptr(m_strFileName.c_str(), TEXT("rb"))){
		if (bCreateIfNotFound) {
			file_ptr newFile (m_strFileName.c_str(), TEXT("wb"));
			if (newFile) {
				_fputtc(0xfeff, newFile);
			}
		}
	}
	else {
		Read();
	}
}


CSettingFile::~CSettingFile(void)
{
	Save();
	Clear();
}


//! 保持设置文件
bool CSettingFile::Save()
{
	return m_bSettingChanged && SaveAs(m_strFileName);
}


//! 保持设置文件为指定文件。
bool CSettingFile::SaveAs(const TSTRING & strFileName)
{
	file_ptr file(strFileName.c_str(), TEXT("wb"));
	if (NULL == file) {
		return false;
	}
	m_strFileName = strFileName;

	_fputtc(0xfeff, file);
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if ( ! m_Settings[i].first.empty()) {
			WriteStringToFile(_T("[") + m_Settings[i].first + _T("]\r\n"), file);
		}
		for (StrStrMap::const_iterator iter = m_Settings[i].second.begin(); iter != m_Settings[i].second.end(); ++iter) {
			WriteStringToFile(iter->first + _T(" = ") + iter->second + _T("\r\n"), file);
		}

		WriteStringToFile(_T("\r\n"), file);
	}
	m_bSettingChanged = false;
	return true;
}


//! 清除所有设置
void CSettingFile::Clear()
{
	m_Settings.clear();
	m_Settings.push_back(Section());
}



//! 从文件读入设置
const bool CSettingFile::Read()
{
	Clear();

	file_ptr file(m_strFileName.c_str(), TEXT("rb"));
	if (NULL == file) {
		return false;
	}
	fseek(file, 0, SEEK_SET);
	int chUnicode = _fgettc(file);
	if (chUnicode != 0xfeff) {
		return false;
	}

	//初始化 m_Settings
	TSTRING strLine;
	TSTRING strItemKey;
	TSTRING strItemValue;
	while ( GetLine(file, strLine) ) {
		strLine = StripSpaces(strLine);
		if (strLine.empty()) {
			continue;
		}

		if ( '[' == strLine[0]) {
			// section
			TSTRING::size_type pos = strLine.find(']');
			if (pos != strLine.npos) {
				m_Settings.push_back(Section());
				m_Settings[m_Settings.size() - 1].first = StripSpaces(strLine.substr(1, pos - 1));
			}
		}
		else {
			// item
			TSTRING::size_type pos = strLine.find( '=' );
			if ( pos != strLine.npos && pos > 0 ) {
				strItemKey = StripSpaces(strLine.substr(0, pos));
				strItemValue = StripSpaces(strLine.substr(pos + 1));
				m_Settings[m_Settings.size() - 1].second[strItemKey] = strItemValue;
			}
		}
	}
	return true;
}


//! 获取 段落 内的所有的字段内容
int CSettingFile::GetSection(const TSTRING & strSection, std::vector<TSTRING> & vStrKey, std::vector<TSTRING> & vStrValue)
{
	vStrKey.clear();
	vStrValue.clear();
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			for (StrStrMap::const_iterator iter = m_Settings[i].second.begin(); iter != m_Settings[i].second.end(); ++iter ) {
				vStrKey.push_back(iter->first);
				vStrValue.push_back(iter->second);
			}
			return static_cast<int>(vStrKey.size());
		}
	}
	return 0;
}


//! 搜索匹配的所有字段的内容
int CSettingFile::TryGet(const TSTRING & strIndex, std::vector<TSTRING> & vStr)
{
	vStr.clear();
	StrStrMap::const_iterator iter;
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		iter = m_Settings[i].second.find(strIndex);
		if (iter != m_Settings[i].second.end()) {
			vStr.push_back(iter->second);
		}
	}
	return static_cast<int>(vStr.size());
}


//! 获取指定字段的内容
bool CSettingFile::Get(const TSTRING & strSection, const TSTRING & strIndex, TSTRING & str)
{
	str.clear();
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			StrStrMap::const_iterator iter = m_Settings[i].second.find(strIndex);
			if (iter != m_Settings[i].second.end()){
				str = iter->second;
				return true;
			}
			break;
		}
	}
	return false;
}


//! 添加段落
bool CSettingFile::AddSection(const TSTRING & strSection)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			return false;
		}
	}
	m_Settings.push_back(Section());//空白
	m_Settings[m_Settings.size() - 1].first = strSection;
	m_bSettingChanged = true;
	return true;
}


//! 清空段落内容
bool CSettingFile::ClearSection(const TSTRING & strSection)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			m_Settings[i].second.clear();
			m_bSettingChanged = true;
			return true;
		}
	}
	return false;
}


//! 设置指定段落中指定字段的值；
bool CSettingFile::Set(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str, bool bCreate)
{
	if (bCreate){
		return Add(strSection, strIndex, str);
	}
	else {
		for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
			if (m_Settings[i].first == strSection) {
				StrStrMap::iterator iter = m_Settings[i].second.find(strIndex);
				if (iter != m_Settings[i].second.end()){
					iter->second = str;
					m_bSettingChanged = true;
					return true;
				}
				break;
			}
		}
		return false;
	}
}


//! 在指定段落中添加字段；
const bool CSettingFile::Add(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			if (m_Settings[i].second[strIndex] != str) {
				m_Settings[i].second[strIndex] = str;
				m_bSettingChanged = true;
			}
			return true;
		}
	}
	return false;
}



//! 撤销上次保存以来的所有更改
bool CSettingFile::DisCard()
{
	return Read();
}


