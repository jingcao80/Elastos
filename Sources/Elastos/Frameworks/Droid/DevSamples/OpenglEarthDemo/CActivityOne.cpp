#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <stdio.h>
#include "R.h"

using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::IO::IInputStream;
using Elastos::IO::ICloseable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Opengl::CGLSurfaceView;
using Elastos::Droid::Opengl::IGLES20;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Opengl::IMatrix;
using Elastos::Droid::Opengl::CMatrixGL;
using Elastos::Droid::Opengl::IGLUtils;
using Elastos::Droid::Opengl::CGLUtils;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

CAR_INTERFACE_IMPL(MyListener, IViewOnClickListener)

MyListener::MyListener(
    /* [in] */ SceneRenderer* render)
{
    mRender = render;
}

ECode MyListener::OnClick(
   /* [in] */ IView* v)
{
    AutoPtr<IGLSurfaceView> gView = IGLSurfaceView::Probe(v);
    if (gView) {
        gView->RequestRender();
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(SceneRenderer, IRenderer)

SceneRenderer::SceneRenderer(
    /* [in] */ CActivityOne* host,
    /* [in] */ IGLSurfaceView* v)
    : mHost(host)
    , mView(v)
{}

ECode SceneRenderer::OnSurfaceCreated(
    /* [in] */ IGL10* gl,
    /* [in] */ IEGLConfig* config)
{
    AutoPtr<IGLES20> gl20;
    CGLES20::AcquireSingleton((IGLES20**)&gl20);
    gl20->GlClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //创建地球对象
    earth = new Earth(mView, 2.0f);
    cloud = new Cloud(mView, 2.02f);
    //创建月球对象
    moon = new Moon(mView, 1.0f);
    //创建小星星天球对象
    cSmall = new Celestial(1, 0, 1000, mView);
    //创建大星星天球对象
    cBig = new Celestial(2, 0, 500, mView);
    //打开深度检测
    gl20->GlEnable(IGLES20::_GL_DEPTH_TEST);
    //初始化变换矩阵
    MatrixState::SetInitStack();
    return NOERROR;
}

ECode SceneRenderer::OnSurfaceChanged(
    /* [in] */ IGL10* gl,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IGLES20> gl20;
    CGLES20::AcquireSingleton((IGLES20**)&gl20);
    ASSERT_SUCCEEDED(gl20->GlViewport(0, 0, width, height));
    Float ratio = (Float) width / height;
    MatrixState::SetProjectFrustum(-ratio, ratio, -1, 1, 4.f, 100);
    //调用此方法产生摄像机9参数位置矩阵
    MatrixState::SetCamera(0, 0 , 7.2f, 0.f, 0.f, 0.f, 0.f, 1.0f, 0.0f);
    //打开背面剪裁
    gl20->GlEnable(IGLES20::_GL_CULL_FACE);
    //初始化纹理
    textureIdEarth = mHost->InitTexture(R::drawable::earth);
    textureIdEarthNight = mHost->InitTexture(R::drawable::earthn);
    textureIdMoon = mHost->InitTexture(R::drawable::moon);
    textureIdCloud = mHost->InitTexture(R::drawable::cloud);
    //设置太阳灯光的初始位置
    MatrixState::SetLightLocationSun(100,5,0);
    rthread = new RotateThread(this);
    rthread->Start();
    return NOERROR;
}

ECode SceneRenderer::OnDrawFrame(
    /* [in] */ IGL10* gl)
{
    AutoPtr<IGLES20> gl20;
    CGLES20::AcquireSingleton((IGLES20**)&gl20);
    //清除深度缓冲与颜色缓冲
    gl20->GlClear(IGLES20::_GL_DEPTH_BUFFER_BIT | IGLES20::_GL_COLOR_BUFFER_BIT);

    //保护现场
    MatrixState::PushMatrix();
    //地球自转
    MatrixState::Rotate(eAngle, 0, 1, 0);
    //绘制纹理圆球
    earth->DrawSelf(textureIdEarth, textureIdEarthNight);

    //开启混合
    gl20->GlEnable(IGLES20::_GL_BLEND);
    //设置混合因子
    gl20->GlBlendFunc(IGLES20::_GL_SRC_ALPHA, IGLES20::_GL_ONE_MINUS_SRC_ALPHA);
    cloud->DrawSelf(textureIdCloud);//绘制云层
    //关闭混合
    gl20->GlDisable(IGLES20::_GL_BLEND);


    //推坐标系到月球位置
    MatrixState::Transtate(2.f, 0, 0);
    //月球自转
    MatrixState::Rotate(eAngle, 0, 1, 0);
    //绘制月球
    moon->DrawSelf(textureIdMoon);
    //恢复现场
    MatrixState::PopMatrix();

    //保护现场
    MatrixState::PushMatrix();
    MatrixState::Rotate(cAngle, 0, 1, 0);
    cSmall->DrawSelf();
    cBig->DrawSelf();
    //恢复现场
    MatrixState::PopMatrix();
    return NOERROR;
}

RotateThread::RotateThread(
    /* [in] */ SceneRenderer* render)
{
    Thread::constructor();
    mRender = render;
}

ECode RotateThread::Run()
{
    while(TRUE){
        //地球自转角度
        mRender->eAngle = mRender->eAngle + 1.f;
        //天球自转角度
        mRender->cAngle = mRender->cAngle + 0.1f;
        if (mRender->eAngle >= 360.f) {
            mRender->eAngle -= 360.f;
        }
        if (mRender->cAngle >= 360.f) {
            mRender->cAngle -= 360.f;
        }
        Thread::Sleep(20);
    }
    return NOERROR;
}

//=============================================================================================


ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Slogger::D("zhangjingcheng", " >> OnCreate...");
    Boolean rst;
    RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &rst);
    SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_PORTRAIT);
    CGLSurfaceView::New(this, (IGLSurfaceView**)&mView);
    mView->SetEGLContextClientVersion(2);
    mRenderer = new SceneRenderer(this, mView);
    mView->SetRenderer(mRenderer);
    mView->SetRenderMode(IGLSurfaceView::RENDERMODE_CONTINUOUSLY);
    mView->RequestFocus(&rst);
    mView->SetFocusableInTouchMode(TRUE);
    AutoPtr<MyListener> l = new MyListener(mRenderer);
    mView->SetOnClickListener(l);
    SetContentView(mView);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Activity::OnStart();
    Slogger::D("CActivityOne", " >> OnStart...");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    Activity::OnResume();
    Slogger::D("CActivityOne", " >> OnResume...");
    mView->OnResume();
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Activity::OnPause();
    Slogger::D("CActivityOne", " >> OnPause...");
    mView->OnPause();
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    Activity::OnStop();
    Slogger::D("CActivityOne", " >> OnStop...");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    Slogger::D("CActivityOne", " >> OnDestroy...");
    return NOERROR;
}

Int32 CActivityOne::InitTexture(
        /* [in] */ Int32 drawableId)
{
    //生成纹理ID
    AutoPtr<ArrayOf<Int32> > textures = ArrayOf<Int32>::Alloc(1);
    AutoPtr<IGLES20> gl20;
    CGLES20::AcquireSingleton((IGLES20**)&gl20);
    gl20->GlGenTextures
    (
            1,          //产生的纹理id的数量
            textures,   //纹理id的数组
            0           //偏移量
    );
    Int32 textureId = (*textures)[0];
    gl20->GlBindTexture(IGLES20::_GL_TEXTURE_2D, textureId);
    gl20->GlTexParameterf(IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_MIN_FILTER,IGLES20::_GL_NEAREST);
    gl20->GlTexParameterf(IGLES20::_GL_TEXTURE_2D,IGLES20::_GL_TEXTURE_MAG_FILTER,IGLES20::_GL_LINEAR);
    gl20->GlTexParameterf(IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_WRAP_S,IGLES20::_GL_CLAMP_TO_EDGE);
    gl20->GlTexParameterf(IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_WRAP_T,IGLES20::_GL_CLAMP_TO_EDGE);

    //通过输入流加载图片===============begin===================
    AutoPtr<IInputStream> is;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->OpenRawResource(drawableId, (IInputStream**)&is);
    AutoPtr<IBitmap> bitmapTmp;
    // try
    // {
        AutoPtr<IBitmapFactory> bFactory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bFactory);
        bFactory->DecodeStream(is, (IBitmap**)&bitmapTmp);
    // }
    // finally
    // {
    //     try
    //     {
    //         is.close();
    //     }
    //     catch(IOException e)
    //     {
    //         e.printStackTrace();
    //     }
    // }
    //通过输入流加载图片===============end=====================

    //实际加载纹理
    AutoPtr<IGLUtils> glUtils;
    CGLUtils::AcquireSingleton((IGLUtils**)&glUtils);
    glUtils->TexImage2DEx2
    (
            IGLES20::_GL_TEXTURE_2D,   //纹理类型，在OpenGL ES中必须为GL10.GL_TEXTURE_2D
            0,                    //纹理的层次，0表示基本图像层，可以理解为直接贴图
            bitmapTmp,            //纹理图像
            0                     //纹理边框尺寸
    );
    bitmapTmp->Recycle();          //纹理加载成功后释放图片
    is->Close();

    return textureId;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Slogger::D("CActivityOne", " >> OnActivityResult...");
    return NOERROR;
}

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
