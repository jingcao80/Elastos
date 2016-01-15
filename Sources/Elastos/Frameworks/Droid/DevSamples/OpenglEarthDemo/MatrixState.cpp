#include "MatrixState.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Opengl::CMatrixGL;
using Elastos::Droid::Opengl::IMatrix;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

static AutoPtr<IMatrix> InitMatrix()
{
    AutoPtr<IMatrix> matrix;
    CMatrixGL::AcquireSingleton((IMatrix**)&matrix);
    return matrix;
}

AutoPtr<ArrayOf<Float> > MatrixState::sProjMatrix = ArrayOf<Float>::Alloc(16);
AutoPtr<ArrayOf<Float> > MatrixState::sVMatrix = ArrayOf<Float>::Alloc(16);
AutoPtr<ArrayOf<Float> > MatrixState::sCurrMatrix = ArrayOf<Float>::Alloc(16);
AutoPtr<ArrayOf<Float> > MatrixState::sLightLocationSun = ArrayOf<Float>::Alloc(3);
AutoPtr<IFloatBuffer> MatrixState::sCameraFB;
AutoPtr<IFloatBuffer> MatrixState::sLightPositionFBSun;
Stack<AutoPtr<ArrayOf<Float> > > MatrixState::sStack;
AutoPtr<IMatrix> MatrixState::sMatrix = InitMatrix();

ECode MatrixState::SetInitStack()
{
    return sMatrix->SetRotateM(sCurrMatrix, 0, 0, 1, 0, 0);
}

ECode MatrixState::PushMatrix()
{
    sStack.Push(sCurrMatrix->Clone());
    return NOERROR;
}

ECode MatrixState::PopMatrix()
{
    sCurrMatrix = sStack.GetTop();
    sStack.Pop();
    return NOERROR;
}

ECode MatrixState::Transtate(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z)
{
    return sMatrix->TranslateM(sCurrMatrix, 0, x, y, z);
}

ECode MatrixState::Rotate(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z)
{
    return sMatrix->RotateM(sCurrMatrix, 0, angle, x, y, z);
}

ECode MatrixState::SetCamera(
         /* [in] */ Float cx,
         /* [in] */ Float cy,
         /* [in] */ Float cz,
         /* [in] */ Float tx,
         /* [in] */ Float ty,
         /* [in] */ Float tz,
         /* [in] */ Float upx,
         /* [in] */ Float upy,
         /* [in] */ Float upz)
{
    sMatrix->SetLookAtM(sVMatrix, 0, cx, cy, cz, tx, ty, tz, upx, upy, upz);

    AutoPtr<ArrayOf<Float> > cameraLocation = ArrayOf<Float>::Alloc(3);
    (*cameraLocation)[0] = cx;
    (*cameraLocation)[1] = cy;
    (*cameraLocation)[2] = cz;

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> llbb;
    helper->AllocateDirect(3 * 4, (IByteBuffer**)&llbb);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    llbb->SetOrder(nativeOrder);
    llbb->AsFloatBuffer((IFloatBuffer**)&sCameraFB);

    sCameraFB->PutFloats(*cameraLocation);
    sCameraFB->SetPosition(0);
    return NOERROR;
}

ECode MatrixState::SetProjectFrustum(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far)
{
    return sMatrix->FrustumM(sProjMatrix, 0, left, right, bottom, top, near, far);
}

ECode MatrixState::SetProjectOrtho(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far)
{
    return sMatrix->OrthoM(sProjMatrix, 0, left, right, bottom, top, near, far);
}

AutoPtr<ArrayOf<Float> > MatrixState::GetFinalMatrix()
{
    AutoPtr<ArrayOf<Float> > finalMatrix = ArrayOf<Float>::Alloc(16);
    sMatrix->MultiplyMM(finalMatrix, 0, sVMatrix, 0, sCurrMatrix, 0);
    sMatrix->MultiplyMM(finalMatrix, 0, sProjMatrix, 0, finalMatrix, 0);
    return finalMatrix;
}

AutoPtr<ArrayOf<Float> > MatrixState::GetMMatrix()
{
    return sCurrMatrix;
}

ECode MatrixState::SetLightLocationSun(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z)
{
    (*sLightLocationSun)[0] = x;
    (*sLightLocationSun)[1] = y;
    (*sLightLocationSun)[2] = z;
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> llbb;
    helper->AllocateDirect(3 * 4, (IByteBuffer**)&llbb);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    llbb->SetOrder(nativeOrder);

    llbb->AsFloatBuffer((IFloatBuffer**)&sLightPositionFBSun);
    sLightPositionFBSun->PutFloats(*sLightLocationSun);
    sLightPositionFBSun->SetPosition(0);

    return NOERROR;
}


} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos