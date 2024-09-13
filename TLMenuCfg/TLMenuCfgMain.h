/***************************************************************
 * Name:      TLMenuCfgMain.h
 * Purpose:   Defines Application Frame
 * Author:     (lichao)
 * Created:   2010-10-31
 * Copyright:  (lichao)
 * License:	GPL 3.0
 **************************************************************/

#ifndef TLMENUCFGMAIN_H
#define TLMENUCFGMAIN_H

//(*Headers(TLMenuCfgDialog)
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/bmpcbox.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "MenuData.h"

class TLMenuCfgDialog: public wxDialog
{
    public:

        TLMenuCfgDialog(wxWindow* parent,wxWindowID id = -1);
        virtual ~TLMenuCfgDialog();

    private:

        //(*Handlers(TLMenuCfgDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnbtnUpClick(wxCommandEvent& event);
        void OnInit(wxInitDialogEvent& event);
        void OnTreeMenuSelectionChanged(wxTreeEvent& event);
        void OnbtnSaveClick(wxCommandEvent& event);
        void OnTreeMenuSelChanging(wxTreeEvent& event);
        void OntxtTargetText(wxCommandEvent& event);
        void OntxtNameOrFilterText(wxCommandEvent& event);
        void OntxtIconText(wxCommandEvent& event);
        void OnbtnReloadClick(wxCommandEvent& event);
        void OnbtnDownClick(wxCommandEvent& event);
        void OnbtnDelClick(wxCommandEvent& event);
        void OnbtnNewDirClick(wxCommandEvent& event);
        void OnbtnNewItemClick(wxCommandEvent& event);
        void OnbtnApplyClick(wxCommandEvent& event);
        void OnbtnFindTargetClick(wxCommandEvent& event);
        void OnBitmapButton2Click(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnTreeMenuBeginDrag(wxTreeEvent& event);
        void OnTreeMenuEndDrag(wxTreeEvent& event);
        void OnSearchTextChange(wxCommandEvent& event);
        void Onm_searchTextEnter(wxCommandEvent& event);
        void Onm_cbIconSelected(wxCommandEvent& event);
        //*)
        void OnHotKey(wxCommandEvent& event);

        //(*Identifiers(TLMenuCfgDialog)
        static const long ID_STATICTEXT2;
        static const long ID_SEARCHCTRL1;
        static const long ID_TREECTRL_MENU;
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON3;
        static const long ID_BITMAPBUTTON4;
        static const long ID_BITMAPBUTTON5;
        static const long ID_BITMAPBUTTON2;
        static const long ID_CHECKBOX4;
        static const long ID_CHECKBOX5;
        static const long ID_STATICTEXT1;
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL1;
        static const long ID_BITMAPBUTTON6;
        static const long ID_STATICTEXT3;
        static const long ID_CHECKBOX2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL3;
        static const long ID_BITMAPCOMBOBOX1;
        static const long ID_BITMAPBUTTON7;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON2;
        static const long ID_BUTTON7;
        //*)

        //(*Declarations(TLMenuCfgDialog)
        wxBitmapButton* m_btnFindTarget;
        wxBitmapButton* m_btnDown;
        wxBitmapComboBox* m_cbIcon;
        wxBitmapButton* m_btnNewDir;
        wxButton* m_btnClose;
        wxStaticText* m_stcCustomizeIcon;
        wxBitmapButton* m_btnDel;
        wxSearchCtrl* m_search;
        wxButton* m_btnSave;
        wxBitmapButton* m_btnUp;
        wxBitmapButton* m_btnNewItem;
        wxBitmapButton* m_btnFindIcon;
        wxTreeCtrl* m_TreeMenu;
        wxTextCtrl* m_txtTarget;
        wxCheckBox* m_flgSep;
        wxCheckBox* m_flgTitle;
        wxBoxSizer* BoxSizer1;
        wxCheckBox* m_flgWildCard;
        wxStaticText* m_stcNameFilter;
        wxStaticText* m_stcTarget;
        wxTextCtrl* m_txtIcon;
        wxStaticText* m_stcMenu;
        wxButton* m_btnApply;
        wxButton* m_btnReload;
        wxTextCtrl* m_txtNameOrFilter;
        wxCheckBox* m_flgMenu;
        //*)

        DECLARE_EVENT_TABLE()

		bool InfoChgFlg() const { return m_bInfoUnsaved; }
		void InfoChgFlg(const bool val);
		bool MenuChgFlg() const { return m_bMenuChanged; }
		void MenuChgFlg(const bool val);
		void UpdateItemDisplay(wxTreeCtrl &tree, wxTreeItemId item, const bool refreshImage = true);
		void MenuDataToTree(const CItem &mi, wxTreeCtrl &tree, wxTreeItemId id);
		void MenuDataToTree(const CMenuData &mi, wxTreeCtrl &tree, wxTreeItemId id);
		void TreeToMenuData(const wxTreeCtrl &tree, const wxTreeItemId idRoot, CMenuData &menu);
		bool SaveToFile();
		wxTreeItemId InsertItem(wxTreeCtrl &tree, const wxTreeItemId & item, bool before = true, const TSTRING &strName = _T(""));
		wxTreeItemId MoveItem(wxTreeCtrl &tree, wxTreeItemId from, wxTreeItemId to, const bool before = true);
		wxTreeItemId CopyItem(wxTreeCtrl &tree, wxTreeItemId from, wxTreeItemId to, const bool before = true);
		bool ReadItemInfo();
		bool SaveItemInfo();
		void CheckFlg(wxCheckBox* ctrl, const bool val);
		void UpdateFlgs();
		enum E_MoveDirection {e_up, e_down};
		bool Move(const wxTreeItemId from, const wxTreeItemId to, const E_MoveDirection);
		bool AllowChangeSel();
		bool SetNameFromTarget();

		bool m_bInfoUnsaved;	//!< if there is an unsaved change in current item.
		bool m_bTargetChanged;
		bool m_bNameFilterChanged;
		bool m_bIconChanged;

		bool m_bMenuChanged;	//!< if there is a saved change in any item.
		CMenuData m_menuData;
		wxImageList m_iconlist;
		int m_indexUnknown;
		int m_indexFolder;
		int m_indexFolderOpen;
		int m_indexSep;
		int m_indexTitle;
		int m_indexWildCard;
		wxTreeItemId m_dragSrc;

		TSTRING m_fileName;
		void SetIconPathModifiedFlag();
		void TryExtractIcons();
		wxString m_iconExtractPathOld;
};

#endif // TLMENUCFGMAIN_H
