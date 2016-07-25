#ifndef __ELASTOS_DROID_TELECOM_CREMOTECONFERENCE_H__
#define __ELASTOS_DROID_TELECOM_CREMOTECONFERENCE_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CRemoteConference.h"
#include "elastos/droid/telecom/RemoteConference.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteConference)
    , public RemoteConference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTECONFERENCE_H__
