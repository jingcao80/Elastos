
#ifndef __ELASTOS_DROID_NET_RETURNOUTVALUE_H__
#define __ELASTOS_DROID_NET_RETURNOUTVALUE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;

//======================================================================
// FOR_EACH
//======================================================================
/**
 * Usage:
 * If java code likes that followed
 *
 * for(RouteInfo route : mRoutes) {
 *    ...
 * }
 *
 * and if mRoutes is a Collection object or sub class of Collection,
 * you can use the macro FOR_EACH as followed
 *
 * FOR_EACH(iter, mRoutes) {
 *    AutoPtr<IRouteInfo> route = IRouteInfo::Probe(Ptr(iter)->Func(iter->GetNext));
 *    ...
 * }
 *
 */
#define FOR_EACH(iterator, container) AutoPtr<IIterator> iterator;      \
        ICollection::Probe(container)->GetIterator((IIterator**)&iterator);                 \
        while (ReturnOutValue(iterator, iterator->HasNext))

//======================================================================
// FUNC_RETURN
//======================================================================
/**
 * Usage:
 * Use
 *
 * FUNC_RETURN(value);
 *
 * instead of
 *
 * *result = value;
 * return NOERROR;
 *
 * or
 *
 * *result = value;
 * REFCOUNT_ADD(value)
 * return NOERROR;
 *
 * to avoid forgetting of REFCOUNT_ADD.
 * If the error code is not NOERROR, replace
 *
 * FUNC_RETURN(value);
 *
 * with
 *
 * FUNC_RETURN_ERROR_CODE(value, errcode);
 *
 */
template <typename T>
inline void funcReturnVal(T** result)
{
    REFCOUNT_ADD(*result)
}

template <typename T>
inline void funcReturnVal(ArrayOf<T*>** result)
{
    REFCOUNT_ADD(*result)
}

template <typename T>
inline void funcReturnVal(ArrayOf<T>** result)
{
    REFCOUNT_ADD(*result)
}

template <typename T>
inline void funcReturnVal(T* result)
{
    return;
}

#define FUNC_RETURN_ERROR_CODE(obj, errCode) {*result = obj;\
                                funcReturnVal(result);\
                                return errCode;}

#define FUNC_RETURN(obj) FUNC_RETURN_ERROR_CODE(obj, NOERROR)

//======================================================================
// Ptr()->Func()
//======================================================================
/**
 * Usage:
 * Use
 *
 * if(Ptr(map)->Func(map->IsEmpty)) {
 *     ...
 * }
 *
 * to replace
 *
 * Boolean b;
 * map->IsEmpty(&b);
 * if (b) {
 *     ...
 * }
 *
 */
template <typename T_OBJ>
class Redefine;

template <typename T>
AutoPtr<Redefine<T> > Ptr(T* obj)
{
    AutoPtr<Redefine<T> > rev = new Redefine<T>(obj);
    return rev;
}

template <typename T>
AutoPtr<Redefine<T> > Ptr(AutoPtr<T>& obj)
{
    return Ptr(obj.Get());
}

template <typename T_OBJ>
class Redefine
    : public Object
{
public:
    Redefine(T_OBJ* obj)
        : mPtr(obj)
    {}

    template <typename T_REV>
    T_REV Func(ECode (T_OBJ::*func)(T_REV*))
    {
        T_REV rev;
        (mPtr->*func)(&rev);
        return rev;
    }

    template <typename T_REV>
    AutoPtr<T_REV> Func(ECode (T_OBJ::*func)(T_REV**))
    {
        AutoPtr<T_REV> rev;
        (mPtr->*func)((T_REV**)&rev);
        return rev;
    }

    template <typename T_REV>
    AutoPtr<Redefine<T_REV> > GetPtr(ECode (T_OBJ::*func)(T_REV**))
    {
        AutoPtr<T_REV> rev;
        (mPtr->*func)((T_REV**)&rev);
        return Ptr(rev);
    }

private:
    AutoPtr<T_OBJ> mPtr;
};

// @Deprecated
//======================================================================
// RETN_OUT_VAL
//======================================================================
template <typename T_OBJ, typename T_REV>
T_REV ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_REV*))
{
    T_REV rev;
    (obj->*func)(&rev);
    return rev;
}

template <typename T_OBJ, typename T_REV>
AutoPtr<T_REV> ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_REV**))
{
    AutoPtr<T_REV> rev;
    (obj->*func)((T_REV**)&rev);
    return rev;
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
AutoPtr<T_REV> ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_PARA1 para1, T_REV**), T_PARA1 para1)
{
    AutoPtr<T_REV> rev;
    (obj->*func)(para1, (T_REV**)&rev);
    return rev;
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
T_REV ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_PARA1 para1, T_REV*), T_PARA1 para1)
{
    T_REV rev;
    (obj->*func)(para1, &rev);
    return rev;
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
T_REV ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_PARA1* para1, T_REV*), T_PARA1* para1)
{
    T_REV rev;
    (obj->*func)(para1, &rev);
    return rev;
}

/**
 * no object, such as global function, static function
 * involve this function with type parameter list, for it can't identify the T_REV type
 */

template <typename T_REV, typename T_PARA1>
AutoPtr<T_REV> ReturnOutValue(ECode (*func)(T_PARA1*, T_REV**), T_PARA1* para1)
{
    AutoPtr<T_REV> rev;
    (*func)(para1, (T_REV**)&rev);
    return rev;
}

/**
 * Extended
 */
template <typename T_OBJ, typename T_REV>
T_REV ReturnOutValue(AutoPtr<T_OBJ> obj, ECode (T_OBJ::*func)(T_REV*))
{
    return ReturnOutValue(obj.Get(), func);
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
T_REV ReturnOutValue(AutoPtr<T_OBJ> obj, ECode (T_OBJ::*func)(T_PARA1* para1, T_REV*), T_PARA1* para1)
{
    return ReturnOutValue(obj.Get(), func, para1);
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
AutoPtr<T_REV> ReturnOutValue(AutoPtr<T_OBJ> obj, ECode (T_OBJ::*func)(T_PARA1 para1, T_REV**), T_PARA1 para1)
{
    return ReturnOutValue(obj.Get(), func, para1);
}

template <typename T_OBJ, typename T_REV, typename T_PARA1>
T_REV ReturnOutValue(T_OBJ* obj, ECode (T_OBJ::*func)(T_PARA1* para1, T_REV*), AutoPtr<T_PARA1> para1)
{
    return ReturnOutValue(obj, func, para1.Get());
}

template <typename T_REV, typename T_PARA1>
AutoPtr<T_REV> ReturnOutValue(ECode (*func)(T_PARA1*, T_REV**), AutoPtr<T_PARA1> para1)
{
    return ReturnOutValue(func, para1.Get());
}

#define RETN_OUT_VAL(obj, func, arg...) ReturnOutValue(obj, (obj)->func, ##arg)

#endif // __ELASTOS_DROID_NET_RETURNOUTVALUE_H__
