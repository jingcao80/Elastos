
#ifndef __ELASTOS_DROID_PREFERENCE_SEEKBARDIALOGPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_SEEKBARDIALOGPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/DialogPreference.h"

using Elastos::Droid::Widget::ISeekBar;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * @hide
 */
class SeekBarDialogPreference
    : public DialogPreference
    , public ISeekBarDialogPreference
{
public:
    CAR_INTERFACE_DECL()

    SeekBarDialogPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    // Allow subclasses to override the action buttons
    virtual CARAPI CreateActionButtons();

    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    static CARAPI GetSeekBar(
        /* [in] */ IView* dialogView,
        /* [out] */ ISeekBar** bar);

private:
    static const String TAG;
    AutoPtr<IDrawable> mMyIcon;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_SEEKBARDIALOGPREFERENCE_H__
