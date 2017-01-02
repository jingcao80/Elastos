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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_QSDUALTILELABEL_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_QSDUALTILELABEL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/widget/LinearLayout.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/**
 * Text displayed over one or two lines, centered horizontally.  A caret is always drawn at the end
 * of the first line, and considered part of the content for centering purposes.
 *
 * Text overflow rules:
 *   First line: break on a word, unless a single word takes up the entire line - in which case
 *               truncate.
 *   Second line: ellipsis if necessary
 */
class QSDualTileLabel
    : public LinearLayout
    , public IQSDualTileLabel
{
private:
    class UpdateTextRunnable: public Runnable
    {
    public:
        UpdateTextRunnable(
            /* [in] */ QSDualTileLabel* host);

        // @Override
        CARAPI Run();

    private:
        QSDualTileLabel* mHost;
    };

    class OnLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnLayoutChangeListener(
            /* [in] */ QSDualTileLabel* host);

        // @Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        QSDualTileLabel* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    QSDualTileLabel(
        /* [in] */ IContext* context);

    CARAPI SetFirstLineCaret(
        /* [in] */ IDrawable* d);

    CARAPI SetText(
        /* [in] */ ICharSequence* text);

    CARAPI GetText(
        /* [out] */ String* text);

    CARAPI SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);

    CARAPI SetTextColor(
        /* [in] */ Int32 color);

    CARAPI SetTypeface(
        /* [in] */ ITypeface* tf);

private:
    static CARAPI_(AutoPtr<IViewGroupLayoutParams>) NewLinearLayoutParams();

    CARAPI_(AutoPtr<ITextView>) InitTextView();

    CARAPI_(void) RescheduleUpdateText();

    CARAPI_(void) UpdateText();

private:
    AutoPtr<ITextView> mFirstLine;
    AutoPtr<IImageView> mFirstLineCaret;
    AutoPtr<ITextView> mSecondLine;
    Int32 mHorizontalPaddingPx;

    String mText;
    AutoPtr<IRunnable> mUpdateText;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_QSDUALTILELABEL_H__
