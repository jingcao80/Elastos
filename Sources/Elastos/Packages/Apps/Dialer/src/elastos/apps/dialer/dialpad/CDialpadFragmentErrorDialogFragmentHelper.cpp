#include "DialpadFragment.h"
#include "CDialpadFragmentErrorDialogFragmentHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CDialpadFragmentErrorDialogFragmentHelper, Singleton, IDialpadFragmentErrorDialogFragmentHelper)

CAR_SINGLETON_IMPL(CDialpadFragmentErrorDialogFragmentHelper)

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALUE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialogFragment::ErrorDialogFragment::NewInstance(messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALUE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialogFragment::ErrorDialogFragment::NewInstance(titleResId, messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos