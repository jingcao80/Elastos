
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