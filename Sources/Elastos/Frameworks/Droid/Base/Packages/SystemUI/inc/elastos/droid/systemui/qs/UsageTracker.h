
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_USAGETRACKER_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_USAGETRACKER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include "_Elastos_Droid_SystemUI_Qs_CUsageTrackerReceiver.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::SystemUI::StatusBar::Policy::IListenable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

class CUsageTrackerReceiver;

class UsageTracker
    : public Object
    , public IListenable
{
public:
    CAR_INTERFACE_DECL()

    UsageTracker(
        /* [in] */ IContext* context,
        /* [in] */ QSTile/*Class<?>*/* tile);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listen);

    CARAPI_(Boolean) IsRecentlyUsed();

    CARAPI_(void) TrackUsage();

    CARAPI_(void) Reset();

private:
    CARAPI_(AutoPtr<ISharedPreferences>) GetSharedPrefs();

private:
    static const Int64 MILLIS_PER_DAY;

    AutoPtr<IContext> mContext;
    Int64 mTimeToShowTile;
    String mPrefKey;
    String mResetAction;

    Boolean mRegistered;
    AutoPtr<IBroadcastReceiver> mReceiver;
    friend class CUsageTrackerReceiver;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_USAGETRACKER_H__
