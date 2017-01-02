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

#include "elastos/droid/preference/CPreference.h"
#include "elastos/droid/preference/CPreferenceBaseSavedState.h"
#include "elastos/droid/preference/CTwoStatePreferenceSavedState.h"
#include "elastos/droid/preference/CCheckBoxPreference.h"
#include "elastos/droid/preference/CDialogPreferenceSavedState.h"
#include "elastos/droid/preference/CEditTextPreference.h"
#include "elastos/droid/preference/CEditTextPreferenceSavedState.h"
#include "elastos/droid/preference/CListPreference.h"
#include "elastos/droid/preference/CListPreferenceSavedState.h"
#include "elastos/droid/preference/CMultiCheckPreference.h"
#include "elastos/droid/preference/CMultiCheckPreferenceSavedState.h"
#include "elastos/droid/preference/CMultiSelectListPreference.h"
#include "elastos/droid/preference/CMultiSelectListPreferenceSavedState.h"
#include "elastos/droid/preference/CSeekBarDialogPreference.h"
#include "elastos/droid/preference/CSeekBarVolumizer.h"
#include "elastos/droid/preference/CSwitchPreference.h"
#include "elastos/droid/preference/CVolumePreference.h"
#include "elastos/droid/preference/CVolumePreferenceVolumnStore.h"
#include "elastos/droid/preference/CVolumePreferenceSavedState.h"
#include "elastos/droid/preference/CPreferenceInflater.h"
#include "elastos/droid/preference/CPreferenceCategory.h"
#include "elastos/droid/preference/CPreferenceManager.h"
#include "elastos/droid/preference/CRingtonePreference.h"
#include "elastos/droid/preference/CPreferenceFrameLayout.h"
#include "elastos/droid/preference/CPreferenceFrameLayoutLayoutParams.h"
#include "elastos/droid/preference/CPreferenceScreen.h"
#include "elastos/droid/preference/CPreferenceScreenSavedState.h"
#include "elastos/droid/preference/CSeekBarPreference.h"
#include "elastos/droid/preference/CSeekBarPreferenceSavedState.h"
#include "elastos/droid/preference/CPreferenceActivityHeader.h"
#include "elastos/droid/preference/CPreferenceGroupAdapter.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_OBJECT_IMPL(CPreference)

CAR_OBJECT_IMPL(CPreferenceBaseSavedState)

CAR_OBJECT_IMPL(CTwoStatePreferenceSavedState)

CAR_OBJECT_IMPL(CCheckBoxPreference)

CAR_OBJECT_IMPL(CDialogPreferenceSavedState)

CAR_OBJECT_IMPL(CEditTextPreference)

CAR_OBJECT_IMPL(CEditTextPreferenceSavedState)

CAR_OBJECT_IMPL(CListPreference)

CAR_OBJECT_IMPL(CListPreferenceSavedState)

CAR_OBJECT_IMPL(CMultiCheckPreference)

CAR_OBJECT_IMPL(CMultiCheckPreferenceSavedState)

CAR_OBJECT_IMPL(CMultiSelectListPreference)

CAR_OBJECT_IMPL(CMultiSelectListPreferenceSavedState)

CAR_OBJECT_IMPL(CSeekBarDialogPreference)

CAR_OBJECT_IMPL(CSeekBarVolumizer)

CAR_OBJECT_IMPL(CSwitchPreference)

CAR_OBJECT_IMPL(CVolumePreference)

CAR_OBJECT_IMPL(CVolumePreferenceVolumnStore)

CAR_OBJECT_IMPL(CVolumePreferenceSavedState)

CAR_OBJECT_IMPL(CPreferenceInflater)

CAR_OBJECT_IMPL(CPreferenceCategory)

CAR_OBJECT_IMPL(CPreferenceManager)

CAR_OBJECT_IMPL(CRingtonePreference)

CAR_OBJECT_IMPL(CPreferenceFrameLayout)

CAR_OBJECT_IMPL(CPreferenceFrameLayoutLayoutParams)

CAR_OBJECT_IMPL(CPreferenceScreen)

CAR_OBJECT_IMPL(CPreferenceScreenSavedState)

CAR_OBJECT_IMPL(CSeekBarPreference)

CAR_OBJECT_IMPL(CSeekBarPreferenceSavedState)

CAR_OBJECT_IMPL(CPreferenceActivityHeader)

CAR_OBJECT_IMPL(CPreferenceGroupAdapter)

} // namespace Preference
} // namespace Droid
} // namespace Elastos