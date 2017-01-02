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

#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include "elastos/droid/server/firewall/OrFilter.h"
#include "elastos/droid/server/firewall/NotFilter.h"
#include "elastos/droid/server/firewall/StringFilter.h"
#include "elastos/droid/server/firewall/CategoryFilter.h"
#include "elastos/droid/server/firewall/SenderFilter.h"
#include "elastos/droid/server/firewall/SenderPackageFilter.h"
#include "elastos/droid/server/firewall/SenderPermissionFilter.h"
#include "elastos/droid/server/firewall/PortFilter.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/utility/Xml.h"

#include <cutils/log.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::EIID_IFileObserver;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::ICollection;
using Elastos::Utility::Arrays;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

const String IntentFirewall::TAG("IntentFirewall");

static AutoPtr<IFile> Init_RULES_DIR()
{
    // e.g. /data/system/ifw or /data/secure/system/ifw

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dir;
    env->GetSystemSecureDirectory((IFile**)&dir);
    AutoPtr<IFile> f;
    CFile::New((IFile*)dir, String("ifw"), (IFile**)&f);
    return f;
}
const AutoPtr<IFile> IntentFirewall::RULES_DIR = Init_RULES_DIR();

const Int32 IntentFirewall::LOG_PACKAGES_MAX_LENGTH = 150;
const Int32 IntentFirewall::LOG_PACKAGES_SUFFICIENT_LENGTH = 125;

const String IntentFirewall::TAG_RULES("rules");
const String IntentFirewall::TAG_ACTIVITY("activity");
const String IntentFirewall::TAG_SERVICE("service");
const String IntentFirewall::TAG_BROADCAST("broadcast");

const Int32 IntentFirewall::TYPE_ACTIVITY = 0;
const Int32 IntentFirewall::TYPE_BROADCAST = 1;
const Int32 IntentFirewall::TYPE_SERVICE = 2;

static AutoPtr<ArrayOf<FilterFactory*> > Init_factories()
{
    AutoPtr<ArrayOf<FilterFactory*> > fct = ArrayOf<FilterFactory*>::Alloc(18);

    fct->Set(0, AndFilter::FACTORY);
    fct->Set(1, OrFilter::FACTORY);
    fct->Set(2, NotFilter::FACTORY);

    fct->Set(3, StringFilter::ACTION);
    fct->Set(4, StringFilter::COMPONENT);
    fct->Set(5, StringFilter::COMPONENT_NAME);
    fct->Set(6, StringFilter::COMPONENT_PACKAGE);
    fct->Set(7, StringFilter::DATA);
    fct->Set(8, StringFilter::HOST);
    fct->Set(9, StringFilter::MIME_TYPE);
    fct->Set(10, StringFilter::SCHEME);
    fct->Set(11, StringFilter::PATH);
    fct->Set(12, StringFilter::SSP);

    fct->Set(13, CategoryFilter::FACTORY);
    fct->Set(14, SenderFilter::FACTORY);
    fct->Set(15, SenderPackageFilter::FACTORY);
    fct->Set(16, SenderPermissionFilter::FACTORY);
    fct->Set(17, PortFilter::FACTORY);
    return fct;
}
AutoPtr<ArrayOf<FilterFactory*> > IntentFirewall::mFactories = Init_factories();

static AutoPtr<HashMap<String, AutoPtr<FilterFactory> > > Init_factoryMap()
{
    // load factor ~= .75
    AutoPtr<HashMap<String, AutoPtr<FilterFactory> > > fmap =
            new HashMap<String, AutoPtr<FilterFactory> >((*IntentFirewall::mFactories).GetLength() * 4 / 3);
    for (Int32 i = 0; i < (*IntentFirewall::mFactories).GetLength(); i++) {
        AutoPtr<FilterFactory> factory = (*IntentFirewall::mFactories)[i];
        (*fmap)[factory->GetTagName()] = factory;
    }
    return fmap;
}
AutoPtr<HashMap<String, AutoPtr<FilterFactory> > > IntentFirewall::mFactoryMap = Init_factoryMap();

//------------------------------------------------------------------------------
// Rule
//------------------------------------------------------------------------------
const String Rule::TAG_INTENT_FILTER("intent-filter");
const String Rule::TAG_COMPONENT_FILTER("component-filter");
const String Rule::ATTR_NAME("name");

const String Rule::ATTR_BLOCK("block");
const String Rule::ATTR_LOG("log");

CAR_INTERFACE_IMPL(Rule, AndFilter, IRule)

Rule::Rule()
    : mBlock(FALSE)
    , mLog(FALSE)
{
    CArrayList::New(1, (IArrayList**)&mIntentFilters);
    CArrayList::New(0, (IArrayList**)&mComponentFilters);
}

ECode Rule::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    String str;

    parser->GetAttributeValue(String(NULL), ATTR_BLOCK, &str);
    mBlock = StringUtils::ParseBoolean(str);

    parser->GetAttributeValue(String(NULL), ATTR_LOG, &str);
    mLog = StringUtils::ParseBoolean(str);

    return FilterList::ReadFromXml(parser);
}

ECode Rule::ReadChild(
    /* [in] */ IXmlPullParser* parser)
{
    String currentTag;
    parser->GetName(&currentTag);

    if (currentTag.Equals(TAG_INTENT_FILTER)) {
        AutoPtr<FirewallIntentFilter> intentFilter = new FirewallIntentFilter(this);
        intentFilter->ReadFromXml(parser);
        mIntentFilters->Add(IIntentFilter::Probe(intentFilter));
    }
    else if (currentTag.Equals(TAG_COMPONENT_FILTER)) {
        String componentStr;
        parser->GetAttributeValue(String(NULL), ATTR_NAME, &componentStr);
        if (componentStr.IsNull()) {
            Slogger::I("Rule", "Component name must be specified.%p", parser);
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        AutoPtr<IComponentNameHelper> hlp;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&hlp);
        AutoPtr<IComponentName> componentName;
        hlp->UnflattenFromString(componentStr, (IComponentName**)&componentName);
        if (componentName == NULL) {
            Slogger::I("Rule", "Invalid component name: %s", componentStr.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        mComponentFilters->Add(componentName);
    }
    else {
        return AndFilter::ReadChild(parser);
    }
    return NOERROR;
 }

ECode Rule::GetIntentFilterCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mIntentFilters->GetSize(result);
}

ECode Rule::GetIntentFilter(
    /* [in] */ Int32 index,
    /* [out] */ IFirewallIntentFilter** result)
{
    AutoPtr<IInterface> obj;
    mIntentFilters->Get(index, (IInterface**)&obj);
    *result = IFirewallIntentFilter::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Rule::GetComponentFilterCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mComponentFilters->GetSize(result);
}

ECode Rule::GetComponentFilter(
    /* [in] */ Int32 index,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mComponentFilters->Get(index, (IInterface**)&obj);
    *result = IComponentName::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Rule::GetBlock(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlock;
    return NOERROR;
}

ECode Rule::GetLog(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLog;
    return NOERROR;
}

//------------------------------------------------------------------------------
// FirewallIntentFilter
//------------------------------------------------------------------------------
CAR_INTERFACE_IMPL(FirewallIntentFilter, IntentFilter, IFirewallIntentFilter)

FirewallIntentFilter::FirewallIntentFilter(
    /* [in] */ Rule* rule)
{
    mRule = rule;
}

//------------------------------------------------------------------------------
// IntentFirewall::FirewallIntentResolver
//------------------------------------------------------------------------------
CAR_INTERFACE_IMPL(IntentFirewall::FirewallIntentResolver, IntentResolver_FirewallIntentFilter_Rule, IFirewallIntentResolver)

IntentFirewall::FirewallIntentResolver::FirewallIntentResolver()
{
    CArrayMap::New((IArrayMap**)&mRulesByComponent);
}

Boolean IntentFirewall::FirewallIntentResolver::AllowFilterResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ List<AutoPtr<Rule> >* dest)
{
    assert(0 && "TODO");
    // return !dest->Contains(filter->mRule);
    return FALSE;
}

Boolean IntentFirewall::FirewallIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ FirewallIntentFilter* filter)
{
    return TRUE;
}

AutoPtr<ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*> > IntentFirewall::FirewallIntentResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*> > filters =
            ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*>::Alloc(size);
    return filters;
}

AutoPtr<Rule> IntentFirewall::FirewallIntentResolver::NewResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ Int32 userId)
{
    return filter->mRule;
}

void IntentFirewall::FirewallIntentResolver::SortResults(
    /* [in] */ List<AutoPtr<Rule> >* results)
{
    // there's no need to sort the results
    return;
}

ECode IntentFirewall::FirewallIntentResolver::QueryByComponent(
    /* [in] */ IComponentName* componentName,
    /* [in] */ List<AutoPtr<Rule> >* candidateRules)
{
    // TODO
    // AutoPtr<ArrayOf<IRule*> > rules = mRulesByComponent.get(componentName);
    // if (rules != NULL) {
    //     AutoPtr<IList> l;
    //     Arrays::AsList(rules, (IList**)&l);
    //     candidateRules->AddAll(ICollection::Probe(l));
    // }
    return NOERROR;
}

ECode IntentFirewall::FirewallIntentResolver::AddComponentFilter(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IRule* rule)
{
    // TODO
    // AutoPtr<ArrayOf<IRule*> > rules = mRulesByComponent.get(componentName);
    // rules = ArrayUtils.appendElement(Rule.class, rules, rule);
    // mRulesByComponent.put(componentName, rules);
    return NOERROR;
}

//------------------------------------------------------------------------------
// IntentFirewall::FirewallHandler
//------------------------------------------------------------------------------

IntentFirewall::FirewallHandler::FirewallHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ IntentFirewall* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{}

ECode IntentFirewall::FirewallHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    mHost->ReadRulesDir(mHost->GetRulesDir());
    return NOERROR;
}

//------------------------------------------------------------------------------
// IntentFirewall::RuleObserver
//------------------------------------------------------------------------------
const Int32 IntentFirewall::RuleObserver::MONITORED_EVENTS = IFileObserver::CREATE | IFileObserver::MOVED_TO |
                IFileObserver::CLOSE_WRITE | IFileObserver::DELETE | IFileObserver::MOVED_FROM;

IntentFirewall::RuleObserver::RuleObserver(
    /* [in] */ IFile* monitoredDir,
    /* [in] */ IntentFirewall* host)
    : mHost(host)
{
    String str;
    monitoredDir->GetAbsolutePath(&str);
    FileObserver::constructor(str, MONITORED_EVENTS);
}

ECode IntentFirewall::RuleObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    if (path.EndWith(".xml")) {
        // we wait 250ms before taking any action on an event, in order to dedup multiple
        // events. E.g. a delete event followed by a create event followed by a subsequent
        // write+close event
        mHost->mHandler->RemoveMessages(0);
        Boolean bRes = FALSE;
        mHost->mHandler->SendEmptyMessageDelayed(0, 250, &bRes);
    }
    return NOERROR;
}

//=======================================================================================
// IntentFirewall
//=======================================================================================
CAR_INTERFACE_IMPL(IntentFirewall, Object, IIntentFirewall)

IntentFirewall::IntentFirewall()
{
    mActivityResolver = new FirewallIntentResolver();
    mBroadcastResolver = new FirewallIntentResolver();
    mServiceResolver = new FirewallIntentResolver();
}

IntentFirewall::IntentFirewall(
    /* [in] */ IAMSInterface* ams,
    /* [in] */ IHandler* handler)
{
    mActivityResolver = new FirewallIntentResolver();
    mBroadcastResolver = new FirewallIntentResolver();
    mServiceResolver = new FirewallIntentResolver();

    constructor(ams, handler);
}

ECode IntentFirewall::constructor(
    /* [in] */ IAMSInterface* ams,
    /* [in] */ IHandler* handler)
{
    mAms = ams;
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHandler = new FirewallHandler(looper, this);
    AutoPtr<IFile> rulesDir = GetRulesDir();
    Boolean succeeded;
    rulesDir->Mkdirs(&succeeded);

    ReadRulesDir(rulesDir);

    mObserver = new RuleObserver(rulesDir, this);
    IFileObserver::Probe(mObserver)->StartWatching();
    return NOERROR;
}

ECode IntentFirewall::CheckStartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ IApplicationInfo* resolvedApp,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IComponentName> resolvedComponent;
    intent->GetComponent((IComponentName**)&resolvedComponent);

    Int32 uid;
    resolvedApp->GetUid(&uid);

    return CheckIntent(mActivityResolver, resolvedComponent, TYPE_ACTIVITY, intent,
            callerUid, callerPid, resolvedType, uid, ret);
}

ECode IntentFirewall::CheckService(
    /* [in] */ IComponentName* resolvedService,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ IApplicationInfo* resolvedApp,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 uid;
    resolvedApp->GetUid(&uid);

    return CheckIntent(mServiceResolver, resolvedService, TYPE_SERVICE, intent, callerUid,
            callerPid, resolvedType, uid, ret);
}

ECode IntentFirewall::CheckBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IComponentName> resolvedComponent;
    intent->GetComponent((IComponentName**)&resolvedComponent);

    return CheckIntent(mBroadcastResolver, resolvedComponent, TYPE_BROADCAST, intent,
            callerUid, callerPid, resolvedType, receivingUid, ret);
}

ECode IntentFirewall::CheckIntent(
    /* [in] */ IFirewallIntentResolver* _resolver,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ Int32 intentType,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    Boolean log = FALSE;
    Boolean block = FALSE;

    // For the first pass, find all the rules that have at least one intent-filter or
    // component-filter that matches this intent
    AutoPtr<FirewallIntentResolver> resolver = (FirewallIntentResolver*)_resolver;
    AutoPtr<List<AutoPtr<Rule> > > candidateRules;
    candidateRules = resolver->QueryIntent(intent, resolvedType, FALSE, 0);
    if (candidateRules == NULL) {
        candidateRules = new List< AutoPtr<Rule> >();   // new ArrayList<Rule>();
    }
    resolver->QueryByComponent(resolvedComponent, candidateRules);

    // For the second pass, try to match the potentially more specific conditions in each
    // rule against the intent
    for (Int32 i = 0; i < candidateRules->GetSize(); i++) {
        AutoPtr<Rule> rule = (*candidateRules)[i];
        Boolean bMatches = FALSE;
        if ((rule->Matches(this, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid, &bMatches), bMatches)) {
            Boolean ruleBlock = FALSE, ruleLog = FALSE;;
            rule->GetBlock(&ruleBlock);
            block |= ruleBlock;
            rule->GetLog(&ruleLog);
            log |= ruleLog;

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

    *ret = !block;
    return NOERROR;
}

void IntentFirewall::LogIntent(
    /* [in] */ Int32 intentType,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ const String& resolvedType)
{
    // The component shouldn't be NULL, but let's Double check just to be safe
    AutoPtr<IComponentName> cn;
    intent->GetComponent((IComponentName**)&cn);

    String shortComponent(NULL);
    if (cn != NULL) {
        cn->FlattenToShortString(&shortComponent);
    }

    String callerPackages(NULL);
    Int32 callerPackageCount = 0;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    if (pm != NULL) {
        AutoPtr<ArrayOf<String> > callerPackagesArray;
        pm->GetPackagesForUid(callerUid, (ArrayOf<String>**)&callerPackagesArray);
        if (callerPackagesArray != NULL) {
            callerPackageCount = callerPackagesArray->GetLength();
            callerPackages = JoinPackages(callerPackagesArray);
        }
    }

    String action;
    String dataString;
    Int32 flags;
    intent->GetAction(&action);
    intent->GetDataString(&dataString);
    intent->GetFlags(&flags);
    //TODO
/*    EventLogTags->WriteIfwIntentMatched(intentType, shortComponent, callerUid,
            callerPackageCount, callerPackages, action, resolvedType,
            dataString, flags);
*/
}

String IntentFirewall::JoinPackages(
    /* [in] */ ArrayOf<String>* packages)
{
    Boolean first = TRUE;
    StringBuilder sb;
    for (Int32 i = 0; i < packages->GetLength(); i++) {
        String pkg = (*packages)[i];

        // + 1 length for the comma. This logic technically isn't correct for the first entry,
        // but it's not critical.
        if (sb.GetLength() + pkg.GetLength() + 1 < LOG_PACKAGES_MAX_LENGTH) {
            if (!first) {
                sb.Append(',');
            }
            else {
                first = FALSE;
            }
            sb.Append(pkg);
        }
        else if (sb.GetLength() >= LOG_PACKAGES_SUFFICIENT_LENGTH) {
            return sb.ToString();
        }
    }
    if (sb.GetLength() == 0 && packages->GetLength() > 0) {
        String pkg = (*packages)[0];
        // truncating from the end - the last part of the package name is more likely to be
        // interesting/unique
        String sub = pkg.Substring(pkg.GetLength() - LOG_PACKAGES_MAX_LENGTH + 1);
        sub.Append('-');
        return sub;
    }
    return String(NULL);
}

AutoPtr<IFile> IntentFirewall::GetRulesDir()
{
    return RULES_DIR;
}

void IntentFirewall::ReadRulesDir(
    /* [in] */ IFile* rulesDir)
{
    AutoPtr<ArrayOf<FirewallIntentResolver*> > resolvers = ArrayOf<FirewallIntentResolver*>::Alloc(3);
    for (Int32 i = 0; i < resolvers->GetLength(); i++) {
        AutoPtr<FirewallIntentResolver> p = new FirewallIntentResolver();
        resolvers->Set(i, p);
    }

    AutoPtr<ArrayOf<IFile*> > files;
    rulesDir->ListFiles((ArrayOf<IFile*>**)&files);
    if (files != NULL) {
        for (Int32 i = 0; i < files->GetLength(); i++) {
            AutoPtr<IFile> file = (*files)[i];

            String name;
            file->GetName(&name);
            if (name.EndWith(".xml")) {
                ReadRules(file, resolvers);
            }
        }
    }

    Slogger::I(TAG, "Read new rules (A:%d B:%d S:%d)",
        (*resolvers)[TYPE_ACTIVITY]->FilterSet()->GetSize(),
        (*resolvers)[TYPE_BROADCAST]->FilterSet()->GetSize(),
        (*resolvers)[TYPE_SERVICE]->FilterSet()->GetSize());

    {
        AutoPtr<IInterface> l;
        mAms->GetAMSLock((IInterface**)&l);
        AutoLock syncLock(l);
        mActivityResolver = (*resolvers)[TYPE_ACTIVITY];
        mBroadcastResolver = (*resolvers)[TYPE_BROADCAST];
        mServiceResolver = (*resolvers)[TYPE_SERVICE];
    }
}

void IntentFirewall::ReadRules(
    /* [in] */ IFile* rulesFile,
    /* [in] */ ArrayOf<FirewallIntentResolver*>* resolvers)
{
    // some temporary lists to hold the rules while we parse the xml file, so that we can
    // add the rules all at once, after we know there weren't any major structural problems
    // with the xml file
    List<AutoPtr<List<AutoPtr<Rule> > > > rulesByType;// = List<AutoPtr<List<Rule> > >::Alloc(3);
    for (Int32 i = 0; i < 3; i++) {
        rulesByType.PushBack(new List<AutoPtr<Rule> >());
    }

    AutoPtr<IFileInputStream> fis;
    // try {
    CFileInputStream::New(rulesFile, (IFileInputStream**)&fis);
    // } catch (FileNotFoundException ex) {
    //     // Nope, no rules. Nothing else to do!
    //     return;
    // }

    // try {
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);

    parser->SetInput(IInputStream::Probe(fis), String(NULL));

    XmlUtils::BeginDocument(parser, TAG_RULES);

    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);
    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        Int32 ruleType = -1;

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ACTIVITY)) {
            ruleType = TYPE_ACTIVITY;
        }
        else if (tagName.Equals(TAG_BROADCAST)) {
            ruleType = TYPE_BROADCAST;
        }
        else if (tagName.Equals(TAG_SERVICE)) {
            ruleType = TYPE_SERVICE;
        }

        if (ruleType != -1) {
            AutoPtr<Rule> rule = new Rule();

            AutoPtr<List<AutoPtr<Rule> > > rules = rulesByType[ruleType];

            // if we get an error while parsing a particular rule, we'll just ignore
            // that rule and continue on with the next rule
            ECode ec = rule->ReadFromXml(parser);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Error reading an intent firewall rule from %p %d", rulesFile, ec);
                continue;
            }

            rules->PushBack(rule);
        }
    }
    // } catch (XmlPullParserException ex) {
    //     // if there was an error outside of a specific rule, then there are probably
    //     // structural problems with the xml file, and we should completely ignore it
    //     Slogger::E(TAG, "Error reading intent firewall rules from " + rulesFile, ex);
    //     return;
    // } catch (IOException ex) {
    //     Slogger::E(TAG, "Error reading intent firewall rules from " + rulesFile, ex);
    //     return;
    // } finally {
    //     try {
            ICloseable::Probe(fis)->Close();
    //     } catch (IOException ex) {
    //         Slogger::E(TAG, "Error while closing " + rulesFile, ex);
    //     }
    // }

    for (Int32 ruleType = 0; ruleType < rulesByType.GetSize(); ruleType++) {
        AutoPtr<List<AutoPtr<Rule> > > rules = rulesByType[ruleType];
        AutoPtr<FirewallIntentResolver> resolver = (*resolvers)[ruleType];

        for (Int32 ruleIndex = 0; ruleIndex < rules->GetSize(); ruleIndex++) {
            AutoPtr<Rule> rule = (*rules)[ruleIndex];
            Int32 count = 0;
            rule->GetIntentFilterCount(&count);
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IFirewallIntentFilter> p;
                rule->GetIntentFilter(i, (IFirewallIntentFilter**)&p);
                resolver->AddFilter((FirewallIntentFilter*)p.Get());
            }
            Int32 componentCount = 0;
            rule->GetComponentFilterCount(&componentCount);
            for (Int32 i = 0; i < componentCount; i++) {
                AutoPtr<IComponentName> cn;
                rule->GetComponentFilter(i, (IComponentName**)&cn);
                resolver->AddComponentFilter(cn, rule);
            }
        }
    }
}

ECode IntentFirewall::ParseFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    String elementName;
    parser->GetName(&elementName);
    HashMap<String, AutoPtr<FilterFactory> >::Iterator it;

    AutoPtr<FilterFactory> factory;
    it = mFactoryMap->Find(elementName);
    if (it != mFactoryMap->End()) {
        factory = it->mSecond;
    }

    if (factory == NULL) {
        //throw new XmlPullParserException("Unknown element in filter list: " + elementName);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return factory->NewFilter(parser, result);
}

ECode IntentFirewall::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 cp = 0;
    mAms->CheckComponentPermission(permission, pid, uid, owningUid, exported, &cp);
    *ret = cp == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode IntentFirewall::SignaturesMatch(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    ECode ec = NOERROR;
    if (pm != NULL) {
        Int32 result = 0;
        ec = pm->CheckUidSignatures(uid1, uid2, &result);
        if (SUCCEEDED(ec) && result == IPackageManager::SIGNATURE_MATCH) {
            *ret = TRUE;
            return NOERROR;
        }
    }

    return ec;
}

} // Firewall
} // Server
} // Droid
} // Elastos
