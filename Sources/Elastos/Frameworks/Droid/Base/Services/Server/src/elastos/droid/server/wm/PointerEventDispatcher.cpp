
#include "wm/PointerEventDispatcher.h"
#include "UiThread.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::IInputDevice;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

PointerEventDispatcher::PointerEventDispatcher(
    /* [in] */ IInputChannel* inputChannel)
{
    mListenersArray = ArrayOf<IPointerEventListener>::Alloc(0);

    AutoPtr<ILooper> l;
    UiThread::GetHandler()->GetLooper((ILooper**)&l);
    Init(inputChannel, l);
}

ECode PointerEventDispatcher::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    // try {
    Int32 source;
    if (IMotionEvent::Probe(event) != NULL
            && (event->GetSource(&source), source & IInputDevice::SOURCE_CLASS_POINTER != 0)) {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        AutoPtr< ArrayOf<IPointerEventListener*> > listeners;
        synchronized (mListenersLock) {
            if (mListenersArray == NULL) {
                mListenersArray = ArrayOf<IPointerEventListener*>::Alloc(mListeners->GetSize());
                List<AutoPtr<IPointerEventListener> >::Iterator it = mListenersArray.Begin();
                Int32 i = 0;
                for (; it != mListenersArray.End(); ++it, ++i) {
                    mListeners->Set(i, *it);
                }
            }
            listeners = mListenersArray;
        }
        for (Int32 i = 0; i < listeners->GetLength(); ++i) {
            (*listeners)[i]->OnPointerEvent(motionEvent);
        }
    }
    // } finally {
    //     finishInputEvent(event, false);
    // }
    return FinishInputEvent(event, FALSE);
}

ECode PointerEventDispatcher::RegisterInputEventListener(
    /* [in] */ IPointerEventListener* listener)
{
    synchronized (mListenersLock) {
        List<AutoPtr<IPointerEventListener> >::Iterator it = Find(mListeners.Begin(), mListeners.End(), listener);
        if (it == mListeners.End()) {
            Slogger::E("PointerEventDispatcher", "registerInputEventListener: trying to register %p twice.", listener);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mListeners.PushBack(listener);
        mListenersArray = NULL;
    }
}

ECode PointerEventDispatcher::UnregisterInputEventListener(
    /* [in] */ IPointerEventListener* listener)
{
    synchronized (mListenersLock) {
        List<AutoPtr<IPointerEventListener> >::Iterator it = Find(mListeners.Begin(), mListeners.End(), listener);
        if (it == mListeners.End()) {
            Slogger::E("PointerEventDispatcher", "registerInputEventListener: %p not registered.", listener);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mListeners.Erase(listener);
        mListenersArray = NULL;
    }
}

} // Wm
} // Server
} // Droid
} // Elastos
