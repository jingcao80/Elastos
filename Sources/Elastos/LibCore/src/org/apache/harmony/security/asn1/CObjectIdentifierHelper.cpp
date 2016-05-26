
#include "CObjectIdentifierHelper.h"
#include "CObjectIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {
CAR_SINGLETON_IMPL(CObjectIdentifierHelper)

CAR_INTERFACE_IMPL(CObjectIdentifierHelper, Singleton, IObjectIdentifierHelper)

ECode CObjectIdentifierHelper::Validate(
    /* [in] */ ArrayOf<Int32>* oid)
{
    return CObjectIdentifier::Validate(oid);
}

ECode CObjectIdentifierHelper::ToString(
    /* [in] */ ArrayOf<Int32>* oid,
    /* [out] */ String* str)
{
    return CObjectIdentifier::ToString(oid, str);
}

ECode CObjectIdentifierHelper::ToIntArray(
    /* [in] */ const String& str,
    /* [out, callee] */ ArrayOf<Int32>** intArray)
{
    return CObjectIdentifier::ToIntArray(str, intArray);
}

ECode CObjectIdentifierHelper::IsOID(
    /* [in] */ const String& str,
    /* [out] */ Boolean* isOID)
{
    return CObjectIdentifier::IsOID(str, isOID);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

