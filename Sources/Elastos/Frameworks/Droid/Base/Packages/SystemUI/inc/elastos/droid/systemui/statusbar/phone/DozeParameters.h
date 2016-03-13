
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DOZEPARAMETERS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DOZEPARAMETERS_H__

#include "_SystemUI.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class DozeParameters
    : public Object
{
public:
    class PulseSchedule: public Object
    {
    public:
        static CARAPI_(AutoPtr<PulseSchedule>) Parse(
            /* [in] */ const String& spec);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI_(Int64) GetNextTime(
            /* [in] */ Int64 now,
            /* [in] */ Int64 notificationTime);

        static CARAPI_(AutoPtr<IPattern>) InitStatic();

    public:
        static AutoPtr<IPattern> PATTERN;

        String mSpec;
        AutoPtr<ArrayOf<Int32> > mSchedule;
    };

public:
    DozeParameters(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(Boolean) GetDisplayStateSupported();

    CARAPI_(Int32) GetPulseDuration();

    CARAPI_(Int32) GetPulseInDuration();

    CARAPI_(Int32) GetPulseVisibleDuration();

    CARAPI_(Int32) GetPulseOutDuration();

    CARAPI_(Boolean) GetPulseOnSigMotion();

    CARAPI_(Boolean) GetVibrateOnSigMotion();

    CARAPI_(Boolean) GetPulseOnPickup();

    CARAPI_(Boolean) GetVibrateOnPickup();

    CARAPI_(Boolean) GetPulseOnNotifications();

    CARAPI_(AutoPtr<PulseSchedule>) GetPulseSchedule();

    CARAPI_(Int32) GetPulseScheduleResets();

    CARAPI_(Int32) GetPickupVibrationThreshold();

private:
    CARAPI_(Boolean) GetBoolean(
        /* [in] */ const String& propName,
        /* [in] */ Int32 resId);

    CARAPI_(Int32) GetInt32(
        /* [in] */ const String& propName,
        /* [in] */ Int32 resId);

    CARAPI_(String) GetString(
        /* [in] */ const String& propName,
        /* [in] */ Int32 resId);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 MAX_DURATION;

    AutoPtr<IContext> mContext;

    static AutoPtr<PulseSchedule> sPulseSchedule;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DOZEPARAMETERS_H__
