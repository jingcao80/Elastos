
#include "CSubjectPublicKeyInfoHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CSubjectPublicKeyInfoHelper)

CAR_INTERFACE_IMPL(CSubjectPublicKeyInfoHelper, Singleton, ISubjectPublicKeyInfoHelper)

ECode CSubjectPublicKeyInfoHelper::GetASN1(
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

