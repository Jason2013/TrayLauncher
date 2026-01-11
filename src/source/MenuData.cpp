
#include "MenuData.h"
#include "xmlmenudata.h"

using namespace ns_file_str_ops;

CItem::CItem( Prm strName, Prm strPath, Prm strEx, Prm strWorkDir, bool hide):m_str(4), m_bHide(hide){
		m_str[0] = strName;
		m_str[1] = strPath;
		m_str[2] = strEx;
		m_str[3] = strWorkDir;
	}

bool CItem::OutPut(FILE * pFile, TCHAR pad, int nPad) const{
	if (!pFile)
		return false;
	if (!(Name() == Empty() && Path() == Empty())) {
		TSTRING strLine = tString(nPad, pad) + Name() + _T(" = ") + Path();
		// Always use fixed format: Path|||Icon|||WorkDir|||hide
		// Use empty strings for missing fields to ensure unambiguous parsing
		strLine += _T("|||") + Ex();  // Icon (may be empty)
		strLine += _T("|||") + WorkDir();  // WorkDir (may be empty)
		if (m_bHide) {
			strLine += _T("|||hide");
		}
		WriteStringToFile(strLine, pFile);
	}
	return true;
}



CMenuData::CMenuData( Prm strName, Prm strPath, Prm strEx, Prm strWorkDir, bool hide)
:CItem(strName,strPath,strEx,strWorkDir,hide)
{}

CMenuData::~CMenuData() { Clear();}

void CMenuData::Clear() {
	for (Ui i = 0; i < m_sub.size(); ++i) {
		delete m_sub[i];
		m_sub[i] = 0;
	}
	m_sub.clear();
}



bool CMenuData::AddItem (Ui pos, Prm strName, Prm strPath, Prm strEx, Prm strWorkDir, bool hide) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CItem * p = new CItem(strName, strPath, strEx, strWorkDir, hide);
		m_sub.insert(m_sub.begin() + pos, p);
		return true;
	}
	return false;
}

bool CMenuData::AddMenu(Ui pos, Prm strName, Prm strPath, Prm strEx, Prm strWorkDir, bool hide) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CMenuData * p = new CMenuData(strName, strPath, strEx, strWorkDir, hide);
		m_sub.insert(m_sub.begin() + pos, p);
		return true;
	}
	return false;
}


bool CMenuData::Remove(Ui pos) {
	const bool br (pos > 0 && pos < m_sub.size());
	if (br){
		delete m_sub[pos];
		m_sub.erase(m_sub.begin() + pos);
	}
	return br;
}

bool CMenuData::SaveAs(CRTS strFileName, TCHAR pad, int nPad, int step) const {
	bool r(false);
	if (IsStrEndWith(strFileName, _T(".xml"), false)) {
		XmlMenuData xmd;
		//todo  add encoding= "utf-8";
		MenuDataToXml(*this, xmd);
		r = xmd.SaveFile(strFileName);
	} else {
		wukong::file_ptr outfile(strFileName.c_str(), _T("wb"));
		if (outfile.Get()) {
			_fputtc(0xfeff, outfile.Get());
			r = OutPut(outfile, pad, nPad, step);
		}
	}
	return r;
}

int CMenuData::Load(CRTS strFileName) {
	int r(0);

	if (IsStrEndWith(strFileName, _T(".xml"), false)) {
		XmlMenuData xmd;
		xmd.LoadFile(strFileName);
		r = XmlToMenuData(xmd, *this);
	} else {
		wukong::file_ptr file(strFileName.c_str(), _T("rb"));
		if (file.Get() && _fgettc(file.Get()) == 0xfeff){
			Clear();
			r = LoadFile(file.Get());
		}
	}
	return r;
}

bool CMenuData::OutPut(FILE * pFile, TCHAR pad, int nPad, int step) const {
	if (!pFile)
		return false;
	for (Ui i = 0; i < m_sub.size(); ++i) {
		if (IsMenu(i)) {
			WriteStringToFile(tString(nPad, pad)+_T(">"), pFile);

			Item(i)->CItem::OutPut(pFile, '\t', 1);
			WriteStringToFile(_T("\r\n"), pFile);

			Menu(i)->OutPut(pFile, pad, nPad + step, 1);

			WriteStringToFile(tString(nPad, pad) + _T("<\r\n"), pFile);
		}
		else {
			Item(i)->CItem::OutPut(pFile, pad, nPad);
			_fputtc('\r', pFile);_fputtc('\n', pFile);
		}
	}
	return true;
}

namespace {

void SepPathIconAndWorkDir(TSTRING &strPath, TSTRING &strIcon, TSTRING &strWorkDir, bool &bHide)
{
	TSTRING strSep(_T("|||"));
	const TSTRING & strPathAndExtras = strPath;
	TSTRING::size_type sepPos1 = strPathAndExtras.find(strSep);
	bHide = false;

	if (TSTRING::npos != sepPos1)
	{
		// Fixed format: Path|||Icon|||WorkDir|||hide
		// Parse in fixed order
		strPath = ns_file_str_ops::StripSpaces( strPathAndExtras.substr(0, sepPos1) );
		TSTRING strRemaining = ns_file_str_ops::StripSpaces( strPathAndExtras.substr(sepPos1 + strSep.length()) );
		
		// Parse Icon (second field)
		TSTRING::size_type sepPos2 = strRemaining.find(strSep);
		if (TSTRING::npos != sepPos2)
		{
			strIcon = ns_file_str_ops::StripSpaces( strRemaining.substr(0, sepPos2) );
			TSTRING strRemaining2 = ns_file_str_ops::StripSpaces( strRemaining.substr(sepPos2 + strSep.length()) );
			
			// Parse WorkDir (third field)
			TSTRING::size_type sepPos3 = strRemaining2.find(strSep);
			if (TSTRING::npos != sepPos3)
			{
				strWorkDir = ns_file_str_ops::StripSpaces( strRemaining2.substr(0, sepPos3) );
				TSTRING strHide = ns_file_str_ops::StripSpaces( strRemaining2.substr(sepPos3 + strSep.length()) );
				bHide = (strHide == _T("hide") || strHide == _T("1") || strHide == _T("true"));
			}
			else
			{
				// No third separator - remaining is WorkDir (old format compatibility)
				strWorkDir = strRemaining2;
				bHide = false;
			}
		}
		else
		{
			// No second separator - remaining is Icon (old format compatibility)
			strIcon = strRemaining;
			strWorkDir = CItem::Empty();
			bHide = false;
		}

		// Remove quotes from icon path if present
		if (!strIcon.empty() && '\"' == strIcon[0]) {
			TSTRING::size_type pos = strIcon.find('\"', 1);
			strIcon = strIcon.substr(1, pos == TSTRING::npos ? pos : pos - 1);
		}
	}
	else
	{
		// No separators - just path
		strIcon = CItem::Empty();
		strWorkDir = CItem::Empty();
		bHide = false;
	}
}

}
int CMenuData::LoadFile(FILE *pFile) {

	assert(pFile);
	int nItems = 0;
	tString strLine;
	bool bEof = false;
	tString strName,strPath;
	while(!bEof) {
		bEof = !GetLine(pFile, strLine);
		strLine = StripSpaces(strLine);
		if (strLine.empty() && !bEof) {
			AddItem(Count(), Empty(), Empty() ); //  separater
			continue;
		}

		strLine = StripSpaces( strLine.substr( 0, strLine.find(';') ) );
		if (strLine.empty()) { // comments
			continue;
		}

		const tString::size_type pos = strLine.find('=',0);
		strName = StripSpaces( strLine.substr(0,pos) );
		strPath = (pos == tString::npos) ? Empty() : StripSpaces( strLine.substr(pos + 1) );

		switch (strLine[0]) {
			case '>' :
			case '{' :
				strName = StripSpaces(strName.substr(1));
				{
					TSTRING strIcon, strWorkDir;
					bool bHide = false;
					SepPathIconAndWorkDir(strPath, strIcon, strWorkDir, bHide);
					// AddMenu signature: (pos, name, path, icon, workdir, hide)
					// For submenus, path should be empty string
					if (AddMenu(Count(), strName, _T(""), strIcon, strWorkDir, bHide) ) {
						assert(IsMenu(Count()-1));
						Menu(Count()-1)->LoadFile(pFile);
					}
				}
				break;
			case '<' :
			case '}' :
				return nItems;
				//break;
			default:
				{
					TSTRING strIcon, strWorkDir;
					bool bHide = false;
					SepPathIconAndWorkDir(strPath, strIcon, strWorkDir, bHide);
					nItems += AddItem(Count(), strName, strPath, strIcon, strWorkDir, bHide);
				}
				break;
		}
	}

	return nItems;
}

