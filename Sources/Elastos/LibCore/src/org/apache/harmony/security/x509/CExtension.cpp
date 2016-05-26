
#include "CExtension.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CExtension)

CAR_INTERFACE_IMPL(CExtension, Object, IExtension)

ECode CExtension::GetExtnID(
    /* [out] */ String * pExtnID)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetCritical(
    /* [out] */ Boolean * pCritical)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetExtnValue(
    /* [out, callee] */ ArrayOf<Byte> ** ppExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetRawExtnValue(
    /* [out, callee] */ ArrayOf<Byte> ** ppRawExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::Equals(
    /* [in] */ IInterface * pExt,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetDecodedExtensionValue(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensionValue ** ppExtensionValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetKeyUsageValue(
    /* [out] */ Org::Apache::Harmony::Security::X509::IKeyUsage ** ppUsageValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::GetBasicConstraintsValue(
    /* [out] */ Org::Apache::Harmony::Security::X509::IBasicConstraints ** ppBasicConstrainsValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ Org::Apache::Harmony::Security::X509::IExtensionValue * pExtnValueObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte> * pExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32> * pExtnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte> * pExtnValue2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ ArrayOf<Byte> * pExtnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32> * pExtnID,
    /* [in] */ ArrayOf<Byte> * pExtnValue2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

