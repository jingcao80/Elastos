
#include "elastos/droid/net/EthernetManager.h"
#include "elastos/droid/net/CIpConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(EthernetManager, Object, IEthernetManager)

const String EthernetManager::TAG("EthernetManager");

ECode EthernetManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIEthernetManager* service)
{
    mContext = context;
    mService = service;
    return NOERROR;
}

ECode EthernetManager::GetConfiguration(
    /* [out] */ IIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result)

    if (mService == NULL) {
        return CIpConfiguration::New(result);
    }
        // try {
    ECode ec = mService->GetConfiguration(result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return CIpConfiguration::New(result);
    }
        // }
    return ec;
}

ECode EthernetManager::SetConfiguration(
    /* [in] */ IIpConfiguration* config)
{
    if (mService == NULL) {
        return NOERROR;
    }
        // try {
    ECode ec = mService->SetConfiguration(config);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
