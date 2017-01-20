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

#include "elastos/droid/dialer/settings/DefaultRingtonePreference.h"
#include "Elastos.Droid.Media.h"

using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

CAR_INTERFACE_IMPL(DefaultRingtonePreference, RingtonePreference, IDefaultRingtonePreference);

ECode DefaultRingtonePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RingtonePreference::constructor(context, attrs);
}

ECode DefaultRingtonePreference::OnPrepareRingtonePickerIntent(
    /* [in] */ IIntent* ringtonePickerIntent)
{
    RingtonePreference::OnPrepareRingtonePickerIntent(ringtonePickerIntent);

    /*
     * Since this preference is for choosing the default ringtone, it
     * doesn't make sense to show a 'Default' item.
     */
    ringtonePickerIntent->PutBooleanExtra(IRingtoneManager::EXTRA_RINGTONE_SHOW_DEFAULT, FALSE);
    return NOERROR;
}

ECode DefaultRingtonePreference::OnSaveRingtone(
    /* [in] */ IUri* ringtoneUri)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    helper->SetActualDefaultRingtoneUri(context, type, ringtoneUri);
    return NOERROR;
}

ECode DefaultRingtonePreference::OnRestoreRingtone(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    return helper->GetActualDefaultRingtoneUri(context, type, uri);
}

} // Settings
} // Dialer
} // Droid
} // Elastos
