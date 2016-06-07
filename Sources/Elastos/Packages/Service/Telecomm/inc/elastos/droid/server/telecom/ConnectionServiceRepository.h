
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEREPOSITORY_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEREPOSITORY_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Searches for and returns connection services.
 */
class ConnectionServiceRepository
    : public Object
    , public IServiceBinderListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
        /* [in] */ IContext* context);

    CARAPI GetService(
        /* [in] */ IComponentName* componentName,
        /* [out] */ IConnectionServiceWrapper** result);

    /**
     * Removes the specified service from the cache when the service unbinds.
     *
     * {@inheritDoc}
     */
    // @Override
    CARAPI OnUnbind(
        /* [in] */ IServiceBinder* service);

private:
    AutoPtr<IHashMap> mServiceCache;

    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    AutoPtr<IContext> mContext;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEREPOSITORY_H__
