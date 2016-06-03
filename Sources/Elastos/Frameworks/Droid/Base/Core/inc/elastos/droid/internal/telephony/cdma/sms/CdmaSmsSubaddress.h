#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__

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
{
public:
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

