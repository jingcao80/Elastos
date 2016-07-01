
#include "elastos/droid/dialer/CSpecialCharSequenceMgr.h"
#include "elastos/droid/dialer/SpecialCharSequenceMgr.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CAR_INTERFACE_IMPL(CSpecialCharSequenceMgr, Singleton, ISpecialCharSequenceMgr);

CAR_SINGLETON_IMPL(CSpecialCharSequenceMgr);

ECode CSpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ String input,
    /* [in] */ IEditText* textField,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SpecialCharSequenceMgr::HandleChars(context, input, textField);
    return NOERROR;
}

ECode CSpecialCharSequenceMgr::Cleanup()
{
    SpecialCharSequenceMgr::Cleanup();
    return NOERROR;
}

} // Dialer
} // Droid
} // Elastos
