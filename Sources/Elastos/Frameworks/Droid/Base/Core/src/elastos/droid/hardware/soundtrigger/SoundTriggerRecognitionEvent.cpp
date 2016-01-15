
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerRecognitionEvent.h"
//#include "elastos/droid/media/CAudioFormatBuilder.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Media::IAudioFormatBuilder;
//using Elastos::Droid::Media::CAudioFormatBuilder;
using Elastos::Droid::Hardware::Soundtrigger::ECLSID_CSoundTriggerRecognitionEvent;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(SoundTriggerRecognitionEvent, Object, ISoundTriggerRecognitionEvent, IParcelable)

SoundTriggerRecognitionEvent::SoundTriggerRecognitionEvent()
    : mStatus(0)
    , mSoundModelHandle(0)
    , mCaptureAvailable(FALSE)
    , mCaptureSession(0)
    , mCaptureDelayMs(0)
    , mCapturePreambleMs(0)
    , mTriggerInData(FALSE)
    , mCaptureFormat(NULL)
    , mData(NULL)
{
}
SoundTriggerRecognitionEvent::~SoundTriggerRecognitionEvent()
{
}

ECode SoundTriggerRecognitionEvent::constructor()
{
    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::constructor(
    /* [in] */ Int32 status,
    /* [in] */ Int32 soundModelHandle,
    /* [in] */ Boolean captureAvailable,
    /* [in] */ Int32 captureSession,
    /* [in] */ Int32 captureDelayMs,
    /* [in] */ Int32 capturePreambleMs,
    /* [in] */ Boolean triggerInData,
    /* [in] */ IAudioFormat* captureFormat,
    /* [in] */ ArrayOf<Byte>* data)
{
    mStatus = status;
    mSoundModelHandle = soundModelHandle;
    mCaptureAvailable = captureAvailable;
    mCaptureSession = captureSession;
    mCaptureDelayMs = captureDelayMs;
    mCapturePreambleMs = capturePreambleMs;
    mTriggerInData = triggerInData;
    mCaptureFormat = captureFormat;
    mData = data;
    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerRecognitionEvent** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status;
    FAIL_RETURN(source->ReadInt32(&status))
    Int32 soundModelHandle;
    FAIL_RETURN(source->ReadInt32(&soundModelHandle))
    Boolean captureAvailable;
    FAIL_RETURN(source->ReadBoolean(&captureAvailable))
    Int32 captureSession;
    FAIL_RETURN(source->ReadInt32(&captureSession))
    Int32 captureDelayMs;
    FAIL_RETURN(source->ReadInt32(&captureDelayMs))
    Int32 capturePreambleMs;
    FAIL_RETURN(source->ReadInt32(&capturePreambleMs))
    Boolean triggerInData;
    FAIL_RETURN(source->ReadBoolean(&triggerInData))

    AutoPtr<IAudioFormat> captureFormat;
    Byte tmp;
    FAIL_RETURN(source->ReadByte(&tmp))
    if (tmp == 1) {
        Int32 sampleRate;
        FAIL_RETURN(source->ReadInt32(&sampleRate))
        Int32 encoding;
        FAIL_RETURN(source->ReadInt32(&encoding))
        Int32 channelMask;
        FAIL_RETURN(source->ReadInt32(&channelMask))

        assert(0 && "TODO: CAudioFormatBuilder");
        // AutoPtr<IAudioFormatBuilder> builder;
        // CAudioFormatBuilder::New((IAudioFormatBuilder**)&builder);
        // FAIL_RETURN(builder->SetChannelMask(channelMask))
        // FAIL_RETURN(builder->SetEncoding(encoding))
        // FAIL_RETURN(builder->SetSampleRate(sampleRate))
        // FAIL_RETURN(builder->Build((IAudioFormat**)&captureFormat))
    }

    AutoPtr<ArrayOf<Byte> > data;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&data))

    AutoPtr<SoundTriggerRecognitionEvent> newResult = new SoundTriggerRecognitionEvent();
    FAIL_RETURN(newResult->constructor(status, soundModelHandle, captureAvailable,
            captureSession, captureDelayMs, capturePreambleMs, triggerInData, captureFormat, data))
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mStatus))
    FAIL_RETURN(dest->WriteInt32(mSoundModelHandle))
    FAIL_RETURN(dest->WriteBoolean(mCaptureAvailable))
    FAIL_RETURN(dest->WriteInt32(mCaptureSession))
    FAIL_RETURN(dest->WriteInt32(mCaptureDelayMs))
    FAIL_RETURN(dest->WriteInt32(mCapturePreambleMs))
    FAIL_RETURN(dest->WriteBoolean(mTriggerInData))
    if (mCaptureFormat != NULL) {
        FAIL_RETURN(dest->WriteByte((Byte)1))
        Int32 rate;
        FAIL_RETURN(mCaptureFormat->GetSampleRate(&rate))
        FAIL_RETURN(dest->WriteInt32(rate))
        Int32 encode;
        FAIL_RETURN(mCaptureFormat->GetEncoding(&encode))
        FAIL_RETURN(dest->WriteInt32(encode))
        Int32 mask;
        FAIL_RETURN(mCaptureFormat->GetChannelMask(&mask))
        FAIL_RETURN(dest->WriteInt32(mask))
    } else {
        FAIL_RETURN(dest->WriteByte((Byte)0))
    }
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mData.Get()))

    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mStatus))
    FAIL_RETURN(source->ReadInt32(&mSoundModelHandle))
    FAIL_RETURN(source->ReadBoolean(&mCaptureAvailable))
    FAIL_RETURN(source->ReadInt32(&mCaptureSession))
    FAIL_RETURN(source->ReadInt32(&mCaptureDelayMs))
    FAIL_RETURN(source->ReadInt32(&mCapturePreambleMs))
    FAIL_RETURN(source->ReadBoolean(&mTriggerInData))

    Byte tmp;
    FAIL_RETURN(source->ReadByte(&tmp))
    if (tmp == 1) {
        Int32 rate;
        FAIL_RETURN(source->ReadInt32(&rate))
        Int32 encode;
        FAIL_RETURN(source->ReadInt32(&encode))
        Int32 mask;
        FAIL_RETURN(source->ReadInt32(&mask))

        assert(0 && "TODO: CAudioFormatBuilder");
        // AutoPtr<IAudioFormatBuilder> builder;
        // CAudioFormatBuilder::New((IAudioFormatBuilder**)&builder);
        // FAIL_RETURN(builder->SetChannelMask(mask))
        // FAIL_RETURN(builder->SetEncoding(encode))
        // FAIL_RETURN(builder->SetSampleRate(rate))
        // FAIL_RETURN(builder->Build((IAudioFormat**)&mCaptureFormat))
    }
    else {
        mCaptureFormat = NULL;
    }

    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mData))

    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (THIS_PROBE(IInterface) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    ClassID clsid;
    IObject::Probe(obj)->GetClassID(&clsid);
    if (ECLSID_CSoundTriggerRecognitionEvent != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<SoundTriggerRecognitionEvent> other =
            (SoundTriggerRecognitionEvent*)ISoundTriggerRecognitionEvent::Probe(obj);
    if (mCaptureAvailable != other->mCaptureAvailable) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mCaptureDelayMs != other->mCaptureDelayMs) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mCapturePreambleMs != other->mCapturePreambleMs) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mCaptureSession != other->mCaptureSession) {
        *equal = FALSE;
        return NOERROR;
    }
    if (!Arrays::Equals(mData, other->mData)) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mSoundModelHandle != other->mSoundModelHandle) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mStatus != other->mStatus) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mTriggerInData != other->mTriggerInData) {
        *equal = FALSE;
        return NOERROR;
    }
    Int32 rate1, rate2;
    FAIL_RETURN(mCaptureFormat->GetSampleRate(&rate1))
    FAIL_RETURN(other->mCaptureFormat->GetSampleRate(&rate2))
    if (rate1 != rate2) {
        *equal = FALSE;
        return NOERROR;
    }
    Int32 encode1, encode2;
    FAIL_RETURN(mCaptureFormat->GetEncoding(&encode1))
    FAIL_RETURN(other->mCaptureFormat->GetEncoding(&encode2))
    if (encode1 != encode2) {
        *equal = FALSE;
        return NOERROR;
    }
    Int32 mask1, mask2;
    FAIL_RETURN(mCaptureFormat->GetChannelMask(&mask1))
    FAIL_RETURN(other->mCaptureFormat->GetChannelMask(&mask2))
    if (mask1 != mask2) {
        *equal = FALSE;
        return NOERROR;
    }

    *equal = TRUE;
    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + (mCaptureAvailable ? 1231 : 1237);
    result = prime * result + mCaptureDelayMs;
    result = prime * result + mCapturePreambleMs;
    result = prime * result + mCaptureSession;
    result = prime * result + (mTriggerInData ? 1231 : 1237);
    if (mCaptureFormat != NULL) {
        Int32 rate;
        FAIL_RETURN(mCaptureFormat->GetSampleRate(&rate))
        result = prime * result + rate;
        Int32 encode;
        FAIL_RETURN(mCaptureFormat->GetEncoding(&encode))
        result = prime * result + encode;
        Int32 mask;
        FAIL_RETURN(mCaptureFormat->GetChannelMask(&mask))
        result = prime * result + mask;
    }
    result = prime * result + Arrays::GetHashCode(mData);
    result = prime * result + mSoundModelHandle;
    result = prime * result + mStatus;

    *hashCode = result;
    return NOERROR;
}

ECode SoundTriggerRecognitionEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "RecognitionEvent [status=";
    sb += mStatus;
    sb += ", soundModelHandle=";
    sb += mSoundModelHandle;
    sb += ", captureAvailable=";
    sb += mCaptureAvailable;
    sb += ", captureSession=";
    sb += mCaptureSession;
    sb += ", captureDelayMs=";
    sb += mCaptureDelayMs;
    sb += ", capturePreambleMs=";
    sb += mCapturePreambleMs;
    sb += ", triggerInData=";
    sb += mTriggerInData;
    if (mCaptureFormat == NULL) {
        sb += "";
    }
    else {
        Int32 rate;
        FAIL_RETURN(mCaptureFormat->GetSampleRate(&rate))
        sb += rate;
        Int32 encode;
        FAIL_RETURN(mCaptureFormat->GetEncoding(&encode))
        sb += encode;
        Int32 mask;
        FAIL_RETURN(mCaptureFormat->GetChannelMask(&mask))
        sb += mask;
    }
    sb += ", data=";
    sb += (mData == NULL ? 0 : mData->GetLength());
    sb += "]";

    return sb.ToString(str);
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos