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

#ifndef __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__
#define __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__

#include "_Elastos_Security_CKeyStorePrivateKeyEntry.h"
#include "Object.h"

namespace Elastos {
namespace Security {

CarClass(CKeyStorePrivateKeyEntry)
    , public Object
    , public IKeyStorePrivateKeyEntry
    , public IKeyStoreEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPrivateKey(
        /* [out] */ IPrivateKey **privateKey);

    CARAPI GetCertificateChain(
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **cc);

    CARAPI GetCertificate(
        /* [out] */ Elastos::Security::Cert::ICertificate **cert);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ IPrivateKey *privateKey,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain);

private:
    // Store Certificate chain
    AutoPtr<ArrayOf<Elastos::Security::Cert::ICertificate*> > mChain;

    // Store PrivateKey
    AutoPtr<IPrivateKey> mPrivateKey;

};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__
