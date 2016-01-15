
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CKeyCharacterMapHelper.h"
#include "elastos/droid/view/KeyCharacterMap.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CKeyCharacterMapHelper, Singleton, IKeyCharacterMapHelper);

CAR_SINGLETON_IMPL(CKeyCharacterMapHelper);

ECode CKeyCharacterMapHelper::Load(
    /* [in] */ Int32 deviceId,
    /* [out] */ IKeyCharacterMap** kcm)
{
    return KeyCharacterMap::Load(deviceId, kcm);
}

ECode CKeyCharacterMapHelper::DeviceHasKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* hasKey)
{
    return KeyCharacterMap::DeviceHasKey(keyCode, hasKey);
}

ECode CKeyCharacterMapHelper::GetDeadChar(
    /* [in] */ Int32 accent,
    /* [in] */ Int32 c,
    /* [out] */ Int32* deadChar)
{
    VALIDATE_NOT_NULL(deadChar);
    *deadChar = KeyCharacterMap::GetDeadChar(accent, c);

    return NOERROR;
}

ECode CKeyCharacterMapHelper::DeviceHasKeys(
    /* [in] */ ArrayOf<Int32>* keyCodes,
    /* [in] */ ArrayOf<Boolean>** hasKeys)
{
    return KeyCharacterMap::DeviceHasKeys(keyCodes, hasKeys);
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
