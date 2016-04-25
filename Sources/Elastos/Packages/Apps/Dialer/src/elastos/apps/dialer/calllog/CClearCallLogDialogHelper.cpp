
#include "CClearCallLogDialogHelper.h"
#include "ClearCallLogDialogHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CClearCallLogDialogHelper, Singleton, IClearCallLogDialogHelper)

CAR_SINGLETON_IMPL(CClearCallLogDialogHelper)

ECode CClearCallLogDialogHelper::Show(
    /* [in] */ IFragmentManager* fragmentManager)
{
    ClearCallLogDialogHelper::Show(fragmentManager);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
