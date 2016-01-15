
#ifndef __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__

#include "_Elastos_Droid_Content_CIntentSenderHelper.h"
#include <elastos/core/Singleton.h>

using namespace Elastos;
using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentSenderHelper)
    , public Singleton
    , public IIntentSenderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Convenience function for writing either a IntentSender or null pointer to
     * a Parcel.  You must use this with {@link #readIntentSenderOrNullFromParcel}
     * for later reading it.
     *
     * @param sender The IntentSender to write, or null.
     * @param out Where to write the IntentSender.
     */
    CARAPI WriteIntentSenderOrNullToParcel(
        /* [in] */ IIntentSender* sender,
        /* [in] */ IParcel* parcel);

    /**
     * Convenience function for reading either a Messenger or null pointer from
     * a Parcel.  You must have previously written the Messenger with
     * {@link #writeIntentSenderOrNullToParcel}.
     *
     * @param in The Parcel containing the written Messenger.
     *
     * @return Returns the Messenger read from the Parcel, or null if null had
     * been written.
     */
    CARAPI ReadIntentSenderOrNullFromParcel(
        /* [in] */ IParcel* parcel,
        /* [out] */ IIntentSender** intentSender);

};

} // Content
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__
