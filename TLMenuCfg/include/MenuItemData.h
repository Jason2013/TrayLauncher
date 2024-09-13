#ifndef MENUITEMDATA_H
#define MENUITEMDATA_H

#include <wx/treectrl.h>


class MenuItemData : public wxTreeItemData
{
public:
	MenuItemData(const wxString & name, const wxString & target, const wxString & icon);
	virtual ~MenuItemData();

	const wxString Name() { return m_strName; }
	void Name(const wxString & val) { m_strName = val; }

	const wxString Target() { return m_strTarget; }
	void Target(const wxString & val) { m_strTarget = val; }

	const wxString IconPath() { return m_strIconPath; }
	void IconPath(const wxString & val) { m_strIconPath = val; }

protected:
private:
	wxString m_strName;
	wxString m_strTarget;
	wxString m_strIconPath;
};

#endif // MENUITEMDATA_H
