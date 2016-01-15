
#include "elastos/droid/webkit/native/content/browser/MediaDrmCredentialManager.h"
#include "elastos/droid/webkit/native/content/api/MediaDrmCredentialManager_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                      MediaDrmCredentialManager
//=====================================================================

ECode MediaDrmCredentialManager::ResetCredentials(
    /* [in] */ MediaDrmCredentialManagerCallback* callback)
{
    NativeResetCredentials(callback);
    return NOERROR;
}

ECode MediaDrmCredentialManager::NativeResetCredentials(
    /* [in] */ MediaDrmCredentialManagerCallback* callback)
{
    Elastos_MediaDrmCredentialManager_nativeResetCredentials(TO_IINTERFACE(callback));
    return NOERROR;
}

void MediaDrmCredentialManager::MediaDrmCredentialManagerCallback::OnCredentialResetFinished(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean succeeded)
{
    AutoPtr<MediaDrmCredentialManager::MediaDrmCredentialManagerCallback> mObj = (MediaDrmCredentialManager::MediaDrmCredentialManagerCallback*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("MediaDrmCredentialManager", "MediaDrmCredentialManager::MediaDrmCredentialManagerCallback::OnCredentialResetFinished, mObj is NULL");
        return;
    }
    mObj->OnCredentialResetFinished(succeeded);
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
