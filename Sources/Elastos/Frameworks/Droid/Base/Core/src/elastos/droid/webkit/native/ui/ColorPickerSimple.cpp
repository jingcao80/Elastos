
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/native/ui/ColorPickerSimple.h"
#include "elastos/droid/webkit/native/ui/ColorSuggestion.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Webkit::Ui::ColorSuggestion;
using Elastos::Droid::Webkit::Ui::EIID_IOnColorSuggestionClickListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

//=====================================================================
//                          ColorPickerSimple
//=====================================================================
AutoPtr< ArrayOf<Int32> > ColorPickerSimple::DEFAULT_COLORS = ColorPickerSimple::MiddleInitDefaultColors();
AutoPtr< ArrayOf<Int32> > ColorPickerSimple::DEFAULT_COLOR_LABEL_IDS = ColorPickerSimple::MiddleInitDefaultColorLabelIds();

CAR_INTERFACE_IMPL(ColorPickerSimple, ListView, IOnColorSuggestionClickListener)

ColorPickerSimple::ColorPickerSimple(
    /* [in] */ IContext* context)
    : mOnColorChangedListener(NULL)
{
    // ==================before translated======================
    // super(context);

    ListView::constructor(context);
}

ColorPickerSimple::ColorPickerSimple(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mOnColorChangedListener(NULL)
{
    // ==================before translated======================
    // super(context, attrs);

    ListView::constructor(context, attrs);
}

ColorPickerSimple::ColorPickerSimple(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mOnColorChangedListener(NULL)
{
    // ==================before translated======================
    // super(context, attrs, defStyle);

    ListView::constructor(context, attrs, defStyle);
}

// suggestion type is ColorSuggestion
ECode ColorPickerSimple::Init(
    /* [in] */ ArrayOf<IInterface*>* suggestions,
    /* [in] */ IOnColorChangedListener* onColorChangedListener)
{
    VALIDATE_NOT_NULL(onColorChangedListener);
    // ==================before translated======================
    // mOnColorChangedListener = onColorChangedListener;
    //
    // if (suggestions == null) {
    //     suggestions = new ColorSuggestion[DEFAULT_COLORS.length];
    //     for (int i = 0; i < suggestions.length; ++i) {
    //         suggestions[i] = new ColorSuggestion(DEFAULT_COLORS[i],
    //                 getContext().getString(DEFAULT_COLOR_LABEL_IDS[i]));
    //     }
    // }
    //
    // ColorSuggestionListAdapter adapter = new ColorSuggestionListAdapter(
    //         getContext(), suggestions);
    // adapter.setOnColorSuggestionClickListener(this);
    // setAdapter(adapter);

    assert(0);
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
    adapter->SetOnColorSuggestionClickListener(this);
    SetAdapter(adapter);
    return NOERROR;
}

ECode ColorPickerSimple::OnColorSuggestionClick(
    /* [in] */ IInterface* suggestion)
{
    VALIDATE_NOT_NULL(suggestion);
    // ==================before translated======================
    // mOnColorChangedListener.onColorChanged(suggestion.mColor);

    IObject* objTmp = IObject::Probe(suggestion);
    ColorSuggestion* suggestionTmp = (ColorSuggestion*)objTmp;
    mOnColorChangedListener->OnColorChanged(suggestionTmp->mColor);
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > ColorPickerSimple::MiddleInitDefaultColors()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // Color.RED,
    //          Color.CYAN,
    //          Color.BLUE,
    //          Color.GREEN,
    //          Color.MAGENTA,
    //          Color.YELLOW,
    //          Color.BLACK,
    //          Color.WHITE
    // ->WWZ_SIGN: ARRAY_INIT_END }

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
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // R.string.color_picker_button_red,
    //          R.string.color_picker_button_cyan,
    //          R.string.color_picker_button_blue,
    //          R.string.color_picker_button_green,
    //          R.string.color_picker_button_magenta,
    //          R.string.color_picker_button_yellow,
    //          R.string.color_picker_button_black,
    //          R.string.color_picker_button_white
    // ->WWZ_SIGN: ARRAY_INIT_END }

    Int32 tmps[] = {
        -1/*R::string::color_picker_button_red*/,
        -1/*R::string::color_picker_button_cyan*/,
        -1/*R::string::color_picker_button_blue*/,
        -1/*R::string::color_picker_button_green*/,
        -1/*R::string::color_picker_button_magenta*/,
        -1/*R::string::color_picker_button_yellow*/,
        -1/*R::string::color_picker_button_black*/,
        -1/*R::string::color_picker_button_white*/
    };

    Int32 count = sizeof(tmps) / sizeof(tmps[0]);
    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(count);
    for (Int32 i=0; i<count; ++i) {
        result->Set(i, tmps[i]);
    }

    return result;
}

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


