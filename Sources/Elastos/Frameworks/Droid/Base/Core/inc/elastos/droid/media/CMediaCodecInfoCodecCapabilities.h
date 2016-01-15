
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIES_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoCodecCapabilities)
    , public Object
    , public IMediaCodecInfoCodecCapabilities
{
public:
    CMediaCodecInfoCodecCapabilities();

    virtual ~CMediaCodecInfoCodecCapabilities();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IMediaCodecInfoCodecProfileLevel*>* profLevs,
        /* [in] */ ArrayOf<Int32>* colFmts,
        /* [in] */ Boolean encoder,
        /* [in] */ Int32 flags,
        /* [in] */ IMap* defaultFormatMap,
        /* [in] */ IMap* capabilitiesMap);

    CARAPI constructor(
        /* [in] */ ArrayOf<IMediaCodecInfoCodecProfileLevel*>* profLevs,
        /* [in] */ ArrayOf<Int32>* colFmts,
        /* [in] */ Boolean encoder,
        /* [in] */ Int32 flags,
        /* [in] */ IMediaFormat* defaultFormat,
        /* [in] */ IMediaFormat* info);

    /**
     * Query codec feature capabilities.
     * <p>
     * These features are supported to be used by the codec.  These
     * include optional features that can be turned on, as well as
     * features that are always on.
     */
    CARAPI IsFeatureSupported(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Query codec feature requirements.
     * <p>
     * These features are required to be used by the codec, and as such,
     * they are always turned on.
     */
    CARAPI IsFeatureRequired(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI ValidFeatures(
        /* [out, callee] */ ArrayOf<String>** result);

    /** @hide */
    CARAPI IsRegular(
        /* [out] */ Boolean* result);

    /**
     * Query whether codec supports a given {@link MediaFormat}.
     * @param format media format with optional feature directives.
     * @throws IllegalArgumentException if format is not a valid media format.
     * @return whether the codec capabilities support the given format
     *         and feature requests.
     */
    CARAPI IsFormatSupported(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    /**
     * Returns a MediaFormat object with default values for configurations that have
     * defaults.
     */
    CARAPI GetDefaultFormat(
        /* [out] */ IMediaFormat** result);

    /**
     * Returns the mime type for which this codec-capability object was created.
     */
    CARAPI GetMimeType(
        /* [out] */ String* result);

    /**
     * Returns the audio capabilities or {@code null} if this is not an audio codec.
     */
    CARAPI GetAudioCapabilities(
        /* [out] */ IMediaCodecInfoAudioCapabilities** result);

    /**
     * Returns the encoding capabilities or {@code null} if this is not an encoder.
     */
    CARAPI GetEncoderCapabilities(
        /* [out] */ IMediaCodecInfoEncoderCapabilities** result);

    /**
     * Returns the video capabilities or {@code null} if this is not a video codec.
     */
    CARAPI GetVideoCapabilities(
        /* [out] */ IMediaCodecInfoVideoCapabilities** result);

    /** @hide */
    CARAPI Dup(
        /* [out] */ IMediaCodecInfoCodecCapabilities** result);

    /**
     * Retrieve the codec capabilities for a certain {@code mime type}, {@code
     * profile} and {@code level}.  If the type, or profile-level combination
     * is not understood by the framework, it returns null.
     */
    static CARAPI CreateFromProfileLevel(
        /* [in] */ const String& mime,
        /* [in] */ Int32 profile,
        /* [in] */ Int32 level,
        /* [out] */ IMediaCodecInfoCodecCapabilities** result);

private:
    CARAPI_(AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> >) GetValidFeatures();

    CARAPI_(Boolean) CheckFeature(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags);

    CARAPI_(Boolean) IsAudio();

    CARAPI_(Boolean) IsEncoder();

    CARAPI_(Boolean) IsVideo();

    static CARAPI_(AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> >) Init();

public:
    // LEGACY FIELDS

    // Enumerates supported profile/level combinations as defined
    // by the type of encoded data. These combinations impose restrictions
    // on video resolution, bitrate... and limit the available encoder tools
    // such as B-frame support, arithmetic coding...
    AutoPtr<ArrayOf<IMediaCodecInfoCodecProfileLevel*> > mProfileLevels;  // NOTE this array is modifiable by user

    /**
     * Defined in the OpenMAX IL specs, color format values are drawn from
     * OMX_COLOR_FORMATTYPE.
     */
    AutoPtr<ArrayOf<Int32> > mColorFormats; // NOTE this array is modifiable by user

    // errors while reading profile levels - accessed from sister capabilities
    Int32 mError;

private:
    static const String TAG;

    static const AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > decoderFeatures;

    // CLASSIFICATION
    String mMime;

    Int32 mFlagsSupported;
    Int32 mFlagsRequired;
    Int32 mFlagsVerified;

    // NEW-STYLE CAPABILITIES
    AutoPtr<IMediaCodecInfoAudioCapabilities> mAudioCaps;
    AutoPtr<IMediaCodecInfoVideoCapabilities> mVideoCaps;
    AutoPtr<IMediaCodecInfoEncoderCapabilities> mEncoderCaps;
    AutoPtr<IMediaFormat> mDefaultFormat;
    AutoPtr<IMediaFormat> mCapabilitiesInfo;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIES_H__
