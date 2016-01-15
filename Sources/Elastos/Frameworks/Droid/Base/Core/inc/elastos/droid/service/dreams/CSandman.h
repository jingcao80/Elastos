
#ifndef __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__
#define __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__

#include "_Elastos_Droid_Service_Dreams_CSandman.h"
#include "elastos/droid/service/dreams/Sandman.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CarClass(CSandman)
    , public Singleton
    , public ISandman
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns true if the specified dock app intent should be started.
     * False if we should dream instead, if appropriate.
     */
    CARAPI ShouldStartDockApp(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Starts a dream manually.
     */
    CARAPI StartDreamByUserRequest(
        /* [in] */ IContext* context);

    /**
     * Starts a dream when docked if the system has been configured to do so,
     * otherwise does nothing.
     */
    CARAPI StartDreamWhenDockedIfAppropriate(
        /* [in] */ IContext* context);
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__
