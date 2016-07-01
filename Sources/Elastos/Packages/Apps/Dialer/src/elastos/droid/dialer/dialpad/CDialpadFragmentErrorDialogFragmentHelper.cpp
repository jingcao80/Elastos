#include "elastos/droid/dialer/dialpad/DialpadFragment.h"
#include "elastos/droid/dialer/dialpad/CDialpadFragmentErrorDialogFragmentHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CDialpadFragmentErrorDialogFragmentHelper, Singleton, IDialpadFragmentErrorDialogFragmentHelper);

CAR_SINGLETON_IMPL(CDialpadFragmentErrorDialogFragmentHelper);

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialpadFragment::ErrorDialogFragment::NewInstance(messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialpadFragment::ErrorDialogFragment::NewInstance(titleResId, messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos