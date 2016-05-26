
#include "CMessageImprint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CMessageImprint)

CAR_INTERFACE_IMPL(CMessageImprint, Object, IMessageImprint)

ECode CMessageImprint::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CMessageImprint::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence * pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CMessageImprint::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pAlgId,
    /* [in] */ ArrayOf<Byte> * pHashedMessage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

