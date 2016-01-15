
#ifndef __LIBCORE_ICU_CDATEINTERVALFORMAT_H__
#define __LIBCORE_ICU_CDATEINTERVALFORMAT_H__

#include "Singleton.h"
#include "_Libcore_ICU_CDateIntervalFormat.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;

namespace Libcore {
namespace ICU {

CarClass(CDateIntervalFormat)
    , public Singleton
    , public IDateIntervalFormat
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI FormatDateRange(
        /* [in] */ Int64 startMs,
        /* [in] */ Int64 endMs,
        /* [in] */ Int32 flags,
        /* [in] */ const String& olsonId,
        /* [out] */ String* rst);

    CARAPI FormatDateRange(
        /* [in] */ ILocale* locale,
        /* [in] */ ITimeZone* tz,
        /* [in] */ Int64 startMs,
        /* [in] */ Int64 endMs,
        /* [in] */ Int32 flags,
        /* [out] */ String* rst);

};

} // ICU
} // Libcore

#endif // __LIBCORE_ICU_CDATEINTERVALFORMAT_H__