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

#include "elastos/droid/media/AudioPortEventHandler.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <media/AudioSystem.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Looper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

static const String TAG("AudioPortEventHandler");

const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_PORT_LIST_UPDATED = 1;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_PATCH_LIST_UPDATED = 2;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_SERVICE_DIED = 3;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_NEW_LISTENER = 4;

//===================================================================================
//              NativeAudioPortCallback
//===================================================================================

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class NativeAudioPortCallback: public android::AudioSystem::AudioPortCallback
{
public:
    NativeAudioPortCallback(IWeakReference* weak_thiz);
    ~NativeAudioPortCallback();

    virtual void onAudioPortListUpdate();
    virtual void onAudioPatchListUpdate();
    virtual void onServiceDied();

private:
    void sendEvent(int event);

    AutoPtr<IWeakReference> mObject;    // Weak ref to AudioPortEventHandlerDelegate Java object to call on
};

NativeAudioPortCallback::NativeAudioPortCallback(IWeakReference* weak_thiz)
{
    // We use a weak reference so the SoundTriggerModule object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject = weak_thiz;
}

NativeAudioPortCallback::~NativeAudioPortCallback()
{
}

void NativeAudioPortCallback::sendEvent(int event)
{
    AudioPortEventHandler::PostEventFromNative(mObject, event, 0, 0, NULL);
}

void NativeAudioPortCallback::onAudioPortListUpdate()
{
    sendEvent(AudioPortEventHandler::AUDIOPORT_EVENT_PORT_LIST_UPDATED);
}

void NativeAudioPortCallback::onAudioPatchListUpdate()
{
    sendEvent(AudioPortEventHandler::AUDIOPORT_EVENT_PATCH_LIST_UPDATED);
}

void NativeAudioPortCallback::onServiceDied()
{
    sendEvent(AudioPortEventHandler::AUDIOPORT_EVENT_SERVICE_DIED);
}

//===================================================================================
//              AudioPortEventHandler::EventHandler
//===================================================================================
AudioPortEventHandler::EventHandler::EventHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AudioPortEventHandler* host)
    : Handler(looper)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode AudioPortEventHandler::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> o;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&o);
    AudioPortEventHandler* host = (AudioPortEventHandler*)IObject::Probe(o);

    if (host == NULL) {
        return NOERROR;
    }

    AutoPtr<IArrayList> listeners;
    {
        AutoLock syncLock(this);
        Int32 what;
        msg->GetWhat(&what);
        if (what == AUDIOPORT_EVENT_NEW_LISTENER) {
            CArrayList::New((IArrayList**)&listeners);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Boolean b;
            host->mListeners->Contains(obj, &b);
            if (b) {
                listeners->Add(obj);
            }
        }
        else {
            listeners = host->mListeners;
        }
    }
    Boolean b;
    if (listeners == NULL || (listeners->IsEmpty(&b), b)) {
        return NOERROR;
    }
    // reset audio port cache if the event corresponds to a change coming
    // from audio policy service or if mediaserver process died.
    Int32 what;
    msg->GetWhat(&what);
    if (what == AUDIOPORT_EVENT_PORT_LIST_UPDATED ||
            what == AUDIOPORT_EVENT_PATCH_LIST_UPDATED ||
            what == AUDIOPORT_EVENT_SERVICE_DIED) {
        Int32 gen;
        host->mAudioManager->ResetAudioPortGeneration(&gen);
    }
    AutoPtr<IArrayList> ports;
    CArrayList::New((IArrayList**)&ports);
    AutoPtr<IArrayList> patches;
    CArrayList::New((IArrayList**)&patches);
    if (what != AUDIOPORT_EVENT_SERVICE_DIED) {
        Int32 status;
        host->mAudioManager->UpdateAudioPortCache(ports, patches, &status);
        if (status != IAudioManager::SUCCESS) {
            return NOERROR;
        }
    }

    switch (what) {
    case AUDIOPORT_EVENT_NEW_LISTENER:
    case AUDIOPORT_EVENT_PORT_LIST_UPDATED: {
        // AudioPort[] portList = ports.toArray(new AudioPort[0]);
        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(0);
        AutoPtr<ArrayOf<IInterface*> > array;
        ports->ToArray(objs, (ArrayOf<IInterface*>**)&array);
        AutoPtr<ArrayOf<IAudioPort*> > portList = ArrayOf<IAudioPort*>::Alloc(array->GetLength());
        for (Int32 i = 0; i < array->GetLength(); i++) {
            portList->Set(i, IAudioPort::Probe(((*array)[i])));
        }

        Int32 num;
        listeners->GetSize(&num);
        for (Int32 i = 0; i < num; i++) {
            AutoPtr<IInterface> obj;
            listeners->Get(i, ((IInterface**)&obj));
            IAudioManagerOnAudioPortUpdateListener::Probe(obj)->OnAudioPortListUpdate(portList);
        }
        if (what == AUDIOPORT_EVENT_PORT_LIST_UPDATED) {
            break;
        }
        // FALL THROUGH
    }
    case AUDIOPORT_EVENT_PATCH_LIST_UPDATED: {
        // AudioPatch[] patchList = patches.toArray(new AudioPatch[0]);
        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(0);
        AutoPtr<ArrayOf<IInterface*> > array;
        patches->ToArray(objs, (ArrayOf<IInterface*>**)&array);
        AutoPtr<ArrayOf<IAudioPatch*> > patchList = ArrayOf<IAudioPatch*>::Alloc(array->GetLength());
        for (Int32 i = 0; i < array->GetLength(); i++) {
            patchList->Set(i, IAudioPatch::Probe(((*array)[i])));
        }

        Int32 num;
        listeners->GetSize(&num);
        for (Int32 i = 0; i < num; i++) {
            AutoPtr<IInterface> obj;
            listeners->Get(i, ((IInterface**)&obj));
            IAudioManagerOnAudioPortUpdateListener::Probe(obj)->OnAudioPatchListUpdate(patchList);
        }
        break;
    }
    case AUDIOPORT_EVENT_SERVICE_DIED: {
        Int32 num;
        listeners->GetSize(&num);
        for (Int32 i = 0; i < num; i++) {
            AutoPtr<IInterface> obj;
            listeners->Get(i, ((IInterface**)&obj));
            IAudioManagerOnAudioPortUpdateListener::Probe(obj)->OnServiceDied();
        }
        break;
    }
    default:
        break;
    }
    return NOERROR;
}

//===================================================================================
//              AudioPortEventHandler
//===================================================================================
AudioPortEventHandler::AudioPortEventHandler()
{
}

AudioPortEventHandler::~AudioPortEventHandler()
{
    NativeFinalize();
}

ECode AudioPortEventHandler::constructor(
    /* [in] */ IAudioManager* audioManager)
{
    mAudioManager = audioManager;
    CArrayList::New((IArrayList**)&mListeners);

    // find the looper for our new event handler
    AutoPtr<ILooper> looper = Looper::GetMainLooper();
    if (looper != NULL) {
        mHandler = new EventHandler(looper, this);
    }

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    NativeSetup(wr);
    return NOERROR;
}

ECode AudioPortEventHandler::RegisterListener(
    /* [in] */ IAudioManagerOnAudioPortUpdateListener* l)
{
    {
        AutoLock syncLock(this);
        mListeners->Add(l);
    }
    if (mHandler != NULL) {
        AutoPtr<IMessage> m;
        mHandler->ObtainMessage(AUDIOPORT_EVENT_NEW_LISTENER, 0, 0, l, (IMessage**)&m);
        Boolean b;
        mHandler->SendMessage(m, &b);
    }
    return NOERROR;
}

ECode AudioPortEventHandler::UnregisterListener(
    /* [in] */ IAudioManagerOnAudioPortUpdateListener* l)
{
    {
        AutoLock syncLock(this);
        mListeners->Remove(l);
    }
    return NOERROR;
}

ECode AudioPortEventHandler::Handler(
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandler;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPortEventHandler::NativeSetup(
    /* [in] */ IWeakReference* weak_this)
{
    android::sp<NativeAudioPortCallback> callback = new NativeAudioPortCallback(weak_this);

    if (android::AudioSystem::addAudioPortCallback(callback) == android::NO_ERROR) {
        AutoLock lock(this);
        callback->incStrong(NULL);
        mNativeCallback = reinterpret_cast<Int64>(callback.get());
    }
    return NOERROR;
}

ECode AudioPortEventHandler::NativeFinalize()
{
    AutoLock lock(this);
    android::sp<NativeAudioPortCallback> callback =
            reinterpret_cast<NativeAudioPortCallback*>(mNativeCallback);
    if (callback != 0) {
        callback->decStrong(NULL);
    }
    mNativeCallback = 0;
    return NOERROR;
}

ECode AudioPortEventHandler::PostEventFromNative(
    /* [in] */ IWeakReference* module_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IInterface> o;
    module_ref->Resolve(EIID_IInterface, (IInterface**)&o);
    AudioPortEventHandler* eventHandler = (AudioPortEventHandler*)IObject::Probe(o);

    if (eventHandler == NULL) {
        return NOERROR;
    }

    if (eventHandler != NULL) {
        AutoPtr<IHandler> handler;
        eventHandler->Handler((IHandler**)&handler);
        if (handler != NULL) {
            AutoPtr<IMessage> m;
            handler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&m);
            Boolean b;
            handler->SendMessage(m, &b);
        }
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
