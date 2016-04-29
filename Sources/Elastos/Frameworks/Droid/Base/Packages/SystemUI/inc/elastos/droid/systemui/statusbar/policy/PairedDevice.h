
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class PairedDevice
    : public Object
    , public IBluetoothControllerPairedDevice
{
public:
    CAR_INTERFACE_DECL();

    PairedDevice();

    CARAPI SetId(
        /* [in] */ const String& id);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    CARAPI GetTag(
        /* [out] */ IInterface** tag);

    CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

public:
    String mId;
    String mName;
    Int32 mState;
    AutoPtr<IInterface> mTag;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
