#ifndef __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__

#include "elastos/droid/text/method/NumberKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For digits-only text entry
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class DigitsKeyListener
    : public NumberKeyListener
    , public IDigitsKeyListener
{
public:
    /**
     * Allocates a DigitsKeyListener that accepts the digits 0 through 9.
     */
    DigitsKeyListener();

    virtual ~DigitsKeyListener();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Allocates a DigitsKeyListener that accepts the digits 0 through 9,
     * plus the minus sign (only at the beginning) and/or decimal point
     * (only one per field) if specified.
     */
    CARAPI constructor(
        /* [in] */ Boolean sign,
        /* [in] */ Boolean decimal);

    static CARAPI GetInstance(
        /* [out] */ IDigitsKeyListener** ret);

    static CARAPI GetInstance(
        /* [in] */ Boolean sign,
        /* [in] */ Boolean decimal,
        /* [out] */ IDigitsKeyListener** ret);

    static CARAPI GetInstance(
        /* [in] */ const String& accepted,
        /* [out] */ IDigitsKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    //@Override
    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** ret);

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


protected:
    static const Int32 SIGN;// = 1;
    static const Int32 DECIMAL;// = 2;
    /**
     * The characters that are used.
     *
     * @see KeyEvent#getMatch
     * @see #getAcceptedChars
     */
    AutoPtr< ArrayOf<Char32> > mAccepted;

private:
    static CARAPI_(Boolean) IsSignChar(
        /* [in] */ const Char32 c);

    static CARAPI_(Boolean) IsDecimalPointChar(
        /* [in] */ const Char32 c);

private:
    Boolean mSign;
    Boolean mDecimal;

    static const Char32* CHARACTERS[];

    static const Char32 CHARACTERS0[];
    static const Char32 CHARACTERS1[];
    static const Char32 CHARACTERS2[];
    static const Char32 CHARACTERS3[];

    static AutoPtr< ArrayOf< IDigitsKeyListener* > > sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__
