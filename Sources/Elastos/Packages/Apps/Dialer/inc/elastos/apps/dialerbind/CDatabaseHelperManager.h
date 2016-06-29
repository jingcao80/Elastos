
#ifndef __ELASTOS_APPS_DIALERBIND_CDATABASEHELPERMANAGER_H__
#define __ELASTOS_APPS_DIALERBIND_CDATABASEHELPERMANAGER_H__

#include "_Elastos_Apps_DialerBind_CDatabaseHelperManager.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Apps::Dialer::Database::IDialerDatabaseHelper;

namespace Elastos {
namespace Apps {
namespace DialerBind {

CarClass(CDatabaseHelperManager)
    , public Singleton
    , public IDatabaseHelperManager
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetDatabaseHelper(
        /* [in] */ IContext* context,
        /* [out] */ IDialerDatabaseHelper** helper);

    static CARAPI_(AutoPtr<IDialerDatabaseHelper>) GetDatabaseHelper(
        /* [in] */ IContext* context);

};

} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_CDATABASEHELPERMANAGER_H__
