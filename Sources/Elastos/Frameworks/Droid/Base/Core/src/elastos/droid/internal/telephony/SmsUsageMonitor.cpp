
#include <elastos/core/AutoLock.h>
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"

using Elastos::Core::AutoLock;
//using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
//using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
//using Elastos::Droid::Internal::Utility::IXmlUtils;

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileReader;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Implement the per-application based SMS control, which limits the number of
 * SMS/MMS messages an app can send in the checking period.
 *
 * This code was formerly part of {@link SMSDispatcher}, and has been moved
 * into a separate class to support instantiation of multiple SMSDispatchers on
 * dual-mode devices that require support for both 3GPP and 3GPP2 format messages.
 */
CAR_INTERFACE_IMPL(SmsUsageMonitor, Handler, ISmsUsageMonitor)

const String SmsUsageMonitor::TAG("SmsUsageMonitor");
const Boolean SmsUsageMonitor::DBG = TRUE; // intentionally enable DBG
const Boolean SmsUsageMonitor::VDBG = FALSE;

const String SmsUsageMonitor::SHORT_CODE_PATH("/data/misc/sms/codes");

/** Default checking period for SMS sent without user permission. */
const Int32 SmsUsageMonitor::DEFAULT_SMS_CHECK_PERIOD = 15 * 60 * 1000;      // 15 minutes

/** Default number of SMS sent in checking period without user permission. */
const Int32 SmsUsageMonitor::DEFAULT_SMS_MAX_COUNT = 30;

/** Return value from {@link #checkDestination} for regular phone numbers. */
const Int32 SmsUsageMonitor::CATEGORY_NOT_SHORT_CODE = 0;

/** Return value from {@link #checkDestination} for Free (no cost) short codes. */
const Int32 SmsUsageMonitor::CATEGORY_FREE_SHORT_CODE = 1;

/** Return value from {@link #checkDestination} for standard Rate (non-premium) short codes. */
const Int32 SmsUsageMonitor::CATEGORY_STANDARD_SHORT_CODE = 2;

/** Return value from {@link #checkDestination} for possible premium short codes. */
const Int32 SmsUsageMonitor::CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE = 3;

/** Return value from {@link #checkDestination} for premium short codes. */
const Int32 SmsUsageMonitor::CATEGORY_PREMIUM_SHORT_CODE = 4;

/** Directory for per-app SMS permission XML file. */
const String SmsUsageMonitor::SMS_POLICY_FILE_DIRECTORY("/data/misc/sms");

/** Per-app SMS permission XML filename. */
const String SmsUsageMonitor::SMS_POLICY_FILE_NAME("premium_sms_policy.xml");

/** XML tag for root element. */
const String SmsUsageMonitor::TAG_SHORTCODES("shortcodes");

/** XML tag for short code patterns for a specific country. */
const String SmsUsageMonitor::TAG_SHORTCODE("shortcode");

/** XML attribute for the country code. */
const String SmsUsageMonitor::ATTR_COUNTRY("country");

/** XML attribute for the short code regex pattern. */
const String SmsUsageMonitor::ATTR_PATTERN("pattern");

/** XML attribute for the premium short code regex pattern. */
const String SmsUsageMonitor::ATTR_PREMIUM("premium");

/** XML attribute for the free short code regex pattern. */
const String SmsUsageMonitor::ATTR_FREE("free");

/** XML attribute for the standard rate short code regex pattern. */
const String SmsUsageMonitor::ATTR_STANDARD("standard");

/** XML tag for root element of premium SMS permissions. */
const String SmsUsageMonitor::TAG_SMS_POLICY_BODY("premium-sms-policy");

/** XML tag for a package. */
const String SmsUsageMonitor::TAG_PACKAGE("package");

/** XML attribute for the package name. */
const String SmsUsageMonitor::ATTR_PACKAGE_NAME("name");

/** XML attribute for the package's premium SMS Permission (integer type). */
const String SmsUsageMonitor::ATTR_PACKAGE_SMS_POLICY("sms-policy");


/**
 * SMS short code regex pattern matcher for a specific country.
 */
SmsUsageMonitor::ShortCodePatternMatcher::ShortCodePatternMatcher(
    /* [in] */ const String& shortCodeRegex,
    /* [in] */ const String& premiumShortCodeRegex,
    /* [in] */ const String& freeShortCodeRegex,
    /* [in] */ const String& standardShortCodeRegex)
{
    assert(0 && "TODO");
//    mShortCodePattern = (shortCodeRegex != NULL ? Pattern->Compile(shortCodeRegex) : NULL);
//    mPremiumShortCodePattern = (premiumShortCodeRegex != NULL ?
//            Pattern->Compile(premiumShortCodeRegex) : NULL);
//    mFreeShortCodePattern = (freeShortCodeRegex != NULL ?
//            Pattern->Compile(freeShortCodeRegex) : NULL);
//    mStandardShortCodePattern = (standardShortCodeRegex != NULL ?
//            Pattern->Compile(standardShortCodeRegex) : NULL);
}

Int32 SmsUsageMonitor::ShortCodePatternMatcher::GetNumberCategory(
    /* [in] */ const String& phoneNumber)
{
    assert(0 && "TODO");
//    if (mFreeShortCodePattern != NULL && mFreeShortCodePattern->Matcher(phoneNumber)
//            .Matches()) {
//        return CATEGORY_FREE_SHORT_CODE;
//    }
//    if (mStandardShortCodePattern != NULL && mStandardShortCodePattern->Matcher(phoneNumber)
//            .Matches()) {
//        return CATEGORY_STANDARD_SHORT_CODE;
//    }
//    if (mPremiumShortCodePattern != NULL && mPremiumShortCodePattern->Matcher(phoneNumber)
//            .Matches()) {
//        return CATEGORY_PREMIUM_SHORT_CODE;
//    }
//    if (mShortCodePattern != NULL && mShortCodePattern->Matcher(phoneNumber).Matches()) {
//        return CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE;
//    }
    return CATEGORY_NOT_SHORT_CODE;
}

/**
 * Observe the secure setting for enable flag
 */
SmsUsageMonitor::SettingsObserver::SettingsObserver(
    /* [in] */ IContext* context,
    /* [in] */ IAtomicBoolean* enabled)
{
    assert(0 && "TODO Implement constructor and call ContentObserver::constructor(IHandler*)");
//    mContext = context;
//    mEnabled = enabled;
//    OnChange(FALSE);
}

//@Override
ECode SmsUsageMonitor::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    assert(0 && "TODO");
//    mEnabled->Set(Settings::Global::>GetInt(mContext->GetContentResolver(),
//            Settings::Global::SMS_SHORT_CODE_CONFIRMATION, 1) != 0);
}

SmsUsageMonitor::SettingsObserverHandler::SettingsObserverHandler(
    /* [in] */ IContext* context,
    /* [in] */ IAtomicBoolean* enabled)
{
    assert(0 && "TODO");
//    ContentResolver resolver = context->GetContentResolver();
//    ContentObserver globalObserver = new SettingsObserver(context, enabled);
//    globalObserver->constructor(this); // TODO memory leak, zhaohui
//    resolver->RegisterContentObserver(Settings::Global::>GetUrifor(
//            Settings::Global::SMS_SHORT_CODE_CONFIRMATION), FALSE, globalObserver);
}

/**
 * Observe the global setting for sms limits
 */
SmsUsageMonitor::SmsLimitObserver::SmsLimitObserver(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO Implement constructor and call ContentObserver::constructor(IHandler*)");
//    mContext = context;
//    OnChange(FALSE);
}

//@Override
ECode SmsUsageMonitor::SmsLimitObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return OnChange(selfChange, NULL);
}

//@Override
ECode SmsUsageMonitor::SmsLimitObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    assert(0 && "TODO");
//    ContentResolver resolver = mContext->GetContentResolver();
//    mMaxAllowed = Settings.Secure->GetInt(resolver,
//            Settings::Global::SMS_OUTGOING_CHECK_MAX_COUNT,
//            DEFAULT_SMS_MAX_COUNT);
//
//    mCheckPeriod = Settings.Secure->GetInt(resolver,
//            Settings::Global::SMS_OUTGOING_CHECK_INTERVAL_MS,
//            DEFAULT_SMS_CHECK_PERIOD);
}

SmsUsageMonitor::SmsLimitObserverHandler::SmsLimitObserverHandler(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO");
//    ContentResolver resolver = context->GetContentResolver();
//
//    ContentObserver globalObserver = new SmsLimitObserver(context);
//    globalObserver->constructor(this); // TODO memory leak, zhaohui
//    resolver->RegisterContentObserver(Settings.Secure->GetUrifor(
//            Settings::Global::SMS_OUTGOING_CHECK_MAX_COUNT), FALSE, globalObserver);
//    resolver->RegisterContentObserver(Settings.Secure->GetUrifor(
//            Settings::Global::SMS_OUTGOING_CHECK_INTERVAL_MS), FALSE, globalObserver);
}


SmsUsageMonitor::SmsUsageMonitor()
{
    assert(0 && "TODO");
//    mSmsStamp = new HashMap<String, ArrayList<Long>>();
//    mCheckEnabled = new AtomicBoolean(TRUE);
//    mPatternFile = new File(SHORT_CODE_PATH);
//    mPatternFileLastModified = 0;
//    mPremiumSmsPolicy = new HashMap<String, Integer>();
}

/**
 * Create SMS usage monitor.
 * @param context the context to use to load resources and get TelephonyManager service
 */
ECode SmsUsageMonitor::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    mSmsLimitObserverHandler = new SmsLimitObserverHandler(mContext);

    mSettingsObserverHandler = new SettingsObserverHandler(mContext, mCheckEnabled);

    LoadPremiumSmsPolicyDb();
    return NOERROR;
}

//TODO
/** @hide */
ECode SmsUsageMonitor::MergeShortCodeCategories(
    /* [in] */ Int32 type1,
    /* [in] */ Int32 type2,
    /* [in] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (type1 > type2) *result = type1;
    else *result = type2;
    return NOERROR;
}

/**
 * Return a pattern matcher object for the specified country.
 * @param country the country to search for
 * @return a {@link ShortCodePatternMatcher} for the specified country, or NULL if not found
 */
AutoPtr<SmsUsageMonitor::ShortCodePatternMatcher> SmsUsageMonitor::GetPatternMatcherFromFile(
    /* [in] */ const String& country)
{
    assert(0 && "TODO");
//    FileReader patternReader = NULL;
//    XmlPullParser parser = NULL;
//    try {
//        patternReader = new FileReader(mPatternFile);
//        parser = Xml->NewPullParser();
//        parser->SetInput(patternReader);
//        return GetPatternMatcherFromXmlParser(parser, country);
//    } Catch (FileNotFoundException e) {
//        Rlog->E(TAG, "Short Code Pattern File not found");
//    } Catch (XmlPullParserException e) {
//        Rlog->E(TAG, "XML parser exception reading short code pattern file", e);
//    } finally {
//        mPatternFileLastModified = mPatternFile->LastModified();
//        if (patternReader != NULL) {
//            try {
//                patternReader->Close();
//            } Catch (IOException e) {}
//        }
//    }
    return NULL;
}

AutoPtr<SmsUsageMonitor::ShortCodePatternMatcher> SmsUsageMonitor::GetPatternMatcherFromResource(
    /* [in] */ const String& country)
{
    assert(0 && "TODO");
//    Int32 id = R.xml.sms_short_codes;
//    XmlResourceParser parser = NULL;
//    try {
//        parser = mContext->GetResources()->GetXml(id);
//        return GetPatternMatcherFromXmlParser(parser, country);
//    } finally {
//        if (parser != NULL) parser->Close();
//    }
}

AutoPtr<SmsUsageMonitor::ShortCodePatternMatcher> SmsUsageMonitor::GetPatternMatcherFromXmlParser(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& country)
{
    assert(0 && "TODO");
//    try {
//        XmlUtils->BeginDocument(parser, TAG_SHORTCODES);
//
//        While (TRUE) {
//            XmlUtils->NextElement(parser);
//            String element = parser->GetName();
//            if (element == NULL) {
//                Rlog->E(TAG, "Parsing pattern data found NULL");
//                break;
//            }
//
//            if (element->Equals(TAG_SHORTCODE)) {
//                String currentCountry = parser->GetAttributeValue(NULL, ATTR_COUNTRY);
//                if (VDBG) Rlog->D(TAG, "Found country " + currentCountry);
//                if (country->Equals(currentCountry)) {
//                    String pattern = parser->GetAttributeValue(NULL, ATTR_PATTERN);
//                    String premium = parser->GetAttributeValue(NULL, ATTR_PREMIUM);
//                    String free = parser->GetAttributeValue(NULL, ATTR_FREE);
//                    String standard = parser->GetAttributeValue(NULL, ATTR_STANDARD);
//                    return new ShortCodePatternMatcher(pattern, premium, free, standard);
//                }
//            } else {
//                Rlog->E(TAG, "Error: skipping unknown XML tag " + element);
//            }
//        }
//    } Catch (XmlPullParserException e) {
//        Rlog->E(TAG, "XML parser exception reading short code patterns", e);
//    } Catch (IOException e) {
//        Rlog->E(TAG, "I/O exception reading short code patterns", e);
//    }
//    if (DBG) Rlog->D(TAG, "Country (" + country + ") not found");
    return NULL;    // country not found
}

/** Clear the SMS application list for disposal. */
void SmsUsageMonitor::Dispose()
{
    assert(0 && "TODO");
//    mSmsStamp->Clear();
}

/**
 * Check to see if an application is allowed to send new SMS messages, and confirm with
 * user if the send limit was reached or if a non-system app is potentially sending to a
 * premium SMS short code or number.
 *
 * @param appName the package name of the app requesting to send an SMS
 * @param smsWaiting the number of new messages desired to send
 * @return TRUE if application is allowed to send the requested number
 *  of new sms messages
 */
ECode SmsUsageMonitor::Check(
    /* [in] */ const String& appName,
    /* [in] */ Int32 smsWaiting,
    /* [in] */ Boolean *result)
{
    assert(0 && "TODO");
//    {
//        AutoLock syncLock(mSmsStamp);
//        RemoveExpiredTimestamps();
//
//        ArrayList<Long> sentList = mSmsStamp->Get(appName);
//        if (sentList == NULL) {
//            sentList = new ArrayList<Long>();
//            mSmsStamp->Put(appName, sentList);
//        }
//
//        return IsUnderLimit(sentList, smsWaiting);
//    }
    return NOERROR;
}

/**
 * Check if the destination is a possible premium short code.
 * NOTE: the caller is expected to strip non-digits from the destination number with
 * {@link PhoneNumberUtils#extractNetworkPortion} before calling this method.
 * This happens in {@link SMSDispatcher#sendRawPdu} so that we use the same phone number
 * for testing and in the user confirmation dialog if the user needs to confirm the number.
 * This makes it difficult for malware to fool the user or the short code pattern matcher
 * by using non-ASCII characters to make the number appear to be different from the real
 * destination phone number.
 *
 * @param destAddress the destination address to test for possible short code
 * @return {@link #CATEGORY_NOT_SHORT_CODE}, {@link #CATEGORY_FREE_SHORT_CODE},
 *  {@link #CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE}, or {@link #CATEGORY_PREMIUM_SHORT_CODE}.
 */
ECode SmsUsageMonitor::CheckDestination(
    /* [in] */ const String& destAddress,
    /* [in] */ const String& countryIso,
    /* [in] */ Int32 *result)
{
    assert(0 && "TODO");
//    {
//        AutoLock syncLock(mSettingsObserverHandler);
//        // always allow emergency numbers
//        if (PhoneNumberUtils->IsEmergencyNumber(destAddress, countryIso)) {
//            if (DBG) Rlog->D(TAG, "isEmergencyNumber");
//            return CATEGORY_NOT_SHORT_CODE;
//        }
//        // always allow if the feature is disabled
//        if (!mCheckEnabled->Get()) {
//            if (DBG) Rlog->E(TAG, "check disabled");
//            return CATEGORY_NOT_SHORT_CODE;
//        }
//
//        if (countryIso != NULL) {
//            if (mCurrentCountry == NULL || !countryIso->Equals(mCurrentCountry) ||
//                    mPatternFile->LastModified() != mPatternFileLastModified) {
//                mCurrentPatternMatcher = NULL;
//                if (mPatternFile->Exists()) {
//                    if (DBG) Rlog->D(TAG, "Loading SMS Short Code patterns from file");
//                    mCurrentPatternMatcher = GetPatternMatcherFromFile(countryIso);
//                }
//
//                // if matcher not defined in file, fall back to xml
//                if (mCurrentPatternMatcher == NULL) {
//                    if (DBG) Rlog->D(TAG, "Loading SMS Short Code patterns from resource");
//                    mCurrentPatternMatcher = GetPatternMatcherFromResource(countryIso);
//                }
//                mCurrentCountry = countryIso;
//            }
//        }
//
//        if (mCurrentPatternMatcher != NULL) {
//            return mCurrentPatternMatcher->GetNumberCategory(destAddress);
//        } else {
//            // Generic rule: numbers of 5 digits or less are considered potential short codes
//            Rlog->E(TAG, "No patterns for \"" + countryIso + "\": using generic short code rule");
//            if (destAddress->Length() <= 5) {
//                return CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE;
//            } else {
//                return CATEGORY_NOT_SHORT_CODE;
//            }
//        }
//    }
    return NOERROR;
}

/**
 * Load the premium SMS policy from an XML file.
 * Based on code from NotificationManagerService.
 */
void SmsUsageMonitor::LoadPremiumSmsPolicyDb()
{
    assert(0 && "TODO");
//    {
//        AutoLock syncLock(mPremiumSmsPolicy);
//        if (mPolicyFile == NULL) {
//            File dir = new File(SMS_POLICY_FILE_DIRECTORY);
//            mPolicyFile = new AtomicFile(new File(dir, SMS_POLICY_FILE_NAME));
//
//            mPremiumSmsPolicy->Clear();
//
//            FileInputStream infile = NULL;
//            try {
//                infile = mPolicyFile->OpenRead();
//                final XmlPullParser parser = Xml->NewPullParser();
//                parser->SetInput(infile, NULL);
//
//                XmlUtils->BeginDocument(parser, TAG_SMS_POLICY_BODY);
//
//                While (TRUE) {
//                    XmlUtils->NextElement(parser);
//
//                    String element = parser->GetName();
//                    if (element == NULL) break;
//
//                    if (element->Equals(TAG_PACKAGE)) {
//                        String packageName = parser->GetAttributeValue(NULL, ATTR_PACKAGE_NAME);
//                        String policy = parser->GetAttributeValue(NULL, ATTR_PACKAGE_SMS_POLICY);
//                        if (packageName == NULL) {
//                            Rlog->E(TAG, "Error: missing package name attribute");
//                        } else if (policy == NULL) {
//                            Rlog->E(TAG, "Error: missing package policy attribute");
//                        } else try {
//                            mPremiumSmsPolicy->Put(packageName, Integer->ParseInt(policy));
//                        } Catch (NumberFormatException e) {
//                            Rlog->E(TAG, "Error: non-numeric policy type " + policy);
//                        }
//                    } else {
//                        Rlog->E(TAG, "Error: skipping unknown XML tag " + element);
//                    }
//                }
//            } Catch (FileNotFoundException e) {
//                // No data yet
//            } Catch (IOException e) {
//                Rlog->E(TAG, "Unable to read premium SMS policy database", e);
//            } Catch (NumberFormatException e) {
//                Rlog->E(TAG, "Unable to parse premium SMS policy database", e);
//            } Catch (XmlPullParserException e) {
//                Rlog->E(TAG, "Unable to parse premium SMS policy database", e);
//            } finally {
//                if (infile != NULL) {
//                    try {
//                        infile->Close();
//                    } Catch (IOException ignored) {
//                    }
//                }
//            }
//        }
//    }
}

/**
 * Persist the premium SMS policy to an XML file.
 * Based on code from NotificationManagerService.
 */
void SmsUsageMonitor::WritePremiumSmsPolicyDb()
{
    assert(0 && "TODO");
//    {
//        AutoLock syncLock(mPremiumSmsPolicy);
//        FileOutputStream outfile = NULL;
//        try {
//            outfile = mPolicyFile->StartWrite();
//
//            XmlSerializer out = new FastXmlSerializer();
//            out->SetOutput(outfile, "utf-8");
//
//            out->StartDocument(NULL, TRUE);
//
//            out->StartTag(NULL, TAG_SMS_POLICY_BODY);
//
//            For (Map.Entry<String, Integer> policy : mPremiumSmsPolicy->EntrySet()) {
//                out->StartTag(NULL, TAG_PACKAGE);
//                out->Attribute(NULL, ATTR_PACKAGE_NAME, policy->GetKey());
//                out->Attribute(NULL, ATTR_PACKAGE_SMS_POLICY, policy->GetValue()->ToString());
//                out->EndTag(NULL, TAG_PACKAGE);
//            }
//
//            out->EndTag(NULL, TAG_SMS_POLICY_BODY);
//            out->EndDocument();
//
//            mPolicyFile->FinishWrite(outfile);
//        } Catch (IOException e) {
//            Rlog->E(TAG, "Unable to write premium SMS policy database", e);
//            if (outfile != NULL) {
//                mPolicyFile->FailWrite(outfile);
//            }
//        }
//    }
}

/**
 * Returns the premium SMS permission for the specified package. if the package has never
 * been seen before, the default {@link #PREMIUM_SMS_PERMISSION_ASK_USER}
 * will be returned.
 * @param packageName the name of the package to query permission
 * @return one of {@link #PREMIUM_SMS_PERMISSION_UNKNOWN},
 *  {@link #PREMIUM_SMS_PERMISSION_ASK_USER},
 *  {@link #PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
 *  {@link #PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
 * @throws SecurityException if the caller is not a system process
 */
ECode SmsUsageMonitor::GetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 * result)
{
    assert(0 && "TODO");
//    CheckCallerIsSystemOrSameApp(packageName);
//    {
//        AutoLock syncLock(mPremiumSmsPolicy);
//        Integer policy = mPremiumSmsPolicy->Get(packageName);
//        if (policy == NULL) {
//            return PREMIUM_SMS_PERMISSION_UNKNOWN;
//        } else {
//            return policy;
//        }
//    }
    return NOERROR;
}

/**
 * Sets the premium SMS permission for the specified package and save the value asynchronously
 * to persistent storage.
 * @param packageName the name of the package to set permission
 * @param permission one of {@link #PREMIUM_SMS_PERMISSION_ASK_USER},
 *  {@link #PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
 *  {@link #PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
 * @throws SecurityException if the caller is not a system process
 */
ECode SmsUsageMonitor::SetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 permission)
{
    assert(0 && "TODO");
//    CheckCallerIsSystemOrPhoneApp();
//    if (permission < PREMIUM_SMS_PERMISSION_ASK_USER
//            || permission > PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW) {
//        throw new IllegalArgumentException("invalid SMS permission type " + permission);
//    }
//    {    AutoLock syncLock(mPremiumSmsPolicy);
//        mPremiumSmsPolicy->Put(packageName, permission);
//    }
//    // write policy file in the background
//    new Thread(new Runnable() {
//        //@Override
//        CARAPI Run() {
//            WritePremiumSmsPolicyDb();
//        }
//    }).Start();
}

void SmsUsageMonitor::CheckCallerIsSystemOrSameApp(
    /* [in] */ const String& pkg)
{
    assert(0 && "TODO");
//    Int32 uid = Binder->GetCallingUid();
//    if (UserHandle->GetAppId(uid) == Process.SYSTEM_UID || uid == 0) {
//        return;
//    }
//    try {
//        ApplicationInfo ai = AppGlobals->GetPackageManager()->GetApplicationInfo(
//                pkg, 0, UserHandle->GetCallingUserId());
//        if (!UserHandle->IsSameApp(ai.uid, uid)) {
//            throw new SecurityException("Calling uid " + uid + " gave package"
//                    + pkg + " which is owned by uid " + ai.uid);
//        }
//    } Catch (RemoteException re) {
//        throw new SecurityException("Unknown package " + pkg + "\n" + re);
//    }
}

void SmsUsageMonitor::CheckCallerIsSystemOrPhoneApp()
{
    assert(0 && "TODO");
//    Int32 uid = Binder->GetCallingUid();
//    Int32 appId = UserHandle->GetAppId(uid);
//    if (appId == Process.SYSTEM_UID || appId == Process.PHONE_UID || uid == 0) {
//        return;
//    }
//    throw new SecurityException("Disallowed call for uid " + uid);
}

/**
 * Remove keys containing only old timestamps. This can happen if an SMS app is used
 * to send messages and then uninstalled.
 */
void SmsUsageMonitor::RemoveExpiredTimestamps()
{
    assert(0 && "TODO");
//    Int64 beginCheckPeriod = System->CurrentTimeMillis() - mCheckPeriod;
//
//    {    AutoLock syncLock(mSmsStamp);
//        Iterator<Map.Entry<String, ArrayList<Long>>> iter = mSmsStamp->EntrySet()->Iterator();
//        While (iter->HasNext()) {
//            Map.Entry<String, ArrayList<Long>> entry = iter->Next();
//            ArrayList<Long> oldList = entry->GetValue();
//            if (oldList->IsEmpty() || oldList->Get(oldList->Size() - 1) < beginCheckPeriod) {
//                iter->Remove();
//            }
//        }
//    }
}

Boolean SmsUsageMonitor::IsUnderLimit(
    /* [in] */ IArrayList* sent,
    /* [in] */ Int32 smsWaiting)
{
    assert(0 && "TODO");
//    Long ct = System->CurrentTimeMillis();
//    Int64 beginCheckPeriod = ct - mCheckPeriod;
//
//    if (VDBG) Log("SMS send size=" + sent->Size() + " time=" + ct);
//
//    While (!sent->IsEmpty() && sent->Get(0) < beginCheckPeriod) {
//        sent->Remove(0);
//    }
//
//    if ((sent->Size() + smsWaiting) <= mMaxAllowed) {
//        For (Int32 i = 0; i < smsWaiting; i++ ) {
//            sent->Add(ct);
//        }
//        return TRUE;
//    }
//    return FALSE;
}

void SmsUsageMonitor::Log(
    /* [in] */ const String& msg)
{
    assert(0 && "TODO");
//    Rlog->D(TAG, msg);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
