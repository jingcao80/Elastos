#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CSmartDialNameMatcherHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialNameMatcherHelper)
    , public Singleton
    , public ISmartDialNameMatcherHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @return Phone number consisting of digits from 0-9
     */
    CARAPI NormalizeNumber(
        /* [in] */ const String& number,
        /* [in] */ ISmartDialMap* map,
        /* [out] */ String* result);

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @param offset Offset to start from
     * @return Phone number consisting of digits from 0-9
     */
    CARAPI NormalizeNumber(
        /* [in] */ String number,
        /* [in] */ Int32 offset,
        /* [in] */ ISmartDialMap* map,
        /* [out] */ String* result);

    CARAPI GetLATIN_SMART_DIAL_MAP(
        /* [out] */ ISmartDialMap** map);
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__
