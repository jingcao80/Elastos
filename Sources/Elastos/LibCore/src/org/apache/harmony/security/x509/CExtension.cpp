
#include "org/apache/harmony/security/x509/CExtension.h"

using Elastos::Security::Cert::EIID_IExtension;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CExtension)

CAR_INTERFACE_IMPL_2(CExtension, Object, IX509Extension, IExtension)

ECode CExtension::GetId(
    /* [out] */ String* id)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::IsCritical(
    /* [out] */ Boolean* isCritical)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetValue(
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::Encode(
    /* [in] */ IOutputStream* outValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetExtnID(
    /* [out] */ String* pExtnID)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetCritical(
    /* [out] */ Boolean* pCritical)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetExtnValue(
    /* [out, callee] */ ArrayOf<Byte>** ppExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetRawExtnValue(
    /* [out, callee] */ ArrayOf<Byte>** ppRawExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::Equals(
    /* [in] */ IInterface* pExt,
    /* [out] */ Boolean* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetHashCode(
    /* [out] */ Int32* pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetDecodedExtensionValue(
    /* [out] */ IExtensionValue** ppExtensionValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetKeyUsageValue(
    /* [out] */ IKeyUsage** ppUsageValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetBasicConstraintsValue(
    /* [out] */ IBasicConstraints** ppBasicConstrainsValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ IExtensionValue* pExtnValueObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte>* pExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32>* pExtnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte>* pExtnValue2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ ArrayOf<Byte>* pExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32>* pExtnID,
    /* [in] */ ArrayOf<Byte>* pExtnValue2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org