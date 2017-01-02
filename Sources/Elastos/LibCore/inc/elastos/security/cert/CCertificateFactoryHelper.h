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

#ifndef __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__
#define __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__

#include "_Elastos_Security_Cert_CCertificateFactoryHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCertificateFactoryHelper)
    , public Singleton
    , public ICertificateFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [out] */ ICertificateFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const String& provider,
        /* [out] */ ICertificateFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [out] */ ICertificateFactory** factory);
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__
