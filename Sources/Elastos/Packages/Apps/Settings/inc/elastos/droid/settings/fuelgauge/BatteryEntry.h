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

#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_BATTERYENTRY_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_BATTERYENTRY_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Thread.h>
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Internal::Os::BatterySipperDrainType;
using Elastos::Droid::Os::IBatteryStatsUid;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserManager;
using Elastos::Core::Thread;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

/**
 * Wraps the power usage data of a BatterySipper with information about package name
 * and icon image.
 */
class BatteryEntry
    : public Object
    , public IBatteryEntry
{
protected:
    class UidToDetail
        : public Object
    {
    public:
        TO_STRING_IMPL("BatteryEntry::UidToDetail")

        UidToDetail()
        {}

    public:
        String mName;
        String mPackageName;
        AutoPtr<IDrawable> mIcon;
    };

private:
    class NameAndIconLoader
        : public Thread
    {
    public:
        TO_STRING_IMPL("BatteryEntry::NameAndIconLoader")

        NameAndIconLoader();

        CARAPI constructor();

        CARAPI Abort();

        //@Override
        CARAPI Run();

    private:
        Boolean mAbort;
    };

public:
    TO_STRING_IMPL("BatteryEntry")

    CAR_INTERFACE_DECL()

    static CARAPI_(void) StartRequestQueue();

    static CARAPI_(void) StopRequestQueue();

    static CARAPI_(void) ClearUidCache();

    BatteryEntry(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IUserManager* um,
        /* [in] */ IBatterySipper* sipper);

    CARAPI_(AutoPtr<IDrawable>) GetIcon();

    /**
     * Gets the application name
     */
    CARAPI_(String) GetLabel();

    CARAPI_(void) GetQuickNameIconForUid(
        /* [in] */ IBatteryStatsUid* uidObj);

    /**
     * Loads the app label and icon image and stores into the cache.
     */
    CARAPI LoadNameAndIcon();

public:
    static const Int32 MSG_UPDATE_NAME_ICON = 1;
    static const Int32 MSG_REPORT_FULLY_DRAWN = 2;

    static const AutoPtr<IHashMap> sUidCache;//HashMap<String,UidToDetail>

    static const AutoPtr<IArrayList> sRequestQueue; //ArrayList<BatteryEntry>
    static AutoPtr<IHandler> sHandler;

    AutoPtr<IContext> mContext;
    AutoPtr<IBatterySipper> mSipper;

    String mName;
    AutoPtr<IDrawable> mIcon;
    Int32 mIconId; // For passing to the detail screen.
    String mDefaultPackageName;

private:
    static AutoPtr<BatteryEntry::NameAndIconLoader> sRequestThread;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_BATTERYENTRY_H__