#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CPDUPERSISTERHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CPDUPERSISTERHELPER_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CPduPersisterHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CPduPersisterHelper)
    , public Singleton
    , public IPduPersisterHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetPduPersister(
        /* [in] */ IContext* context,
        /* [out] */ IPduPersister** result);

    CARAPI ConvertUriToPath(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [out] */ String* result);

    CARAPI ToIsoString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ String* result);

    CARAPI GetBytes(
        /* [in] */ const String& data,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CPDUPERSISTERHELPER_H__
