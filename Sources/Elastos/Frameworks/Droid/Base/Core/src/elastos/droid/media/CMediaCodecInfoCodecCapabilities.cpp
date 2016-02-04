#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoAudioCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/media/CMediaCodecInfoEncoderCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoVideoCapabilities.h"
#include "elastos/droid/media/CMediaFormat.h"
#include <elastos/utility/Arrays.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::Arrays;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CMediaCodecInfo::Feature, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaCodecInfoCodecCapabilities::TAG("CodecCapabilities");

const AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > CMediaCodecInfoCodecCapabilities::decoderFeatures = Init();

CAR_INTERFACE_IMPL(CMediaCodecInfoCodecCapabilities, Object, IMediaCodecInfoCodecCapabilities)

CAR_OBJECT_IMPL(CMediaCodecInfoCodecCapabilities)

CMediaCodecInfoCodecCapabilities::CMediaCodecInfoCodecCapabilities()
    : mError(0)
    , mFlagsSupported(0)
    , mFlagsRequired(0)
    , mFlagsVerified(0)
{
}

CMediaCodecInfoCodecCapabilities::~CMediaCodecInfoCodecCapabilities()
{
}

ECode CMediaCodecInfoCodecCapabilities::constructor()
{
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::constructor(
    /* [in] */ ArrayOf<IMediaCodecInfoCodecProfileLevel*>* profLevs,
    /* [in] */ ArrayOf<Int32>* colFmts,
    /* [in] */ Boolean encoder,
    /* [in] */ Int32 flags,
    /* [in] */ IMap* defaultFormatMap,
    /* [in] */ IMap* capabilitiesMap)
{
    AutoPtr<IMediaFormat> f1;
    CMediaFormat::New(defaultFormatMap, (IMediaFormat**)&f1);
    AutoPtr<IMediaFormat> f2;
    CMediaFormat::New(capabilitiesMap, (IMediaFormat**)&f2);

    return constructor(profLevs, colFmts, encoder, flags, f1, f2);
}

ECode CMediaCodecInfoCodecCapabilities::constructor(
    /* [in] */ ArrayOf<IMediaCodecInfoCodecProfileLevel*>* profLevs,
    /* [in] */ ArrayOf<Int32>* colFmts,
    /* [in] */ Boolean encoder,
    /* [in] */ Int32 flags,
    /* [in] */ IMediaFormat* defaultFormat,
    /* [in] */ IMediaFormat* info)
{
    AutoPtr<IMap> map;
    info->GetMap((IMap**)&map);
    mProfileLevels = profLevs;
    mColorFormats = colFmts;
    mFlagsVerified = flags;
    mDefaultFormat = defaultFormat;
    mCapabilitiesInfo = info;
    mDefaultFormat->GetString(IMediaFormat::KEY_MIME, &mMime);

    if (mMime.ToLowerCase().StartWith("audio/")) {
        CMediaCodecInfoAudioCapabilities::Create(info, this, (IMediaCodecInfoAudioCapabilities**)&mAudioCaps);
        mAudioCaps->SetDefaultFormat(mDefaultFormat);
    }
    else if (mMime.ToLowerCase().StartWith("video/")) {
        CMediaCodecInfoVideoCapabilities::Create(info, this, (IMediaCodecInfoVideoCapabilities**)&mVideoCaps);
    }
    if (encoder) {
        CMediaCodecInfoEncoderCapabilities::Create(info, this, (IMediaCodecInfoEncoderCapabilities**)&mEncoderCaps);
        mEncoderCaps->SetDefaultFormat(mDefaultFormat);
    }

    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > features = GetValidFeatures();
    for (Int32 i = 0; i < features->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*features)[i];
        String key = IMediaFormat::KEY_FEATURE_ + feat->mName;
        AutoPtr<ICharSequence> csq;
        CString::New(key, (ICharSequence**)&csq);
        AutoPtr<IInterface> obj;
        map->Get(csq, (IInterface**)&obj);
        AutoPtr<IInteger32> yesNo = IInteger32::Probe(obj);

        if (yesNo == NULL) {
            continue;
        }

        Int32 val;
        yesNo->GetValue(&val);
        if (val > 0) {
            mFlagsRequired |= feat->mValue;
        }
        mFlagsSupported |= feat->mValue;
        mDefaultFormat->SetInt32(key, 1);
        // TODO restrict features by mFlagsVerified once all codecs reliably verify them
    }
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::IsFeatureSupported(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CheckFeature(name, mFlagsSupported);
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::IsFeatureRequired(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CheckFeature(name, mFlagsRequired);
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::ValidFeatures(
    /* [out, callee] */ ArrayOf<String>** result)
{
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > features = GetValidFeatures();
    AutoPtr<ArrayOf<String> > res = ArrayOf<String>::Alloc(features->GetLength());
    for (Int32 i = 0; i < res->GetLength(); i++) {
        res->Set(i, (*features)[i]->mName);
    }
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::IsRegular(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // regular codecs only require default features
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > features = GetValidFeatures();
    for (Int32 i = 0; i < features->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*features)[i];
        Boolean b;
        IsFeatureRequired(feat->mName, &b);
        if (!feat->mDefault && b) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::IsFormatSupported(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMap> map;
    format->GetMap((IMap**)&map);
    AutoPtr<ICharSequence> csq;
    CString::New(IMediaFormat::KEY_MIME, (ICharSequence**)&csq);
    AutoPtr<IInterface> obj;
    map->Get(csq, (IInterface**)&obj);
    String mime;
    ICharSequence::Probe(obj)->ToString(&mime);

    // mime must match if present
    if (!mime.IsNull() && !mMime.EqualsIgnoreCase(mime)) {
        *result = FALSE;
        return NOERROR;
    }

    // check feature support
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > features = GetValidFeatures();
    for (Int32 i = 0; i < features->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*features)[i];
        AutoPtr<ICharSequence> cs;
        CString::New(IMediaFormat::KEY_FEATURE_ + feat->mName, (ICharSequence**)&cs);
        AutoPtr<IInterface> obj;
        map->Get(cs, (IInterface**)&obj);
        AutoPtr<IInteger32> yesNo = IInteger32::Probe(obj);
        Int32 val;
        yesNo->GetValue(&val);
        if (yesNo == NULL) {
            continue;
        }
        Boolean b1, b2;
        IsFeatureSupported(feat->mName, &b1);
        IsFeatureRequired(feat->mName, &b2);
        if ((val == 1 && !b1) ||
                (val == 0 && b2)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    Boolean b;
    if (mAudioCaps != NULL && !(mAudioCaps->SupportsFormat(format, &b), b)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mVideoCaps != NULL && !(mVideoCaps->SupportsFormat(format, &b), b)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mEncoderCaps != NULL && !(mEncoderCaps->SupportsFormat(format, &b), b)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::GetDefaultFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDefaultFormat;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::GetMimeType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMime;
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::GetAudioCapabilities(
    /* [out] */ IMediaCodecInfoAudioCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioCaps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::GetEncoderCapabilities(
    /* [out] */ IMediaCodecInfoEncoderCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEncoderCaps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::GetVideoCapabilities(
    /* [out] */ IMediaCodecInfoVideoCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoCaps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoCodecCapabilities::Dup(
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IMediaCodecInfoCodecProfileLevel*> > array1;
    Arrays::CopyOf(mProfileLevels, mProfileLevels->GetLength(),
            (ArrayOf<IMediaCodecInfoCodecProfileLevel*>**)&array1);
    AutoPtr<ArrayOf<Int32> > array2;
    Arrays::CopyOf(mColorFormats, mColorFormats->GetLength(), (ArrayOf<Int32>**)&array2);

    return CMediaCodecInfoCodecCapabilities::New(
        // clone writable arrays
        array1, array2, IsEncoder(),
        mFlagsVerified, mDefaultFormat, mCapabilitiesInfo, result);
}

ECode CMediaCodecInfoCodecCapabilities::CreateFromProfileLevel(
    /* [in] */ const String& mime,
    /* [in] */ Int32 profile,
    /* [in] */ Int32 level,
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<CMediaCodecInfoCodecProfileLevel> pl;
    CMediaCodecInfoCodecProfileLevel::NewByFriend((CMediaCodecInfoCodecProfileLevel**)&pl);
    pl->mProfile = profile;
    pl->mLevel = level;
    AutoPtr<IMediaFormat> defaultFormat;
    CMediaFormat::New((IMediaFormat**)&defaultFormat);
    defaultFormat->SetString(IMediaFormat::KEY_MIME, mime);

    AutoPtr<ArrayOf<IMediaCodecInfoCodecProfileLevel*> > profLevs =
            ArrayOf<IMediaCodecInfoCodecProfileLevel*>::Alloc(1);
    profLevs->Set(1, pl);
    AutoPtr<ArrayOf<Int32> > colFmts = ArrayOf<Int32>::Alloc(0);
    AutoPtr<IMediaFormat> format;
    CMediaFormat::New((IMediaFormat**)&format);

    AutoPtr<CMediaCodecInfoCodecCapabilities> ret;
    CMediaCodecInfoCodecCapabilities::NewByFriend(
        profLevs, colFmts, TRUE /* encoder */,
        0 /* flags */, defaultFormat, format /* info */,
        (CMediaCodecInfoCodecCapabilities**)&ret);
    if (ret->mError != 0) {
        *result = NULL;
        return NOERROR;
    }
    *result = ret;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > CMediaCodecInfoCodecCapabilities::GetValidFeatures()
{
    if (!IsEncoder()) {
        return decoderFeatures;
    }
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > array =
            ArrayOf<CMediaCodecInfo::Feature*>::Alloc(0);
    return array;
}

Boolean CMediaCodecInfoCodecCapabilities::CheckFeature(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags)
{
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > array = GetValidFeatures();
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*array)[i];
        if (feat->mName.Equals(name)) {
            return (flags & feat->mValue) != 0;
        }
    }
    return FALSE;
}

Boolean CMediaCodecInfoCodecCapabilities::IsAudio()
{
    return mAudioCaps != NULL;
}

Boolean CMediaCodecInfoCodecCapabilities::IsEncoder()
{
    return mEncoderCaps != NULL;
}

Boolean CMediaCodecInfoCodecCapabilities::IsVideo()
{
    return mVideoCaps != NULL;
}

AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > CMediaCodecInfoCodecCapabilities::Init()
{
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > array = ArrayOf<CMediaCodecInfo::Feature*>::Alloc(3);

    AutoPtr<CMediaCodecInfo::Feature> feat =
            new CMediaCodecInfo::Feature(IMediaCodecInfoCodecCapabilities::FEATURE_AdaptivePlayback,
            (1 << 0), TRUE);
    array->Set(0, feat);

    feat = NULL;
    feat = new CMediaCodecInfo::Feature(IMediaCodecInfoCodecCapabilities::FEATURE_SecurePlayback, (1 << 1), FALSE);
    array->Set(1, feat);

    feat = NULL;
    feat = new CMediaCodecInfo::Feature(IMediaCodecInfoCodecCapabilities::FEATURE_TunneledPlayback, (1 << 2), FALSE);
    array->Set(2, feat);

    return array;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
