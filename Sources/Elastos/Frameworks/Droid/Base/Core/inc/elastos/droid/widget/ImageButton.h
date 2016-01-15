
#ifndef __ELASTOS_DROID_WIDGET_IMAGEBUTTON_H__
#define __ELASTOS_DROID_WIDGET_IMAGEBUTTON_H__

#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>
 * Displays a button with an image (instead of text) that can be pressed
 * or clicked by the user. By default, an ImageButton looks like a regular
 * {@link android.widget.Button}, with the standard button background
 * that changes color during different button states. The image on the surface
 * of the button is defined either by the {@code android:src} attribute in the
 * {@code &lt;ImageButton&gt;} XML element or by the
 * {@link #setImageResource(int)} method.</p>
 *
 * <p>To remove the standard button background image, define your own
 * background image or set the background color to be transparent.</p>
 * <p>To indicate the different button states (focused, selected, etc.), you can
 * define a different image for each state. E.g., a blue image by default, an
 * orange one for when focused, and a yellow one for when pressed. An easy way to
 * do this is with an XML drawable "selector." For example:</p>
 * <pre>
 * &lt;?xml version="1.0" encoding="utf-8"?&gt;
 * &lt;selector xmlns:android="http://schemas.android.com/apk/res/android"&gt;
 *     &lt;item android:state_pressed="true"
 *           android:drawable="@drawable/button_pressed" /&gt; &lt;!-- pressed --&gt;
 *     &lt;item android:state_focused="true"
 *           android:drawable="@drawable/button_focused" /&gt; &lt;!-- focused --&gt;
 *     &lt;item android:drawable="@drawable/button_normal" /&gt; &lt;!-- default --&gt;
 * &lt;/selector&gt;</pre>
 *
 * <p>Save the XML file in your project {@code res/drawable/} folder and then
 * reference it as a drawable for the source of your ImageButton (in the
 * {@code android:src} attribute). Android will automatically change the image
 * based on the state of the button and the corresponding images
 * defined in the XML.</p>
 *
 * <p>The order of the {@code &lt;item>} elements is important because they are
 * evaluated in order. This is why the "normal" button image comes last, because
 * it will only be applied after {@code android:state_pressed} and {@code
 * android:state_focused} have both evaluated false.</p>
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-formstuff.html">Form Stuff
 * tutorial</a>.</p>
 *
 * <p><strong>XML attributes</strong></p>
 * <p>
 * See {@link android.R.styleable#ImageView Button Attributes},
 * {@link android.R.styleable#View View Attributes}
 * </p>
 */
class ImageButton
    : public Elastos::Droid::Widget::ImageView
    , public IImageButton
{
public:
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

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //@Override
    CARAPI_(Boolean) OnSetAlpha(
        /* [in] */ Int32 alpha);

public:
    const static String IMAGEBUTTON_NAME;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_IMAGEBUTTON_H__
