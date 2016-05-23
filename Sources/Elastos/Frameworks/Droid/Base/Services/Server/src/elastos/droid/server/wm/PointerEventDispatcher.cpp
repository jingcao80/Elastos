
#include "elastos/droid/server/wm/PointerEventDispatcher.h"
#include "elastos/droid/server/UiThread.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Server::UiThread;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

PointerEventDispatcher::PointerEventDispatcher()
{
    mListenersArray = ArrayOf<IPointerEventListener*>::Alloc(0);
}

ECode PointerEventDispatcher::constructor(
    /* [in] */ IInputChannel* inputChannel)
{
    AutoPtr<ILooper> l;
    UiThread::GetHandler()->GetLooper((ILooper**)&l);
    return InputEventReceiver::constructor(inputChannel, l);
}

ECode PointerEventDispatcher::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    // try {
    Int32 source;
    if (IMotionEvent::Probe(event) != NULL
            && (event->GetSource(&source), (source & IInputDevice::SOURCE_CLASS_POINTER) != 0)) {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        AutoPtr< ArrayOf<IPointerEventListener*> > listeners;
        {    AutoLock syncLock(mListenersLock);
            if (mListenersArray == NULL) {
                mListenersArray = ArrayOf<IPointerEventListener*>::Alloc(mListeners.GetSize());
                List<AutoPtr<IPointerEventListener> >::Iterator it = mListeners.Begin();
                Int32 i = 0;
                for (; it != mListeners.End(); ++it, ++i) {
                    mListenersArray->Set(i, *it);
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
    {    AutoLock syncLock(mListenersLock);
        AutoPtr<IPointerEventListener> temp = listener;
        List<AutoPtr<IPointerEventListener> >::Iterator it = Find(mListeners.Begin(), mListeners.End(), temp);
        if (it != mListeners.End()) {
            Slogger::E("PointerEventDispatcher", "registerInputEventListener: trying to register %p twice.", listener);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mListeners.PushBack(listener);
        mListenersArray = NULL;
    }
    return NOERROR;
}

ECode PointerEventDispatcher::UnregisterInputEventListener(
    /* [in] */ IPointerEventListener* listener)
{
    {    AutoLock syncLock(mListenersLock);
        AutoPtr<IPointerEventListener> temp = listener;
        List<AutoPtr<IPointerEventListener> >::Iterator it = Find(mListeners.Begin(), mListeners.End(), temp);
        if (it == mListeners.End()) {
            Slogger::E("PointerEventDispatcher", "registerInputEventListener: %p not registered.", listener);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mListeners.Remove(listener);
        mListenersArray = NULL;
    }
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
