#include "dialpad/SmartDialMatchPosition.h"
#include "dialpad/CSmartDialMatchPositionHelper.h"

namespace Elastos {
namespace Apps {
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
} // Apps
} // Elastos