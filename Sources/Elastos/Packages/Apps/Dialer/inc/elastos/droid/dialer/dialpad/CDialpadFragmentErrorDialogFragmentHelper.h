#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CDialpadFragmentErrorDialogFragmentHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CDialpadFragmentErrorDialogFragmentHelper)
    , public Singleton
    , public IDialpadFragmentErrorDialogFragmentHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI NewInstance(
        /* [in] */ Int32 messageResId,
        /* [out] */ IDialpadFragmentErrorDialogFragment** fragment);

    CARAPI NewInstance(
        /* [in] */ Int32 titleResId,
        /* [in] */ Int32 messageResId,
        /* [out] */ IDialpadFragmentErrorDialogFragment** fragment);

};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
