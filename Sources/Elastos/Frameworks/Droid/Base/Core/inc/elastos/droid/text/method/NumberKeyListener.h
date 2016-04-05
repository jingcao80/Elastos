#ifndef __ELASTOS_DROID_TEXT_METHOD_NUMBERKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_NUMBERKEYLISTENER_H__

#include "elastos/droid/text/method/BaseKeyListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For numeric text entry
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
//public abstract
class NumberKeyListener
    : public BaseKeyListener
    , public IInputFilter
    , public INumberKeyListener
{
public:
    CAR_INTERFACE_DECL()

    NumberKeyListener();

    virtual ~NumberKeyListener();

    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** ret);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

protected:
    /**
     * You can say which characters you can accept.
     */
    virtual CARAPI_(AutoPtr< ArrayOf<Char32> >) GetAcceptedChars() = 0;

    CARAPI_(Int32) Lookup(
        /* [in] */ IKeyEvent* event,
        /* [in] */ ISpannable* content);

    static CARAPI_(Boolean) Ok(
        /* [in] */ ArrayOf<Char32>* accept,
        /* [in] */ Char32 c);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_NUMBERKEYLISTENER_H__
