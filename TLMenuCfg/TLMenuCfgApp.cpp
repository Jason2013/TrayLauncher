/***************************************************************
 * Name:      TLMenuCfgApp.cpp
 * Purpose:   Code for Application Class
 * Author:     (lichao)
 * Created:   2010-10-31
 * Copyright:  (lichao)
 * License: GPL 3.0
 **************************************************************/

#include "wx_pch.h"
#include "TLMenuCfgApp.h"
#include <wx/snglinst.h>
#include <wx/cshelp.h>

//(*AppHeaders
#include "TLMenuCfgMain.h"
#include <wx/image.h>
//*)


bool IsOnlyInstance()
{
	static wxSingleInstanceChecker checker(::wxGetUserId() + _T("lch_TL_MenuCfg_Single_Instance_checker"));
	return (!checker.IsAnotherRunning());
}


IMPLEMENT_APP(TLMenuCfgApp);


//! ExtraSettings: �����в�����������Ŀ
std::map<wxString, wxString> & ExtraSettings()
{
	static std::map<wxString, wxString> setting;
	return setting;
}



//! ProcessArgSettings: ��ȡ�����в�������

//! @param argc : ��������
//! @param argv : �����ַ�������
//! @return void :
//! @author lichao
//! @date 2009-10-13
//! @note [��]
void ProcessArgSettings(int argc, TCHAR ** argv)
{
	wxString str;
	for (int i = 1; i < argc; ++i) {
		str = argv[i];
		int n = str.find('=');
		if (TSTRING::npos != static_cast<unsigned int>(n) && n > 2) {
			if (str.substr(0,2) == _T("--"))
			{
				ExtraSettings()[str.substr(2,n-2)] = str.substr(n+1);
			}
		}
	}
}

bool TLMenuCfgApp::OnInit()
{
	if (!IsOnlyInstance())
	{
		return false;
	}

	wxSimpleHelpProvider* provider = new wxSimpleHelpProvider;
	wxHelpProvider::Set(provider);

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
		ProcessArgSettings(argc, argv);
		TLMenuCfgDialog Dlg(0);
		SetTopWindow(&Dlg);
		Dlg.ShowModal();
		wxsOK = false;
    }
    //*)

    delete wxHelpProvider::Set(NULL);
    return wxsOK;

}
