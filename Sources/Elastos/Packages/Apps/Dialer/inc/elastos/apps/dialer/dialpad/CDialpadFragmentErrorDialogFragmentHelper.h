#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CDialpadFragmentErrorDialogFragmentHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Apps {
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
