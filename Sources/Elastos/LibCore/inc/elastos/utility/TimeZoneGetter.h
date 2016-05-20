#ifndef __ELASTOS_UTILITY_TIMEZONEGETTER_H__
#define __ELASTOS_UTILITY_TIMEZONEGETTER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::ITimeZoneGetter;

namespace Elastos {
namespace Utility {

class ECO_PUBLIC TimeZoneGetter
    : public Object
    , public ITimeZoneGetter
{
public:
    TimeZoneGetter();

    virtual ~TimeZoneGetter();

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<ITimeZoneGetter>) GetInstance();

    static CARAPI SetInstance(
        /* [in] */ ITimeZoneGetter* getter);

    virtual CARAPI GetId(
        /* [out] */ String* string) = 0;

    CARAPI ToString(
        /* [out] */ String* str);
private:
    ECO_LOCAL static AutoPtr<ITimeZoneGetter> sInstance;
};

class DefaultTimeZoneGetter
    : public TimeZoneGetter
{
public:
    virtual CARAPI GetId(
        /* [out] */ String* string);

private:
    String mId;
};

}// namespace Utility
}// namespace Utility

#endif // __ELASTOS_UTILITY_TIMEZONEGETTER_H__
