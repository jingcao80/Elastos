//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
