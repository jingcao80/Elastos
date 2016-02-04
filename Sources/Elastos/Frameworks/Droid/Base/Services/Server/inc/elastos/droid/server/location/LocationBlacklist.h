
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBLACKLIST_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBLACKLIST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Allows applications to be blacklisted from location updates at run-time.
 *
 * This is a silent blacklist. Applications can still call Location Manager
 * API's, but they just won't receive any locations.
 */
class LocationBlacklist
    : public ContentObserver
{
public:
    LocationBlacklist(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    CARAPI Init();

    /**
     * Return true if in blacklist
     * (package name matches blacklist, and does not match whitelist)
     */
    CARAPI IsBlacklisted(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) ReloadBlacklistLocked();

    CARAPI_(void) ReloadBlacklist();

    /**
     * Return true if any of packages are in whitelist
     */
    CARAPI_(Boolean) InWhitelist(
        /* [in] */ const String& pkg);

    CARAPI_(AutoPtr<ArrayOf<String> >) GetStringArrayLocked(
        /* [in] */ const String& key);

private:
    static const String TAG;
    static const Boolean D;
    static const String BLACKLIST_CONFIG_NAME;
    static const String WHITELIST_CONFIG_NAME;

    AutoPtr<IContext> mContext;

    // all fields below synchronized on mLock
    AutoPtr<ArrayOf<String> > mWhitelist;
    AutoPtr<ArrayOf<String> > mBlacklist;

    Int32 mCurrentUserId;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONBLACKLIST_H__
