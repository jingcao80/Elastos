#include "VisualizerView.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AudioFxDemo {

VisualizerView::VisualizerView(
    /* [in] */ IContext* context)
{
    CRect::New((IRect**)&mRect);
    CPaint::New((IPaint**)&mForePaint);
    Init();
}

ECode VisualizerView::UpdateVisualizer(
    /* [in] */ const ArrayOf<Byte>& bytes)
{
    mBytes = &bytes;
    //Invalidate();
    return NOERROR;
}

ECode VisualizerView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    // View::OnDraw(canvas);

    //mBytes就是采集来的数据 这里是个大小为1024的数组，里面的数据都是byts类型，所以大小为-127到12
    if (mBytes == NULL) {
        return NOERROR;
    }

    if (mPoints == NULL || mPoints->GetLength() < mBytes->GetLength() * 4) {
        //mPoints主要用来存储要画直线的4个坐标（每个点两个坐标，所以一条直线需要两个点，也就是4个坐标）
        mPoints = ArrayOf<Float>::Alloc(mBytes->GetLength() * 4);
    }

    mRect->Set(0, 0, GetWidth(), GetHeight());

    Int32 height;
    mRect->GetHeight(&height);
    Int32 width;
    mRect->GetWidth(&width);

    for (Int32 i = 0; i < mBytes->GetLength() - 1; i++) {
        (*mPoints)[i * 4] = width * i / (mBytes->GetLength() - 1);
        (*mPoints)[i * 4 + 1] = height / 2
                + ((Byte) ((*mBytes)[i] + 128)) * (height / 2) / 128;
        (*mPoints)[i * 4 + 2] = width * (i + 1) / (mBytes->GetLength() - 1);
        (*mPoints)[i * 4 + 3] = height / 2
                + ((Byte) ((*mBytes)[i + 1] + 128)) * (height / 2) / 128;
    }
    //循环结束后，就得到了这一次波形的所有刻画坐标，直接画在画布上就好了
    // canvas->DrawLines(mPoints, mForePaint);

    return canvas->DrawLines(mPoints, mForePaint);
}

void VisualizerView::Init()
{
    mBytes = NULL;

    mForePaint->SetStrokeWidth(1.0f);
    mForePaint->SetAntiAlias(TRUE);
    AutoPtr<IColor> c;
    CColor::AcquireSingleton((IColor**)&c);
    Int32 rgb;
    c->Rgb(0, 128, 255, &rgb);
    mForePaint->SetColor(rgb);
}

} // namespace AudioFxDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos