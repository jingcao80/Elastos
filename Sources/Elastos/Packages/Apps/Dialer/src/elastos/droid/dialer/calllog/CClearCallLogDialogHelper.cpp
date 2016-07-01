
#include "elastos/droid/dialer/calllog/CClearCallLogDialogHelper.h"
#include "elastos/droid/dialer/calllog/ClearCallLogDialog.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CClearCallLogDialogHelper, Singleton, IClearCallLogDialogHelper);

CAR_SINGLETON_IMPL(CClearCallLogDialogHelper);

ECode CClearCallLogDialogHelper::Show(
    /* [in] */ IFragmentManager* fragmentManager)
{
    ClearCallLogDialog::Show(fragmentManager);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
