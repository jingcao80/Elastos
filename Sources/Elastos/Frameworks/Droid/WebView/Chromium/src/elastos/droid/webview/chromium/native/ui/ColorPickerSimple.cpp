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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerSimple.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorSuggestion.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/R_Ui.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::R;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::ColorSuggestion;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::EIID_IOnColorSuggestionClickListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

//=====================================================================
//                          ColorPickerSimple
//=====================================================================
AutoPtr< ArrayOf<Int32> > ColorPickerSimple::DEFAULT_COLORS = ColorPickerSimple::MiddleInitDefaultColors();
AutoPtr< ArrayOf<Int32> > ColorPickerSimple::DEFAULT_COLOR_LABEL_IDS = ColorPickerSimple::MiddleInitDefaultColorLabelIds();

CAR_INTERFACE_IMPL(ColorPickerSimple::InnerListener, Object, IOnColorSuggestionClickListener)

ColorPickerSimple::InnerListener::InnerListener(
    /* [in] */ ColorPickerSimple* host)
    : mHost(host)
{
}

ECode ColorPickerSimple::InnerListener::OnColorSuggestionClick(
    /* [in] */ IInterface* suggestion)
{
    return mHost->OnColorSuggestionClick(suggestion);
}


ECode ColorPickerSimple::constructor(
    /* [in] */ IContext* context)
{
    return ListView::constructor(context);
}

ECode ColorPickerSimple::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ListView::constructor(context, attrs);
}

ECode ColorPickerSimple::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ListView::constructor(context, attrs, defStyle);
}

// suggestion type is ColorSuggestion
ECode ColorPickerSimple::Init(
    /* [in] */ ArrayOf<IInterface*>* suggestions,
    /* [in] */ IOnColorChangedListener* onColorChangedListener)
{
    VALIDATE_NOT_NULL(onColorChangedListener);

    mOnColorChangedListener = onColorChangedListener;
    if (NULL == suggestions) {
        //tye is ColorSuggestion
        suggestions = ArrayOf<IInterface*>::Alloc(DEFAULT_COLORS->GetLength());
        String idsTemp;
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        for (Int32 i = 0; i < suggestions->GetLength(); ++i) {
            context->GetString((*DEFAULT_COLOR_LABEL_IDS)[i], &idsTemp);
            const String kk("kk");
            suggestions->Set(i, TO_IINTERFACE(new ColorSuggestion((*DEFAULT_COLORS)[i], kk)));
        }
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ColorSuggestionListAdapter> adapter = new ColorSuggestionListAdapter(context, suggestions);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    adapter->SetOnColorSuggestionClickListener(listener);
    SetAdapter(adapter);
    return NOERROR;
}

ECode ColorPickerSimple::OnColorSuggestionClick(
    /* [in] */ IInterface* suggestion)
{
    VALIDATE_NOT_NULL(suggestion);

    IObject* objTmp = IObject::Probe(suggestion);
    ColorSuggestion* suggestionTmp = (ColorSuggestion*)objTmp;
    mOnColorChangedListener->OnColorChanged(suggestionTmp->mColor);
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > ColorPickerSimple::MiddleInitDefaultColors()
{
    Int32 tmps[] = {
        IColor::RED,
        IColor::CYAN,
        IColor::BLUE,
        IColor::GREEN,
        IColor::MAGENTA,
        IColor::YELLOW,
        IColor::BLACK,
        IColor::WHITE
    };

    Int32 count = sizeof(tmps) / sizeof(tmps[0]);
    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(count);
    for (Int32 i=0; i<count; ++i) {
        result->Set(i, tmps[i]);
    }

    return result;
}

AutoPtr< ArrayOf<Int32> > ColorPickerSimple::MiddleInitDefaultColorLabelIds()
{
    Int32 tmps[] = {
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_red,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_cyan,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_blue,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_green,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_magenta,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_yellow,
        Elastos::Droid::Webkit::Webview::Chromium::Ui::R::string::color_picker_button_black,
        R::string::color_picker_button_white
    };

    Int32 count = sizeof(tmps) / sizeof(tmps[0]);
    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(count);
    for (Int32 i=0; i<count; ++i) {
        result->Set(i, tmps[i]);
    }

    return result;
}

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


