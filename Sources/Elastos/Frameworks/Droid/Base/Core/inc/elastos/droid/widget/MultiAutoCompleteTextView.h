//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
