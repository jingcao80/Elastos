#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/settings/VoiceInputOutputSettings.h"
#include "elastos/droid/settings/voice/VoiceInputHelper.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Settings::Voice::VoiceInputHelper;
using Elastos::Droid::Speech::Tts::CTtsEngines;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {

const String VoiceInputOutputSettings::TAG("VoiceInputOutputSettings");

const String VoiceInputOutputSettings::KEY_VOICE_CATEGORY("voice_category");
const String VoiceInputOutputSettings::KEY_VOICE_INPUT_SETTINGS("voice_input_settings");
const String VoiceInputOutputSettings::KEY_TTS_SETTINGS("tts_settings");

VoiceInputOutputSettings::VoiceInputOutputSettings(
    /* [in] */ ISettingsPreferenceFragment* fragment)
    : mFragment(fragment)
{
    AutoPtr<IPreferenceScreen> screen;
    IPreferenceFragment::Probe(fragment)->GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IContext> context;
    IPreference::Probe(screen)->GetContext((IContext**)&context);
    CTtsEngines::New(context, (ITtsEngines**)&mTtsEngines);
}

VoiceInputOutputSettings::~VoiceInputOutputSettings()
{}

ECode VoiceInputOutputSettings::OnCreate()
{
    AutoPtr<IPreferenceScreen> screen;
    IPreferenceFragment::Probe(mFragment)->GetPreferenceScreen((IPreferenceScreen**)&screen);
    mParent = IPreferenceGroup::Probe(screen);
    AutoPtr<IPreference> pref;
    mParent->FindPreference(CoreUtils::Convert(KEY_VOICE_CATEGORY), (IPreference**)&pref);
    mVoiceCategory = IPreferenceCategory::Probe(pref);
    AutoPtr<IPreferenceGroup> prefGroup = IPreferenceGroup::Probe(mVoiceCategory);
    prefGroup->FindPreference(CoreUtils::Convert(KEY_VOICE_INPUT_SETTINGS),
            (IPreference**)&mVoiceInputSettingsPref);
    mTtsSettingsPref = NULL;
    prefGroup->FindPreference(CoreUtils::Convert(KEY_TTS_SETTINGS), (IPreference**)&mTtsSettingsPref);

    PopulateOrRemovePreferences();
    return NOERROR;
}

void VoiceInputOutputSettings::PopulateOrRemovePreferences()
{
    Boolean hasVoiceInputPrefs = PopulateOrRemoveVoiceInputPrefs();
    Boolean hasTtsPrefs = PopulateOrRemoveTtsPrefs();
    if (!hasVoiceInputPrefs && !hasTtsPrefs) {
        // There were no TTS settings and no recognizer settings,
        // so it should be safe to hide the preference category
        // entirely.
        AutoPtr<IPreferenceScreen> screen;
        IPreferenceFragment::Probe(mFragment)->GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(
                IPreference::Probe(mVoiceCategory), &res);
    }
}

Boolean VoiceInputOutputSettings::PopulateOrRemoveVoiceInputPrefs()
{
    AutoPtr<IActivity> activity;
    IFragment::Probe(mFragment)->GetActivity((IActivity**)&activity);
    AutoPtr<VoiceInputHelper> helper = new VoiceInputHelper(IContext::Probe(activity));
    if (!helper->HasItems()) {
        Boolean res;
        IPreferenceGroup::Probe(mVoiceCategory)->RemovePreference(
                mVoiceInputSettingsPref, &res);
        return FALSE;
    }

    return TRUE;
}

Boolean VoiceInputOutputSettings::PopulateOrRemoveTtsPrefs()
{
    AutoPtr<IList> list;
    mTtsEngines->GetEngines((IList**)&list);
    Boolean res;
    list->IsEmpty(&res);
    if (res) {
        IPreferenceGroup::Probe(mVoiceCategory)->RemovePreference(mTtsSettingsPref, &res);
        return FALSE;
    }

    return TRUE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
