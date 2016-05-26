
#include "CNameConstraintsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CNameConstraintsHelper)

CAR_INTERFACE_IMPL(CNameConstraintsHelper, Singleton, INameConstraintsHelper)

ECode CNameConstraintsHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ Org::Apache::Harmony::Security::X509::INameConstraints ** ppObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraintsHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Sequence ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

