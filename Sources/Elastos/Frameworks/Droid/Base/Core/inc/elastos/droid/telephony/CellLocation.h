
#ifndef __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellLocation
    : public Object
    , public ICellLocation
{
public:

    CAR_INTERFACE_DECL()

    /**
    * Request an update of the current location.  If the location has changed,
    * a broadcast will be sent to everyone registered with {@link
    * PhoneStateListener#LISTEN_CELL_LOCATION}.
    */
    static CARAPI RequestLocationUpdate();

    /**
     * Create a new CellLocation from a intent notifier Bundle
     *
     * This method is used by PhoneStateIntentReceiver and maybe by
     * external applications.
     *
     * @param bundle Bundle from intent notifier
     * @return newly created CellLocation
     *
     * @hide
     */
    static CARAPI NewFromBundle(
        /* [in] */ IBundle* bundle,
        /* [out] */ ICellLocation** cl);

    static CARAPI GetEmpty(
        /* [out] */ ICellLocation** cl);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* bundle);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__
