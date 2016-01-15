
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__

#include "_Elastos_Droid_Media_CMediaCodecBufferInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecBufferInfo)
    , public Object
    , public IMediaCodecBufferInfo
{
public:
    CMediaCodecBufferInfo();

    virtual ~CMediaCodecBufferInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Set(
        /* [in] */ Int32 newOffset,
        /* [in] */ Int32 newSize,
        /* [in] */ Int64 newTimeUs,
        /* [in] */ Int32 newFlags);

    CARAPI SetOffset(
        /* [in] */ Int32 offset);

    CARAPI SetSize(
        /* [in] */ Int32 newSize);

    CARAPI SetPresentationTimeUs(
        /* [in] */ Int64 newTimeUs);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI GetSize(
        /* [out] */ Int32* newSize);

    CARAPI GetPresentationTimeUs(
        /* [out] */ Int64* newTimeUs);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

private:
    Int32 mOffset;
    Int32 mSize;
    Int64 mPresentationTimeUs;
    Int32 mFlags;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__
