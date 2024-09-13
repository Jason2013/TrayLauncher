#ifndef LCH_MENU_DATA_H
#define LCH_MENU_DATA_H

#include <auto_handle.h>

#include "FileStrFnc.h"

class CMenuData;
class CItem
{
	friend class CMenuData;
protected:
	typedef tString TS;
	typedef const TS CS;
	typedef const TS & CRTS;
	typedef TS & RTS;
	typedef CRTS Prm;
	typedef unsigned int Ui;
	inline static CS Empty() {return _T("");}
public:
	explicit CItem( Prm strName, Prm strPath = Empty(), Prm strEx = Empty() );

	virtual ~CItem() {}

	Prm Name() const { return Get(0); }
	Prm Path() const { return Get(1); }
	Prm Ex() const { return Get(2);}
	Prm Icon() const {return Ex().length()?Ex():Path();}

	TS & Name() { return Get(0); }
	TS & Path() { return Get(1); }
	Prm Name(Prm strName) { return Get(0) = strName; }
	Prm Path(Prm strPath) { return Get(1) = strPath; }
	Prm Ex(Prm strEx) { return Get(2) = strEx; }
	Prm Icon(Prm strEx) { return Get(2) = strEx; }

private:
	bool OutPut(FILE * pFile, wchar_t pad = '\t', int nPad = 0) const;
	Prm Get(Ui n) const { return m_str[n]; }
	TS & Get(Ui n) { return m_str[n]; }

private:
	std::vector<TS> m_str; // { name, path }
	//TS::value_type m_sep;// '='

};


class CMenuData : public CItem//, private ns_file_str_ops
{
public:
	using CItem::Name;
	using CItem::Path;

	explicit CMenuData( Prm strName, Prm strPath = Empty(), Prm strEx = Empty() );
	~CMenuData();
	Ui Count() const { return m_sub.size(); }

	const CMenuData * Menu(Ui pos) const { return dynamic_cast<const CMenuData*>(Item(pos)); }
	CMenuData * Menu(Ui pos) { return dynamic_cast<CMenuData*>(Item(pos)); }
	const CItem * Item(Ui pos) const {  return pos < m_sub.size() ? m_sub[pos] : 0; }
	CItem * Item(Ui pos) {  return pos < m_sub.size() ? m_sub[pos] : 0; }
	//CItem & Ref(Ui pos) {
	//	return *m_sub.at(pos);
	//}
	bool IsMenu(Ui pos) const { return Menu(pos) != 0; }

	void Clear();

	bool AddItem (Ui pos, Prm strName, Prm strPath, Prm strEx = Empty()) ;

	bool AddMenu(Ui pos, Prm strName, Prm strPath, Prm strEx = Empty());

	bool Remove(Ui pos) ;

	bool SaveAs(CRTS strFileName, TCHAR pad = '\t', int nPad = 0, int step = 1) const;

	int Load(CRTS strFileName);


private:
	//! 输出格式化内容到文件,递归应用于子菜单
	
	//! @param pFile 已经打开的文件
	//! @param pad 子菜单的前缀字符( 如用tab缩进)
	//! @param nPad 固定添加的前缀字符个数
	//! @param pFile 子菜单的缩进级数(pad的个数)
	bool OutPut(FILE * pFile, TCHAR pad, int nPad, int step) const;
	int LoadFile(FILE *pFile) ;
	std::vector<CItem*> m_sub;

	//{{ disabled features
	CMenuData & operator = (const CMenuData &);
	CMenuData(const CMenuData &);
	//}}
};

#endif
