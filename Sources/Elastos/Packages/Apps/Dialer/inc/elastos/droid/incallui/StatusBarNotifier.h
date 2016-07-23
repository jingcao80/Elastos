
#ifndef __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
#define __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class StatusBarNotifier
    : public Object
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    StatusBarNotifier(
        /* [in] */ IContext* context,
        /* [in] */ ContactInfoCache* contactInfoCache);

    /**
     * Creates notifications according to the state we receive from {@link InCallPresenter}.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
