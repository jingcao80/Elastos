
#include "elastos/droid/database/CrossProcessCursorWrapper.h"
#include "elastos/droid/database/DatabaseUtils.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_2(CrossProcessCursorWrapper, CursorWrapper, ICrossProcessCursorWrapper, ICrossProcessCursor)

ECode CrossProcessCursorWrapper::FillWindow(
    /* [in] */ Int32 position,
    /* [in] */ ICursorWindow* window)
{
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->FillWindow(position, window);
    }

    DatabaseUtils::CursorFillWindow(mCursor, position, window);
    return NOERROR;
}

ECode CrossProcessCursorWrapper::GetWindow(
    /* [out] */ ICursorWindow** window)
{
    VALIDATE_NOT_NULL(window)
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->GetWindow(window);
    }
    *window = NULL;
    return NOERROR;
}

ECode CrossProcessCursorWrapper::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICrossProcessCursor* cpc = ICrossProcessCursor::Probe(mCursor);
    if (cpc != NULL) {
        return cpc->OnMove(oldPosition, newPosition, result);
    }
    *result = TRUE;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos