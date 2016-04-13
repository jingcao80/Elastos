#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/media/CMediaCodecInfoEncoderCapabilities.h"
#include "elastos/droid/media/Utils.h"
#include "elastos/droid/utility/CRange.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Utility::CRange;
using Elastos::Droid::Utility::IRange;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CMediaCodecInfo::Feature, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

const AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > CMediaCodecInfoEncoderCapabilities::bitrates = Init();

CAR_INTERFACE_IMPL(CMediaCodecInfoEncoderCapabilities, Object, IMediaCodecInfoEncoderCapabilities)

CAR_OBJECT_IMPL(CMediaCodecInfoEncoderCapabilities)

CMediaCodecInfoEncoderCapabilities::CMediaCodecInfoEncoderCapabilities()
    : mBitControl(0)
{
}

CMediaCodecInfoEncoderCapabilities::~CMediaCodecInfoEncoderCapabilities()
{
}

ECode CMediaCodecInfoEncoderCapabilities::constructor()
{
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::GetQualityRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mQualityRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::GetComplexityRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mComplexityRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::IsBitrateModeSupported(
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    for (Int32 i = 0; i < bitrates->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*bitrates)[i];
        if (mode == feat->mValue) {
            *result = (mBitControl & (1 << mode)) != 0;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::Init(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent)
{
    // no support for complexity or quality yet
    mParent = parent;

    AutoPtr<IInteger32> begin;
    CInteger32::New(0, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(0, (IInteger32**)&end);
    CRange::Create(begin, end, (IRange**)&mComplexityRange);
    CRange::Create(begin, end, (IRange**)&mQualityRange);
    mBitControl = (1 << IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_VBR);

    ApplyLevelLimits();
    ParseFromInfo(info);
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::SetDefaultFormat(
    /* [in] */ IMediaFormat* format)
{
    // don't list trivial quality/complexity as default for now
    AutoPtr<IInterface> upper;
    mQualityRange->GetUpper((IInterface**)&upper);
    AutoPtr<IInterface> lower;
    mQualityRange->GetLower((IInterface**)&lower);
    Boolean b;
    IObject::Probe(upper)->Equals(lower, &b);
    if (!b && mDefaultQuality != NULL) {
        Int32 val;
        mDefaultQuality->GetValue(&val);
        format->SetInt32(IMediaFormat::KEY_QUALITY, val);
    }
    upper = NULL;
    mComplexityRange->GetUpper((IInterface**)&upper);
    lower = NULL;
    mComplexityRange->GetLower((IInterface**)&lower);
    IObject::Probe(upper)->Equals(lower, &b);
    if (!b && mDefaultComplexity != NULL) {
        Int32 val;
        mDefaultComplexity->GetValue(&val);
        format->SetInt32(IMediaFormat::KEY_COMPLEXITY, val);
    }
    // bitrates are listed in order of preference
    for (Int32 i = 0; i < bitrates->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*bitrates)[i];
        if ((mBitControl & (1 << feat->mValue)) != 0) {
            format->SetInt32(IMediaFormat::KEY_BITRATE_MODE, feat->mValue);
            break;
        }
    }
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::SupportsFormat(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IMap> map;
    format->GetMap((IMap**)&map);
    String mime;
    mParent->GetMimeType(&mime);

    AutoPtr<ICharSequence> cs;
    CString::New(IMediaFormat::KEY_BITRATE_MODE, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> mode = IInteger32::Probe(obj);
    if (mode != NULL) {
        Int32 val;
        mode->GetValue(&val);
        Boolean b;
        IsBitrateModeSupported(val, &b);
        if (!b) {
            *result = FALSE;
            return NOERROR;
        }
    }

    cs = NULL;
    CString::New(IMediaFormat::KEY_COMPLEXITY, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> complexity = IInteger32::Probe(obj);
    if (IMediaFormat::MIMETYPE_AUDIO_FLAC.EqualsIgnoreCase(mime)) {
        cs = NULL;
        CString::New(IMediaFormat::KEY_FLAC_COMPRESSION_LEVEL, (ICharSequence**)&cs);
        obj = NULL;
        map->Get(cs, (IInterface**)&obj);
        AutoPtr<IInteger32> flacComplexity = IInteger32::Probe(obj);
        if (complexity == NULL) {
            complexity = flacComplexity;
        }
        else if (flacComplexity != NULL && complexity != flacComplexity) {
            // throw new IllegalArgumentException(
            //         "conflicting values for complexity and " +
            //         "flac-compression-level");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // other audio parameters
    cs = NULL;
    CString::New(IMediaFormat::KEY_PROFILE, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> profile = IInteger32::Probe(obj);

    if (IMediaFormat::MIMETYPE_AUDIO_AAC.EqualsIgnoreCase(mime)) {
        cs = NULL;
        CString::New(IMediaFormat::KEY_AAC_PROFILE, (ICharSequence**)&cs);
        obj = NULL;
        map->Get(cs, (IInterface**)&obj);
        AutoPtr<IInteger32> aacProfile = IInteger32::Probe(obj);
        if (profile == NULL) {
            profile = aacProfile;
        }
        else if (aacProfile != NULL && aacProfile != profile) {
            // throw new IllegalArgumentException(
            //         "conflicting values for profile and aac-profile");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    cs = NULL;
    CString::New(IMediaFormat::KEY_QUALITY, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> quality = IInteger32::Probe(obj);

    *result = Supports(complexity, quality, profile);
    return NOERROR;
}

ECode CMediaCodecInfoEncoderCapabilities::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoEncoderCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaCodecInfoEncoderCapabilities> caps;
    CMediaCodecInfoEncoderCapabilities::New((IMediaCodecInfoEncoderCapabilities**)&caps);
    caps->Init(info, parent);
    *result = caps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Int32 CMediaCodecInfoEncoderCapabilities::ParseBitrateMode(
    /* [in] */ const String& mode)
{
    for (Int32 i = 0; i < bitrates->GetLength(); i++) {
        AutoPtr<CMediaCodecInfo::Feature> feat = (*bitrates)[i];
        if (feat->mName.EqualsIgnoreCase(mode)) {
            return feat->mValue;
        }
    }
    return 0;
}

void CMediaCodecInfoEncoderCapabilities::ApplyLevelLimits()
{
    String mime;
    mParent->GetMimeType(&mime);
    if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_FLAC)) {
        AutoPtr<IInteger32> begin;
        CInteger32::New(0, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(8, (IInteger32**)&end);
        CRange::Create(begin, end, (IRange**)&mComplexityRange);

        mBitControl = (1 << IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_CQ);
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_AMR_NB)
            || mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_AMR_WB)
            || mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_G711_ALAW)
            || mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_G711_MLAW)
            || mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_MSGSM)) {
        mBitControl = (1 << IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_CBR);
    }
}

void CMediaCodecInfoEncoderCapabilities::ParseFromInfo(
    /* [in] */ IMediaFormat* info)
{
    AutoPtr<IMap> map;
    info->GetMap((IMap**)&map);

    Boolean b;
    if (info->ContainsKey(String("complexity-range"), &b), b) {
        String str;
        info->GetString(String("complexity-range"), &str);
        mComplexityRange = Utils::ParseIntRange(CoreUtils::Convert(str), mComplexityRange);
        // TODO should we limit this to level limits?
    }
    if (info->ContainsKey(String("quality-range"), &b), b) {
        String str;
        info->GetString(String("quality-range"), &str);
        mQualityRange = Utils::ParseIntRange(CoreUtils::Convert(str), mQualityRange);
    }
    if (info->ContainsKey(String("feature-bitrate-control"), &b), b) {
        String str;
        info->GetString(String("feature-bitrate-control"), &str);
        AutoPtr<ArrayOf<String> > array;
        StringUtils::Split(str, String(","), (ArrayOf<String>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            mBitControl |= ParseBitrateMode((*array)[i]);
        }
    }

    // try {
    AutoPtr<ICharSequence> cs;
    CString::New(String("complexity-default"), (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    Int32 val;
    StringUtils::Parse(str, &val);
    CInteger32::New(val, (IInteger32**)&mDefaultComplexity);
    // } catch (NumberFormatException e) { }

    // try {
    cs = NULL;
    CString::New(String("quality-default"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    ICharSequence::Probe(obj)->ToString(&str);
    StringUtils::Parse(str, &val);
    CInteger32::New(val, (IInteger32**)&mDefaultQuality);
    // } catch (NumberFormatException e) { }

    cs = NULL;
    CString::New(String("quality-scale"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    ICharSequence::Probe(obj)->ToString(&mQualityScale);
}

Boolean CMediaCodecInfoEncoderCapabilities::Supports(
    /* [in] */ IInteger32* complexity,
    /* [in] */ IInteger32* quality,
    /* [in] */ IInteger32* profile)
{
    Boolean ok = TRUE;
    if (ok && complexity != NULL) {
        mComplexityRange->Contains(complexity, &ok);
    }
    if (ok && quality != NULL) {
        mQualityRange->Contains(quality, &ok);
    }
    if (ok && profile != NULL) {
        AutoPtr<CMediaCodecInfoCodecCapabilities> cc = (CMediaCodecInfoCodecCapabilities*)mParent.Get();
        for (Int32 i = 0; i < cc->mProfileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> pl =
                    (CMediaCodecInfoCodecProfileLevel*)(*cc->mProfileLevels)[i];
            Int32 val;
            profile->GetValue(&val);
            if (pl->mProfile == val) {
                profile = NULL;
                break;
            }
        }
        ok = profile == NULL;
    }
    return ok;
}

AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > CMediaCodecInfoEncoderCapabilities::Init()
{
    AutoPtr<ArrayOf<CMediaCodecInfo::Feature*> > array = ArrayOf<CMediaCodecInfo::Feature*>::Alloc(3);

    AutoPtr<CMediaCodecInfo::Feature> feat =
            new CMediaCodecInfo::Feature(String("VBR"), IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_VBR, TRUE);
    array->Set(0, feat);

    feat = NULL;
    feat = new CMediaCodecInfo::Feature(String("CBR"), IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_CBR, FALSE);
    array->Set(1, feat);

    feat = NULL;
    feat = new CMediaCodecInfo::Feature(String("CQ"),  IMediaCodecInfoEncoderCapabilities::BITRATE_MODE_CQ,  FALSE);
    array->Set(2, feat);

    return array;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
