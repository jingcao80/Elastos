#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_QSUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_QSUTILS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class QSUtils
{
public:
    static CARAPI GetAvailableTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    static CARAPI GetDefaultTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    static CARAPI GetDefaultTilesAsString(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    static CARAPI DeviceSupportsLte(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsDdsSupported(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsMobileData(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsBluetooth(
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsNfc(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsFlashLight(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI DeviceSupportsCompass(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(void) FilterTiles(
        /* [in] */ IContext* context,
        /* [in] */ IList* tiles);

    static CARAPI_(void) FilterTiles(
        /* [in] */ IContext* context);

private:
    static Boolean sAvailableTilesFiltered;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_QSUTILS_H__
