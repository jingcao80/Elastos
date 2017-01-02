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

#include "elastos/droid/webkit/webview/chromium/native/components/ColorChooserElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/components/api/ColorChooserElastos_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Ui::EIID_IOnColorChangedListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

//=====================================================================
//           ColorChooserElastos::InnerOnColorChangedListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorChooserElastos::InnerOnColorChangedListener, Object, IOnColorChangedListener)

ColorChooserElastos::InnerOnColorChangedListener::InnerOnColorChangedListener(
    /* [in] */ ColorChooserElastos* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorChooserElastos::InnerOnColorChangedListener::OnColorChanged(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mDialog.dismiss();
    // nativeOnColorChosen(mNativeColorChooserAndroid, color);

    assert(NULL != mOwner);
    assert(NULL != mOwner->mDialog);
    mOwner->mDialog->Dismiss();
    mOwner->NativeOnColorChosen(mOwner->mNativeColorChooserElastos, color);
    return NOERROR;
}

//=====================================================================
//                         ColorChooserElastos
//=====================================================================
ECode ColorChooserElastos::CloseColorChooser()
{
    // ==================before translated======================
    // mDialog.dismiss();

    mDialog->Dismiss();
    return NOERROR;
}

AutoPtr<ColorChooserElastos> ColorChooserElastos::CreateColorChooserElastos(
    /* [in] */ Handle64 nativeColorChooserElastos,
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int32 initialColor,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    // ==================before translated======================
    // ColorChooserAndroid chooser = new ColorChooserAndroid(nativeColorChooserAndroid,
    //     contentViewCore.getContext(), initialColor, suggestions);
    // chooser.openColorChooser();
    // return chooser;

    AutoPtr<IContext> context = contentViewCore->GetContext();
    AutoPtr<ColorChooserElastos> chooser = new ColorChooserElastos(nativeColorChooserElastos,
         context, initialColor, suggestions);
    return chooser;
}

ColorChooserElastos::ColorChooserElastos(
    /* [in] */ Handle64 nativeColorChooserElastos,
    /* [in] */ IContext* context,
    /* [in] */ Int32 initialColor,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    // ==================before translated======================
    // OnColorChangedListener listener = new OnColorChangedListener() {
    //   @Override
    //   public void onColorChanged(int color) {
    //       mDialog.dismiss();
    //       nativeOnColorChosen(mNativeColorChooserAndroid, color);
    //   }
    // };
    //
    // mNativeColorChooserAndroid = nativeColorChooserAndroid;
    // mDialog = new ColorPickerDialog(context, listener, initialColor, suggestions);

    AutoPtr<IOnColorChangedListener> listener = new InnerOnColorChangedListener(this);
    mNativeColorChooserElastos = nativeColorChooserElastos;
    mDialog = new ColorPickerDialog(context, listener, initialColor, suggestions);
}

ECode ColorChooserElastos::OpenColorChooser()
{
    // ==================before translated======================
    // mDialog.show();

    mDialog->Show();
    return NOERROR;
}

AutoPtr< ArrayOf<IInterface*> > ColorChooserElastos::CreateColorSuggestionArray(
    /* [in] */ Int32 size)
{
    // ==================before translated======================
    // return new ColorSuggestion[size];

    AutoPtr< ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(size);
    return result;
}

void ColorChooserElastos::AddToColorSuggestionArray(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 index,
    /* [in] */ Int32 color,
    /* [in] */ const String& label)
{
    //VALIDATE_NOT_NULL(array);
    // ==================before translated======================
    // array[index] = new ColorSuggestion(color, label);

    AutoPtr<ColorSuggestion> cs = new ColorSuggestion(color, label);
    AutoPtr<IInterface> item = TO_IINTERFACE(cs);
    array->Set(index, item);
    //return NOERROR;
}

ECode ColorChooserElastos::NativeOnColorChosen(
    /* [in] */ Handle64 nativeColorChooserElastos,
    /* [in] */ Int32 color)
{
    Elastos_ColorChooserAndroid_nativeOnColorChosen(TO_IINTERFACE(this), nativeColorChooserElastos, color);
    return NOERROR;
}

void ColorChooserElastos::CloseColorChooser(
    /* [in] */ IInterface* obj)
{
    ColorChooserElastos* mObj = (ColorChooserElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ColorChooserElastos", "ColorChooserElastos::CloseColorChooser, mObj is NULL");
        return;
    }
    mObj->CloseColorChooser();
}

AutoPtr<IInterface> ColorChooserElastos::CreateColorChooserElastos(
    /* [in] */ Handle64 nativeColorChooserElastos,
    /* [in] */ IInterface* contentViewCore,
    /* [in] */ Int32 initialColor,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    ContentViewCore* cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return TO_IINTERFACE(CreateColorChooserElastos(nativeColorChooserElastos, cvc, initialColor, suggestions));
}

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

