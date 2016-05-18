
#ifndef __ELASTOS_DROID_VIEW_CKEYCHARACTERMAPHELPER_H__
#define __ELASTOS_DROID_VIEW_CKEYCHARACTERMAPHELPER_H__

#include "_Elastos_Droid_View_CKeyCharacterMapHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CKeyCharacterMapHelper)
    , public Singleton
    , public IKeyCharacterMapHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Load(
        /* [in] */ Int32 deviceId,
        /* [out] */ IKeyCharacterMap** kcm);

    CARAPI DeviceHasKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* hasKey);

    CARAPI GetDeadChar(
        /* [in] */ Int32 accent,
        /* [in] */ Int32 c,
        /* [out] */ Int32* deadChar);

    CARAPI DeviceHasKeys(
        /* [in] */ ArrayOf<Int32>* keyCodes,
        /* [in] */ ArrayOf<Boolean>** hasKeys);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CKEYCHARACTERMAPHELPER_H__
