#pragma once
#include <stdheaders.h>

class CHotkey
{
	CHotkey & operator = (const CHotkey &);
	CHotkey(const CHotkey &);
	typedef int ID;

public:
	CHotkey(HWND hWnd);
	~CHotkey(void);
	bool Add(ID id, const TSTRING & keyStr);
	bool Remove(ID id);
	void Clear();
	bool Find(ID id, TSTRING & keyStr);
	bool Find(ID id);

private:
	UINT GetVK(const TSTRING & str);
	typedef std::map<int, TSTRING> HKMap;
	typedef HKMap::iterator Iter;
	HKMap m_keys;
	const HWND m_hWnd;
	void InitStrToVkeyMap();
	typedef std::map<TSTRING, unsigned int> StrToVKey;
	StrToVKey m_specialStrToVkey;
};
