#include "DoorAdapter.h"
#include "CClient.h"
#include "R.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

static const String TAG("DoorAdapter");

DoorAdapter::UpdateRunnable::UpdateRunnable(
    /* [in] */ DoorAdapter* host)
    : mHost(host)
{
}

ECode DoorAdapter::UpdateRunnable::Run()
{
    Logger::I(TAG, " >> NotifyDataSetChanged");
    return mHost->NotifyDataSetChanged();
}

DoorAdapter::UpdateUIRunnable::UpdateUIRunnable(
    /* [in] */ DoorAdapter* host,
    /* [in] */ IDoorAdapterItem* door,
    /* [in] */ Boolean add)
    : mHost(host)
    , mDoor(door)
    , mIsAdd(add)
{
}

ECode DoorAdapter::UpdateUIRunnable::Run()
{
    AutoPtr<IDoorAdapterItem> item = mDoor;
   if (mIsAdd) {
        Logger::I(TAG, " >> Add item %s", TO_CSTR(mDoor));
        mHost->mList.PushBack(item);
    }
    else {
        Logger::I(TAG, " >> Remove item %s", TO_CSTR(mDoor));
        mHost->mList.Remove(item);
    }
    return mHost->NotifyDataSetChanged();
}

DoorAdapter::DoorViewHolder::DoorViewHolder(
    /* [in] */ ITextView* name,
    /* [in] */ ICheckBox* isOpen)
    : mName(name)
    , mIsOpen(isOpen)
{
}

//=======================================================================================
// DoorAdapter
//=======================================================================================
ECode DoorAdapter::constructor(
    /* [in] */ IHandler* handle)
{
    mHandler = handle;
    return NOERROR;
}

ECode DoorAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mList.GetSize();
    return NOERROR;
}

ECode DoorAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = mList[position];
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode DoorAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = (Int64)(mList[position].Get());
    return NOERROR;
}

ECode DoorAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* inConvertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> convertView = inConvertView;
    if (convertView == NULL) {
        AutoPtr<IContext> ctx;
        IView::Probe(parent)->GetContext((IContext**)&ctx);

        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(ctx, (ILayoutInflater**)&inflater);
        inflater->Inflate(R::layout::door, parent, FALSE, (IView**)&convertView);
        AutoPtr<IView> view;
        convertView->FindViewById(R::id::Name, (IView**)&view);
        AutoPtr<ITextView> name = ITextView::Probe(view);
        view = NULL;
        convertView->FindViewById(R::id::IsOpen, (IView**)&view);
        AutoPtr<ICheckBox> check = ICheckBox::Probe(view);
        AutoPtr<DoorViewHolder> holder = new DoorViewHolder(name, check);
        convertView->SetTag((IObject*)holder.Get());
    }

    AutoPtr<IInterface> obj;
    convertView->GetTag((IInterface**)&obj);
    DoorViewHolder* data = (DoorViewHolder*)IObject::Probe(obj);
    AutoPtr<IDoorAdapterItem> item = mList[position];
    String name;
    item->GetName(&name);
    Boolean bval;
    item->GetIsOpen(&bval);
    data->mName->SetText(CoreUtils::Convert(name));
    ICheckable::Probe(data->mIsOpen)->SetChecked(bval);
    *result = convertView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/** Add an extra door to the adapter and update the UI accordingly. */
ECode DoorAdapter::Add(
    /* [in] */ IDoorAdapterItem* door)
{
    return UpdateUI(door, TRUE);
}

/** Removes a door from the adapter and update the UI accordingly. */
ECode DoorAdapter::Remove(
    /* [in] */ IDoorAdapterItem* door)
{
    AutoPtr<IDoorAdapterItem> item = door;
    List<AutoPtr<IDoorAdapterItem> >::Iterator it = Find(mList.Begin(), mList.End(), item);
    if (it != mList.End()) {
        UpdateUI(door, FALSE);
    }
    return NOERROR;
}

/** Updates the UI. Make sure the latest changes are shown on the UI. */
ECode DoorAdapter::UpdateUI(
    /* [in] */ IDoorAdapterItem* door,
    /* [in] */ Boolean add)
{
    AutoPtr<IRunnable> runnable = new UpdateUIRunnable(this, door, add);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(CClient::MESSAGE_UPDATE_UI, runnable, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

/** Show a notification in the UI upon receiving an signal from a door. */
ECode DoorAdapter::SendSignal(
    /* [in] */ const String& string)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(CClient::MESSAGE_INCOMING_EVENT, CoreUtils::Convert(string), (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

/** update UI after receiving an event from a door. */
ECode DoorAdapter::PropertyUpdate(
    /* [in] */ IDoorAdapterItem* item)
{
    String name;
    item->GetName(&name);
    Boolean isOpen;
    item->GetIsOpen(&isOpen);
    StringBuilder sb(name);
    sb += " ";
    sb += isOpen ? "opened" : "closed";
    SendSignal(sb.ToString());

    Logger::I(TAG, " >> PropertyUpdate: %s", sb.ToString().string());

    AutoPtr<IRunnable> runnable = new UpdateRunnable(this);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(CClient::MESSAGE_UPDATE_UI, runnable, (IMessage**)&msg);
    mHandler->SendMessage(msg, &isOpen);
    return NOERROR;
}

/** Sends notification of events from local provided doors. */
ECode DoorAdapter::SendDoorEvent(
    /* [in] */ const String& string)
{
    Logger::I(TAG, " >> SendDoorEvent: %s", string.string());

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(CClient::MESSAGE_DOOR_EVENT, CoreUtils::Convert(string), (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}


} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos
