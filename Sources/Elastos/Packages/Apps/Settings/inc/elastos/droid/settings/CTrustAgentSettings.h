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

#ifndef __ELASTOS_DROID_SETTINGS_CTRUSTAGENTSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CTRUSTAGENTSETTINGS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Settings_CTrustAgentSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CTrustAgentSettings)
    , public SettingsPreferenceFragment
{
public:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        TO_STRING_IMPL("CTrustAgentSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CTrustAgentSettings* host);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CTrustAgentSettings* mHost;
    };

    class AgentInfo
        : public Object
        , public IAgentInfo
    {
    public:
        TO_STRING_IMPL("CTrustAgentSettings::AgentInfo")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        Int32 CompareTo(
            /* [in] */ AgentInfo* other);

    public:
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<IComponentName> mComponent; // service that implements ITrustAgent
        AutoPtr<ISwitchPreference> mPreference;
        AutoPtr<IDrawable> mIcon;
    };

public:
    TO_STRING_IMPL("CTrustedCredentialsSettings")

    CAR_OBJECT_DECL()

    CTrustAgentSettings();

    virtual ~CTrustAgentSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    CARAPI OnResume();

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

protected:
    CARAPI_(AutoPtr<IArrayMap>) /*ArrayMap<ComponentName, AgentInfo>*/ FindAvailableTrustAgents();

private:
    CARAPI_(void) UpdateAgents();

    CARAPI_(void) LoadActiveAgents();

    CARAPI_(void) SaveActiveAgents();

private:
    static const String SERVICE_INTERFACE;
    AutoPtr<IArrayMap> mAvailableAgents; //ArrayMap<ComponentName, AgentInfo>
    AutoPtr<IArraySet> mActiveAgents; //ArraySet<ComponentName>
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CTRUSTAGENTSETTINGS_H__