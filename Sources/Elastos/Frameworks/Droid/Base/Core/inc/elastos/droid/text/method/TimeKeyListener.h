#ifndef __ELASTOS_DROID_TEXT_METHOD_TIMEKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_TIMEKEYLISTENER_H__

#include "elastos/droid/text/method/NumberKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For entering times in a text field.
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class TimeKeyListener
    : public NumberKeyListener
    , public ITimeKeyListener
{
public:
    TimeKeyListener();

    virtual ~TimeKeyListener();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ ITimeKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    static CARAPI GetCHARACTERS(
        /* [out] */ ArrayOf<Char32>** ret);

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

public:
    /**
     * The characters that are used.
     *
     * @see KeyEvent#getMatch
     * @see #getAcceptedChars
     */
    static const AutoPtr<ArrayOf<Char32> > CHARACTERS;

private:
    static AutoPtr<ITimeKeyListener> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_TIMEKEYLISTENER_H__
