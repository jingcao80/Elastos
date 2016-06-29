#ifndef __ELASTOS_APPS_DIALER_CSPECIALCHARSEQUENCEMGR_H__
#define __ELASTOS_APPS_DIALER_CSPECIALCHARSEQUENCEMGR_H__

#include "_Elastos_Apps_Dialer_CSpecialCharSequenceMgr.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Apps {
namespace Dialer {

CarClass(CSpecialCharSequenceMgr)
    , public Singleton
    , public ISpecialCharSequenceMgr
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ String input,
        /* [in] */ IEditText* textField,
        /* [out] */ Boolean* result);

    /**
     * Cleanup everything around this class. Must be run inside the main thread.
     *
     * This should be called when the screen becomes background.
     */
    CARAPI Cleanup();
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CSPECIALCHARSEQUENCEMGR_H__
