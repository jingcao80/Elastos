#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CCharacterSetsHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CCharacterSetsHelper)
    , public Singleton
    , public ICharacterSetsHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetMimeName(
        /* [in] */ Int32 mibEnumValue,
        /* [out] */ String* result);

    CARAPI GetMibEnumValue(
        /* [in] */ const String& mimeName,
        /* [out] */ Int32* result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__
