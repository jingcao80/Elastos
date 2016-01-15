
#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTHELPER_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTHELPER_H__

#include "_Elastos_Droid_App_CApplicationErrorReportHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CApplicationErrorReportHelper)
    , public Singleton
    , public IApplicationErrorReportHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI constructor();

    CARAPI GetErrorReportReceiver(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 appFlags,
        /* [out] */ IComponentName** receiver);

    CARAPI GetErrorReportReceiver(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& errorPackage,
        /* [in] */ const String& receiverPackage,
        /* [out] */ IComponentName** receiver);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTHELPER_H__
