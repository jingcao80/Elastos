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

#include "cert/CCertificateFactoryHelper.h"
#include "cert/CertificateFactory.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CCertificateFactoryHelper, Singleton, ICertificateFactoryHelper)

CAR_SINGLETON_IMPL(CCertificateFactoryHelper)

ECode CCertificateFactoryHelper::GetInstance(
    /* [in] */ const String& type,
    /* [out] */ ICertificateFactory** factory)
{
    return CertificateFactory::GetInstance(type, factory);
}

ECode CCertificateFactoryHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ const String& provider,
    /* [out] */ ICertificateFactory** factory)
{
    return CertificateFactory::GetInstance(type, provider, factory);
}

ECode CCertificateFactoryHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [out] */ ICertificateFactory** factory)
{
    return CertificateFactory::GetInstance(type, provider, factory);
}

} // end Cert
} // end Security
} // end Elastos
