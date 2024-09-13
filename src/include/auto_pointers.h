//! some smart handle.
//! \author : lichao
//! \date :
// 2007-09-27 created

#ifndef SIMPLE_AUTO_POINTER_H
#define SIMPLE_AUTO_POINTER_H
#include "auto_handle.h"
#include <windows.h>
using namespace wukong;


//////////////////////////////////////////////////////////////////////
/////////////////          派生 应用         //////////////////////////
//////////////////////////////////////////////////////////////////////

class GdiDestroyer {
public:
	static void Free(HGDIOBJ obj) {DeleteObject(obj); }
protected:
	GdiDestroyer(){}
	~GdiDestroyer(){}
};
//! GDI 自动指针
template <class T>
class gdi_ptr : public auto_handle<T, gdi_ptr<T>, false>, public GdiDestroyer
{
    typedef auto_handle<T, gdi_ptr<T>, false> Super;
public:
	gdi_ptr(){}
    gdi_ptr(const T & obj):Super(obj){}
	~gdi_ptr(){MustBeTrue<Convertion<HGDIOBJ, T>::ok2To1>();}
	gdi_ptr & operator = (T obj) {Super::operator = (obj); return *this;}
	gdi_ptr & operator = (gdi_ptr & another) {Super::operator = (another); return *this;}

private:
	// 禁用
	template <class _T> gdi_ptr(gdi_ptr<_T> &) ;
	template <class _T> gdi_ptr & operator = (const gdi_ptr<_T> &);
	//gdi_ptr & operator = (const gdi_ptr &);
};

//! 图标 自动指针
class IconDestroyer{public : static void Free(HICON &hIcon) {DestroyIcon(hIcon); hIcon = NULL;}  };
typedef auto_handle<HICON, IconDestroyer, 0, false>  icon_ptr, Icon_Handle, Icon_H;

//! Memory DC 自动指针
class MemDcDestroyer {public : static void Free(HDC &hdc) {DeleteDC(hdc); hdc = NULL;}  };
typedef auto_handle<HDC, MemDcDestroyer, 0> MemDC_Handle, MemDC_H;

//! Window 自动指针
class WindowDestroyer {public : static void Free(HWND &hwnd) {DestroyWindow(hwnd); hwnd = NULL;}  };
typedef auto_handle<HWND, WindowDestroyer, 0> AutoHwnd;

//! HOOK
class HookDestroyer {public : static void Free(HHOOK hhk) {UnhookWindowsHookEx(hhk);}   };
typedef auto_handle<HHOOK, HookDestroyer, 0, false> AutoHook;


#endif // SIMPLE_AUTO_POINTER_H
