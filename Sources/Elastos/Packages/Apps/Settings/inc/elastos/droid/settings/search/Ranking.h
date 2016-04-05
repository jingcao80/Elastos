#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_RANKING_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_RANKING_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

/**
 * Utility class for dealing with Search Ranking.
 */
class Ranking
{
public:
    static CARAPI_(Int32) GetRankForClassName(
        /* [in] */ const String& className);

    static CARAPI_(Int32) GetBaseRankForAuthority(
        /* [in] */ const String& authority);

public:
    static const Int32 RANK_WIFI;
    static const Int32 RANK_BT;
    static const Int32 RANK_SIM;
    static const Int32 RANK_DATA_USAGE;
    static const Int32 RANK_WIRELESS;
    static const Int32 RANK_HOME;
    static const Int32 RANK_DISPLAY;
    static const Int32 RANK_WALLPAPER;
    static const Int32 RANK_NOTIFICATIONS;
    static const Int32 RANK_MEMORY;
    static const Int32 RANK_POWER_USAGE;
    static const Int32 RANK_USERS;
    static const Int32 RANK_LOCATION;
    static const Int32 RANK_SECURITY;
    static const Int32 RANK_IME;
    static const Int32 RANK_PRIVACY;
    static const Int32 RANK_DATE_TIME;
    static const Int32 RANK_ACCESSIBILITY;
    static const Int32 RANK_PRINTING;
    static const Int32 RANK_DEVELOPEMENT;
    static const Int32 RANK_DEVICE_INFO;

    static const Int32 RANK_UNDEFINED;
    static const Int32 RANK_OTHERS;
    static const Int32 BASE_RANK_DEFAULT;

    static Int32 sCurrentBaseRank;

private:
    // static HashMap<String, Integer> sRankMap = new HashMap<String, Integer>();
    static AutoPtr<IHashMap> sRankMap;
    // static HashMap<String, Integer> sBaseRankMap = new HashMap<String, Integer>();
    static AutoPtr<IHashMap> sBaseRankMap;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_RANKING_H__
