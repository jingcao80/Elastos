#ifndef __ELASTOS_UTILITY_TIMEZONEGETTER_H__
#define __ELASTOS_UTILITY_TIMEZONEGETTER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

namespace Elastos {
namespace Utility {

class TimeZoneGetter
{
private:
    class DefaultTimeZoneGetter
        : public Object
        , public ITimeZoneGetter
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI GetId(
            /* [out] */ String* string);

    private:
        String mId;
    };

public:
    static CARAPI_(AutoPtr<ITimeZoneGetter>) GetInstance();

    static CARAPI SetInstance(
        /* [in] */ ITimeZoneGetter* getter);

private:
    TimeZoneGetter();
    TimeZoneGetter(const TimeZoneGetter&);

private:
    static AutoPtr<ITimeZoneGetter> sInstance;
};

}// namespace Utility
}// namespace Utility

#endif // __ELASTOS_UTILITY_TIMEZONEGETTER_H__
