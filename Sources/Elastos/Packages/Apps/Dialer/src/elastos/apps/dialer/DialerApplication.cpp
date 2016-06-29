
#include "elastos/apps/dialer/DialerApplication.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Apps {
namespace Dialer {

CAR_INTERFACE_IMPL(DialerApplication, Application, IDialerApplication);

ECode DialerApplication::OnCreate()
{
    Application::OnCreate();
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    assert(0 && "TODO");
    // ExtensionsFactory.init(context);
    return NOERROR;
}

ECode DialerApplication::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    assert(0 && "TODO");
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
} // Apps
} // Elastos
