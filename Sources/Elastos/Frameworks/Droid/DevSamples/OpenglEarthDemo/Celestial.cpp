#include "Celestial.h"
#include "MatrixState.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Math.h>
#include "ShaderUtil.h"
#include <elastos/utility/logging/Slogger.h>
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Opengl::CMatrixGL;
using Elastos::Droid::Opengl::IMatrix;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

Celestial::Celestial(
    /* [in] */ Float scale,
    /* [in] */ Float yAngle,
    /* [in] */ Int32 vCount,
    /* [in] */ IGLSurfaceView* mv)
    : UNIT_SIZE(10.f)
{
    mYAngle = yAngle;
    mScale = scale;
    mVCount = vCount;
    InitVertexData();
    InitShader(mv);
}

ECode Celestial::InitVertexData()
{
    AutoPtr<ArrayOf<Float> > vertices = ArrayOf<Float>::Alloc(mVCount * 3);
    for (Int32 i = 0; i < mVCount; i++)
    {
        Double angleTempJD = Elastos::Core::Math::DOUBLE_PI * 2 * Elastos::Core::Math::Random();
        Double angleTempWD = Elastos::Core::Math::DOUBLE_PI * (Elastos::Core::Math::Random() - 0.5f);
        (*vertices)[i*3] = (Float)(UNIT_SIZE * Elastos::Core::Math::Cos(angleTempWD) * Elastos::Core::Math::Sin(angleTempJD));
        (*vertices)[i*3+1] = (Float)(UNIT_SIZE * Elastos::Core::Math::Sin(angleTempWD));
        (*vertices)[i*3+2] = (Float)(UNIT_SIZE * Elastos::Core::Math::Cos(angleTempWD) * Elastos::Core::Math::Cos(angleTempJD));
    }

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> vbb;
    helper->AllocateDirect(vertices->GetLength() * 4, (IByteBuffer**)&vbb);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    vbb->SetOrder(nativeOrder);
    vbb->AsFloatBuffer((IFloatBuffer**)&mVertexBuffer);

    mVertexBuffer->PutFloats(*vertices);
    mVertexBuffer->SetPosition(0);

    return NOERROR;
}

ECode Celestial::InitShader(
    /* [in] */ IGLSurfaceView* mv)
{
    AutoPtr<IResources> res;
    mv->GetResources((IResources**)&res);
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    mVertexShader = ShaderUtil::LoadFromAssetsFile(String("vertex_xk.sh"), res);
    mFragmentShader = ShaderUtil::LoadFromAssetsFile(String("frag_xk.sh"), res);
    mProgram = ShaderUtil::CreateProgram(mVertexShader, mFragmentShader);
    gl->GlGetAttribLocation(mProgram, String("aPosition"), &maPositionHandle);
    gl->GlGetUniformLocation(mProgram, String("uMVPMatrix"), &muMVPMatrixHandle);
    gl->GlGetUniformLocation(mProgram, String("uPointSize"), &uPointSizeHandle);
    return NOERROR;
}

ECode Celestial::DrawSelf()
{
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    gl->GlUseProgram(mProgram); //制定使用某套着色器程序
    //将最终变换矩阵传入着色器程序
    gl->GlUniformMatrix4fv(muMVPMatrixHandle, 1, FALSE, MatrixState::GetFinalMatrix(), 0);
    gl->GlUniform1f(uPointSizeHandle, mScale);  //将顶点尺寸传入着色器程序
    gl->GlVertexAttribPointer( //为画笔指定顶点位置数据
            maPositionHandle,
            3,
            IGLES20::_GL_FLOAT,
            FALSE,
            3*4,
            mVertexBuffer
    );
    //允许顶点位置数据数组
    gl->GlEnableVertexAttribArray(maPositionHandle);
    gl->GlDrawArrays(IGLES20::_GL_POINTS, 0, mVCount); //绘制星星点
    return NOERROR;
}

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos