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

#include "elastos/droid/systemui/usb/UsbPermissionActivity.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::R;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

CAR_INTERFACE_IMPL_2(UsbPermissionActivity::InnerListener, Object,
    IDialogInterfaceOnClickListener, ICompoundButtonOnCheckedChangeListener)

UsbPermissionActivity::InnerListener::InnerListener(
    /* [in] */ UsbPermissionActivity* host)
    : mHost(host)
{}

ECode UsbPermissionActivity::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

ECode UsbPermissionActivity::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

//-------------------------------------------------------------------------
// UsbPermissionActivity
//-------------------------------------------------------------------------

const String UsbPermissionActivity::TAG("UsbPermissionActivity");

UsbPermissionActivity::UsbPermissionActivity()
    : mUid(0)
    , mPermissionGranted(FALSE)
{}

ECode UsbPermissionActivity::constructor()
{
    return NOERROR;
}

ECode UsbPermissionActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AlertActivity::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IUsbManager::EXTRA_DEVICE, (IParcelable**)&parcelable);
    mDevice = IUsbDevice::Probe(parcelable);

    AutoPtr<IParcelable> parcelable2;
    intent->GetParcelableExtra(IUsbManager::EXTRA_ACCESSORY, (IParcelable**)&parcelable2);
    mAccessory = IUsbAccessory::Probe(parcelable2);

    AutoPtr<IParcelable> parcelable3;
    intent->GetParcelableExtra(IIntent::EXTRA_INTENT, (IParcelable**)&parcelable3);
    mPendingIntent = IPendingIntent::Probe(parcelable3);

    intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &mUid);
    intent->GetStringExtra(String("package"), &mPackageName);

    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IApplicationInfo> aInfo;
    ECode ec = packageManager->GetApplicationInfo(mPackageName, 0, (IApplicationInfo**)&aInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to look up package name%08x", ec);
        Finish();
        return E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    IPackageItemInfo::Probe(aInfo)->LoadLabel(packageManager, (ICharSequence**)&cs);
    String appName;
    cs->ToString(&appName);

    const AutoPtr<IAlertControllerAlertParams> ap = mAlertParams;

    AutoPtr<IDrawable> icon;
    IPackageItemInfo::Probe(aInfo)->LoadIcon(packageManager, (IDrawable**)&icon);

    ap->SetIcon(icon);
    ap->SetTitle(cs);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, cs);
    if (mDevice == NULL) {
        String s;
        GetString(R::string::usb_accessory_permission_prompt, array, &s);
        AutoPtr<ICharSequence> cs2;
        CString::New(s, (ICharSequence**)&cs2);
        ap->SetMessage(cs2);
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mAccessory);
    }
    else {
        String s;
        GetString(R::string::usb_device_permission_prompt, array, &s);
        AutoPtr<ICharSequence> cs3;
        CString::New(s, (ICharSequence**)&cs3);
        ap->SetMessage(cs3);
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mDevice);
    }

    String s2;
    GetString(R::string::ok, &s2);
    AutoPtr<ICharSequence> cs4;
    CString::New(s2, (ICharSequence**)&cs4);
    ap->SetPositiveButtonText(cs4);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    String s3;
    GetString(Elastos::Droid::R::string::cancel, &s3);
    AutoPtr<ICharSequence> cs5;
    CString::New(s3, (ICharSequence**)&cs5);
    ap->SetNegativeButtonText(cs5);
    ap->SetPositiveButtonListener(listener);
    ap->SetNegativeButtonListener(listener);

    // add "always use" checkbox
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> v;
    inflater->Inflate(Elastos::Droid::R::layout::always_use_checkbox, NULL, (IView**)&v);
    ap->SetView(v);
    AutoPtr<IView> v2;
    ap->GetView((IView**)&v2);
    AutoPtr<IView> v3;
    v2->FindViewById(Elastos::Droid::R::id::alwaysUse, (IView**)&v3);
    mAlwaysUse = ICheckBox::Probe(v3);
    if (mDevice == NULL) {
        ITextView::Probe(mAlwaysUse)->SetText(R::string::always_use_accessory);
    }
    else {
        ITextView::Probe(mAlwaysUse)->SetText(R::string::always_use_device);
    }
    ICompoundButton::Probe(mAlwaysUse)->SetOnCheckedChangeListener(listener);
    AutoPtr<IView> v4;
    v2->FindViewById(Elastos::Droid::R::id::clearDefaultHint, (IView**)&v4);
    mClearDefaultHint = ITextView::Probe(v4);
    IView::Probe(mClearDefaultHint)->SetVisibility(IView::GONE);

    SetupAlert();
    return NOERROR;
}

ECode UsbPermissionActivity::OnDestroy()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(USB_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(obj);
    AutoPtr<IIUsbManager> service = IIUsbManager::Probe(b);

    // send response via pending intent
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // try {
    Boolean isChecked;
    ICheckable::Probe(mAlwaysUse)->IsChecked(&isChecked);
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    Int32 uid;
    uhh->GetUserId(mUid, &uid);
    const Int32 userId = uid;
    if (mDevice != NULL) {
        intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(mDevice));
        if (mPermissionGranted) {
            service->GrantDevicePermission(mDevice, mUid);
            if (isChecked) {
                service->SetDevicePackage(mDevice, mPackageName, userId);
            }
        }
    }
    if (mAccessory != NULL) {
        intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(mAccessory));
        if (mPermissionGranted) {
            service->GrantAccessoryPermission(mAccessory, mUid);
            if (isChecked) {
                service->SetAccessoryPackage(mAccessory, mPackageName, userId);
            }
        }
    }
    intent->PutExtra(IUsbManager::EXTRA_PERMISSION_GRANTED, mPermissionGranted);
    mPendingIntent->Send(this, 0, intent);
    // } catch (PendingIntent.CanceledException e) {
        // Log.w(TAG, "PendingIntent was cancelled");
    // } catch (RemoteException e) {
        // Log.e(TAG, "IUsbService connection failed", e);
    // }

    if (mDisconnectedReceiver != NULL) {
        UnregisterReceiver(mDisconnectedReceiver);
    }
    AlertActivity::OnDestroy();
    return NOERROR;
}

ECode UsbPermissionActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        mPermissionGranted = TRUE;
    }
    Finish();
    return NOERROR;
}

ECode UsbPermissionActivity::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    if (mClearDefaultHint == NULL) return E_NULL_POINTER_EXCEPTION;

    if(isChecked) {
        IView::Probe(mClearDefaultHint)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mClearDefaultHint)->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos