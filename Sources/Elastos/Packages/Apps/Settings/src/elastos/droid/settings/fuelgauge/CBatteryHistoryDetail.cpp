
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryDetail.h"
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryChart.h"
#include "../R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Internal::Os::IBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::CBatteryStatsHelperHelper;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

const String CBatteryHistoryDetail::EXTRA_STATS("stats");
const String CBatteryHistoryDetail::EXTRA_BROADCAST("broadcast");

CAR_OBJECT_IMPL(CBatteryHistoryDetail)

CBatteryHistoryDetail::CBatteryHistoryDetail()
{}

CBatteryHistoryDetail::~CBatteryHistoryDetail()
{}

ECode CBatteryHistoryDetail::constructor()
{
    return Fragment::constructor();
}

ECode CBatteryHistoryDetail::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Fragment::OnCreate(icicle);
    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    String histFile;
    arguments->GetString(EXTRA_STATS, &histFile);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IBatteryStatsHelperHelper> helper;
    CBatteryStatsHelperHelper::AcquireSingleton((IBatteryStatsHelperHelper**)&helper);
    helper->StatsFromFile(IContext::Probe(activity), histFile, (IBatteryStats**)&mStats);
    AutoPtr<IParcelable> parcelable;
    arguments->GetParcelable(EXTRA_BROADCAST, (IParcelable**)&parcelable);
    mBatteryBroadcast = IIntent::Probe(parcelable);
    return NOERROR;
}

ECode CBatteryHistoryDetail::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::battery_history_chart, NULL, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::battery_history_chart, (IView**)&tmp);
    CBatteryHistoryChart* chart = (CBatteryHistoryChart*)tmp.Get();
    chart->SetStats(mStats, mBatteryBroadcast);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos