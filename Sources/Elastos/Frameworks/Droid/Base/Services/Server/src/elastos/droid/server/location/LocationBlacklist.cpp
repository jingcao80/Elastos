
#include "location/LocationBlacklist.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::StringUtils;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String LocationBlacklist::TAG("LocationBlacklist");
const Boolean LocationBlacklist::D = FALSE;//ILocationManagerService::D;
const String LocationBlacklist::BLACKLIST_CONFIG_NAME("locationPackagePrefixBlacklist");
const String LocationBlacklist::WHITELIST_CONFIG_NAME("locationPackagePrefixWhitelist");

LocationBlacklist::LocationBlacklist()
                 : mCurrentUserId(IUserHandle::USER_OWNER)
{}

LocationBlacklist::LocationBlacklist(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler) : ContentObserver(handler)
{
    mContext = context;
    mCurrentUserId = IUserHandle::USER_OWNER;
}

void LocationBlacklist::Init()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(BLACKLIST_CONFIG_NAME, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);
    ReloadBlacklist();
}

/**
 * Return true if in blacklist
 * (package name matches blacklist, and does not match whitelist)
 */
Boolean LocationBlacklist::IsBlacklisted(
    /* [in] */ const String& packageName)
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        Int32 length = mBlacklist->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (packageName.StartWith((*mBlacklist)[i])) {
                if (InWhitelist(packageName)) {
                    continue;
                } else {
//                  if (D) Log.d(TAG, "dropping location (blacklisted): "
//                          + packageName + " matches " + black);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

//@Override
ECode LocationBlacklist::OnChange(
    /* [in] */ Boolean selfChange)
{
    ReloadBlacklist();
    return NOERROR;
}

void LocationBlacklist::SwitchUser(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mLock);
    mCurrentUserId = userId;
    ReloadBlacklistLocked();
}

void LocationBlacklist::Dump(
    /* [in] */ IPrintWriter* pw)
{
//  pw.println("mWhitelist=" + Arrays.toString(mWhitelist) + " mBlacklist=" +
//           Arrays.toString(mBlacklist));
}

void LocationBlacklist::ReloadBlacklistLocked()
{
    mWhitelist = GetStringArrayLocked(WHITELIST_CONFIG_NAME);
//    Slog.i(TAG, "whitelist: " + Arrays.toString(mWhitelist));
    mBlacklist = GetStringArrayLocked(BLACKLIST_CONFIG_NAME);
//    Slog.i(TAG, "blacklist: " + Arrays.toString(mBlacklist));
}

void LocationBlacklist::ReloadBlacklist()
{
    AutoLock lock(mLock);
    ReloadBlacklistLocked();
}

/**
 * Return true if any of packages are in whitelist
 */
Boolean LocationBlacklist::InWhitelist(
    /* [in] */ const String& pkg)
{
    {
        AutoLock lock(mLock);
        Int32 length = mWhitelist->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (pkg.StartWith((*mWhitelist)[i])) return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<ArrayOf<String> > LocationBlacklist::GetStringArrayLocked(
    /* [in] */ const String& key)
{
    String flatString;
    {
        AutoLock lock(mLock);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->GetStringForUser(cr, key, mCurrentUserId, &flatString);
    }
    if (flatString.IsNullOrEmpty()) {
        return ArrayOf<String>::Alloc(0);
    }
    AutoPtr<ArrayOf<String> > splitStrings;
    StringUtils::Split(flatString, String(","), (ArrayOf<String>**)&splitStrings);
    if (splitStrings == NULL) return ArrayOf<String>::Alloc(0);

    AutoPtr< List<String> > rstTemp = new List<String>();
    for (Int32 i = 0; i < splitStrings->GetLength(); i++) {
        (*splitStrings)[i] = (*splitStrings)[i].Trim();
        if ((*splitStrings)[i].IsEmpty()) {
            continue;
        }
        rstTemp->PushBack((*splitStrings)[i]);
    }

    Int32 size = rstTemp->GetSize();
    if (size == 0) {
        return ArrayOf<String>::Alloc(0);
    }
    AutoPtr<ArrayOf<String> > rst = ArrayOf<String>::Alloc(size);
    List<String>::Iterator it = rstTemp->Begin();
    for (Int32 i = 0; (i < size)&&(it != rstTemp->End()); it++, i++) {
        (*rst)[i] = *it;
    }
    return rst;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
