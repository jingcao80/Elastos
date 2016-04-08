
#ifndef __CGECKOBATTERYMANAGER_H__
#define __CGECKOBATTERYMANAGER_H__

#include "_CGeckoBatteryManager.h"
//#include "BroadcastReceiver.h"

using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

CarClass(CGeckoBatteryManager)//, public BroadcastReceiver
{
public:
    static Boolean IsCharging();

    static Double GetLevel();

    static Double GetRemainingTime();

    static Void EnableNotifications();

    static Void DisableNotifications();

    static AutoPtr<ArrayOf<Double> > GetCurrentInformation();

protected:
    CARAPI OnRecieve(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent);

private:
      // Those constants should be keep in sync with the ones in:
  // dom/battery/Constants.h

  static AutoPtr<IDate>    sLastLevelChange;
  static Boolean sNotificationsEnabled;
  static Double  sLevel;
  static Boolean sCharging;
  static Double  sRemainingTime;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CGECKOBATTERYMANAGER_H__
