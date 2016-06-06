
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsHeader
    : public Object
    , public ISmsHeader
{
public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Create structured SmsHeader object from serialized byte array representation.
     * (see TS 23.040 9.2.3.24)
     * @param data is user data header bytes
     * @return SmsHeader object
     */
    static CARAPI FromByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsHeader** result);

    /**
     * Create serialized Byte array representation from structured SmsHeader object.
     * (see TS 23.040 9.2.3.24)
     * @return Byte array representing the SmsHeader
     */
    static CARAPI ToByteArray(
        /* [in] */ ISmsHeader* smsHeader,
        /* [out, callee] */ ArrayOf<Byte>** result);

public:


};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__
