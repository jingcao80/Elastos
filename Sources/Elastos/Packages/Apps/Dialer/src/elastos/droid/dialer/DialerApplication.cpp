
#include "elastos/droid/dialer/DialerApplication.h"
#include "Elastos.Droid.Content.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {

ECode DialerApplication::OnCreate()
{
    Application::OnCreate();
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    // TODO:
    // ExtensionsFactory.init(context);
    return NOERROR;
}

ECode DialerApplication::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    // TODO:
    Logger::E("CDialerApplication", "TODO: get ContactPhotoManager service");
    // if (IContactPhotoManager::CONTACT_PHOTO_SERVICE.Equals(name)) {
    //     if (mContactPhotoManager == NULL) {
    //         assert(0 && "TODO");
    //         // mContactPhotoManager = ContactPhotoManager::CreateContactPhotoManager(this);
    //         RegisterComponentCallbacks(mContactPhotoManager);
    //         mContactPhotoManager->PreloadPhotosInBackground();
    //     }
    //     *object = mContactPhotoManager;
    //     REFCOUNT_ADD(*object);
    //     return NOERROR;
    // }

    return Application::GetSystemService(name, object);
}

} // Dialer
} // Droid
} // Elastos
