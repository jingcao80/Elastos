
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTDEVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTDEVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CastDevice
    : public Object
    , public ICastControllerCastDevice
{
public:
    CAR_INTERFACE_DECL()

    CastDevice();

    CARAPI SetId(
        /* [in] */ const String& value);

    CARAPI GetId(
        /* [out] */ String* value);

    CARAPI SetName(
        /* [in] */ const String& value);

    CARAPI GetName(
        /* [out] */ String* value);

    CARAPI SetDescription(
        /* [in] */ const String& value);

    CARAPI GetDescription(
        /* [out] */ String* value);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    CARAPI GetTag(
        /* [out] */ IInterface** tag);

public:
    String mId;
    String mName;
    String mDescription;
    Int32 mState;
    AutoPtr<IInterface> mTag;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTDEVICE_H__
