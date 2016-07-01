
#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__

#include "_Elastos_Droid_Dialer_Interactions_CPhoneDisambiguationDialogFragmentHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IFragmentManager;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CarClass(CPhoneDisambiguationDialogFragmentHelper)
    , public Singleton
    , public IPhoneDisambiguationDialogFragmentHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Show(
        /* [in] */ IFragmentManager* fragmentManager,
        /* [in] */ IArrayList* phoneList,
        /* [in] */ Int32 interactionType,
        /* [in] */ const String& callOrigin);
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__
