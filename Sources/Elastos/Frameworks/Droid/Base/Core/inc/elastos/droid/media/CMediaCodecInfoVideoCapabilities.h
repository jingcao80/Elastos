
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Media_CMediaCodecInfoVideoCapabilities.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IDouble;
using Elastos::Core::IInteger32;
using Elastos::Droid::Utility::IRange;
using Elastos::Droid::Utility::ISize;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoVideoCapabilities)
    , public Object
    , public IMediaCodecInfoVideoCapabilities
{
public:
    CMediaCodecInfoVideoCapabilities();

    virtual ~CMediaCodecInfoVideoCapabilities();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Returns the range of supported bitrates in bits/second.
     */
    CARAPI GetBitrateRange(
        /* [out] */ IRange** result);

    /**
     * Returns the range of supported video widths.
     */
    CARAPI GetSupportedWidths(
        /* [out] */ IRange** result);

    /**
     * Returns the range of supported video heights.
     */
    CARAPI GetSupportedHeights(
        /* [out] */ IRange** result);

    /**
     * Returns the alignment requirement for video width (in pixels).
     *
     * This is a power-of-2 value that video width must be a
     * multiple of.
     */
    CARAPI GetWidthAlignment(
        /* [out] */ Int32* result);

    /**
     * Returns the alignment requirement for video height (in pixels).
     *
     * This is a power-of-2 value that video height must be a
     * multiple of.
     */
    CARAPI GetHeightAlignment(
        /* [out] */ Int32* result);

    /**
     * Return the upper limit on the smaller dimension of width or height.
     * <p></p>
     * Some codecs have a limit on the smaller dimension, whether it be
     * the width or the height.  E.g. a codec may only be able to handle
     * up to 1920x1080 both in landscape and portrait mode (1080x1920).
     * In this case the maximum width and height are both 1920, but the
     * smaller dimension limit will be 1080. For other codecs, this is
     * {@code Math.min(getSupportedWidths().getUpper(),
     * getSupportedHeights().getUpper())}.
     *
     * @hide
     */
    CARAPI GetSmallerDimensionUpperLimit(
        /* [out] */ Int32* result);

    /**
     * Returns the range of supported frame rates.
     * <p>
     * This is not a performance indicator.  Rather, it expresses the
     * limits specified in the coding standard, based on the complexities
     * of encoding material for later playback at a certain frame rate,
     * or the decoding of such material in non-realtime.
     */
    CARAPI GetSupportedFrameRates(
        /* [out] */ IRange** result);

    /**
     * Returns the range of supported video widths for a video height.
     * @param height the height of the video
     */
    CARAPI GetSupportedWidthsFor(
        /* [in] */ Int32 height,
        /* [out] */ IRange** result);

    /**
     * Returns the range of supported video heights for a video width
     * @param width the width of the video
     */
    CARAPI GetSupportedHeightsFor(
        /* [in] */ Int32 width,
        /* [out] */ IRange** result);

    /**
     * Returns the range of supported video frame rates for a video size.
     * <p>
     * This is not a performance indicator.  Rather, it expresses the limits specified in
     * the coding standard, based on the complexities of encoding material of a given
     * size for later playback at a certain frame rate, or the decoding of such material
     * in non-realtime.
     * @param width the width of the video
     * @param height the height of the video
     */
    CARAPI GetSupportedFrameRatesFor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IRange** result);

    /**
     * Returns whether a given video size ({@code width} and
     * {@code height}) and {@code frameRate} combination is supported.
     */
    CARAPI AreSizeAndRateSupported(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Double frameRate,
        /* [out] */ Boolean* result);

    /**
     * Returns whether a given video size ({@code width} and
     * {@code height}) is supported.
     */
    CARAPI IsSizeSupported(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * @throws java.lang.ClassCastException */
    CARAPI SupportsFormat(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI Init(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent);

    /** @hide */
    CARAPI GetBlockSize(
        /* [out] */ ISize** result);

    /** @hide */
    CARAPI GetBlockCountRange(
        /* [out] */ IRange** result);

    /** @hide */
    CARAPI GetBlocksPerSecondRange(
        /* [out] */ IRange** result);

    /** @hide */
    CARAPI GetAspectRatioRange(
        /* [in] */ Boolean blocks,
        /* [out] */ IRange** result);

        /** @hide */
    static CARAPI Create(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
        /* [out] */ IMediaCodecInfoVideoCapabilities** result);

private:
    CARAPI_(Boolean) Supports(
        /* [in] */ IInteger32* width,
        /* [in] */ IInteger32* height,
        /* [in] */ IDouble* rate);

    CARAPI_(void) InitWithPlatformLimits();

    CARAPI_(void) ParseFromInfo(
        /* [in] */ IMediaFormat* info);

    CARAPI_(void) ApplyBlockLimits(
        /* [in] */ Int32 blockWidth,
        /* [in] */ Int32 blockHeight,
        /* [in] */ IRange* counts,
        /* [in] */ IRange* rates,
        /* [in] */ IRange* ratios);

    CARAPI_(void) ApplyAlignment(
        /* [in] */ Int32 widthAlignment,
        /* [in] */ Int32 heightAlignment);

    CARAPI_(void) UpdateLimits();

    CARAPI_(void) ApplyMacroBlockLimits(
        /* [in] */ Int32 maxHorizontalBlocks,
        /* [in] */ Int32 maxVerticalBlocks,
        /* [in] */ Int32 maxBlocks,
        /* [in] */ Int64 maxBlocksPerSecond,
        /* [in] */ Int32 blockWidth,
        /* [in] */ Int32 blockHeight,
        /* [in] */ Int32 widthAlignment,
        /* [in] */ Int32 heightAlignment);

    CARAPI_(void) ApplyLevelLimits();

private:
    static const String TAG;

    AutoPtr<IMediaCodecInfoCodecCapabilities> mParent;
    AutoPtr<IRange> mBitrateRange;
    AutoPtr<IRange> mHeightRange;
    AutoPtr<IRange> mWidthRange;
    AutoPtr<IRange> mBlockCountRange;
    AutoPtr<IRange> mHorizontalBlockRange;
    AutoPtr<IRange> mVerticalBlockRange;
    AutoPtr<IRange> mAspectRatioRange;
    AutoPtr<IRange> mBlockAspectRatioRange;
    AutoPtr<IRange> mBlocksPerSecondRange;
    AutoPtr<IRange> mFrameRateRange;

    Int32 mBlockWidth;
    Int32 mBlockHeight;
    Int32 mWidthAlignment;
    Int32 mHeightAlignment;
    Int32 mSmallerDimensionUpperLimit;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIES_H__
