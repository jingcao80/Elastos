
#include "elastos/droid/dialer/DialerApplication.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/extensions/ExtensionsFactory.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::Extensions::ExtensionsFactory;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Dialer {

ECode DialerApplication::OnCreate()
{
    Application::OnCreate();
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    ExtensionsFactory::Init(context);
    return NOERROR;
}

ECode DialerApplication::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (ContactPhotoManager::CONTACT_PHOTO_SERVICE.Equals(name)) {
        if (mContactPhotoManager == NULL) {
            mContactPhotoManager = ContactPhotoManager::CreateContactPhotoManager(IContext::Probe(this));
            RegisterComponentCallbacks(IComponentCallbacks::Probe(mContactPhotoManager));
            mContactPhotoManager->PreloadPhotosInBackground();
        }
        *object = TO_IINTERFACE(mContactPhotoManager);
        REFCOUNT_ADD(*object)
        return NOERROR;
    }

    return Application::GetSystemService(name, object);
}

} // Dialer
} // Droid
} // Elastos
