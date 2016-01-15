
#include "elastos/droid/internal/widget/CLockPatternViewCellHelper.h"
#include "elastos/droid/internal/widget/LockPatternView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_SINGLETON_IMPL(CLockPatternViewCellHelper);
CAR_INTERFACE_IMPL(CLockPatternViewCellHelper, Singleton, ILockPatternViewCellHelper);
ECode CLockPatternViewCellHelper::Of(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ ILockPatternViewCell** cell)
{
    VALIDATE_NOT_NULL(cell);
    AutoPtr<ILockPatternViewCell> c = LockPatternView::Cell::Of(row, column);
    *cell = c;
    REFCOUNT_ADD(*cell);
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
