#ifndef __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__
#define __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__

#include "_Elastos_Droid_Dialer_CDialtactsActivityHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CDialtactsActivityHelper)
    , public Singleton
    , public IDialtactsActivityHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /** Returns an Intent to launch Call Settings screen */
    CARAPI GetCallSettingsIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetAddNumberToContactIntent(
        /* [in] */ICharSequence* text,
        /* [out] */ IIntent** intent);
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__
