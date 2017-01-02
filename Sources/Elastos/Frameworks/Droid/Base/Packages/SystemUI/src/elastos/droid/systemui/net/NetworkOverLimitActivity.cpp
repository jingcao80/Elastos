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

#include "elastos/droid/systemui/net/NetworkOverLimitActivity.h"
#include "elastos/droid/os/ServiceManager.h"
#include "Elastos.Droid.View.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IINetworkPolicyManager;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Net {

//====================================
// NetworkOverLimitActivity::MyDialogInterfaceOnClickListener
//====================================

CAR_INTERFACE_IMPL(NetworkOverLimitActivity::MyDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

NetworkOverLimitActivity::MyDialogInterfaceOnClickListener::MyDialogInterfaceOnClickListener(
    /* [in] */ INetworkTemplate* temp,
    /* [in] */ NetworkOverLimitActivity* host)
    : mHost(host)
    , mTemp(temp)
{}

ECode NetworkOverLimitActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->SnoozePolicy(mTemp);
}

//====================================
// NetworkOverLimitActivity::MyDialogInterfaceOnDismissListener
//====================================

CAR_INTERFACE_IMPL(NetworkOverLimitActivity::MyDialogInterfaceOnDismissListener, Object, IDialogInterfaceOnDismissListener)

NetworkOverLimitActivity::MyDialogInterfaceOnDismissListener::MyDialogInterfaceOnDismissListener(
    /* [in] */ NetworkOverLimitActivity* host)
    : mHost(host)
{}

ECode NetworkOverLimitActivity::MyDialogInterfaceOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->Finish();
}

//====================================
// NetworkOverLimitActivity
//====================================

CAR_INTERFACE_IMPL(NetworkOverLimitActivity, Activity, INetworkOverLimitActivity)

const String NetworkOverLimitActivity::TAG("NetworkOverLimitActivity");

ECode NetworkOverLimitActivity::constructor()
{
    return Activity::constructor();
}

ECode NetworkOverLimitActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(INetworkPolicyManager::EXTRA_NETWORK_TEMPLATE, (IParcelable**)&parcelable);
    AutoPtr<INetworkTemplate> temp = INetworkTemplate::Probe(parcelable);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(GetLimitedDialogTitleForTemplate(temp));
    builder->SetMessage(R::string::data_usage_disabled_dialog);
    builder->SetPositiveButton(R::string::ok, NULL);
    AutoPtr<MyDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener(temp, this);
    builder->SetNegativeButton(R::string::data_usage_disabled_dialog_enable, (IDialogInterfaceOnClickListener*)listener);
    AutoPtr<IAlertDialog> adialog;
    builder->Create((IAlertDialog**)&adialog);
    AutoPtr<IDialog> dialog = IDialog::Probe(adialog);
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    dialog->SetOnDismissListener(new MyDialogInterfaceOnDismissListener(this));
    dialog->Show();
    return NOERROR;
}

ECode NetworkOverLimitActivity::SnoozePolicy(
    /* [in] */ INetworkTemplate* temp)
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NETWORK_POLICY_SERVICE);
    AutoPtr<IINetworkPolicyManager> policyService = IINetworkPolicyManager::Probe(obj);
    ECode ec = policyService->SnoozeLimit(temp);
    if (FAILED(ec)) {
        Logger::W(TAG, "problem snoozing network policy%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

Int32 NetworkOverLimitActivity::GetLimitedDialogTitleForTemplate(
    /* [in] */ INetworkTemplate* temp)
{
    Int32 result;
    temp->GetMatchRule(&result);
    switch (result) {
        case INetworkTemplate::MATCH_MOBILE_3G_LOWER:
            return R::string::data_usage_disabled_dialog_3g_title;
        case INetworkTemplate::MATCH_MOBILE_4G:
            return R::string::data_usage_disabled_dialog_4g_title;
        case INetworkTemplate::MATCH_MOBILE_ALL:
            return R::string::data_usage_disabled_dialog_mobile_title;
        default:
            return R::string::data_usage_disabled_dialog_title;
    }
}

} // namespace Net
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos