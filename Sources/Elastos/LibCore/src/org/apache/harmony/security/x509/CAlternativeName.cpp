
#include "org/apache/harmony/security/x509/CAlternativeName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CAlternativeName)

CAR_INTERFACE_IMPL(CAlternativeName, ExtensionValue, IAlternativeName)

ECode CAlternativeName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        assert(0);
        //mEncoding = GeneralNames.ASN1.encode(alternativeNames);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CAlternativeName::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append((mWhich) ? String("Subject") : String("Issuer"));
    sb->Append(String(" Alternative Names [\n"));
    mAlternativeNames->DumpValue(sb, prefix + String("  "));
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CAlternativeName::constructor(
    /* [in] */ Boolean which,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    mWhich = which;
    assert(0);
    //mAlternativeNames = (GeneralNames) GeneralNames.ASN1.decode(encoding);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org