
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBLACKLIST_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBLACKLIST_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::StringUtils;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Content::IContentResolver;

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
class LocationBlacklist : public ContentObserver
{
private:

    static const String TAG;

public:

    LocationBlacklist();

    LocationBlacklist(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    CARAPI_(void) Init();

    /**
     * Return true if in blacklist
     * (package name matches blacklist, and does not match whitelist)
     */
    CARAPI_(Boolean) IsBlacklisted(
        /* [in] */ const String& packageName);

    //@Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    CARAPI_(void) SwitchUser(
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
    static const Boolean D;
    static const String BLACKLIST_CONFIG_NAME;
    static const String WHITELIST_CONFIG_NAME;

    AutoPtr<IContext> mContext;
    Object mLock;

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
