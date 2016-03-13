#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Egg_CLLandHelper.h"
#include <elastos/core/Singleton.h>
#include "_SystemUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Egg{

CarClass(CLLandHelper)
    , public Singleton
    , public ILLandHelper
{
public:
    CAR_SINGLETON_DECL()
    CAR_INTERFACE_DECL()

    CARAPI L(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<IInterface* >* objects);

    CARAPI Lerp(
        /* [in] */ Float x,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Rlerp(
        /* [in] */ Float v,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Clamp(
        /* [in] */ Float f,
        /* [out] */ Float* res);

    CARAPI Frand(
        /* [out] */ Float* f);

    CARAPI Frand(
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Irand(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b,
        /* [out] */ Int32* f);
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__