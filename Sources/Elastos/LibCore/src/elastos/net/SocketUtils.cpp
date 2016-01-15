
#include "SocketUtils.h"

namespace Elastos {
namespace Net {

void SocketUtils::SetCreated(
    /* [in] */ ISocket* s)
{
    assert(s != NULL);
    Socket* so = (Socket*)s;
    so->SetIsCreated(TRUE);
}

} // namespace Net
} // namespace Elastos