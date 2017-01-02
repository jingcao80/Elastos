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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_Pkcs7_CContentInfoHelper.h"
#include "core/Singleton.h"

using Elastos::Core::Singleton;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CarClass(CContentInfoHelper)
    , public Singleton
    , public IContentInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Int32>** data);

    CARAPI GetSignedData(
        /* [out, callee] */ ArrayOf<Int32>** signedData);

    CARAPI GetEnvelopedData(
        /* [out, callee] */ ArrayOf<Int32>** envelopedData);

    CARAPI GetSignedAndEnvelopedData(
        /* [out, callee] */ ArrayOf<Int32>** signedAndEnvelopedData);

    CARAPI GetDigestedData(
        /* [out, callee] */ ArrayOf<Int32>** digestedData);

    CARAPI GetEncryptedData(
        /* [out, callee] */ ArrayOf<Int32>** encryptedData);

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** asn1);
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CCONTENTINFOHELPER_H__
