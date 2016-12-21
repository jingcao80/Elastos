#include "elastos/droid/server/firewall/IntentFirewall.h"
#include "elastos/droid/server/firewall/OrFilter.h"
#include "elastos/droid/server/firewall/NotFilter.h"
#include "elastos/droid/server/firewall/StringFilter.h"
#include "elastos/droid/server/firewall/CategoryFilter.h"
#include "elastos/droid/server/firewall/SenderFilter.h"
#include "elastos/droid/server/firewall/SenderPackageFilter.h"
#include "elastos/droid/server/firewall/SenderPermissionFilter.h"
#include "elastos/droid/server/firewall/PortFilter.h"

/*
#include "elastos/droid/app/AppGlobals.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Droid::Utility::CArrayMap;
*/
#include <cutils/log.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/utility/Xml.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Internal.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include "elastos/droid/app/AppGlobals.h"
//#include "elastos/droid/os/FileObserver.h"

// using Elastos::Droid::Os::CEnvironment;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::Xml;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Arrays;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::EIID_IFileObserver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

const String IntentFirewall::TAG("IntentFirewall");

const AutoPtr<IFile> IntentFirewall::RULES_DIR;

const Int32 IntentFirewall::LOG_PACKAGES_MAX_LENGTH = 150;
const Int32 IntentFirewall::LOG_PACKAGES_SUFFICIENT_LENGTH = 125;

const String IntentFirewall::TAG_RULES("rules");
const String IntentFirewall::TAG_ACTIVITY("activity");
const String IntentFirewall::TAG_SERVICE("service");
const String IntentFirewall::TAG_BROADCAST("broadcast");

const Int32 IntentFirewall::TYPE_ACTIVITY = 0;
const Int32 IntentFirewall::TYPE_BROADCAST = 1;
const Int32 IntentFirewall::TYPE_SERVICE = 2;

AutoPtr<HashMap<String, AutoPtr<FilterFactory> > > IntentFirewall::factoryMap;
AutoPtr<ArrayOf<FilterFactory*> > IntentFirewall::factories;

void IntentFirewall::initIntentFirewall()
{
    // e.g. /data/system/ifw or /data/secure/system/ifw

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dir;
    env->GetSystemSecureDirectory((IFile**)&dir);
    CFile::New((IFile*)dir, String("ifw"), (IFile**)&IntentFirewall::RULES_DIR);

    factories = ArrayOf<FilterFactory*>::Alloc(18);

    (*factories)[0] = AndFilter::FACTORY;
    (*factories)[1] = OrFilter::FACTORY;
    (*factories)[2] = NotFilter::FACTORY;

    (*factories)[3] = StringFilter::ACTION;
    (*factories)[4] = StringFilter::COMPONENT;
    (*factories)[5] = StringFilter::COMPONENT_NAME;
    (*factories)[6] = StringFilter::COMPONENT_PACKAGE;
    (*factories)[7] = StringFilter::DATA;
    (*factories)[8] = StringFilter::HOST;
    (*factories)[9] = StringFilter::MIME_TYPE;
    (*factories)[10] = StringFilter::SCHEME;
    (*factories)[11] = StringFilter::PATH;
    (*factories)[12] = StringFilter::SSP;

    (*factories)[13] = CategoryFilter::FACTORY;
    (*factories)[14] = SenderFilter::FACTORY;
    (*factories)[15] = SenderPackageFilter::FACTORY;
    (*factories)[16] = SenderPermissionFilter::FACTORY;
    (*factories)[17] = PortFilter::FACTORY;

    /* don't initialize them here

    mActivityResolver = new FirewallIntentResolver();
    mBroadcastResolver = new FirewallIntentResolver();
    mServiceResolver = new FirewallIntentResolver();
    */

    // load factor ~= .75
    factoryMap = new HashMap<String, AutoPtr<FilterFactory> >((*factories).GetLength() * 4 / 3);
    for (Int32 i = 0;  i < (*factories).GetLength();  i++) {
        AutoPtr<FilterFactory> factory = (*factories)[i];
        (*factoryMap)[factory->GetTagName()] = factory;
    }
}

//------------------------------------------------------------------------------
// Rule
//------------------------------------------------------------------------------

const String Rule::TAG_INTENT_FILTER("intent-filter");
const String Rule::TAG_COMPONENT_FILTER("component-filter");
const String Rule::ATTR_NAME("name");

const String Rule::ATTR_BLOCK("block");
const String Rule::ATTR_LOG("log");

Rule* Rule::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    String str;

    parser->GetAttributeValue(String(NULL), ATTR_BLOCK, &str);
    block = StringUtils::ParseBoolean(str);

    parser->GetAttributeValue(String(NULL), ATTR_LOG, &str);
    log = StringUtils::ParseBoolean(str);

    FilterList::ReadFromXml(parser);
    return this;
}

Int32 Rule::GetIntentFilterCount()
{
    Int32 size;

    (*mIntentFilters).GetSize(&size);
    return size;
}

FirewallIntentFilter* Rule::GetIntentFilter(
    /* [in] */ Int32 index)
{
    AutoPtr<FirewallIntentFilter> filter;
    AutoPtr<IInterface> obj;

    mIntentFilters->Get(index, (IInterface**)&obj);
    filter = (FirewallIntentFilter*)IObject::Probe(obj);
    return filter;
}

Int32 Rule::GetComponentFilterCount()
{
    Int32 size;

    (*mComponentFilters).GetSize(&size);
    return size;
}

AutoPtr<IComponentName> Rule::GetComponentFilter(
    /* [in] */ Int32 index)
{
    AutoPtr<IComponentName> name;
    AutoPtr<IInterface> obj;

    mComponentFilters->Get(index, (IInterface**)&obj);
    name = (IComponentName*)IObject::Probe(obj);
    return name;

}

Boolean Rule::GetBlock()
{
    return block;
}

Boolean Rule::GetLog()
{
    return log;
}

void Rule::ReadChild(
    /* [in] */ IXmlPullParser* parser)
{
    String currentTag;
/*
    parser->GetName(&currentTag);

    if (currentTag.Equals(TAG_INTENT_FILTER)) {
        AutoPtr<FirewallIntentFilter> intentFilter = new FirewallIntentFilter(this);
        intentFilter->ReadFromXml(parser);
        mIntentFilters->add(intentFilter);
    } else if (currentTag.Equals(TAG_COMPONENT_FILTER)) {
        String componentStr = parser->GetAttributeValue(null, ATTR_NAME);
        if (componentStr == NULL) {
            //throw new XmlPullParserException("Component name must be specified.",
            //        parser, null);
        }

        ComponentName componentName = ComponentName.unflattenFromString(componentStr);
        if (componentName == NULL) {
            throw new XmlPullParserException("Invalid component name: " + componentStr);
        }

        mComponentFilters.add(componentName);
    } else {
        super.readChild(parser);
    }
*/
 }


//------------------------------------------------------------------------------
// FirewallIntentFilter
//------------------------------------------------------------------------------

FirewallIntentFilter::FirewallIntentFilter(
    /* [in] */ Rule* rule)
{
    mRule = rule;
}

//------------------------------------------------------------------------------
// IntentFirewall::FirewallIntentResolver
//------------------------------------------------------------------------------

IntentFirewall::FirewallIntentResolver::FirewallIntentResolver()
{
    CArrayMap::New((IArrayMap**)&mRulesByComponent);
}

ECode IntentFirewall::FirewallIntentResolver::QueryByComponent(
    /* [in] */ IComponentName* componentName,
    /* [in] */ List<Rule>* candidateRules)
{
/*
    Rule[] rules = mRulesByComponent.get(componentName);
    if (rules != null) {
        candidateRules.addAll(Arrays.asList(rules));
    }
*/
    return NOERROR;
}

ECode IntentFirewall::FirewallIntentResolver::AddComponentFilter(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Rule* rule)
{
    return NOERROR;
}

//@Override
Boolean IntentFirewall::FirewallIntentResolver::AllowFilterResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ List<Rule>* dest)
{
    return TRUE;
}

//@Override
Boolean IntentFirewall::FirewallIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ FirewallIntentFilter* filter)
{
    return TRUE;
}

//@Override
AutoPtr<ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*> > IntentFirewall::FirewallIntentResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*> > filters =
            ArrayOf<Elastos::Droid::Server::Firewall::FirewallIntentFilter*>::Alloc(size);
    return filters;
}

//@Override
AutoPtr<Rule> IntentFirewall::FirewallIntentResolver::NewResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ Int32 userId)
{
    return filter->mRule;
}

//@Override
void IntentFirewall::FirewallIntentResolver::SortResults(
    /* [in] */ List<Rule>* results)
{
    // there's no need to sort the results
    return;
}


//------------------------------------------------------------------------------
// IntentFirewall::FirewallHandler
//------------------------------------------------------------------------------

IntentFirewall::FirewallHandler::FirewallHandler(
    /* [in] */ ILooper* looper)
{}

//@Override
ECode IntentFirewall::FirewallHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}


//------------------------------------------------------------------------------
// IntentFirewall::RuleObserver
//------------------------------------------------------------------------------

IntentFirewall::RuleObserver::RuleObserver(
    /* [in] */ IFile* monitoredDir)
{}

//@Override
ECode IntentFirewall::RuleObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    return NOERROR;
}


//=======================================================================================
// IntentFirewall
//=======================================================================================

IntentFirewall::IntentFirewall(
    /* [in] */ IAMSInterface* ams,
    /* [in] */ IHandler* handler)
{
    mAms = ams;
    AutoPtr<ILooper> looper;
    Boolean succeeded;

    handler->GetLooper((ILooper**)&looper);
    mHandler = new FirewallHandler(looper);
    AutoPtr<IFile> rulesDir = GetRulesDir();
    rulesDir->Mkdirs(&succeeded);

    ReadRulesDir(rulesDir);

    mObserver = new RuleObserver(rulesDir);
    ((FileObserver*)mObserver)->StartWatching();
}

ECode IntentFirewall::CheckStartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ IApplicationInfo* resolvedApp,
    /* [out] */ Boolean *ret)
{
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
    AutoPtr<IComponentName> resolvedComponent;
    intent->GetComponent((IComponentName**)&resolvedComponent);

    return CheckIntent(mBroadcastResolver, resolvedComponent, TYPE_BROADCAST, intent,
            callerUid, callerPid, resolvedType, receivingUid, ret);
}

ECode IntentFirewall::CheckIntent(
    /* [in] */ FirewallIntentResolver* resolver,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ Int32 intentType,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
//    Boolean log = FALSE;
    Boolean block = FALSE;
/*
    // For the first pass, find all the rules that have at least one intent-filter or
    // component-filter that matches this intent
    AutoPtr<List<Rule> > candidateRules;
    candidateRules = resolver->QueryIntent(intent, resolvedType, FALSE, 0);
    if (candidateRules == NULL) {
        candidateRules = new ArrayList<Rule>();
    }
    resolver->QueryByComponent(resolvedComponent, candidateRules);

    // For the second pass, try to match the potentially more specific conditions in each
    // rule against the intent
    for (Int32 i = 0;  i < candidateRules->Size();  i++) {
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
*/
    return !block;
}

ECode IntentFirewall::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported,
    /* [out] */ Boolean *ret)
{
//    return mAms.checkComponentPermission(permission, pid, uid, owningUid, exported) ==
//            PackageManager.PERMISSION_GRANTED;
    return NOERROR;
}

ECode IntentFirewall::SignaturesMatch(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Boolean *ret)
{
    *ret = FALSE;

    AutoPtr<IIPackageManager> pm;
    pm = AppGlobals::GetPackageManager();
    if (pm != NULL) {
        Int32 result;
        pm->CheckUidSignatures(uid1, uid2, &result);
        if (result == IPackageManager::SIGNATURE_MATCH) {
            *ret = TRUE;
        }
    }

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

    String shortComponent;
    if (cn != NULL) {
        cn->FlattenToShortString(&shortComponent);
    }

    String callerPackages;
    Int32 callerPackageCount = 0;
    AutoPtr<IIPackageManager> pm;
    pm = AppGlobals::GetPackageManager();
    if (pm != NULL) {
        AutoPtr<ArrayOf<String> > callerPackagesArray;
        pm->GetPackagesForUid(callerUid, (ArrayOf<String>**)&callerPackagesArray);
        if (callerPackagesArray != NULL) {
            callerPackageCount = (*callerPackagesArray).GetLength();
            callerPackages = JoinPackages((const ArrayOf<String>&)callerPackagesArray);
        }
    }

    String action;
    String dataString;
    Int32 flags;
    intent->GetAction(&action);
    intent->GetDataString(&dataString);
    intent->GetFlags(&flags);
/*    EventLogTags->WriteIfwIntentMatched(intentType, shortComponent, callerUid,
            callerPackageCount, callerPackages, action, resolvedType,
            dataString, flags);
*/
}

String IntentFirewall::JoinPackages(
    /* [in] */ const ArrayOf<String>& packages)
{
/*
    Boolean first = TRUE;
    StringBuilder sb = new StringBuilder();
    for (Int32 i = 0;  i < packages.GetLength();  i++) {
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
*/
    return String(NULL);
}

AutoPtr<IFile> IntentFirewall::GetRulesDir()
{
    return RULES_DIR;
}

void IntentFirewall::ReadRulesDir(
    /* [in] */ IFile* rulesDir)
{
/*
    AutoPtr<ArrayOf<AutoPtr<FirewallIntentResolver> > > resolvers;
    resolvers = ArrayOf<AutoPtr<FirewallIntentResolver> >::Alloc(3);
    for (Int32 i = 0;  i < (*resolvers).GetLength();  i++) {
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

    {    AutoLock syncLock(mAms->GetAMSLock());
        mActivityResolver = resolvers[TYPE_ACTIVITY];
        mBroadcastResolver = resolvers[TYPE_BROADCAST];
        mServiceResolver = resolvers[TYPE_SERVICE];
    }
*/
}

void IntentFirewall::ReadRules(
    /* [in] */ IFile* rulesFile,
    /* [in] */ const ArrayOf<FirewallIntentResolver>& resolvers)
{
/*
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
*/
}

AutoPtr<IFilter> IntentFirewall::ParseFilter(
    /* [in] */ IXmlPullParser* parser)
{
   String elementName;
   parser->GetName(&elementName);
   HashMap<String, AutoPtr<FilterFactory> >::Iterator it;


    AutoPtr<FilterFactory> factory;
    it = factoryMap->Find(elementName);
    if (it != factoryMap->End()) {
        factory = it->mSecond;
    }

    if (factory == NULL) {
        //throw new XmlPullParserException("Unknown element in filter list: " + elementName);
        return NULL;
    }
    return factory->NewFilter(parser);
}

} // Firewall
} // Server
} // Droid
} // Elastos

