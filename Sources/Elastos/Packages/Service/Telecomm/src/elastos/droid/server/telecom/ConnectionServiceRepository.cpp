
#include "elastos/droid/server/telecom/ConnectionServiceRepository.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(ConnectionServiceRepository, Object, IServiceBinderListener)

ECode ConnectionServiceRepository::constructor(
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
    /* [in] */ IContext* context)
{
    CHashMap::New((IHashMap**)&mServiceCache);
    mPhoneAccountRegistrar = phoneAccountRegistrar;
    mContext = context;
    return NOERROR;
}

ECode ConnectionServiceRepository::GetService(
    /* [in] */ IComponentName* componentName,
    /* [out] */ IConnectionServiceWrapper** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mServiceCache->Get(componentName, (IInterface**)&obj);
    AutoPtr<IConnectionServiceWrapper> service = IConnectionServiceWrapper::Probe(obj);
    if (service == NULL) {
        service = new ConnectionServiceWrapper();
        ((ConnectionServiceWrapper*) service.Get())->constructor(
                componentName,
                this,
                mPhoneAccountRegistrar,
                mContext);
        ((ConnectionServiceWrapper*) service.Get())->AddListener(this);
        mServiceCache->Put(componentName, service);
    }
    *result = service;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionServiceRepository::OnUnbind(
    /* [in] */ IServiceBinder* service)
{
    AutoPtr<IComponentName> componentName;
    ((ServiceBinder*) service)->GetComponentName((IComponentName**)&componentName);
    return mServiceCache->Remove(componentName);
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
