#ifndef __VISUALIZERVIEW_H__
#define __VISUALIZERVIEW_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AudioFxDemo {

class VisualizerView
{
public:
    VisualizerView(
        /* [in] */ IContext* context);

    CARAPI UpdateVisualizer(
        /* [in] */ const ArrayOf<Byte>& bytes);

protected:
    //@Override
    CARAPI OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) Init();

private:
    AutoPtr<ArrayOf<Byte> > mBytes;
    AutoPtr<ArrayOf<Float> > mPoints;
    AutoPtr<IRect> mRect;

    AutoPtr<IPaint> mForePaint;
};

} // namespace AudioFxDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __VISUALIZERVIEW_H__