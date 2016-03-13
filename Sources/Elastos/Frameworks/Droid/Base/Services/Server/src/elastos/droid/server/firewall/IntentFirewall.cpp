#include "elastos/droid/server/firewall/IntentFirewall.h"
#include "elastos/droid/app/AppGlobals.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

String IntentFirewall::TAG("IntentFirewall");

AutoPtr<IFile> IntentFirewall::RULES_DIR;

Int32 IntentFirewall::LOG_PACKAGES_MAX_LENGTH = 150;
Int32 IntentFirewall::LOG_PACKAGES_SUFFICIENT_LENGTH = 125;

String IntentFirewall::TAG_RULES("rules");
String IntentFirewall::TAG_ACTIVITY("activity");
String IntentFirewall::TAG_SERVICE("service");
String IntentFirewall::TAG_BROADCAST("broadcast");

Int32 IntentFirewall::TYPE_ACTIVITY = 0;
Int32 IntentFirewall::TYPE_BROADCAST = 1;
Int32 IntentFirewall::TYPE_SERVICE = 2;

HashMap<String, AutoPtr<FilterFactory> > IntentFirewall::factoryMap;
ArrayOf<AutoPtr<FilterFactory> > IntentFirewall::factories;

AutoPtr<AMSInterface> IntentFirewall::mAms;
AutoPtr<RuleObserver> IntentFirewall::mObserver;

AutoPtr<FirewallIntentResolver> IntentFirewall::mActivityResolver;      // = new FirewallIntentResolver();
AutoPtr<FirewallIntentResolver> IntentFirewall::mBroadcastResolver;     // = new FirewallIntentResolver();
AutoPtr<FirewallIntentResolver> IntentFirewall::mServiceResolver;       // = new FirewallIntentResolver();

IntentFirewall::initIntentFirewall()
{
    // e.g. /data/system/ifw or /data/secure/system/ifw

    AutoPtr<IFile> dir = Environment::GetSystemSecureDirectory();
    CFile::New(dir, String("ifw"), &IntentFirewall::RULES_DIR);

    factories = ArrayOf<AutoPtr<FilterFactory> >::Alloc(18);

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

    mActivityResolver = new FirewallIntentResolver();
    mBroadcastResolver = new FirewallIntentResolver();
    mServiceResolver = new FirewallIntentResolver();

    // load factor ~= .75
    factoryMap = new HashMap<String, AutoPtr<FilterFactory> >(factories.GetLength() * 4 / 3);
    for (Int32 i = 0;  i < factories.GetLength;  i++) {
        AutoPtr<FilterFactory> factory = (*factories)[i];
        factoryMap->Put(factory->GetTagName(), factory);
    }
}

//------------------------------------------------------------------------------
// IntentFirewall::Rule
//------------------------------------------------------------------------------

String IntentFirewall::Rule::TAG_INTENT_FILTER("intent-filter");
String IntentFirewall::Rule::TAG_COMPONENT_FILTER("component-filter");
String IntentFirewall::Rule::ATTR_NAME("name");

String IntentFirewall::Rule::ATTR_BLOCK("block");
String IntentFirewall::Rule::ATTR_LOG("log");

AutoPtr<IArrayList> IntentFirewall::Rule::mIntentFilters;       // = new ArrayList<FirewallIntentFilter>(1);
AutoPtr<IArrayList> IntentFirewall::Rule::mComponentFilters;    // = new ArrayList<ComponentName>(0);
Boolean IntentFirewall::Rule::block;
Boolean IntentFirewall::Rule::log;

Rule* IntentFirewall::Rule::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    String str;

    parser->GetAttributeValue(NULL, ATTR_BLOCK, &str);
    block = StringUtils::ParseBoolean(str);

    parser->GetAttributeValue(NULL, ATTR_LOG, &str);
    log = StringUtils::ParseBoolean(str);

    IntentFirewall::ReadFromXml(parser);
    return this;
}

Int32 IntentFirewall::Rule::GetIntentFilterCount()
{
    Int32 size;

    (*mIntentFilters)->GetSize(&size);
    return size;
}

FirewallIntentFilter* IntentFirewall::Rule::GetIntentFilter(
    /* [in] */ Int32 index)
{
    AutoPtr<FirewallIntentFilter> filter;
    AutoPtr<IInterface> obj;

    (*mIntentFilters)->Get(index, (IInterface**)&obj);
    filter = (FirewallIntentFilter*)IObject::Probe(obj);
    return filter;
}

Int32 IntentFirewall::Rule::GetComponentFilterCount()
{
    Int32 size;

    (*mComponentFilters)->GetSize(&size);
    return size;
}

AutoPtr<IComponentName> IntentFirewall::Rule::GetComponentFilter(
    /* [in] */ Int32 index)
{
    AutoPtr<IComponentName> name;
    AutoPtr<IInterface> obj;

    (*mComponentFilters)->Get(index, (IInterface**)&obj);
    name = (IComponentName*)IObject::Probe(obj);
    return name;

}

Boolean IntentFirewall::Rule::GetBlock()
{
    return block;
}

Boolean IntentFirewall::Rule::GetLog()
{
    return log;
}

void IntentFirewall::Rule::ReadChild(
    /* [in] */ IXmlPullParser* parser)
{
    String currentTag;

    parser->GetName(&currentTag);

    if (currentTag.equals(TAG_INTENT_FILTER)) {
        FirewallIntentFilter intentFilter = new FirewallIntentFilter(this);
        intentFilter.readFromXml(parser);
        mIntentFilters.add(intentFilter);
    } else if (currentTag.equals(TAG_COMPONENT_FILTER)) {
        String componentStr = parser.getAttributeValue(null, ATTR_NAME);
        if (componentStr == null) {
            throw new XmlPullParserException("Component name must be specified.",
                    parser, null);
        }

        ComponentName componentName = ComponentName.unflattenFromString(componentStr);
        if (componentName == null) {
            throw new XmlPullParserException("Invalid component name: " + componentStr);
        }

        mComponentFilters.add(componentName);
    } else {
        super.readChild(parser);
    }
 }


//------------------------------------------------------------------------------
// IntentFirewall::FirewallIntentFilter
//------------------------------------------------------------------------------

IntentFirewall::FirewallIntentFilter::FirewallIntentFilter(
    /* [in] */ Rule* rule)
{
    mRule = rule;
}


//------------------------------------------------------------------------------
// IntentFirewall::FirewallIntentResolver
//------------------------------------------------------------------------------

AutoPtr<IArrayMap> IntentFirewall::FirewallIntentResolver::mRulesByComponent;

IntentFirewall::FirewallIntentResolver::FirewallIntentResolver()
{
    CArrayMap::New((IArrayMap**)&mRulesByComponent);
}

ECode IntentFirewall::FirewallIntentResolver::QueryByComponent(
    /* [in] */ IComponentName* componentName,
    /* [in] */ List<Rule>* candidateRules)
{
    Rule[] rules = mRulesByComponent.get(componentName);
    if (rules != null) {
        candidateRules.addAll(Arrays.asList(rules));
    }
}

ECode IntentFirewall::FirewallIntentResolver::AddComponentFilter(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Rule* rule);

//@Override
Boolean IntentFirewall::FirewallIntentResolver::AllowFilterResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ List<Rule>* dest);

//@Override
Boolean IntentFirewall::FirewallIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ FirewallIntentFilter* filter);

//@Override
AutoPtr<ArrayOf<FirewallIntentFilter> > IntentFirewall::FirewallIntentResolver::NewArray(
    /* [in] */ Int32 size);

//@Override
AutoPtr<Rule> IntentFirewall::FirewallIntentResolver::NewResult(
    /* [in] */ FirewallIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ Int32 userId);

//@Override
void IntentFirewall::FirewallIntentResolver::SortResults(
    /* [in] */ List<Rule>* results)
{
    // there's no need to sort the results
    return;
}

//=======================================================================================
// IntentFirewall
//=======================================================================================

IntentFirewall::IntentFirewall(
    /* [in] */ IAMSInterface* ams,
    /* [in] */ IHandler* handler)
{
    mAms = ams;
    mHandler = new FirewallHandler(handler->GetLooper());
    File rulesDir = GetRulesDir();
    rulesDir->Mkdirs();

    ReadRulesDir(rulesDir);

    mObserver = new RuleObserver(rulesDir);
    mObserver->StartWatching();
}

Boolean IntentFirewall::CheckStartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ IApplicationInfo* resolvedApp)
{
    return CheckIntent(mActivityResolver, intent->GetComponent(), TYPE_ACTIVITY, intent,
            callerUid, callerPid, resolvedType, resolvedApp.uid);
}

Boolean IntentFirewall::CheckService(
    /* [in] */ IComponentName* resolvedService,
    /* [in] */ Intent intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ IApplicationInfo* resolvedApp)
{
    return CheckIntent(mServiceResolver, resolvedService, TYPE_SERVICE, intent, callerUid,
            callerPid, resolvedType, resolvedApp.uid);
}

Boolean IntentFirewall::CheckBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid)
{
    return CheckIntent(mBroadcastResolver, intent->GetComponent(), TYPE_BROADCAST, intent,
            callerUid, callerPid, resolvedType, receivingUid);
}

Boolean IntentFirewall::CheckIntent(
    /* [in] */ FirewallIntentResolver* resolver,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ Int32 intentType,
    /* [in] */ IIntent intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid)
{
    Boolean log = FALSE;
    Boolean block = FALSE;

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

    return !block;
}

void IntentFirewall::LogIntent(
    /* [in] */ Int32 intentType,
    /* [in] */ Intent intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ const String& resolvedType)
{
    // The component shouldn't be NULL, but let's Double check just to be safe
    AutoPtr<IComponentName> cn;
    intent->GetComponent((IComponentName**)&cn);

    String shortComponent = NULL;
    if (cn != NULL) {
        cn->FlattenToShortString(&shortComponent);
    }

    String callerPackages = NULL;
    Int32 callerPackageCount = 0;
    AutoPtr<IPackageManager> pm;
    AppGlobals::GetPackageManager((IPackageManager**)&pm);
    if (pm != NULL) {
        AutoPtr<ArrayOf<String> > callerPackagesArray;
        pm->GetPackagesForUid(callerUid, (ArrayOf<String>**)&callerPackagesArray);
        if (callerPackagesArray != NULL) {
            callerPackageCount = callerPackagesArray.GetLength();
            callerPackages = JoinPackages(callerPackagesArray);
        }
    }

    EventLogTags->WriteIfwIntentMatched(intentType, shortComponent, callerUid,
            callerPackageCount, callerPackages, intent->GetAction(), resolvedType,
            intent->GetDataString(), intent->GetFlags());
}

String IntentFirewall::JoinPackages(
    /* [in] */ const ArrayOf<String>& packages)
{
    Boolean first = TRUE;
    StringBuilder sb = new StringBuilder();
    for (Int32 i = 0;  i < packages.GetLength();  i++) {
        String pkg = packages[i];

        // + 1 length for the comma. This logic technically isn't correct for the first entry,
        // but it's not critical.
        if (sb->Length() + pkg->Length() + 1 < LOG_PACKAGES_MAX_LENGTH) {
            if (!first) {
                sb->Append(",");
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

AutoPtr<IFile> IntentFirewall::GetRulesDir()
{
    return RULES_DIR;
}

void IntentFirewall::ReadRules(
    /* [in] */ IFile* rulesFile,
    /* [in] */ const ArrayOf<FirewallIntentResolver>& resolvers)
{
    AutoPtr<ArrayOf<AutoPtr<FirewallIntentResolver> > > resolvers;
    resolvers = ArrayOf<AutoPtr<FirewallIntentResolver> >::Alloc(3);
    for (Int32 i = 0;  i < resolvers.GetLength();  i++) {
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

void IntentFirewall::ReadRulesDir(
    /* [in] */ IFile* rulesDir)
{
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

AutoPtr<IFilter> IntentFirewall::ParseFilter(
    /* [in] */ IXmlPullParser* parser)
{
   String elementName = parser->GetName();

    FilterFactory factory = factoryMap->Get(elementName);

    if (factory == NULL) {
        throw new XmlPullParserException("Unknown element in filter list: " + elementName);
    }
    return factory->NewFilter(parser);
}

} // Firewall
} // Server
} // Droid
} // Elastos

