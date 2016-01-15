
#ifndef __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__

#include "_Elastos_Droid_Text_Format_CTimeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CTimeHelper)
    , public Singleton
    , public ITimeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Compare(
        /* [in] */ ITime* a,
        /* [in] */ ITime* b,
        /* [out] */ Int32* ret);

    CARAPI GetCurrentTimezone(
        /* [out] */ String* ret);

    CARAPI IsEpoch(
        /* [in] */ ITime* time,
        /* [out] */ Boolean* ret);

    CARAPI GetJulianDay(
        /* [in] */ Int64 millis,
        /* [in] */ Int64 gmtoff,
        /* [out] */ Int32* ret);

    CARAPI GetWeeksSinceEpochFromJulianDay(
        /* [in] */ Int32 julianDay,
        /* [in] */ Int32 firstDayOfWeek,
        /* [out] */ Int32* ret);

    CARAPI GetJulianMondayFromWeeksSinceEpoch(
        /* [in] */ Int32 week,
        /* [out] */ Int32* ret);
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__
