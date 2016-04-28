
#ifndef __ELASTOS_DROID_SERVER_AM_CORESETTINGSOBSERVER_H__
#define __ELASTOS_DROID_SERVER_AM_CORESETTINGSOBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

/**
 * Helper class for watching a set of core settings which the framework
 * propagates to application processes to avoid multiple lookups and potentially
 * disk I/O operations. Note: This class assumes that all core settings reside
 * in {@link Settings.Secure}.
 */
class CoreSettingsObserver
    : public ContentObserver
{
public:
    TO_STRING_IMPL("CoreSettingsObserver")

    CoreSettingsObserver();

    CARAPI constructor(
        /* [in] */ CActivityManagerService* activityManagerService);

    CARAPI_(AutoPtr<IBundle>) GetCoreSettingsLocked();

    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    static CARAPI_(Boolean) Init();

private:
    CARAPI_(void) SendCoreSettings();

    CARAPI_(void) BeginObserveCoreSettings();

    CARAPI_(void) PopulateSettings(
        /* [in] */ IBundle* snapshot,
        /* [in] */ HashMap<String, String>* map);

private:
   static const String TAG;

   // mapping form property name to its type
   static HashMap<String, String> sSecureSettingToTypeMap; // = new HashMap<String, Class<?>>();
   static HashMap<String, String> sSystemSettingToTypeMap; // = new HashMap<String, Class<?>>();
   static HashMap<String, String> sGlobalSettingToTypeMap; // = new HashMap<String, Class<?>>();
   static Boolean sInit;

    AutoPtr<IBundle> mCoreSettings;

    CActivityManagerService* mActivityManagerService;   // weak-ref
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CORESETTINGSOBSERVER_H__
