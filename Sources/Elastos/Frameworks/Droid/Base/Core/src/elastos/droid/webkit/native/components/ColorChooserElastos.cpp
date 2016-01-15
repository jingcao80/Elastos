
#include "elastos/droid/webkit/native/components/ColorChooserElastos.h"
#include "elastos/droid/webkit/native/components/api/ColorChooserElastos_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Ui::EIID_IOnColorChangedListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
    //mOwner->mDialog->Dismiss();
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

    //mDialog->Dismiss();
    return NOERROR;
}

AutoPtr<ColorChooserElastos> ColorChooserElastos::CreateColorChooserElastos(
    /* [in] */ Int64 nativeColorChooserElastos,
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
    /* [in] */ Int64 nativeColorChooserElastos,
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

    assert(0);
    //mDialog->Show();
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
    /* [in] */ Int64 nativeColorChooserElastos,
    /* [in] */ Int32 color)
{
    Elastos_ColorChooserAndroid_nativeOnColorChosen(THIS_PROBE(IInterface), (Handle32)nativeColorChooserElastos, color);
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
    /* [in] */ Int64 nativeColorChooserElastos,
    /* [in] */ IInterface* contentViewCore,
    /* [in] */ Int32 initialColor,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    ContentViewCore* cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return TO_IINTERFACE(CreateColorChooserElastos(nativeColorChooserElastos, cvc, initialColor, suggestions));
}

} // namespace Components
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

