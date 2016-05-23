/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.internal.telephony;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IXmlUtils;

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

/**
 * Implement the per-application based SMS control, which limits the number of
 * SMS/MMS messages an app can send in the checking period.
 *
 * This code was formerly part of {@link SMSDispatcher}, and has been moved
 * into a separate class to support instantiation of multiple SMSDispatchers on
 * dual-mode devices that require support for both 3GPP and 3GPP2 format messages.
 */
public class SmsUsageMonitor {
    private static const String TAG = "SmsUsageMonitor";
    private static const Boolean DBG = TRUE; // intentionally enable DBG
    private static const Boolean VDBG = FALSE;

    private static const String SHORT_CODE_PATH = "/data/misc/sms/codes";

    /** Default checking period for SMS sent without user permission. */
    private static const Int32 DEFAULT_SMS_CHECK_PERIOD = 15 * 60 * 1000;      // 15 minutes

    /** Default number of SMS sent in checking period without user permission. */
    private static const Int32 DEFAULT_SMS_MAX_COUNT = 30;

    /** Return value from {@link #checkDestination} for regular phone numbers. */
    static const Int32 CATEGORY_NOT_SHORT_CODE = 0;

    /** Return value from {@link #checkDestination} for Free (no cost) short codes. */
    static const Int32 CATEGORY_FREE_SHORT_CODE = 1;

    /** Return value from {@link #checkDestination} for standard Rate (non-premium) short codes. */
    static const Int32 CATEGORY_STANDARD_SHORT_CODE = 2;

    /** Return value from {@link #checkDestination} for possible premium short codes. */
    static const Int32 CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE = 3;

    /** Return value from {@link #checkDestination} for premium short codes. */
    static const Int32 CATEGORY_PREMIUM_SHORT_CODE = 4;

    /** @hide */
    public static Int32 MergeShortCodeCategories(Int32 type1, Int32 type2) {
        If (type1 > type2) return type1;
        return type2;
    }

    /** Premium SMS permission for a new Package (ask user when first premium SMS sent). */
    public static const Int32 PREMIUM_SMS_PERMISSION_UNKNOWN = 0;

    /** Default premium SMS Permission (ask user for each premium SMS sent). */
    public static const Int32 PREMIUM_SMS_PERMISSION_ASK_USER = 1;

    /** Premium SMS permission when the owner has denied the app from sending premium SMS. */
    public static const Int32 PREMIUM_SMS_PERMISSION_NEVER_ALLOW = 2;

    /** Premium SMS permission when the owner has allowed the app to send premium SMS. */
    public static const Int32 PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW = 3;

    private Int32 mCheckPeriod;
    private Int32 mMaxAllowed;

    private final HashMap<String, ArrayList<Long>> mSmsStamp =
            new HashMap<String, ArrayList<Long>>();

    /** Context for retrieving regexes from XML resource. */
    private final Context mContext;

    /** Country code for the cached short code pattern matcher. */
    private String mCurrentCountry;

    /** Cached short code pattern matcher for {@link #mCurrentCountry}. */
    private ShortCodePatternMatcher mCurrentPatternMatcher;

    /** Notice when the enabled setting changes - can be changed through gservices */
    private final AtomicBoolean mCheckEnabled = new AtomicBoolean(TRUE);

    /** Handler for responding to content observer updates. */
    private final SettingsObserverHandler mSettingsObserverHandler;

    /** Handler for responding to content observer updates sms limits. */
    private final SmsLimitObserverHandler mSmsLimitObserverHandler;

    /** File holding the patterns */
    private final File mPatternFile = new File(SHORT_CODE_PATH);

    /** Last modified time for pattern file */
    private Int64 mPatternFileLastModified = 0;

    /** Directory for per-app SMS permission XML file. */
    private static const String SMS_POLICY_FILE_DIRECTORY = "/data/misc/sms";

    /** Per-app SMS permission XML filename. */
    private static const String SMS_POLICY_FILE_NAME = "premium_sms_policy.xml";

    /** XML tag for root element. */
    private static const String TAG_SHORTCODES = "shortcodes";

    /** XML tag for short code patterns for a specific country. */
    private static const String TAG_SHORTCODE = "shortcode";

    /** XML attribute for the country code. */
    private static const String ATTR_COUNTRY = "country";

    /** XML attribute for the short code regex pattern. */
    private static const String ATTR_PATTERN = "pattern";

    /** XML attribute for the premium short code regex pattern. */
    private static const String ATTR_PREMIUM = "premium";

    /** XML attribute for the free short code regex pattern. */
    private static const String ATTR_FREE = "free";

    /** XML attribute for the standard rate short code regex pattern. */
    private static const String ATTR_STANDARD = "standard";

    /** Stored copy of premium SMS package permissions. */
    private AtomicFile mPolicyFile;

    /** Loaded copy of premium SMS package permissions. */
    private final HashMap<String, Integer> mPremiumSmsPolicy = new HashMap<String, Integer>();

    /** XML tag for root element of premium SMS permissions. */
    private static const String TAG_SMS_POLICY_BODY = "premium-sms-policy";

    /** XML tag for a package. */
    private static const String TAG_PACKAGE = "package";

    /** XML attribute for the package name. */
    private static const String ATTR_PACKAGE_NAME = "name";

    /** XML attribute for the package's premium SMS Permission (integer type). */
    private static const String ATTR_PACKAGE_SMS_POLICY = "sms-policy";

    /**
     * SMS short code regex pattern matcher for a specific country.
     */
    private static const class ShortCodePatternMatcher {
        private final Pattern mShortCodePattern;
        private final Pattern mPremiumShortCodePattern;
        private final Pattern mFreeShortCodePattern;
        private final Pattern mStandardShortCodePattern;

        ShortCodePatternMatcher(String shortCodeRegex, String premiumShortCodeRegex,
                String freeShortCodeRegex, String standardShortCodeRegex) {
            mShortCodePattern = (shortCodeRegex != NULL ? Pattern->Compile(shortCodeRegex) : NULL);
            mPremiumShortCodePattern = (premiumShortCodeRegex != NULL ?
                    Pattern->Compile(premiumShortCodeRegex) : NULL);
            mFreeShortCodePattern = (freeShortCodeRegex != NULL ?
                    Pattern->Compile(freeShortCodeRegex) : NULL);
            mStandardShortCodePattern = (standardShortCodeRegex != NULL ?
                    Pattern->Compile(standardShortCodeRegex) : NULL);
        }

        Int32 GetNumberCategory(String phoneNumber) {
            If (mFreeShortCodePattern != NULL && mFreeShortCodePattern->Matcher(phoneNumber)
                    .Matches()) {
                return CATEGORY_FREE_SHORT_CODE;
            }
            If (mStandardShortCodePattern != NULL && mStandardShortCodePattern->Matcher(phoneNumber)
                    .Matches()) {
                return CATEGORY_STANDARD_SHORT_CODE;
            }
            If (mPremiumShortCodePattern != NULL && mPremiumShortCodePattern->Matcher(phoneNumber)
                    .Matches()) {
                return CATEGORY_PREMIUM_SHORT_CODE;
            }
            If (mShortCodePattern != NULL && mShortCodePattern->Matcher(phoneNumber).Matches()) {
                return CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE;
            }
            return CATEGORY_NOT_SHORT_CODE;
        }
    }

    /**
     * Observe the secure setting for enable flag
     */
    private static class SettingsObserver extends ContentObserver {
        private final Context mContext;
        private final AtomicBoolean mEnabled;

        SettingsObserver(Handler handler, Context context, AtomicBoolean enabled) {
            Super(handler);
            mContext = context;
            mEnabled = enabled;
            OnChange(FALSE);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            mEnabled->Set(Settings::Global::>GetInt(mContext->GetContentResolver(),
                    Settings::Global::SMS_SHORT_CODE_CONFIRMATION, 1) != 0);
        }
    }

    private static class SettingsObserverHandler extends Handler {
        SettingsObserverHandler(Context context, AtomicBoolean enabled) {
            ContentResolver resolver = context->GetContentResolver();
            ContentObserver globalObserver = new SettingsObserver(this, context, enabled);
            resolver->RegisterContentObserver(Settings::Global::>GetUriFor(
                    Settings::Global::SMS_SHORT_CODE_CONFIRMATION), FALSE, globalObserver);
        }
    }

    /**
     * Observe the global setting for sms limits
     */
    private class SmsLimitObserver extends ContentObserver {
        private final Context mContext;

        SmsLimitObserver(Handler handler, Context context) {
            Super(handler);
            mContext = context;
            OnChange(FALSE);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            OnChange(selfChange, NULL);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            ContentResolver resolver = mContext->GetContentResolver();
            mMaxAllowed = Settings.Secure->GetInt(resolver,
                    Settings::Global::SMS_OUTGOING_CHECK_MAX_COUNT,
                    DEFAULT_SMS_MAX_COUNT);

            mCheckPeriod = Settings.Secure->GetInt(resolver,
                    Settings::Global::SMS_OUTGOING_CHECK_INTERVAL_MS,
                    DEFAULT_SMS_CHECK_PERIOD);
        }
    }

    private class SmsLimitObserverHandler extends Handler {
        SmsLimitObserverHandler(Context context) {
            ContentResolver resolver = context->GetContentResolver();

            ContentObserver globalObserver = new SmsLimitObserver(this, context);

            resolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                    Settings::Global::SMS_OUTGOING_CHECK_MAX_COUNT), FALSE, globalObserver);
            resolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                    Settings::Global::SMS_OUTGOING_CHECK_INTERVAL_MS), FALSE, globalObserver);
        }
    }

    /**
     * Create SMS usage monitor.
     * @param context the context to use to load resources and get TelephonyManager service
     */
    public SmsUsageMonitor(Context context) {
        mContext = context;

        mSmsLimitObserverHandler = new SmsLimitObserverHandler(mContext);

        mSettingsObserverHandler = new SettingsObserverHandler(mContext, mCheckEnabled);

        LoadPremiumSmsPolicyDb();
    }

    /**
     * Return a pattern matcher object for the specified country.
     * @param country the country to search for
     * @return a {@link ShortCodePatternMatcher} for the specified country, or NULL if not found
     */
    private ShortCodePatternMatcher GetPatternMatcherFromFile(String country) {
        FileReader patternReader = NULL;
        XmlPullParser parser = NULL;
        try {
            patternReader = new FileReader(mPatternFile);
            parser = Xml->NewPullParser();
            parser->SetInput(patternReader);
            return GetPatternMatcherFromXmlParser(parser, country);
        } Catch (FileNotFoundException e) {
            Rlog->E(TAG, "Short Code Pattern File not found");
        } Catch (XmlPullParserException e) {
            Rlog->E(TAG, "XML parser exception reading short code pattern file", e);
        } finally {
            mPatternFileLastModified = mPatternFile->LastModified();
            If (patternReader != NULL) {
                try {
                    patternReader->Close();
                } Catch (IOException e) {}
            }
        }
        return NULL;
    }

    private ShortCodePatternMatcher GetPatternMatcherFromResource(String country) {
        Int32 id = R.xml.sms_short_codes;
        XmlResourceParser parser = NULL;
        try {
            parser = mContext->GetResources()->GetXml(id);
            return GetPatternMatcherFromXmlParser(parser, country);
        } finally {
            If (parser != NULL) parser->Close();
        }
    }

    private ShortCodePatternMatcher GetPatternMatcherFromXmlParser(XmlPullParser parser,
            String country) {
        try {
            XmlUtils->BeginDocument(parser, TAG_SHORTCODES);

            While (TRUE) {
                XmlUtils->NextElement(parser);
                String element = parser->GetName();
                If (element == NULL) {
                    Rlog->E(TAG, "Parsing pattern data found NULL");
                    break;
                }

                If (element->Equals(TAG_SHORTCODE)) {
                    String currentCountry = parser->GetAttributeValue(NULL, ATTR_COUNTRY);
                    If (VDBG) Rlog->D(TAG, "Found country " + currentCountry);
                    If (country->Equals(currentCountry)) {
                        String pattern = parser->GetAttributeValue(NULL, ATTR_PATTERN);
                        String premium = parser->GetAttributeValue(NULL, ATTR_PREMIUM);
                        String free = parser->GetAttributeValue(NULL, ATTR_FREE);
                        String standard = parser->GetAttributeValue(NULL, ATTR_STANDARD);
                        return new ShortCodePatternMatcher(pattern, premium, free, standard);
                    }
                } else {
                    Rlog->E(TAG, "Error: skipping unknown XML tag " + element);
                }
            }
        } Catch (XmlPullParserException e) {
            Rlog->E(TAG, "XML parser exception reading short code patterns", e);
        } Catch (IOException e) {
            Rlog->E(TAG, "I/O exception reading short code patterns", e);
        }
        If (DBG) Rlog->D(TAG, "Country (" + country + ") not found");
        return NULL;    // country not found
    }

    /** Clear the SMS application list for disposal. */
    void Dispose() {
        mSmsStamp->Clear();
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
    public Boolean Check(String appName, Int32 smsWaiting) {
        {    AutoLock syncLock(mSmsStamp);
            RemoveExpiredTimestamps();

            ArrayList<Long> sentList = mSmsStamp->Get(appName);
            If (sentList == NULL) {
                sentList = new ArrayList<Long>();
                mSmsStamp->Put(appName, sentList);
            }

            return IsUnderLimit(sentList, smsWaiting);
        }
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
    public Int32 CheckDestination(String destAddress, String countryIso) {
        {    AutoLock syncLock(mSettingsObserverHandler);
            // always allow emergency numbers
            If (PhoneNumberUtils->IsEmergencyNumber(destAddress, countryIso)) {
                If (DBG) Rlog->D(TAG, "isEmergencyNumber");
                return CATEGORY_NOT_SHORT_CODE;
            }
            // always allow if the feature is disabled
            If (!mCheckEnabled->Get()) {
                If (DBG) Rlog->E(TAG, "check disabled");
                return CATEGORY_NOT_SHORT_CODE;
            }

            If (countryIso != NULL) {
                If (mCurrentCountry == NULL || !countryIso->Equals(mCurrentCountry) ||
                        mPatternFile->LastModified() != mPatternFileLastModified) {
                    mCurrentPatternMatcher = NULL;
                    If (mPatternFile->Exists()) {
                        If (DBG) Rlog->D(TAG, "Loading SMS Short Code patterns from file");
                        mCurrentPatternMatcher = GetPatternMatcherFromFile(countryIso);
                    }

                    // if matcher not defined in file, fall back to xml
                    If (mCurrentPatternMatcher == NULL) {
                        If (DBG) Rlog->D(TAG, "Loading SMS Short Code patterns from resource");
                        mCurrentPatternMatcher = GetPatternMatcherFromResource(countryIso);
                    }
                    mCurrentCountry = countryIso;
                }
            }

            If (mCurrentPatternMatcher != NULL) {
                return mCurrentPatternMatcher->GetNumberCategory(destAddress);
            } else {
                // Generic rule: numbers of 5 digits or less are considered potential short codes
                Rlog->E(TAG, "No patterns for \"" + countryIso + "\": using generic short code rule");
                If (destAddress->Length() <= 5) {
                    return CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE;
                } else {
                    return CATEGORY_NOT_SHORT_CODE;
                }
            }
        }
    }

    /**
     * Load the premium SMS policy from an XML file.
     * Based on code from NotificationManagerService.
     */
    private void LoadPremiumSmsPolicyDb() {
        {    AutoLock syncLock(mPremiumSmsPolicy);
            If (mPolicyFile == NULL) {
                File dir = new File(SMS_POLICY_FILE_DIRECTORY);
                mPolicyFile = new AtomicFile(new File(dir, SMS_POLICY_FILE_NAME));

                mPremiumSmsPolicy->Clear();

                FileInputStream infile = NULL;
                try {
                    infile = mPolicyFile->OpenRead();
                    final XmlPullParser parser = Xml->NewPullParser();
                    parser->SetInput(infile, NULL);

                    XmlUtils->BeginDocument(parser, TAG_SMS_POLICY_BODY);

                    While (TRUE) {
                        XmlUtils->NextElement(parser);

                        String element = parser->GetName();
                        If (element == NULL) break;

                        If (element->Equals(TAG_PACKAGE)) {
                            String packageName = parser->GetAttributeValue(NULL, ATTR_PACKAGE_NAME);
                            String policy = parser->GetAttributeValue(NULL, ATTR_PACKAGE_SMS_POLICY);
                            If (packageName == NULL) {
                                Rlog->E(TAG, "Error: missing package name attribute");
                            } else If (policy == NULL) {
                                Rlog->E(TAG, "Error: missing package policy attribute");
                            } else try {
                                mPremiumSmsPolicy->Put(packageName, Integer->ParseInt(policy));
                            } Catch (NumberFormatException e) {
                                Rlog->E(TAG, "Error: non-numeric policy type " + policy);
                            }
                        } else {
                            Rlog->E(TAG, "Error: skipping unknown XML tag " + element);
                        }
                    }
                } Catch (FileNotFoundException e) {
                    // No data yet
                } Catch (IOException e) {
                    Rlog->E(TAG, "Unable to read premium SMS policy database", e);
                } Catch (NumberFormatException e) {
                    Rlog->E(TAG, "Unable to parse premium SMS policy database", e);
                } Catch (XmlPullParserException e) {
                    Rlog->E(TAG, "Unable to parse premium SMS policy database", e);
                } finally {
                    If (infile != NULL) {
                        try {
                            infile->Close();
                        } Catch (IOException ignored) {
                        }
                    }
                }
            }
        }
    }

    /**
     * Persist the premium SMS policy to an XML file.
     * Based on code from NotificationManagerService.
     */
    private void WritePremiumSmsPolicyDb() {
        {    AutoLock syncLock(mPremiumSmsPolicy);
            FileOutputStream outfile = NULL;
            try {
                outfile = mPolicyFile->StartWrite();

                XmlSerializer out = new FastXmlSerializer();
                out->SetOutput(outfile, "utf-8");

                out->StartDocument(NULL, TRUE);

                out->StartTag(NULL, TAG_SMS_POLICY_BODY);

                For (Map.Entry<String, Integer> policy : mPremiumSmsPolicy->EntrySet()) {
                    out->StartTag(NULL, TAG_PACKAGE);
                    out->Attribute(NULL, ATTR_PACKAGE_NAME, policy->GetKey());
                    out->Attribute(NULL, ATTR_PACKAGE_SMS_POLICY, policy->GetValue()->ToString());
                    out->EndTag(NULL, TAG_PACKAGE);
                }

                out->EndTag(NULL, TAG_SMS_POLICY_BODY);
                out->EndDocument();

                mPolicyFile->FinishWrite(outfile);
            } Catch (IOException e) {
                Rlog->E(TAG, "Unable to write premium SMS policy database", e);
                If (outfile != NULL) {
                    mPolicyFile->FailWrite(outfile);
                }
            }
        }
    }

    /**
     * Returns the premium SMS permission for the specified package. If the package has never
     * been seen before, the default {@link #PREMIUM_SMS_PERMISSION_ASK_USER}
     * will be returned.
     * @param packageName the name of the package to query permission
     * @return one of {@link #PREMIUM_SMS_PERMISSION_UNKNOWN},
     *  {@link #PREMIUM_SMS_PERMISSION_ASK_USER},
     *  {@link #PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
     *  {@link #PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
     * @throws SecurityException if the caller is not a system process
     */
    public Int32 GetPremiumSmsPermission(String packageName) {
        CheckCallerIsSystemOrSameApp(packageName);
        {    AutoLock syncLock(mPremiumSmsPolicy);
            Integer policy = mPremiumSmsPolicy->Get(packageName);
            If (policy == NULL) {
                return PREMIUM_SMS_PERMISSION_UNKNOWN;
            } else {
                return policy;
            }
        }
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
    CARAPI SetPremiumSmsPermission(String packageName, Int32 permission) {
        CheckCallerIsSystemOrPhoneApp();
        If (permission < PREMIUM_SMS_PERMISSION_ASK_USER
                || permission > PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW) {
            throw new IllegalArgumentException("invalid SMS permission type " + permission);
        }
        {    AutoLock syncLock(mPremiumSmsPolicy);
            mPremiumSmsPolicy->Put(packageName, permission);
        }
        // write policy file in the background
        new Thread(new Runnable() {
            //@Override
            CARAPI Run() {
                WritePremiumSmsPolicyDb();
            }
        }).Start();
    }

    private static void CheckCallerIsSystemOrSameApp(String pkg) {
        Int32 uid = Binder->GetCallingUid();
        If (UserHandle->GetAppId(uid) == Process.SYSTEM_UID || uid == 0) {
            return;
        }
        try {
            ApplicationInfo ai = AppGlobals->GetPackageManager()->GetApplicationInfo(
                    pkg, 0, UserHandle->GetCallingUserId());
            If (!UserHandle->IsSameApp(ai.uid, uid)) {
                throw new SecurityException("Calling uid " + uid + " gave package"
                        + pkg + " which is owned by uid " + ai.uid);
            }
        } Catch (RemoteException re) {
            throw new SecurityException("Unknown package " + pkg + "\n" + re);
        }
    }

    private static void CheckCallerIsSystemOrPhoneApp() {
        Int32 uid = Binder->GetCallingUid();
        Int32 appId = UserHandle->GetAppId(uid);
        If (appId == Process.SYSTEM_UID || appId == Process.PHONE_UID || uid == 0) {
            return;
        }
        throw new SecurityException("Disallowed call for uid " + uid);
    }

    /**
     * Remove keys containing only old timestamps. This can happen if an SMS app is used
     * to send messages and then uninstalled.
     */
    private void RemoveExpiredTimestamps() {
        Int64 beginCheckPeriod = System->CurrentTimeMillis() - mCheckPeriod;

        {    AutoLock syncLock(mSmsStamp);
            Iterator<Map.Entry<String, ArrayList<Long>>> iter = mSmsStamp->EntrySet()->Iterator();
            While (iter->HasNext()) {
                Map.Entry<String, ArrayList<Long>> entry = iter->Next();
                ArrayList<Long> oldList = entry->GetValue();
                If (oldList->IsEmpty() || oldList->Get(oldList->Size() - 1) < beginCheckPeriod) {
                    iter->Remove();
                }
            }
        }
    }

    private Boolean IsUnderLimit(ArrayList<Long> sent, Int32 smsWaiting) {
        Long ct = System->CurrentTimeMillis();
        Int64 beginCheckPeriod = ct - mCheckPeriod;

        If (VDBG) Log("SMS send size=" + sent->Size() + " time=" + ct);

        While (!sent->IsEmpty() && sent->Get(0) < beginCheckPeriod) {
            sent->Remove(0);
        }

        If ((sent->Size() + smsWaiting) <= mMaxAllowed) {
            For (Int32 i = 0; i < smsWaiting; i++ ) {
                sent->Add(ct);
            }
            return TRUE;
        }
        return FALSE;
    }

    private static void Log(String msg) {
        Rlog->D(TAG, msg);
    }
}
