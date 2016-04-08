
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Triangle.h"
#include "MatrixState.h"
#include "Moon.h"
#include "Earth.h"
#include "Celestial.h"
#include "Cloud.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Opengl::IGLSurfaceView;
using Elastos::Droid::Opengl::IRenderer;
using Elastosx::Microedition::Khronos::Egl::IEGLConfig;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastos::Droid::Opengl::EIID_IRenderer;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class CActivityOne;

class SceneRenderer
    : public IRenderer
    , public ElRefBase
{
public:
    CAR_INTERFACE_DECL()

    SceneRenderer(
        /* [in] */ CActivityOne* host,
        /* [in] */ IGLSurfaceView* v);

    CARAPI OnSurfaceCreated(
        /* [in] */ IGL10* gl,
        /* [in] */ IEGLConfig* config);

    CARAPI OnSurfaceChanged(
        /* [in] */ IGL10* gl,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI OnDrawFrame(
        /* [in] */ IGL10* gl);

    AutoPtr<CActivityOne> mHost;
    AutoPtr<Earth> earth;
    AutoPtr<Moon> moon;
    AutoPtr<Celestial> cSmall;
    AutoPtr<Celestial> cBig;
    AutoPtr<Cloud> cloud;
    AutoPtr<IGLSurfaceView> mView;
    AutoPtr<IThread> rthread;
    Float eAngle;
    Float cAngle;
    Int32 textureIdEarth;//系统分配的地球纹理id
    Int32 textureIdEarthNight;//系统分配的地球夜晚纹理id
    Int32 textureIdMoon;//系统分配的月球纹理id
    Int32 textureIdCloud;//系统分配的云层纹理id
};

class RotateThread : public ThreadBase
{
public:
    RotateThread(
        /* [in] */ SceneRenderer* render);

    CARAPI Run();

    SceneRenderer* mRender;
};

class MyListener
        : public IViewOnClickListener
        , public ElRefBase
{
public:
    CAR_INTERFACE_DECL()

    MyListener(
        /* [in] */ SceneRenderer* host);
    CARAPI OnClick(
       /* [in] */ IView* v);

private:
    SceneRenderer* mRender;

};

class CActivityOne : public Activity
{
public:
    CARAPI_(Int32) InitTexture(
        /* [in] */ Int32 drawableId);
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();



private:

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    AutoPtr<IGLSurfaceView> mView;
    AutoPtr<SceneRenderer> mRenderer;
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
