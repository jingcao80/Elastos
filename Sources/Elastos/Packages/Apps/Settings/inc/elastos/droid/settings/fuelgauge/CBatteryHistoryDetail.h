#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYDETAIL_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYDETAIL_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Settings_Fuelgauge_CBatteryHistoryDetail.h"
#include "elastos/droid/app/Fragment.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CarClass(CBatteryHistoryDetail)
    , public Fragment
{
public:
    CAR_OBJECT_DECL()

    CBatteryHistoryDetail();

    ~CBatteryHistoryDetail();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

public:
    static const String EXTRA_STATS;
    static const String EXTRA_BROADCAST;

private:
    AutoPtr<IBatteryStats> mStats;
    AutoPtr<IIntent> mBatteryBroadcast;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYDETAIL_H__