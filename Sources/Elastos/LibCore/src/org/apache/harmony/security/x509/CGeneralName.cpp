
#include "org/apache/harmony/security/x509/CGeneralName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CGeneralName)

CAR_INTERFACE_IMPL(CGeneralName, Object, IGeneralName)

ECode CGeneralName::GetTag(
    /* [out] */ Int32* pTag)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetName(
    /* [out] */ IInterface** ppName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::Equals(
    /* [in] */ IInterface* pOther,
    /* [out] */ Boolean* pEquals)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetHashCode(
    /* [out] */ Int32* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::IsAcceptable(
    /* [in] */ IGeneralName* pGname,
    /* [out] */ Boolean* pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetAsList(
    /* [out] */ IList** ppList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetEncodedName(
    /* [out, callee] */ ArrayOf<Byte>** ppEncodedName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Int32 tag,
    /* [in] */ const String& name)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ IOtherName* pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ IORAddress* pName1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ IName* pName2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ IEDIPartyName* pName3)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ ArrayOf<Byte>* pName4)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte>* pName1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org