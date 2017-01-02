//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Media.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerModule.h"
#include "elastos/droid/hardware/soundtrigger/SoundTrigger.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerConfidenceLevel.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseRecognitionExtra.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseRecognitionEvent.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerRecognitionEvent.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerSoundModelEvent.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <soundtrigger/SoundTrigger.h>
#include <binder/MemoryDealer.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerConfidenceLevel;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerKeyphraseRecognitionExtra;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerKeyphraseRecognitionEvent;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerRecognitionEvent;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerSoundModelEvent;
using Elastos::Core::AutoLock;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

const Int32 SoundTriggerModule::EVENT_RECOGNITION = 1;
const Int32 SoundTriggerModule::EVENT_SERVICE_DIED = 2;
const Int32 SoundTriggerModule::EVENT_SOUNDMODEL = 3;
const Int32 SoundTriggerModule::EVENT_SERVICE_STATE_CHANGE = 4;

enum {
    SOUNDTRIGGER_STATUS_OK = 0,
    SOUNDTRIGGER_STATUS_ERROR = INT_MIN,
    SOUNDTRIGGER_PERMISSION_DENIED = -1,
    SOUNDTRIGGER_STATUS_NO_INIT = -19,
    SOUNDTRIGGER_STATUS_BAD_VALUE = -22,
    SOUNDTRIGGER_STATUS_DEAD_OBJECT = -32,
    SOUNDTRIGGER_INVALID_OPERATION = -38,
};

enum  {
    SOUNDTRIGGER_EVENT_RECOGNITION = 1,
    SOUNDTRIGGER_EVENT_SERVICE_DIED = 2,
    SOUNDTRIGGER_EVENT_SOUNDMODEL = 3,
    SOUNDTRIGGER_EVENT_SERVICE_STATE_CHANGE = 4,
};

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNISoundTriggerCallback : public android::SoundTriggerCallback
{
public:
    JNISoundTriggerCallback(IWeakReference* weak_thiz);
    ~JNISoundTriggerCallback();

    virtual void onRecognitionEvent(struct sound_trigger_recognition_event *event);
    virtual void onSoundModelEvent(struct sound_trigger_model_event *event);
    virtual void onServiceStateChange(sound_trigger_service_state_t state);
    virtual void onServiceDied();

private:
    //jclass      mClass;                 // Reference to SoundTrigger class
    AutoPtr<IWeakReference> mWeakObject;  // Weak ref to SoundTrigger Java object to call on
};

JNISoundTriggerCallback::JNISoundTriggerCallback(IWeakReference* weak_thiz)
{

    // Hold onto the SoundTriggerModule class for use in calling the static method
    // that posts events to the application thread.
    // jclass clazz = env->GetObjectClass(thiz);
    // if (clazz == NULL) {
    //     ALOGE("Can't find class %s", kModuleClassPathName);
    //     return;
    // }
    // mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the SoundTriggerModule object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    // mObject  = env->NewGlobalRef(weak_thiz);
    mWeakObject  = weak_thiz;
}

JNISoundTriggerCallback::~JNISoundTriggerCallback()
{
    // remove global references
    // JNIEnv *env = AndroidRuntime::getJNIEnv();
    // env->DeleteGlobalRef(mObject);
    // env->DeleteGlobalRef(mClass);
}

void JNISoundTriggerCallback::onRecognitionEvent(struct sound_trigger_recognition_event *event)
{
    //JNIEnv *env = AndroidRuntime::getJNIEnv();
    //jobject jEvent = NULL;
    AutoPtr<ISoundTriggerRecognitionEvent> jEvent;
    //jbyteArray jData = NULL;
    AutoPtr<ArrayOf<Byte> > jData;

    if (event->data_size) {
        //jData = env->NewByteArray(event->data_size);
        jData = ArrayOf<Byte>::Alloc(event->data_size);
        //jbyte *nData = env->GetByteArrayElements(jData, NULL);
        Byte* nData = jData->GetPayload();
        memcpy(nData, (char *)event + event->data_offset, event->data_size);
        //env->ReleaseByteArrayElements(jData, nData, 0);
    }

    // /jobject jAudioFormat = NULL;
    AutoPtr<IAudioFormat> jAudioFormat;
    if (event->trigger_in_data || event->capture_available) {
        // jAudioFormat = env->NewObject(gAudioFormatClass,
        //                             gAudioFormatCstor,
        //                             audioFormatFromNative(event->audio_config.format),
        //                             event->audio_config.sample_rate,
        //                             inChannelMaskFromNative(event->audio_config.channel_mask));
        assert(0 && "TODO: CAudioFormat");
        // CAudioFormat::New(audioFormatFromNative(event->audio_config.format),
        //         event->audio_config.sample_rate,
        //         inChannelMaskFromNative(event->audio_config.channel_mask),
        //         (IAudioFormat**)&jAudioFormat);


    }
    if (event->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        struct sound_trigger_phrase_recognition_event *phraseEvent =
                (struct sound_trigger_phrase_recognition_event *)event;

        // jobjectArray jExtras = env->NewObjectArray(phraseEvent->num_phrases,
        //                                           gKeyphraseRecognitionExtraClass, NULL);
        AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > jExtras =
                ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>::Alloc(phraseEvent->num_phrases);
        if (jExtras == NULL) {
            return;
        }

        for (size_t i = 0; i < phraseEvent->num_phrases; i++) {
            // jobjectArray jConfidenceLevels = env->NewObjectArray(
            //                                             phraseEvent->phrase_extras[i].num_levels,
            //                                             gConfidenceLevelClass, NULL);
            AutoPtr<ArrayOf<ISoundTriggerConfidenceLevel*> > jConfidenceLevels =
                    ArrayOf<ISoundTriggerConfidenceLevel*>::Alloc(phraseEvent->phrase_extras[i].num_levels);

            if (jConfidenceLevels == NULL) {
                return;
            }
            for (size_t j = 0; j < phraseEvent->phrase_extras[i].num_levels; j++) {
                // jobject jConfidenceLevel = env->NewObject(gConfidenceLevelClass,
                //                                   gConfidenceLevelCstor,
                //                                   phraseEvent->phrase_extras[i].levels[j].user_id,
                //                                   phraseEvent->phrase_extras[i].levels[j].level);
                AutoPtr<ISoundTriggerConfidenceLevel> jConfidenceLevel;
                CSoundTriggerConfidenceLevel::New(phraseEvent->phrase_extras[i].levels[j].user_id,
                                                  phraseEvent->phrase_extras[i].levels[j].level,
                                                  (ISoundTriggerConfidenceLevel**)&jConfidenceLevel);
                //env->SetObjectArrayElement(jConfidenceLevels, j, jConfidenceLevel);
                jConfidenceLevels->Set(j, jConfidenceLevel);
                //env->DeleteLocalRef(jConfidenceLevel);
            }

            // jobject jNewExtra = env->NewObject(gKeyphraseRecognitionExtraClass,
            //                                    gKeyphraseRecognitionExtraCstor,
            //                                    phraseEvent->phrase_extras[i].id,
            //                                    phraseEvent->phrase_extras[i].recognition_modes,
            //                                    phraseEvent->phrase_extras[i].confidence_level,
            //                                    jConfidenceLevels);

            AutoPtr<ISoundTriggerKeyphraseRecognitionExtra> jNewExtra;
            CSoundTriggerKeyphraseRecognitionExtra::New(phraseEvent->phrase_extras[i].id,
                    phraseEvent->phrase_extras[i].recognition_modes,
                    phraseEvent->phrase_extras[i].confidence_level,
                    jConfidenceLevels, (ISoundTriggerKeyphraseRecognitionExtra**)&jNewExtra);

            if (jNewExtra == NULL) {
                return;
            }
            //env->SetObjectArrayElement(jExtras, i, jNewExtra);
            jExtras->Set(i, jNewExtra);
            // env->DeleteLocalRef(jNewExtra);
            // env->DeleteLocalRef(jConfidenceLevels);
        }
        // jEvent = env->NewObject(gKeyphraseRecognitionEventClass, gKeyphraseRecognitionEventCstor,
        //                         event->status, event->model, event->capture_available,
        //                         event->capture_session, event->capture_delay_ms,
        //                         event->capture_preamble_ms, event->trigger_in_data,
        //                         jAudioFormat, jData, jExtras);

        CSoundTriggerKeyphraseRecognitionEvent::New(event->status, event->model, event->capture_available,
                event->capture_session, event->capture_delay_ms,
                event->capture_preamble_ms, event->trigger_in_data,
                jAudioFormat, jData, jExtras, (ISoundTriggerRecognitionEvent**)&jEvent);

        // env->DeleteLocalRef(jAudioFormat);
        // env->DeleteLocalRef(jData);
    } else {
        // jEvent = env->NewObject(gRecognitionEventClass, gRecognitionEventCstor,
        //                         event->status, event->model, event->capture_available,
        //                         event->capture_session, event->capture_delay_ms,
        //                         event->capture_preamble_ms, event->trigger_in_data,
        //                         jAudioFormat, jData);

        CSoundTriggerRecognitionEvent::New(event->status, event->model, event->capture_available,
                event->capture_session, event->capture_delay_ms,
                event->capture_preamble_ms, event->trigger_in_data,
                jAudioFormat, jData, (ISoundTriggerRecognitionEvent**)&jEvent);
        // env->DeleteLocalRef(jAudioFormat);
        // env->DeleteLocalRef(jData);
    }

    // env->CallStaticVoidMethod(mClass, gPostEventFromNative, mObject,
    //                           SOUNDTRIGGER_EVENT_RECOGNITION, 0, 0, jEvent);
    SoundTriggerModule::PostEventFromNative(mWeakObject, SOUNDTRIGGER_EVENT_RECOGNITION,
            0, 0, TO_IINTERFACE(jEvent));

    //env->DeleteLocalRef(jEvent);
    // if (env->ExceptionCheck()) {
    //     ALOGW("An exception occurred while notifying an event.");
    //     env->ExceptionClear();
    // }
}

void JNISoundTriggerCallback::onSoundModelEvent(struct sound_trigger_model_event *event)
{
    //JNIEnv *env = AndroidRuntime::getJNIEnv();
    //jobject jEvent = NULL;
    AutoPtr<ISoundTriggerSoundModelEvent> jEvent;
    //jbyteArray jData = NULL;
    AutoPtr<ArrayOf<Byte> > jData;

    if (event->data_size) {
        //jData = env->NewByteArray(event->data_size);
        jData = ArrayOf<Byte>::Alloc(event->data_size);
        //jbyte *nData = env->GetByteArrayElements(jData, NULL);
        Byte* nData = jData->GetPayload();
        memcpy(nData, (char *)event + event->data_offset, event->data_size);
        //env->ReleaseByteArrayElements(jData, nData, 0);
    }

    // jEvent = env->NewObject(gSoundModelEventClass, gSoundModelEventCstor,
    //                         event->status, event->model, jData);
    CSoundTriggerSoundModelEvent::New(event->status, event->model, jData,
            (ISoundTriggerSoundModelEvent**)&jEvent);

    //env->DeleteLocalRef(jData);
    // env->CallStaticVoidMethod(mClass, gPostEventFromNative, mObject,
    //                           SOUNDTRIGGER_EVENT_SOUNDMODEL, 0, 0, jEvent);
    SoundTriggerModule::PostEventFromNative(mWeakObject, SOUNDTRIGGER_EVENT_SOUNDMODEL, 0,
            0, TO_IINTERFACE(jEvent));
    // env->DeleteLocalRef(jEvent);
    // if (env->ExceptionCheck()) {
    //     ALOGW("An exception occurred while notifying an event.");
    //     env->ExceptionClear();
    // }
}

void JNISoundTriggerCallback::onServiceStateChange(sound_trigger_service_state_t state)
{
    // JNIEnv *env = AndroidRuntime::getJNIEnv();
    // env->CallStaticVoidMethod(mClass, gPostEventFromNative, mObject,
    //                                     SOUNDTRIGGER_EVENT_SERVICE_STATE_CHANGE, state, 0, NULL);
    SoundTriggerModule::PostEventFromNative(mWeakObject, SOUNDTRIGGER_EVENT_SERVICE_STATE_CHANGE, state,
            0, NULL);
    // if (env->ExceptionCheck()) {
    //     ALOGW("An exception occurred while notifying an event.");
    //     env->ExceptionClear();
    // }
}

void JNISoundTriggerCallback::onServiceDied()
{
    // JNIEnv *env = AndroidRuntime::getJNIEnv();

    // env->CallStaticVoidMethod(mClass, gPostEventFromNative, mObject,
    //                           SOUNDTRIGGER_EVENT_SERVICE_DIED, 0, 0, NULL);
    SoundTriggerModule::PostEventFromNative(mWeakObject, SOUNDTRIGGER_EVENT_SERVICE_DIED, 0,
            0, NULL);
    // if (env->ExceptionCheck()) {
    //     ALOGW("An exception occurred while notifying an event.");
    //     env->ExceptionClear();
    // }
}

SoundTriggerModule::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ ISoundTriggerStatusListener* listener)
    : Handler(looper)
    , mListener(listener)
{
}

ECode SoundTriggerModule::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch(what) {
        case EVENT_RECOGNITION:
            if (mListener != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                mListener->OnRecognition(ISoundTriggerRecognitionEvent::Probe(obj));
            }
            break;
        case EVENT_SOUNDMODEL:
            if (mListener != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                mListener->OnSoundModelUpdate(ISoundTriggerSoundModelEvent::Probe(obj));
            }
            break;
        case EVENT_SERVICE_STATE_CHANGE:
            if (mListener != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mListener->OnServiceStateChange(arg1);
            }
            break;
        case EVENT_SERVICE_DIED:
            if (mListener != NULL) {
                mListener->OnServiceDied();
            }
            break;
        default:
            break;
    }
    return NOERROR;
}

SoundTriggerModule::NativeEventHandlerDelegate::NativeEventHandlerDelegate(
    /* [in] */ ISoundTriggerStatusListener* listener,
    /* [in] */ IHandler* handler)
{
    // find the looper for our new event handler
    AutoPtr<ILooper> looper;
    if (handler != NULL) {
        handler->GetLooper((ILooper**)&looper);
    } else {
        looper = Looper::GetMainLooper();
    }

    // construct the event handler with this looper
    if (looper != NULL) {
        // implement the event handler delegate

        mHandler = new MyHandler(looper, listener);
    } else {
        mHandler = NULL;
    }
}

ECode SoundTriggerModule::NativeEventHandlerDelegate::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = mHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SoundTriggerModule, Object, ISoundTriggerModule)

SoundTriggerModule::SoundTriggerModule()
    : mNativeContext(0)
    , mId(0)
{
}

SoundTriggerModule::~SoundTriggerModule()
{
    native_finalize();
}

ECode SoundTriggerModule::constructor(
    /* [in] */ Int32 moduleId,
    /* [in] */ ISoundTriggerStatusListener* listener,
    /* [in] */ IHandler* handler)
{
    mId = moduleId;
    mEventHandlerDelegate = new NativeEventHandlerDelegate(listener, handler);
    native_setup(TO_IINTERFACE(this));
    return NOERROR;
}

static Object gLock;

static android::sp<android::SoundTrigger> getSoundTrigger(SoundTriggerModule* mode)
{
    {    AutoLock syncLock(gLock);
        Int64 context;
        mode->GetNativeContext(&context);
        android::SoundTrigger* const st = reinterpret_cast<android::SoundTrigger*>(context);
        return android::sp<android::SoundTrigger>(st);
    }
    return NULL;
}

static android::sp<android::SoundTrigger> setSoundTrigger(SoundTriggerModule* mode,
        const android::sp<android::SoundTrigger>& module)
{
    {    AutoLock syncLock(gLock);
        Int64 context;
        mode->GetNativeContext(&context);
        android::sp<android::SoundTrigger> old = reinterpret_cast<android::SoundTrigger*>(context);
        if (module.get()) {
            module->incStrong((void*)setSoundTrigger);
        }
        if (old != 0) {
            old->decStrong((void*)setSoundTrigger);
        }
        mode->SetNativeContext(reinterpret_cast<Int64>(module.get()));

        return old;
    }
    return NULL;
}

ECode SoundTriggerModule::NativeDetach()
{
    ALOGV("detach");
    android::sp<android::SoundTrigger> module = setSoundTrigger(this, 0);
    ALOGV("detach module %p", module.get());
    if (module != 0) {
        ALOGV("detach module->detach()");
        module->detach();
    }
    return NOERROR;
}

ECode SoundTriggerModule::NativeLoadSoundModel(
    /* [in] */ ISoundTriggerSoundModel* model,
    /* [in] */ ArrayOf<Int32>* soundModelHandle,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status = SOUNDTRIGGER_STATUS_OK;
    //jbyte *nData = NULL;
    Byte *nData = NULL;
    struct sound_trigger_sound_model *nSoundModel;
    //jbyteArray jData;
    AutoPtr<ArrayOf<Byte> > jData;
    android::sp<android::MemoryDealer> memoryDealer;
    android::sp<android::IMemory> memory;
    size_t size;
    sound_model_handle_t handle;
    AutoPtr<IUUID> jUuid;
    //jstring jUuidString;
    String jUuidString;
    const char *nUuidString;

    ALOGV("loadSoundModel");
    android::sp<android::SoundTrigger> module = getSoundTrigger(this);
    if (module == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }
    if (soundModelHandle == NULL) {
        *result = SOUNDTRIGGER_STATUS_BAD_VALUE;
        return NOERROR;
    }
    //jsize jHandleLen = env->GetArrayLength(soundModelHandle);
    Int32 jHandleLen = soundModelHandle->GetLength();
    if (jHandleLen == 0) {
        *result = SOUNDTRIGGER_STATUS_BAD_VALUE;
        return NOERROR;
    }
    //jint *nHandle = env->GetIntArrayElements(soundModelHandle, NULL);
    Int32 *nHandle = soundModelHandle->GetPayload();
    if (nHandle == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }

    if (ISoundTriggerSoundModel::Probe(model) == NULL) {
        status = SOUNDTRIGGER_STATUS_BAD_VALUE;
        goto exit;
    }

    size_t offset;
    sound_trigger_sound_model_type_t type;
    if (ISoundTriggerKeyphraseSoundModel::Probe(model) != NULL) {
        offset = sizeof(struct sound_trigger_phrase_sound_model);
        type = SOUND_MODEL_TYPE_KEYPHRASE;
    } else {
        offset = sizeof(struct sound_trigger_sound_model);
        type = SOUND_MODEL_TYPE_UNKNOWN;
    }

    model->GetUuid((IUUID**)&jUuid);
    jUuid->ToString(&jUuidString);
    nUuidString = jUuidString.string();
    sound_trigger_uuid_t nUuid;
    android::SoundTrigger::stringToGuid(nUuidString, &nUuid);

    sound_trigger_uuid_t nVendorUuid;
    jUuid = NULL;
    model->GetVendorUuid((IUUID**)&jUuid);
    if (jUuid != NULL) {
        jUuid->ToString(&jUuidString);
        nUuidString = jUuidString.string();
        android::SoundTrigger::stringToGuid(nUuidString, &nVendorUuid);
    } else {
        android::SoundTrigger::stringToGuid("00000000-0000-0000-0000-000000000000", &nVendorUuid);
    }

    model->Getdata((ArrayOf<Byte>**)&jData);
    if (jData == NULL) {
        status = SOUNDTRIGGER_STATUS_BAD_VALUE;
        goto exit;
    }
    size = jData->GetLength();
    nData = jData->GetPayload();
    if (jData == NULL) {
        status = SOUNDTRIGGER_STATUS_ERROR;
        goto exit;
    }

    memoryDealer = new android::MemoryDealer(offset + size, "SoundTrigge-JNI::LoadModel");
    if (memoryDealer == 0) {
        status = SOUNDTRIGGER_STATUS_ERROR;
        goto exit;
    }
    memory = memoryDealer->allocate(offset + size);
    if (memory == 0 || memory->pointer() == NULL) {
        status = SOUNDTRIGGER_STATUS_ERROR;
        goto exit;
    }

    nSoundModel = (struct sound_trigger_sound_model *)memory->pointer();

    nSoundModel->type = type;
    nSoundModel->uuid = nUuid;
    nSoundModel->vendor_uuid = nVendorUuid;
    nSoundModel->data_size = size;
    nSoundModel->data_offset = offset;
    memcpy((char *)nSoundModel + offset, nData, size);
    if (type == SOUND_MODEL_TYPE_KEYPHRASE) {
        struct sound_trigger_phrase_sound_model *phraseModel =
                (struct sound_trigger_phrase_sound_model *)nSoundModel;

        AutoPtr<ArrayOf<ISoundTriggerKeyphrase*> > jPhrases;
        ISoundTriggerKeyphraseSoundModel::Probe(model)->GetKeyphrases((ArrayOf<ISoundTriggerKeyphrase*>**)&jPhrases);
        if (jPhrases == NULL) {
            status = SOUNDTRIGGER_STATUS_BAD_VALUE;
            goto exit;
        }

        size_t numPhrases = jPhrases->GetLength();
        phraseModel->num_phrases = numPhrases;
        ALOGV("loadSoundModel numPhrases %zu", numPhrases);
        for (size_t i = 0; i < numPhrases; i++) {
            AutoPtr<ISoundTriggerKeyphrase> jPhrase = (*jPhrases)[i];
            Int32 id;
            jPhrase->GetId(&id);
            phraseModel->phrases[i].id = id;

            Int32 modes;
            jPhrase->GetRecognitionModes(&modes);
            phraseModel->phrases[i].recognition_mode = modes;

            AutoPtr<ArrayOf<Int32> > jUsers;
            jPhrase->GetUsers((ArrayOf<Int32>**)&jUsers);
            phraseModel->phrases[i].num_users = jUsers->GetLength();
            Int32 *nUsers = jUsers->GetPayload();
            memcpy(phraseModel->phrases[i].users,
                   nUsers,
                   phraseModel->phrases[i].num_users * sizeof(int));

            String jLocale;
            jPhrase->GetLocale(&jLocale);
            const char *nLocale = jLocale.string();
            strncpy(phraseModel->phrases[i].locale,
                    nLocale,
                    SOUND_TRIGGER_MAX_LOCALE_LEN);
            String jText;
            jPhrase->GetText(&jText);
            const char *nText = jText.string();
            strncpy(phraseModel->phrases[i].text,
                    nText,
                    SOUND_TRIGGER_MAX_STRING_LEN);

            ALOGV("loadSoundModel phrases %zu text %s locale %s",
                  i, phraseModel->phrases[i].text, phraseModel->phrases[i].locale);
        }
    }
    status = module->loadSoundModel(memory, &handle);
    ALOGV("loadSoundModel status %d handle %d", status, handle);

exit:
    if (nHandle != NULL) {
        nHandle[0] = (Int32)handle;
    }
    *result = status;
    return NOERROR;
}

ECode SoundTriggerModule::NativeUnloadSoundModel(
    /* [in] */ Int32 soundModelHandle,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status = SOUNDTRIGGER_STATUS_OK;
    ALOGV("unloadSoundModel");
    android::sp<android::SoundTrigger> module = getSoundTrigger(this);
    if (module == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }
    status = module->unloadSoundModel((sound_model_handle_t)soundModelHandle);

    *result = status;
    return NOERROR;
}

ECode SoundTriggerModule::NativeStartRecognition(
    /* [in] */ Int32 soundModelHandle,
    /* [in] */ ISoundTriggerRecognitionConfig* jConfig,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status = SOUNDTRIGGER_STATUS_OK;
    ALOGV("startRecognition");
    android::sp<android::SoundTrigger> module = getSoundTrigger(this);
    if (module == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > jData;
    jConfig->GetData((ArrayOf<Byte>**)&jData);
    size_t dataSize = 0;
    Byte *nData = NULL;
    if (jData != NULL) {
        dataSize = jData->GetLength();
        if (dataSize == 0) {
            *result = SOUNDTRIGGER_STATUS_BAD_VALUE;
            return NOERROR;
        }
        nData = jData->GetPayload();
        if (nData == NULL) {
            *result = SOUNDTRIGGER_STATUS_ERROR;
            return NOERROR;
        }
    }

    size_t totalSize = sizeof(struct sound_trigger_recognition_config) + dataSize;
    android::sp<android::MemoryDealer> memoryDealer =
            new android::MemoryDealer(totalSize, "SoundTrigge-JNI::StartRecognition");
    if (memoryDealer == 0) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }
    android::sp<android::IMemory> memory = memoryDealer->allocate(totalSize);
    if (memory == 0 || memory->pointer() == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }
    if (dataSize != 0) {
        memcpy((char *)memory->pointer() + sizeof(struct sound_trigger_recognition_config),
                nData,
                dataSize);
    }
    struct sound_trigger_recognition_config *config =
                                    (struct sound_trigger_recognition_config *)memory->pointer();
    config->data_size = dataSize;
    config->data_offset = sizeof(struct sound_trigger_recognition_config);

    Boolean requested;
    jConfig->GetCaptureRequested(&requested);
    if (requested == TRUE) {
        config->capture_requested = true;
    }

    config->num_phrases = 0;
    AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > jPhrases;
    jConfig->GetKeyphrases((ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>**)&jPhrases);
    if (jPhrases != NULL) {
        config->num_phrases = jPhrases->GetLength();
    }
    ALOGV("startRecognition num phrases %d", config->num_phrases);
    for (size_t i = 0; i < config->num_phrases; i++) {
        AutoPtr<ISoundTriggerKeyphraseRecognitionExtra> jPhrase = (*jPhrases)[i];
        Int32 id;
        jPhrase->GetId(&id);
        config->phrases[i].id = id;

        Int32 modes;
        jPhrase->GetRecognitionModes(&modes);
        config->phrases[i].recognition_modes = modes;

        Int32 level;
        jPhrase->GetCoarseConfidenceLevel(&level);
        config->phrases[i].confidence_level = level;
        config->phrases[i].num_levels = 0;

        AutoPtr<ArrayOf<ISoundTriggerConfidenceLevel*> > jConfidenceLevels;
        jPhrase->GetConfidenceLevels((ArrayOf<ISoundTriggerConfidenceLevel*>**)&jConfidenceLevels);
        if (jConfidenceLevels != NULL) {
            config->phrases[i].num_levels = jConfidenceLevels->GetLength();
        }
        ALOGV("startRecognition phrase %zu num_levels %d", i, config->phrases[i].num_levels);
        for (size_t j = 0; j < config->phrases[i].num_levels; j++) {
            AutoPtr<ISoundTriggerConfidenceLevel> jConfidenceLevel = (*jConfidenceLevels)[j];
            Int32 userId;
            jConfidenceLevel->GetUserId(&userId);
            config->phrases[i].levels[j].user_id = userId;
            Int32 confidenceLevel;
            jConfidenceLevel->GetConfidenceLevel(&confidenceLevel);
            config->phrases[i].levels[j].level = confidenceLevel;
        }
        ALOGV("startRecognition phrases %zu", i);
    }

    status = module->startRecognition(soundModelHandle, memory);
    return status;

    return NOERROR;
}

ECode SoundTriggerModule::NativeStopRecognition(
    /* [in] */ Int32 soundModelHandle,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status = SOUNDTRIGGER_STATUS_OK;
    ALOGV("stopRecognition");
    android::sp<android::SoundTrigger> module = getSoundTrigger(this);
    if (module == NULL) {
        *result = SOUNDTRIGGER_STATUS_ERROR;
        return NOERROR;
    }
    status = module->stopRecognition(soundModelHandle);
    *result = status;
    return NOERROR;
}

void SoundTriggerModule::native_setup(
    /* [in] */ IInterface* module_this)
{
    ALOGV("setup");

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    android::sp<JNISoundTriggerCallback> callback = new JNISoundTriggerCallback(wr);

    sound_trigger_module_handle_t handle = (sound_trigger_module_handle_t)mId;

    android::sp<android::SoundTrigger> module = android::SoundTrigger::attach(handle, callback);
    if (module == 0) {
        return;
    }

    setSoundTrigger(this, module);
}

void SoundTriggerModule::native_finalize()
{
    ALOGV("finalize");
    android::sp<android::SoundTrigger> module = getSoundTrigger(this);
    if (module != 0) {
        ALOGW("SoundTrigger finalized without being detached");
    }
    NativeDetach();
}

ECode SoundTriggerModule::PostEventFromNative(
    /* [in] */ IInterface* module_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISoundTriggerModule> module = ISoundTriggerModule::Probe(module_ref);
    if (module == NULL) {
        return NOERROR;
    }

    AutoPtr<NativeEventHandlerDelegate> delegate = ((SoundTriggerModule*)module.Get())->mEventHandlerDelegate;
    if (delegate != NULL) {
        AutoPtr<IHandler> handler;
        delegate->GetHandler((IHandler**)&handler);
        if (handler != NULL) {
            AutoPtr<IMessage> m;
            handler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&m);
            Boolean result;
            handler->SendMessage(m, &result);
        }
    }
    return NOERROR;
}

ECode SoundTriggerModule::GetNativeContext(
    /* [out] */ Int64* context)
{
    VALIDATE_NOT_NULL(context);

    *context = mNativeContext;
    return NOERROR;
}

ECode SoundTriggerModule::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode SoundTriggerModule::SetNativeContext(
    /* [in] */ Int64 context)
{
    mNativeContext = context;
    return NOERROR;
}

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos