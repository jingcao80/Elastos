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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "CGalaxyView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

//====================================================================
// CGalaxyView::SHCallback
//====================================================================
CAR_INTERFACE_IMPL(CGalaxyView::SHCallback, Object, ISurfaceHolderCallback)

CGalaxyView::SHCallback::SHCallback(
    /* [in] */ CGalaxyView* host)
    : mHost(host)
{}

ECode CGalaxyView::SHCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceCreated(holder);
}

ECode CGalaxyView::SHCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return mHost->SurfaceChanged(holder, format, width, height);
}

ECode CGalaxyView::SHCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceDestroyed(holder);
}


//===================================================================
// CGalaxyView
//===================================================================
CAR_OBJECT_IMPL(CGalaxyView)

ECode CGalaxyView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SurfaceView::constructor(context));
    Init();
    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);
    return NOERROR;
}

ECode CGalaxyView::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    {
        AutoLock lock(this);
        if (mRS != NULL) {
            mRS->SetSurface(holder, width, height);
        }
    }
    if (mRS == NULL) {
        AutoPtr<RenderScript::SurfaceConfig> sc = new RenderScript::SurfaceConfig();
        FAIL_RETURN(CreateRenderScript(sc, (RenderScript**)&mRS))
        mRS->SetSurface(holder, width, height);

        AutoPtr<IDisplayMetrics> metrics;
        CDisplayMetrics::New((IDisplayMetrics**)&metrics);

        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IInterface> service;
        ctx->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        AutoPtr<IDisplay> display;
        IWindowManager::Probe(service)->GetDefaultDisplay((IDisplay**)&display);
        display->GetMetrics(metrics);

        mRender = new GalaxyRS();
        Int32 dpi;
        metrics->GetDensityDpi(&dpi);
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        mRender->Init(dpi, mRS, res, width, height);
    }
    else {
        mRender->CreateProgramVertex();
    }
    return NOERROR;
}

ECode CGalaxyView::OnDetachedFromWindow()
{
    if (mRS != NULL) {
        mRS->SetSurface(NULL, 0, 0);
        mRS = NULL;
        DestroyRenderScript();
    }
    return NOERROR;
}

void CGalaxyView::Init()
{
    // Install a SurfaceHolder.Callback so we get notified when the
    // underlying surface is created and destroyed
    AutoPtr<ISurfaceHolder> holder;
    GetHolder((ISurfaceHolder**)&holder);
    mCallback = new SHCallback(this);
    holder->AddCallback(mCallback);
}

ECode CGalaxyView::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    mSurfaceHolder = holder;
    return NOERROR;
}

ECode CGalaxyView::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    {
        AutoLock lock(this);
        // Surface will be destroyed when we return
        if (mRS != NULL) {
            mRS->SetSurface(NULL, 0, 0);
        }
    }
    return NOERROR;
}

ECode CGalaxyView::CreateRenderScript(
    /* [in] */ RenderScript::SurfaceConfig* sc,
    /* [out] */ RenderScript** result)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<RenderScript> rs = new RenderScript();
    FAIL_RETURN(rs->constructor(ctx, sc));
    SetRenderScript(rs);
    *result = rs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGalaxyView::DestroyRenderScript()
{
    {
        AutoLock lock(this);
        mRS->Destroy();
        mRS = NULL;
    }
    return NOERROR;
}

ECode CGalaxyView::SetRenderScript(
    /* [in] */ RenderScript* rs)
{
    mRS = rs;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
