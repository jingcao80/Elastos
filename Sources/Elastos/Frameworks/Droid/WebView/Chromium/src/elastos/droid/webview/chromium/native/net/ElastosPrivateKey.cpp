#include "elastos/droid/webkit/webview/chromium/native/net/ElastosPrivateKey.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/ElastosPrivateKey_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

AutoPtr<IInterface> ElastosPrivateKey::GetKeyStore(
    /* [in] */ IInterface* obj)
{
    ElastosPrivateKey* mObj = (ElastosPrivateKey*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosPrivateKey", "ElastosPrivateKey::GetKeyStore, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->GetKeyStore());
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


