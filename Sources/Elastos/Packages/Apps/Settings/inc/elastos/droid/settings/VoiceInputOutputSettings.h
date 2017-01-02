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

#ifndef __ELASTOS_DROID_SETTINGS_VOICEINPUTOUTPUTSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_VOICEINPUTOUTPUTSETTINGS_H__

#include "Elastos.Droid.Speech.h"
#include <elastos/core/Object.h>
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Speech::Tts::ITtsEngines;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Settings screen for voice input/output.
 */
class VoiceInputOutputSettings
    : public Object
{
public:
    VoiceInputOutputSettings(
        /* [in] */ ISettingsPreferenceFragment* fragment);

    ~VoiceInputOutputSettings();

    CARAPI OnCreate();

private:
    CARAPI_(void) PopulateOrRemovePreferences();

    CARAPI_(Boolean) PopulateOrRemoveVoiceInputPrefs();

    CARAPI_(Boolean) PopulateOrRemoveTtsPrefs();

private:
    static const String TAG;

    static const String KEY_VOICE_CATEGORY;
    static const String KEY_VOICE_INPUT_SETTINGS;
    static const String KEY_TTS_SETTINGS;

    AutoPtr<IPreferenceGroup> mParent;
    AutoPtr<IPreferenceCategory> mVoiceCategory;
    AutoPtr<IPreference> mVoiceInputSettingsPref;
    AutoPtr<IPreference> mTtsSettingsPref;
    AutoPtr<ISettingsPreferenceFragment> mFragment;
    AutoPtr<ITtsEngines> mTtsEngines;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_VOICEINPUTOUTPUTSETTINGS_H__
