
#include "CDoorService.h"
#include <elastos/core/Thread.h>

using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::GlobalBroadcast_Off;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::ISignalEmitter;
using Org::Alljoyn::Bus::CSignalEmitter;
// using Org::Alljoyn::Bus::CPropertyChangedEmitter;
using Elastos::Core::Thread;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

CAR_INTERFACE_IMPL_3(CDoorService, Object, IDoorService, IDoor, IBusObject)

CAR_OBJECT_IMPL(CDoorService)

CDoorService::CDoorService()
    : mIsOpen(FALSE)
    , mKeyCode(0)
{}

CDoorService::~CDoorService()
{}

ECode CDoorService::constructor(
    /* [in] */ const String& location,
    /* [in] */ IHandler* handler)
{
    mLocation = location;
    mBushandler = (BusHandler*)handler;
    assert(0 && "TODO");
    // CPropertyChangedEmitter::New(this, IBusAttachment::SESSION_ID_ALL_HOSTED,
    //     GlobalBroadcast_Off, (IPropertyChangedEmitter**)&mPce);
    return NOERROR;
}

ECode CDoorService::GetIsOpen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    SendUiMessage(String("Property IsOpen is queried"));
    *value = mIsOpen;
    return NOERROR;
}

ECode CDoorService::GetLocation(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    SendUiMessage(String("Property Location is queried"));
    *value = mLocation;
    return NOERROR;
}

ECode CDoorService::GetKeyCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    SendUiMessage(String("Property KeyCode is queried"));
    *value = mKeyCode;
    return NOERROR;
}

ECode CDoorService::OpenRunnable::Run()
{
    return mHost->SendDoorEvent(TRUE);
}

ECode CDoorService::Open()
{
    SendUiMessage(String("Method Open is called"));
    if (!mIsOpen) {
        mIsOpen = TRUE;
        // Schedule a task to send out an event to all interested parties.
        AutoPtr<IRunnable> runnable = new OpenRunnable(this);
        SendWorkerMessage(runnable);
    }
    return NOERROR;
}

ECode CDoorService::CloseRunnable::Run()
{
    return mHost->SendDoorEvent(FALSE);
}

ECode CDoorService::Close()
{
    SendUiMessage(String("Method Close is called"));
    if (mIsOpen) {
        mIsOpen = FALSE;
        // Schedule a task to send out an event to all interested parties.
        AutoPtr<IRunnable> runnable = new CloseRunnable(this);
        SendWorkerMessage(runnable);
    }
    return NOERROR;
}

ECode CDoorService::KnockAndRunRunnable::Run()
{
    if (!mHost->mIsOpen) {
        mHost->mIsOpen = true;
        mHost->SendDoorEvent(TRUE);
    }

    Thread::Sleep(250);

    // Create a signal emitter and send out the personPassedTHrough
    // signal
    AutoPtr<ISignalEmitter> se;
    CSignalEmitter::New((IBusObject*)this, IBusAttachment::SESSION_ID_ALL_HOSTED,
        GlobalBroadcast_Off, (ISignalEmitter**)&se);
    AutoPtr<IDoor> door;
    se->GetInterface(EIID_IDoor, (IInterface**)&door);
    door->PersonPassedThrough(String("owner"));

    // Close the door and send an event
    mHost->mIsOpen = FALSE;
    mHost->SendDoorEvent(FALSE);

    assert(0 && "TODO");
    // Invalidate the keyCode
    // mHost->mPce->PropertyChanged(
    //     IDoor::DOOR_INTF_NAME, String("KeyCode"), new Variant(0));

    return NOERROR;
}

ECode CDoorService::KnockAndRun()
{
    SendUiMessage(String("Method KnockAndRun is called"));
    // Schedule a background task to to the knock and run behavior
    AutoPtr<IRunnable> runnable = new KnockAndRunRunnable(this);
    SendWorkerMessage(runnable);
    return NOERROR;
}

ECode CDoorService::PersonPassedThrough(
    /* [in] */ const String& person)
{
    // personPassedThrough is a signal. No implementation is required.
    return NOERROR;
}

ECode CDoorService::GetInternalLocation(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mLocation;
    return NOERROR;
}

ECode CDoorService::SendDoorEvent(
    /* [in] */ Boolean b)
{
    assert(0 && "TODO");
    // mPce->PropertyChanged(IDoor::DOOR_INTF_NAME, String("IsOpen"), new Variant(b));
    return NOERROR;
}

void CDoorService::SendUiMessage(
    /* [in] */ const String& string)
{
    mBushandler->SendUIMessage(string, this);
}

void CDoorService::SendWorkerMessage(
    /* [in] */ IRunnable* runnable)
{
    AutoPtr<IMessage> msg;
    mBushandler->ObtainMessage(BusHandler::EXECUTE_TASK, runnable, (IMessage**)&msg);
    Boolean bval;
    mBushandler->SendMessage(msg, &bval);
}


} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos