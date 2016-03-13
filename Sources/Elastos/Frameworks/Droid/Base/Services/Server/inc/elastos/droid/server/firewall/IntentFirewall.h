#ifndef __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__
#define __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"
#include "elastos/droid/server/IntentResolver.h"

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::IXml;
using Elastos::Droid::internal.util.ArrayUtils;
using Elastos::Droid::internal.util.XmlUtils;
using Elastos::Droid::Server::IEventLogTags;
using Elastos::Droid::Server::IIntentResolver;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileNotFoundException;
using Elastos::IO::IIOException;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class IntentFirewall : Object
{
private:
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
     * intent. All top level conditions (but not filters) in the rule must match for the rule as a
     * whole to match.
     *
     * If the rule matches, then we block or log the intent, as specified by the rule. If multiple
     * rules match, we combine the block/log flags from any matching rule.
     */
    class Rule
        : public AndFilter
    {
    public:
        //@Override
        //throws IOException, XmlPullParserException
        Rule* ReadFromXml(
            /* [in] */ IXmlPullParser* parser);

        Int32 GetIntentFilterCount();

        FirewallIntentFilter* GetIntentFilter(
            /* [in] */ Int32 index);

        Int32 GetComponentFilterCount();

        AutoPtr<IComponentName> GetComponentFilter(
            /* [in] */ Int32 index);

        Boolean GetBlock();

        Boolean GetLog();

    protected:
        //@Override
        //throws IOException, XmlPullParserException
         void ReadChild(
            /* [in] */ IXmlPullParser* parser);

    private:
        static const String TAG_INTENT_FILTER;      // = "intent-filter";
        static const String TAG_COMPONENT_FILTER;   // = "component-filter";
        static const String ATTR_NAME;              // = "name";

        static const String ATTR_BLOCK;             // = "block";
        static const String ATTR_LOG;               // = "log";

        AutoPtr<IArrayList> mIntentFilters; //= new ArrayList<FirewallIntentFilter>(1);
        AutoPtr<IArrayList> mComponentFilters;    // = new ArrayList<ComponentName>(0);
        Boolean block;
        Boolean log;
    };

    class FirewallIntentFilter
        : public Object
        , public IIntentFilter
    {
    public:
        FirewallIntentFilter(
            /* [in] */ Rule* rule);

    private:
        AutoPtr<Rule> mRule;
    };

    class FirewallIntentResolver
            : public IntentResolver<FirewallIntentFilter, Rule>
    {
    public:
        CARAPI QueryByComponent(
            /* [in] */ IComponentName* componentName,
            /* [in] */ List<Rule>* candidateRules);

        CARAPI AddComponentFilter(
            /* [in] */ IComponentName* componentName,
            /* [in] */ Rule* rule);

    protected:
        //@Override
        Boolean AllowFilterResult(
            /* [in] */ FirewallIntentFilter* filter,
            /* [in] */ List<Rule>* dest);

        //@Override
        Boolean IsPackageForFilter(
            /* [in] */ const String& packageName,
            /* [in] */ FirewallIntentFilter* filter);

        //@Override
        AutoPtr<ArrayOf<FirewallIntentFilter> > NewArray(
            /* [in] */ Int32 size);

        //@Override
        AutoPtr<Rule> NewResult(
            /* [in] */ FirewallIntentFilter* filter,
            /* [in] */ Int32 match,
            /* [in] */ Int32 userId);

        //@Override
        void SortResults(
            /* [in] */ List<Rule>* results);

    private:
        AutoPtr<IArrayMap> mRulesByComponent;  // = new ArrayMap<ComponentName, Rule[]>(0);
    };

    class FirewallHandler
        : public IHandler
    {
    public:
        FirewallHandler(
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

    /**
     * Monitors for the creation/deletion/modification of any .xml files in the rule directory
     */
    class RuleObserver
        : public FileObserver
    {
    public:
        RuleObserver(
            /* [in] */ IFile* monitoredDir);

        //@Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    private:
        static const Int32 MONITORED_EVENTS;
    };

public:
    IntentFirewall(
        /* [in] */ IAMSInterface* ams,
        /* [in] */ IHandler* handler);

    /**
     * This is called from ActivityManager to check if a start activity intent should be allowed.
     * It is assumed the caller is already holding the global ActivityManagerService lock.
     */
    Boolean CheckStartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ IApplicationInfo* resolvedApp);

    Boolean CheckService(
        /* [in] */ IComponentName* resolvedService,
        /* [in] */ Intent intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ IApplicationInfo* resolvedApp);

    Boolean CheckBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid);

    Boolean CheckIntent(
        /* [in] */ FirewallIntentResolver* resolver,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ Int32 intentType,
        /* [in] */ IIntent intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid);

public:
    static initIntentFirewall();
    static AutoPtr<IFile> GetRulesDir();

public:
    static const String TAG;        // = "IntentFirewall";

    // throws IOException, XmlPullParserException
    static IFilter* ParseFilter(
        /* [in] */ IXmlPullParser* parser);

    // e.g. /data/system/ifw or /data/secure/system/ifw
private:
    static const File RULES_DIR;    // = new File(Environment->GetSystemSecureDirectory(), "ifw");

    static const Int32 LOG_PACKAGES_MAX_LENGTH;     // = 150;
    static const Int32 LOG_PACKAGES_SUFFICIENT_LENGTH;      // = 125;

    static const String TAG_RULES;          // = "rules";
    static const String TAG_ACTIVITY;       // = "activity";
    static const String TAG_SERVICE;        // = "service";
    static const String TAG_BROADCAST;      // = "broadcast";

    static const Int32 TYPE_ACTIVITY;       // = 0;
    static const Int32 TYPE_BROADCAST;      // = 1;
    static const Int32 TYPE_SERVICE;        // = 2;

    static const HashMap<String, AutoPtr<FilterFactory> > factoryMap;

    AutoPtr<AMSInterface> mAms;
    AutoPtr<RuleObserver> mObserver;

    AutoPtr<FirewallIntentResolver> mActivityResolver;      // = new FirewallIntentResolver();
    AutoPtr<FirewallIntentResolver> mBroadcastResolver;     // = new FirewallIntentResolver();
    AutoPtr<FirewallIntentResolver> mServiceResolver;       // = new FirewallIntentResolver();

    static const ArrayOf<FilterFactory> factories;

private:
    static void LogIntent(
        /* [in] */ Int32 intentType,
        /* [in] */ Intent intent,
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
    static String JoinPackages(
        /* [in] */ const ArrayOf<String>& packages);

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
    void ReadRulesDir(
        /* [in] */ IFile* rulesDir);

    /**
     * Reads rules from the given file and add them to the given resolvers
     */
    void ReadRules(
        /* [in] */ IFile* rulesFile,
        /* [in] */ const ArrayOf<FirewallIntentResolver>& resolvers);

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
    Boolean CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported);

    Boolean SignaturesMatch(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2);

public:
    AutoPtr<FirewallHandler> mHandler;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_IntentFirewall_H__


