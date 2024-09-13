
#include "MenuData.h"
using namespace ns_file_str_ops;

CItem::CItem( Prm strName, Prm strPath, Prm strEx):m_str(3){
		m_str[0] = strName;
		m_str[1] = strPath;
		m_str[2] = strEx;
}

bool CItem::OutPut(FILE * pFile, TCHAR pad, int nPad) const{
	if (!pFile)
		return false;
	if (!(Name() == Empty() && Path() == Empty())) {
		WriteStringToFile(tString(nPad, pad) + Name() + _T(" = ") + Path(), pFile);
		if (!Ex().empty()) {
			WriteStringToFile(tString(3,'|') + Ex(), pFile);
		}
	}
	return true;
}



CMenuData::CMenuData( Prm strName, Prm strPath, Prm strEx)
:CItem(strName,strPath,strEx)
{}

CMenuData::~CMenuData() { Clear();}

void CMenuData::Clear() {
	for (Ui i = 0; i < m_sub.size(); ++i) {
		delete m_sub[i];
		m_sub[i] = 0;
	}
	m_sub.clear();
}

bool CMenuData::AddItem (Ui pos, Prm strName, Prm strPath, Prm strEx) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CItem * p = new CItem(strName, strPath, strEx);
		m_sub.insert(m_sub.begin() + pos, p);
		return true;
	}
	return false;
}

bool CMenuData::AddMenu(Ui pos, Prm strName, Prm strPath, Prm strEx) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CMenuData * p = new CMenuData(strName, strPath, strEx);
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
	wukong::file_ptr outfile(strFileName.c_str(), _T("wb"));
	bool r(false);
	if (outfile.Get()) {
		_fputtc(0xfeff, outfile.Get());
		r = OutPut(outfile, pad, nPad, step);
	}
	return r;
}

int CMenuData::Load(CRTS strFileName) {
	wukong::file_ptr file(strFileName.c_str(), _T("rb"));
	int r(0);
	if (file.Get() && _fgettc(file.Get()) == 0xfeff){
		Clear();
		r = LoadFile(file.Get());
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

				if (AddMenu(Count(), strName, strPath ) ) {
					assert(IsMenu(Count()-1));
					Menu(Count()-1)->LoadFile(pFile);
				}
				break;
			case '<' :
			case '}' :
				return nItems;
				//break;
			default:
				nItems += AddItem(Count(), strName, strPath);
				break;
		}
	}

	return nItems;
}

