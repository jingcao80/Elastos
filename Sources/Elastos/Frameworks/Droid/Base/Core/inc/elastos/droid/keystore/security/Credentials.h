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

#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__
#define _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__

#include "Elastos.Droid.KeyStore.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::IKeyPair;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * {@hide}
 */
class Credentials
    : public Object
    , public ICredentials
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();
    /**
     * Convert objects to a PEM format which is used for
     * CA_CERTIFICATE and USER_CERTIFICATE entries.
     */
    static CARAPI ConvertToPem(
       /* [in] */ ArrayOf<ICertificate*>* objects,
       /* [out] */ ArrayOf<Byte>** result);

    /**
     * Convert objects from PEM format, which is used for
     * CA_CERTIFICATE and USER_CERTIFICATE entries.
     */
    static CARAPI ConvertFromPem(
       /* [in] */ ArrayOf<Byte>* bytes,
       /* [out] */ IList** result); // List<X509Certificate>


    static CARAPI GetInstance(
       /* [out] */ ICredentials** instance);

    /**
     * Delete all types (private key, certificate, CA certificate) for a
     * particular {@code alias}. All three can exist for any given alias.
     * Returns {@code true} if there was at least one of those types.
     */
    static CARAPI DeleteCertificateTypesForAlias(
       /* [in] */ IKeyStore* keystore,
       /* [in] */ const String& alias,
       /* [out] */ Boolean* result);

    /**
     * Delete all types (private key, certificate, CA certificate) for a
     * particular {@code alias}. All three can exist for any given alias.
     * Returns {@code true} if there was at least one of those types.
     */
    static CARAPI DeleteAllTypesForAlias(
       /* [in] */ IKeyStore* keystore,
       /* [in] */ const String& alias,
       /* [out] */ Boolean* ret);

    using Object::Unlock;

    CARAPI Unlock(
        /* [in] */ IContext* context);

    CARAPI Install(
        /* [in] */ IContext* context);

    CARAPI Install(
        /* [in] */ IContext* context,
        /* [in] */ IKeyPair* pair);

    CARAPI Install(
        /* [in] */ IContext* context,
        /* [in] */ const String& type,
        /* [in] */ ArrayOf<Byte>* value);

private:
    static const String LOGTAG;// = "Credentials";
    static AutoPtr<ICredentials> singleton;
};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__
