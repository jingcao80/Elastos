
#ifndef __ELASTOS_DROID_WIDGET_EDITTEXT_H__
#define __ELASTOS_DROID_WIDGET_EDITTEXT_H__

#include "elastos/droid/R.h"
#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Widget {

/*
 * This is supposed to be a *very* thin veneer over TextView.
 * Do not make any changes here that do anything that a TextView
 * with a key listener and a movement method wouldn't do!
 */

/**
 * EditText is a thin veneer over TextView that configures itself
 * to be editable.
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-formstuff.html">Form Stuff
 * tutorial</a>.</p>
 * <p>
 * <b>XML attributes</b>
 * <p>
 * See {@link android.R.styleable#EditText EditText Attributes},
 * {@link android.R.styleable#TextView TextView Attributes},
 * {@link android.R.styleable#View View Attributes}
 */
class ECO_PUBLIC EditText
    : public TextView
    , public IEditText
{
public:
    CAR_INTERFACE_DECL()

    EditText();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::editTextStyle/*com.android.internal.R.attr.editTextStyle*/,
        /* [in] */ Int32 defStyleRes = 0);

    //@Override
    virtual CARAPI GetText(
        /* [out] */ ICharSequence** text);

    using TextView::SetText;

    //@Override
    virtual CARAPI SetText(
        /* [in] */ ICharSequence* text,
        /* [in] */ BufferType type);

    /**
     * Convenience for {@link Selection#setSelection(Spannable, Int32, Int32)}.
     */
    virtual CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 stop);

    /**
     * Convenience for {@link Selection#setSelection(Spannable, Int32)}.
     */
    virtual CARAPI SetSelection(
        /* [in] */ Int32 index);

    /**
     * Convenience for {@link Selection#selectAll}.
     */
    virtual CARAPI SelectAll();

    /**
     * Convenience for {@link Selection#extendSelection}.
     */
    virtual CARAPI ExtendSelection(
        /* [in] */ Int32 index);

    //@Override
    CARAPI SetEllipsize(
        /* [in] */ TextUtilsTruncateAt ellipsis);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);


protected:

    //@Override
    CARAPI_(Boolean) GetDefaultEditable();

    //@Override
    CARAPI_(AutoPtr<IMovementMethod>) GetDefaultMovementMethod();

private:
    ECO_LOCAL const static String EDITTEXT_NAME;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
