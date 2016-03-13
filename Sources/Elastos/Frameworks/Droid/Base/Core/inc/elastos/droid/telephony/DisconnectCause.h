
#ifndef __ELASTOS_DROID_TELEPHONY_DISCONNECTCAUSE_H__
#define __ELASTOS_DROID_TELEPHONY_DISCONNECTCAUSE_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

class DisconnectCause
{
public:
    /** Returns descriptive string for the specified disconnect cause. */
    static CARAPI ToString(
        /* [in] */ Int32 cause,
        /* [out] */ String* str);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_DISCONNECTCAUSE_H__
