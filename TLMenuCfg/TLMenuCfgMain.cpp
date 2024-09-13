/***************************************************************
 * Name:      TLMenuCfgMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     (lichao)
 * Created:   2010-10-31
 * Copyright:  (lichao)
 * License: GPL 3.0
 **************************************************************/

#include "wx_pch.h"
#include "TLMenuCfgMain.h"
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <wx/mimetype.h>
#include <wx/dnd.h>
#include "MenuItemData.h"
#include "language.h"
#include "SettingFile.h"
#include <wx/cshelp.h>
#include <Shlwapi.h>
#include <deque>
#include "FileStrFnc.h"

//(*InternalHeaders(TLMenuCfgDialog)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#define STC_ASSERT(expr) { typedef int Arr[(expr)?1:-1]; }


std::map<wxString, wxString> & ExtraSettings();

CSettingFile & Settings()
{
	static CSettingFile settings(ExtraSettings()[_T("ini")].empty() ? _T("TL.ini") : static_cast<const TCHAR *>(ExtraSettings()[_T("ini")].c_str()));
	return settings;
}

void InitLanguage()
{
	TSTRING strLanguage;

	if (! Settings().Get(sectionGeneral, keyLanguage, strLanguage)) {
		//Settings().AddSection(sectionGeneral);
		//strLanguage.clear();
		//Settings().Set(sectionGeneral, keyLanguage, strLanguage,true);
	}

	SetLanguageFile(strLanguage.c_str());
}


//helper functions
enum wxbuildinfoformat {
	short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
	wxString wxbuild(wxVERSION_STRING);

	if (format == long_f )
	{
#if defined(__WXMSW__)
		wxbuild << _T("-Windows");
#elif defined(__UNIX__)
		wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
		wxbuild << _T("-Unicode build");
#else
		wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
	}

	return wxbuild;
}

//(*IdInit(TLMenuCfgDialog)
const long TLMenuCfgDialog::ID_STATICTEXT2 = wxNewId();
const long TLMenuCfgDialog::ID_SEARCHCTRL1 = wxNewId();
const long TLMenuCfgDialog::ID_TREECTRL_MENU = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON1 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON3 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON4 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON5 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON2 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX4 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX5 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT1 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX1 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL1 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON6 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT3 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX2 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL2 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT4 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL3 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPCOMBOBOX1 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON7 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON3 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON4 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON2 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON7 = wxNewId();
//*)

const long ID_SAVE_OR_APPLY = wxNewId();
const long ID_DELETE_ITEM = wxNewId();

BEGIN_EVENT_TABLE(TLMenuCfgDialog,wxDialog)
	//(*EventTable(TLMenuCfgDialog)
	//*)
END_EVENT_TABLE()

TLMenuCfgDialog::TLMenuCfgDialog(wxWindow* parent,wxWindowID id)
	:m_bInfoUnsaved(false),
	 m_bMenuChanged(false),
	 m_menuData(_T("root")),
	 m_iconlist(16, 16, true, 128),
	 m_indexUnknown(-1),
	 m_indexFolder(-1),
	 m_indexFolderOpen(-1),
	 m_indexSep(-1),
	 m_indexTitle(-1),
	 m_indexWildCard(-1),
	 m_fileName(_T("TLCmd.txt"))
{
	//(*Initialize(TLMenuCfgDialog)
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer15;
	wxBoxSizer* BoxSizer19;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer10;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer8;
	wxBoxSizer* BoxSizer13;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer11;
	wxBoxSizer* BoxSizer16;
	wxBoxSizer* BoxSizer18;
	wxBoxSizer* BoxSizer12;
	wxBoxSizer* BoxSizer14;
	wxBoxSizer* BoxSizer17;
	wxBoxSizer* BoxSizer9;
	wxBoxSizer* BoxSizer3;

	Create(parent, wxID_ANY, _("Tray Launcher Command Editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(693,416));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_stcMenu = new wxStaticText(this, ID_STATICTEXT2, _("\"Menu\""), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(m_stcMenu, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_search = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxSize(85,22), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	BoxSizer2->Add(m_search, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(BoxSizer2, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	m_TreeMenu = new wxTreeCtrl(this, ID_TREECTRL_MENU, wxDefaultPosition, wxSize(198,372), wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL_MENU"));
	BoxSizer8->Add(m_TreeMenu, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	m_btnUp = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_UP")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	BoxSizer5->Add(m_btnUp, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnDown = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_DOWN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
	m_btnDown->SetDefault();
	BoxSizer5->Add(m_btnDown, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnNewDir = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW_DIR")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
	m_btnNewDir->SetDefault();
	BoxSizer5->Add(m_btnNewDir, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnNewItem = new wxBitmapButton(this, ID_BITMAPBUTTON5, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
	m_btnNewItem->SetDefault();
	BoxSizer5->Add(m_btnNewItem, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(20,23,1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnDel = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DELETE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	m_btnDel->SetDefault();
	BoxSizer5->Add(m_btnDel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer8->Add(BoxSizer5, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	BoxSizer4->Add(BoxSizer8, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer4, 0, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	BoxSizer7 = new wxBoxSizer(wxVERTICAL);
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	m_flgMenu = new wxCheckBox(this, ID_CHECKBOX4, _("IsSubMenu"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	m_flgMenu->SetValue(false);
	m_flgMenu->Disable();
	BoxSizer9->Add(m_flgMenu, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgSep = new wxCheckBox(this, ID_CHECKBOX5, _("IsSeperater"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	m_flgSep->SetValue(false);
	m_flgSep->Disable();
	BoxSizer9->Add(m_flgSep, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer9, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11 = new wxBoxSizer(wxVERTICAL);
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	m_stcTarget = new wxStaticText(this, ID_STATICTEXT1, _("Target"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer10->Add(m_stcTarget, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgWildCard = new wxCheckBox(this, ID_CHECKBOX1, _("IsWildCard (*Mode)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_flgWildCard->SetValue(false);
	m_flgWildCard->Disable();
	BoxSizer10->Add(m_flgWildCard, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11->Add(BoxSizer10, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
	m_txtTarget = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxSize(456,24), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	m_txtTarget->SetMaxLength(512);
	BoxSizer17->Add(m_txtTarget, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnFindTarget = new wxBitmapButton(this, ID_BITMAPBUTTON6, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
	m_btnFindTarget->SetDefault();
	BoxSizer17->Add(m_btnFindTarget, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11->Add(BoxSizer17, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer11, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13 = new wxBoxSizer(wxVERTICAL);
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	m_stcNameFilter = new wxStaticText(this, ID_STATICTEXT3, _("Name / Filter"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer12->Add(m_stcNameFilter, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgTitle = new wxCheckBox(this, ID_CHECKBOX2, _("IsTitle"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	m_flgTitle->SetValue(false);
	m_flgTitle->Disable();
	BoxSizer12->Add(m_flgTitle, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13->Add(BoxSizer12, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
	m_txtNameOrFilter = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	m_txtNameOrFilter->SetMaxLength(128);
	BoxSizer18->Add(m_txtNameOrFilter, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13->Add(BoxSizer18, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer13, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15 = new wxBoxSizer(wxVERTICAL);
	BoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
	m_stcCustomizeIcon = new wxStaticText(this, ID_STATICTEXT4, _("Customize Icon"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer14->Add(m_stcCustomizeIcon, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15->Add(BoxSizer14, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
	m_txtIcon = new wxTextCtrl(this, ID_TEXTCTRL3, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	m_txtIcon->SetMaxLength(256);
	BoxSizer19->Add(m_txtIcon, 3, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_cbIcon = new wxBitmapComboBox(this, ID_BITMAPCOMBOBOX1, wxEmptyString, wxDefaultPosition, wxSize(47,23), 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_BITMAPCOMBOBOX1"));
	BoxSizer19->Add(m_cbIcon, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnFindIcon = new wxBitmapButton(this, ID_BITMAPBUTTON7, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON7"));
	BoxSizer19->Add(m_btnFindIcon, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15->Add(BoxSizer19, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer15, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
	m_btnSave = new wxButton(this, ID_BUTTON3, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer16->Add(m_btnSave, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnReload = new wxButton(this, ID_BUTTON4, _("Reload"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	BoxSizer16->Add(m_btnReload, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer16, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	m_btnClose = new wxButton(this, ID_BUTTON2, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer6->Add(m_btnClose, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	m_btnApply = new wxButton(this, ID_BUTTON7, _("Apply"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
	m_btnApply->Disable();
	BoxSizer6->Add(m_btnApply, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer6, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5);
	BoxSizer3->Add(BoxSizer7, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	BoxSizer1->Add(BoxSizer3, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OnSearchTextChange);
	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&TLMenuCfgDialog::Onm_searchTextEnter);
	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_BEGIN_DRAG,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuBeginDrag);
	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_END_DRAG,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuEndDrag);
	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuSelectionChanged);
	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_SEL_CHANGING,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuSelChanging);
	Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnUpClick);
	Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnDownClick);
	Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnNewDirClick);
	Connect(ID_BITMAPBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnNewItemClick);
	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnDelClick);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtTargetText);
	Connect(ID_BITMAPBUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnFindTargetClick);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtNameOrFilterText);
	Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtIconText);
	Connect(ID_BITMAPCOMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&TLMenuCfgDialog::Onm_cbIconSelected);
	Connect(ID_BITMAPBUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnBitmapButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnSaveClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnReloadClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnQuit);
	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnApplyClick);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&TLMenuCfgDialog::OnInit);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TLMenuCfgDialog::OnClose);
	//*)

	GetSizer()->Layout();

	InitLanguage();

	Connect(ID_SAVE_OR_APPLY, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&TLMenuCfgDialog::OnHotKey);
	Connect(ID_DELETE_ITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&TLMenuCfgDialog::OnHotKey);

	TSTRING strFileName;
	if (Settings().Get(sectionGeneral, keyCommand, strFileName) && !strFileName.empty())
	{
		m_fileName = strFileName;
	}

	// update language
	SetTitle(_LNG(STR_DlgTitle) + (_T("  -  [ ") + m_fileName + _T(" ]")) );
	m_stcMenu->SetLabel(_LNG(STC_Menu));
	m_btnClose->SetLabel(_LNG(BTN_Close));
	m_btnApply->SetLabel(_LNG(BTN_Apply));
	m_btnSave->SetLabel(_LNG(BTN_Save));
	m_btnReload->SetLabel(_LNG(BTN_Reload));

	m_flgMenu->SetLabel(_LNG(BTN_IsMenu));
	m_flgSep->SetLabel(_LNG(BTN_IsSep));
	m_flgTitle->SetLabel(_LNG(BTN_IsTitle));
	m_flgWildCard->SetLabel(_LNG(BTN_IsWildCard));

	m_stcTarget->SetLabel(_LNG(STC_Target));
	m_stcNameFilter->SetLabel(_LNG(STC_DispName));
	m_stcCustomizeIcon->SetLabel(_LNG(STC_IconPath));

	m_txtNameOrFilter->SetHelpText(_LNG(STR_Invalid_NameOrFilter));

	// use menu for hotkeys like Ctrl-S, not displayed.
	const int acc [] =
	{
		// flg1, key1, cmd1,
		// flg2, key2, cmd2,
		// ...

		//wxACCEL_NORMAL, WXK_DELETE, ID_DELETE_ITEM,
		wxACCEL_CTRL, 'S', ID_SAVE_OR_APPLY
	};
	STC_ASSERT((sizeof(acc) / sizeof(acc[0])) % 3 == 0);
	const int NItems = sizeof(acc) / sizeof(acc[0]) / 3;

	if (NItems > 0)
	{
		wxAcceleratorEntry entries[NItems];

		for (int i = 0; i < NItems; ++i)
		{
			const int index = 3*i;
			entries[0].Set(acc[index], acc[index + 1], acc[index + 2]);
		}

		wxAcceleratorTable accel(NItems, entries);
		this->SetAcceleratorTable(accel);
	}
}

TLMenuCfgDialog::~TLMenuCfgDialog()
{
	//(*Destroy(TLMenuCfgDialog)
	//*)
}

void TLMenuCfgDialog::OnQuit(wxCommandEvent& event)
{
	Close();
}

void TLMenuCfgDialog::OnAbout(wxCommandEvent& event)
{
	wxString msg = wxbuildinfo(long_f);
	wxMessageBox(msg, _("Welcome to..."));
}


namespace {

class MyFileDropTarget : public wxFileDropTarget
{
public:
	MyFileDropTarget(wxTextCtrl *pEdit, const wxString & pre = _T(""), const wxString &post = _T(""))
	:m_pEdit(pEdit),
	m_prefix(pre),
	m_postfix(post) {}

private:
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filenames)
	{
		if (m_pEdit && filenames.Count())
		{
			m_pEdit->SetValue(m_prefix + filenames[0] + m_postfix);
			return true;
		}

		return false;
	}
	wxTextCtrl *m_pEdit;
	wxString m_prefix;
	wxString m_postfix;
};

//! \brief read name, path, and icon path from CItem
//!
//! \param mi const CItem& : the menu item to read from
//! \param strName TSTRING& : save name
//! \param strPath TSTRING& : save path
//! \param strIcon TSTRING& : sava icon path
//! \return void
//! Called only in this file.
//!
void GetMenuStrings(const CItem &mi, TSTRING &strName, TSTRING &strPath, TSTRING &strIcon)
{
	TSTRING strSep(_T("|||"));
	strName = mi.Name();
	strPath = mi.Path();
	strIcon = mi.Ex();
}

const wxString ExpandEnvString(const wxString & path)
{
	wxString str;
	const int N = 512;
	std::vector<TCHAR> buf(N+1);
	if (ExpandEnvironmentStrings(path.c_str(), &buf[0], N))
	{
		str = &buf[0];
	}
	return str;
}


bool GetIconSize(HICON hIcon, int &w, int &h)
{
	bool r = false;
	ICONINFO iconInfo = {0};
	BITMAP bitmap = {0};
	if (GetIconInfo(hIcon, &iconInfo))
	{
		if(GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bitmap))
		{
			w = bitmap.bmWidth;
			h = bitmap.bmHeight;
		}
		DeleteObject(iconInfo.hbmColor);
		DeleteObject(iconInfo.hbmMask);
		r = true;
	}
	return r;
}

wxIcon WxExtractIcon(const wxString &path, const int index, const bool smallIcon = false)
{
	HICON hIcon = 0;
	if (smallIcon)
	{
		ExtractIconEx(path.c_str(), index, 0, &hIcon, 1);
	}
	else
	{
		ExtractIconEx(path.c_str(), index, &hIcon, 0, 1);
	}

	int w = 0;
	int h = 0;
	wxIcon icon;

	if (hIcon && GetIconSize(hIcon, w, h))
	{
		icon.SetHICON(hIcon);
		icon.SetSize(w, h);
	}
	return icon;
}

bool ExtractAllIcons(const wxString &path, std::deque<wxIcon> &icons, bool smallIcon = false)
{
	int const num_icon = reinterpret_cast<int>(ExtractIcon(wxGetInstance(), path.c_str(), -1));

	for (int i = 0; i < num_icon; ++i)
	{
		wxIcon icon(WxExtractIcon(path, i, smallIcon));
		if (!icon.Ok())
		{
			break;
		}
		icons.push_back(icon);
	}
	return !icons.empty();
}

unsigned int FindIconIndexSepCharPos(const wxString &path)
{
	unsigned pos = path.find_last_of(',');
	if(pos == wxString::npos)
	{
		return pos;
	}
	for (unsigned int i = pos + 1; i < path.size(); ++i)
	{
		if((path[i]) < '0' || path[i] > '9')
		{
			return wxString::npos;
		}
	}
	return pos;
}


wxIcon ResizeIcon(const wxIcon &icon, int w, int h)
{
	wxIcon ret;
	if (icon.Ok())
	{
		wxImage img = wxBitmap(icon).ConvertToImage();
		ret.CopyFromBitmap(wxBitmap(img.Scale(w, h)));
	}
	return ret;
}


wxIcon GetFileIcon(const wxString & path, const int moreTry = 1, const int width = -1, const int height = -1)
{
	// disable loadicon error msg;
	wxLogNull logNo;

	wxIcon icon(path, wxBITMAP_TYPE_ICO, width, height);

	if (!icon.Ok())
	{
		// try to extract index
		unsigned sep = FindIconIndexSepCharPos(path);
		long index = 0;
		if(sep != wxString::npos && path.substr(sep + 1).ToLong(&index) && index > 0)
		{
			icon = WxExtractIcon(path.substr(0, sep).Strip(), index, width == 16 && height == 16);
			if (width > 0 && height > 0 && (width != 16 || height != 16))
			{
				icon = ResizeIcon(icon, width, height);
			}
		}
	}

	if (!icon.IsOk())
	{
		wxFileName fn(path);

		if (fn.IsOk())
		{
			// use wxFileType
			wxFileType *p = 0;

			if (wxDirExists(path))
			{
				icon.LoadFile(_T("explorer.exe"), wxBITMAP_TYPE_ICO, width, height);
			}
			else if (wxFileExists(path))
			{
				if (!fn.GetExt().empty())
				{
					p = wxTheMimeTypesManager->GetFileTypeFromExtension(fn.GetExt());
				}
			}

			if (p)
			{
				wxIconLocation il;
				p->GetIcon(&il);
				icon = wxIcon(il);

				if (!icon.IsOk())
				{
					wxString cmdline(p->GetOpenCommand(path));

					if (!cmdline.empty())
					{
						TSTRING cmd, param;
						ns_file_str_ops::GetCmdAndParam(static_cast<const TCHAR*>(cmdline.c_str()), cmd, param);
						icon.LoadFile(cmd, wxBITMAP_TYPE_ICO, width, height);
					}
				}

				delete p;
			}

		}
	}

	// try expand evirenment variables
	if (!icon.IsOk() && path.find('%') != wxString::npos)
	{
		wxString expath(ExpandEnvString(path));
		if (!expath.empty() && expath != path)
		{
			icon = GetFileIcon(expath, moreTry, width, height);
		}
	}

	if (!icon.IsOk())
	{
		TSTRING strPath;
		if (ns_file_str_ops::FindExe(static_cast<const TCHAR*>(path.c_str()), strPath)){
			icon.LoadFile(strPath, wxBITMAP_TYPE_ICO, width, height);
		}
	}

	if (!icon.IsOk() && moreTry > 0)
	{
		//try get the executable
		TSTRING cmd, param;
		ns_file_str_ops::GetCmdAndParam(static_cast<const TCHAR*>(path.c_str()), cmd, param);

		if (path != cmd)
		{
			icon = GetFileIcon(cmd, moreTry - 1, width, height);
		}
	}

	return icon;
}

} // end of namespace

//! \brief transfer menu item info (name,path and icon) to tree item data.
//!
//! \param mi const CItem&, menu item to read
//! \param tree wxTreeCtrl&, ref of the tree
//! \param id wxTreeItemId, id of the tree item.
//! \return void
//! this function deal with a menu item(leaf), not a sub-menu.
//!
void TLMenuCfgDialog::MenuDataToTree(const CItem &mi, wxTreeCtrl &tree, wxTreeItemId id)
{
	assert(id.IsOk());
	TSTRING strName, strPath, strIcon;
	GetMenuStrings(mi, strName, strPath, strIcon);

	tree.SetItemData(id, new MenuItemData(strName, strPath, strIcon));
	UpdateItemDisplay(tree, id);
}

//! \brief transfer sub-menu info (name,path and icon) to tree item data.
//!
//! \param mi const CMenuData&, submenu to read
//! \param tree wxTreeCtrl&, ref of the tree
//! \param id wxTreeItemId, id of the tree item
//! \return void
//! this function will create tree items for children of mi recursively.
//!
void TLMenuCfgDialog::MenuDataToTree(const CMenuData &mi, wxTreeCtrl &tree, wxTreeItemId id)
{
	assert(id.IsOk());

	tree.SetItemData(id, new MenuItemData(mi.Name(), _T(""), mi.Icon()));

	for (unsigned int i = 0; i < mi.Count(); ++i)
	{
		wxTreeItemId idsub(tree.AppendItem(id, mi.Item(i)->Name()));

		if (mi.IsMenu(i)) { MenuDataToTree(*mi.Menu(i), tree, idsub); }
		else { MenuDataToTree(*mi.Item(i), tree, idsub); }
	}

	UpdateItemDisplay(tree, id);

}

wxTreeItemId TLMenuCfgDialog::InsertItem(wxTreeCtrl &tree, const wxTreeItemId & item, bool before, const TSTRING &strName)
{
	wxTreeItemId id;
	assert(!id.IsOk());

	if (item.IsOk())
	{
		if (wxTreeItemId parent = tree.GetItemParent(item))
		{
			wxTreeItemId pos = before ? tree.GetPrevSibling(item) : item;

			if (pos)
			{
				id = tree.InsertItem(parent, pos, strName);
			}
			else
			{
				id = tree.PrependItem(parent, strName);
			}
		}
	}

	return id;
}

wxTreeItemId TLMenuCfgDialog::MoveItem(wxTreeCtrl &tree, wxTreeItemId from, wxTreeItemId to, const bool before)
{
	wxTreeItemId no;

	if(!from.IsOk() || !to.IsOk() || from == to || !tree.GetItemParent(to).IsOk())
	{
		return no;
	}

	if ((before && tree.GetPrevSibling(to) == from) ||
	        (!before && tree.GetPrevSibling(from) == to))
	{
		return no;
	}

	bool bNeedFreezeAndUnfreeze = !tree.IsFrozen();

	if (bNeedFreezeAndUnfreeze)
	{
		tree.Freeze(); //disable screen update
	}

	wxTreeItemId item = CopyItem(tree, from, to, before);

	if(item.IsOk())
	{
		// clear old data;
		tree.Delete(from);

		//MenuChgFlg(true);
	}

	if (bNeedFreezeAndUnfreeze)
	{
		tree.Thaw();//enable screen update
	}

	return item;
}

wxTreeItemId TLMenuCfgDialog::CopyItem(wxTreeCtrl &tree, wxTreeItemId from, wxTreeItemId to, const bool before)
{
	wxTreeItemId item;

	if(!from.IsOk() || !to.IsOk() || !tree.GetItemParent(to).IsOk())
	{
		return item;
	}

	wxTreeItemId upItem = to;

	while (upItem.IsOk())
	{
		if (upItem == m_dragSrc)
		{
			return item;
		}

		upItem = m_TreeMenu->GetItemParent(upItem);
	}


	const bool bFromExpanded = tree.IsExpanded(from);

	const bool bNeedFreezeAndUnfreeze = !tree.IsFrozen();

	if (bNeedFreezeAndUnfreeze)
	{
		tree.Freeze(); //disable screen update
	}

	item = InsertItem(tree, to, before);

	if (!item.IsOk())
	{
		if (bNeedFreezeAndUnfreeze)
		{
			tree.Thaw();//enable screen update
		}

		return item;
	}

	MenuChgFlg(true);

	//tree.SetItemText(id, tree.GetItemText(from));

	if (MenuItemData *pData = static_cast<MenuItemData *>(tree.GetItemData(from)))
	{
		tree.SetItemData(item, new MenuItemData(*pData));
		tree.SetItemImage(item, tree.GetItemImage(from));
		tree.SetItemText(item, tree.GetItemText(from));
	}

	//UpdateItemDisplay(tree, item);//, false);

	if (tree.HasChildren(from))
	{
		tree.SetItemImage(item, tree.GetItemImage(from, wxTreeItemIcon_Expanded), wxTreeItemIcon_Expanded);

		wxTreeItemId tmpItem = tree.AppendItem(item, _T(""));
		wxTreeItemIdValue cookie = &item;

		for (wxTreeItemId id = tree.GetFirstChild(from, cookie); id.IsOk(); id = tree.GetNextChild(from, cookie))
		{
			CopyItem(tree, id, tmpItem, true);
		}

		tree.Delete(tmpItem);

		if (bFromExpanded)
		{
			tree.Expand(item);
		}
		else
		{
			tree.Collapse(item);
		}
	}

	if (bNeedFreezeAndUnfreeze)
	{
		tree.Thaw();//enable screen update
	}

	return item;
}


void TLMenuCfgDialog::OnInit(wxInitDialogEvent& event)
{
	int icon_h = 16;
	int icon_w = 16;
	m_iconlist.GetSize(0, icon_w, icon_h);

	if(icon_w > icon_h)
	{
		icon_w = icon_h;
	}
	else
	{
		icon_h = icon_w;
	}

	SetIcon(wxICON(IDI_APP_TLMC));
	m_TreeMenu->SetImageList(&m_iconlist);

	m_indexFolder = m_iconlist.Add(wxICON(IDI_FOLDER));
	m_indexFolderOpen = m_iconlist.Add(wxICON(IDI_FOLDER_OPEN));
	m_indexUnknown = m_iconlist.Add(wxICON(IDI_UNKNOWN));

	TSTRING strSkin;

	if(Settings().Get(sectionGeneral, keySkin, strSkin) && !strSkin.empty()) {
		// disable loadicon error msg;
		wxLogNull logNo;

		strSkin = _T(".\\skin\\") + strSkin + _T("\\icons\\");
		wxIcon icon;

		if (icon.LoadFile(strSkin + _T("close.ico"), wxBITMAP_TYPE_ICO, icon_w, icon_h))
		{
			m_indexFolder = m_iconlist.Add(icon);
		}

		if (icon.LoadFile(strSkin + _T("open.ico"), wxBITMAP_TYPE_ICO, icon_w, icon_h))
		{
			m_indexFolderOpen = m_iconlist.Add(icon);
		}

		if (icon.LoadFile(strSkin + _T("unknown.ico"), wxBITMAP_TYPE_ICO, icon_w, icon_h))
		{
			m_indexUnknown = m_iconlist.Add(icon);
		}
	}


	m_menuData.Load(m_fileName);

	wxTreeItemId idRoot = m_TreeMenu->AddRoot(m_menuData.Name());

	MenuDataToTree(m_menuData, *m_TreeMenu, idRoot);

	// if empty, add a demo item;
	if (!m_TreeMenu->HasChildren(idRoot))
	{
		wxTreeItemId demoItem = m_TreeMenu->AppendItem(idRoot, _T(""));
		m_TreeMenu->SetItemData(demoItem, new MenuItemData(_LNG(STR_DisplayName), _LNG(STR_PathToTarget), _T("")));
		UpdateItemDisplay(*m_TreeMenu, demoItem);
	}

	m_TreeMenu->SelectItem(m_TreeMenu->GetFirstVisibleItem());

	m_txtTarget->SetDropTarget(new MyFileDropTarget(m_txtTarget, _T("\""), _T("\"")));
	m_txtIcon->SetDropTarget(new MyFileDropTarget(m_txtIcon));
}

void TLMenuCfgDialog::InfoChgFlg(const bool val)
{
	if (val != m_bInfoUnsaved)
	{
		m_bInfoUnsaved = val;

		if (!val)
		{
			m_bTargetChanged = false;
			m_bNameFilterChanged = false;
			m_bIconChanged = false;
			wxFont font(m_txtTarget->GetFont());
			font.SetWeight(wxNORMAL);

			m_txtTarget->SetFont(font);
			m_txtNameOrFilter->SetFont(font);
			m_txtIcon->SetFont(font);
		}
	}
}

void TLMenuCfgDialog::MenuChgFlg(const bool val)
{
	if (val != MenuChgFlg())
	{
		m_bMenuChanged = val;

		m_btnApply->Enable(val);

		if (val)
		{
			SetTitle(_T(" * ") + GetTitle());
		}
		else
		{
			wxString title(GetTitle());
			assert(title.Left(3) == _T(" * "));
			SetTitle(title.substr(3));
		}
	}
}


//! \brief update the display of tree item according to its item data
//!
//! \param tree wxTreeCtrl&, the tree ctrl to update
//! \param item wxTreeItemId, the item to update
//! \return void
//!
//!
void TLMenuCfgDialog::UpdateItemDisplay(wxTreeCtrl &tree, wxTreeItemId item, const bool refreshImage)
{
	assert(item.IsOk());
	MenuItemData * itemData(static_cast<MenuItemData*>(tree.GetItemData(item)));
	assert(itemData && dynamic_cast<MenuItemData*>(tree.GetItemData(item)));

	TSTRING strDisplay(itemData->Name());

	if (strDisplay.empty())
	{
		strDisplay = _T("< ??? >");

		if (itemData->Target().empty() && itemData->IconPath().empty())
		{
			strDisplay = _T("----------------");
		}
		else if(itemData->Target() == _T("\\\\**"))
		{
			strDisplay = _LNG(STR_MyComputer);
		}
		else if(itemData->Target().Right(1) == _T("*"))
		{
			strDisplay = _T("[* * *]");
		}
	}

	tree.SetItemText(item, strDisplay);

	const int noImage = -1;

	if (tree.GetImageList() && refreshImage)
	{
		if (itemData->Target().Right(1) == _T("*"))
		{
			m_TreeMenu->SetItemImage(item, m_indexWildCard);
			return;
		}
		int icon_w = 16;
		int icon_h = 16;
		tree.GetImageList()->GetSize(0, icon_w, icon_h);
		if(icon_w > icon_h)
		{
			icon_w = icon_h;
		}
		else
		{
			icon_h = icon_w;
		}
		// Target may have parameters,
		// more try only for target, NOT for iconpath.
		wxIcon icon(GetFileIcon(itemData->IconPath().empty() ? itemData->Target() : itemData->IconPath(), itemData->IconPath().empty(), icon_w, icon_h));

		const int oldIndex = tree.GetItemImage(item);

		if (icon.IsOk())
		{
			if (oldIndex == noImage ||
			        oldIndex == m_indexFolder ||
			        oldIndex == m_indexFolderOpen ||
			        oldIndex == m_indexUnknown ||
			        oldIndex == m_indexSep ||
			        oldIndex == m_indexTitle ||
			        oldIndex == m_indexWildCard)
			{
				tree.SetItemImage(item, tree.GetImageList()->Add(icon));
			}
			else
			{
				tree.GetImageList()->Replace(oldIndex, icon);
			}
		}
		else if (tree.HasChildren(item))
		{
			// just leave the previous image alone if any;
			// as deleting affects all following images's indices.

			tree.SetItemImage(item, m_indexFolder);
		}
		else
		{
			// No icon for separator and title
			if (itemData->IconPath().empty() && itemData->Target().empty() )
			{
				tree.SetItemImage(item, itemData->Name().empty() ? m_indexSep : m_indexTitle);
			}
			else
			{
				tree.SetItemImage(item, m_indexUnknown);
			}
		}

		// for sub-tree, add an icon for expanded state
		if (tree.HasChildren(item))
		{
			tree.SetItemImage(item, m_indexFolderOpen, wxTreeItemIcon_Expanded);
		}
	}
}


void TLMenuCfgDialog::OnTreeMenuSelectionChanged(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if (item.IsOk())
	{
		assert(item == m_TreeMenu->GetSelection());
		// update detail info
		ReadItemInfo();

		UpdateFlgs();
	}
}

void TLMenuCfgDialog::OnbtnSaveClick(wxCommandEvent& event)
{
	if(InfoChgFlg())
	{
		SaveItemInfo();
	}
}

void TLMenuCfgDialog::OnTreeMenuSelChanging(wxTreeEvent& event)
{
	if (InfoChgFlg() && event.GetOldItem().IsOk())
	{
		if (!AllowChangeSel())
		{
			event.Veto();
		}
	}
}

void TLMenuCfgDialog::CheckFlg(wxCheckBox* ctrl, const bool val)
{
	assert(ctrl);

	if (ctrl->GetValue() != val)
	{
		ctrl->SetValue(val);
		wxFont font(ctrl->GetFont());
		font.SetWeight(val ? wxBOLD : wxNORMAL);
		ctrl->SetFont(font);
	}
}


bool TLMenuCfgDialog::SetNameFromTarget()
{
	bool ret = false;
	if (!m_txtNameOrFilter->IsModified())
	{
		TSTRING strCmd, strParam;
		ns_file_str_ops::GetCmdAndParam(static_cast<const TCHAR*>(m_txtTarget->GetValue().c_str()), strCmd, strParam);

		strCmd = strCmd.substr(strCmd.find_last_of('\\') + 1);
		if (ns_file_str_ops::IsPathExe(strCmd))
		{
			strCmd.resize(strCmd.length()-4);
		}
		m_txtNameOrFilter->SetValue(strCmd);
		ret = true;
	}
	return ret;
}


void TLMenuCfgDialog::OntxtTargetText(wxCommandEvent& event)
{
	m_bTargetChanged = true;
	InfoChgFlg(true);

	wxFont font(m_txtTarget->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtTarget->SetFont(font);
	}

	UpdateFlgs();
}

void TLMenuCfgDialog::OntxtNameOrFilterText(wxCommandEvent& event)
{
	wxString strForbid(_T("<>="));
	wxString str(event.GetString());

	const wxString::size_type pos = str.find_first_of(strForbid);
	if (pos != wxString::npos)
	{
		for (unsigned int i = 0; i < strForbid.length(); ++i)
		{
			str.Replace(strForbid.Mid(i, 1), _T(""));
		}
		m_txtNameOrFilter->ChangeValue(str);
		m_txtNameOrFilter->SetInsertionPoint(pos);
		wxHelpProvider::Get()->ShowHelp(m_txtNameOrFilter);


		//wxMessageBox(_LNG(STR_Invalid_NameOrFilter), _T("TrayLauncher"));
		return;
	}

	m_bNameFilterChanged = true;
	InfoChgFlg(true);

	wxFont font(m_txtNameOrFilter->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtNameOrFilter->SetFont(font);
	}

	UpdateFlgs();

}

void TLMenuCfgDialog::SetIconPathModifiedFlag()
{
	m_bIconChanged = true;
	InfoChgFlg(true);

	wxFont font(m_txtIcon->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtIcon->SetFont(font);
	}

	UpdateFlgs();
}



void TLMenuCfgDialog::TryExtractIcons()
{
	m_cbIcon->Freeze();

	wxString path_and_index(m_txtIcon->GetValue());
	if (!path_and_index.empty())
	{
		TString str(path_and_index);
		if(ns_file_str_ops::StripCharsAtEnds(str, _T("\" ")))
		{
			path_and_index = str;
		}
	}
	unsigned sep = FindIconIndexSepCharPos(path_and_index);
	wxString file_path = path_and_index.substr(0, sep).Strip();

	if (m_iconExtractPathOld != file_path)
	{
		m_iconExtractPathOld = file_path;

		m_cbIcon->Clear();
		std::deque<wxIcon> icons;
		ExtractAllIcons(file_path, icons, true);
		if (icons.size() > 1)
		{
			for (unsigned int i = 0; i < icons.size(); ++i)
			{
				// @todo (lichao#1#): Text is not refreshed properly : \
				If cancel the drop down list, \
				later selection will change the bitmap,\
				but the text remain the same.\
				maybe a wxbitmapcombobox bug? \
				try to implement a ownerdrawcombox to fix this later.)
				m_cbIcon->Append(_T(""), //wxString::Format(_T("%d"), i),
									icons[i]);
			}
		}
	}

	if (m_cbIcon->GetCount() > 1)
	{
		m_cbIcon->Enable();
		if (sep == wxString::npos)
		{
			m_cbIcon->SetSelection(0);
		}
		else
		{
			long index = 0;
			if (path_and_index.substr(sep + 1).ToLong(&index) && index > 0 && static_cast<unsigned long>(index) < m_cbIcon->GetCount())
			{
				m_cbIcon->SetSelection(index);
				m_txtIcon->ChangeValue(file_path + wxString::Format(_T(",%d"), index));
			}
			else
			{
				m_cbIcon->SetSelection(-1); // select none.
			}
		}
	}
	else
	{
		m_cbIcon->Disable();
	}
	m_cbIcon->Thaw();

}

void TLMenuCfgDialog::OntxtIconText(wxCommandEvent& event)
{
	wxString path_and_index(m_txtIcon->GetValue());
	if (!path_and_index.empty())
	{
		TString str(path_and_index);
		if(ns_file_str_ops::StripCharsAtEnds(str, _T("\" ")))
		{
			m_txtIcon->ChangeValue(str);
			m_txtIcon->SetInsertionPointEnd();
		}
	}
	TryExtractIcons();
	SetIconPathModifiedFlag();
}

void TLMenuCfgDialog::UpdateFlgs()
{
	wxTreeItemId item = m_TreeMenu->GetSelection();
	const bool isMenu = item.IsOk() && m_TreeMenu->HasChildren(item);
	const bool isItem = !isMenu;
	const bool oldWildCard = m_flgWildCard->GetValue();

	CheckFlg(m_flgMenu, isMenu);

	CheckFlg(m_flgWildCard, isItem && m_txtTarget->GetValue().Right(1) == _T("*"));
	CheckFlg(m_flgTitle, isItem && m_txtTarget->IsEmpty() && !m_txtNameOrFilter->IsEmpty());

	CheckFlg(m_flgSep,
	         isItem &&
	         m_txtTarget->IsEmpty() &&
	         m_txtIcon->IsEmpty() &&
	         m_txtNameOrFilter->IsEmpty());

	if (oldWildCard != m_flgWildCard->GetValue())
	{
		m_stcNameFilter->SetLabel( m_flgWildCard->GetValue()?
		                           _LNG(STC_Filter):
		                           _LNG(STC_DispName));
	}

	const bool enableIcon = isMenu || (!m_flgWildCard->GetValue() && !m_txtTarget->IsEmpty());

	m_txtIcon->Enable( enableIcon );

	m_btnFindIcon->Enable( enableIcon );

	// target editable only for items, Not for submenus.
	m_stcTarget->Enable(isItem);

	m_txtTarget->Enable(isItem);

	m_btnFindTarget->Enable(isItem);

}

void TLMenuCfgDialog::OnbtnReloadClick(wxCommandEvent& event)
{
	if(InfoChgFlg())
	{
		ReadItemInfo();
	}
}

bool TLMenuCfgDialog::ReadItemInfo()
{
	bool ret = false;
	wxTreeItemId item(m_TreeMenu->GetSelection());

	if (item.IsOk())
	{
		if (wxTreeItemData *pData = m_TreeMenu->GetItemData(item))
		{
			assert(dynamic_cast<MenuItemData*>(pData));
			MenuItemData *p = static_cast<MenuItemData*>(pData);
			m_txtTarget->ChangeValue(p->Target());
			m_txtNameOrFilter->ChangeValue(p->Name());
			m_txtIcon->ChangeValue(p->IconPath());
			TryExtractIcons();
		}
		else
		{
			m_txtTarget->Clear();
			m_txtNameOrFilter->Clear();
			m_txtIcon->Clear();
		}

		InfoChgFlg(false);
		ret = true;
	}

	return ret;
}


bool TLMenuCfgDialog::SaveItemInfo()
{
	bool ret = false;
	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		assert(InfoChgFlg());

		if (wxTreeItemData *pData = m_TreeMenu->GetItemData(item))
		{
			assert(dynamic_cast<MenuItemData *> (pData));
			MenuItemData *p = static_cast<MenuItemData *> (pData);
			using ns_file_str_ops::StripSpaces;
			p->Name(m_txtNameOrFilter->GetValue().Trim(true).Trim(false));
			p->Target(m_txtTarget->GetValue().Trim(true).Trim(false));

			const bool enableIcon = m_flgMenu->GetValue() || (!m_flgWildCard->GetValue() && !m_txtTarget->IsEmpty());

			if (enableIcon)
			{
				p->IconPath(m_txtIcon->GetValue().Trim(true).Trim(false));
			}
			else
			{
				// wildcard or title, no custome icon
				assert(!m_txtIcon->IsEnabled());
				p->IconPath(_T(""));
			}

			/* removi image affects following images' indices.
			// NOTE: Calling Remove(-1) will remove all images from list;
			if (m_TreeMenu->GetImageList() && m_TreeMenu->GetItemImage(item) != -1)
			{
				assert (m_TreeMenu->GetImageList() == &m_iconlist);

				m_iconlist.Remove(m_TreeMenu->GetItemImage(item));
				m_TreeMenu->SetItemImage(item, -1); // reset image;
			}
			//*/

			UpdateItemDisplay(*m_TreeMenu, item);

			MenuChgFlg(true);

			ReadItemInfo();
			//InfoChgFlg(false);
			ret = true;
		}
	}

	return ret;
}

bool TLMenuCfgDialog::Move(const wxTreeItemId from, const wxTreeItemId to, const E_MoveDirection direction)
{
	wxTreeItemId ret;

	if (!from.IsOk() || !to.IsOk())
	{
		return ret;
	}

	const bool isFromSel = (from == m_TreeMenu->GetSelection());

	wxString strTar, strName, strIcon;

	const bool tar = m_bTargetChanged;

	const bool name = m_bNameFilterChanged;

	const bool icon = m_bIconChanged;

	if (isFromSel)
	{
		if(tar)	{ strTar = m_txtTarget->GetValue(); }

		if(name) { strName = m_txtNameOrFilter->GetValue(); }

		if(icon) { strIcon = m_txtIcon->GetValue(); }
	}

	if (direction == e_up)
	{
		ret = MoveItem(*m_TreeMenu, from, to, true);
	}
	else
	{
		ret = MoveItem(*m_TreeMenu, from, to, false);
	}

	if (ret && isFromSel)
	{
		m_TreeMenu->SelectItem(ret);

		if (tar) { m_txtTarget->SetValue(strTar); }

		if (name) { m_txtNameOrFilter->SetValue(strName); }

		if (icon) { m_txtIcon->SetValue(strIcon); }
	}

	return ret;
}


void TLMenuCfgDialog::OnbtnUpClick(wxCommandEvent& event)
{
	Move(m_TreeMenu->GetSelection(), m_TreeMenu->GetPrevSibling(m_TreeMenu->GetSelection()), e_up);
}


void TLMenuCfgDialog::OnbtnDownClick(wxCommandEvent& event)
{
	Move(m_TreeMenu->GetSelection(), m_TreeMenu->GetNextSibling(m_TreeMenu->GetSelection()), e_down);
}

void TLMenuCfgDialog::OnbtnDelClick(wxCommandEvent& event)
{
	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		if (m_TreeMenu->GetPrevSibling(item) || m_TreeMenu->GetNextSibling(item))
		{
			if (!m_TreeMenu->HasChildren(item) ||
			        wxMessageBox(_LNG(STR_Ask_Delete_Menu), _LNG(STR_Confirm), wxYES_NO) == wxYES)
			{

				/*
				// it seems remove a image will affect the index of other images,
				// and all following images will be wrongly displayed.
				// so I just leave them unused.
				//
				// NOTE: Calling Remove(-1) will remove all images from list;
				if (m_TreeMenu->GetImageList() && m_TreeMenu->GetItemImage(item) != -1)
				{
					assert (m_TreeMenu->GetImageList() == &m_iconlist);

					m_iconlist.Remove(m_TreeMenu->GetItemImage(item));
				}
				//*/
				m_TreeMenu->Delete(item);
				MenuChgFlg(true);
			}
		}
		else
		{
			// is only child
			wxMessageBox(_LNG(STR_Err_Del_Only_Child), _LNG(STR_Failed));
		}
	}
}

void TLMenuCfgDialog::OnbtnNewDirClick(wxCommandEvent& event)
{
	if(!AllowChangeSel())
		return;

	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		wxTreeItemId dir = InsertItem(*m_TreeMenu, item);

		if (dir.IsOk())
		{
			// add a sub item, to make it a sub-tree
			wxTreeItemId subItem = m_TreeMenu->AppendItem(dir, _T(""));

			if (subItem.IsOk())
			{
				m_TreeMenu->SetItemData(dir, new MenuItemData(_LNG(STR_DisplayName), _T(""), _T("")));
				UpdateItemDisplay(*m_TreeMenu, dir);
				m_TreeMenu->SetItemData(subItem, new MenuItemData(_LNG(STR_DisplayName), _LNG(STR_PathToTarget), _T("")));
				UpdateItemDisplay(*m_TreeMenu, subItem);
				m_TreeMenu->SelectItem(dir);
				MenuChgFlg(true);
			}
			else
			{
				m_TreeMenu->Delete(dir);
			}
		}
	}
}

void TLMenuCfgDialog::OnbtnNewItemClick(wxCommandEvent& event)
{
	if(!AllowChangeSel())
		return;

	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		wxTreeItemId add = InsertItem(*m_TreeMenu, item);

		if (add.IsOk())
		{
			m_TreeMenu->SetItemData(add, new MenuItemData(_LNG(STR_DisplayName), _LNG(STR_PathToTarget), _T("")));
			UpdateItemDisplay(*m_TreeMenu, add);
			m_TreeMenu->SelectItem(add);
			MenuChgFlg(true);
		}
	}
}

void TLMenuCfgDialog::OnbtnApplyClick(wxCommandEvent& event)
{
	SaveToFile();
}

bool TLMenuCfgDialog::SaveToFile()
{
	if (m_bMenuChanged)
	{
		// save menu to file
		CMenuData &menu = m_menuData;
		menu.Clear();
		TreeToMenuData(*m_TreeMenu, m_TreeMenu->GetRootItem(), menu);
		menu.SaveAs(m_fileName);
		MenuChgFlg(false);
		return true;
	}

	return false;
}


void TLMenuCfgDialog::OnbtnFindTargetClick(wxCommandEvent& event)
{
	wxString filename(wxFileSelectorEx(_LNG(STR_Choose_Target)));

	if ( !filename.empty() )
	{
		m_txtTarget->SetValue(_T("\"") + filename + _T("\""));
		SetNameFromTarget();
	}
}


void TLMenuCfgDialog::OnBitmapButton2Click(wxCommandEvent& event)
{
	wxString filename(wxFileSelector(_LNG(STR_Choose_Icon)));

	if ( !filename.empty() )
	{
		m_txtIcon->SetValue(filename);
	}
}


void TLMenuCfgDialog::TreeToMenuData(const wxTreeCtrl &tree, const wxTreeItemId item, CMenuData &menu)
{
	if (!item.IsOk())
	{
		return;
	}

	MenuItemData *p = static_cast<MenuItemData*>(tree.GetItemData(item));
	assert(p);
	menu.Name(static_cast<const TCHAR*>(p->Name().c_str()));
	menu.Path(static_cast<const TCHAR*>(p->IconPath().c_str()));

	wxTreeItemId vcookie = item;
	wxTreeItemIdValue cookie = &vcookie;

	for (wxTreeItemId id = tree.GetFirstChild(item, cookie); id.IsOk(); id = tree.GetNextChild(item, cookie))
	{
		if (!tree.HasChildren(id))
		{
			MenuItemData *p = static_cast<MenuItemData*>(tree.GetItemData(id));
			assert(p);
			menu.AddItem(menu.Count(), p->Name().wc_str(), p->Target().wc_str(), p->IconPath().wc_str());
		}
		else
		{
			menu.AddMenu(menu.Count(), _T(""), _T(""), _T(""));
			assert(menu.Menu(menu.Count()-1));
			TreeToMenuData(tree, id, *menu.Menu(menu.Count()-1));
		}
	}
}

void TLMenuCfgDialog::OnClose(wxCloseEvent& event)
{
	if ( event.CanVeto() && (InfoChgFlg() || MenuChgFlg()) )
	{
		const int ans = wxMessageBox(_LNG(STR_Exit_Ask_Save), _LNG(STR_Confirm), wxYES_NO | wxCANCEL);

		if (ans == wxCANCEL)
		{
			return;
		}
		else if (ans == wxYES)
		{
			if (InfoChgFlg())
			{
				SaveItemInfo();
			}

			SaveToFile();
		}
	}

	EndModal(0);
}

void TLMenuCfgDialog::OnTreeMenuBeginDrag(wxTreeEvent& event)
{
	m_dragSrc = event.GetItem();

	if (m_dragSrc.IsOk())
	{
		event.Allow();
	}
}

void TLMenuCfgDialog::OnTreeMenuEndDrag(wxTreeEvent& event)
{
	assert(m_dragSrc.IsOk());
	Move(m_dragSrc, event.GetItem(), e_up);
}


void TLMenuCfgDialog::OnHotKey(wxCommandEvent& event)
{
	const int id (event.GetId());

	if (1 || id == ID_SAVE_OR_APPLY)
	{
		if (InfoChgFlg())
		{
			SaveItemInfo();
		}
		else if (MenuChgFlg())
		{
			SaveToFile();
		}
	}
	else if (id == ID_DELETE_ITEM)
	{
	}
}

bool TLMenuCfgDialog::AllowChangeSel()
{
	bool bAllow = true;

	if (InfoChgFlg())
	{
		switch(wxMessageBox(_LNG(STR_Ask_Save_Item_Info), _LNG(STR_Confirm) , wxYES_NO | wxCANCEL))
		{
		case wxYES:
			SaveItemInfo();
			break;
		case wxNO:
			// do Nothing, change item without saving.
			ReadItemInfo();
			break;
		case wxCANCEL:
			bAllow = false;
			break;
		default:
			assert(false);
		}
	}

	return bAllow;
}

int SearchTree(const wxTreeCtrl &tree, const wxTreeItemId item, const wxString &str, std::vector<wxTreeItemId> &found)
{
	if (!item.IsOk())
	{
		return 0;
	}

	wxTreeItemId vcookie = item;
	wxTreeItemIdValue cookie = &vcookie;

	for (wxTreeItemId id = tree.GetFirstChild(item, cookie); id.IsOk(); id = tree.GetNextChild(item, cookie))
	{
		MenuItemData *p = static_cast<MenuItemData*>(tree.GetItemData(id));
		assert(p);
		if (p->Name().Lower().Find(str) != wxNOT_FOUND)
		{
			found.push_back(id);
		}

		if (tree.HasChildren(id))
		{
			SearchTree(tree, id, str, found);
		}
	}
	return found.size();
}

void TLMenuCfgDialog::OnSearchTextChange(wxCommandEvent& event)
{
	wxString text (m_search->GetValue());
	if(text.empty())
	{
		return;
	}
	text.MakeLower();

	// search the tree control for the input string.
	std::vector<wxTreeItemId> found;
	SearchTree(*m_TreeMenu, m_TreeMenu->GetRootItem(), text, found);
	if(!found.empty())
	{
		// try match current selection; select first match if not match.
		wxTreeItemId sel = m_TreeMenu->GetSelection();
		unsigned int match = 0;
		while(match < found.size() && sel != found[match])
		{
			++match;
		}
		m_TreeMenu->SelectItem(match == found.size() ? found[0] : found[match]);
	}
}

void TLMenuCfgDialog::Onm_searchTextEnter(wxCommandEvent& event)
{
	wxString text (m_search->GetValue());
	if(text.empty())
	{
		return;
	}
	text.MakeLower();

	std::vector<wxTreeItemId> found;
	SearchTree(*m_TreeMenu, m_TreeMenu->GetRootItem(), text, found);
	if(!found.empty())
	{
		// try match current selection and select next match; select first match if not match.
		wxTreeItemId sel = m_TreeMenu->GetSelection();
		unsigned int match = 0;
		while(match + 1 < found.size() && sel != found[match])
		{
			++match;
		}
		m_TreeMenu->SelectItem(match == found.size()-1 ? found[0] : found[match+1]);
	}
}


void TLMenuCfgDialog::Onm_cbIconSelected(wxCommandEvent& event)
{
	int n = m_cbIcon->GetSelection();
	if (n >= 0)
	{
		// change icon path text
		const wxString path_and_index(m_txtIcon->GetValue());
		unsigned sep = FindIconIndexSepCharPos(path_and_index);
		wxString file_path = path_and_index.substr(0, sep).Strip();

		if (n > 0)
		{
			file_path += wxString::Format(_T(",%d"), n);
		}
		if (file_path != path_and_index)
		{
			m_txtIcon->ChangeValue(file_path);	// NO event for the change, and
			SetIconPathModifiedFlag();			// we manually set it changed here.
		}
	}
}
