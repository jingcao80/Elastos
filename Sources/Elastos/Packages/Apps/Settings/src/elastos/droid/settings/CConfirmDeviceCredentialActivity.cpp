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

#include "elastos/droid/settings/CConfirmDeviceCredentialActivity.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CConfirmDeviceCredentialActivity
//===============================================================================

const String CConfirmDeviceCredentialActivity::TAG("CConfirmDeviceCredentialActivity");

CAR_OBJECT_IMPL(CConfirmDeviceCredentialActivity)

CConfirmDeviceCredentialActivity::CConfirmDeviceCredentialActivity()
{}

CConfirmDeviceCredentialActivity::~CConfirmDeviceCredentialActivity()
{}

ECode CConfirmDeviceCredentialActivity::constructor()
{
    return Activity::constructor();
}

AutoPtr<IIntent> CConfirmDeviceCredentialActivity::CreateIntent(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* details)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("Elastos.Droid.Settings"),
            String("Elastos.Droid.Settings.CConfirmDeviceCredentialActivity"));
    intent->PutExtra(IKeyguardManager::EXTRA_TITLE, title);
    intent->PutExtra(IKeyguardManager::EXTRA_DESCRIPTION, details);
    return intent;
}

ECode CConfirmDeviceCredentialActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    String title;
    intent->GetStringExtra(IKeyguardManager::EXTRA_TITLE, &title);
    String details;
    intent->GetStringExtra(IKeyguardManager::EXTRA_DESCRIPTION, &details);

    AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
    helper->constructor(this);
    if (!helper->LaunchConfirmationActivity(0 /* request code */, CoreUtils::Convert(title), CoreUtils::Convert(details))) {
        Logger::D(TAG, "No pattern, password or PIN set.");
        SetResult(IActivity::RESULT_OK);
        Finish();
    }
    return NOERROR;
}

ECode CConfirmDeviceCredentialActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Boolean credentialsConfirmed = (resultCode == IActivity::RESULT_OK);
    Logger::D(TAG, "Device credentials confirmed: %d", credentialsConfirmed);
    SetResult(credentialsConfirmed ? IActivity::RESULT_OK : IActivity::RESULT_CANCELED);
    Finish();
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos