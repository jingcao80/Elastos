#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/telephony/utility/BlacklistUtils.h"
#include "elastos/droid/internal/telephony/CCallerInfo.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ITelephonyBlacklist;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Internal::Telephony::CCallerInfo;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

const String BlacklistUtils::TAG("BlacklistUtils");
const Boolean BlacklistUtils::DEBUG = FALSE;

Boolean BlacklistUtils::AddOrUpdate(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 valid)
{
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);

    if ((valid & BLOCK_CALLS) != 0) {
        cv->Put(ITelephonyBlacklist::PHONE_MODE, (flags & BLOCK_CALLS) != 0 ? 1 : 0);
    }
    if ((valid & BLOCK_MESSAGES) != 0) {
        cv->Put(ITelephonyBlacklist::MESSAGE_MODE, (flags & BLOCK_MESSAGES) != 0 ? 1 : 0);
    }

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->WithAppendedPath(Elastos::Droid::Provider::Telephony::Blacklist::CONTENT_FILTER_BYNUMBER_URI, number, (IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 count = 0;
    cr->Update(uri, cv, String(NULL), NULL, &count);

    return count > 0;
}

Int32 BlacklistUtils::IsListed(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 mode)
{
    if (!IsBlacklistEnabled(context)) {
        return MATCH_NONE;
    }

    if (DEBUG) {
        Logger::D(TAG, "Checking number %s against the Blacklist for mode %d", number.string(), mode);
    }

    String type;

    if (mode == BLOCK_CALLS) {
        if (DEBUG) Logger::D(TAG, "Checking if an incoming call should be blocked");
        type = ITelephonyBlacklist::PHONE_MODE;
    } else if (mode == BLOCK_MESSAGES) {
        if (DEBUG) Logger::D(TAG, "Checking if an incoming message should be blocked");
        type = ITelephonyBlacklist::MESSAGE_MODE;
    } else {
        Logger::E(TAG, "Invalid mode %d", mode);
        return MATCH_NONE;
    }

    // Private and unknown number matching
    if (TextUtils::IsEmpty(number)) {
        if (IsBlacklistPrivateNumberEnabled(context, mode)) {
            if (DEBUG) Logger::D(TAG, "Blacklist matched due to private number");
            return MATCH_PRIVATE;
        }
        return MATCH_NONE;
    }

    if (IsBlacklistUnknownNumberEnabled(context, mode)) {
        AutoPtr<ICallerInfo> ci;
        CCallerInfo::GetCallerInfo(context, number, (ICallerInfo**)&ci);
        Boolean contactExists;
        ci->GetContactExists(&contactExists);
        if (!contactExists) {
            if (DEBUG) Logger::D(TAG, "Blacklist matched due to unknown number");
            return MATCH_UNKNOWN;
        }
    }

    AutoPtr<IUriBuilder> builder;
    Elastos::Droid::Provider::Telephony::Blacklist::CONTENT_FILTER_BYNUMBER_URI->BuildUpon((IUriBuilder**)&builder);
    builder->AppendPath(number);
    if (IsBlacklistRegexEnabled(context)) {
        builder->AppendQueryParameter(ITelephonyBlacklist::REGEX_KEY, String("1"));
    }

    Int32 result = MATCH_NONE;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> c;
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, ITelephonyBlacklist::IS_REGEX);
    array->Set(1, type);
    cr->Query(uri, array, String(NULL), NULL, String(NULL), (ICursor**)&c);

    if (c != NULL) {
        if (DEBUG) {
            Int32 count;
            c->GetCount(&count);
            Logger::D(TAG, "Blacklist query successful, %d matches", count);
        }
        Int32 regexColumnIndex;
        c->GetColumnIndexOrThrow(ITelephonyBlacklist::IS_REGEX, &regexColumnIndex);
        Int32 modeColumnIndex;
        c->GetColumnIndexOrThrow(type, &modeColumnIndex);
        Boolean whitelisted = FALSE;

        Boolean moved;
        c->MoveToPosition(-1, &moved);
        Boolean next = FALSE;
        while (c->MoveToNext(&next), next) {
            Int32 index, value;
            Boolean isRegex = (c->GetInt32(regexColumnIndex, &index), index) != 0;
            Boolean blocked = (c->GetInt32(modeColumnIndex, &value), value) != 0;

            if (!isRegex) {
                whitelisted = !blocked;
                result = MATCH_LIST;
                if (blocked) {
                    break;
                }
            }
            else if (blocked) {
                result = MATCH_REGEX;
            }
        }
        if (whitelisted) {
            result = MATCH_NONE;
        }
        ICloseable::Probe(c)->Close();
    }

    if (DEBUG) Logger::D(TAG, "Blacklist check result for number %s is %d", number.string(), result);
    return result;
}

Boolean BlacklistUtils::IsBlacklistEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    return Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::PHONE_BLACKLIST_ENABLED, 1,
            IUserHandle::USER_CURRENT_OR_SELF, &res);
    return res != 0;
}

Boolean BlacklistUtils::IsBlacklistNotifyEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::PHONE_BLACKLIST_NOTIFY_ENABLED, 1,
            IUserHandle::USER_CURRENT_OR_SELF, &res);
    return res != 0;
}

Boolean BlacklistUtils::IsBlacklistPrivateNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::System::GetInt32ForUser(cr,
                ISettingsSystem::PHONE_BLACKLIST_PRIVATE_NUMBER_MODE, 0,
                IUserHandle::USER_CURRENT_OR_SELF, &res);
    return (res & mode) != 0;
}

Boolean BlacklistUtils::IsBlacklistUnknownNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::PHONE_BLACKLIST_UNKNOWN_NUMBER_MODE, 0,
            IUserHandle::USER_CURRENT_OR_SELF,  &res);
    return (res & mode) != 0;
}

Boolean BlacklistUtils::IsBlacklistRegexEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::PHONE_BLACKLIST_REGEX_ENABLED, 0,
            IUserHandle::USER_CURRENT_OR_SELF, &res);
    return res != 0;
}

} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

