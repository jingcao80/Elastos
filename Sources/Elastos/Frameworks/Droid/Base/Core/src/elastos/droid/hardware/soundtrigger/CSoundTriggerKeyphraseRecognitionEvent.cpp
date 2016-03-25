
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseRecognitionEvent.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(CSoundTriggerKeyphraseRecognitionEvent, SoundTriggerRecognitionEvent,
        ISoundTriggerKeyphraseRecognitionEvent)

CAR_OBJECT_IMPL(CSoundTriggerKeyphraseRecognitionEvent)

CSoundTriggerKeyphraseRecognitionEvent::CSoundTriggerKeyphraseRecognitionEvent()
    : mKeyphraseExtras(NULL)
{
}

ECode CSoundTriggerKeyphraseRecognitionEvent::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::constructor(
    /* [in] */ Int32 status,
    /* [in] */ Int32 soundModelHandle,
    /* [in] */ Boolean captureAvailable,
    /* [in] */ Int32 captureSession,
    /* [in] */ Int32 captureDelayMs,
    /* [in] */ Int32 capturePreambleMs,
    /* [in] */ Boolean triggerInData,
    /* [in] */ IAudioFormat* captureFormat,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>* keyphraseExtras)
{
    FAIL_RETURN(SoundTriggerRecognitionEvent::constructor(status, soundModelHandle, captureAvailable, captureSession, captureDelayMs,
          capturePreambleMs, triggerInData, captureFormat, data))
    mKeyphraseExtras = keyphraseExtras;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerKeyphraseRecognitionEvent** result)
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

    // KeyphraseRecognitionExtra[] keyphraseExtras =
    //         in.createTypedArray(KeyphraseRecognitionExtra.CREATOR);
    assert(0 && "TODO: KeyphraseRecognitionExtra.CREATOR");
    AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > keyphraseExtras;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&keyphraseExtras))

    AutoPtr<CSoundTriggerKeyphraseRecognitionEvent> newResult;
    CSoundTriggerKeyphraseRecognitionEvent::NewByFriend(status, soundModelHandle, captureAvailable,
                    captureSession, captureDelayMs, capturePreambleMs, triggerInData,
                    captureFormat, data, keyphraseExtras, (CSoundTriggerKeyphraseRecognitionEvent**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::WriteToParcel(
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

    //dest.writeTypedArray(keyphraseExtras, flags);
    assert(0 && "TODO: writeTypedArray");
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mKeyphraseExtras.Get()))
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::ReadFromParcel(
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
        // FAIL_RETURN(builder->Build((IAudioFormat**)&mCaptureFormat))
    }
    else {
        mCaptureFormat = NULL;
    }

    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mData))
    // KeyphraseRecognitionExtra[] keyphraseExtras =
    //         in.createTypedArray(KeyphraseRecognitionExtra.CREATOR);
    assert(0 && "TODO: KeyphraseRecognitionExtra.CREATOR");
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mKeyphraseExtras))
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    ClassID clsid;
    IObject::Probe(obj)->GetClassID(&clsid);
    if (ECLSID_CSoundTriggerKeyphraseRecognitionEvent != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<CSoundTriggerKeyphraseRecognitionEvent> other = (CSoundTriggerKeyphraseRecognitionEvent*)ISoundTriggerKeyphraseRecognitionEvent::Probe(obj);
    if (!Arrays::Equals(mKeyphraseExtras, other->mKeyphraseExtras)) {
        *equal = FALSE;
        return NOERROR;
    }
    *equal = TRUE;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    const Int32 prime = 31;
    Int32 result ;
    FAIL_RETURN(SoundTriggerRecognitionEvent::GetHashCode(&result))
    result = prime * result + Arrays::GetHashCode(mKeyphraseExtras);
    *hashCode = result;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "KeyphraseRecognitionEvent [keyphraseExtras=";
    sb += Arrays::ToString(mKeyphraseExtras);
    sb += ", status=";
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
        sb += ", sampleRate=";
        Int32 rate;
        FAIL_RETURN(mCaptureFormat->GetSampleRate(&rate))
        sb += rate;
        sb += ", encoding=";
        Int32 encoding;
        FAIL_RETURN(mCaptureFormat->GetEncoding(&encoding))
        sb += encoding;
        sb += ", channelMask=";
        Int32 mask;
        FAIL_RETURN(mCaptureFormat->GetChannelMask(&mask))
        sb += mask;
    }
    sb += ", data=";
    sb += (mData == NULL ? 0 : mData->GetLength());
    sb += sb += "]";

    return sb.ToString(str);
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
