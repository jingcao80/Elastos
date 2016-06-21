#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cdma.sms;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

class CdmaSmsSubaddress
    : public Object
    , public ICdmaSmsSubaddress
{
public:
    CAR_INTERFACE_DECL();

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI SetOdd(
        /* [in] */ Byte odd);

    CARAPI GetOdd(
        /* [out] */ Byte* result);

    CARAPI SetOrigBytes(
        /* [in] */ ArrayOf<Byte>* origBytes);

    CARAPI GetOrigBytes(
        /* [out, callee] */ ArrayOf<Byte>** result);

    Int32 type;
    Byte odd;
    AutoPtr<ArrayOf<Byte> > origBytes;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__

