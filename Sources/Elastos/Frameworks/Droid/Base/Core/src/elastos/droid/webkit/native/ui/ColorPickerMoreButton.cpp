
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/webkit/native/ui/ColorPickerMoreButton.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PaintStyle_STROKE;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

//=====================================================================
//                        ColorPickerMoreButton
//=====================================================================
ColorPickerMoreButton::ColorPickerMoreButton(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mBorderPaint(NULL)
{
    // ==================before translated======================
    // super(context, attrs);
    // init();

    Button::constructor(context, attrs);
    Init();
}

ColorPickerMoreButton::ColorPickerMoreButton(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mBorderPaint(NULL)
{
    // ==================before translated======================
    // super(context, attrs, defStyle);
    // init();

    Button::constructor(context, attrs, defStyle);
    Init();
}

ECode ColorPickerMoreButton::Init()
{
    // ==================before translated======================
    // mBorderPaint = new Paint();
    // mBorderPaint.setStyle(Paint.Style.STROKE);
    // mBorderPaint.setColor(Color.WHITE);
    // // Set the width to one pixel.
    // mBorderPaint.setStrokeWidth(1.0f);
    // // And make sure the border doesn't bleed into the outside.
    // mBorderPaint.setAntiAlias(false);

    CPaint::New((IPaint**)&mBorderPaint);
    mBorderPaint->SetStyle(PaintStyle_STROKE);
    mBorderPaint->SetColor(IColor::WHITE);
    // Set the width to one pixel.
    mBorderPaint->SetStrokeWidth(1.0f);
    // And make sure the border doesn't bleed into the outside.
    mBorderPaint->SetAntiAlias(FALSE);
    return NOERROR;
}

void ColorPickerMoreButton::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    // ==================before translated======================
    // canvas.drawRect(0.5f, 0.5f, getWidth() - 1.5f, getHeight() - 1.5f, mBorderPaint);
    // super.onDraw(canvas);

    Int32 width = 0, height = 0;
    GetWidth(&width);
    GetHeight(&height);

    canvas->DrawRect(0.5f, 0.5f, width - 1.5f, height - 1.5f, mBorderPaint);
    Button::OnDraw(canvas);
}

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


