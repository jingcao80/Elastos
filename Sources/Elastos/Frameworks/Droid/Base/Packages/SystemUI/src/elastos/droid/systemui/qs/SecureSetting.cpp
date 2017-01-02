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

#include "elastos/droid/systemui/qs/SecureSetting.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IListenable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_INTERFACE_IMPL(SecureSetting, ContentObserver, IListenable)

SecureSetting::SecureSetting()
    : mListening(FALSE)
    , mUserId(0)
{}

ECode SecureSetting::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& settingName)
{
    ContentObserver::constructor(handler);

    mContext = context;
    mSettingName = settingName;
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    helper->GetCurrentUser(&mUserId);
    return SetListening(TRUE);
}

Int32 SecureSetting::GetValue()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 v = 0;
    Elastos::Droid::Provider::Settings::Secure::GetInt32ForUser(cr, mSettingName, 0, mUserId, &v);
    return v;
}

void SecureSetting::SetValue(
    /* [in] */ Int32 value)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean tmp = FALSE;
    Elastos::Droid::Provider::Settings::Secure::PutInt32ForUser(cr, mSettingName, value, mUserId, &tmp);
}

ECode SecureSetting::SetListening(
    /* [in] */ Boolean listening)
{
    mListening = listening;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (listening) {
        AutoPtr<IUri> uri;
        Elastos::Droid::Provider::Settings::Secure::GetUriFor(mSettingName, (IUri**)&uri);
        cr->RegisterContentObserver(uri, FALSE, this, mUserId);
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

ECode SecureSetting::OnChange(
    /* [in] */ Boolean selfChange)
{
    HandleValueChanged(GetValue());
    return NOERROR;
}

void SecureSetting::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    if (mListening) {
        SetListening(FALSE);
        SetListening(TRUE);
    }
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
