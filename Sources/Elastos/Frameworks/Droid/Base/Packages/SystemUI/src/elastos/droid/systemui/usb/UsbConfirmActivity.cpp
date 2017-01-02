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

#include "elastos/droid/systemui/usb/UsbConfirmActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include "R.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {


CAR_INTERFACE_IMPL_2(UsbConfirmActivity::InnerListener, Object,
    IDialogInterfaceOnClickListener, ICompoundButtonOnCheckedChangeListener)

UsbConfirmActivity::InnerListener::InnerListener(
    /* [in] */ UsbConfirmActivity* host)
    : mHost(host)
{}

ECode UsbConfirmActivity::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

ECode UsbConfirmActivity::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

const String UsbConfirmActivity::TAG("UsbConfirmActivity");

UsbConfirmActivity::UsbConfirmActivity()
    : mPermissionGranted(FALSE)
{}

ECode UsbConfirmActivity::constructor()
{
    return NOERROR;
}

ECode UsbConfirmActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AlertActivity::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IUsbManager::EXTRA_DEVICE, (IParcelable**)&parcelable);
    mDevice = IUsbDevice::Probe(parcelable);
    parcelable = NULL;
    intent->GetParcelableExtra(IUsbManager::EXTRA_ACCESSORY, (IParcelable**)&parcelable);
    mAccessory = IUsbAccessory::Probe(parcelable);
    parcelable = NULL;
    intent->GetParcelableExtra(String("rinfo"), (IParcelable**)&parcelable);
    mResolveInfo = IResolveInfo::Probe(parcelable);
    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<ICharSequence> label;
    mResolveInfo->LoadLabel(packageManager, (ICharSequence**)&label);
    String appName;
    label->ToString(&appName);

    const AutoPtr<IAlertControllerAlertParams> ap = mAlertParams;
    AutoPtr<IDrawable> icon;
    mResolveInfo->LoadIcon(packageManager, (IDrawable**)&icon);
    ap->SetIcon(icon);
    ap->SetTitle(label);

    AutoPtr<ArrayOf<IInterface*> > aa = ArrayOf<IInterface*>::Alloc(1);
    aa->Set(0, label.Get());
    if (mDevice == NULL) {
        String str1;
        GetString(R::string::usb_accessory_confirm_prompt, aa, &str1);
        AutoPtr<ICharSequence> message;
        CString::New(str1, (ICharSequence**)&message);
        ap->SetMessage(message);
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mAccessory);
    }
    else {
        String str2;
        GetString(R::string::usb_device_confirm_prompt, aa, &str2);
        AutoPtr<ICharSequence> message;
        CString::New(str2, (ICharSequence**)&message);
        ap->SetMessage(message);
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mDevice);
    }
    String str3;
    GetString(R::string::ok, &str3);
    AutoPtr<ICharSequence> text;
    CString::New(str3, (ICharSequence**)&text);
    ap->SetPositiveButtonText(text);

    AutoPtr<InnerListener> listener = new InnerListener(this);

    String str4;
    GetString(Elastos::Droid::R::string::cancel, &str4);
    AutoPtr<ICharSequence> text2;
    CString::New(str4, (ICharSequence**)&text2);
    ap->SetNegativeButtonText(text2);
    ap->SetPositiveButtonListener(listener);
    ap->SetNegativeButtonListener(listener);

    // add "always use" checkbox
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::R::layout::always_use_checkbox, NULL, (IView**)&view);
    ap->SetView(view);
    AutoPtr<IView> view2;
    view->FindViewById(Elastos::Droid::R::id::alwaysUse, (IView**)&view2);
    mAlwaysUse = ICheckBox::Probe(view2);
    if (mDevice == NULL) {
        AutoPtr<ICharSequence> cs1;
        CString::New(StringUtils::ToString(R::string::always_use_accessory), (ICharSequence**)&cs1);
        ITextView::Probe(mAlwaysUse)->SetText(cs1);
    }
    else {
        AutoPtr<ICharSequence> cs2;
        CString::New(StringUtils::ToString(R::string::always_use_device), (ICharSequence**)&cs2);
        ITextView::Probe(mAlwaysUse)->SetText(cs2);
    }
    ICompoundButton::Probe(mAlwaysUse)->SetOnCheckedChangeListener(listener);
    AutoPtr<IView> view3;
    view->FindViewById(Elastos::Droid::R::id::clearDefaultHint, (IView**)&view3);
    mClearDefaultHint = ITextView::Probe(view3);
    IView::Probe(mClearDefaultHint)->SetVisibility(IView::GONE);

    SetupAlert();
    return NOERROR;
}

ECode UsbConfirmActivity::OnDestroy()
{
    if (mDisconnectedReceiver != NULL) {
        UnregisterReceiver(mDisconnectedReceiver);
    }
    AlertActivity::OnDestroy();
    return NOERROR;
}

ECode UsbConfirmActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        // try {
        AutoPtr<IInterface> obj = ServiceManager::GetService(USB_SERVICE);
        AutoPtr<IBinder> b = IBinder::Probe(obj);
        AutoPtr<IIUsbManager> service = IIUsbManager::Probe(b);
        AutoPtr<IActivityInfo> ai;
        mResolveInfo->GetActivityInfo((IActivityInfo**)&ai);
        AutoPtr<IComponentInfo> ci = IComponentInfo::Probe(ai);
        AutoPtr<IApplicationInfo> appi;
        ci->GetApplicationInfo((IApplicationInfo**)&appi);
        Int32 id;
        appi->GetUid(&id);
        const Int32 uid = id;

        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 uuid;
        uhh->GetMyUserId(&uuid);
        const Int32 userId = uuid;
        Boolean alwaysUse;
        ICheckable::Probe(mAlwaysUse)->IsChecked(&alwaysUse);

        AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(ci);
        String name, pkgName;
        pii->GetPackageName(&pkgName);
        pii->GetName(&name);
        AutoPtr<IIntent> intent;
        if (mDevice != NULL) {
            CIntent::New(IUsbManager::ACTION_USB_DEVICE_ATTACHED, (IIntent**)&intent);
            intent->PutExtra(IUsbManager::EXTRA_DEVICE, IParcelable::Probe(mDevice));

            // grant permission for the device
            service->GrantDevicePermission(mDevice, uid);
            // set or clear default setting
            if (alwaysUse) {
                service->SetDevicePackage(mDevice, pkgName, userId);
            }
            else {
                service->SetDevicePackage(mDevice, String(NULL), userId);
            }
        }
        else if (mAccessory != NULL) {
            CIntent::New(IUsbManager::ACTION_USB_ACCESSORY_ATTACHED, (IIntent**)&intent);
            intent->PutExtra(IUsbManager::EXTRA_ACCESSORY, IParcelable::Probe(mAccessory));

            // grant permission for the accessory
            service->GrantAccessoryPermission(mAccessory, uid);
            // set or clear default setting
            if (alwaysUse) {
                service->SetAccessoryPackage(mAccessory, pkgName, userId);
            }
            else {
                service->SetAccessoryPackage(mAccessory, String(NULL), userId);
            }
        }

        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        AutoPtr<IComponentName> cn;
        CComponentName::New(pkgName, name, (IComponentName**)&cn);

        intent->SetComponent(cn);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        StartActivityAsUser(intent, uh);
        // } catch (Exception e) {
        //     Log.e(TAG, "Unable to start activity", e);
        // }
    }
    Finish();
    return NOERROR;
}

ECode UsbConfirmActivity::OnCheckedChanged(
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
