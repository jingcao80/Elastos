
#include "org/apache/harmony/security/x509/ExtensionValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_INTERFACE_IMPL(ExtensionValue, Object, IExtensionValue)

ECode ExtensionValue::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode ExtensionValue::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Unparseable extension value:\n"));
    if (mEncoding == NULL) {
        GetEncoded((ArrayOf<Byte>**)&mEncoding);
    }
    if (mEncoding == NULL) {
        sb->Append(String("NULL\n"));
    }
    else {
        String str;
        assert(0);
        //CArrayHelper::ToString(mEncoding, prefix, &str);
        sb->Append(str);
    }
    return NOERROR;
}

ECode ExtensionValue::DumpValue(
    /* [in] */ IStringBuilder* sb)
{
    return DumpValue(sb, String(""));
}

ECode ExtensionValue::constructor()
{
    return NOERROR;
}

ECode ExtensionValue::constructor(
    /* [in] */ ArrayOf<Byte>* pEncoding)
{
    mEncoding = mEncoding;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org