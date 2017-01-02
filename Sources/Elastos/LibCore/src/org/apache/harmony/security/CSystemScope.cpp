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

#include "CSystemScope.h"

using Elastos::Security::EIID_IIdentityScope;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CAR_OBJECT_IMPL(CSystemScope)

CAR_INTERFACE_IMPL_2(CSystemScope, Object, ISystemScope, IIdentityScope)

ECode CSystemScope::AddCertificate(
    /* [in] */ ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveCertificate(
    /* [in] */ ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Certificates(
    /* [out, callee] */ ArrayOf<ICertificate *> ** ppCerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::ToString(
    /* [in] */ Boolean detailed,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetScope(
    /* [out] */ IIdentityScope ** ppScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::SetPublicKey(
    /* [in] */ IPublicKey * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetPublicKey(
    /* [out] */ IPublicKey ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::SetInfo(
    /* [in] */ const String& info)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetInfo(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ const String& name,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ IPrincipal * pPrincipal,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ IPublicKey * pKey,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::AddIdentity(
    /* [in] */ IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveIdentity(
    /* [in] */ IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Identities(
    /* [out] */ IEnumeration ** ppIds)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor(
    /* [in] */ const String& name)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor(
    /* [in] */ const String& name,
    /* [in] */ IIdentityScope * pScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org