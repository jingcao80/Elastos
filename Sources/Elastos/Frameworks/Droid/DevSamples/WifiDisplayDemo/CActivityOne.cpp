
#include "CActivityOne.h"
#include "R.h"
#include <R.h>
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <unistd.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::CObjectStringMap;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ITextView;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiDisplayDemo {

String CActivityOne::TAG("WifiDisplayDemo");

//==========================================================================================
// CActivityOne::MyListener
//==========================================================================================
CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    else if (riid == EIID_IRadioGroupOnCheckedChangeListener) {
        return this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
    /* [in] */  IRadioGroup* group,
    /* [in] */  Int32 checkedId)
{
    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Slogger::D("CActivityOne::ListView", "OnItemClick position %d", position);
    ITextView* tv = ITextView::Probe(view);
    assert(tv != NULL);

    AutoPtr<ICharSequence> cs;
    tv->GetText((ICharSequence**)&cs);
    String deviceInfo;
    cs->ToString(&deviceInfo);

    AutoPtr<ArrayOf<String> > infos;
    StringUtils::Split(deviceInfo, String("="), (ArrayOf<String>**)&infos);
    String name = (*infos)[0];
    String address = (*infos)[1];
    mHost->ConnectToWifiDisplay(address);
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
        /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    if (id == R::id::RenameButton) {
        mHost->OnRenameButtonClick();
    }
    else if (id == R::id::ScanButton) {
        mHost->OnScanButtonClick();
    }
    else if (id == R::id::ConnectButton) {
        mHost->OnConnectButtonClick();
    }
    else if (id == R::id::DisconnectButton) {
        mHost->OnDisconnectButtonClick();
    }
    else if (id == R::id::ForgetButton) {
        mHost->OnForgetButtonClick();
    }
    else if (id == R::id::GetStatusButton) {
        mHost->OnGetStatusButtonClick();
    }
    return NOERROR;
}

//======================================================================
// CActivityOne::DisplayListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DisplayListener, IDisplayListener)

CActivityOne::DisplayListener::DisplayListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::DisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayAdded %d", displayId);
    return NOERROR;
}

ECode CActivityOne::DisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayChanged %d", displayId);
    return NOERROR;
}

ECode CActivityOne::DisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayRemoved %d", displayId);
    return NOERROR;
}

//======================================================================
// CActivityOne
//======================================================================
CActivityOne::CActivityOne()
{}

ECode CActivityOne::ShowToast(
    /* [in] */ const String& info,
    /* [in] */ Int32 style)
{
    // AutoPtr<ICharSequence> cs;
    // CStringWrapper::New(info, (ICharSequence**)&cs);
    // AutoPtr<IToastHelper> helper;
    // CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    // AutoPtr<IToast> toast;
    // helper->MakeText(this, cs, style, (IToast**)&toast);
    // toast->Show();

    AppendInfo(info);
    return NOERROR;
}

ECode CActivityOne::SetupViews()
{
    AutoPtr<MyListener> l = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::InfoTextView);
    assert(view != NULL);
    mInfoTextView = ITextView::Probe(view);
    view = FindViewById(R::id::DisplayListView);
    assert(view != NULL);
    mDisplayListView = IListView::Probe(view);

    view = FindViewById(R::id::ScanButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::RenameButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::ConnectButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DisconnectButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::ForgetButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::GetStatusButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DisplayListView);
    mDisplayListView = IListView::Probe(view);
    assert(mDisplayListView != NULL);

    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mDisplayListView->SetDivider(drawable);
    mDisplayListView->SetDividerHeight(1);
    mDisplayListView->SetOnItemClickListener(l);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D(TAG, " >> OnCreate");
    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::main);

    SetupViews();

    if (mDisplayManager == NULL) {
        GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&mDisplayManager);

        AutoPtr<IDisplayListener> listener = new DisplayListener(this);
        mDisplayManager->RegisterDisplayListener(listener, NULL);
    }

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    Slogger::D(TAG, " >> OnResume");
    Activity::OnResume();
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Slogger::D(TAG, " >> OnPause");
    Activity::OnPause();
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    Slogger::D(TAG, " >> OnStop");
    OnDisconnectButtonClick();
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    Slogger::D(TAG, " >> OnDestroy");
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

ECode CActivityOne::UpdateListView()
{
    Slogger::D(TAG, "Update Wifi Displays, count: %d", mWifiDisplays ? mWifiDisplays->GetLength() : 0);

    if (mWifiDisplays == NULL || mWifiDisplays->GetLength() == 0) {
        mDisplayListView->SetAdapter(NULL);
        return NOERROR;
    }

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    String address, name;
    IWifiDisplay* display;
    for (Int32 i = 0; i < mWifiDisplays->GetLength(); ++i) {
        display = (*mWifiDisplays)[i];
        display->GetDeviceAddress(&address);
        display->GetDeviceName(&name);

        StringBuilder sb(name);
        sb += "=";
        sb += address;

        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    mDisplayListView->SetAdapter(adapter.Get());
    return NOERROR;
}

ECode CActivityOne::AppendInfo(
    /* [in] */ const String& info)
{
    AutoPtr<ICharSequence> curInfo;
    mInfoTextView->GetText((ICharSequence**)&curInfo);

    StringBuilder sb("");
    if (curInfo != NULL) {
        String temp;
        curInfo->ToString(&temp);
        sb += temp;
        sb += "\n";
    }

    sb += info;

    curInfo = sb.ToCharSequence();
    mInfoTextView->SetText(curInfo);
    return NOERROR;
}

String CActivityOne::GetAddress()
{
    if (mWifiDisplays == NULL || mWifiDisplays->GetLength() == 0) {
        return String(NULL);
    }

    String address;
    IWifiDisplay* display = (*mWifiDisplays)[0];
    display->GetDeviceAddress(&address);
    return address;
}

ECode CActivityOne::ConnectToWifiDisplay(
    /* [in] */ const String& address)
{
    assert(!address.IsNullOrEmpty());

    if (mDisplayManager) {
        Slogger::D(TAG, "ConnectWifiDisplay: %s", address.string());
        mDisplayManager->ConnectWifiDisplay(address);
    }
    return NOERROR;
}

ECode CActivityOne::OnScanButtonClick()
{
    if (mDisplayManager) {
        mDisplayManager->ScanWifiDisplays();
    }
    return NOERROR;
}

ECode CActivityOne::OnRenameButtonClick()
{
    String address = GetAddress();
    if (address.IsNull()) {
        Slogger::D(TAG, "No wifi display found!");
        return NOERROR;
    }

    if (mDisplayManager) {
        String alias("NicePeerName");
        mDisplayManager->RenameWifiDisplay(address, alias);
    }
    return NOERROR;
}

ECode CActivityOne::OnConnectButtonClick()
{
    String address = GetAddress();
    if (address.IsNull()) {
        Slogger::D(TAG, "No wifi display found!");
        return NOERROR;
    }

    return ConnectToWifiDisplay(address);
}

ECode CActivityOne::OnDisconnectButtonClick()
{
    if (mDisplayManager) {
        mDisplayManager->DisconnectWifiDisplay();
    }
    return NOERROR;
}

ECode CActivityOne::OnForgetButtonClick()
{
    String address = GetAddress();
    if (address.IsNull()) {
        Slogger::D(TAG, "No wifi display found!");
        return NOERROR;
    }

    if (mDisplayManager) {
        mDisplayManager->ForgetWifiDisplay(address);
    }
    return NOERROR;
}

ECode CActivityOne::OnGetStatusButtonClick()
{
    if (mDisplayManager) {
        AutoPtr<IWifiDisplayStatus> status;
        mDisplayManager->GetWifiDisplayStatus((IWifiDisplayStatus**)&status);

        mWifiDisplays = NULL;
        if (status != NULL) {
            String statusInfo;
            status->ToString(&statusInfo);
            Slogger::D(TAG, "GetWifiDisplayStatus: %s", statusInfo.string());

            status->GetAvailableDisplays((ArrayOf<IWifiDisplay*>**)&mWifiDisplays);
        }
        else {
            Slogger::E(TAG, "GetWifiDisplayStatus: failed!");
        }

        UpdateListView();
    }
    return NOERROR;
}

ECode CActivityOne::ResetData()
{
    mWifiDisplays = NULL;
    UpdateListView();
    return NOERROR;
}


} // namespace WifiDisplayDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
