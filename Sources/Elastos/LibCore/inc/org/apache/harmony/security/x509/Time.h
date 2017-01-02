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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "ASN1Choice.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IASN1Choice;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

/**
 * Class represents the work with the following X.509 structure:
 * (as specified in RFC 3280 -
 *  Internet X.509 Public Key Infrastructure.
 *  Certificate and Certificate Revocation List (CRL) Profile.
 *  http://www.ietf.org/rfc/rfc3280.txt):
 *
 * <pre>
 * Time ::= CHOICE {
 *       utcTime        UTCTime,
 *       generalTime    GeneralizedTime
 * }
 * </pre>
 */
class Time
{
private:
    class ASN1ChoiceWrapper: public ASN1Choice
    {
    public:
        ASN1ChoiceWrapper();

        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* result);

        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

private:
    static CARAPI_(AutoPtr<IASN1Choice>) InitASN1();

public:
    static AutoPtr<IASN1Choice> ASN1;

private:
    static const Int64 JAN_01_2050;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__
