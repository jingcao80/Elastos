#ifndef __ELASTOS_DROID_TEXT_METHOD_QWERTYKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_QWERTYKEYLISTENER_H__

#include "elastos/droid/text/method/BaseKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

class QwertyKeyListener
    : public BaseKeyListener
    , public IQwertyKeyListener
{
protected:
    class Replaced
        : public Object
        , public INoCopySpan
    {
    public:
        Replaced();

        ~Replaced();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ArrayOf<Char32>* text);

    public:
        AutoPtr<ArrayOf<Char32> > mText;
    };

public:
    friend class TextKeyListener;

    CAR_INTERFACE_DECL()

    QwertyKeyListener();

    virtual ~QwertyKeyListener();

    CARAPI constructor(
        /* [in] */ Capitalize cap,
        /* [in] */ Boolean autotext);

    CARAPI constructor(
        /* [in] */ Capitalize cap,
        /* [in] */ Boolean autotext,
        /* [in] */ Boolean fullKeyboard);

    static CARAPI GetInstance(
        /* [in] */ Boolean autoText,
        /* [in] */ Capitalize cap,
        /* [out] */ IQwertyKeyListener** ret);

    static CARAPI GetInstanceForFullKeyboard(
        /* [out] */ IQwertyKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    static CARAPI MarkAsReplaced(
        /* [in] */ ISpannable* content,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const String& original);

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

private:
    CARAPI_(String) GetReplacement(
        /* [in] */ ICharSequence* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IView* view);

    CARAPI_(Boolean) ShowCharacterPicker(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Char32 c,
        /* [in] */ Boolean insert,
        /* [in] */ Int32 count);

    static CARAPI_(String) ToTitleCase(
        /* [in] */ const String& src);

public:
    static HashMap<Char32, String> PICKER_SETS;
    static Boolean sInitPickerSet;

private:
    Capitalize mAutoCap;
    Boolean mAutoText;
    Boolean mFullKeyboard;
    static const Int32 CAPITALIZELENGTH;
    static AutoPtr<ArrayOf<IQwertyKeyListener*> > sInstance;
    static AutoPtr<IQwertyKeyListener> sFullKeyboardInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_METHOD_QWERTYKEYLISTENER_H__
