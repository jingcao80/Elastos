
#include "CASN1StringTypeHelper.h"
#include <ASN1StringType.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


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

