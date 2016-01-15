
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIES_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Media_CMediaCodecInfoEncoderCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IRange;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoEncoderCapabilities)
    , public Object
    , public IMediaCodecInfoEncoderCapabilities
{
public:
    friend class CMediaCodecInfoEncoderCapabilities;

    CMediaCodecInfoEncoderCapabilities();

    virtual ~CMediaCodecInfoEncoderCapabilities();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Returns the supported range of quality values.
     *
     * @hide
     */
    CARAPI GetQualityRange(
        /* [out] */ IRange** result);

    /**
     * Returns the supported range of encoder complexity values.
     * <p>
     * Some codecs may support multiple complexity levels, where higher
     * complexity values use more encoder tools (e.g. perform more
     * intensive calculations) to improve the quality or the compression
     * ratio.  Use a lower value to save power and/or time.
     */
    CARAPI GetComplexityRange(
        /* [out] */ IRange** result);

    /**
     * Query whether a bitrate mode is supported.
     */
    CARAPI IsBitrateModeSupported(
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI Init(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent);

    /** @hide */
    CARAPI SetDefaultFormat(
        /* [in] */ IMediaFormat* format);

    /** @hide */
    CARAPI SupportsFormat(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI Create(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
        /* [out] */ IMediaCodecInfoEncoderCapabilities** result);

private:
    static CARAPI_(Int32) ParseBitrateMode(
        /* [in] */ const String& mode);

    CARAPI_(void) ApplyLevelLimits();

    CARAPI_(void) ParseFromInfo(
        /* [in] */ IMediaFormat* info);

    CARAPI_(Boolean) Supports(
        /* [in] */ IInteger32* complexity,
        /* [in] */ IInteger32* quality,
        /* [in] */ IInteger32* profile);

    static CARAPI_(AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> >) Init();

private:
    static const AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > bitrates;

    AutoPtr<IRange> mQualityRange;
    AutoPtr<IRange> mComplexityRange;
    AutoPtr<IMediaCodecInfoCodecCapabilities> mParent;

    Int32 mBitControl;
    AutoPtr<IInteger32> mDefaultComplexity;
    AutoPtr<IInteger32> mDefaultQuality;
    String mQualityScale;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIES_H__
