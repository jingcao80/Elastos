
#include "CCertificateMessage.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CCertificateMessage)

CAR_INTERFACE_IMPL(CCertificateMessage, Object, ICertificateMessage)

ECode CCertificateMessage::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateMessage::GetAuthType(
    /* [out] */ String * pAuthType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateMessage::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
    /* [in] */ Int32 length)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateMessage::constructor(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pCerts)
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

