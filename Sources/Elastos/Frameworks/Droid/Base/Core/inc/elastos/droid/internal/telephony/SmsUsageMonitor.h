
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSUSAGEMONITOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSUSAGEMONITOR_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "elastos/droid/content/Context.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/CAtomicFile.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Core::Object;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::IAtomicFile;

using Elastos::IO::IFile;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Regex::IPattern;

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsUsageMonitor
    : public Handler
    , public ISmsUsageMonitor
{
private:
    class ShortCodePatternMatcher : public Object
    {
    public:
        ShortCodePatternMatcher(
            /* [in] */ const String& shortCodeRegex,
            /* [in] */ const String& premiumShortCodeRegex,
            /* [in] */ const String& freeShortCodeRegex,
            /* [in] */ const String& standardShortCodeRegex);

        CARAPI GetNumberCategory(
            /* [in] */ const String& phoneNumber);

    private:
        AutoPtr<IPattern> mShortCodePattern;
        AutoPtr<IPattern> mPremiumShortCodePattern;
        AutoPtr<IPattern> mFreeShortCodePattern;
        AutoPtr<IPattern> mStandardShortCodePattern;
    };

    class SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context,
            /* [in] */ IAtomicBoolean* enabled);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IAtomicBoolean> mEnabled;
    };

    class SettingsObserverHandler : public Handler
    {
    public:
        TO_STRING_IMPL("SmsUsageMonitor::SettingsObserverHandler")

        SettingsObserverHandler(
            /* [in] */ IContext* context,
            /* [in] */ IAtomicBoolean* enabled);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IAtomicBoolean> mEnabled;
    };

    class SmsLimitObserver : public ContentObserver
    {
    public:
        SmsLimitObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IContext> mContext;
    };

    class SmsLimitObserverHandler : public Handler
    {
    public:
        TO_STRING_IMPL("SmsUsageMonitor::SmsLimitObserverHandler")

        SmsLimitObserverHandler(
            /* [in] */ IContext* context);
    };

public:
    TO_STRING_IMPL("SmsUsageMonitor")

    CAR_INTERFACE_DECL()

    SmsUsageMonitor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Check(
        /* [in] */ const String& appName,
        /* [in] */ Int32 smsWaiting,
        /* [out] */ Boolean* result);

    CARAPI CheckDestination(
        /* [in] */ const String& destAddress,
        /* [in] */ const String& countryIso,
        /* [out] */ Int32* result);

    CARAPI GetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    CARAPI SetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 permission);

    static CARAPI MergeShortCodeCategories(
        /* [in] */ Int32 type1,
        /* [in] */ Int32 type2,
        /* [in] */ Int32* result);

    /** Clear the SMS application list for disposal. */
    void Dispose();

private:
    /**
     * Return a pattern matcher object for the specified country.
     * @param country the country to search for
     * @return a {@link ShortCodePatternMatcher} for the specified country, or NULL if not found
     */
    AutoPtr<ShortCodePatternMatcher> GetPatternMatcherFromFile(
        /* [in] */ const String& country);

    AutoPtr<ShortCodePatternMatcher> GetPatternMatcherFromResource(
        /* [in] */ const String& country);

    AutoPtr<ShortCodePatternMatcher> GetPatternMatcherFromXmlParser(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& country);

    /**
     * Load the premium SMS policy from an XML file.
     * Based on code from NotificationManagerService.
     */
    void LoadPremiumSmsPolicyDb();

    /**
     * Persist the premium SMS policy to an XML file.
     * Based on code from NotificationManagerService.
     */
    void WritePremiumSmsPolicyDb();

    void CheckCallerIsSystemOrSameApp(
        /* [in] */ const String& pkg);

    void CheckCallerIsSystemOrPhoneApp();

    /**
     * Remove keys containing only old timestamps. This can happen if an SMS app is used
     * to send messages and then uninstalled.
     */
    void RemoveExpiredTimestamps();

    Boolean IsUnderLimit(
        /* [in] */ IArrayList* sent,
        /* [in] */ Int32 smsWaiting);

    void Log(
        /* [in] */ const String& msg);

public:
    /** Return value from {@link #checkDestination} for regular phone numbers. */
    static const Int32 CATEGORY_NOT_SHORT_CODE;

    /** Return value from {@link #checkDestination} for Free (no cost) short codes. */
    static const Int32 CATEGORY_FREE_SHORT_CODE;

    /** Return value from {@link #checkDestination} for standard Rate (non-premium) short codes. */
    static const Int32 CATEGORY_STANDARD_SHORT_CODE;

    /** Return value from {@link #checkDestination} for possible premium short codes. */
    static const Int32 CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE;

    /** Return value from {@link #checkDestination} for premium short codes. */
    static const Int32 CATEGORY_PREMIUM_SHORT_CODE;

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;

    static const String SHORT_CODE_PATH;

    /** Default checking period for SMS sent without user permission. */
    static const Int32 DEFAULT_SMS_CHECK_PERIOD;

    /** Default number of SMS sent in checking period without user permission. */
    static const Int32 DEFAULT_SMS_MAX_COUNT;

    Int32 mCheckPeriod;
    Int32 mMaxAllowed;

    AutoPtr<IHashMap> mSmsStamp;

    /** Context for retrieving regexes from XML resource. */
    AutoPtr<IContext> mContext;

    /** Country code for the cached short code pattern matcher. */
    String mCurrentCountry;

    /** Cached short code pattern matcher for {@link #mCurrentCountry}. */
    AutoPtr<ShortCodePatternMatcher> mCurrentPatternMatcher;

    /** Notice when the enabled setting changes - can be changed through gservices */
    AutoPtr<IAtomicBoolean> mCheckEnabled;

    /** Handler for responding to content observer updates. */
    AutoPtr<SettingsObserverHandler> mSettingsObserverHandler;

    /** Handler for responding to content observer updates sms limits. */
    AutoPtr<SmsLimitObserverHandler> mSmsLimitObserverHandler;

    /** File holding the patterns */
    AutoPtr<IFile> mPatternFile;

    /** Last modified time for pattern file */
    Int64 mPatternFileLastModified;

    /** Directory for per-app SMS permission XML file. */
    static const String SMS_POLICY_FILE_DIRECTORY;

    /** Per-app SMS permission XML filename. */
    static const String SMS_POLICY_FILE_NAME;

    /** XML tag for root element. */
    static const String TAG_SHORTCODES;

    /** XML tag for short code patterns for a specific country. */
    static const String TAG_SHORTCODE;

    /** XML attribute for the country code. */
    static const String ATTR_COUNTRY;

    /** XML attribute for the short code regex pattern. */
    static const String ATTR_PATTERN;

    /** XML attribute for the premium short code regex pattern. */
    static const String ATTR_PREMIUM;

    /** XML attribute for the free short code regex pattern. */
    static const String ATTR_FREE;

    /** XML attribute for the standard rate short code regex pattern. */
    static const String ATTR_STANDARD;

    /** Stored copy of premium SMS package permissions. */
    AutoPtr<IAtomicFile> mPolicyFile;

    /** Loaded copy of premium SMS package permissions. */
    AutoPtr<IHashMap> mPremiumSmsPolicy;

    /** XML tag for root element of premium SMS permissions. */
    static const String TAG_SMS_POLICY_BODY;

    /** XML tag for a package. */
    static const String TAG_PACKAGE;

    /** XML attribute for the package name. */
    static const String ATTR_PACKAGE_NAME;

    /** XML attribute for the package's premium SMS Permission (integer type). */
    static const String ATTR_PACKAGE_SMS_POLICY;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSUSAGEMONITOR_H__
