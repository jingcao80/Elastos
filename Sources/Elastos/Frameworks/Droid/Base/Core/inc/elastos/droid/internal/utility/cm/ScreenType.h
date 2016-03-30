#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class ScreenType
{
public:
    static CARAPI IsPhone(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI IsHybrid(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI IsTablet(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Int32) GetScreenType(
        /* [in] */ IContext* context);

private:
    // Device type reference
    static Int32 sDeviceType;

    // Device types
    const static Int32 DEVICE_PHONE;
    const static Int32 DEVICE_HYBRID;
    const static Int32 DEVICE_TABLET;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__
