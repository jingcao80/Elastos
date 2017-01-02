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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__

#include "asn1/ASN1SetOf.h"

using Org::Apache::Harmony::Security::Asn1::ASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class AuthenticatedAttributes : public Object
{
private:
    class ASN1SetOfDerived : public ASN1SetOf
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);
    };

public:
    static AutoPtr<ASN1SetOf> InitStatic();

    CARAPI GetAttributes(
        /* [out] */ IList** attributes);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    TO_STRING_IMPL("AuthenticatedAttributes")

private:
    AuthenticatedAttributes(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [in] */ IList* authenticatedAttributes);

public:
    static AutoPtr<ASN1SetOf> ASN1;

private:
    AutoPtr<ArrayOf<Byte> > mEncoding;
    AutoPtr<IList> mAuthenticatedAttributes;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__
