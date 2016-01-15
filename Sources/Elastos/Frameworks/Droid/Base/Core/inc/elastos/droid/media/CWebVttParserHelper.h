#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTPARSERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTPARSERHELPER_H__

#include "_Elastos_Droid_Media_CWebVttParserHelper.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttParserHelper)
    , public Singleton
    , public IWebVttParserHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ParseFloatPercentage(
        /* [in] */ const String& s,
        /* [out] */ Float* result);

    CARAPI ParseIntPercentage(
        /* [in] */ const String& s,
        /* [out] */ Int32* result);

    CARAPI ParseTimestampMs(
        /* [in] */ const String& s,
        /* [out] */ Int64* result);

    CARAPI TimeToString(
        /* [in] */ Int64 timeMs,
        /* [out] */ String* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTPARSERHELPER_H__