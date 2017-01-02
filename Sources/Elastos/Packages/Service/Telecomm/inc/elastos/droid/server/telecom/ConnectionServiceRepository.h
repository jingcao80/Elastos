//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
