#include "MenuItemData.h"

MenuItemData::MenuItemData(const wxString & name, const wxString & target, const wxString & icon):
	m_strName(name), m_strTarget(target), m_strIconPath(icon)
{
	//ctor
}

MenuItemData::~MenuItemData()
{
	//dtor
}
