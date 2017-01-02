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

#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/CTrustAgentSettings.h"
#include "elastos/droid/settings/TrustAgentUtils.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::CSwitchPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::IComparable;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CTrustAgentSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustAgentSettings::InnerListener, Object, IPreferenceOnPreferenceChangeListener)

CTrustAgentSettings::InnerListener::InnerListener(
    /* [in] */ CTrustAgentSettings* host)
    : mHost(host)
{}

ECode CTrustAgentSettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, newValue, result);
}

//===============================================================================
//                  CTrustAgentSettings::AgentInfo
//===============================================================================

CAR_INTERFACE_IMPL(CTrustAgentSettings::AgentInfo, Object, IAgentInfo)

ECode CTrustAgentSettings::AgentInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (IAgentInfo::Probe(other) != NULL) {
        *result = Object::Equals(mComponent, ((AgentInfo*)IAgentInfo::Probe(other))->mComponent);
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

Int32 CTrustAgentSettings::AgentInfo::CompareTo(
    /* [in] */ AgentInfo* other)
{
    Int32 result;
    IComparable::Probe(mComponent)->CompareTo(other->mComponent, &result);
    return result;
}

//===============================================================================
//                  CTrustAgentSettings
//===============================================================================

const String CTrustAgentSettings::SERVICE_INTERFACE = ITrustAgentService::SERVICE_INTERFACE;

CAR_OBJECT_IMPL(CTrustAgentSettings)

CTrustAgentSettings::CTrustAgentSettings()
{}

CTrustAgentSettings::~CTrustAgentSettings()
{}

ECode CTrustAgentSettings::constructor()
{
    CArraySet::New((IArraySet**)&mActiveAgents);
    return SettingsPreferenceFragment::constructor();
}

ECode CTrustAgentSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);
    AddPreferencesFromResource(R::xml::trust_agent_settings);
    return NOERROR;
}

ECode CTrustAgentSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    UpdateAgents();
    return NOERROR;
}

void CTrustAgentSettings::UpdateAgents()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    if (mAvailableAgents == NULL) {
        mAvailableAgents = FindAvailableTrustAgents();
    }
    if (mLockPatternUtils == NULL) {
        CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    }
    LoadActiveAgents();
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IPreference> pref;
    IPreferenceGroup::Probe(screen)->FindPreference(CoreUtils::Convert("trust_agents"), (IPreference**)&pref);
    IPreferenceGroup* category = IPreferenceGroup::Probe(pref);
    category->RemoveAll();
    AutoPtr<InnerListener> listener = new InnerListener(this);
    Int32 count;
    mAvailableAgents->GetSize(&count);
    Boolean res;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mAvailableAgents->GetValueAt(i, (IInterface**)&obj);
        AgentInfo* agent = (AgentInfo*) IAgentInfo::Probe(obj);
        AutoPtr<ISwitchPreference> preference;
        CSwitchPreference::New(context, (ISwitchPreference**)&preference);
        agent->mPreference = preference;
        IPreference* _pref = IPreference::Probe(preference);
        _pref->SetPersistent(FALSE);
        _pref->SetTitle(agent->mLabel);
        _pref->SetIcon(agent->mIcon);
        _pref->SetPersistent(FALSE);
        _pref->SetOnPreferenceChangeListener(listener);
        ITwoStatePreference::Probe(preference)->SetChecked((mActiveAgents->Contains(agent->mComponent, &res), res));
        category->AddPreference(IPreference::Probe(agent->mPreference), &res);
    }
}

void CTrustAgentSettings::LoadActiveAgents()
{
    AutoPtr<IList> activeTrustAgents; //List<ComponentName>
    mLockPatternUtils->GetEnabledTrustAgents((IList**)&activeTrustAgents);
    if (activeTrustAgents != NULL) {
        mActiveAgents->AddAll(ICollection::Probe(activeTrustAgents));
    }
}

void CTrustAgentSettings::SaveActiveAgents()
{
    mLockPatternUtils->SetEnabledTrustAgents(ICollection::Probe(mActiveAgents));
}

AutoPtr<IArrayMap> CTrustAgentSettings::FindAvailableTrustAgents()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> trustAgentIntent;
    CIntent::New(SERVICE_INTERFACE, (IIntent**)&trustAgentIntent);
    AutoPtr<IList> resolveInfos;// List<ResolveInfo>
    pm->QueryIntentServices(trustAgentIntent, IPackageManager::GET_META_DATA, (IList**)&resolveInfos);

    AutoPtr<IArrayMap> agents;//ArrayMap<ComponentName, AgentInfo>
    CArrayMap::New((IArrayMap**)&agents);
    Int32 count;
    resolveInfos->GetSize(&count);
    agents->EnsureCapacity(count);
    for (Int32 i = 0; i < count; i++ ) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) continue;
        if (!TrustAgentUtils::CheckProvidePermission(resolveInfo, pm)) continue;
        AutoPtr<IComponentName> name = TrustAgentUtils::GetComponentName(resolveInfo);
        AutoPtr<AgentInfo> agentInfo = new AgentInfo();
        resolveInfo->LoadLabel(pm, (ICharSequence**)&agentInfo->mLabel);
        resolveInfo->LoadIcon(pm, (IDrawable**)&agentInfo->mIcon);
        agentInfo->mComponent = name;
        agents->Put(name, (IAgentInfo*)agentInfo);
    }
    return agents;
}

ECode CTrustAgentSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (ISwitchPreference::Probe(preference) != NULL) {
        Int32 count;
        mAvailableAgents->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            mAvailableAgents->GetValueAt(i, (IInterface**)&obj);
            AgentInfo* agent = (AgentInfo*) IAgentInfo::Probe(obj);
            if (agent->mPreference.Get() == ISwitchPreference::Probe(preference)) {
                Boolean value;
                IBoolean::Probe(newValue)->GetValue(&value);
                if (value) {
                    Boolean res;
                    if (mActiveAgents->Contains(agent->mComponent, &res), !res) {
                        mActiveAgents->Add(agent->mComponent);
                    }
                }
                else {
                    mActiveAgents->Remove(agent->mComponent);
                }
                SaveActiveAgents();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos