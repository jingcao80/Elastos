#ifndef __ELASTOS_DROID_MEDIA_WEBVTTPARSERHELPER_H__
#define __ELASTOS_DROID_MEDIA_WEBVTTPARSERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

class WebVttParserHelper
{
public:
    static CARAPI ParseFloatPercentage(
        /* [in] */ const String& s,
        /* [out] */ Float* result);

    static CARAPI ParseIntPercentage(
        /* [in] */ const String& s,
        /* [out] */ Int32* result);

    static CARAPI ParseTimestampMs(
        /* [in] */ const String& s,
        /* [out] */ Int64* result);

    static CARAPI TimeToString(
        /* [in] */ Int64 timeMs,
        /* [out] */ String* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_WEBVTTPARSERHELPER_H__