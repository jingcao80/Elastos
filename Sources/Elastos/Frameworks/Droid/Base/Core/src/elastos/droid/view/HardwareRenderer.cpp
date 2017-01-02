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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/HardwareRenderer.h"
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/view/GLES20Canvas.h"

using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace View {

//========================================================================================
//              HardwareRenderer::
//========================================================================================
//String HardwareRenderer::LOG_TAG("HardwareRenderer");

const String HardwareRenderer::CACHE_PATH_SHADERS("com.android.opengl.shaders_cache");

const String HardwareRenderer::RENDER_DIRTY_REGIONS_PROPERTY("debug.hwui.render_dirty_regions");

const String HardwareRenderer::PROFILE_PROPERTY("debug.hwui.profile");

const String HardwareRenderer::PROFILE_PROPERTY_VISUALIZE_BARS("visual_bars");

const String HardwareRenderer::PROFILE_MAXFRAMES_PROPERTY("debug.hwui.profile.maxframes");

const String HardwareRenderer::PRINT_CONFIG_PROPERTY("debug.hwui.print_config");

const String HardwareRenderer::DEBUG_DIRTY_REGIONS_PROPERTY("debug.hwui.show_dirty_regions");

const String HardwareRenderer::DEBUG_SHOW_LAYERS_UPDATES_PROPERTY("debug.hwui.show_layers_updates");

const String HardwareRenderer::DEBUG_OVERDRAW_PROPERTY("debug.hwui.overdraw");

const String HardwareRenderer::OVERDRAW_PROPERTY_SHOW("show");

const String HardwareRenderer::DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY("debug.hwui.show_non_rect_clip");

Boolean HardwareRenderer::sRendererDisabled = FALSE;

Boolean HardwareRenderer::sSystemRendererDisabled = FALSE;

Boolean HardwareRenderer::sTrimForeground = FALSE;

CAR_INTERFACE_IMPL(HardwareRenderer, Object, IHardwareRenderer)

HardwareRenderer::HardwareRenderer()
    : mEnabled(FALSE)
    , mRequested(TRUE)
{
}

HardwareRenderer::~HardwareRenderer()
{}

ECode HardwareRenderer::Disable(
    /* [in] */ Boolean system)
{
    sRendererDisabled = TRUE;
    if (system) {
        sSystemRendererDisabled = TRUE;
    }
    return NOERROR;
}

ECode HardwareRenderer::EnableForegroundTrimming()
{
    sTrimForeground = TRUE;
    return NOERROR;
}

ECode HardwareRenderer::IsAvailable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = GLES20Canvas::IsAvailable();
    return NOERROR;
}

ECode HardwareRenderer::SetupDiskCache(
    /* [in] */ IFile* cacheDir)
{
    AutoPtr<IFile> f;
    CFile::New(cacheDir, CACHE_PATH_SHADERS, (IFile**)&f);
    String path;
    f->GetAbsolutePath(&path);
    ThreadedRenderer::NativeSetupShadersDiskCache(path);
    return NOERROR;
}

ECode HardwareRenderer::InitializeIfNeeded(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ISurface* surface,
    /* [in] */ IRect* surfaceInsets,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mRequested) {
        // We lost the gl context, so recreate it.
        Boolean enabled;
        if (IsEnabled(&enabled), !enabled) {
            Boolean bInit = FALSE;
            if ((Initialize(surface, &bInit), bInit)) {
                Setup(width, height, surfaceInsets);
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<HardwareRenderer> HardwareRenderer::Create(
    /* [in] */ IContext* context,
    /* [in] */ Boolean translucent)
{
    AutoPtr<ThreadedRenderer> renderer;
    if (GLES20Canvas::IsAvailable()) {
        renderer = new ThreadedRenderer();
        renderer->constructor(context, translucent);
    }
    return renderer;
}

ECode HardwareRenderer::TrimMemory(
    /* [in] */ Int32 level)
{
    ThreadedRenderer::TrimMemory(level);
    return NOERROR;
}

ECode HardwareRenderer::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mEnabled;
    return NOERROR;
}

ECode HardwareRenderer::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    return NOERROR;
}

ECode HardwareRenderer::IsRequested(
    /* [out] */ Boolean* requested)
{
    VALIDATE_NOT_NULL(requested)
    *requested = mRequested;
    return NOERROR;
}

ECode HardwareRenderer::SetRequested(
    /* [in] */ Boolean requested)
{
    mRequested = requested;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
