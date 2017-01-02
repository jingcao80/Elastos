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

#include "elastos/droid/systemui/usb/UsbDebuggingActivity.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

CAR_INTERFACE_IMPL(UsbDebuggingActivity::InnerListener, Object, IDialogInterfaceOnClickListener)

UsbDebuggingActivity::InnerListener::InnerListener(
    /* [in] */ UsbDebuggingActivity* host)
    : mHost(host)
{}

ECode UsbDebuggingActivity::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

//-------------------------------------------------------------------------
// UsbDebuggingActivity::UsbDisconnectedReceiver
//-------------------------------------------------------------------------

UsbDebuggingActivity::UsbDisconnectedReceiver::UsbDisconnectedReceiver(
    /* [in] */ IActivity* activity,
    /* [in] */ UsbDebuggingActivity* host)
    : mHost(host)
    , mActivity(activity)
{
}

ECode UsbDebuggingActivity::UsbDisconnectedReceiver::OnReceive(
    /* [in] */ IContext* content,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (!IUsbManager::ACTION_USB_STATE.Equals(action)) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean connected;
    intent->GetBooleanExtra(IUsbManager::USB_CONNECTED, FALSE, &connected);
    if (!connected) {
        mActivity->Finish();
    }
    return NOERROR;
}

//-------------------------------------------------------------------------
// UsbDebuggingActivity
//-------------------------------------------------------------------------

const String UsbDebuggingActivity::TAG("UsbDebuggingActivity");

CAR_INTERFACE_IMPL(UsbDebuggingActivity, AlertActivity, IDialogInterfaceOnClickListener)

ECode UsbDebuggingActivity::constructor()
{
    return NOERROR;
}

ECode UsbDebuggingActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AlertActivity::OnCreate(icicle);

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Int32 i;
    sp->GetInt32(String("service.adb.tcp.port"), 0, &i);

    if (i == 0) {
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, this);
    }

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String fingerprints;
    intent->GetStringExtra(String("fingerprints"), &fingerprints);
    intent->GetStringExtra(String("key"), &mKey);;

    if (fingerprints.IsNull() || mKey.IsNull()) {
        Finish();
        return E_NULL_POINTER_EXCEPTION;
    }

    const AutoPtr<IAlertControllerAlertParams> ap = AlertActivity::mAlertParams;

    String str;
    GetString(R::string::usb_debugging_title, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    ap->SetTitle(cs);

    AutoPtr<ArrayOf<IInterface*> > fa = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs2;
    CString::New(fingerprints, (ICharSequence**)&cs2);
    fa->Set(0, cs2);
    String str2;
    GetString(R::string::usb_debugging_message, fa, &str2);
    AutoPtr<ICharSequence> cs3;
    CString::New(str2, (ICharSequence**)&cs3);
    ap->SetMessage(cs3);

    String str3;
    GetString(R::string::ok, &str3);
    AutoPtr<ICharSequence> cs4;
    CString::New(str3, (ICharSequence**)&cs4);
    ap->SetPositiveButtonText(cs4);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    String str4;
    GetString(Elastos::Droid::R::string::cancel, &str4);
    AutoPtr<ICharSequence> cs5;
    CString::New(str4, (ICharSequence**)&cs5);
    ap->SetNegativeButtonText(cs5);
    ap->SetPositiveButtonListener(listener);
    ap->SetNegativeButtonListener(listener);

    // add "always allow" checkbox
    AutoPtr<IContext> ctx;
    ap->GetContext((IContext**)&ctx);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(ctx, (ILayoutInflater**)&inflater);

    AutoPtr<IView> checkbox;
    inflater->Inflate(Elastos::Droid::R::layout::always_use_checkbox, NULL, (IView**)&checkbox);
    AutoPtr<IView> vv;
    checkbox->FindViewById(Elastos::Droid::R::id::alwaysUse, (IView**)&vv);
    mAlwaysAllow = ICheckBox::Probe(vv);
    String str5;
    GetString(R::string::usb_debugging_always, &str5);
    AutoPtr<ICharSequence> cs6;
    CString::New(str5, (ICharSequence**)&cs6);
    ITextView::Probe(mAlwaysAllow)->SetText(cs6);
    ap->SetView(checkbox);

    SetupAlert();
    return NOERROR;
}

ECode UsbDebuggingActivity::OnStart()
{
    AlertActivity::OnStart();
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mDisconnectedReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode UsbDebuggingActivity::OnStop()
{
    if (mDisconnectedReceiver != NULL) {
        UnregisterReceiver(mDisconnectedReceiver);
    }
    AlertActivity::OnStop();
    return NOERROR;
}

ECode UsbDebuggingActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Boolean allow = (which == IDialogInterface::BUTTON_POSITIVE);
    Boolean isChecked;
    ICheckable::Probe(mAlwaysAllow)->IsChecked(&isChecked);
    Boolean alwaysAllow = allow && isChecked;
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(USB_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(obj);
    AutoPtr<IIUsbManager> service = IIUsbManager::Probe(b);
    if (allow) {
        service->AllowUsbDebugging(alwaysAllow, mKey);
    }
    else {
        service->DenyUsbDebugging();
    }
    // } catch (Exception e) {
    Logger::E(TAG, "Unable to notify Usb service");
    // }
    Finish();
    return NOERROR;
}

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos