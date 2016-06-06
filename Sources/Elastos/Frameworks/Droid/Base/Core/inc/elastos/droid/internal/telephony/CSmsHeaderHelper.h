
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsHeaderHelper.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsHeaderHelper)
    , public Singleton
    , public ISmsHeaderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create structured SmsHeader object from serialized byte array representation.
     * (see TS 23.040 9.2.3.24)
     * @param data is user data header bytes
     * @return SmsHeader object
     */
    CARAPI FromByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsHeader** result);

    /**
     * Create serialized Byte array representation from structured SmsHeader object.
     * (see TS 23.040 9.2.3.24)
     * @return Byte array representing the SmsHeader
     */
    CARAPI ToByteArray(
        /* [in] */ ISmsHeader* smsHeader,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__
