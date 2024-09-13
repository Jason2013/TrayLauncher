#ifndef WUKONG_TYPETRAITS_H
#define WUKONG_TYPETRAITS_H
#include <string>
#include <typeinfo>

namespace wukong {
;
template<bool bIsUnicode> class StringType{ public : typedef std::wstring ResultType; };
template <> class StringType<false> { public : typedef std::string ResultType; };


class NullType ;

// Compile Time Assert, use like this : MestBeTrue<exp>();
template <bool bShouldBeTrue> class MustBeTrue ;
template <> class MustBeTrue<true> {public : enum {result = true};  };


// 剥除  /////////////////////////////////////////
//剥除 const 修饰
template <typename T> class StripConst { public : typedef T ResultType; typedef T StepType; };
template <typename T> class StripConst<const T> { public : typedef typename StripConst<T>::ResultType ResultType; typedef T StepType; };

//剥除 N 维 指针修饰
template <typename T> class StripPointer { public : typedef T ResultType; typedef T StepType; };
template <typename T> class StripPointer<T*> { public : typedef typename StripPointer<T>::ResultType ResultType; typedef T StepType; };

//剥除 引用 修饰
template <typename T> class StripRef { public : typedef T ResultType; typedef T StepType; };
template <typename T> class StripRef<T&> { public : typedef typename StripRef<T>::ResultType ResultType; typedef T StepType; };

//剥除 N 维 数组修饰
template <typename T> class StripN { public : typedef T ResultType; };
template <typename T, int N> class StripN<T [N]> { public : typedef typename StripN<T>::ResultType ResultType; typedef T StepType; };

//剥除 所有的 指针,数组 引用 和 const 等等修饰，得到原始的类型, 可以和类型判别一起用
template <typename T> class StripType { public : typedef T ResultType; typedef T StepType; };
template <typename T> class StripType<T*> { public : typedef typename StripType<T>::ResultType ResultType; typedef T StepType; };
template <typename T> class StripType<const T> { public : typedef typename StripType<T>::ResultType ResultType; typedef T StepType; };
template <typename T, int N> class StripType<T [N]> { public : typedef typename StripType<T>::ResultType ResultType; typedef T StepType; };
template <typename T> class StripType<T &> { public : typedef typename StripType<T>::ResultType ResultType; typedef T StepType;};
template <typename T> class StripType<volatile T> { public : typedef typename StripType<T>::ResultType ResultType; typedef T StepType; };
template <typename T> class StripType<T []> ;//无定义,禁用



//类型文字描述 //////////////////////////////////////////

//typedef StringType<sizeof(*_T("")) == sizeof(wchar_t)>::ResultType TString;
typedef std::string TString;
template <typename T> class NameOf { public : static const TString Result() { static const TString str ( typeid(T).name() ); return str; } };
template <typename T> class NameOf<T*> { public : static const TString Result() {static const TString  str ( "Pointer to " + NameOf<T>::Result() ); return str;} };
template <typename T> class NameOf<const T> { public : static const TString Result() {static const TString str ( "const " + NameOf<T>::Result() ); return str;} };
template <typename T> class NameOf<T &> { public : static const TString Result() {static const TString str ( "Ref of " + NameOf<T>::Result() ); return str;} };
template <typename T, int N>
class NameOf<T [N]> {
	static const TString IntToString(int n){
		TString str;
		do{
			str.insert(str.begin(), static_cast<TString::value_type>(n%10 + '0') );
		} while(n /= 10);
		return str;
	}
public :
	static const TString Result() {static const TString str ( IntToString(N) + " size Array of " + NameOf<T>::Result() ); return str;}
};
template <typename T> class NameOf<T []> ;


// 鉴别类型 //////////////////////////////////////
template <typename T> class IsPointer { public: enum { result = false }; typedef NullType OrgType; };
template <typename T> class IsPointer<T*> {  public: enum { result = true  }; typedef T OrgType; };

template <typename T> class IsConst { public: enum { result = false }; typedef T OrgType; };
template <typename T> class IsConst<const T> { public: enum { result = true }; typedef T OrgType; };

template <typename T> class IsArray { public: enum { result = false }; typedef NullType OrgType; };
template <typename T, int N> class IsArray<T [N]> { public: enum { result = true }; typedef T OrgType; };

template <typename T> class IsRef	 { public: enum { result = false }; };
template <typename T> class IsRef<T&> { public: enum { result = true  }; };

template <typename T> class IsArrayPointer {
 public :
	enum {result = IsPointer<T>::result && IsArray<typename StripPointer<T>::StepType>::result };
};

template <typename T> class IsMemberPointer { public: enum { result = false }; };
template <typename T, typename U> class IsMemberPointer<U T::*> { public: enum { result = true  }; };


//判断类型是否是用户定义类型
template <typename T> class IsClass {
	template <typename T1, typename T2> class IsSameType { public: enum { result = false }; };
	template <typename T1> class IsSameType<T1, T1> { public: enum { result = true }; };
	typedef typename StripType<T>::ResultType PureType;

public :
	enum { result = IsSameType<PureType, T>::result &&
		!IsSameType<T, bool>::result &&
		!IsSameType<T, signed char>::result &&
		!IsSameType<T, unsigned char>::result &&
		!IsSameType<T, wchar_t>::result &&
		!IsSameType<T, signed short>::result &&
		!IsSameType<T, unsigned short>::result &&
		!IsSameType<T, signed int>::result &&
		!IsSameType<T, unsigned int>::result &&
		!IsSameType<T, signed long>::result &&
		!IsSameType<T, unsigned long>::result &&
		!IsSameType<T, float>::result &&
		!IsSameType<T, double>::result &&
		!IsSameType<T, void>::result
	};
};

//判别类型关系 、、、、、、、、、、
// 完全是同一个类型
template <typename T1, typename T2> 	class IsSameType	{public : enum {result = false}; };
template <typename T> 					class IsSameType<T,T>{public : enum {result = true }; };

class CChecker {
public:
	typedef char SmallType;
	typedef struct { SmallType sth[2];} BigType ;
};

//鉴别类间的可转换性
template <class T1, class T2>
class Convertion{

	typedef CChecker::SmallType SmallType;
	typedef CChecker::BigType BigType;

	static SmallType Test(T1);
	static BigType Test(...);
	static T2 MakeT2();
public:
	enum {ok2To1 = (sizeof(Test(MakeT2())) == sizeof(SmallType)),
		ok1To2 = Convertion<T2, T1>::ok2To1,
		okEachToEach = (ok2To1 && ok1To2)
	};
};

// 派生关系
template <typename B, typename D, bool bAllowSameType = true>
class IsBaseDerived
{
public :
	enum
	{result = Convertion<const B *, const D *>::ok2To1
				&& ! IsSameType<const B *, const void *>::result
				&& (bAllowSameType || ! IsSameType<const B *, const D *>::result)
	};
};


template <typename T>
class TypeTraits
{
private:

public:
	enum {
		isPointer = IsPointer<T>::result,
		isRef = IsRef<T>::result,
		isClass = IsClass<T>::result,
		isMemberPointer = IsMemberPointer<T>::result
	};
};

template <int N> class Int2Type {public:	enum {value = N};};

// Type version of oprator ? : 
template <bool bCond, typename T, typename F>
struct SelectType { typedef T ResultType; };

template <typename T, typename F>
struct SelectType<false, T, F> { typedef F ResultType; };


}// end of namespace wukong

#endif // WUKONG_TYPETRAITS_H

