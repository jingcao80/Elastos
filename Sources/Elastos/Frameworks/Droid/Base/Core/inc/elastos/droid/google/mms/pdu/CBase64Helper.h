#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CBASE64HELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CBASE64HELPER_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CBase64Helper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CBase64Helper)
    , public Singleton
    , public IBase64Helper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI DecodeBase64(
        /* [in] */ ArrayOf<Byte>* base64Data,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI DiscardNonBase64(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CBASE64HELPER_H__
