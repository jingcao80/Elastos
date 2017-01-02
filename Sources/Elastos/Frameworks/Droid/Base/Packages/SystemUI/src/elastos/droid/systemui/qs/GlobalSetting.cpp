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

#include "elastos/droid/systemui/qs/GlobalSetting.h"
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IListenable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_INTERFACE_IMPL_2(GlobalSetting, ContentObserver, IGlobalSetting, IListenable);

GlobalSetting::GlobalSetting()
{
}

ECode GlobalSetting::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& settingName)
{
    mContext = context;
    mSettingName = settingName;
    return ContentObserver::constructor(handler);
}

ECode GlobalSetting::GetValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    return Elastos::Droid::Provider::Settings::Global::GetInt32(cr, mSettingName, 0, result);
}

ECode GlobalSetting::SetValue(
    /* [in] */ Int32 value)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean tmp = FALSE;
    Elastos::Droid::Provider::Settings::Global::PutInt32(cr, mSettingName, value, &tmp);
    return NOERROR;
}

ECode GlobalSetting::SetListening(
    /* [in] */ Boolean listening)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (listening) {
        AutoPtr<IUri> uri;
        Elastos::Droid::Provider::Settings::Global::GetUriFor(mSettingName, (IUri**)&uri);
        cr->RegisterContentObserver(uri, FALSE, this);
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

ECode GlobalSetting::OnChange(
    /* [in] */ Boolean selfChange)
{
    Int32 v = 0;
    GetValue(&v);
    HandleValueChanged(v);
    return NOERROR;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
