#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/media/CMediaCodecInfoAudioCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/Utils.h"
#include "elastos/droid/utility/CRange.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CRange;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaCodecInfoAudioCapabilities::TAG("AudioCapabilities");

const Int32 CMediaCodecInfoAudioCapabilities::MAX_INPUT_CHANNEL_COUNT = 30;

CAR_INTERFACE_IMPL(CMediaCodecInfoAudioCapabilities, Object, IMediaCodecInfoAudioCapabilities)

CAR_OBJECT_IMPL(CMediaCodecInfoAudioCapabilities)

CMediaCodecInfoAudioCapabilities::CMediaCodecInfoAudioCapabilities()
    : mMaxInputChannelCount(0)
{
}

CMediaCodecInfoAudioCapabilities::~CMediaCodecInfoAudioCapabilities()
{
}

ECode CMediaCodecInfoAudioCapabilities::constructor()
{
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::GetBitrateRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBitrateRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::GetSupportedSampleRates(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    return Arrays::CopyOf(mSampleRates,
            mSampleRates->GetLength(), result);
}

ECode CMediaCodecInfoAudioCapabilities::GetSupportedSampleRateRanges(
    /* [out, callee] */ ArrayOf<IRange*>** result)
{
    VALIDATE_NOT_NULL(result)
    return Arrays::CopyOf(mSampleRateRanges,
            mSampleRateRanges->GetLength(), result);
}

ECode CMediaCodecInfoAudioCapabilities::GetMaxInputChannelCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxInputChannelCount;
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::Init(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent)
{
    mParent = parent;
    InitWithPlatformLimits();
    ApplyLevelLimits();
    ParseFromInfo(info);
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::IsSampleRateSupported(
    /* [in] */ Int32 sampleRate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> val;
    CInteger32::New(sampleRate, (IInteger32**)&val);
    *result = Supports(val, NULL);
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::SetDefaultFormat(
    /* [in] */ IMediaFormat* format)
{
    // report settings that have only a single choice
    AutoPtr<IInterface> upper;
    mBitrateRange->GetUpper((IInterface**)&upper);
    AutoPtr<IInterface> lower;
    mBitrateRange->GetLower((IInterface**)&lower);
    Boolean b;
    IObject::Probe(lower)->Equals(upper, &b);
    if (b) {
        Int32 val;
        IInteger32::Probe(lower)->GetValue(&val);
        format->SetInt32(IMediaFormat::KEY_BIT_RATE, val);
    }
    if (mMaxInputChannelCount == 1) {
        // mono-only format
        format->SetInt32(IMediaFormat::KEY_CHANNEL_COUNT, 1);
    }
    if (mSampleRates != NULL && mSampleRates->GetLength() == 1) {
        format->SetInt32(IMediaFormat::KEY_SAMPLE_RATE, (*mSampleRates)[0]);
    }
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::SupportsFormat(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map;
    format->GetMap((IMap**)&map);
    AutoPtr<ICharSequence> cs;
    CString::New(IMediaFormat::KEY_SAMPLE_RATE, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> sampleRate = IInteger32::Probe(obj);
    cs = NULL;
    CString::New(IMediaFormat::KEY_CHANNEL_COUNT, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> channels = IInteger32::Probe(obj);
    if (!Supports(sampleRate, channels)) {
        *result = FALSE;
        return NOERROR;
    }

    // nothing to do for:
    // KEY_CHANNEL_MASK: codecs don't get this
    // KEY_IS_ADTS:      required feature for all AAC decoders
    *result = TRUE;
    return NOERROR;
}

ECode CMediaCodecInfoAudioCapabilities::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoAudioCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaCodecInfoAudioCapabilities> caps;
    CMediaCodecInfoAudioCapabilities::New((IMediaCodecInfoAudioCapabilities**)&caps);
    caps->Init(info, parent);
    *result = caps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CMediaCodecInfoAudioCapabilities::InitWithPlatformLimits()
{
    AutoPtr<IInteger32> begin;
    CInteger32::New(0, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(Elastos::Core::Math::INT32_MAX_VALUE, (IInteger32**)&end);
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&mBitrateRange);

    mMaxInputChannelCount = MAX_INPUT_CHANNEL_COUNT;
    begin = NULL;
    CInteger32::New(8000, (IInteger32**)&begin);
    end = NULL;
    CInteger32::New(96000, (IInteger32**)&end);
    AutoPtr<IRange> r;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&r);
    mSampleRateRanges = ArrayOf<IRange*>::Alloc(1);
    mSampleRateRanges->Set(0, r);

    mSampleRates = NULL;
}

Boolean CMediaCodecInfoAudioCapabilities::Supports(
    /* [in] */ IInteger32* sampleRate,
    /* [in] */ IInteger32* inputChannels)
{
    // channels and sample rates are checked orthogonally
    if (inputChannels != NULL) {
        Int32 val;
        inputChannels->GetValue(&val);
        if (val < 1 || val > mMaxInputChannelCount) {
            return FALSE;
        }
    }
    if (sampleRate != NULL) {
        Int32 ix;
        ix = Utils::BinarySearchDistinctRanges(mSampleRateRanges, sampleRate);
        if (ix < 0) {
            return FALSE;
        }
    }
    return TRUE;
}

void CMediaCodecInfoAudioCapabilities::LimitSampleRates(
    /* [in] */ ArrayOf<Int32>* rates)
{
    Arrays::Sort(rates);
    AutoPtr<IArrayList> ranges;
    CArrayList::New((IArrayList**)&ranges);
    for (Int32 i = 0; i < rates->GetLength(); i++) {
        Int32 rate = (*rates)[i];
        AutoPtr<IInteger32> val;
        CInteger32::New(rate, (IInteger32**)&val);
        if (Supports(val, NULL /* channels */)) {
            AutoPtr<IRange> r;
            CRange::Create(EIID_IInteger32, val, val, (IRange**)&r);
            ranges->Add(r);
        }
    }
    AutoPtr<ArrayOf<IInterface*> > array;
    ranges->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        mSampleRateRanges->Set(i, IRange::Probe((*array)[i]));
    }
    CreateDiscreteSampleRates();
}

void CMediaCodecInfoAudioCapabilities::CreateDiscreteSampleRates()
{
    mSampleRates = ArrayOf<Int32>::Alloc(mSampleRateRanges->GetLength());
    for (Int32 i = 0; i < mSampleRateRanges->GetLength(); i++) {
        Int32 val;

        AutoPtr<IInterface> lower;
        (*mSampleRateRanges)[i]->GetLower((IInterface**)&lower);
        IInteger32::Probe(lower)->GetValue(&val);
        mSampleRates->Set(i, val);
    }
}

void CMediaCodecInfoAudioCapabilities::LimitSampleRates(
    /* [in] */ ArrayOf<IRange*>* rateRanges)
{
    Utils::SortDistinctRanges(rateRanges);
    mSampleRateRanges = Utils::IntersectSortedDistinctRanges(mSampleRateRanges, rateRanges);

    // check if all values are discrete
    for (Int32 i = 0; i < mSampleRateRanges->GetLength(); i++) {
        AutoPtr<IRange> range = (*mSampleRateRanges)[i];

        AutoPtr<IInterface> upper;
        range->GetUpper((IInterface**)&upper);
        AutoPtr<IInterface> lower;
        range->GetLower((IInterface**)&lower);
        Boolean b;
        IObject::Probe(lower)->Equals(upper, &b);
        if (!b) {
            mSampleRates = NULL;
            return;
        }
    }
    CreateDiscreteSampleRates();
}

void CMediaCodecInfoAudioCapabilities::ApplyLevelLimits()
{
    AutoPtr<ArrayOf<Int32> > sampleRates;
    AutoPtr<IRange> sampleRateRange;
    AutoPtr<IRange> bitRates;
    Int32 maxChannels = 0;
    String mime;
    mParent->GetMimeType(&mime);

    if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_MPEG)) {
        sampleRates = ArrayOf<Int32>::Alloc(9);
        sampleRates->Set(0, 8000);
        sampleRates->Set(1, 11025);
        sampleRates->Set(2, 12000);
        sampleRates->Set(3, 16000);
        sampleRates->Set(4, 22050);
        sampleRates->Set(5, 24000);
        sampleRates->Set(6, 32000);
        sampleRates->Set(7, 44100);
        sampleRates->Set(8, 48000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(8000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(320000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 2;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_AMR_NB)) {
        sampleRates = ArrayOf<Int32>::Alloc(1);
        sampleRates->Set(0, 8000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(4750, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(12200, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 1;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_AMR_WB)) {
        sampleRates = ArrayOf<Int32>::Alloc(1);
        sampleRates->Set(0, 16000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(6600, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(23850, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 1;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_AAC)) {
        sampleRates = ArrayOf<Int32>::Alloc(13);
        sampleRates->Set(0, 7350);
        sampleRates->Set(1, 8000);
        sampleRates->Set(2, 11025);
        sampleRates->Set(3, 12000);
        sampleRates->Set(4, 16000);
        sampleRates->Set(5, 22050);
        sampleRates->Set(6, 24000);
        sampleRates->Set(7, 32000);
        sampleRates->Set(8, 44100);
        sampleRates->Set(9, 48000);
        sampleRates->Set(10, 64000);
        sampleRates->Set(11, 88200);
        sampleRates->Set(12, 96000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(8000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(510000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 48;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_VORBIS)) {
        sampleRates = ArrayOf<Int32>::Alloc(6);
        sampleRates->Set(0, 8000);
        sampleRates->Set(1, 12000);
        sampleRates->Set(2, 16000);
        sampleRates->Set(3, 24000);
        sampleRates->Set(4, 48000);
        sampleRates->Set(5, 192000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(32000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(500000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 255;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_OPUS)) {
        sampleRates = ArrayOf<Int32>::Alloc(5);
        sampleRates->Set(0, 8000);
        sampleRates->Set(1, 12000);
        sampleRates->Set(2, 16000);
        sampleRates->Set(3, 24000);
        sampleRates->Set(4, 48000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(6000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(510000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 255;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_RAW)) {
        AutoPtr<IInteger32> begin;
        CInteger32::New(1, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(96000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&sampleRateRange);

        end = NULL;
        CInteger32::New(10000000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 8;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_FLAC)) {
        AutoPtr<IInteger32> begin;
        CInteger32::New(1, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(655350, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&sampleRateRange);

        // lossless codec, so bitrate is ignored
        maxChannels = 255;
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_G711_ALAW)
            || mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_G711_MLAW)) {
        sampleRates = ArrayOf<Int32>::Alloc(1);
        sampleRates->Set(0, 8000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(64000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(64000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);
        // platform allows multiple channels for this format
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_AUDIO_MSGSM)) {
        sampleRates = ArrayOf<Int32>::Alloc(1);
        sampleRates->Set(0, 8000);

        AutoPtr<IInteger32> begin;
        CInteger32::New(13000, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(13000, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&bitRates);

        maxChannels = 1;
    }
    else {
        Logger::W(TAG, "Unsupported mime %s", mime.string());
        ((CMediaCodecInfoCodecCapabilities*)mParent.Get())->mError |= CMediaCodecInfo::ERROR_UNSUPPORTED;
    }

    // restrict ranges
    if (sampleRates != NULL) {
        LimitSampleRates(sampleRates);
    }
    else if (sampleRateRange != NULL) {
        AutoPtr<ArrayOf<IRange*> > array =
                ArrayOf<IRange*>::Alloc(1);
        array->Set(0, sampleRateRange);
        LimitSampleRates(array);
    }
    ApplyLimits(maxChannels, bitRates);
}

void CMediaCodecInfoAudioCapabilities::ApplyLimits(
    /* [in] */ Int32 maxInputChannels,
    /* [in] */ IRange* bitRates)
{
    AutoPtr<IInteger32> begin;
    CInteger32::New(1, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(mMaxInputChannelCount, (IInteger32**)&end);
    AutoPtr<IRange> r;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&r);
    AutoPtr<IInteger32> val;
    CInteger32::New(maxInputChannels, (IInteger32**)&val);
    AutoPtr<IInterface> obj;
    r->Clamp(val, (IInterface**)&obj);
    IInteger32::Probe(obj)->GetValue(&mMaxInputChannelCount);
    if (bitRates != NULL) {
        mBitrateRange->Intersect(bitRates, (IRange**)&mBitrateRange);
    }
}

void CMediaCodecInfoAudioCapabilities::ParseFromInfo(
    /* [in] */ IMediaFormat* info)
{
    Int32 maxInputChannels = MAX_INPUT_CHANNEL_COUNT;
    AutoPtr<IRange> bitRates = CMediaCodecInfo::POSITIVE_INTEGERS;

    Boolean b;
    if (info->ContainsKey(String("sample-rate-ranges"), &b), b) {
        String str;
        info->GetString(String("sample-rate-ranges"), &str);
        AutoPtr<ArrayOf<String> > rateStrings;
        StringUtils::Split(str, String(","), (ArrayOf<String>**)&rateStrings);

        AutoPtr<ArrayOf<IRange*> > rateRanges =
                ArrayOf<IRange*>::Alloc(rateStrings->GetLength());
        for (Int32 i = 0; i < rateStrings->GetLength(); i++) {
            AutoPtr<ICharSequence> cs;
            CString::New((*rateStrings)[i], (ICharSequence**)&cs);
            AutoPtr<IRange> r = Utils::ParseIntRange(cs, (IRange*)NULL);
            rateRanges->Set(i, r);
        }
        LimitSampleRates(rateRanges);
    }
    if (info->ContainsKey(String("max-channel-count"), &b), b) {
        String str;
        info->GetString(String("max-channel-count"), &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        maxInputChannels = Utils::ParseIntSafely(cs, maxInputChannels);
    }
    if (info->ContainsKey(String("bitrate-range"), &b), b) {
        String str;
        info->GetString(String("bitrate"), &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        AutoPtr<IRange> range;
        range = Utils::ParseIntRange(cs, bitRates);
        bitRates->Intersect(range, (IRange**)&bitRates);
    }
    ApplyLimits(maxInputChannels, bitRates);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
