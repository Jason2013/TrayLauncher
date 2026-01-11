#include "wx_pch.h"
#include "MenuItemData.h"

MenuItemData::MenuItemData(const wxString & name, const wxString & target, const wxString & icon, const wxString & workDir, bool hide):
	m_strName(name), m_strTarget(target), m_strIconPath(icon), m_strWorkDir(workDir), m_bHide(hide)
{
	//ctor
}

MenuItemData::MenuItemData(const MenuItemData & other):
	m_strName(other.m_strName), m_strTarget(other.m_strTarget), m_strIconPath(other.m_strIconPath), m_strWorkDir(other.m_strWorkDir), m_bHide(other.m_bHide)
{
	//copy ctor
}

MenuItemData::~MenuItemData()
{
	//dtor
}
