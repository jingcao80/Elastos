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

#include "elastos/droid/systemui/usb/UsbResolverActivity.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

//-------------------------------------------------------------------------
// UsbResolverActivity
//-------------------------------------------------------------------------

const String UsbResolverActivity::TAG("UsbResolverActivity");
const String UsbResolverActivity::EXTRA_RESOLVE_INFOS("rlist");

ECode UsbResolverActivity::constructor()
{
    return NOERROR;
}

ECode UsbResolverActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> targetParcelable;
    intent->GetParcelableExtra(IIntent::EXTRA_INTENT, (IParcelable**)&targetParcelable);
    if (IIntent::Probe(targetParcelable) == NULL) {
        Logger::W("UsbResolverActivity", "Target is not an intent: %s", TO_CSTR(targetParcelable));
        Finish();
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IIntent> target = IIntent::Probe(targetParcelable);
    AutoPtr<IArrayList> rList;
    intent->GetParcelableArrayListExtra(EXTRA_RESOLVE_INFOS, (IArrayList**)&rList);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ICharSequence> title;
    res->GetText(Elastos::Droid::R::string::chooseUsbActivity, (ICharSequence**)&title);
    ResolverActivity::OnCreate(savedInstanceState, target, title, NULL, IList::Probe(rList),
        TRUE /* Set alwaysUseOption to true to enable "always use this app" checkbox. */ );

    AutoPtr<IView> v;
    FindViewById(Elastos::Droid::R::id::alwaysUse, (IView**)&v);
    AutoPtr<ICheckBox> alwaysUse = ICheckBox::Probe(v);

    if (alwaysUse != NULL) {
        if (mDevice == NULL) {
            ITextView::Probe(alwaysUse)->SetText(R::string::always_use_accessory);
        }
        else {
            ITextView::Probe(alwaysUse)->SetText(R::string::always_use_device);
        }
    }

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IUsbManager::EXTRA_DEVICE, (IParcelable**)&parcelable);
    mDevice = IUsbDevice::Probe(parcelable);
    if (mDevice != NULL) {
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mDevice);
    }
    else {
        AutoPtr<IParcelable> parcelable2;
        intent->GetParcelableExtra(IUsbManager::EXTRA_ACCESSORY, (IParcelable**)&parcelable2);
        mAccessory = IUsbAccessory::Probe(parcelable2);
        if (mAccessory == NULL) {
            Logger::E(TAG, "no device or accessory");
            Finish();
            return E_NULL_POINTER_EXCEPTION;
        }
        mDisconnectedReceiver = new UsbDisconnectedReceiver(this, mAccessory);
    }
    return NOERROR;
}

ECode UsbResolverActivity::OnDestroy()
{
    if (mDisconnectedReceiver != NULL) {
        UnregisterReceiver(mDisconnectedReceiver);
    }
    ResolverActivity::OnDestroy();
    return NOERROR;
}

ECode UsbResolverActivity::OnIntentSelected(
    /* [in] */ IResolveInfo* ri,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean alwaysCheck)
{
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(USB_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(obj);
    AutoPtr<IIUsbManager> service = IIUsbManager::Probe(b);

    AutoPtr<IActivityInfo> ai;
    ri->GetActivityInfo((IActivityInfo**)&ai);
    AutoPtr<IComponentInfo> ci = IComponentInfo::Probe(ai);
    AutoPtr<IApplicationInfo> appi;
    ci->GetApplicationInfo((IApplicationInfo**)&appi);
    Int32 id;
    appi->GetUid(&id);
    const Int32 uid = id;

    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    Int32 id2;
    uhh->GetMyUserId(&id2);
    const Int32 userId = id2;

    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(ai);
    String packageName;
    pii->GetPackageName(&packageName);
    if (mDevice != NULL) {
        // grant permission for the device
        service->GrantDevicePermission(mDevice, uid);
        // set or clear default setting
        if (alwaysCheck) {
            service->SetDevicePackage(mDevice, packageName, userId);
        }
        else {
            service->SetDevicePackage(mDevice, String(NULL), userId);
        }
    }
    else if (mAccessory != NULL) {
        // grant permission for the accessory
        service->GrantAccessoryPermission(mAccessory, uid);
        // set or clear default setting
        if (alwaysCheck) {
            service->SetAccessoryPackage(mAccessory, packageName, userId);
        }
        else {
            service->SetAccessoryPackage(mAccessory, String(NULL), userId);
        }
    }

    AutoPtr<IUserHandle> uh;
    CUserHandle::New(userId, (IUserHandle**)&uh);
    ECode ec = StartActivityAsUser(intent, uh);
    if (FAILED(ec)) {
        Logger::E(TAG, "startActivity failed%08x", ec);
        return E_ACTIVITY_NOT_FOUND_EXCEPTION;
    }

    // } catch (RemoteException e) {
        // Log.e(TAG, "onIntentSelected failed", e);
    // }
    return NOERROR;
}

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos