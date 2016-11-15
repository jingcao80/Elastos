#include "CNodeListener.h"

#define JSActCarClassHead EE(JSPkgName,JSActName)

#include JSActCarClassHead

#include <elastos/droid/widget/EditText.h>

using Elastos::Droid::Widget::EditText;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

JSCarClass(JSActName)
    , public EditText
    , public ICalculatorEditText
{
public:
    CAR_INTERFACE_DECL()
    CAR_OBJECT_DECL()

private:
    class SuperObject
        : public Object
        , public ICalculatorEditTextSuperObject
    {
    public:
        SuperObject(
            /* [in] */ JSActName* host)
            : mHost(host)
        {};

        CAR_INTERFACE_DECL()

        CARAPI SetTextSize(
            /* [in] */ Int32 unit,
            /* [in] */ Float size);

    private:
        JSActName* mHost;
    };

public:
    CCalculatorEditText();

    virtual ~CCalculatorEditText();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI _constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI _OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI _OnSaveInstanceState(
        /* [out] */ IParcelable** result);
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** result);

    CARAPI _SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);
    CARAPI SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);

    CARAPI _GetCompoundPaddingTop(
        /* [out] */ Int32* result);
    CARAPI GetCompoundPaddingTop(
        /* [out] */ Int32* result);

    CARAPI _GetCompoundPaddingBottom(
        /* [out] */ Int32* result);
    CARAPI GetCompoundPaddingBottom(
        /* [out] */ Int32* result);

protected:
    CARAPI _OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI _OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

public:
    CARAPI GetSuperObject(
        /* [out] */ ICalculatorEditTextSuperObject** ppSuperObject);

private:
        AutoPtr<ICalculatorEditTextListener> mListener;
        AutoPtr<ICalculatorEditTextSuperObject> mSuperObject;
};

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos