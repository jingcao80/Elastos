#ifndef __ELASTOS_DROID_APP_CPACKAGEINSTALLOBSERVER2_H__
#define __ELASTOS_DROID_APP_CPACKAGEINSTALLOBSERVER2_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CPackageInstallObserver2.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver2;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CPackageInstallObserver2)
    , public Object
    , public IIPackageInstallObserver2
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IPackageInstallObserver* host);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageInstalled(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg,
        /* [in] */ IBundle* extras);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWeakReference> mWeakHost;
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_APP_CPACKAGEINSTALLOBSERVER2_H__
