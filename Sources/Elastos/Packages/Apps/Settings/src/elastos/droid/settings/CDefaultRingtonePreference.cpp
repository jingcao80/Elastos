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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/settings/CDefaultRingtonePreference.h"

using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CDefaultRingtonePreference::TAG("CDefaultRingtonePreference");

CAR_OBJECT_IMPL(CDefaultRingtonePreference)

CDefaultRingtonePreference::CDefaultRingtonePreference()
{}

ECode CDefaultRingtonePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RingtonePreference::constructor(context, attrs);
}

ECode CDefaultRingtonePreference::OnPrepareRingtonePickerIntent(
    /* [in] */ IIntent* ringtonePickerIntent)
{
    RingtonePreference::OnPrepareRingtonePickerIntent(ringtonePickerIntent);

    /*
     * Since this preference is for choosing the default ringtone, it
     * doesn't make sense to show a 'Default' item.
     */
    return ringtonePickerIntent->PutBooleanExtra(IRingtoneManager::EXTRA_RINGTONE_SHOW_DEFAULT, FALSE);
}

ECode CDefaultRingtonePreference::OnSaveRingtone(
    /* [in] */ IUri* ringtoneUri)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    return helper->SetActualDefaultRingtoneUri(context, type, ringtoneUri);
}

ECode CDefaultRingtonePreference::OnRestoreRingtone(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    return helper->GetActualDefaultRingtoneUri(context, type, uri);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
