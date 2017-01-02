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

#include "elastos/droid/keystore/security/CCredentialsHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CCredentialsHelper, Singleton, ICredentialsHelper)
CAR_SINGLETON_IMPL(CCredentialsHelper)

ECode CCredentialsHelper::ConvertToPem(
    /* [in] */ ArrayOf<ICertificate*>* objects,
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    return Credentials::ConvertToPem(objects, ret);
    return NOERROR;
}

ECode CCredentialsHelper::ConvertFromPem(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ IList** ret)
{
    return Credentials::ConvertFromPem(bytes, ret);
}

ECode CCredentialsHelper::GetInstance(
    /* [out] */ ICredentials** instance)
{
    return Credentials::GetInstance(instance);
}

ECode CCredentialsHelper::DeleteAllTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* ret)
{
    return Credentials::DeleteAllTypesForAlias(keystore, alias, ret);
}

ECode CCredentialsHelper::DeleteCertificateTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* ret)
{
    return Credentials::DeleteCertificateTypesForAlias(keystore, alias, ret);
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
