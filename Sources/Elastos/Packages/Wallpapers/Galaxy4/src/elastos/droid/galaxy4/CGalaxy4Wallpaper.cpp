//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CGalaxy4Wallpaper.h"

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CAR_OBJECT_IMPL(CGalaxy4Wallpaper)

ECode CGalaxy4Wallpaper::constructor()
{
    return WallpaperService::constructor();
}

ECode CGalaxy4Wallpaper::OnCreateEngine(
    /* [out] */ IWallpaperServiceEngine** engine)
{
    VALIDATE_NOT_NULL(engine);
    *engine = new RenderScriptEngine(this);
    REFCOUNT_ADD(*engine);
    return NOERROR;
}


ECode CGalaxy4Wallpaper::RenderScriptEngine::OnCreate(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    FAIL_RETURN(Engine::OnCreate(surfaceHolder))
    SetTouchEventsEnabled(TRUE);
    surfaceHolder->SetSizeFromLayout();
    surfaceHolder->SetFormat(IPixelFormat::RGBA_8888);

    AutoPtr<IDisplayMetrics> metrics;
    CDisplayMetrics::New((IDisplayMetrics**)&metrics);
    AutoPtr<IApplication> app;
    mHost->GetApplication((IApplication**)&app);
    AutoPtr<IInterface> service;
    IContext::Probe(app)->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IDisplay> display;
    IWindowManager::Probe(service)->GetDefaultDisplay((IDisplay**)&display);
    display->GetMetrics(metrics);
    metrics->GetDensityDpi(&mDensityDPI);
    return NOERROR;
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnDestroy()
{
    FAIL_RETURN(Engine::OnDestroy())
    DestroyRenderer();
    return NOERROR;
}

void CGalaxy4Wallpaper::RenderScriptEngine::DestroyRenderer()
{
    if (mWallpaperRS != NULL) {
        mWallpaperRS->Stop();
        mWallpaperRS = NULL;
    }

    if (mRenderScript != NULL) {
        mRenderScript->SetSurface(NULL, 0, 0);
        mRenderScript->Destroy();
        mRenderScript = NULL;
    }
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnSurfaceCreated(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    FAIL_RETURN(Engine::OnSurfaceCreated(surfaceHolder))

    AutoPtr<RenderScript::SurfaceConfig> sc = new RenderScript::SurfaceConfig();
    mRenderScript = new RenderScript();
    FAIL_RETURN(mRenderScript->constructor((IContext*)this, sc));
    mRenderScript->SetPriority(RenderScript::LOW);
    return NOERROR;
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnSurfaceDestroyed(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    FAIL_RETURN(Engine::OnSurfaceDestroyed(surfaceHolder))
    DestroyRenderer();
    return NOERROR;
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnSurfaceChanged(
    /* [in] */ ISurfaceHolder* surfaceHolder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(Engine::OnSurfaceChanged(surfaceHolder, format, width, height))

    if (mRenderScript != NULL) {
        mRenderScript->SetSurface(surfaceHolder, width, height);
    }

    if (mWallpaperRS == NULL) {
        mWallpaperRS = new GalaxyRS();
        AutoPtr<IResources> res;
        mHost->GetResources((IResources**)&res);
        mWallpaperRS->Init(mDensityDPI, mRenderScript, res, width, height);
        mWallpaperRS->Start();
    }
    else {
        mWallpaperRS->Resize(width, height);
    }
    return NOERROR;
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean resultRequested,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    if (mWallpaperRS != NULL) {
        // return mWallpaperRS.onCommand(action, x, y, z, extras, resultRequested);
    }
    *result = NULL;
    return NOERROR;
}

ECode CGalaxy4Wallpaper::RenderScriptEngine::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    FAIL_RETURN(Engine::OnVisibilityChanged(visible))
    if (mWallpaperRS != NULL) {
        if (visible) {
            mWallpaperRS->Start();
        }
        else {
            mWallpaperRS->Stop();
        }
    }
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
