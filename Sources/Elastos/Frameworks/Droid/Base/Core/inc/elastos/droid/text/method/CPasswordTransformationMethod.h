#ifndef __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHOD_H__

#include "_Elastos_Droid_Text_Method_CPasswordTransformationMethod.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Text::IGetChars;
using Elastos::Droid::Text::Style::IUpdateLayout;
using Elastos::Droid::Text::Style::IUpdateAppearance;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CPasswordTransformationMethod)
    , public Object
    , public IPasswordTransformationMethod
    , public ITransformationMethod
    , public ITextWatcher
    , public INoCopySpan
{
private:
    //static
    class PasswordCharSequence
        : public Object
        , public IGetChars
        , public ICharSequence
    {
    public:
        CAR_INTERFACE_DECL()

        PasswordCharSequence();

        CARAPI constructor(
            /* [in] */ ICharSequence* source);

        CARAPI GetLength(
            /* [out] */ Int32* number);

        CARAPI GetCharAt(
            /* [in] */ Int32 i,
            /* [out] */ Char32* c);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** csq);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetChars(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ ArrayOf<Char32>* dest,
            /* [in] */ Int32 off);

    private:
        AutoPtr<ICharSequence> mSource;
    };

    //static
    class Visible
        : public Handler
        , public IUpdateLayout
        , public IUpdateAppearance
        , public IRunnable
        , public IPasswordTransformationMethodVisible
    {
        friend class PasswordCharSequence;

    public:
        Visible();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISpannable* sp,
            /* [in] */ IPasswordTransformationMethod* ptm);

        // CARAPI HandleMessage(
        //     /* [in] */ IMessage* msg);

        CARAPI Run();

    private:
        AutoPtr<ISpannable> mText;
        AutoPtr<IPasswordTransformationMethod> mTransformer;
    };

    /**
     * Used to stash a reference back to the View in the Editable so we
     * can use it to check the settings.
     */
    //private static
    class ViewReference
        : public Object
        , public IWeakReference
        , public INoCopySpan
    {
    public:
        ViewReference();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IView* v);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPasswordTransformationMethod();

    virtual ~CPasswordTransformationMethod();

    CARAPI constructor();

    CARAPI GetTransformation(
        /* [in] */ ICharSequence* source,
        /* [in] */ IView* view,
        /* [out] */ ICharSequence** csq);

    static CARAPI GetInstance(
        /* [out] */ IPasswordTransformationMethod** ret);

    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    CARAPI OnFocusChanged(
        /* [in] */ IView* view,
        /* [in] */ ICharSequence* sourceText,
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

private:
    static CARAPI_(void) RemoveVisibleSpans(
        /* [in] */ ISpannable* sp);

private:
    static AutoPtr<IPasswordTransformationMethod> sInstance;
    static Char32 DOT;// = '\u2022';
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHOD_H__
