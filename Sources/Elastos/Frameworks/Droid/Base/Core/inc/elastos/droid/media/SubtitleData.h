#ifndef __ELASTOS_DROID_MEDIA_SUBTITLEDATA_H__
#define __ELASTOS_DROID_MEDIA_SUBTITLEDATA_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class SubtitleData
    : public Object
    , public ISubtitleData
{
public:
    CAR_INTERFACE_DECL()

    SubtitleData();

    virtual ~SubtitleData();

    CARAPI constructor();

    CARAPI GetTrackIndex(
        /* [out] */ Int32* trackIndex);

    CARAPI GetStartTimeUs(
        /* [out] */ Int64 * result);

    CARAPI GetDurationUs(
        /* [out] */ Int64 * result);

    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Byte>** result);

private:
    CARAPI_(Boolean) ParseParcel(
        /* [in] */ IParcel* in);

private:
    static const String TAG;
    Int32 mTrackIndex;
    Int64 mStartTimeUs;
    Int64 mDurationUs;
    AutoPtr<ArrayOf<Byte> > mData;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_SUBTITLEDATA_H__