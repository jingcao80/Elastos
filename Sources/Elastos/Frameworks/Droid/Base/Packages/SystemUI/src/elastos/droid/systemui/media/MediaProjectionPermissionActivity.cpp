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


#include "elastos/droid/systemui/media/MediaProjectionPermissionActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Droid::Media::Projection::IMediaProjectionManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Media {

CAR_INTERFACE_IMPL_2(MediaProjectionPermissionActivity::InnerListener, Object,
    IDialogInterfaceOnClickListener, ICompoundButtonOnCheckedChangeListener)


MediaProjectionPermissionActivity::InnerListener::InnerListener(
    /* [in] */ MediaProjectionPermissionActivity* host)
    : mHost(host)
{
}

// @Override
ECode MediaProjectionPermissionActivity::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

// @Override
ECode MediaProjectionPermissionActivity::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}


const String MediaProjectionPermissionActivity::TAG("MediaProjectionPermissionActivity");


MediaProjectionPermissionActivity::MediaProjectionPermissionActivity()
    : mUid(0)
{}

ECode MediaProjectionPermissionActivity::constructor()
{
    return NOERROR;
}

ECode MediaProjectionPermissionActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AlertActivity::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    GetCallingPackage(&mPackageName);
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::MEDIA_PROJECTION_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(service);
    mService = IIMediaProjectionManager::Probe(b);

    if (mPackageName.IsNull()) {
        Finish();
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IApplicationInfo> aInfo;

    ECode ec = packageManager->GetApplicationInfo(mPackageName, 0, (IApplicationInfo**)&aInfo);
    aInfo->GetUid(&mUid);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to look up package name%08x", ec);
        Finish();
        return E_NAME_NOT_FOUND_EXCEPTION;
    }

    // try {
    Boolean result;
    ec = mService->HasProjectionPermission(mUid, mPackageName, &result);
    if (result) {
        SetResult(IActivity::RESULT_OK,
            GetMediaProjectionIntent(mUid, mPackageName, FALSE /*permanentGrant*/));
        Finish();
        return E_NULL_POINTER_EXCEPTION;
    }
    if (FAILED(ec)) {
        Logger::E(TAG, "Error checking projection permissions%08x", ec);
        Finish();
        return E_REMOTE_EXCEPTION;
    }

    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(aInfo)->LoadLabel(packageManager, (ICharSequence**)&label);
    String appName;
    label->ToString(&appName);

    const AutoPtr<IAlertControllerAlertParams> ap = mAlertParams;
    AutoPtr<IDrawable> icon;
    IPackageItemInfo::Probe(aInfo)->LoadIcon(packageManager, (IDrawable**)&icon);
    ap->SetIcon(icon);

    AutoPtr<InnerListener> listener = new InnerListener(this);

    AutoPtr<ICharSequence> csappname;
    CString::New(appName, (ICharSequence**)&csappname);
    AutoPtr<ArrayOf<IInterface*> > arrappname = ArrayOf<IInterface*>::Alloc(1);
    arrappname->Set(0, csappname);
    String str;
    GetString(R::string::media_projection_dialog_text, arrappname, &str);
    AutoPtr<ICharSequence> message;
    CString::New(str, (ICharSequence**)&message);
    ap->SetMessage(message);
    String str2;
    GetString(R::string::media_projection_action_text, &str2);
    AutoPtr<ICharSequence> ptext;
    CString::New(str2, (ICharSequence**)&ptext);
    ap->SetPositiveButtonText(ptext);
    String str3;
    GetString(Elastos::Droid::R::string::cancel, &str3);
    AutoPtr<ICharSequence> ntext;
    CString::New(str3, (ICharSequence**)&ntext);
    ap->SetNegativeButtonText(ntext);
    ap->SetPositiveButtonListener(listener);
    ap->SetNegativeButtonListener(listener);

    // add "always use" checkbox
    AutoPtr<IInterface> inflaterObj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflaterObj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(inflaterObj);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::remember_permission_checkbox, NULL, (IView**)&view);
    ap->SetView(view);
    AutoPtr<IView> v;
    view->FindViewById(R::id::remember, (IView**)&v);

    AutoPtr<ICheckBox> rememberPermissionCheckbox = ICheckBox::Probe(v);
    ICompoundButton::Probe(rememberPermissionCheckbox)->SetOnCheckedChangeListener(listener);
    SetupAlert();
    return NOERROR;
}

ECode MediaProjectionPermissionActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        ECode ec = SetResult(IActivity::RESULT_OK,
            GetMediaProjectionIntent(
                mUid, mPackageName, mPermanentGrant));
        if (FAILED(ec)) {
            Logger::E(TAG, "Error granting projection permission%08x", ec);
            SetResult(RESULT_CANCELED);
            return E_REMOTE_EXCEPTION;
        }
        Finish();
    }
    return NOERROR;
}

ECode MediaProjectionPermissionActivity::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    mPermanentGrant = isChecked;
    return NOERROR;
}


AutoPtr<IIntent> MediaProjectionPermissionActivity::GetMediaProjectionIntent(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean permanentGrant)
{
    AutoPtr<IIMediaProjection> projection;
    mService->CreateProjection(uid, packageName,
        IMediaProjectionManager::TYPE_SCREEN_CAPTURE, permanentGrant,
        (IIMediaProjection**)&projection);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(IMediaProjectionManager::EXTRA_MEDIA_PROJECTION, IBinder::Probe(projection));
    return intent;
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

