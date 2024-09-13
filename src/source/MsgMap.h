
#ifndef TRAYLAUNCHER_MSGMAP_H
#define TRAYLAUNCHER_MSGMAP_H

const UINT UM_NEWINSTANCE = WM_USER + 3;

class CSettingFile;
CSettingFile & Settings();


class CMsgMap
{
//public:
	typedef LRESULT (*MsgProc)(HWND, UINT, WPARAM, LPARAM);
	typedef std::map<const UINT,MsgProc> MsgMap;
	typedef MsgMap :: const_iterator MsgIter;

public:
	bool Add(UINT message, MsgProc proc) { m_msgMap[message] = proc; return true; }
	bool Romove(UINT message) {
		const bool r = Contain(message);
		if (r) {
			m_msgMap.erase(message);
		}
		return r;
	}
	bool Contain(UINT message) const
	{
		return ( m_msgMap.find(message) != m_msgMap.end() ); 
	}
	bool ProcessMessage(HWND hWnd, UINT message, WPARAM wP, LPARAM lP, LRESULT * pResult = NULL) const 
	{
		bool r = Contain(message);
		if (r) {
			LRESULT result = (*m_msgMap.find(message)).second(hWnd, message, wP, lP);
			if(pResult)
				*pResult = result;
		}
		return r;
	}

private:
	MsgMap m_msgMap;
};

#endif // TRAYLAUNCHER_MSGMAP_H

