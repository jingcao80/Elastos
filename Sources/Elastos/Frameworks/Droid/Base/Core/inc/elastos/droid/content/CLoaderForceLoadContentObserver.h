
#ifndef __ELASTOS_DROID_CONTENT_CLOADERFORCELOADCONTENTOBSERVER_H__
#define __ELASTOS_DROID_CONTENT_CLOADERFORCELOADCONTENTOBSERVER_H__

#include "_Elastos_Droid_Content_CLoaderForceLoadContentObserver.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Database::ContentObserver;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * An implementation of a ContentObserver that takes care of connecting
 * it to the Loader to have the loader re-load its data when the observer
 * is told it has changed.  You do not normally need to use this yourself;
 * it is used for you by {@link CursorLoader} to take care of executing
 * an update when the cursor's backing data changes.
 */
CarClass(CLoaderForceLoadContentObserver)
    , public ContentObserver
    , public ILoaderForceLoadContentObserver
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ILoader* loader);

    CARAPI DeliverSelfNotifications(
        /* [out] */ Boolean* result);

    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

private:
    AutoPtr<IWeakReference> mWeakLoader;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CLOADERFORCELOADCONTENTOBSERVER_H__
