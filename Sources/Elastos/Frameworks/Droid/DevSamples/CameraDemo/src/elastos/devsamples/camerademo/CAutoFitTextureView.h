
#ifndef __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__
#define __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__

#include "_Elastos_DevSamples_CameraDemo_CAutoFitTextureView.h"
#include <elastos/droid/view/TextureView.h>

using Elastos::Droid::View::TextureView;

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

CarClass(CAutoFitTextureView)
    , public TextureView
{
public:
    CAR_OBJECT_DECL()

    CAutoFitTextureView();

    /**
     * Sets the aspect ratio for this view. The size of the view will be measured based on the ratio
     * calculated from the parameters. Note that the actual sizes of parameters don't matter, that
     * is, calling setAspectRatio(2, 3) and setAspectRatio(4, 6) make the same result.
     *
     * @param width  Relative horizontal size
     * @param height Relative vertical size
     */
    void SetAspectRatio(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    Int32 mRatioWidth;
    Int32 mRatioHeight;
};

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__
