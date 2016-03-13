#ifndef __ELASTOS_DROID_TEXT_METHOD_DIALERKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_DIALERKEYLISTENER_H__

#include "elastos/droid/text/method/NumberKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For dialing-only text entry
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class DialerKeyListener
    : public NumberKeyListener
    , public IDialerKeyListener
{
public:
    DialerKeyListener();

    virtual ~DialerKeyListener();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
        /* [out] */ IDialerKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    static CARAPI GetCHARACTERS(
        /* [out, callee] */ ArrayOf<Char32>** ret);

    //override
    CARAPI OnKeyUp(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI ClearMetaKeyState(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 states);

protected:
    //@Override
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetAcceptedChars();

    /**
     * Overrides the superclass's lookup method to prefer the number field
     * from the KeyEvent.
     */
    CARAPI_(Int32) Lookup(
        /* [in] */ IKeyEvent* event,
        /* [in] */ ISpannable* content);

public:
    /**
     * The characters that are used.
     *
     * @see KeyEvent#getMatch
     * @see #getAcceptedChars
     */
    static const AutoPtr<ArrayOf<Char32> > CHARACTERS;

private:
    static AutoPtr<IDialerKeyListener> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_DIALERKEYLISTENER_H__
