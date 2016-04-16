#ifndef __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CLauncherProviderHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherProviderHelper)
    , public Singleton
    , public ILauncherProviderHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_APPWIDGET_RESET_URI(
        /* [out] */ IUri** uri);

    /**
     * Build a query string that will match any row where the column matches
     * anything in the values list.
     */
    CARAPI BuildOrWhereString(
        /* [in] */ const String& column,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ String* result);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__