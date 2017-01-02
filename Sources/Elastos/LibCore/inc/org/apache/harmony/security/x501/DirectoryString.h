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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__

#include "ASN1Choice.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IASN1Choice;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

/**
 * The class encapsulates the ASN.1 DER encoding/decoding work
 * with the DirectoryString structure
 * (as specified in RFC 3280 -
 *  Internet X.509 Public Key Infrastructure.
 *  Certificate and Certificate Revocation List (CRL) Profile.
 *  http://www.ietf.org/rfc/rfc3280.txt):
 *
 * <pre>
 *   DirectoryString ::= CHOICE {
 *        teletexString             TeletexString   (SIZE (1..MAX)),
 *        printableString           PrintableString (SIZE (1..MAX)),
 *        universalString           UniversalString (SIZE (1..MAX)),
 *        utf8String              UTF8String      (SIZE (1..MAX)),
 *        bmpString               BMPString       (SIZE (1..MAX))
 *   }
 * </pre>
 */
class DirectoryString
{
private:
    class ASN1ChoiceCls: public ASN1Choice
    {
    public:
        ASN1ChoiceCls();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* asn1Type);

        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* index);

        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

private:
    static CARAPI_(AutoPtr<IASN1Choice>) Init();

public:
    static AutoPtr<IASN1Choice> ASN1;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__
