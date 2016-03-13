
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

class CurrentUserTracker
    : public BroadcastReceiver
{
public:
    CurrentUserTracker(
        /* [in] */ IContext* context);

    CARAPI_(Int32) GetCurrentUserId();

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI_(void) StartTracking();

    CARAPI_(void) StopTracking();

    virtual CARAPI_(void) OnUserSwitched(
        /* [in] */ Int32 newUserId) = 0;

    CARAPI_(Boolean) IsCurrentUserOwner();

private:
    AutoPtr<IContext> mContext;
    Int32 mCurrentUserId;
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CURRENTUSERTRACKER_H__