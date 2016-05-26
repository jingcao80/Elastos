
#include "CGeneralName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CGeneralName)

CAR_INTERFACE_IMPL(CGeneralName, Object, IGeneralName)

ECode CGeneralName::GetTag(
    /* [out] */ Int32 * pTag)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetName(
    /* [out] */ IInterface ** ppName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::Equals(
    /* [in] */ IInterface * pOther,
    /* [out] */ Boolean * pEquals)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetHashCode(
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::IsAcceptable(
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralName * pGname,
    /* [out] */ Boolean * pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetAsList(
    /* [out] */ Elastos::Utility::IList ** ppList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::GetEncodedName(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncodedName)
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
    /* [in] */ Org::Apache::Harmony::Security::X509::IOtherName * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IORAddress * pName1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X501::IName * pName2)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IEDIPartyName * pName3)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ ArrayOf<Byte> * pName4)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralName::constructor(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte> * pName1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

