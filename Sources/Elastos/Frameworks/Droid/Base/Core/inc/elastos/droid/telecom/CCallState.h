#ifndef __ELASTOS_DROID_TELECOM_CCALLSTATE_H__
#define __ELASTOS_DROID_TELECOM_CCALLSTATE_H__

#include "_Elastos_Droid_Telecom_CCallState.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Defines call-state constants of the different states in which a call can exist. Although states
 * have the notion of normal transitions, due to the volatile nature of telephony systems, code
 * that uses these states should be resilient to unexpected state changes outside of what is
 * considered traditional.
 *
 * {@hide}
 */
CarClass(CCallState)
    , public Singleton
    , public ICallState
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [in] */ Int32 callState,
        /* [out] */ String* result);
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCALLSTATE_H__