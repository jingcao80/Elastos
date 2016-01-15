
#ifndef __ELASTOS_DROID_WIDGET_MULTIAUTOCOMPLETETEXTVIEW_H__
#define __ELASTOS_DROID_WIDGET_MULTIAUTOCOMPLETETEXTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AutoCompleteTextView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;

namespace Elastos {
namespace Droid {
namespace Widget {

class MultiAutoCompleteTextView
    : public AutoCompleteTextView
    , public IMultiAutoCompleteTextView
{
public:
    class CommaTokenizer
        : public ITokenizer
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        CommaTokenizer();

        CARAPI FindTokenStart(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 cursor,
            /* [out] */ Int32* start);

        CARAPI FindTokenEnd(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 cursor,
            /* [out] */ Int32* end);

        CARAPI TerminateToken(
            /* [in] */ ICharSequence* text,
            /* [out] */ ICharSequence** res);
    };

public:
    CAR_INTERFACE_DECL()

    MultiAutoCompleteTextView();

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

    virtual CARAPI SetTokenizer(
        /* [in] */ ITokenizer* t);

    CARAPI EnoughToFilter(
        /* [out] */ Boolean* enough);

    CARAPI PerformValidation();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    CARAPI_(void) PerformFiltering(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 keyCode);

    CARAPI_(void) PerformFiltering(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 keyCode);

    CARAPI_(void) ReplaceText(
        /* [in] */ ICharSequence* text);

    CARAPI_(void) FinishInit();

private:
    AutoPtr<ITokenizer> mTokenizer;
    static const String MULTIAUTOCOMPLETETEXTVIEW_NAME;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif
