
#include "Hotkey.h"
#include "FileStrFnc.h"

//! 按照指定的字符(ch)分割输入字符串(inStr)，输出到指定向量(vStr). 空字符串也有效。
unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr);


CHotkey::CHotkey(HWND hWnd):m_hWnd(hWnd)
{
	InitStrToVkeyMap();
}

CHotkey::~CHotkey(void)
{
	Clear();
}

void CHotkey::Clear() {
	for (Iter it = m_keys.begin(); it != m_keys.end(); ++it) {
		UnregisterHotKey(m_hWnd, it->first);
	}
	m_keys.clear();
}

//! map string (or char) to virtual key code, return 0 on failure.
UINT CHotkey::GetVK(const TSTRING &strVK) {
	UINT vk = 0;
	if (strVK.size() == 1) {
		const TSTRING::value_type ch = strVK[0];
		if ('0' <=ch && ch <= '9') {
			vk = ch;
		} else if ('a' <=ch && ch <= 'z') {
			vk = ch - 'a' + 'A';
		} else if ('A' <=ch && ch <= 'Z') {
			vk = ch;
		}
	} else if (m_specialStrToVkey.find(strVK) != m_specialStrToVkey.end()) {
		vk = m_specialStrToVkey[strVK];
	}
	return vk;
}

//! Add hotkey, return true if successful
bool CHotkey::Add(CHotkey::ID id, const TSTRING &inKeyStr) {
	Remove(id);
	TSTRING keyStr(inKeyStr);
	ns_file_str_ops::ToLowerCase(keyStr);

	std::vector<TSTRING> vKey;
	GetSeparatedString(keyStr, '+', vKey);
	for (unsigned int i = 0; i < vKey.size(); ++i) {
		vKey[i] = ns_file_str_ops::StripSpaces(vKey[i]);
	}

	if (vKey.size() < 2) {
		return false;
	}

	const UINT vk = GetVK(vKey.back());
	if (!vk) {
		return false;
	}
	UINT mode = 0;
	for (unsigned int i = 0; i < vKey.size()-1; ++i) {
		if (_T("ctrl") == vKey[i]) {
			mode |= MOD_CONTROL;
		} else if (_T("alt") == vKey[i]) {
			mode |= MOD_ALT;
		} else if (_T("shift") == vKey[i]) {
			mode |= MOD_SHIFT;
		} else if (_T("win") == vKey[i]) {
			mode |= MOD_WIN;
		} else {
			return false;
		}
	}

	if (vk == VK_LWIN || vk == VK_RWIN) { mode |= MOD_WIN; }
	else if (vk == VK_LSHIFT || vk == VK_RSHIFT) { mode |= MOD_SHIFT; }
	else if (vk == VK_LCONTROL || vk == VK_RCONTROL) { mode |= MOD_CONTROL; }
	else if (vk == VK_LMENU || vk == VK_RMENU) { mode |= MOD_ALT; }

	bool r = false;
	if (RegisterHotKey(m_hWnd, id, mode, vk)) {
		m_keys[id] = keyStr;
		r = true;
	}
	return r;
}

bool CHotkey::Remove(CHotkey::ID id) {
	bool r =Find(id);
	if (r) {
		m_keys.erase(id);
		UnregisterHotKey(m_hWnd, id);
	}
	return r;
}

bool CHotkey::Find(CHotkey::ID id) {
	return m_keys.find(id) != m_keys.end();
}


bool CHotkey::Find(CHotkey::ID id, TSTRING & keyStr) {
	if (!Find(id)) {
		 return false;
	}
	keyStr = m_keys[id];
	return true;
}

void CHotkey::InitStrToVkeyMap()
{
	m_specialStrToVkey[_T("home")] = VK_HOME;
	m_specialStrToVkey[_T("space")] = VK_SPACE;
	m_specialStrToVkey[_T("prior")] = VK_PRIOR;
	m_specialStrToVkey[_T("next")] = VK_NEXT;
	m_specialStrToVkey[_T("end")] = VK_END;
	m_specialStrToVkey[_T("home")] = VK_HOME;
	m_specialStrToVkey[_T("left")] = VK_LEFT;
	m_specialStrToVkey[_T("up")] = VK_UP;
	m_specialStrToVkey[_T("right")] = VK_RIGHT;
	m_specialStrToVkey[_T("down")] = VK_DOWN;
	m_specialStrToVkey[_T("select")] = VK_SELECT;
	m_specialStrToVkey[_T("print")] = VK_PRINT;
	m_specialStrToVkey[_T("execute")] = VK_EXECUTE;
	m_specialStrToVkey[_T("snapshot")] = VK_SNAPSHOT;
	m_specialStrToVkey[_T("insert")] = VK_INSERT;
	m_specialStrToVkey[_T("delete")] = VK_DELETE;
	m_specialStrToVkey[_T("help")] = VK_HELP;
	m_specialStrToVkey[_T("f1")] = VK_F1;
	m_specialStrToVkey[_T("f2")] = VK_F2;
	m_specialStrToVkey[_T("f3")] = VK_F3;
	m_specialStrToVkey[_T("f4")] = VK_F4;
	m_specialStrToVkey[_T("f5")] = VK_F5;
	m_specialStrToVkey[_T("f6")] = VK_F6;
	m_specialStrToVkey[_T("f7")] = VK_F7;
	m_specialStrToVkey[_T("f8")] = VK_F8;
	m_specialStrToVkey[_T("f9")] = VK_F9;
	m_specialStrToVkey[_T("f10")] = VK_F10;
	m_specialStrToVkey[_T("f11")] = VK_F11;
	m_specialStrToVkey[_T("f12")] = VK_F12;
	m_specialStrToVkey[_T("lwin")] = VK_LWIN;
	m_specialStrToVkey[_T("rwin")] = VK_RWIN;
	m_specialStrToVkey[_T("lshift")] = VK_LSHIFT;
	m_specialStrToVkey[_T("rshift")] = VK_RSHIFT;
	m_specialStrToVkey[_T("lcontrol")] = VK_LCONTROL;
	m_specialStrToVkey[_T("rcontrol")] = VK_RCONTROL;
	m_specialStrToVkey[_T("lmenu")] = VK_LMENU;
	m_specialStrToVkey[_T("rmenu")] = VK_RMENU;
	m_specialStrToVkey[_T("escape")] = VK_ESCAPE;
	m_specialStrToVkey[_T("esc")] = VK_ESCAPE;

}
