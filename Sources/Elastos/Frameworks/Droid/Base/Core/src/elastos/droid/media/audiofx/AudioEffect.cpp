#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"
#include "elastos/droid/media/audiofx/CAudioEffectDescriptor.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

#include <media/AudioEffect.h>

#include <elastos.h>
#include <cutils/log.h>
#include <utils/Errors.h>
#include <stddef.h>
#include <stdint.h>

using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Media::Audiofx::CAudioEffectDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String AudioEffect::TAG("AudioEffect-JAVA");

CAR_INTERFACE_IMPL(AudioEffect, Object, IAudioEffect)

AudioEffect::NativeEventHandler::NativeEventHandler(
    /* [in] */ AudioEffect* ae,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mAudioEffect(ae)
{
}

ECode AudioEffect::NativeEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mAudioEffect == NULL) {
        return NOERROR;
    }

    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    AutoPtr<IAudioEffectOnEnableStatusChangeListener> enableStatusChangeListener;
    AutoPtr<IAudioEffectOnControlStatusChangeListener> controlStatusChangeListener;
    AutoPtr<IAudioEffectOnParameterChangeListener> parameterChangeListener;

    Object& lock = mAudioEffect->mListenerLock;
    switch (what) {
        case IAudioEffect::NATIVE_EVENT_ENABLED_STATUS:
            {
                synchronized(lock);
                enableStatusChangeListener = mAudioEffect->mEnableStatusChangeListener;
            }
            if (enableStatusChangeListener != NULL) {
                enableStatusChangeListener->OnEnableStatusChange((IAudioEffect*)mAudioEffect->Probe(EIID_IAudioEffect), (arg1 != 0));
            }
            break;
        case IAudioEffect::NATIVE_EVENT_CONTROL_STATUS:
            {
                synchronized(lock);
                controlStatusChangeListener = mAudioEffect->mControlChangeStatusListener;
            }
            if (controlStatusChangeListener != NULL) {
                controlStatusChangeListener->OnControlStatusChange((IAudioEffect*)mAudioEffect->Probe(EIID_IAudioEffect), (arg1 != 0));
            }
            break;
        case IAudioEffect::NATIVE_EVENT_PARAMETER_CHANGED:
            {
                synchronized(lock);
                parameterChangeListener = mAudioEffect->mParameterChangeListener;
            }
            if (parameterChangeListener != NULL) {
                // arg1 contains offset of parameter value from start of
                // byte array
                Int32 vOffset = arg1;
                // AutoPtr<IObjectContainer> obj;
                // AutoPtr<ArrayOf<Byte> > p;
                // msg->GetObj((IInterface**)&obj);
                // Int32 count;
                // obj->GetObjectCount(&count);
                // p = ArrayOf<Byte>::Alloc(count);
                // AutoPtr<IObjectEnumerator> enumerator;
                // obj->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                // Boolean hasNext = FALSE;
                // Int32 i = 0;
                // while(enumerator->MoveNext(&hasNext), hasNext) {
                //     AutoPtr<IByte> ib;
                //     enumerator->Current((IInterface**)&ib);
                //     Byte b;
                //     ib->GetValue(&b);
                //     p->Set(i, b);
                //     i++;
                // }
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
                Int32 count;
                array->GetLength(&count);
                AutoPtr<ArrayOf<Byte> > p;
                p = ArrayOf<Byte>::Alloc(count);
                for(Int32 i = 0; i < count; i++) {
                    obj = NULL;
                    array->Get(i, (IInterface**)&obj);
                    AutoPtr<IByte> ib = IByte::Probe(obj);
                    Byte b;
                    ib->GetValue(&b);
                    p->Set(i, b);
                }

                // See effect_param_t in EffectApi.h for psize and vsize
                // fields offsets
                Int32 status, psize, vsize;
                mAudioEffect->ByteArrayToInt32(p, 0, &status);
                mAudioEffect->ByteArrayToInt32(p, 4, &psize);
                mAudioEffect->ByteArrayToInt32(p, 8, &vsize);
                AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(psize);
                AutoPtr<ArrayOf<Byte> > value = ArrayOf<Byte>::Alloc(vsize);
                // System.arraycopy(p, 12, param, 0, psize);
                param->Copy(p, 12, psize);
                // System.arraycopy(p, vOffset, value, 0, vsize);
                value->Copy(p, vOffset, vsize);

                parameterChangeListener->OnParameterChange((IAudioEffect*)mAudioEffect, status, param, value);
            }
        default:
            // Logger::E(TAG, "handleMessage() Unknown event type: " + msg.what);
            break;

    }
    return NOERROR;
}

AudioEffect::AudioEffect()
    : mState(IAudioEffect::STATE_UNINITIALIZED)
{
}

AudioEffect::~AudioEffect()
{
    NativeFinalize();
}

ECode AudioEffect::constructor()
{
    return NOERROR;
}

ECode AudioEffect::constructor(
    /* [in] */ IUUID* type,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    VALIDATE_NOT_NULL(type);
    VALIDATE_NOT_NULL(uuid);
    AutoPtr<ArrayOf<Int32> > id = ArrayOf<Int32>::Alloc(1);
    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > desc = ArrayOf<IAudioEffectDescriptor*>::Alloc(1);
    // native initialization
    Int32 initResult = 0;
    String typeStr,uuidStr;
    type->ToString(&typeStr);
    uuid->ToString(&uuidStr);
    initResult = NativeSetup(typeStr, uuidStr, priority,audioSession,(ArrayOf<Int32>*) (id),(ArrayOf<IAudioEffectDescriptor*>*)(desc));
    if (initResult != IAudioEffect::SUCCESS && initResult != IAudioEffect::ALREADY_EXISTS) {
        /*
            Log.e(TAG, "Error code " + initResult
                    + " when initializing AudioEffect.");
        */
        switch (initResult) {
        case IAudioEffect::ERROR_BAD_VALUE:
           // throw (new IllegalArgumentException("Effect type: " + type
           //         + " not supported."));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case IAudioEffect::ERROR_INVALID_OPERATION:
           // throw (new UnsupportedOperationException(
           //         "Effect library not loaded"));
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        default:
           // throw (new RuntimeException(
           //             "Cannot initialize effect engine for type: " + type
           //                     + "Error: " + initResult));
            return E_RUNTIME_EXCEPTION;
        }
    }
    mId = (*id)[0];
    mDescriptor = (*desc)[0];

    synchronized(mStateLock);
    mState = IAudioEffect::STATE_INITIALIZED;
    return NOERROR;
}

ECode AudioEffect::ReleaseResources()
{
    synchronized(mStateLock);
    NativeRelease();
    mState = IAudioEffect::STATE_UNINITIALIZED;
    return NOERROR;
}

ECode AudioEffect::GetDescriptor(
    /* [out] */ IAudioEffectDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);

    FAIL_RETURN(CheckState(String("GetDescriptor()")));
    *descriptor = mDescriptor;
    REFCOUNT_ADD(*descriptor);

    return NOERROR;
}

ECode AudioEffect::QueryEffects(
    /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);

    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > temp = NativeQueryEffects();
    *descriptor = temp;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

ECode AudioEffect::QueryPreProcessings(
    /* [in] */ Int32 audioSession,
    /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);

    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > temp = NativeQueryPreProcessing(audioSession);
    *descriptor = temp;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

ECode AudioEffect::IsEffectTypeAvailable(
    /* [in] */ IUUID* type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > desc;
    QueryEffects((ArrayOf<IAudioEffectDescriptor*>**)&desc);

    for (Int32 i = 0; i < desc->GetLength(); i++) {
        AutoPtr<IUUID> type;
        (*desc)[i]->GetType((IUUID**)&type);
        Boolean b;
        if ((type->Equals(type, &b)), b) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode AudioEffect::SetEnabled(
    /* [in]  */ Boolean enabled,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckState(String("SetEnabled()")));
    *result = NativeSetEnabled(enabled);
    return NOERROR;
}

ECode AudioEffect::SetParameter(
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckState(String("SetParameter()")));
    *result = NativeSetParameter(param->GetLength(), param, value->GetLength(), value);
    return NOERROR;
}

ECode AudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param, (ArrayOf<Byte>**)&p);
    AutoPtr< ArrayOf<Byte> > v;
    Int32ToByteArray(value, (ArrayOf<Byte>**)&v);
    return SetParameter(p.Get(), v.Get(), result);
}

ECode AudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ Int16 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param, (ArrayOf<Byte>**)&p);
    AutoPtr< ArrayOf<Byte> > v;
    Int16ToByteArray(value, (ArrayOf<Byte>**)&v);
    return SetParameter(p.Get(), v.Get(), result);
}

ECode AudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param, (ArrayOf<Byte>**)&p);
    return SetParameter(p.Get(), value, result);
}

ECode AudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    if (param->GetLength() > 2 || value->GetLength() > 2) {
        *result = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1],(ArrayOf<Byte>**)&p2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }
    AutoPtr< ArrayOf<Byte> > v;
    Int32ToByteArray((*value)[0],(ArrayOf<Byte>**)&v);
    if (value->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > v2;
        Int32ToByteArray((*value)[1],(ArrayOf<Byte>**)&v2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(v, v2, (ArrayOf<Byte>**)&tempArrayByte);
        v = tempArrayByte;
    }
    return SetParameter(p.Get(), v.Get(), result);
}

ECode AudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    if (param->GetLength() > 2 || value->GetLength() > 2) {
        *result = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1],(ArrayOf<Byte>**)&p2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }
    AutoPtr< ArrayOf<Byte> > v;
    Int16ToByteArray((*value)[0],(ArrayOf<Byte>**)&v);
    if (value->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > v2;
        Int16ToByteArray((*value)[1],(ArrayOf<Byte>**)&v2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(v, v2, (ArrayOf<Byte>**)&tempArrayByte);
        v = tempArrayByte;
    }
    return SetParameter(p.Get(), v.Get(), result);
}

ECode AudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    if (param->GetLength()> 2) {
        *result = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1],(ArrayOf<Byte>**)&p2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }
    return SetParameter(p.Get(), value, result);
}

ECode AudioEffect::GetParameter(
    /* [in] */ ArrayOf<Byte>* param,
    /* [out] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    FAIL_RETURN(CheckState(String("GetParameter()")));
    *status = NativeGetParameter(param->GetLength(), param, value->GetLength(), value);
    return NOERROR;
}

ECode AudioEffect::GetParameter(
    /* [in] */ const Int32 param,
    /* [out] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param,(ArrayOf<Byte>**)&p);
    return GetParameter(p.Get(), value, status);
}

ECode AudioEffect::GetParameter(
    /* [in] */ Int32 param,
    /* [out] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    if ((*value).GetLength() > 2) {
        *status = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param,(ArrayOf<Byte>**)&p);

    AutoPtr<ArrayOf<Byte> > v = ArrayOf<Byte>::Alloc((*value).GetLength() * 4);

    GetParameter(p.Get(), v, status);

    if (*status == 4 || *status == 8){
        Int32 tempInt32;
        ByteArrayToInt32(v, &tempInt32);
        (*value)[0] = tempInt32;
            if (*status == 8)
            {
                ByteArrayToInt32(v, 4, &tempInt32);
                (*value)[1] = tempInt32;
            }
        *status /= 4;
    }
    else{
        *status = IAudioEffect::ERROR;
    }

    return NOERROR;
}

ECode AudioEffect::GetParameter(
    /* [in] */ Int32 param,
    /* [out] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    if ((*value).GetLength() > 2) {
        *status = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }

    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray(param,(ArrayOf<Byte>**)&p);

    AutoPtr<ArrayOf<Byte> > v = ArrayOf<Byte>::Alloc((*value).GetLength() * 2);

    GetParameter(p.Get(), v, status);

    if (*status == 2 || *status == 4) {
        Int16 tempInt16;
        ByteArrayToInt16(v, &tempInt16);
        (*value)[0] = tempInt16;
        if (*status == 4) {
            ByteArrayToInt16(v, 2, &tempInt16);
            (*value)[1] = tempInt16;
        }
        *status /= 2;
    }
    else {
        *status = IAudioEffect::ERROR;
    }
    return NOERROR;
}

ECode AudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [out] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    if (param->GetLength() > 2 || (*value).GetLength() > 2) {
        *status = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1],(ArrayOf<Byte>**)&p2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }
    AutoPtr<ArrayOf<Byte> > v = ArrayOf<Byte>::Alloc((*value).GetLength() * 4);

    GetParameter(p.Get(), v, status);

    if (*status == 4 || *status == 8) {
        Int32 tempInt32;
        ByteArrayToInt32(v, &tempInt32);
        (*value)[0] = tempInt32;
        if (*status == 8) {
            ByteArrayToInt32(v, 4, &tempInt32);
            (*value)[1] = tempInt32;
        }
        *status /= 4;
    }
    else {
        *status = IAudioEffect::ERROR;
    }
    return NOERROR;
}

ECode AudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [out] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    if (param->GetLength() > 2 || (*value).GetLength() > 2) {
        *status = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1],(ArrayOf<Byte>**)&p2);
        AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }
    AutoPtr<ArrayOf<Byte> > v = ArrayOf<Byte>::Alloc((*value).GetLength() * 2);

    GetParameter(p.Get(), v, status);

    if (*status == 2 || *status == 4) {
        Int16 tempInt16;
        ByteArrayToInt16(v, &tempInt16);
        (*value)[0] = tempInt16;
        if (*status == 4) {
            ByteArrayToInt16(v, 2, &tempInt16);
            (*value)[1] = tempInt16;
        }
        *status /= 2;
    }
    else {
        *status = IAudioEffect::ERROR;
    }
    return NOERROR;
}

ECode AudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(param);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(status);

    if (param->GetLength() > 2) {
        *status = IAudioEffect::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > p;
    Int32ToByteArray((*param)[0],(ArrayOf<Byte>**)&p);
    if (param->GetLength() > 1) {
        AutoPtr< ArrayOf<Byte> > p2;
        Int32ToByteArray((*param)[1], (ArrayOf<Byte>**)&p2);
         AutoPtr< ArrayOf<Byte> > tempArrayByte;
        ConcatArrays(p, p2, (ArrayOf<Byte>**)&tempArrayByte);
        p = tempArrayByte;
    }

    return GetParameter(p.Get(), value, status);
}

ECode AudioEffect::Command(
    /* [in] */ Int32 cmdCode,
    /* [in] */ ArrayOf<Byte>* command,
    /* [in] */ ArrayOf<Byte>* reply,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(reply);
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckState(String("Command()")));
    *result = NativeCommand(cmdCode, command->GetLength(), command, reply->GetLength(), reply);
    return NOERROR;
}

ECode AudioEffect::GetId(
    /* [out] */ Int32* Id)
{
    VALIDATE_NOT_NULL(Id);

    FAIL_RETURN(CheckState(String("GetId()")));
    *Id = mId;
    return NOERROR;
}

ECode AudioEffect::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    FAIL_RETURN(CheckState(String("GetEnabled()")));
    *enabled = NativeGetEnabled();
    return NOERROR;
}

ECode AudioEffect::HasControl(
    /* [out] */ Boolean* control)
{
    VALIDATE_NOT_NULL(control);
    FAIL_RETURN(CheckState(String("HasControl()")));
    *control = NativeHasControl();
    return NOERROR;
}

ECode AudioEffect::SetEnableStatusListener(
    /* [in] */ IAudioEffectOnEnableStatusChangeListener* listener)
{
    synchronized(mListenerLock);
    mEnableStatusChangeListener = listener;
    if ((listener != NULL) && (mNativeEventHandler == NULL)) {
        CreateNativeEventHandler();
    }
    return NOERROR;
}

ECode AudioEffect::SetControlStatusListener(
    /* [in] */ IAudioEffectOnControlStatusChangeListener* listener)
{
    synchronized(mListenerLock);
    mControlChangeStatusListener = listener;
    if ((listener != NULL) && (mNativeEventHandler == NULL)) {
        CreateNativeEventHandler();
    }
    return NOERROR;
}

ECode AudioEffect::SetParameterListener(
    /* [in] */ IAudioEffectOnParameterChangeListener* listener)
{
    synchronized(mListenerLock);
    mParameterChangeListener = listener;
    if ((listener != NULL) && (mNativeEventHandler == NULL)) {
        CreateNativeEventHandler();
    }
    return NOERROR;
}

void AudioEffect::CreateNativeEventHandler()
{
    AutoPtr<ILooper> myLooper;
    AutoPtr<ILooper> mainLooper;
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->GetMyLooper((ILooper**)&myLooper);
    helper->GetMainLooper((ILooper**)&mainLooper);
    if (myLooper != NULL) {
        mNativeEventHandler = new NativeEventHandler(this, myLooper);
    } else if (mainLooper != NULL) {
        mNativeEventHandler = new NativeEventHandler(this, mainLooper);
    } else {
        mNativeEventHandler = NULL;
    }
}

void AudioEffect::PostEventFromNative(
    /* [in] */ AudioEffect* effect_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AudioEffect* effect = effect_ref;
    if (effect == NULL) {
        return;
    }
    if (effect->mNativeEventHandler != NULL) {
        AutoPtr<IHandler> handler = effect->mNativeEventHandler;

        AutoPtr<IMessage> msg;
        handler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
        Boolean result;
        handler->SendMessage(msg, &result);
    }
}

struct effect_callback_cookie {
    //jclass      audioEffect_class;  // AudioEffect class
    //jobject     audioEffect_ref;    // AudioEffect object instance
    AudioEffect*  audioEffect_ref;
};

class AudioEffectJniStorage {
    public:
        effect_callback_cookie mCallbackData;
    AudioEffectJniStorage() {
    }

    ~AudioEffectJniStorage() {
    }

};

#define AUDIOEFFECT_SUCCESS                      0
#define AUDIOEFFECT_ERROR                       -1
#define AUDIOEFFECT_ERROR_ALREADY_EXISTS        -2
#define AUDIOEFFECT_ERROR_NO_INIT               -3
#define AUDIOEFFECT_ERROR_BAD_VALUE             -4
#define AUDIOEFFECT_ERROR_INVALID_OPERATION     -5
#define AUDIOEFFECT_ERROR_NO_MEMORY             -6
#define AUDIOEFFECT_ERROR_DEAD_OBJECT           -7

static Int32 translateError(Int32 code) {
    switch(code) {
    case android::NO_ERROR:
        return AUDIOEFFECT_SUCCESS;
    case android::ALREADY_EXISTS:
        return AUDIOEFFECT_ERROR_ALREADY_EXISTS;
    case android::NO_INIT:
        return AUDIOEFFECT_ERROR_NO_INIT;
    case android::BAD_VALUE:
        return AUDIOEFFECT_ERROR_BAD_VALUE;
    case android::INVALID_OPERATION:
        return AUDIOEFFECT_ERROR_INVALID_OPERATION;
    case android::NO_MEMORY:
        return AUDIOEFFECT_ERROR_NO_MEMORY;
    case android::DEAD_OBJECT:
        return AUDIOEFFECT_ERROR_DEAD_OBJECT;
    default:
        return AUDIOEFFECT_ERROR;
    }
}

void AudioEffect::effectCallback(
    /* [in] */ int event,
    /* [in] */ void* user,
    /* [in] */ void *info)
{
    effect_param_t *p;
    int arg1 = 0;
    int arg2 = 0;
    AutoPtr<ArrayOf<Byte> > array;
    Byte *bytes;
    bool param;
    size_t size;

    effect_callback_cookie *callbackInfo = (effect_callback_cookie *)user;

    if (!user) {
        ALOGW("effectCallback error user %p", user);
        return;
    }

    AutoPtr<IArrayOf> obj;
    AutoPtr<IByte> byte;
    switch (event) {
    case android::AudioEffect::EVENT_CONTROL_STATUS_CHANGED:
        if (info == 0) {
            ALOGW("EVENT_CONTROL_STATUS_CHANGED info == NULL");
            goto effectCallback_Exit;
        }
        param = *(bool *)info;
        arg1 = (int)param;
        ALOGV("EVENT_CONTROL_STATUS_CHANGED");
        break;
    case android::AudioEffect::EVENT_ENABLE_STATUS_CHANGED:
        if (info == 0) {
            ALOGW("EVENT_ENABLE_STATUS_CHANGED info == NULL");
            goto effectCallback_Exit;
        }
        param = *(bool *)info;
        arg1 = (int)param;
        ALOGV("EVENT_ENABLE_STATUS_CHANGED");
        break;
    case android::AudioEffect::EVENT_PARAMETER_CHANGED:
        if (info == 0) {
            ALOGW("EVENT_PARAMETER_CHANGED info == NULL");
            goto effectCallback_Exit;
        }
        p = (effect_param_t *)info;
        if (p->psize == 0 || p->vsize == 0) {
            goto effectCallback_Exit;
        }
        // arg1 contains offset of parameter value from start of byte array
        arg1 = sizeof(effect_param_t) + ((p->psize - 1) / sizeof(int) + 1) * sizeof(int);
        size = arg1 + p->vsize;
        array = ArrayOf<Byte>::Alloc(size);
        if (array == NULL) {
            ALOGE("effectCallback: Couldn't allocate byte array for parameter data");
            goto effectCallback_Exit;
        }
        bytes = array->GetPayload();
        memcpy(bytes, p, size);

        CArrayOf::New(EIID_IByte, size,(IArrayOf**)&obj);
        for(Int32 i = 0; i < size; i++) {
            byte = NULL;
            CByte::New((*array)[i], (IByte**)&byte);
            obj->Set(i, byte);
        }

        ALOGV("EVENT_PARAMETER_CHANGED");
       break;
    case android::AudioEffect::EVENT_ERROR:
        ALOGW("EVENT_ERROR");
        break;
    }

    PostEventFromNative(callbackInfo->audioEffect_ref, event, arg1, arg2, obj);

effectCallback_Exit:
    return;
}

void AudioEffect::NativeInit()
{
}

Int32 AudioEffect::NativeSetup(
    /* [in] */ const String& type,
    /* [in] */ const String& uuid,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Int32>* jId,
    /* [in] */ ArrayOf<IAudioEffectDescriptor*>* javadesc)
{
    AudioEffectJniStorage* lpJniStorage = NULL;
    Int32 lStatus = AUDIOEFFECT_ERROR_NO_MEMORY;
    android::AudioEffect* lpAudioEffect = NULL;
    const char *typeStr = NULL;
    const char *uuidStr = NULL;
    effect_descriptor_t desc;
    AutoPtr<IAudioEffectDescriptor> jdesc;
    char str[EFFECT_STRING_LEN_MAX];
    String jdescType;
    String jdescUuid;
    String jdescConnect;
    String jdescName;
    String jdescImplementor;

    if (type != NULL) {
        typeStr = type.string();
        if (typeStr == NULL) {  // Out of memory
            goto setup_failure;
        }
    }

    if (uuid != NULL) {
        uuidStr = uuid.string();
        if (uuidStr == NULL) {  // Out of memory
            goto setup_failure;
        }
    }

    if (typeStr == NULL && uuidStr == NULL) {
        lStatus = AUDIOEFFECT_ERROR_BAD_VALUE;
        goto setup_failure;
    }

    lpJniStorage = new AudioEffectJniStorage();
    if (lpJniStorage == NULL) {
        goto setup_failure;
    }

    lpJniStorage->mCallbackData.audioEffect_ref = this;

    if (jId == NULL) {
        lStatus = AUDIOEFFECT_ERROR_BAD_VALUE;
        goto setup_failure;
    }

    lpAudioEffect = new android::AudioEffect(typeStr, uuidStr, priority,
        effectCallback, &lpJniStorage->mCallbackData, sessionId, 0);

    if (lpAudioEffect == NULL) {
        goto setup_failure;
    }

    lStatus = translateError(lpAudioEffect->initCheck());
    if (lStatus != AUDIOEFFECT_SUCCESS && lStatus != AUDIOEFFECT_ERROR_ALREADY_EXISTS) {
        goto setup_failure;
    }

    (*jId)[0] = lpAudioEffect->id();

    if (typeStr) {
        typeStr = NULL;
    }

    if (uuidStr) {
        uuidStr = NULL;
    }

    // get the effect descriptor
    desc = lpAudioEffect->descriptor();

    android::AudioEffect::guidToString(&desc.type, str, EFFECT_STRING_LEN_MAX);
    jdescType = str;

    android::AudioEffect::guidToString(&desc.uuid, str, EFFECT_STRING_LEN_MAX);
    jdescUuid = str;

    if ((desc.flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_AUXILIARY) {
        jdescConnect = "Auxiliary";
    } else if ((desc.flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_PRE_PROC) {
        jdescConnect = "Pre Processing";
    } else {
        jdescConnect = "Insert";
    }

    jdescName = desc.name;
    jdescImplementor = desc.implementor;

    CAudioEffectDescriptor::New(jdescType, jdescUuid, jdescConnect, jdescName, jdescImplementor, (IAudioEffectDescriptor**)&jdesc);

    if (jdesc == NULL) {
        goto setup_failure;
    }

    javadesc->Set(0, jdesc);

    mNativeAudioEffect = (Int32)lpAudioEffect;

    mJniData = (Int32)lpJniStorage;

    return AUDIOEFFECT_SUCCESS;

    // failures:
    setup_failure:

    if (lpAudioEffect) {
        delete lpAudioEffect;
    }

    mNativeAudioEffect = 0;

    if (lpJniStorage) {
        delete lpJniStorage;
    }

    mJniData = 0;

    return lStatus;
}

void AudioEffect::NativeFinalize()
{
    // delete the AudioEffect object
    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;
    if (lpAudioEffect) {
        delete lpAudioEffect;
    }

    // delete the JNI data
    AudioEffectJniStorage* lpJniStorage = (AudioEffectJniStorage*)mJniData;
    if (lpJniStorage) {
        delete lpJniStorage;
    }
}

void AudioEffect::NativeRelease()
{
    // do everything a call to finalize would
    NativeFinalize();
    mNativeAudioEffect = 0;
    mJniData = 0;
}

Int32 AudioEffect::NativeSetEnabled(
    /* [in] */ Boolean enabled)
{
    // retrieve the AudioEffect object
    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;

    if (lpAudioEffect == NULL) {
        return AUDIOEFFECT_ERROR_NO_INIT;
    }

    return translateError(lpAudioEffect->setEnabled(enabled));
}

Boolean AudioEffect::NativeGetEnabled()
{
    // retrieve the AudioEffect object
    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;
    if (lpAudioEffect == NULL) {
        return FALSE;
    }
    if (lpAudioEffect->getEnabled()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

Boolean AudioEffect::NativeHasControl()
{
    // retrieve the AudioEffect object
    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;

    if (lpAudioEffect == NULL) {
        return FALSE;
    }

    if (lpAudioEffect->initCheck() == android::NO_ERROR) {
        return TRUE;
    } else {
        return FALSE;
    }
}

Int32 AudioEffect::NativeSetParameter(
    /* [in] */ Int32 psize,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ Int32 vsize,
    /* [in] */ ArrayOf<Byte>* value)
{
    // retrieve the AudioEffect object
    Byte* lpValue = NULL;
    Byte* lpParam = NULL;
    Int32 lStatus = AUDIOEFFECT_ERROR_BAD_VALUE;
    effect_param_t *p;
    Int32 voffset;

    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;

    if (lpAudioEffect == NULL) {
        return AUDIOEFFECT_ERROR_NO_INIT;
    }

    if (psize == 0 || vsize == 0 || param == NULL || value == NULL) {
        return AUDIOEFFECT_ERROR_BAD_VALUE;
    }

    // get the pointer for the param from the java array
    lpParam = param->GetPayload();
    if (lpParam == NULL) {
        goto setParameter_Exit;
    }

    // get the pointer for the value from the java array
    lpValue = value->GetPayload();
    if (lpValue == NULL) {
        goto setParameter_Exit;
    }

    voffset = ((psize - 1) / sizeof(int) + 1) * sizeof(int);
    p = (effect_param_t *) malloc(sizeof(effect_param_t) + voffset + vsize);
    memcpy(p->data, lpParam, psize);
    p->psize = psize;
    memcpy(p->data + voffset, lpValue, vsize);
    p->vsize = vsize;

    lStatus = lpAudioEffect->setParameter(p);
    if (lStatus == android::NO_ERROR) {
        lStatus = p->status;
    }

    free(p);

    setParameter_Exit:
    return translateError(lStatus);
}

Int32 AudioEffect::NativeGetParameter(
    /* [in] */ Int32 psize,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ Int32 vsize,
    /* [in] */ ArrayOf<Byte>* value)
{
    // retrieve the AudioEffect object
    Byte* lpParam = NULL;
    Byte* lpValue = NULL;
    Int32 lStatus = AUDIOEFFECT_ERROR_BAD_VALUE;
    effect_param_t *p;
    int voffset;

    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;

    if (lpAudioEffect == NULL) {
        return AUDIOEFFECT_ERROR_NO_INIT;
    }

    if (psize == 0 || vsize == 0 || param == NULL || value == NULL) {
        return AUDIOEFFECT_ERROR_BAD_VALUE;
    }

    // get the pointer for the param from the java array
    lpParam = param->GetPayload();  //(Byte *) env->GetPrimitiveArrayCritical(param, NULL);
    if (lpParam == NULL) {
        goto getParameter_Exit;
    }

    // get the pointer for the value from the java array
    lpValue = value->GetPayload();  //(Byte *) env->GetPrimitiveArrayCritical(value, NULL);
    if (lpValue == NULL) {
        goto getParameter_Exit;
    }

    voffset = ((psize - 1) / sizeof(int) + 1) * sizeof(int);
    p = (effect_param_t *) malloc(sizeof(effect_param_t) + voffset
            + vsize);
    memcpy(p->data, lpParam, psize);
    p->psize = psize;
    p->vsize = vsize;

    lStatus = lpAudioEffect->getParameter(p);
    if (lStatus == android::NO_ERROR) {
        lStatus = p->status;
        if (lStatus == android::NO_ERROR) {
            memcpy(lpValue, p->data + voffset, p->vsize);
            vsize = p->vsize;
        }
    }

    free(p);

    getParameter_Exit:
    if (lStatus == android::NO_ERROR) {
        return vsize;
    }
    return translateError(lStatus);
}

Int32 AudioEffect::NativeCommand(
    /* [in] */ Int32 cmdCode,
    /* [in] */ Int32 cmdSize,
    /* [in] */ ArrayOf<Byte>* cmdData,
    /* [in] */ Int32 replySize,
    /* [in] */ ArrayOf<Byte>* repData)
{
    Byte* pCmdData = NULL;
    Byte* pReplyData = NULL;
    Int32 lStatus = AUDIOEFFECT_ERROR_BAD_VALUE;

    // retrieve the AudioEffect object
    android::AudioEffect* lpAudioEffect = (android::AudioEffect *)mNativeAudioEffect;

    if (lpAudioEffect == NULL) {
        return AUDIOEFFECT_ERROR_NO_INIT;
    }

    if ((cmdSize != 0 && cmdData == NULL) || (replySize != 0 && repData == NULL)) {
        return AUDIOEFFECT_ERROR_BAD_VALUE;
    }

    // get the pointer for the command from the java array
    if (cmdSize != 0) {
        pCmdData = cmdData->GetPayload();
        if (cmdData == NULL) {
            goto command_Exit;
        }
    }

    // get the pointer for the reply from the java array
    if (replySize != 0 && repData != NULL) {
        pReplyData = repData->GetPayload();
        if (pReplyData == NULL) {
            goto command_Exit;
        }
    }

    lStatus = translateError(lpAudioEffect->command((uint32_t)cmdCode,
                                                    (uint32_t)cmdSize,
                                                    pCmdData,
                                                    (uint32_t *)&replySize,
                                                    pReplyData));

    command_Exit:
    if (lStatus == android::NO_ERROR) {
        return replySize;
    }
    return lStatus;
}

AutoPtr<ArrayOf<IAudioEffectDescriptor* > > AudioEffect::NativeQueryEffects()
{
    effect_descriptor_t desc;
    char str[EFFECT_STRING_LEN_MAX];
    uint32_t totalEffectsCount = 0;
    uint32_t returnedEffectsCount = 0;
    uint32_t i = 0;
    String jdescType;
    String jdescUuid;
    String jdescConnect;
    String jdescName;
    String jdescImplementor;
    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > ret;

    if (android::AudioEffect::queryNumberEffects(&totalEffectsCount) != android::NO_ERROR) {
        return NULL;
    }

    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > temp = ArrayOf<IAudioEffectDescriptor*>::Alloc(totalEffectsCount);
    if (temp == NULL) {
        return temp;
    }

    ALOGV("queryEffects() totalEffectsCount: %d", totalEffectsCount);

    for (i = 0; i < totalEffectsCount; i++) {
        if (android::AudioEffect::queryEffect(i, &desc) != android::NO_ERROR) {
            goto queryEffects_failure;
        }

        if ((desc.flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_AUXILIARY) {
            jdescConnect = "Auxiliary";
        } else if ((desc.flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_INSERT) {
            jdescConnect = "Insert";
        } else if ((desc.flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_PRE_PROC) {
            jdescConnect = "Pre Processing";
        } else {
            continue;
        }

        android::AudioEffect::guidToString(&desc.type, str, EFFECT_STRING_LEN_MAX);
        jdescType = str;

        android::AudioEffect::guidToString(&desc.uuid, str, EFFECT_STRING_LEN_MAX);
        jdescUuid = str;

        jdescName = desc.name;
        jdescImplementor = desc.implementor;

        AutoPtr<IAudioEffectDescriptor> jdesc;
        CAudioEffectDescriptor::New(jdescType, jdescUuid, jdescConnect, jdescName, jdescImplementor, (IAudioEffectDescriptor**)&jdesc);

        if (jdesc == NULL) {
            ALOGE("env->NewObject(fields.clazzDesc, fields.midDescCstor)");
            goto queryEffects_failure;
        }

        temp->Set(returnedEffectsCount++, jdesc);
   }

    if (returnedEffectsCount == 0) {
        goto queryEffects_failure;
    }
    ret = ArrayOf<IAudioEffectDescriptor*>::Alloc(returnedEffectsCount);
    if (ret == NULL) {
        goto queryEffects_failure;
    }
    for (i = 0; i < returnedEffectsCount; i++) {
        ret->Set(i, (*temp)[i]);
    }
    return ret;

queryEffects_failure:

    return NULL;
}

AutoPtr<ArrayOf<IAudioEffectDescriptor* > > AudioEffect::NativeQueryPreProcessing(
    /* [in] */ Int32 audioSession)
{
    effect_descriptor_t *descriptors = new effect_descriptor_t[android::AudioEffect::kMaxPreProcessing];
    uint32_t numEffects = android::AudioEffect::kMaxPreProcessing;

    android::status_t status = android::AudioEffect::queryDefaultPreProcessing(audioSession,descriptors,&numEffects);

    // if ((status != android::NO_ERROR && status != android::NO_MEMORY) || numEffects == 0) {
    //     delete[] descriptors;
    //     return NULL;
    // }
    // if (status == android::NO_MEMORY) {
    //     delete [] descriptors;
    //     descriptors = new effect_descriptor_t[numEffects];
    //     status = android::AudioEffect::queryDefaultPreProcessing(audioSession,descriptors,&numEffects);
    // }
    if (status != android::NO_ERROR || numEffects == 0) {
        delete[] descriptors;
        return NULL;
    }

    ALOGV("queryDefaultPreProcessing() got %d effects", numEffects);

    AutoPtr<ArrayOf<IAudioEffectDescriptor*> > ret = ArrayOf<IAudioEffectDescriptor*>::Alloc(numEffects);
    if (ret == NULL) {
        delete[] descriptors;
        return ret;
    }

    char str[EFFECT_STRING_LEN_MAX];
    String jdescType;
    String jdescUuid;
    String jdescConnect;
    String jdescName;
    String jdescImplementor;

    for (uint32_t i = 0; i < numEffects; i++) {

        android::AudioEffect::guidToString(&descriptors[i].type, str, EFFECT_STRING_LEN_MAX);
        jdescType = str;
        android::AudioEffect::guidToString(&descriptors[i].uuid, str, EFFECT_STRING_LEN_MAX);
        jdescUuid = str;
        jdescConnect = "Pre Processing";
        jdescName = descriptors[i].name;
        jdescImplementor = descriptors[i].implementor;

        AutoPtr<IAudioEffectDescriptor> jdesc;
        CAudioEffectDescriptor::New(jdescType, jdescUuid, jdescConnect, jdescName, jdescImplementor, (IAudioEffectDescriptor**)&jdesc);

        if (jdesc == NULL) {
            ALOGE("env->NewObject(fields.clazzDesc, fields.midDescCstor)");
            return NULL;;
        }

        ret->Set(i, jdesc);
   }

   return ret;
}

ECode AudioEffect::CheckState(
    /* [in] */ const String& methodName)
{
    synchronized(mStateLock);
    if (mState != IAudioEffect::STATE_INITIALIZED) {
       // throw (new IllegalStateException(methodName
       //         + " called on uninitialized AudioEffect."));
        Logger::E(TAG, "%s called on uninitialized AudioEffect.", methodName.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode AudioEffect::CheckStatus(
    /* [in] */ Int32 status)
{
    Boolean b;
    if ((IsError(status, &b)),b) {
        switch (status) {
            case IAudioEffect::ERROR_BAD_VALUE:
               // throw (new IllegalArgumentException(
               //         "AudioEffect: bad parameter value"));
                Logger::E(TAG, "AudioEffect: bad parameter value");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            case IAudioEffect::ERROR_INVALID_OPERATION:
               // throw (new UnsupportedOperationException(
               //         "AudioEffect: invalid parameter operation"));
                Logger::E(TAG, "AudioEffect: invalid parameter operation");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            default:
               // throw (new RuntimeException("AudioEffect: set/get parameter error"));
                Logger::E(TAG, "AudioEffect: set/get parameter error");
                return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode AudioEffect::IsError(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    *result = status < 0;
    return NOERROR;
}

ECode AudioEffect::ByteArrayToInt32(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return ByteArrayToInt32(valueBuf, 0, result);
}

ECode AudioEffect::ByteArrayToInt32(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IByteBuffer> converter;
    AutoPtr<IByteBufferHelper> tempByteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&tempByteBufferHelper);
    tempByteBufferHelper->Wrap(valueBuf,(IByteBuffer**)&converter);
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder nativeOrder;
    helper->GetNativeOrder(&nativeOrder);
    converter->SetOrder(nativeOrder);
    return converter->GetInt32(offset, result);
}

ECode AudioEffect::Int32ToByteArray(
    /* [in] */ Int32 value,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IByteBuffer> converter;
    AutoPtr<IByteBufferHelper> tempByteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&tempByteBufferHelper);
    tempByteBufferHelper->Allocate(4,(IByteBuffer**)&converter);
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder nativeOrder;
    helper->GetNativeOrder(&nativeOrder);
    converter->SetOrder(nativeOrder);
    converter->PutInt32(value);
    return converter->GetArray(result);
}

ECode AudioEffect::ByteArrayToInt16(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);

    return ByteArrayToInt16(valueBuf, 0, result);
}

ECode AudioEffect::ByteArrayToInt16(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [in] */ Int32 offset,
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IByteBuffer> converter;
    AutoPtr<IByteBufferHelper> tempByteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&tempByteBufferHelper);
    tempByteBufferHelper->Wrap(valueBuf,(IByteBuffer**)&converter);
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder nativeOrder;
    helper->GetNativeOrder(&nativeOrder);
    converter->SetOrder(nativeOrder);
    return converter->GetInt16(offset, result);
}

ECode AudioEffect::Int16ToByteArray(
    /* [in] */ Int16 value,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IByteBuffer> converter;
    AutoPtr<IByteBufferHelper> tempByteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&tempByteBufferHelper);
    tempByteBufferHelper->Allocate(2,(IByteBuffer**)&converter);
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder nativeOrder;
    helper->GetNativeOrder(&nativeOrder);
    converter->SetOrder(nativeOrder);
    converter->PutInt16(value);
    return converter->GetArray(result);
}

ECode AudioEffect::ConcatArrays(
    /* [in] */ ArrayOf<Byte>* array1,
    /* [in] */ ArrayOf<Byte>* array2,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 len = array1->GetLength() + array2->GetLength();
    AutoPtr<ArrayOf<Byte> > newArray = ArrayOf<Byte>::Alloc(len);
    if (newArray == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    newArray->Copy(array1);
    newArray->Copy(array1->GetLength(), array2, 0, array2->GetLength());
    *result = newArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
