
#include "elastos/droid/webkit/webview/chromium/native/content/browser/TouchEventSynthesizer.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/TouchEventSynthesizer_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::CPointerProperties;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::EIID_IInputEvent;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                        TouchEventSynthesizer
//=====================================================================
const Int32 TouchEventSynthesizer::MAX_NUM_POINTERS;
const Int32 TouchEventSynthesizer::ACTION_START;
const Int32 TouchEventSynthesizer::ACTION_MOVE;
const Int32 TouchEventSynthesizer::ACTION_CANCEL;
const Int32 TouchEventSynthesizer::ACTION_END;

TouchEventSynthesizer::TouchEventSynthesizer(
    /* [in] */ ContentViewCore* contentViewCore)
    : mContentViewCore(contentViewCore)
{
    mPointerProperties = ArrayOf<IPointerProperties*>::Alloc(MAX_NUM_POINTERS);
    mPointerCoords = ArrayOf<IPointerCoords*>::Alloc(MAX_NUM_POINTERS);
}

ECode TouchEventSynthesizer::SetPointer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 id)
{
    assert (0 <= index && index < MAX_NUM_POINTERS);

    // Convert coordinates from density independent pixels to density dependent pixels.
    Float scaleFactor = mContentViewCore->GetRenderCoordinates()->GetDeviceScaleFactor();

    AutoPtr<IPointerCoords> coords;
    CPointerCoords::New((IPointerCoords**)&coords);
    coords->SetX(scaleFactor * x);
    coords->SetY(scaleFactor * y);
    coords->SetPressure(1.0f);
    (*mPointerCoords)[index] = coords;

    AutoPtr<IPointerProperties> properties;
    CPointerProperties::New((IPointerProperties**)&properties);
    properties->SetId(id);
    (*mPointerProperties)[index] = properties;

    return NOERROR;
}

ECode TouchEventSynthesizer::Inject(
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ Int64 timeInMs)
{
    AutoPtr<IMotionEventHelper> motionEventHelper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&motionEventHelper);

    switch (action) {
        case ACTION_START: {
            mDownTimeInMs = timeInMs;
            AutoPtr<IMotionEvent> event;
            motionEventHelper->Obtain(
                    mDownTimeInMs, timeInMs, IMotionEvent::ACTION_DOWN, 1,
                    mPointerProperties, mPointerCoords,
                    0, 0, 1, 1, 0, 0, 0, 0,
                    (IMotionEvent**)&event);
            mContentViewCore->OnTouchEvent(event);
            AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
            inputEvent->Recycle();

            if (pointerCount > 1) {
                motionEventHelper->Obtain(
                        mDownTimeInMs, timeInMs,
                        IMotionEvent::ACTION_POINTER_DOWN, pointerCount,
                        mPointerProperties, mPointerCoords,
                        0, 0, 1, 1, 0, 0, 0, 0,
                        (IMotionEvent**)&event);
                mContentViewCore->OnTouchEvent(event);
                AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
                inputEvent->Recycle();
            }
            break;
        }
        case ACTION_MOVE: {
            AutoPtr<IMotionEvent> event;
            motionEventHelper->Obtain(mDownTimeInMs, timeInMs,
                    IMotionEvent::ACTION_MOVE,
                    pointerCount, mPointerProperties, mPointerCoords,
                    0, 0, 1, 1, 0, 0, 0, 0,
                    (IMotionEvent**)&event);
            mContentViewCore->OnTouchEvent(event);
            AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
            inputEvent->Recycle();
            break;
        }
        case ACTION_CANCEL: {
            AutoPtr<IMotionEvent> event;
            motionEventHelper->Obtain(
                    mDownTimeInMs, timeInMs, IMotionEvent::ACTION_CANCEL, 1,
                    mPointerProperties, mPointerCoords,
                    0, 0, 1, 1, 0, 0, 0, 0,
                    (IMotionEvent**)&event);
            mContentViewCore->OnTouchEvent(event);
            AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
            inputEvent->Recycle();
            break;
        }
        case ACTION_END: {
            if (pointerCount > 1) {
                AutoPtr<IMotionEvent> event;
                motionEventHelper->Obtain(
                    mDownTimeInMs, timeInMs, IMotionEvent::ACTION_POINTER_UP,
                    pointerCount, mPointerProperties, mPointerCoords,
                    0, 0, 1, 1, 0, 0, 0, 0,
                    (IMotionEvent**)&event);
                mContentViewCore->OnTouchEvent(event);
                AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
                inputEvent->Recycle();
            }

            AutoPtr<IMotionEvent> event;
            motionEventHelper->Obtain(
                    mDownTimeInMs, timeInMs, IMotionEvent::ACTION_UP, 1,
                    mPointerProperties, mPointerCoords,
                    0, 0, 1, 1, 0, 0, 0, 0,
                    (IMotionEvent**)&event);
            mContentViewCore->OnTouchEvent(event);
            AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
            inputEvent->Recycle();
            break;
        }
    }

    return NOERROR;
}

void TouchEventSynthesizer::SetPointer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 id)
{
    AutoPtr<TouchEventSynthesizer> mObj = (TouchEventSynthesizer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("TouchEventSynthesizer", "TouchEventSynthesizer::SetPointer, mObj is NULL");
        return;
    }
    mObj->SetPointer(index, x, y, id);
}

void TouchEventSynthesizer::Inject(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ Int64 timeInMs)
{
    AutoPtr<TouchEventSynthesizer> mObj = (TouchEventSynthesizer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("TouchEventSynthesizer", "TouchEventSynthesizer::Inject, mObj is NULL");
        return;
    }
    mObj->Inject(action, pointerCount, timeInMs);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
