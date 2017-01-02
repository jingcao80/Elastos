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

#include "CASN1StringTypeHelper.h"
#include <ASN1StringType.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1StringTypeHelper)

CAR_INTERFACE_IMPL(CASN1StringTypeHelper, Singleton, IASN1StringTypeHelper)

ECode CASN1StringTypeHelper::GetBMPSTRING(
    /* [out] */ IASN1Type** BMPSTRING)
{
    return ASN1StringType::GetBMPSTRING(BMPSTRING);
}

ECode CASN1StringTypeHelper::GetIA5STRING(
    /* [out] */ IASN1Type** IA5STRING)
{
    return ASN1StringType::GetIA5STRING(IA5STRING);
}

ECode CASN1StringTypeHelper::GetGENERALSTRING(
    /* [out] */ IASN1Type** GENERALSTRING)
{
    return ASN1StringType::GetGENERALSTRING(GENERALSTRING);
}

ECode CASN1StringTypeHelper::GetPRINTABLESTRING(
    /* [out] */ IASN1Type** PRINTABLESTRING)
{
    return ASN1StringType::GetPRINTABLESTRING(PRINTABLESTRING);
}

ECode CASN1StringTypeHelper::GetTELETEXSTRING(
    /* [out] */ IASN1Type** TELETEXSTRING)
{
    return ASN1StringType::GetTELETEXSTRING(TELETEXSTRING);
}

ECode CASN1StringTypeHelper::GetUNIVERSALSTRING(
    /* [out] */ IASN1Type** UNIVERSALSTRING)
{
    return ASN1StringType::GetUNIVERSALSTRING(UNIVERSALSTRING);
}

ECode CASN1StringTypeHelper::GetUTF8STRING(
    /* [out] */ IASN1Type** UTF8STRING)
{
    return ASN1StringType::GetUTF8STRING(UTF8STRING);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

