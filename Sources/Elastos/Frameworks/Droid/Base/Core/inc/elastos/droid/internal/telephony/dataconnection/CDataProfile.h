
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILE_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDataProfile.h"
#include "elastos/droid/internal/telephony/dataconnection/DataProfile.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CarClass(CDataProfile)
    , public DataProfile
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATAPROFILE_H__
