
#ifndef __ELASTOS_APPS_DIALERBINDER_CDATABASEHELPERMANAGER_H__
#define __ELASTOS_APPS_DIALERBINDER_CDATABASEHELPERMANAGER_H__

#include "_Elastos_Apps_Dialer_CDatabaseHelperManager.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace DialerBinder {

CarClass(CDatabaseHelperManager)
    , public singleton
    , public IDatabaseHelperManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDatabaseHelper(
        /* [in] */ IContext* context,
        /* [out] */ IDialerDatabaseHelper** helper);

    static CARAPI_(AutoPtr<IDialerDatabaseHelper>) GetDatabaseHelper(
        /* [in] */ IContext* context);

};

} // DialerBinder
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBINDER_CDATABASEHELPERMANAGER_H__
