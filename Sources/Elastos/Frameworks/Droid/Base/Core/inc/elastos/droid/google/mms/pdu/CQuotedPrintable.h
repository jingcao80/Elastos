#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CQUOTEDPRINTABLE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CQUOTEDPRINTABLE_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CQuotedPrintable.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CQuotedPrintable)
    , public Singleton
    , public IQuotedPrintable
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI DecodeQuotedPrintable(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CQUOTEDPRINTABLE_H__
