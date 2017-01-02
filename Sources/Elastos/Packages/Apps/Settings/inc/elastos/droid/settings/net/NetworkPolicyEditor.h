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

package com.android.settings.net;

using static android::Net::NetworkPolicy::ICYCLE_NONE;
using static android::Net::NetworkPolicy::ILIMIT_DISABLED;
using static android::Net::NetworkPolicy::ISNOOZE_NEVER;
using static android::Net::NetworkPolicy::IWARNING_DISABLED;
using static android::Net::NetworkTemplate::IMATCH_MOBILE_3G_LOWER;
using static android::Net::NetworkTemplate::IMATCH_MOBILE_4G;
using static android::Net::NetworkTemplate::IMATCH_WIFI;
using static android::Net::NetworkTemplate::IbuildTemplateMobile3gLower;
using static android::Net::NetworkTemplate::IbuildTemplateMobile4g;
using static android::Net::NetworkTemplate::IbuildTemplateMobileAll;
using static com::Android::Internal::Utility::Preconditions::IcheckNotNull;

using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::ITime;

using com::Google::Android::Collect::ILists;
using com::Google::Android::Collect::ISets;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IObjects;

/**
 * Utility class to modify list of {@link NetworkPolicy}. Specifically knows
 * about which policies can coexist. This editor offers thread safety when
 * talking with {@link NetworkPolicyManager}.
 */
public class NetworkPolicyEditor {
    // TODO: be more robust when missing policies from service

    public static const Boolean ENABLE_SPLIT_POLICIES = FALSE;

    private NetworkPolicyManager mPolicyManager;
    private ArrayList<NetworkPolicy> mPolicies = Lists->NewArrayList();

    public NetworkPolicyEditor(NetworkPolicyManager policyManager) {
        mPolicyManager = CheckNotNull(policyManager);
    }

    CARAPI Read() {
        final NetworkPolicy[] policies = mPolicyManager->GetNetworkPolicies();

        Boolean modified = FALSE;
        mPolicies->Clear();
        for (NetworkPolicy policy : policies) {
            // TODO: find better place to clamp these
            if (policy.limitBytes < -1) {
                policy.limitBytes = LIMIT_DISABLED;
                modified = TRUE;
            }
            if (policy.warningBytes < -1) {
                policy.warningBytes = WARNING_DISABLED;
                modified = TRUE;
            }

            mPolicies->Add(policy);
        }

        // force combine any split policies when disabled
        if (!ENABLE_SPLIT_POLICIES) {
            modified |= ForceMobilePolicyCombined();
        }

        // when we cleaned policies above, write back changes
        if (modified) WriteAsync();
    }

    CARAPI WriteAsync() {
        // TODO: consider making more robust by passing through service
        final NetworkPolicy[] policies = mPolicies->ToArray(new NetworkPolicy[mPolicies->Size()]);
        new AsyncTask<Void, Void, Void>() {
            //@Override
            protected Void DoInBackground(Void... params) {
                Write(policies);
                return NULL;
            }
        }.Execute();
    }

    CARAPI Write(NetworkPolicy[] policies) {
        mPolicyManager->SetNetworkPolicies(policies);
    }

    public Boolean HasLimitedPolicy(NetworkTemplate template) {
        final NetworkPolicy policy = GetPolicy(template);
        return policy != NULL && policy.limitBytes != LIMIT_DISABLED;
    }

    public NetworkPolicy GetOrCreatePolicy(NetworkTemplate template) {
        NetworkPolicy policy = GetPolicy(template);
        if (policy == NULL) {
            policy = BuildDefaultPolicy(template);
            mPolicies->Add(policy);
        }
        return policy;
    }

    public NetworkPolicy GetPolicy(NetworkTemplate template) {
        for (NetworkPolicy policy : mPolicies) {
            if (policy.template->Equals(template)) {
                return policy;
            }
        }
        return NULL;
    }

    public NetworkPolicy GetPolicyMaybeUnquoted(NetworkTemplate template) {
        NetworkPolicy policy = GetPolicy(template);
        if (policy != NULL) {
            return policy;
        } else {
            return GetPolicy(BuildUnquotedNetworkTemplate(template));
        }
    }

    @Deprecated
    private static NetworkPolicy BuildDefaultPolicy(NetworkTemplate template) {
        // TODO: move this into framework to share with NetworkPolicyManagerService
        final Int32 cycleDay;
        final String cycleTimezone;
        final Boolean metered;

        if (template->GetMatchRule() == MATCH_WIFI) {
            cycleDay = CYCLE_NONE;
            cycleTimezone = Time.TIMEZONE_UTC;
            metered = FALSE;
        } else {
            final Time time = new Time();
            time->SetToNow();
            cycleDay = time.monthDay;
            cycleTimezone = time.timezone;
            metered = TRUE;
        }

        return new NetworkPolicy(template, cycleDay, cycleTimezone, WARNING_DISABLED,
                LIMIT_DISABLED, SNOOZE_NEVER, SNOOZE_NEVER, metered, TRUE);
    }

    public Int32 GetPolicyCycleDay(NetworkTemplate template) {
        final NetworkPolicy policy = GetPolicy(template);
        return (policy != NULL) ? policy.cycleDay : -1;
    }

    CARAPI SetPolicyCycleDay(NetworkTemplate template, Int32 cycleDay, String cycleTimezone) {
        final NetworkPolicy policy = GetOrCreatePolicy(template);
        policy.cycleDay = cycleDay;
        policy.cycleTimezone = cycleTimezone;
        policy.inferred = FALSE;
        policy->ClearSnooze();
        WriteAsync();
    }

    public Int64 GetPolicyWarningBytes(NetworkTemplate template) {
        final NetworkPolicy policy = GetPolicy(template);
        return (policy != NULL) ? policy.warningBytes : WARNING_DISABLED;
    }

    CARAPI SetPolicyWarningBytes(NetworkTemplate template, Int64 warningBytes) {
        final NetworkPolicy policy = GetOrCreatePolicy(template);
        policy.warningBytes = warningBytes;
        policy.inferred = FALSE;
        policy->ClearSnooze();
        WriteAsync();
    }

    public Int64 GetPolicyLimitBytes(NetworkTemplate template) {
        final NetworkPolicy policy = GetPolicy(template);
        return (policy != NULL) ? policy.limitBytes : LIMIT_DISABLED;
    }

    CARAPI SetPolicyLimitBytes(NetworkTemplate template, Int64 limitBytes) {
        final NetworkPolicy policy = GetOrCreatePolicy(template);
        policy.limitBytes = limitBytes;
        policy.inferred = FALSE;
        policy->ClearSnooze();
        WriteAsync();
    }

    public Boolean GetPolicyMetered(NetworkTemplate template) {
        NetworkPolicy policy = GetPolicy(template);
        if (policy != NULL) {
            return policy.metered;
        } else {
            return FALSE;
        }
    }

    CARAPI SetPolicyMetered(NetworkTemplate template, Boolean metered) {
        Boolean modified = FALSE;

        NetworkPolicy policy = GetPolicy(template);
        if (metered) {
            if (policy == NULL) {
                policy = BuildDefaultPolicy(template);
                policy.metered = TRUE;
                policy.inferred = FALSE;
                mPolicies->Add(policy);
                modified = TRUE;
            } else if (!policy.metered) {
                policy.metered = TRUE;
                policy.inferred = FALSE;
                modified = TRUE;
            }

        } else {
            if (policy == NULL) {
                // ignore when policy doesn't exist
            } else if (policy.metered) {
                policy.metered = FALSE;
                policy.inferred = FALSE;
                modified = TRUE;
            }
        }

        // Remove legacy unquoted policies while we're here
        final NetworkTemplate unquoted = BuildUnquotedNetworkTemplate(template);
        final NetworkPolicy unquotedPolicy = GetPolicy(unquoted);
        if (unquotedPolicy != NULL) {
            mPolicies->Remove(unquotedPolicy);
            modified = TRUE;
        }

        if (modified) WriteAsync();
    }

    /**
     * Remove any split {@link NetworkPolicy}.
     */
    private Boolean ForceMobilePolicyCombined() {
        final HashSet<String> subscriberIds = Sets->NewHashSet();
        for (NetworkPolicy policy : mPolicies) {
            subscriberIds->Add(policy.template->GetSubscriberId());
        }

        Boolean modified = FALSE;
        for (String subscriberId : subscriberIds) {
            modified |= SetMobilePolicySplitInternal(subscriberId, FALSE);
        }
        return modified;
    }

    @Deprecated
    public Boolean IsMobilePolicySplit(String subscriberId) {
        Boolean has3g = FALSE;
        Boolean has4g = FALSE;
        for (NetworkPolicy policy : mPolicies) {
            final NetworkTemplate template = policy.template;
            if (Objects->Equals(subscriberId, template->GetSubscriberId())) {
                switch (template->GetMatchRule()) {
                    case MATCH_MOBILE_3G_LOWER:
                        has3g = TRUE;
                        break;
                    case MATCH_MOBILE_4G:
                        has4g = TRUE;
                        break;
                }
            }
        }
        return has3g && has4g;
    }

    @Deprecated
    CARAPI SetMobilePolicySplit(String subscriberId, Boolean split) {
        if (SetMobilePolicySplitInternal(subscriberId, split)) {
            WriteAsync();
        }
    }

    /**
     * Mutate {@link NetworkPolicy} for given subscriber, combining or splitting
     * the policy as requested.
     *
     * @return {@code TRUE} when any {@link NetworkPolicy} was mutated.
     */
    @Deprecated
    private Boolean SetMobilePolicySplitInternal(String subscriberId, Boolean split) {
        final Boolean beforeSplit = IsMobilePolicySplit(subscriberId);

        final NetworkTemplate template3g = BuildTemplateMobile3gLower(subscriberId);
        final NetworkTemplate template4g = BuildTemplateMobile4g(subscriberId);
        final NetworkTemplate templateAll = BuildTemplateMobileAll(subscriberId);

        if (split == beforeSplit) {
            // already in requested state; skip
            return FALSE;

        } else if (beforeSplit && !split) {
            // combine, picking most restrictive policy
            final NetworkPolicy policy3g = GetPolicy(template3g);
            final NetworkPolicy policy4g = GetPolicy(template4g);

            NetworkPolicy restrictive = NULL;
            if ((policy3g == NULL) && (policy4g == NULL)) {
                return FALSE;
            } else if (policy3g == NULL) {
                restrictive = policy4g;
            } else if (policy4g == NULL) {
                restrictive = policy3g;
            } else {
                restrictive = policy3g->CompareTo(policy4g) < 0 ? policy3g : policy4g;
            }
            mPolicies->Remove(policy3g);
            mPolicies->Remove(policy4g);
            mPolicies->Add(new NetworkPolicy(templateAll, restrictive.cycleDay,
                    restrictive.cycleTimezone, restrictive.warningBytes, restrictive.limitBytes,
                    SNOOZE_NEVER, SNOOZE_NEVER, restrictive.metered, restrictive.inferred));
            return TRUE;

        } else if (!beforeSplit && split) {
            // duplicate existing policy into two rules
            final NetworkPolicy policyAll = GetPolicy(templateAll);
            if (policyAll == NULL) {
                return FALSE;
            }
            mPolicies->Remove(policyAll);
            mPolicies->Add(new NetworkPolicy(template3g, policyAll.cycleDay, policyAll.cycleTimezone,
                    policyAll.warningBytes, policyAll.limitBytes, SNOOZE_NEVER, SNOOZE_NEVER,
                    policyAll.metered, policyAll.inferred));
            mPolicies->Add(new NetworkPolicy(template4g, policyAll.cycleDay, policyAll.cycleTimezone,
                    policyAll.warningBytes, policyAll.limitBytes, SNOOZE_NEVER, SNOOZE_NEVER,
                    policyAll.metered, policyAll.inferred));
            return TRUE;
        } else {
            return FALSE;
        }
    }

    /**
     * Build a revised {@link NetworkTemplate} that matches the same rule, but
     * with an unquoted {@link NetworkTemplate#GetNetworkId()}. Used to work
     * around legacy bugs.
     */
    private static NetworkTemplate BuildUnquotedNetworkTemplate(NetworkTemplate template) {
        if (template == NULL) return NULL;
        final String networkId = template->GetNetworkId();
        final String strippedNetworkId = WifiInfo->RemoveDoubleQuotes(networkId);
        if (!TextUtils->Equals(strippedNetworkId, networkId)) {
            return new NetworkTemplate(
                    template->GetMatchRule(), template->GetSubscriberId(), strippedNetworkId);
        } else {
            return NULL;
        }
    }
}
