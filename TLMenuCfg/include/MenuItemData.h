#ifndef MENUITEMDATA_H
#define MENUITEMDATA_H

#include <wx/treectrl.h>


class MenuItemData : public wxTreeItemData
{
public:
	MenuItemData(const wxString & name, const wxString & target, const wxString & icon, const wxString & workDir = wxEmptyString, bool hide = false);
	MenuItemData(const MenuItemData & other);
	virtual ~MenuItemData();

	const wxString Name() { return m_strName; }
	void Name(const wxString & val) { m_strName = val; }

	const wxString Target() { return m_strTarget; }
	void Target(const wxString & val) { m_strTarget = val; }

	const wxString IconPath() { return m_strIconPath; }
	void IconPath(const wxString & val) { m_strIconPath = val; }

	const wxString WorkDir() { return m_strWorkDir; }
	void WorkDir(const wxString & val) { m_strWorkDir = val; }

	bool Hide() const { return m_bHide; }
	void Hide(bool val) { m_bHide = val; }

protected:
private:
	wxString m_strName;
	wxString m_strTarget;
	wxString m_strIconPath;
	wxString m_strWorkDir;
	bool m_bHide;
};

#endif // MENUITEMDATA_H
