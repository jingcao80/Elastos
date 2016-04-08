#ifndef __MATRIXSTATE_H__
#define __MATRIXSTATE_H__

#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/Stack.h"

using Elastos::Utility::Stack;
using Elastos::IO::IFloatBuffer;
using Elastos::Droid::Opengl::IMatrix;
using Elastos::Droid::Opengl::CMatrix;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class MatrixState
{
public:
    static CARAPI SetInitStack();

    static CARAPI PushMatrix();

    static CARAPI PopMatrix();

    static CARAPI Transtate(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    static CARAPI Rotate(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    static CARAPI SetCamera(
         /* [in] */ Float cx,
         /* [in] */ Float cy,
         /* [in] */ Float cz,
         /* [in] */ Float tx,
         /* [in] */ Float ty,
         /* [in] */ Float tz,
         /* [in] */ Float upx,
         /* [in] */ Float upy,
         /* [in] */ Float upz);

    static CARAPI SetProjectFrustum(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far);

    static CARAPI SetProjectOrtho(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) GetFinalMatrix();

    static CARAPI_(AutoPtr<ArrayOf<Float> >) GetMMatrix();

    static CARAPI SetLightLocationSun(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

public:
    static AutoPtr<ArrayOf<Float> > sProjMatrix;
    static AutoPtr<ArrayOf<Float> > sVMatrix;
    static AutoPtr<ArrayOf<Float> > sCurrMatrix;
    static AutoPtr<ArrayOf<Float> > sLightLocationSun;
    static Stack<AutoPtr<ArrayOf<Float> > > sStack;

    static AutoPtr<IFloatBuffer> sCameraFB;
    static AutoPtr<IFloatBuffer> sLightPositionFBSun;
    static AutoPtr<IMatrix> sMatrix;
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif