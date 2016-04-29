
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CNextAlarmControllerBroadcastReceiver;
class NextAlarmController
    : public Object
    , public INextAlarmController
{
public:
    CAR_INTERFACE_DECL();

    NextAlarmController(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ INextAlarmChangeCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ INextAlarmChangeCallback* cb);

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) UpdateNextAlarm();

    CARAPI_(void) FireNextAlarmChanged();

private:
    AutoPtr<IArrayList> mChangeCallbacks;  /*<NextAlarmChangeCallback*/

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IAlarmClockInfo> mNextAlarm;
    friend class CNextAlarmControllerBroadcastReceiver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__
