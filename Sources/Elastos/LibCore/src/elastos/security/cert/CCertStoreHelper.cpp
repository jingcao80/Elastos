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

#include "CCertStoreHelper.h"
#include "CertStore.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CCertStoreHelper, Singleton, ICertStoreHelper)
CAR_SINGLETON_IMPL(CCertStoreHelper)
ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, certStore);
}

ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [in] */ const String& provider,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, provider, certStore);
}

ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters * params,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, provider, certStore);
}

ECode CCertStoreHelper::GetDefaultType(
    /* [out] */ String *type)
{
    return CertStore::GetDefaultType(type);
}

}
}
}
