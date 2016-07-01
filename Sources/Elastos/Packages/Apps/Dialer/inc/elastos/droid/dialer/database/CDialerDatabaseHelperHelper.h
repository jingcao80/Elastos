#ifndef __ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPERHELPER_H__
#define __ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPERHELPER_H__

#include "_Elastos_Droid_Dialer_Database_CDialerDatabaseHelperHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Database {

CarClass(CDialerDatabaseHelperHelper)
    , public Singleton
    , public IDialerDatabaseHelperHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Access function to get the singleton instance of DialerDatabaseHelper.
     */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IDialerDatabaseHelper** helper);

};

} // Database
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPERHELPER_H__
