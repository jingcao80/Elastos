

package com.android.settings;

using Elastos::Utility::IList;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;

public class TrustAgentSettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceChangeListener {
    private static const String SERVICE_INTERFACE = TrustAgentService.SERVICE_INTERFACE;
    private ArrayMap<ComponentName, AgentInfo> mAvailableAgents;
    private final ArraySet<ComponentName> mActiveAgents = new ArraySet<ComponentName>();
    private LockPatternUtils mLockPatternUtils;

    public class AgentInfo {
        CharSequence label;
        ComponentName component; // service that implements ITrustAgent
        SwitchPreference preference;
        public Drawable icon;

        //@Override
        public Boolean Equals(Object other) {
            if (other instanceof AgentInfo) {
                return component->Equals(((AgentInfo)other).component);
            }
            return TRUE;
        }

        public Int32 CompareTo(AgentInfo other) {
            return component->CompareTo(other.component);
        }
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        AddPreferencesFromResource(R.xml.trust_agent_settings);
    }

    CARAPI OnResume() {
        super->OnResume();
        UpdateAgents();
    };

    private void UpdateAgents() {
        final Context context = GetActivity();
        if (mAvailableAgents == NULL) {
            mAvailableAgents = FindAvailableTrustAgents();
        }
        if (mLockPatternUtils == NULL) {
            mLockPatternUtils = new LockPatternUtils(GetActivity());
        }
        LoadActiveAgents();
        PreferenceGroup category =
                (PreferenceGroup) GetPreferenceScreen()->FindPreference("trust_agents");
        category->RemoveAll();
        final Int32 count = mAvailableAgents->Size();
        for (Int32 i = 0; i < count; i++) {
            AgentInfo agent = mAvailableAgents->ValueAt(i);
            final SwitchPreference preference = new SwitchPreference(context);
            agent.preference = preference;
            preference->SetPersistent(FALSE);
            preference->SetTitle(agent.label);
            preference->SetIcon(agent.icon);
            preference->SetPersistent(FALSE);
            preference->SetOnPreferenceChangeListener(this);
            preference->SetChecked(mActiveAgents->Contains(agent.component));
            category->AddPreference(agent.preference);
        }
    }

    private void LoadActiveAgents() {
        List<ComponentName> activeTrustAgents = mLockPatternUtils->GetEnabledTrustAgents();
        if (activeTrustAgents != NULL) {
            mActiveAgents->AddAll(activeTrustAgents);
        }
    }

    private void SaveActiveAgents() {
        mLockPatternUtils->SetEnabledTrustAgents(mActiveAgents);
    }

    ArrayMap<ComponentName, AgentInfo> FindAvailableTrustAgents() {
        PackageManager pm = GetActivity()->GetPackageManager();
        Intent trustAgentIntent = new Intent(SERVICE_INTERFACE);
        List<ResolveInfo> resolveInfos = pm->QueryIntentServices(trustAgentIntent,
                PackageManager.GET_META_DATA);

        ArrayMap<ComponentName, AgentInfo> agents = new ArrayMap<ComponentName, AgentInfo>();
        final Int32 count = resolveInfos->Size();
        agents->EnsureCapacity(count);
        for (Int32 i = 0; i < count; i++ ) {
            ResolveInfo resolveInfo = resolveInfos->Get(i);
            if (resolveInfo.serviceInfo == NULL) continue;
            if (!TrustAgentUtils->CheckProvidePermission(resolveInfo, pm)) continue;
            ComponentName name = TrustAgentUtils->GetComponentName(resolveInfo);
            AgentInfo agentInfo = new AgentInfo();
            agentInfo.label = resolveInfo->LoadLabel(pm);
            agentInfo.icon = resolveInfo->LoadIcon(pm);
            agentInfo.component = name;
            agents->Put(name, agentInfo);
        }
        return agents;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference instanceof SwitchPreference) {
            final Int32 count = mAvailableAgents->Size();
            for (Int32 i = 0; i < count; i++) {
                AgentInfo agent = mAvailableAgents->ValueAt(i);
                if (agent.preference == preference) {
                    if ((Boolean) newValue) {
                        if (!mActiveAgents->Contains(agent.component)) {
                            mActiveAgents->Add(agent.component);
                        }
                    } else {
                        mActiveAgents->Remove(agent.component);
                    }
                    SaveActiveAgents();
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

}
