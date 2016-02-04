
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECMEDIAIMAGE_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECMEDIAIMAGE_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_Media_CMediaCodecMediaImage.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/Image.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecMediaImage)
    , public Image
    , public IMediaCodecMediaImage
{
private:
    class MediaPlane
        : public Image::Plane
    {
    public:
        MediaPlane(
            /* [in] */ IByteBuffer* buffer,
            /* [in] */ Int32 rowInc,
            /* [in] */ Int32 colInc,
            /* [in] */ CMediaCodecMediaImage* host)
            : mRowInc(rowInc)
            , mColInc(colInc)
            , mData(buffer)
            , mHost(host)
        {}

        CARAPI GetBuffer(
            /* [out] */ IByteBuffer** result);

        CARAPI GetPixelStride(
            /* [out] */ Int32* result);

        CARAPI GetRowStride(
            /* [out] */ Int32* result);

    private:
        Int32 mRowInc;
        Int32 mColInc;
        AutoPtr<IByteBuffer> mData;
        CMediaCodecMediaImage* mHost;
    };

public:
    CMediaCodecMediaImage();

    virtual ~CMediaCodecMediaImage();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ IByteBuffer* info,
        /* [in] */ Boolean readOnly,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 xOffset,
        /* [in] */ Int32 yOffset,
        /* [in] */ IRect* cropRect);

    CARAPI GetFormat(
        /* [out] */ Int32* result);

    CARAPI GetHeight(
        /* [out] */ Int32* result);

    CARAPI GetWidth(
        /* [out] */ Int32* result);

    CARAPI GetTimestamp(
        /* [out] */ Int64* result);

    CARAPI GetPlanes(
        /* [out, callee] */ ArrayOf<IImagePlane*>** result);

    CARAPI Close();

    /**
     * Set the crop rectangle associated with this frame.
     * <p>
     * The crop rectangle specifies the region of valid pixels in the image,
     * using coordinates in the largest-resolution plane.
     */
    CARAPI SetCropRect(
        /* [in] */ IRect* cropRect);

private:
    CARAPI CheckValid();

    CARAPI_(Int32) ReadInt32(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Boolean asLong);

private:
    Boolean mIsReadOnly;
    Boolean mIsValid;
    Int32 mWidth;
    Int32 mHeight;
    Int32 mFormat;
    Int64 mTimestamp;
    AutoPtr<ArrayOf<IImagePlane*> > mPlanes;
    AutoPtr<IByteBuffer> mBuffer;
    AutoPtr<IByteBuffer> mInfo;
    Int32 mXOffset;
    Int32 mYOffset;

    static const Int32 TYPE_YUV;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECMEDIAIMAGE_H__
