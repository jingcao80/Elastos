/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.firewall;

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

public class IntentFirewall {
    static const String TAG = "IntentFirewall";

    // e.g. /data/system/ifw or /data/secure/system/ifw
    private static const File RULES_DIR = new File(Environment->GetSystemSecureDirectory(), "ifw");

    private static const Int32 LOG_PACKAGES_MAX_LENGTH = 150;
    private static const Int32 LOG_PACKAGES_SUFFICIENT_LENGTH = 125;

    private static const String TAG_RULES = "rules";
    private static const String TAG_ACTIVITY = "activity";
    private static const String TAG_SERVICE = "service";
    private static const String TAG_BROADCAST = "broadcast";

    private static const Int32 TYPE_ACTIVITY = 0;
    private static const Int32 TYPE_BROADCAST = 1;
    private static const Int32 TYPE_SERVICE = 2;

    private static const HashMap<String, FilterFactory> factoryMap;

    private final AMSInterface mAms;

    private final RuleObserver mObserver;

    private FirewallIntentResolver mActivityResolver = new FirewallIntentResolver();
    private FirewallIntentResolver mBroadcastResolver = new FirewallIntentResolver();
    private FirewallIntentResolver mServiceResolver = new FirewallIntentResolver();

    static {
        FilterFactory[] factories = new FilterFactory[] {
                AndFilter.FACTORY,
                OrFilter.FACTORY,
                NotFilter.FACTORY,

                StringFilter.ACTION,
                StringFilter.COMPONENT,
                StringFilter.COMPONENT_NAME,
                StringFilter.COMPONENT_PACKAGE,
                StringFilter.DATA,
                StringFilter.HOST,
                StringFilter.MIME_TYPE,
                StringFilter.SCHEME,
                StringFilter.PATH,
                StringFilter.SSP,

                CategoryFilter.FACTORY,
                SenderFilter.FACTORY,
                SenderPackageFilter.FACTORY,
                SenderPermissionFilter.FACTORY,
                PortFilter.FACTORY
        };

        // load factor ~= .75
        factoryMap = new HashMap<String, FilterFactory>(factories.length * 4 / 3);
        for (Int32 i=0; i<factories.length; i++) {
            FilterFactory factory = factories[i];
            factoryMap->Put(factory->GetTagName(), factory);
        }
    }

    public IntentFirewall(AMSInterface ams, Handler handler) {
        mAms = ams;
        mHandler = new FirewallHandler(handler->GetLooper());
        File rulesDir = GetRulesDir();
        rulesDir->Mkdirs();

        ReadRulesDir(rulesDir);

        mObserver = new RuleObserver(rulesDir);
        mObserver->StartWatching();
    }

    /**
     * This is called from ActivityManager to check if a start activity intent should be allowed.
     * It is assumed the caller is already holding the global ActivityManagerService lock.
     */
    public Boolean CheckStartActivity(Intent intent, Int32 callerUid, Int32 callerPid,
            String resolvedType, ApplicationInfo resolvedApp) {
        return CheckIntent(mActivityResolver, intent->GetComponent(), TYPE_ACTIVITY, intent,
                callerUid, callerPid, resolvedType, resolvedApp.uid);
    }

    public Boolean CheckService(ComponentName resolvedService, Intent intent, Int32 callerUid,
            Int32 callerPid, String resolvedType, ApplicationInfo resolvedApp) {
        return CheckIntent(mServiceResolver, resolvedService, TYPE_SERVICE, intent, callerUid,
                callerPid, resolvedType, resolvedApp.uid);
    }

    public Boolean CheckBroadcast(Intent intent, Int32 callerUid, Int32 callerPid,
            String resolvedType, Int32 receivingUid) {
        return CheckIntent(mBroadcastResolver, intent->GetComponent(), TYPE_BROADCAST, intent,
                callerUid, callerPid, resolvedType, receivingUid);
    }

    public Boolean CheckIntent(FirewallIntentResolver resolver, ComponentName resolvedComponent,
            Int32 intentType, Intent intent, Int32 callerUid, Int32 callerPid, String resolvedType,
            Int32 receivingUid) {
        Boolean log = FALSE;
        Boolean block = FALSE;

        // For the first pass, find all the rules that have at least one intent-filter or
        // component-filter that matches this intent
        List<Rule> candidateRules;
        candidateRules = resolver->QueryIntent(intent, resolvedType, FALSE, 0);
        if (candidateRules == NULL) {
            candidateRules = new ArrayList<Rule>();
        }
        resolver->QueryByComponent(resolvedComponent, candidateRules);

        // For the second pass, try to match the potentially more specific conditions in each
        // rule against the intent
        for (Int32 i=0; i<candidateRules->Size(); i++) {
            Rule rule = candidateRules->Get(i);
            if (rule->Matches(this, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                    receivingUid)) {
                block |= rule->GetBlock();
                log |= rule->GetLog();

                // if we've already determined that we should both block and log, there's no need
                // to continue trying rules
                if (block && log) {
                    break;
                }
            }
        }

        if (log) {
            LogIntent(intentType, intent, callerUid, resolvedType);
        }

        return !block;
    }

    private static void LogIntent(Int32 intentType, Intent intent, Int32 callerUid,
            String resolvedType) {
        // The component shouldn't be NULL, but let's Double check just to be safe
        ComponentName cn = intent->GetComponent();
        String shortComponent = NULL;
        if (cn != NULL) {
            shortComponent = cn->FlattenToShortString();
        }

        String callerPackages = NULL;
        Int32 callerPackageCount = 0;
        IPackageManager pm = AppGlobals->GetPackageManager();
        if (pm != NULL) {
            try {
                String[] callerPackagesArray = pm->GetPackagesForUid(callerUid);
                if (callerPackagesArray != NULL) {
                    callerPackageCount = callerPackagesArray.length;
                    callerPackages = JoinPackages(callerPackagesArray);
                }
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Remote exception while retrieving packages", ex);
            }
        }

        EventLogTags->WriteIfwIntentMatched(intentType, shortComponent, callerUid,
                callerPackageCount, callerPackages, intent->GetAction(), resolvedType,
                intent->GetDataString(), intent->GetFlags());
    }

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
    private static String JoinPackages(String[] packages) {
        Boolean first = TRUE;
        StringBuilder sb = new StringBuilder();
        for (Int32 i=0; i<packages.length; i++) {
            String pkg = packages[i];

            // + 1 length for the comma. This logic technically isn't correct for the first entry,
            // but it's not critical.
            if (sb->Length() + pkg->Length() + 1 < LOG_PACKAGES_MAX_LENGTH) {
                if (!first) {
                    sb->Append(',');
                } else {
                    first = FALSE;
                }
                sb->Append(pkg);
            } else if (sb->Length() >= LOG_PACKAGES_SUFFICIENT_LENGTH) {
                return sb->ToString();
            }
        }
        if (sb->Length() == 0 && packages.length > 0) {
            String pkg = packages[0];
            // truncating from the end - the last part of the package name is more likely to be
            // interesting/unique
            return pkg->Substring(pkg->Length() - LOG_PACKAGES_MAX_LENGTH + 1) + '-';
        }
        return NULL;
    }

    public static File GetRulesDir() {
        return RULES_DIR;
    }

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
    private void ReadRulesDir(File rulesDir) {
        FirewallIntentResolver[] resolvers = new FirewallIntentResolver[3];
        for (Int32 i=0; i<resolvers.length; i++) {
            resolvers[i] = new FirewallIntentResolver();
        }

        File[] files = rulesDir->ListFiles();
        if (files != NULL) {
            for (Int32 i=0; i<files.length; i++) {
                File file = files[i];

                if (file->GetName()->EndsWith(".xml")) {
                    ReadRules(file, resolvers);
                }
            }
        }

        Slogger::I(TAG, "Read new rules (A:" + resolvers[TYPE_ACTIVITY].FilterSet()->Size() +
                " B:" + resolvers[TYPE_BROADCAST].FilterSet()->Size() +
                " S:" + resolvers[TYPE_SERVICE].FilterSet()->Size() + ")");

        Synchronized(mAms->GetAMSLock()) {
            mActivityResolver = resolvers[TYPE_ACTIVITY];
            mBroadcastResolver = resolvers[TYPE_BROADCAST];
            mServiceResolver = resolvers[TYPE_SERVICE];
        }
    }

    /**
     * Reads rules from the given file and add them to the given resolvers
     */
    private void ReadRules(File rulesFile, FirewallIntentResolver[] resolvers) {
        // some temporary lists to hold the rules while we parse the xml file, so that we can
        // add the rules all at once, after we know there weren't any major structural problems
        // with the xml file
        List<List<Rule>> rulesByType = new ArrayList<List<Rule>>(3);
        for (Int32 i=0; i<3; i++) {
            rulesByType->Add(new ArrayList<Rule>());
        }

        FileInputStream fis;
        try {
            fis = new FileInputStream(rulesFile);
        } catch (FileNotFoundException ex) {
            // Nope, no rules. Nothing else to do!
            return;
        }

        try {
            XmlPullParser parser = Xml->NewPullParser();

            parser->SetInput(fis, NULL);

            XmlUtils->BeginDocument(parser, TAG_RULES);

            Int32 outerDepth = parser->GetDepth();
            while (XmlUtils->NextElementWithin(parser, outerDepth)) {
                Int32 ruleType = -1;

                String tagName = parser->GetName();
                if (tagName->Equals(TAG_ACTIVITY)) {
                    ruleType = TYPE_ACTIVITY;
                } else if (tagName->Equals(TAG_BROADCAST)) {
                    ruleType = TYPE_BROADCAST;
                } else if (tagName->Equals(TAG_SERVICE)) {
                    ruleType = TYPE_SERVICE;
                }

                if (ruleType != -1) {
                    Rule rule = new Rule();

                    List<Rule> rules = rulesByType->Get(ruleType);

                    // if we get an error while parsing a particular rule, we'll just ignore
                    // that rule and continue on with the next rule
                    try {
                        rule->ReadFromXml(parser);
                    } catch (XmlPullParserException ex) {
                        Slogger::E(TAG, "Error reading an intent firewall rule from " + rulesFile, ex);
                        continue;
                    }

                    rules->Add(rule);
                }
            }
        } catch (XmlPullParserException ex) {
            // if there was an error outside of a specific rule, then there are probably
            // structural problems with the xml file, and we should completely ignore it
            Slogger::E(TAG, "Error reading intent firewall rules from " + rulesFile, ex);
            return;
        } catch (IOException ex) {
            Slogger::E(TAG, "Error reading intent firewall rules from " + rulesFile, ex);
            return;
        } finally {
            try {
                fis->Close();
            } catch (IOException ex) {
                Slogger::E(TAG, "Error while closing " + rulesFile, ex);
            }
        }

        for (Int32 ruleType=0; ruleType<rulesByType->Size(); ruleType++) {
            List<Rule> rules = rulesByType->Get(ruleType);
            FirewallIntentResolver resolver = resolvers[ruleType];

            for (Int32 ruleIndex=0; ruleIndex<rules->Size(); ruleIndex++) {
                Rule rule = rules->Get(ruleIndex);
                for (Int32 i=0; i<rule->GetIntentFilterCount(); i++) {
                    resolver->AddFilter(rule->GetIntentFilter(i));
                }
                for (Int32 i=0; i<rule->GetComponentFilterCount(); i++) {
                    resolver->AddComponentFilter(rule->GetComponentFilter(i), rule);
                }
            }
        }
    }

    static Filter ParseFilter(XmlPullParser parser) throws IOException, XmlPullParserException {
        String elementName = parser->GetName();

        FilterFactory factory = factoryMap->Get(elementName);

        if (factory == NULL) {
            throw new XmlPullParserException("Unknown element in filter list: " + elementName);
        }
        return factory->NewFilter(parser);
    }

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
    private static class Rule extends AndFilter {
        private static const String TAG_INTENT_FILTER = "intent-filter";
        private static const String TAG_COMPONENT_FILTER = "component-filter";
        private static const String ATTR_NAME = "name";

        private static const String ATTR_BLOCK = "block";
        private static const String ATTR_LOG = "log";

        private final ArrayList<FirewallIntentFilter> mIntentFilters =
                new ArrayList<FirewallIntentFilter>(1);
        private final ArrayList<ComponentName> mComponentFilters = new ArrayList<ComponentName>(0);
        private Boolean block;
        private Boolean log;

        //@Override
        public Rule ReadFromXml(XmlPullParser parser) throws IOException, XmlPullParserException {
            block = Boolean->ParseBoolean(parser->GetAttributeValue(NULL, ATTR_BLOCK));
            log = Boolean->ParseBoolean(parser->GetAttributeValue(NULL, ATTR_LOG));

            super->ReadFromXml(parser);
            return this;
        }

        //@Override
        protected void ReadChild(XmlPullParser parser) throws IOException, XmlPullParserException {
            String currentTag = parser->GetName();

            if (currentTag->Equals(TAG_INTENT_FILTER)) {
                FirewallIntentFilter intentFilter = new FirewallIntentFilter(this);
                intentFilter->ReadFromXml(parser);
                mIntentFilters->Add(intentFilter);
            } else if (currentTag->Equals(TAG_COMPONENT_FILTER)) {
                String componentStr = parser->GetAttributeValue(NULL, ATTR_NAME);
                if (componentStr == NULL) {
                    throw new XmlPullParserException("Component name must be specified.",
                            parser, NULL);
                }

                ComponentName componentName = ComponentName->UnflattenFromString(componentStr);
                if (componentName == NULL) {
                    throw new XmlPullParserException("Invalid component name: " + componentStr);
                }

                mComponentFilters->Add(componentName);
            } else {
                super->ReadChild(parser);
            }
        }

        public Int32 GetIntentFilterCount() {
            return mIntentFilters->Size();
        }

        public FirewallIntentFilter GetIntentFilter(Int32 index) {
            return mIntentFilters->Get(index);
        }

        public Int32 GetComponentFilterCount() {
            return mComponentFilters->Size();
        }

        public ComponentName GetComponentFilter(Int32 index) {
            return mComponentFilters->Get(index);
        }
        public Boolean GetBlock() {
            return block;
        }

        public Boolean GetLog() {
            return log;
        }
    }

    private static class FirewallIntentFilter extends IntentFilter {
        private final Rule rule;

        public FirewallIntentFilter(Rule rule) {
            this.rule = rule;
        }
    }

    private static class FirewallIntentResolver
            extends IntentResolver<FirewallIntentFilter, Rule> {
        //@Override
        protected Boolean AllowFilterResult(FirewallIntentFilter filter, List<Rule> dest) {
            return !dest->Contains(filter.rule);
        }

        //@Override
        protected Boolean IsPackageForFilter(String packageName, FirewallIntentFilter filter) {
            return TRUE;
        }

        //@Override
        protected FirewallIntentFilter[] NewArray(Int32 size) {
            return new FirewallIntentFilter[size];
        }

        //@Override
        protected Rule NewResult(FirewallIntentFilter filter, Int32 match, Int32 userId) {
            return filter.rule;
        }

        //@Override
        protected void SortResults(List<Rule> results) {
            // there's no need to sort the results
            return;
        }

        CARAPI QueryByComponent(ComponentName componentName, List<Rule> candidateRules) {
            Rule[] rules = mRulesByComponent->Get(componentName);
            if (rules != NULL) {
                candidateRules->AddAll(Arrays->AsList(rules));
            }
        }

        CARAPI AddComponentFilter(ComponentName componentName, Rule rule) {
            Rule[] rules = mRulesByComponent->Get(componentName);
            rules = ArrayUtils->AppendElement(Rule.class, rules, rule);
            mRulesByComponent->Put(componentName, rules);
        }

        private final ArrayMap<ComponentName, Rule[]> mRulesByComponent =
                new ArrayMap<ComponentName, Rule[]>(0);
    }

    final FirewallHandler mHandler;

    private final class FirewallHandler extends Handler {
        public FirewallHandler(Looper looper) {
            Super(looper, NULL, TRUE);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            ReadRulesDir(GetRulesDir());
        }
    };

    /**
     * Monitors for the creation/deletion/modification of any .xml files in the rule directory
     */
    private class RuleObserver extends FileObserver {
        private static const Int32 MONITORED_EVENTS = FileObserver.CREATE|FileObserver.MOVED_TO|
                FileObserver.CLOSE_WRITE|FileObserver.DELETE|FileObserver.MOVED_FROM;

        public RuleObserver(File monitoredDir) {
            Super(monitoredDir->GetAbsolutePath(), MONITORED_EVENTS);
        }

        //@Override
        CARAPI OnEvent(Int32 event, String path) {
            if (path->EndsWith(".xml")) {
                // we wait 250ms before taking any action on an event, in order to dedup multiple
                // events. E.g. a delete event followed by a create event followed by a subsequent
                // write+close event
                mHandler->RemoveMessages(0);
                mHandler->SendEmptyMessageDelayed(0, 250);
            }
        }
    }

    /**
     * This interface contains the methods we need from ActivityManagerService. This allows AMS to
     * export these methods to us without making them public, and also makes it easier to test this
     * component.
     */
    public interface AMSInterface {
        Int32 CheckComponentPermission(String permission, Int32 pid, Int32 uid,
                Int32 owningUid, Boolean exported);
        Object GetAMSLock();
    }

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
    Boolean CheckComponentPermission(String permission, Int32 pid, Int32 uid, Int32 owningUid,
            Boolean exported) {
        return mAms->CheckComponentPermission(permission, pid, uid, owningUid, exported) ==
                PackageManager.PERMISSION_GRANTED;
    }

    Boolean SignaturesMatch(Int32 uid1, Int32 uid2) {
        try {
            IPackageManager pm = AppGlobals->GetPackageManager();
            return pm->CheckUidSignatures(uid1, uid2) == PackageManager.SIGNATURE_MATCH;
        } catch (RemoteException ex) {
            Slogger::E(TAG, "Remote exception while checking signatures", ex);
            return FALSE;
        }
    }

}
