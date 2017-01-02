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

#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__

#include "_Elastos_Droid_KeyStore_Security_CCredentialsHelper.h"
#include "elastos/droid/keystore/security/Credentials.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CCredentialsHelper)
    , public Singleton
    , public ICredentialsHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI ConvertToPem(
        /* [in] */ ArrayOf<ICertificate*>* objects,
        /* [out, callee] */ ArrayOf<Byte>** ret);

    CARAPI ConvertFromPem(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ IList** ret);

    CARAPI GetInstance(
        /* [out] */ ICredentials** instance);

    CARAPI DeleteAllTypesForAlias(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* ret);

    CARAPI DeleteCertificateTypesForAlias(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* ret);

};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__
