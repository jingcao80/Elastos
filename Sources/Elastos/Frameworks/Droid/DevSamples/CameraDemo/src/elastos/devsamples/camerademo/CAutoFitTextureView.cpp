
#include "CAutoFitTextureView.h"

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

static const String TAG("CAutoFitTextureView");

CAR_OBJECT_IMPL(CAutoFitTextureView)

CAutoFitTextureView::CAutoFitTextureView()
    : mRatioWidth(0)
    , mRatioHeight(0)
{}

void CAutoFitTextureView::SetAspectRatio(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (width < 0 || height < 0) {
        Logger::E(TAG, "Size cannot be negative.");
        return;
    }
    mRatioWidth = width;
    mRatioHeight = height;
    RequestLayout();
}

ECode CAutoFitTextureView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextureView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 width = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = View::MeasureSpec::GetSize(heightMeasureSpec);
    if (0 == mRatioWidth || 0 == mRatioHeight) {
        SetMeasuredDimension(width, height);
    }
    else {
        if (width < height * mRatioWidth / mRatioHeight) {
            SetMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
        }
        else {
            SetMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
        }
    }
    return NOERROR;
}

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos