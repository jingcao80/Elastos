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

#ifndef __ELASTOS_DROID_WIDGET_TEXTSWITCHER_H__
#define __ELASTOS_DROID_WIDGET_TEXTSWITCHER_H__

#include <elastos/droid/widget/ViewSwitcher.h>

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Specialized {@link android.widget.ViewSwitcher} that contains
 * only children of type {@link android.widget.TextView}.
 *
 * A TextSwitcher is useful to animate a label on screen. Whenever
 * {@link #setText(CharSequence)} is called, TextSwitcher animates the current text
 * out and animates the new text in.
 */
class ECO_PUBLIC TextSwitcher
    : public ViewSwitcher
    , public ITextSwitcher
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new empty TextSwitcher.
     *
     * @param context the application's environment
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates a new empty TextSwitcher for the given context and with the
     * specified set attributes.
     *
     * @param context the application environment
     * @param attrs a collection of attributes
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * {@inheritDoc}
     *
     * @throws IllegalArgumentException if child is not an instance of
     *         {@link android.widget.TextView}
     */
    using ViewSwitcher::AddView;

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);
    /**
     * Sets the text of the next view and switches to the next view. This can
     * be used to animate the old text out and animate the next text in.
     *
     * @param text the new text to display
     */
    CARAPI SetText(
        /* [in] */ ICharSequence* text);

    /**
     * Sets the text of the text view that is currently showing.  This does
     * not perform the animations.
     *
     * @param text the new text to display
     */
    CARAPI SetCurrentText(
        /* [in] */ ICharSequence* text);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_TEXTSWITCHER_H__
