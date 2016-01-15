#ifndef __CELESTIAL_H__
#define __CELESTIAL_H__

#include "ShaderUtil.h"
#include <elastos/core/Math.h>

using Elastos::IO::IFloatBuffer;
using Elastos::Droid::Opengl::IGLSurfaceView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class Celestial : public ElRefBase
{
public:
    Celestial(
        /* [in] */ Float scale,
        /* [in] */ Float yAngle,
        /* [in] */ Int32 vCount,
        /* [in] */ IGLSurfaceView* mv);

    CARAPI InitVertexData();

    CARAPI InitShader(
        /* [in] */ IGLSurfaceView* mv);

    CARAPI DrawSelf();

    Float UNIT_SIZE;
    AutoPtr<IFloatBuffer> mVertexBuffer;
    Int32 mVCount;
    Float mYAngle;
    Float mScale;
    String mVertexShader;
    String mFragmentShader;
    Int32 mProgram;
    Int32 muMVPMatrixHandle;
    Int32 maPositionHandle;
    Int32 uPointSizeHandle;
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
#endif