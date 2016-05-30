#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Provider.h"

//using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISettingsSystem;
//using Elastos::Droid::Database::ICursor;
//using Elastos::Droid::Net::IUri;
//using Elastos::Droid::Os::IUserHandle;
//using Elastos::Droid::Provider::ISettings;
//using Elastos::Droid::Provider::Telephony::IBlacklist;
//using Elastos::Droid::Text::ITextUtils;
//using Elastos::Droid::Utility::ILog;

//using Elastos::Utility::IArrayList;
//using Elastos::Utility::IList;

//using Elastos::Droid::Internal::Telephony::ICallerInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

/**
 * Blacklist Utility Class
 * @hide
 */
class BlacklistUtils
{
public:
    static Boolean AddOrUpdate(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 valid);

    /**
     * Check if the number is in the blacklist
     * @param number: Number to check
     * @return one of: MATCH_NONE, MATCH_PRIVATE, MATCH_UNKNOWN, MATCH_LIST or MATCH_REGEX
     */
    static Int32 IsListed(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistEnabled(
        /* [in] */ IContext* context);

    static Boolean IsBlacklistNotifyEnabled(
        /* [in] */ IContext* context);

    static Boolean IsBlacklistPrivateNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistUnknownNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistRegexEnabled(
        /* [in] */ IContext* context);

public:
    // Blacklist matching type
    static const Int32 MATCH_NONE = 0;
    static const Int32 MATCH_PRIVATE = 1;
    static const Int32 MATCH_UNKNOWN = 2;
    static const Int32 MATCH_LIST = 3;
    static const Int32 MATCH_REGEX = 4;

    static const Int32 BLOCK_CALLS = ISettingsSystem::BLACKLIST_BLOCK << ISettingsSystem::BLACKLIST_PHONE_SHIFT;
    static const Int32 BLOCK_MESSAGES = ISettingsSystem::BLACKLIST_BLOCK << ISettingsSystem::BLACKLIST_MESSAGE_SHIFT;

private:
    static const String TAG;// = "BlacklistUtils";
    static const Boolean DEBUG;// = FALSE;

};

} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H
