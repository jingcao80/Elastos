#ifndef __ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPERHELPER_H__
#define __ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPERHELPER_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Database {

CarClass(CDialerDatabaseHelperHelper)
    , public Singleton
    , public IDialerDatabaseHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Access function to get the singleton instance of DialerDatabaseHelper.
     */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IDialerDatabaseHelper** helper);

};

} // Database
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPERHELPER_H__
