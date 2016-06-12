
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDataProfileHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CarClass(CDataProfileHelper)
    , public Singleton
    , public IDataProfileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ToParcel(
        /* [in] */ IParcel* pc,
        /* [in] */ ArrayOf<IDataProfile*>* dps,
        /* [out] */ IParcel** result);
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILEHELPER_H__
