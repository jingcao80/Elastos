
#include "CASN1Oid.h"
#include <cmdef.h>

using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IInteger32;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1TYPE_METHODS_IMPL(CASN1Oid, ASN1Oid)

ECode CASN1Oid::constructor()
{
    return ASN1Oid::Init(IASN1Constants::TAG_OID);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

