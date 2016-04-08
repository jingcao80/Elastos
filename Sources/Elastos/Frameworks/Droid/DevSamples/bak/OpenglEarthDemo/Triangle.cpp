#include "Triangle.h"
#include "elastos/droid/ext/frameworkext.h"
#include "ShaderUtil.h"
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
using Elastos::Droid::Opengl::CMatrix;
using Elastos::Droid::Opengl::IMatrix;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

AutoPtr<ArrayOf<Float> > Triangle::mProjMatrix = ArrayOf<Float>::Alloc(16);//4x4���� ͶӰ��
AutoPtr<ArrayOf<Float> > Triangle::mVMatrix = ArrayOf<Float>::Alloc(16);//�����λ�ó���9�������
AutoPtr<ArrayOf<Float> > Triangle::mMVPMatrix = NULL;//��������õ��ܱ任����
AutoPtr<ArrayOf<Float> > Triangle::mMMatrix = ArrayOf<Float>::Alloc(16);//����������ƶ���ת������ת��ƽ��

Triangle::Triangle(
    /* [in] */ IGLSurfaceView* v)
    : mProgram(0)
    , muMVPMatrixHandle(0)
    , maPositionHandle(0)
    , maColorHandle(0)
    , vCount(0)
    , xAngle(0.f)
{
    InitVertexData();
    InitShader(v);
}

ECode Triangle::InitVertexData()
{
    //���������ݵĳ�ʼ��
    vCount = 3;
    Float UNIT_SIZE = 0.2f;
    AutoPtr<ArrayOf<Float> > vertices = ArrayOf<Float>::Alloc(9);
    (*vertices)[0] = -4*UNIT_SIZE;
    (*vertices)[1] = 0;
    (*vertices)[2] = 0;

    (*vertices)[3] = 0;
    (*vertices)[4] = -4*UNIT_SIZE;
    (*vertices)[5] = 0;

    (*vertices)[6] = 4*UNIT_SIZE;
    (*vertices)[7] = 0;
    (*vertices)[8] = 0;

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> vbb;
    helper->AllocateDirect(vertices->GetLength()*4, (IByteBuffer**)&vbb);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    vbb->SetOrder(nativeOrder);
    vbb->AsFloatBuffer((IFloatBuffer**)&mVertexBuffer);
    mVertexBuffer->PutFloats(*vertices);
    mVertexBuffer->SetPosition(0);

    AutoPtr<ArrayOf<Float> > colors = ArrayOf<Float>::Alloc(12);
    (*colors)[0] = 1;
    (*colors)[1] = 1;
    (*colors)[2] = 1;
    (*colors)[3] = 0;

    (*colors)[4] = 0;
    (*colors)[5] = 0;
    (*colors)[6] = 1;
    (*colors)[7] = 0;

    (*colors)[8] = 0;
    (*colors)[9] = 1;
    (*colors)[10] = 0;
    (*colors)[11] = 0;

    AutoPtr<IByteBuffer> cbb;
    helper->AllocateDirect(colors->GetLength()*4, (IByteBuffer**)&cbb);
    cbb->SetOrder(nativeOrder);
    cbb->AsFloatBuffer((IFloatBuffer**)&mColorBuffer);

    mColorBuffer->PutFloats(*colors);
    mColorBuffer->SetPosition(0);
    return NOERROR;
}

ECode Triangle::InitShader(
    /* [in] */ IGLSurfaceView* v)
{
    AutoPtr<IResources> res;
    v->GetResources((IResources**)&res);
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    mVertexShader=ShaderUtil::LoadFromAssetsFile(String("vertex.sh"), res);
    mFragmentShader=ShaderUtil::LoadFromAssetsFile(String("frag.sh"), res);
    mProgram = ShaderUtil::CreateProgram(mVertexShader, mFragmentShader);
    FAIL_RETURN(gl->GlGetAttribLocation(mProgram, String("aPosition"), &maPositionHandle));
    FAIL_RETURN(gl->GlGetAttribLocation(mProgram, String("aColor"), &maColorHandle));
    FAIL_RETURN(gl->GlGetUniformLocation(mProgram, String("uMVPMatrix"), &muMVPMatrixHandle));
    return NOERROR;
}

ECode Triangle::DrawSelf()
{
    SLOGGERD("Opengl", "Triangle::DrawSelf")
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    ASSERT_SUCCEEDED(gl->GlUseProgram(mProgram));
    AutoPtr<IMatrix> matrix;
    CMatrix::AcquireSingleton((IMatrix**)&matrix);
    ASSERT_SUCCEEDED(matrix->SetRotateM(mMMatrix, 0, 0, 0, 1, 0));
    ASSERT_SUCCEEDED(matrix->TranslateM(mMMatrix, 0 ,0, 0, 1));
    ASSERT_SUCCEEDED(matrix->RotateM(mMMatrix, 0, xAngle, 1, 0, 0));
    //
    ASSERT_SUCCEEDED(gl->GlUniformMatrix4fv(muMVPMatrixHandle, 1, FALSE, GetFinalMatrix(mMMatrix), 0));
    ASSERT_SUCCEEDED(gl->GlVertexAttribPointer(
        maPositionHandle,
        3,
        IGLES20::_GL_FLOAT,
        FALSE,
        3*4,
        mVertexBuffer
    ));
    ASSERT_SUCCEEDED(gl->GlVertexAttribPointerEx
    (
        maColorHandle,
        4,
        IGLES20::_GL_FLOAT,
        FALSE,
        4*4,
        mColorBuffer
    ));
    ASSERT_SUCCEEDED(gl->GlEnableVertexAttribArray(maPositionHandle));
    ASSERT_SUCCEEDED(gl->GlEnableVertexAttribArray(maColorHandle));
    ASSERT_SUCCEEDED(gl->GlDrawArrays(IGLES20::_GL_TRIANGLES, 0, vCount));
    return NOERROR;
}

AutoPtr<ArrayOf<Float> > Triangle::GetFinalMatrix(
    /* [in] */ ArrayOf<Float>* spec)
{
    mMVPMatrix = ArrayOf<Float>::Alloc(16);
    AutoPtr<IMatrix> matrix;
    CMatrix::AcquireSingleton((IMatrix**)&matrix);
    ASSERT_SUCCEEDED(matrix->MultiplyMM(mMVPMatrix, 0, mVMatrix, 0, spec, 0));
    ASSERT_SUCCEEDED(matrix->MultiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mMVPMatrix, 0));

    return mMVPMatrix;
}

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos