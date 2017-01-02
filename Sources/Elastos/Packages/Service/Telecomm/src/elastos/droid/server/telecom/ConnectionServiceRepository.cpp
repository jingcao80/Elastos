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
