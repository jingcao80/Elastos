#ifndef __ELASTOS_APPS_DIALER_CDIALTACTSACTIVITYHELPER_H__
#define __ELASTOS_APPS_DIALER_CDIALTACTSACTIVITYHELPER_H__

#include "_Elastos_Apps_Dialer_CDialtactsActivityHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer{

CarClass(CDialtactsActivityHelper)
    , public Singleton
    , public IDialtactsActivityHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Returns an Intent to launch Call Settings screen */
    CARAPI GetCallSettingsIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetAddNumberToContactIntent(
        /* [in] */ICharSequence* text,
        /* [out] */ IIntent** intent);
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CDIALTACTSACTIVITYHELPER_H__
