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

#include "elastos/droid/renderscript/CRenderScriptHelper.h"
#include "elastos/droid/renderscript/RenderScript.h"

namespace Elastos {
namespace Droid {
namespace RenderScript {

CAR_INTERFACE_IMPL(CRenderScriptHelper, Singleton, IRenderScriptHelper)

CAR_SINGLETON_IMPL(CRenderScriptHelper)

ECode CRenderScriptHelper::GetMinorID(
    /* [out] */ Int64* id)
{
    return RenderScript::GetMinorID(id);
}

ECode CRenderScriptHelper::SetupDiskCache(
    /* [in] */ IFile* cacheDir)
{
    return RenderScript::SetupDiskCache(cacheDir);
}

ECode CRenderScriptHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 sdkVersion,
    /* [out] */ IRenderScript** rs)
{
    return RenderScript::Create(ctx, sdkVersion, rs);
}

ECode CRenderScriptHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 sdkVersion,
    /* [in] */ RenderScriptContextType ct,
    /* [in] */ Int32 flags,
    /* [out] */ IRenderScript** rs)
{
    return RenderScript::Create(ctx, sdkVersion, ct, flags, rs);
}

ECode CRenderScriptHelper::Create(
    /* [in] */ IContext* ctx,
    /* [out] */ IRenderScript** rs)
{
    return RenderScript::Create(ctx, rs);
}

ECode CRenderScriptHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ RenderScriptContextType ct,
    /* [out] */ IRenderScript** rs)
{
    return RenderScript::Create(ctx, ct, rs);
}

ECode CRenderScriptHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ RenderScriptContextType ct,
    /* [in] */ Int32 flags,
    /* [out] */ IRenderScript** rs)
{
    return RenderScript::Create(ctx, ct, flags, rs);
}

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos