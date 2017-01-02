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

#ifndef __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__
#define __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/content/IntentFilter.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/FileObserver.h"
#include "elastos/droid/server/firewall/AndFilter.h"
#include "elastos/droid/server/firewall/FilterFactory.h"
#include "elastos/droid/server/IntentResolver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::IArrayMap;

using Elastos::IO::IFile;
using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class Rule;

class FirewallIntentFilter
    : public IntentFilter
    , public IFirewallIntentFilter
{
public:
    CAR_INTERFACE_DECL()

    FirewallIntentFilter(
        /* [in] */ Rule* rule);

public:
    AutoPtr<Rule> mRule;
};

/**
 * Represents a single activity/service/broadcast rule within one of the xml files.
 *
 * Rules are matched against an incoming intent in two phases. The goal of the first phase
 * is to select a subset of rules that might match a given intent.
 *
 * For the first phase, we use a combination of intent filters (via an IntentResolver)
 * and component filters to select which rules to check. If a rule has multiple intent or
 * component filters, only a single filter must match for the rule to be passed on to the
 * second phase.
 *
 * In the second phase, we check the specific conditions in each rule against the values in the
 * intent. All top level conditi ons (but not filters) in the rule must match for the rule as a
 * whole to match.
 *
 * If the rule matches, then we block or log the intent, as specified by the rule. If multiple
 * rules match, we combine the block/log flags from any matching rule.
 */
class Rule
    : public AndFilter
    , public IRule
{
public:
    CAR_INTERFACE_DECL()

    Rule();

    //@Override
    //throws IOException, XmlPullParserException
    CARAPI ReadFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI GetIntentFilterCount(
        /* [out] */ Int32* result);

    CARAPI GetIntentFilter(
        /* [in] */ Int32 index,
        /* [out] */ IFirewallIntentFilter** result);

    CARAPI GetComponentFilterCount(
        /* [out] */ Int32* result);

    CARAPI GetComponentFilter(
        /* [in] */ Int32 index,
        /* [out] */ IComponentName** result);

    CARAPI GetBlock(
        /* [out] */ Boolean* result);

    CARAPI GetLog(
        /* [out] */ Boolean* result);

protected:
    //@Override
    //throws IOException, XmlPullParserException
     CARAPI ReadChild(
        /* [in] */ IXmlPullParser* parser);

private:
    static const String TAG_INTENT_FILTER;
    static const String TAG_COMPONENT_FILTER;
    static const String ATTR_NAME;

    static const String ATTR_BLOCK;
    static const String ATTR_LOG;

    AutoPtr<IArrayList> mIntentFilters;     // ArrayList<FirewallIntentFilter>
    AutoPtr<IArrayList> mComponentFilters;  // ArrayList<ComponentName>
    Boolean mBlock;
    Boolean mLog;
};

} // Firewall
} // Server
} // Droid
} // Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Firewall::FirewallIntentFilter);
DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Firewall::FirewallIntentFilter, IInterface);

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class IntentFirewall
    : public Object
    , public IIntentFirewall
{
private:
    typedef IntentResolver<Elastos::Droid::Server::Firewall::FirewallIntentFilter, Elastos::Droid::Server::Firewall::Rule> IntentResolver_FirewallIntentFilter_Rule;
    class FirewallIntentResolver
        : public IntentResolver_FirewallIntentFilter_Rule
        , public IFirewallIntentResolver
    {
    public:
        CAR_INTERFACE_DECL()

        FirewallIntentResolver();

        CARAPI QueryByComponent(
            /* [in] */ IComponentName* componentName,
            /* [in] */ List<AutoPtr<Rule> >* candidateRules);

        CARAPI AddComponentFilter(
            /* [in] */ IComponentName* componentName,
            /* [in] */ IRule* rule);

    protected:
        //@Override
        CARAPI_(Boolean) AllowFilterResult(
            /* [in] */ FirewallIntentFilter* filter,
            /* [in] */ List<AutoPtr<Rule> >* dest);

        //@Override
        CARAPI_(Boolean) IsPackageForFilter(
            /* [in] */ const String& packageName,
            /* [in] */ FirewallIntentFilter* filter);

        //@Override
        CARAPI_(AutoPtr<ArrayOf<FirewallIntentFilter*> >) NewArray(
            /* [in] */ Int32 size);

        //@Override
        CARAPI_(AutoPtr<Rule>) NewResult(
            /* [in] */ FirewallIntentFilter* filter,
            /* [in] */ Int32 match,
            /* [in] */ Int32 userId);

        //@Override
        CARAPI_(void) SortResults(
            /* [in] */ List<AutoPtr<Rule> >* results);

    private:
        AutoPtr<IArrayMap> mRulesByComponent;
    };

    class FirewallHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("IntentFirewall::FirewallHandler")

        FirewallHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ IntentFirewall* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    public:
        IntentFirewall* mHost;
    };

    /**
     * Monitors for the creation/deletion/modification of any .xml files in the rule directory
     */
    class RuleObserver
        : public FileObserver
    {
    public:
        RuleObserver(
            /* [in] */ IFile* monitoredDir,
            /* [in] */ IntentFirewall* host);

        //@Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    private:
        static const Int32 MONITORED_EVENTS;
        IntentFirewall* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    IntentFirewall();

    IntentFirewall(
        /* [in] */ IAMSInterface* ams,
        /* [in] */ IHandler* handler);

    CARAPI constructor(
        /* [in] */ IAMSInterface* ams,
        /* [in] */ IHandler* handler);

    /**
     * This is called from ActivityManager to check if a start activity intent should be allowed.
     * It is assumed the caller is already holding the global ActivityManagerService lock.
     */
    CARAPI CheckStartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ IApplicationInfo* resolvedApp,
        /* [out] */ Boolean *ret);

    CARAPI CheckService(
        /* [in] */ IComponentName* resolvedService,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ IApplicationInfo* resolvedApp,
        /* [out] */ Boolean *ret);

    CARAPI CheckBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

    CARAPI CheckIntent(
        /* [in] */ IFirewallIntentResolver* resolver,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ Int32 intentType,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

    /**
     * Checks if the caller has access to a component
     *
     * @param permission If present, the caller must have this permission
     * @param pid The pid of the caller
     * @param uid The uid of the caller
     * @param owningUid The uid of the application that owns the component
     * @param exported Whether the component is exported
     * @return True if the caller can access the described component
     */
    CARAPI CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported,
        /* [out] */ Boolean *ret);

    CARAPI SignaturesMatch(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2,
        /* [out] */ Boolean *ret);

    static CARAPI_(AutoPtr<IFile>) GetRulesDir();

    // throws IOException, XmlPullParserException
    static CARAPI ParseFilter(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IFilter** result);

    static CARAPI_(void) LogIntent(
        /* [in] */ Int32 intentType,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ const String& resolvedType);

    /**
     * Joins a list of package names such that the resulting string is no more than
     * LOG_PACKAGES_MAX_LENGTH.
     *
     * Only full package names will be added to the result, unless every package is longer than the
     * limit, in which case one of the packages will be truncated and added. In this case, an
     * additional '-' character will be added to the end of the string, to denote the truncation.
     *
     * If it encounters a package that won't fit in the remaining space, it will continue on to the
     * next package, unless the total length of the built string so far is greater than
     * LOG_PACKAGES_SUFFICIENT_LENGTH, in which case it will stop and return what it has.
     */
    static CARAPI_(String) JoinPackages(
        /* [in] */ ArrayOf<String>* packages);

    /**
     * Reads rules from all xml files (*.xml) in the given directory, and replaces our set of rules
     * with the newly read rules.
     *
     * We only check for files ending in ".xml", to allow for temporary files that are atomically
     * renamed to .xml
     *
     * All calls to this method from the file observer come through a handler and are inherently
     * serialized
     */
    CARAPI_(void) ReadRulesDir(
        /* [in] */ IFile* rulesDir);

    /**
     * Reads rules from the given file and add them to the given resolvers
     */
    CARAPI_(void) ReadRules(
        /* [in] */ IFile* rulesFile,
        /* [in] */ ArrayOf<FirewallIntentResolver*>* resolvers);

    /**
     * Checks if the caller has access to a component
     *
     * @param permission If present, the caller must have this permission
     * @param pid The pid of the caller
     * @param uid The uid of the caller
     * @param owningUid The uid of the application that owns the component
     * @param exported Whether the component is exported
     * @return True if the caller can access the described component
     */
    CARAPI_(Boolean) CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported);

    CARAPI_(Boolean) SignaturesMatch(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2);

public:
    static const String TAG;

    static const AutoPtr<IFile> RULES_DIR;

    static const Int32 LOG_PACKAGES_MAX_LENGTH;
    static const Int32 LOG_PACKAGES_SUFFICIENT_LENGTH;

    static const String TAG_RULES;
    static const String TAG_ACTIVITY;
    static const String TAG_SERVICE;
    static const String TAG_BROADCAST;

    static const Int32 TYPE_ACTIVITY;
    static const Int32 TYPE_BROADCAST;
    static const Int32 TYPE_SERVICE;

    AutoPtr<IAMSInterface> mAms;
    AutoPtr<RuleObserver> mObserver;

    AutoPtr<FirewallIntentResolver> mActivityResolver;
    AutoPtr<FirewallIntentResolver> mBroadcastResolver;
    AutoPtr<FirewallIntentResolver> mServiceResolver;

    static AutoPtr<ArrayOf<FilterFactory*> > mFactories;
    static AutoPtr<HashMap<String, AutoPtr<FilterFactory> > > mFactoryMap;

    AutoPtr<FirewallHandler> mHandler;
};

} // Firewall
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Firewall::IntentFirewall::FirewallIntentResolver, IInterface)

#endif // __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__


