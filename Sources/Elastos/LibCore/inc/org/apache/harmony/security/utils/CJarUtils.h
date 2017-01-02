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

#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_CJARUTILS_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_CJARUTILS_H__

#include "_Org_Apache_Harmony_Security_Utils_CJarUtils.h"
#include "core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::IInputStream;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CJarUtils)
    , public Singleton
    , public IJarUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI VerifySignature(
        /* [in] */ IInputStream* signature,
        /* [in] */ IInputStream* signatureBlock,
        /* [out, callee] */ ArrayOf<ICertificate*>** sign);

private:
    static CARAPI_(AutoPtr< ArrayOf<IX509Certificate*> >) CreateChain(
        /* [in] */ IX509Certificate* signer,
        /* [in] */ const ArrayOf<IX509Certificate*>& candidates);

    static CARAPI_(AutoPtr<IX509Certificate>) FindCert(
        /* [in] */ IPrincipal* issuer,
        /* [in] */ const ArrayOf<IX509Certificate*>& candidates);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) Init_MESSAGE_DIGEST_OID();

private:
    // as defined in PKCS #9: Selected Attribute Types:
    // http://www.ietf.org/rfc/rfc2985.txt
    static const AutoPtr< ArrayOf<Int32> > MESSAGE_DIGEST_OID;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_CJARUTILS_H__
