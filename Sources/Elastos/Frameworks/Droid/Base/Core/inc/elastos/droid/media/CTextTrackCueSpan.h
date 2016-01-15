#ifndef __ELASTOS_DROID_MEDIA_CTEXTTRACKCUESPAN_H__
#define __ELASTOS_DROID_MEDIA_CTEXTTRACKCUESPAN_H__

#include "_Elastos_Droid_Media_CTextTrackCueSpan.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CTextTrackCueSpan)
    , public Object
    , public ITextTrackCueSpan
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CTextTrackCueSpan();

    virtual ~CTextTrackCueSpan();

    CARAPI constructor(
        /* [in] */ const String& text,
        /* [in] */ Int64 timestamp);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetTimeStampMs(
        /* [out] */ Int64* timestampMs);

    CARAPI SetTimeStampMs(
        /* [in] */ Int64 timestampMs);

    CARAPI GetText(
        /* [out] */ String* result);

    CARAPI SetText(
        /* [in] */ const String& result);

    CARAPI GetEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean result);

private:
    Int64 mTimestampMs;
    Boolean mEnabled;
    String mText;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTEXTTRACKCUESPAN_H__