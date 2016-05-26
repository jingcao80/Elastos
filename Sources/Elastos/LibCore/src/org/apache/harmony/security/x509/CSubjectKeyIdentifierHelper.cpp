
#include "CSubjectKeyIdentifierHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CSubjectKeyIdentifierHelper)

CAR_INTERFACE_IMPL(CSubjectKeyIdentifierHelper, Singleton, ISubjectKeyIdentifierHelper)

ECode CSubjectKeyIdentifierHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ Org::Apache::Harmony::Security::X509::ISubjectKeyIdentifier ** ppIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

