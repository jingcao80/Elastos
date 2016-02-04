#include "elastos/droid/media/AudioPortEventHandler.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Looper;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

const String AudioPortEventHandler::TAG("AudioPortEventHandler");

const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_PORT_LIST_UPDATED = 1;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_PATCH_LIST_UPDATED = 2;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_SERVICE_DIED = 3;
const Int32 AudioPortEventHandler::AUDIOPORT_EVENT_NEW_LISTENER = 4;

ECode AudioPortEventHandler::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IArrayList> listeners;
    synchronized(this) {
        Int32 what;
        msg->GetWhat(&what);
        if (what == AUDIOPORT_EVENT_NEW_LISTENER) {
            CArrayList::New((IArrayList**)&listeners);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Boolean b;
            mHost->mListeners->Contains(obj, &b);
            if (b) {
                listeners->Add(obj);
            }
        } else {
            listeners = mHost->mListeners;
        }
    }
    Boolean b;
    if (listeners->IsEmpty(&b), b) {
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
        mHost->mAudioManager->ResetAudioPortGeneration(&gen);
    }
    AutoPtr<IArrayList> ports;
    CArrayList::New((IArrayList**)&ports);
    AutoPtr<IArrayList> patches;
    CArrayList::New((IArrayList**)&patches);
    if (what != AUDIOPORT_EVENT_SERVICE_DIED) {
        Int32 status;
        mHost->mAudioManager->UpdateAudioPortCache(ports, patches, &status);
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

AudioPortEventHandler::AudioPortEventHandler(
    /* [in] */ IAudioManager* audioManager)
{
    mAudioManager = audioManager;
    CArrayList::New((IArrayList**)&mListeners);

    // find the looper for our new event handler
    AutoPtr<ILooper> looper = Looper::GetMainLooper();

    if (looper != NULL) {
        mHandler = new EventHandler(this, looper);
    }
    else {
        mHandler = NULL;
    }

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    NativeSetup(wr);
}

AudioPortEventHandler::~AudioPortEventHandler()
{
    NativeFinalize();
}

ECode AudioPortEventHandler::RegisterListener(
    /* [in] */ IAudioManagerOnAudioPortUpdateListener* l)
{
    synchronized(this) {
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
    synchronized(this) {
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
    /* [in] */ IInterface* module_this)
{
//TODO: Need JNI
    ALOGV("eventHandlerSetup");

    // sp<JNIAudioPortCallback> callback = new JNIAudioPortCallback(env, thiz, weak_this);

    // AudioSystem::setAudioPortCallback(callback);
    return NOERROR;
}

ECode AudioPortEventHandler::NativeFinalize()
{
//TODO: Need JNI
    ALOGV("eventHandlerFinalize");

    // sp<JNIAudioPortCallback> callback;

    // AudioSystem::setAudioPortCallback(callback);
    return NOERROR;
}

ECode AudioPortEventHandler::PostEventFromNative(
    /* [in] */ IInterface* module_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IWeakReference> wr = IWeakReference::Probe(module_ref);
    AutoPtr<IInterface> o;
    wr->Resolve(EIID_IInterface, (IInterface**)&o);
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
