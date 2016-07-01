
#include "elastos/droid/dialer/interactions/CPhoneDisambiguationDialogFragmentHelper.h"
#include "elastos/droid/dialer/interactions/PhoneNumberInteraction.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CAR_INTERFACE_IMPL(CPhoneDisambiguationDialogFragmentHelper, Singleton, IPhoneDisambiguationDialogFragmentHelper);

CAR_SINGLETON_IMPL(CPhoneDisambiguationDialogFragmentHelper);

ECode CPhoneDisambiguationDialogFragmentHelper::Show(
    /* [in] */ IFragmentManager* fragmentManager,
    /* [in] */ IArrayList* phoneList,
    /* [in] */ Int32 interactionType,
    /* [in] */ const String& callOrigin)
{
    PhoneNumberInteraction::PhoneDisambiguationDialogFragment::Show(
            fragmentManager, phoneList, interactionType, callOrigin);
    return NOERROR;
}

} // Interactions
} // Dialer
} // Droid
} // Elastos