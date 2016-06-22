#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CENCODEDSTRINGVALUEHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CENCODEDSTRINGVALUEHELPER_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CEncodedStringValueHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CEncodedStringValueHelper)
    , public Singleton
    , public IEncodedStringValueHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI Extract(
        /* [in] */ const String& src,
        /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result);

    /**
       * Concatenate an EncodedStringValue[] into a single String.
        */
    CARAPI Concat(
        /* [in] */ ArrayOf<IEncodedStringValue*>* addr,
        /* [out] */ String* result);

    CARAPI Copy(
        /* [in] */ IEncodedStringValue* value,
        /* [out] */ IEncodedStringValue** result);

    CARAPI EncodeStrings(
        /* [in] */ ArrayOf<String>* array,
        /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CENCODEDSTRINGVALUEHELPER_H__
