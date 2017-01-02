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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1StringTypeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1StringTypeHelper)
    , public Singleton
    , public IASN1StringTypeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBMPSTRING(
        /* [out] */ IASN1Type** BMPSTRING);

    CARAPI GetIA5STRING(
        /* [out] */ IASN1Type** IA5STRING);

    CARAPI GetGENERALSTRING(
        /* [out] */ IASN1Type** GENERALSTRING);

    CARAPI GetPRINTABLESTRING(
        /* [out] */ IASN1Type** PRINTABLESTRING);

    CARAPI GetTELETEXSTRING(
        /* [out] */ IASN1Type** TELETEXSTRING);

    CARAPI GetUNIVERSALSTRING(
        /* [out] */ IASN1Type** UNIVERSALSTRING);

    CARAPI GetUTF8STRING(
        /* [out] */ IASN1Type** UTF8STRING);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__
