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

#include "Elastos.Droid.Utility.h"
#include "RenderScript.h"
#include <elastos/core/AutoLock.h>
#include <rs/rs.h>
#include <gui/Surface.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::ISurface;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

AutoPtr<IFile> RenderScript::sCacheDir;

ECode RenderScript::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ SurfaceConfig* sc)
{
    // super(ctx);
    mSurfaceConfig = new SurfaceConfig(sc);

    AutoPtr<IApplicationInfo> appInfo;
    ctx->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 sdkVersion;
    appInfo->GetTargetSdkVersion(&sdkVersion);

    mWidth = 0;
    mHeight = 0;
    mDev = nDeviceCreate();
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 dpi;
    dm->GetDensityDpi(&dpi);
    mContext = nContextCreateGL(mDev, 0, sdkVersion,
            mSurfaceConfig->mColorMin, mSurfaceConfig->mColorPref,
            mSurfaceConfig->mAlphaMin, mSurfaceConfig->mAlphaPref,
            mSurfaceConfig->mDepthMin, mSurfaceConfig->mDepthPref,
            mSurfaceConfig->mStencilMin, mSurfaceConfig->mStencilPref,
            mSurfaceConfig->mSamplesMin, mSurfaceConfig->mSamplesPref,
            mSurfaceConfig->mSamplesQ, dpi);
    if (mContext == 0) {
        // throw new RSDriverException("Failed to create RS context.");
        return E_RUNTIME_EXCEPTION;
    }
    // mMessageThread = new MessageThread(this);
    // mMessageThread.start();

    return NOERROR;
}

ECode RenderScript::BindRootScript(
    /* [in] */ Script* s)
{
    FAIL_RETURN(Validate());
    Int64 id;
    FAIL_RETURN(SafeID(s, &id));
    nContextBindRootScript(id);
    return NOERROR;
}

ECode RenderScript::ValidateObject(
    /* [in] */ BaseObj* o)
{
    if (o != NULL) {
        if (o->mRS != this) {
            // throw new RSIllegalArgumentException("Attempting to use an object across contexts.");
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RenderScript::Validate()
{
    if (mContext == 0) {
        // throw new RSInvalidStateException("Calling RS with no Context active.");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode RenderScript::SetPriority(
    /* [in] */ Priority p)
{
    FAIL_RETURN(Validate());
    nContextSetPriority(p);
    return NOERROR;
}

ECode RenderScript::SetSurface(
    /* [in] */ ISurfaceHolder* sur,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    FAIL_RETURN(Validate());
    AutoPtr<ISurface> s;
    if (sur != NULL) {
        sur->GetSurface((ISurface**)&s);
    }
    mWidth = w;
    mHeight = h;
    nContextSetSurface(w, h, s);
    return NOERROR;
}

ECode RenderScript::Destroy()
{
    FAIL_RETURN(Validate());
    nContextFinish();

    nContextDeinitToClient(mContext);
    // mMessageThread.mRun = false;
    // try {
    //     mMessageThread.join();
    // } catch(InterruptedException e) {
    // }

    FAIL_RETURN(nContextDestroy());

    nDeviceDestroy(mDev);
    mDev = 0;

    return NOERROR;
}

Int64 RenderScript::nDeviceCreate()
{
    return (Int64)(uintptr_t)rsDeviceCreate();
}

void RenderScript::nDeviceDestroy(
    /* [in] */ Int64 dev)
{
    return rsDeviceDestroy((RsDevice)dev);
}

Int64 RenderScript::nContextCreateGL(
    /* [in] */ Int64 dev,
    /* [in] */ Int32 ver,
    /* [in] */ Int32 sdkVer,
    /* [in] */ Int32 colorMin,
    /* [in] */ Int32 colorPref,
    /* [in] */ Int32 alphaMin,
    /* [in] */ Int32 alphaPref,
    /* [in] */ Int32 depthMin,
    /* [in] */ Int32 depthPref,
    /* [in] */ Int32 stencilMin,
    /* [in] */ Int32 stencilPref,
    /* [in] */ Int32 samplesMin,
    /* [in] */ Int32 samplesPref,
    /* [in] */ Float samplesQ,
    /* [in] */ Int32 dpi)
{
    AutoLock lock(this);

    RsSurfaceConfig sc;
    sc.alphaMin = alphaMin;
    sc.alphaPref = alphaPref;
    sc.colorMin = colorMin;
    sc.colorPref = colorPref;
    sc.depthMin = depthMin;
    sc.depthPref = depthPref;
    sc.samplesMin = samplesMin;
    sc.samplesPref = samplesPref;
    sc.samplesQ = samplesQ;

    return (Int64)(uintptr_t)rsContextCreateGL((RsDevice)dev, ver, sdkVer, sc, dpi);
}

void RenderScript::nContextSetSurface(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurface* sur)
{
    AutoLock lock(this);

    ANativeWindow* window = NULL;
    if (sur != NULL) {
        Int64 nativeSurf;
        sur->GetNativeSurface(&nativeSurf);
        window = (ANativeWindow*)reinterpret_cast<android::Surface*>(nativeSurf);
    }

    rsContextSetSurface((RsContext)mContext, w, h, window);
}

void RenderScript::nContextFinish()
{
    AutoLock lock(this);

    rsContextFinish((RsContext)mContext);
}

void RenderScript::nContextDeinitToClient(
    /* [in] */ Int64 con)
{
    rsContextDeinitToClient((RsContext)con);
}

ECode RenderScript::nContextDestroy()
{
    FAIL_RETURN(Validate());

    // take teardown lock
    // teardown lock can only be taken when no objects are being destroyed
    Int64 curCon = 0;
    {
        AutoLock lock(mRWLock);
        curCon = mContext;
        // context is considered dead as of this point
        mContext = 0;
    }

    rsContextDestroy((RsContext)curCon);
    return NOERROR;
}

void RenderScript::nContextSetPriority(
    /* [in] */ Int32 p)
{
    AutoLock lock(this);
    rsContextSetPriority((RsContext)mContext, p);
}

void RenderScript::nContextBindRootScript(
    /* [in] */ Int64 script)
{
    AutoLock lock(this);
    rsContextBindRootScript((RsContext)mContext, (RsScript)script);
}

ECode RenderScript::nScriptCCreate(
    /* [in] */ const String& resName,
    /* [in] */ const String& cacheDir,
    /* [in] */ ArrayOf<Byte>* script,
    /* [in] */ Int32 length,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    AutoLock lock(this);
    FAIL_RETURN(Validate());

    Int64 ret = 0;
    Byte* script_ptr = NULL;
    Int32 remaining;
    if (!script) {
        //jniThrowException(_env, "java/lang/IllegalArgumentException", "script == null");
        goto exit;
    }
    if (length < 0) {
        //jniThrowException(_env, "java/lang/IllegalArgumentException", "length < 0");
        goto exit;
    }
    remaining = script->GetLength();
    if (remaining < length) {
        //jniThrowException(_env, "java/lang/IllegalArgumentException",
        //        "length > script.length - offset");
        goto exit;
    }
    script_ptr = script->GetPayload();

    //rsScriptCSetText((RsContext)con, (const char *)script_ptr, length);

    ret = (Int64)(uintptr_t)rsScriptCCreate((RsContext)mContext,
                                resName.string(), resName.GetByteLength(),
                                cacheDir.string(), cacheDir.GetByteLength(),
                                (const char *)script_ptr, length);

exit:
    *id = ret;
    return NOERROR;
}

ECode RenderScript::nScriptSetVarObj(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate());
    rsScriptSetVarObj((RsContext)mContext, (RsScript)id, slot, (RsObjectBase)val);
    return NOERROR;
}

ECode RenderScript::nMeshCreate(
    /* [in] */ ArrayOf<Int64>* vtx,
    /* [in] */ ArrayOf<Int64>* idx,
    /* [in] */ ArrayOf<Int32>* prim,
    /* [out] */ Int64* mesh)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate());

    Int32 vtxLen = vtx->GetLength();
    Int64* jVtxPtr = vtx->GetPayload();
    RsAllocation* vtxPtr = (RsAllocation*) malloc(sizeof(RsAllocation) * vtxLen);
    for(Int32 i = 0; i < vtxLen; ++i) {
        vtxPtr[i] = (RsAllocation)(uintptr_t)jVtxPtr[i];
    }

    Int32 idxLen = idx->GetLength();
    Int64* jIdxPtr = idx->GetPayload();
    RsAllocation* idxPtr = (RsAllocation*) malloc(sizeof(RsAllocation) * idxLen);
    for(Int32 i = 0; i < idxLen; ++i) {
        idxPtr[i] = (RsAllocation)(uintptr_t)jIdxPtr[i];
    }

    Int32 primLen = prim->GetLength();
    Int32* primPtr = prim->GetPayload();

    Int64 id = (Int64)(uintptr_t)rsMeshCreate((RsContext)mContext,
                               (RsAllocation *)vtxPtr, vtxLen,
                               (RsAllocation *)idxPtr, idxLen,
                               (uint32_t *)primPtr, primLen);

    free(vtxPtr);
    free(idxPtr);
    *mesh = id;
    return NOERROR;
}

ECode RenderScript::SafeID(
    /* [in] */ BaseObj* o,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = 0;
    if (o != NULL) {
        return o->GetID(this, id);
    }
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
