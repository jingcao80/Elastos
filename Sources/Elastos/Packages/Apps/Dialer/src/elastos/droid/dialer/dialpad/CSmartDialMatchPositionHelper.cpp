#include "elastos/droid/dialer/dialpad/SmartDialMatchPosition.h"
#include "elastos/droid/dialer/dialpad/CSmartDialMatchPositionHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CSmartDialMatchPositionHelper, Singleton, ISmartDialMatchPositionHelper);

CAR_SINGLETON_IMPL(CSmartDialMatchPositionHelper);

ECode CSmartDialMatchPositionHelper::AdvanceMatchPositions(
    /* [in] */ IArrayList* inList,
    /* [in] */ Int32 toAdvance)
{
    SmartDialMatchPosition::AdvanceMatchPositions(inList, toAdvance);
    return NOERROR;
}

ECode CSmartDialMatchPositionHelper::Print(
    /* [in] */ IArrayList* list)
{
    SmartDialMatchPosition::Print(list);
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos