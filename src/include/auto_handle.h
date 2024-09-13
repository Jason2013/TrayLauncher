//! Auto Handle template class, do clean up during deconstruction.
//! \author : Chao Li
//! \date :

#ifndef WUKONG_AUTO_HANDLE_H
#define WUKONG_AUTO_HANDLE_H
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include "TypeTraits.h"
#include <tchar.h>

namespace wukong {
;

template <bool bCanBeBase> class CBase {};
template <> class CBase<false> { private : CBase(); ~CBase(); };

template <class T>
	class Holder
	{
	protected:
		typedef T ObjectType;
	private:
		ObjectType m_obj;
	protected:
		Holder(const ObjectType & t):m_obj(t){}
		~Holder(){}
		//inline ObjectType & Private_Data() {return m_obj;}
		inline ObjectType * Protected_Ptr() { return &m_obj; }
		inline ObjectType Protected_Get() const { return m_obj;}
	public:
	};
template <> class Holder<void> ; // no void object
template <typename T> class Holder<T &>; // no refs


template <class T>
	class HolderChooser
	{
	public:
		//typedef Holder<typename SelectType<IsPointer<T>::result, void*, T>::ResultType> ResultType;
		typedef Holder<T> ResultType;
	};

template <class B, class D, class T>
	class OpSwitch {
	protected:
		OpSwitch(){
			MustBeTrue< IsBaseDerived<B, D>::result > ();
			MustBeTrue< IsBaseDerived<typename HolderChooser<T>::ResultType, D>::result > ();
			//MustBeTrue< IsBaseDerived<D, B>::result > ();
			}
		~OpSwitch(){}
	};

template <class T, class D, int iPointer>
	class PtrOpSwitch : public OpSwitch<PtrOpSwitch<T,D,iPointer>, D, T> , public CBase<iPointer>= 0 && iPointer <= 2>
	{
	private:
		T & operator [] (int index) const;
		T & operator * () const;
		T * operator ->() const;
	} ;

template <class T, class D>
	class PtrOpSwitch <T*, D, 1> : public OpSwitch<PtrOpSwitch<T*,D, 1>, D, T*>
	{
	protected:
		PtrOpSwitch(){}
		~PtrOpSwitch(){}
	private:
		T & operator [] (int index) const;
	public:
		T & operator * () const //throw ()
		{
			assert(static_cast<const D* const>(this)->Get());
			return *(static_cast<const D* const>(this)->Get());
		}

		T * operator ->() const //throw ()
		{
			assert(static_cast<const D* const>(this)->Get());
			return static_cast<const D* const>(this)->Get();
		}
	};

template <class T, class D>
	class PtrOpSwitch <T*, D, 2> : public OpSwitch<PtrOpSwitch<T*,D, 2>, D, T*>
	{
	protected:
		PtrOpSwitch(){}
		~PtrOpSwitch(){}
	public :
		T & operator [] (int index) const //throw ()
		{
			assert(static_cast<const D* const>(this)->Get());
			return *(static_cast<const D* const>(this)->Get() + index);
		}

		T & operator * () const //throw ()
		{
			assert(static_cast<const D* const>(this)->Get());
			return *(static_cast<const D* const>(this)->Get());
		}

		T * operator ->() const //throw ()
		{
			assert(static_cast<const D* const>(this)->Get());
			return static_cast<const D* const>(this)->Get();
		}
	};

template <class T, class D, bool bAllowCast = false>
	class CastOpSwitch : public OpSwitch<CastOpSwitch<T,D,bAllowCast>, D, T>
	{
	protected:
		CastOpSwitch(){}
		~CastOpSwitch(){}
	//private : operator T () const;
	};

template <class T, class D>
	class CastOpSwitch <T, D, true> : public OpSwitch<CastOpSwitch<T,D,true>, D, T>
	{
	protected:
		CastOpSwitch(){}
		~CastOpSwitch(){}
	public:
		operator T () const {return (static_cast<const D*>(this)->Get());}
	};

template <class T, class D, int iAllowPointerOpr, bool bAllowCast>
	class PtrCastOpSwitch :
		public PtrOpSwitch<T,D, IsPointer<T>::result ? iAllowPointerOpr : 0 > ,
		public CastOpSwitch<T,D,bAllowCast>
	{
	protected:
		PtrCastOpSwitch(){}
		~PtrCastOpSwitch(){}
	};
//class CChecker {
//public:
//	typedef char SmallType;
//	typedef struct { SmallType sth[2];} BigType ;
//};
	namespace  {
		template <class T>
			struct ReturnValueChecker {
				static CChecker::BigType CheckIt(T (*)()) ;
				template <class R, class P>
					static CChecker::SmallType CheckIt(R (*) (P));
			};
		template <class T>
			struct RefParamChecker {
				static CChecker::BigType CheckIt(void (*)(T&)) ;
				template <class R, class P>
					static CChecker::SmallType CheckIt(R (*) (P));
			};
	};

//template <int N> class Int2Type {public:	enum {value = N};};
;

//! A smart handle(or pointer) class.

//! \param T_Object : class, usually a pointert or handle type
//! \param T_Destroyer: class, T_Destroyer::Free function do the clean up, //Free should better not throw any exception
//! \param bAllowPointerOperationForPointers : int. 0:disable, 1: pointer, 2: pointer and array.
//! \param bAllowCast: bool, if allow direct cast to the stored type.
template <class T_Object, class T_Destroyer, int bAllowPointerOperationForPointers = 1, bool bAllowCast = true, bool bPreferVoidPtr = false>
	class auto_handle :
		public HolderChooser<T_Object>::ResultType,
		public PtrCastOpSwitch<T_Object, auto_handle<T_Object, T_Destroyer, bAllowPointerOperationForPointers , bAllowCast>, bAllowPointerOperationForPointers , bAllowCast>
	{
		typedef typename HolderChooser<T_Object>::ResultType Super;
		using Super::Protected_Ptr;
		using Super::Protected_Get;

		inline T_Object & Private_Data() {return  *static_cast<T_Object * >(static_cast<void*>( Protected_Ptr() ) ); }

		// Disable
		template <class T_O, class T_D, int T_AP, bool T_AC> auto_handle(const auto_handle<T_O, T_D, T_AP, T_AC> & ) ;
		template <class T_O, class T_D, int T_AP, bool T_AC> auto_handle & operator = (const auto_handle<T_O, T_D, T_AP, T_AC> & );
		template <class This> auto_handle(This); // disable all other 1 param constructiors
		auto_handle * operator &();
		const auto_handle * operator &() const;
		//Disable operator new and delete and their [] form
		static void * operator new (size_t size);
		static void operator delete (void *);
		static void * operator new [] (size_t size);
		static void operator delete [] (void *);

		// Parameter type
		template <typename T, bool bSelf> struct ConstParamType { typedef const typename StripRef<T>::ResultType & ResultType; };
		template <typename T> struct ConstParamType<T, true> { typedef const T ResultType; };
		typedef typename ConstParamType<T_Object, IsPointer<T_Object>::result>::ResultType ObjParam;

		// get an invalid object.
		/*
		//1 impl, functor
		template <class Ret, bool bUseZero>
			struct InvalidMaker {
				inline static Ret Get() throw() {return 0;}
			};
		template <class Ret>
			struct InvalidMaker<Ret, false> {
				inline static Ret Get() throw () {return T_Destroyer::Free();}
			};
		/*/
		// 2 impl, function overload,
		inline static ObjParam InvalidMaker(const Int2Type < 1 > * const)
		{
			return T_Destroyer::Free();
		}

		inline static ObjParam InvalidMaker(const Int2Type < 0 > * const)
		{
			return 0;
		}
		// */


	protected:
		//return user defined invalid obj  or  0.
		inline static ObjParam GetInvalidObject()
		{
			//*
			return InvalidMaker (
				static_cast
				<
					Int2Type
					<
						sizeof(ReturnValueChecker<T_Object>::CheckIt(T_Destroyer::Free)) == sizeof(CChecker::BigType) ||
						sizeof(ReturnValueChecker<ObjParam>::CheckIt(T_Destroyer::Free)) == sizeof(CChecker::BigType)
					> *
				> (0) );
			/*/
			return InvalidMaker
				<
					ObjParam ,
					! (	sizeof(ReturnValueChecker<T_Object>::CheckIt(T_Destroyer::Free)) == sizeof(yes) ||
						sizeof(ReturnValueChecker<ObjParam>::CheckIt(T_Destroyer::Free)) == sizeof(yes)
					)
				>::Get();
			// */
		}

	public:
		//using Super::Get;
		inline T_Object Get() const { return static_cast<T_Object>(Protected_Get());}
		// construction and de--.
		explicit auto_handle(ObjParam obj = GetInvalidObject()) : Super(obj) { }
		auto_handle(auto_handle & another) : Super(another.Release()) {
		}
		~auto_handle() {
			assert(MustBeTrue<sizeof(Get()) == sizeof(Protected_Get())>::result);
			//try {
				Private_Free();
			//}catch(...){}
		}

		// assign, move actually
		auto_handle & operator = (auto_handle & another) {
			Reset(another.Release());
			return *this;
		}

		auto_handle & operator = (ObjParam obj)
		{
			Reset(obj);
			return *this;
		}

		void Swap(auto_handle & another) {
			using std::swap;
			swap(Private_Data(), another.Private_Data());
			//T_Object t = Get();
			//Private_Data() = another.Get();
			//another.Private_Data() = t;
		}

		const T_Object Release() { T_Object tmpObj = Get(); Private_Data() = GetInvalidObject(); return tmpObj; }

		// assign new value or free.
		void Reset(ObjParam obj = GetInvalidObject())
		{
			if (Get() != obj) {
				Private_Free();
				Private_Data() = obj;
			}
		}

	private:
		inline void Private_Free() {
			//try {
			// Do the actual clean up.
			PrivateDestroy(
				static_cast
				<
					Int2Type<sizeof(RefParamChecker<T_Object>::CheckIt(T_Destroyer::Free)) == sizeof(CChecker::BigType)> *
				> (0) );
			//}catch(...){assert(false);}
		}

		// function overload, do clean up.
		// use parameter type to decide which to use, the value will be ignored, 0 (zero) is preferred .
		// default, value parameter,,
		inline void PrivateDestroy(const Int2Type<0> * const ) {
			if(Get() != GetInvalidObject()) {
				T_Destroyer::Free(Private_Data());
				Private_Data() = GetInvalidObject();
			}
		}
		//ref parameter
		inline void PrivateDestroy(const Int2Type<1> * const ) {
			T_Destroyer::Free(Private_Data());
		}
	};

template <class T_Object, class T_Destroyer, int bAllowPointerOperationForPointers, bool bAllowCast>//, template <class> class _Validator = Valid>
void swap(auto_handle<T_Object, T_Destroyer, bAllowPointerOperationForPointers , bAllowCast> & one, auto_handle<T_Object, T_Destroyer, bAllowPointerOperationForPointers, bAllowCast> & another)
{
	one.Swap(another);
};

//////////////////////////////////////////////////////////////////////
/////////////////        Derived use         /////////////////////////
//////////////////////////////////////////////////////////////////////

//! file auto ptr
class FileCloser {public : inline static void Free(FILE * &file) { if(file) { fclose(file); file = 0; } } };
class file_ptr: public auto_handle<FILE *,  FileCloser, false>
{
    typedef auto_handle<FILE *, FileCloser, false> Super;
public:
    file_ptr() throw() {}
    file_ptr(const TCHAR * szFileName, const TCHAR * szMode) throw() :Super(_tfopen(szFileName, szMode)) {}
    file_ptr(FILE * file) throw() :Super(file) {}
};

////////////      new  and  new[], very strict type check.
template <class C,bool bAllowCast = false> class Arr;

//! template class : Ptr, smart pointer
template <class C, bool bAllowCast = false>
	class Ptr : public auto_handle<C*, Ptr<C>,1,bAllowCast>
	{
	private:
		typedef C * PtrType;
		typedef auto_handle<C*, Ptr<C>,1,bAllowCast> Super;
		// Disable these
		template <class T, bool b> Ptr(const Ptr<T,b> & ) ;
		template <class T, bool b> Ptr & operator = (const Ptr<T,b> & ) ;
		template <class T, bool b> Ptr & operator = (const Arr<T,b> & ) ;
		//Ptr & operator = (const Ptr & another);
		//Disable other pointers including derived class pointers
		template <class T> Ptr(T * const);
		template <class T> Ptr & operator = (T* const);

	public:
		inline static void Free(C* &p)
		{
			delete p;
			p = 0;
		}
		explicit Ptr(PtrType pC = 0) throw() : Super(pC){}
		//explicit Ptr(C *pC = 0) throw() : Super(pC){}
		Ptr & operator = (Ptr & another) throw() {Super::operator = (another); return *this;}
		Ptr & operator = (PtrType pC) throw() {Super::operator = (pC); return *this;}
	};
	template <bool bAllowCast> class Ptr<void, bAllowCast> ;

//! template class : Arr, smart pointer, array form.
template <class C, bool bAllowCast> // bAllowCast = false
	class Arr : public auto_handle<C*, Arr<C>, 2, bAllowCast>
	{
	private:
		typedef C * PtrType;
		typedef auto_handle<C*, Arr<C>, 2, bAllowCast> Super;
		// Disable these
		template <class T, bool b> Arr (const Arr<T,b> &) ;
		template <class T, bool b> Arr & operator = (const Arr<T,b> &);
		template <class T, bool b> Arr & operator = (const Ptr<T,b> &);
		//Arr & operator = (const Arr & another);
		//Disable other pointers including derived class pointers
		template <class T> Arr(T * const);
		template <class T> Arr & operator = (T* const);
	public:
		inline static void Free(C* &p)
		{
			delete [] p;
			p = 0;
		}
		explicit Arr(PtrType pC = 0) throw() : Super(pC) {}
		Arr & operator = (Arr & another) throw() {Super::operator = (another); return *this;}
		Arr & operator = (PtrType pC) throw() {Super::operator = (pC); return *this;}
	};
	template <bool bAllowCast> class Arr<void, bAllowCast>;

}//namespace


#endif // WUKONG_AUTO_HANDLE_H
