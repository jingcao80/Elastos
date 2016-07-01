#ifndef __ELASTOS_DROID_DIALER_CSPECIALCHARSEQUENCEMGR_H__
#define __ELASTOS_DROID_DIALER_CSPECIALCHARSEQUENCEMGR_H__

#include "_Elastos_Droid_Dialer_CSpecialCharSequenceMgr.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CSPECIALCHARSEQUENCEMGR_H__
