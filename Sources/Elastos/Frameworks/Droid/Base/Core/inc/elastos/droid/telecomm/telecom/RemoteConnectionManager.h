#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONMANAGER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONMANAGER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Telecomm::Internal::IIConnectionService;
using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * @hide
 */
class RemoteConnectionManager
    : public Object
    , public IRemoteConnectionManager
{
public:
    CAR_INTERFACE_DECL()

    RemoteConnectionManager();

    CARAPI constructor(
        /* [in] */ IConnectionService* ourConnectionServiceImpl);

    CARAPI AddConnectionService(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IIConnectionService* outgoingConnectionServiceRpc);

    CARAPI CreateRemoteConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ Boolean isIncoming,
        /* [out] */ IRemoteConnection** result);

    CARAPI ConferenceRemoteConnections(
        /* [in] */ IRemoteConnection* a,
        /* [in] */ IRemoteConnection* b);

private:
    AutoPtr<IMap> mRemoteConnectionServices;
    AutoPtr<IConnectionService> mOurConnectionServiceImpl;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONMANAGER_H__