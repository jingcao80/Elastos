
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CKeyguardIndicationTextView.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view to show hints on Keyguard ("Swipe up to unlock", "Tap again to open").
 */
CarClass(CKeyguardIndicationTextView)
    , public TextView
    , public IKeyguardIndicationTextView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Changes the text with an animation and makes sure a single indication is shown long enough.
     *
     * @param text The text to show.
     */
    CARAPI SwitchIndication(
        /* [in] */ ICharSequence* text);

    /**
     * See {@link #switchIndication}.
     */
    CARAPI SwitchIndication(
        /* [in] */ Int32 textResId);
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
