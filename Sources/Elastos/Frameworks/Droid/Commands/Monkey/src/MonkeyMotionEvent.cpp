#include "MonkeyMotionEvent.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Core.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyMotionEvent::MonkeyMotionEvent()
    : MonkeyEvent()
    , mFlags(0)
    , mEdgeFlags(0)
{
}

MonkeyMotionEvent::MonkeyMotionEvent(
    /* [in] */ Int32 type,
    /* [in] */ Int32 source,
    /* [in] */ Int32 action)
    : MonkeyEvent(type)
    , mDownTime(-1)
    , mEventTime(-1)
    , mAction(action)
    , mPointers(new HashMap<Int32, AutoPtr<IPointerCoords> >())
    , mXPrecision(1)
    , mYPrecision(1)
    , mSource(source)
    , mFlags(0)
    , mEdgeFlags(0)
{
}

MonkeyMotionEvent::~MonkeyMotionEvent()
{
}

ECode MonkeyMotionEvent::Init(
    /* [in] */ Int32 type,
    /* [in] */ Int32 source,
    /* [in] */ Int32 action)
{
    mSource = source;
    mAction = action;
    mDownTime = -1;
    mEventTime = -1;
    mPointers = new HashMap<Int32, AutoPtr<IPointerCoords> >();
    mXPrecision = 1;
    mYPrecision = 1;
    MonkeyEvent::Init(type);
    return NOERROR;
}

Int32 MonkeyMotionEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    AutoPtr<IMotionEvent> me = GetEvent();
    if(verbose > 1 || (verbose > 0 && !mIntermediateNote))
    {
        StringBuilder msg(String(":Sending "));
        msg.AppendString(GetTypeLabel());
        msg.AppendString(String(" ("));
        Int32 actionMasked;
        me->GetActionMasked(&actionMasked);
        switch(actionMasked)
        {
            case IMotionEvent::ACTION_DOWN:
                msg.AppendString(String("ACTION_DOWN"));
                break;
            case IMotionEvent::ACTION_MOVE:
                msg.AppendString(String("ACTION_MOVE"));
                break;
            case IMotionEvent::ACTION_UP:
                msg.AppendString(String("ACTION_UP"));
                break;
            case IMotionEvent::ACTION_CANCEL:
                msg.AppendString(String("ACTION_CANCEL"));
                break;
            case IMotionEvent::ACTION_POINTER_DOWN:
                {
                    Int32 actionIndex, pointerId;
                    me->GetActionIndex(&actionIndex);
                    me->GetPointerId(actionIndex, &pointerId);
                    msg.AppendString(String("ACTION_POINTER_DOWN")
                        + StringUtils::Int32ToString(pointerId, 10));
                    break;
                }
            case IMotionEvent::ACTION_POINTER_UP:
                {
                    Int32 actionIndex, pointerId;
                    me->GetActionIndex(&actionIndex);
                    me->GetPointerId(actionIndex, &pointerId);
                    msg.AppendString(String("ACTION_POINTER_UP")
                        + StringUtils::Int32ToString(pointerId, 10));
                    break;
                }

            default:
                Int32 action;
                me->GetAction(&action);
                msg.AppendInt32(action);
                break;
        }//end switch.

        msg.AppendString(String("):"));
        Int32 pointerCount;
        me->GetPointerCount(&pointerCount);
        for (Int32 i = 0; i < pointerCount; i++) {
            Int32 pointerId;
            Float x, y;
            me->GetX(i, &x);
            me->GetY(i, &y);
            me->GetPointerId(i, &pointerId);
            msg.AppendString(String(" "));
            msg.AppendInt32(pointerId);
            msg.AppendString(String(":("));
            msg.AppendFloat(x);
            msg.AppendString(String(", "));
            msg.AppendFloat(y);
            msg.AppendString(String(")"));
        }

        String tmp;
        msg.ToString(&tmp);
        PFL_EX(tmp.string())
    }

    Boolean ret = FALSE;
    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> inputManager;
    helper->GetInstance((IInputManager**)&inputManager);
    inputManager->InjectInputEvent(me,IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_RESULT, &ret);
    me->Recycle();

    return ret == FALSE ? IMonkeyEvent::INJECT_FAIL : IMonkeyEvent::INJECT_SUCCESS;
}

ECode MonkeyMotionEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AddPointer(id, x, y, 0, 0);
    return NOERROR;
}

ECode MonkeyMotionEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size)
{
    AutoPtr<IPointerCoords> pc;
    CPointerCoords::New((IPointerCoords**)&pc);
    pc->SetX(x);
    pc->SetY(y);
    pc->SetPressure(pressure);
    pc->SetSize(size);
    (*mPointers)[id] = pc;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetIntermediateNote(
    /* [in] */ Boolean b)
{
    mIntermediateNote = b;
    return NOERROR;
}

Boolean MonkeyMotionEvent::GetIntermediateNote()
{
    return mIntermediateNote;
}

Int32 MonkeyMotionEvent::GetAction()
{
    return mAction;
}

Int64 MonkeyMotionEvent::GetDownTime()
{
    return mDownTime;
}

Int64 MonkeyMotionEvent::GetEventTime()
{
    return mEventTime;
}

ECode MonkeyMotionEvent::SetDownTime(
    /* [in] */ Int64 downTime)
{
    mDownTime = downTime;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetEventTime(
    /* [in] */ Int64 eventTime)
{
    mEventTime = eventTime;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetMetaState(
    /* [in] */ Int32 metaState)
{
    mMetaState = metaState;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetPrecision(
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision)
{
    mXPrecision = xPrecision;
    mYPrecision = yPrecision;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetDeviceId(
    /* [in] */ Int32 deviceId)
{
    mDeviceId = deviceId;
    return NOERROR;
}

ECode MonkeyMotionEvent::SetEdgeFlags(
    /* [in] */ Int32 edgeFlags)
{
    mEdgeFlags = edgeFlags;
    return NOERROR;
}

Boolean MonkeyMotionEvent::IsThrottlable()
{
    return (GetAction() == IMotionEvent::ACTION_UP);
}

AutoPtr<IMotionEvent> MonkeyMotionEvent::GetEvent()
{
    AutoPtr<IMotionEvent> pMe;
    Int32 pointerCount = mPointers->GetSize();
    AutoPtr<ArrayOf<Int32> > pointerIds = ArrayOf<Int32>::Alloc(pointerCount);
    AutoPtr<ArrayOf<IPointerCoords*> > pointerCoords = ArrayOf<IPointerCoords*>::Alloc(pointerCount);

    for (Int32 i = 0; i < pointerCount; ++i) {
        AutoPtr<IPointerCoords> pc;
        CPointerCoords::New((IPointerCoords**)&pc);
        pointerCoords->Set(i, pc);
    }

    HashMap< Int32, AutoPtr<IPointerCoords> >::Iterator it = mPointers->Begin();
    for(Int32 i = 0; it != mPointers->End(); ++it, ++i){
        pointerIds->Set(i, it->mFirst);
        pointerCoords->Set(i, it->mSecond);
    }

    AutoPtr<IMotionEventHelper> helper;
    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper))
    helper->Obtain(mDownTime, mEventTime < 0 ? SystemClock::GetUptimeMillis() : mEventTime
        , mAction, pointerCount, pointerIds, pointerCoords, mMetaState, mXPrecision
        , mYPrecision, mDeviceId, mEdgeFlags, mSource, mFlags, (IMotionEvent**)&pMe);
    return pMe;
}
} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

