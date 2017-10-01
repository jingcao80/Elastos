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

#ifndef __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
#define __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__

#include "_Org_Apache_Harmony_Security_CSystemScope.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IEnumeration;
using Elastos::Security::IIdentityScope;
using Elastos::Security::ICertificate;
using Elastos::Security::IPublicKey;
using Elastos::Security::IIdentity;
using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CarClass(CSystemScope)
    , public Object
    , public ISystemScope
    , public IIdentityScope
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AddCertificate(
        /* [in] */ ICertificate* pCertificate);

    CARAPI RemoveCertificate(
        /* [in] */ ICertificate* pCertificate);

    CARAPI Certificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** ppCerts);

    using Object::ToString;

    CARAPI ToString(
        /* [in] */ Boolean detailed,
        /* [out] */ String* pStr);

    CARAPI GetScope(
        /* [out] */ IIdentityScope** ppScope);

    CARAPI SetPublicKey(
        /* [in] */ IPublicKey* pKey);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** ppPubKey);

    CARAPI SetInfo(
        /* [in] */ const String& info);

    CARAPI GetInfo(
        /* [out] */ String* pStr);

    CARAPI GetSize(
        /* [out] */ Int32* pSize);

    CARAPI GetIdentity(
        /* [in] */ const String& name,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI GetIdentity(
        /* [in] */ IPrincipal* pPrincipal,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI GetIdentity(
        /* [in] */ IPublicKey* pKey,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI AddIdentity(
        /* [in] */ IIdentity* pIdentity);

    CARAPI RemoveIdentity(
        /* [in] */ IIdentity* pIdentity);

    CARAPI Identities(
        /* [out] */ IEnumeration** ppIds);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IIdentityScope* pScope);

private:
    // TODO: Add your private member variables here.
};

} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
