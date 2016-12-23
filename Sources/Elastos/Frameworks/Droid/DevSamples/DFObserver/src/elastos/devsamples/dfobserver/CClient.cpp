
#include "CClient.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemLongClickListener;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

static const String TAG("DFObserver::CClient");

//=======================================================================
// UIHandler
//=======================================================================

CClient::UIHandler::UIHandler(
    /* [in] */ CClient* host)
    : Handler(FALSE)
    , mHost(host)
{}

ECode CClient::UIHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);


    switch (what) {
        case MESSAGE_INCOMING_EVENT: {
            StringBuilder sb("Incoming event:  ");
            sb += Object::ToString(obj);
            Logger::I(TAG, sb.ToString());
            mMsgListViewArrayAdapter->Add(CoreUtils::Convert(sb.ToString()));
            break;
        }

        case MESSAGE_UPDATE_UI: {
            Logger::I(TAG, "MESSAGE_UPDATE_UI: %s", TO_CSTR(obj));
            IRunnable::Probe(obj)->Run();
            break;
        }

        case MESSAGE_DOOR_EVENT: {
            StringBuilder sb("Door event:  ");
            sb += Object::ToString(obj);
            Logger::I(TAG, sb.ToString());
            mMsgListViewArrayAdapter->Add(CoreUtils::Convert(sb.ToString()));
            break;
        }

        default:
            break;
    }
    return NOERROR;
}

//=======================================================================
// CClient::MyArrayAdapter
//=======================================================================
CClient::MyArrayAdapter::MyArrayAdapter(
    /* [in] */ CClient* host)
    : mHost(host)
{}

ECode CClient::MyArrayAdapter::Add(
    /* [in] */ IInterface* object)
{
    //only log 25 messages.
    Int32 count;
    GetCount(&count);
    if (count == 25) {
        AutoPtr<IInterface> obj;
        GetItem(0, (IInterface**)&obj);
        Remove(obj);
    }

    return ArrayAdapter::Add(object);
}

//=======================================================================
// CClient::InnerListener
//=======================================================================
CAR_INTERFACE_IMPL_2(CClient::InnerListener, Object, \
    IAdapterViewOnItemClickListener, IAdapterViewOnItemLongClickListener)

CClient::InnerListener::InnerListener(
    /* [in] */ CClient* host)
    : mHost(host)
{}

ECode CClient::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Logger::I(TAG, " >> OnItemClick: TOGGLE_DOOR %d", position);
    AutoPtr<IInterface> obj;
    IAdapterView::Probe(mHost->mDoorListView)->GetItemAtPosition(position, (IInterface**)&obj);
    AutoPtr<IMessage> msg;
    mHost->mBusHandler->ObtainMessage(BusHandler::TOGGLE_DOOR, obj, (IMessage**)&msg);
    Boolean bval;
    mHost->mBusHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CClient::InnerListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> OnItemLongClick: KNOCK_ON_DOOR %d", position);
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    IAdapterView::Probe(mHost->mDoorListView)->GetItemAtPosition(position, (IInterface**)&obj);
    AutoPtr<IMessage> msg;
    mHost->mBusHandler->ObtainMessage(BusHandler::KNOCK_ON_DOOR, obj, (IMessage**)&msg);
    Boolean bval;
    mHost->mBusHandler->SendMessage(msg, &bval);

    *result = TRUE;
    return NOERROR;
}

//=======================================================================
// CClient
//=======================================================================
const Int32 CClient::MESSAGE_INCOMING_EVENT;
const Int32 CClient::MESSAGE_UPDATE_UI;
const Int32 CClient::MESSAGE_DOOR_EVENT;

CAR_OBJECT_IMPL(CClient)

CClient::CClient()
{}

CClient::~CClient()
{
    Logger::I(TAG, " >> Destroy DFObserver::CClient");
}

ECode CClient::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CClient::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    mHandler = new UIHandler(this);

    AutoPtr<MyArrayAdapter> adapter = new MyArrayAdapter(this);
    adapter->constructor(this, R::layout::message);
    mHandler->mMsgListViewArrayAdapter = adapter;

    AutoPtr<IView> view = FindViewById(R::id::MsgListView);
    mMsgListView = IListView::Probe(view);
    IAdapterView::Probe(mMsgListView)->SetAdapter(adapter);

    view = FindViewById(R::id::DoorListView);
    mDoorListView = IListView::Probe(view);
    mDoorListAdapter = new DoorAdapter();
    mDoorListAdapter->constructor(mHandler);
    IAdapterView::Probe(mDoorListView)->SetAdapter(mDoorListAdapter);

    AutoPtr<InnerListener> l = new InnerListener(this);
    IAdapterView::Probe(mDoorListView)->SetOnItemLongClickListener(l);
    IAdapterView::Probe(mDoorListView)->SetOnItemClickListener(l);

    /*
     * Make all AllJoyn calls through a separate handler thread to prevent
     * blocking the UI.
     */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler(mDoorListAdapter, looper);

    /* Connect to an AllJoyn object. */
    AutoPtr<IMessage> msg;
    mBusHandler->ObtainMessage(BusHandler::CONNECT, (IActivity*)this, (IMessage**)&msg);
    Boolean bval;
    mBusHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CClient::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CClient::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CClient::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CClient::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CClient::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    /* Disconnect to prevent resource leaks. */
    Boolean bval;
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &bval);
    return Activity::OnDestroy();
}

ECode CClient::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::I(TAG, " >> OnCreateOptionsMenu()");
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::mainmenu, menu);
    mMenu = menu;

    *result = TRUE;
    return NOERROR;
}

//==========================================================================================
//  CClient::PositiveOnClickListener
//==========================================================================================
CAR_INTERFACE_IMPL(CClient::NilOnClickListener, Object, IDialogInterfaceOnClickListener)

CAR_INTERFACE_IMPL(CClient::PositiveOnClickListener, Object, IDialogInterfaceOnClickListener)

CClient::PositiveOnClickListener::PositiveOnClickListener(
    /* [in] */ CClient* host,
    /* [in] */ IEditText* view)
    : mHost(host)
    , mView(view)
{}

ECode CClient::PositiveOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    AutoPtr<ICharSequence> csq;
    ITextView::Probe(mView)->GetText((ICharSequence**)&csq);
    Logger::I(TAG, " >> OnClick createDoor: %s", TO_CSTR(csq));
    AutoPtr<IMessage> msg;
    mHost->mBusHandler->ObtainMessage(BusHandler::CREATE_DOOR, csq, (IMessage**)&msg);
    Boolean bval;
    return mHost->mBusHandler->SendMessage(msg, &bval);
}


//==========================================================================================
//  CClient::OnMultiChoiceClickListener
//==========================================================================================

CAR_INTERFACE_IMPL(CClient::OnMultiChoiceClickListener, Object, IDialogInterfaceOnMultiChoiceClickListener)

CClient::OnMultiChoiceClickListener::OnMultiChoiceClickListener(
    /* [in] */ CClient* host,
    /* [in] */ ArrayOf<String>* names,
    /* [in] */ ArrayOf<Boolean>* checked)
    : mHost(host)
    , mDoorNames(names)
    , mCheckedDoors(checked)
{}

ECode CClient::OnMultiChoiceClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which,
    /* [in] */ Boolean isChecked)
{
    AutoPtr<IArrayList> doors;
    CArrayList::New((IArrayList**)&doors);
    for (Int32 i = 0; i < mCheckedDoors->GetLength(); i++) {
        if ((*mCheckedDoors)[i]) {
            doors->Add(CoreUtils::Convert((*mDoorNames)[i]));
        }
    }

    Int32 size;
    doors->GetSize(&size);
    if (size > 0) {
        AutoPtr<IMessage> msg;
        mHost->mBusHandler->ObtainMessage(BusHandler::DELETE_DOOR, doors, (IMessage**)&msg);
        Boolean bval;
        return mHost->mBusHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

ECode CClient::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);

    // Handle item selection
    switch (id) {
    case R::id::quit: {
        Logger::I(TAG, " >> OnOptionsItemSelected quit");

        Finish();
        *result = TRUE;
        return NOERROR;
    }

    case R::id::createDoor: {
        Logger::I(TAG, " >> OnOptionsItemSelected createDoor");

        AutoPtr<IAlertDialogBuilder> alert;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&alert);
        alert->SetTitle(CoreUtils::Convert("Create Door"));
        alert->SetMessage(CoreUtils::Convert("Please provide a name for the new door"));
        AutoPtr<IEditText> input;
        CEditText::New(this, (IEditText**)&input);
        alert->SetView(IView::Probe(input));

        AutoPtr<PositiveOnClickListener> pol = new PositiveOnClickListener(this, input);
        alert->SetPositiveButton(Elastos::Droid::R::string::ok, pol);
        AutoPtr<NilOnClickListener> nol = new NilOnClickListener();
        alert->SetNegativeButton(Elastos::Droid::R::string::cancel, nol);

        AutoPtr<IAlertDialog> dlg;
        alert->Show((IAlertDialog**)&dlg);
        *result = TRUE;
        return NOERROR;
    }
    case R::id::deleteDoor: {
        Logger::I(TAG, " >> OnOptionsItemSelected deleteDoor");

        AutoPtr<IAlertDialogBuilder> alert;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&alert);
        alert->SetTitle(CoreUtils::Convert("Delete Doors"));

        AutoPtr<ArrayOf<String> > doorNames = mBusHandler->GetDoorNames();
        AutoPtr<ArrayOf<ICharSequence*> > names = ArrayOf<ICharSequence*>::Alloc(doorNames->GetLength());
        for (Int32 i = 0; i < doorNames->GetLength(); ++i) {
            names->Set(i, CoreUtils::Convert((*doorNames)[i]));
        }
        AutoPtr<ArrayOf<Boolean> > checkedDoors = ArrayOf<Boolean>::Alloc(doorNames->GetLength());
        AutoPtr<OnMultiChoiceClickListener> mccl = new OnMultiChoiceClickListener(this, doorNames, checkedDoors);
        alert->SetMultiChoiceItems(names, checkedDoors, mccl);

        AutoPtr<NilOnClickListener> nol = new NilOnClickListener();
        alert->SetPositiveButton(Elastos::Droid::R::string::ok, nol);
        alert->SetNegativeButton(Elastos::Droid::R::string::cancel, nol);

        AutoPtr<IAlertDialog> dlg;
        alert->Show((IAlertDialog**)&dlg);
        *result = TRUE;
        return NOERROR;
    }

    default:
        return Activity::OnOptionsItemSelected(item, result);
    }

    return NOERROR;
}


} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos
