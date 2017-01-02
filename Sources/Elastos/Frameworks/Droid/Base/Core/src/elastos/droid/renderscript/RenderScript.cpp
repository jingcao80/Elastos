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

#include "elastos/droid/RenderScript/RenderScript.h"
#include "elastos/droid/RenderScript/Allocation.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <androidfw/Asset.h>
#include <gui/Surface.h>
#include <ui/ANativeObjectBase.h>
#include <rs/rs.h>
#include <rs/rsEnv.h>
#include <skia/core/SkBitmap.h>

using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::RenderScript::EIID_IRenderScript;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::View::Surface;
using Elastos::Droid::View::CSurface;
using Elastos::Core::AutoLock;
using Elastos::Core::IByte;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger64;
using Elastos::Utility::Concurrent::Locks::CReentrantReadWriteLock;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

#define PER_ARRAY_TYPE(flag, fnc, readonly, ...) {                                      \
    int len = 0;                                                                        \
    void *ptr = NULL;                                                                   \
    size_t typeBytes = 0;                                                               \
    AutoPtr<IArrayOf> array = IArrayOf::Probe(d);                                       \
    switch(dataType) {                                                                  \
    case RS_TYPE_FLOAT_32:                                                              \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Float> > arrayF = ArrayOf<Float>::Alloc(len);                   \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IFloat::Probe(v)->GetValue(&(*arrayF)[i]);                                  \
        }                                                                               \
        ptr = arrayF->GetPayload();                                                     \
        typeBytes = 4;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    case RS_TYPE_FLOAT_64:                                                              \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Double> > arrayD = ArrayOf<Double>::Alloc(len);                 \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IDouble::Probe(v)->GetValue(&(*arrayD)[i]);                                 \
        }                                                                               \
        ptr = arrayD->GetPayload();                                                     \
        typeBytes = 8;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    case RS_TYPE_SIGNED_8:                                                              \
    case RS_TYPE_UNSIGNED_8:                                                            \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Byte> > arrayB = ArrayOf<Byte>::Alloc(len);                     \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IByte::Probe(v)->GetValue(&(*arrayB)[i]);                                   \
        }                                                                               \
        ptr = arrayB->GetPayload();                                                     \
        typeBytes = 1;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    case RS_TYPE_SIGNED_16:                                                             \
    case RS_TYPE_UNSIGNED_16:                                                           \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Int16> > arrayI16 = ArrayOf<Int16>::Alloc(len);                 \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IInteger16::Probe(v)->GetValue(&(*arrayI16)[i]);                            \
        }                                                                               \
        ptr = arrayI16->GetPayload();                                                   \
        typeBytes = 2;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    case RS_TYPE_SIGNED_32:                                                             \
    case RS_TYPE_UNSIGNED_32:                                                           \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Int32> > arrayI32 = ArrayOf<Int32>::Alloc(len);                 \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IInteger32::Probe(v)->GetValue(&(*arrayI32)[i]);                            \
        }                                                                               \
        ptr = arrayI32->GetPayload();                                                   \
        typeBytes = 4;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    case RS_TYPE_SIGNED_64:                                                             \
    case RS_TYPE_UNSIGNED_64:                                                           \
        array->GetLength(len);                                                          \
        AutoPtr<ArrayOf<Int64> > arrayI64 = ArrayOf<Int64>::Alloc(len);                 \
        for (Int32 i = 0; i < len; i++) {                                               \
            AutoPtr<IInterface> v;                                                      \
            array->Get(i, (IInterface**)&v);                                            \
            IInteger64::Probe(v)->GetValue(&(*arrayI64)[i]);                            \
        }                                                                               \
        ptr = arrayI64->GetPayload();                                                   \
        typeBytes = 8;                                                                  \
        fnc(__VA_ARGS__);                                                               \
        return;                                                                         \
    default:                                                                            \
        break;                                                                          \
    }                                                                                   \
}

class AutoStringArrayToUTF8 {
public:
    AutoStringArrayToUTF8(ArrayOf<String>* strings, Int32 stringsLength)
    : mStrings(strings), mStringsLength(stringsLength) {
        mCStrings = NULL;
        mSizeArray = NULL;
        if (stringsLength > 0) {
            mCStrings = (const char **)calloc(stringsLength, sizeof(char *));
            mSizeArray = (size_t*)calloc(stringsLength, sizeof(size_t));
            for (Int32 ct = 0; ct < stringsLength; ct ++) {
                String s = (*mStrings)[ct];
                mCStrings[ct] = s.string();
                mSizeArray[ct] = s.GetLength();
            }
        }
    }
    ~AutoStringArrayToUTF8() {
        free(mCStrings);
        free(mSizeArray);
    }
    const char **c_str() const { return mCStrings; }
    size_t *c_str_len() const { return mSizeArray; }
    Int32 length() const { return mStringsLength; }

private:
    AutoPtr<ArrayOf<String> > mStrings;
    const char **mCStrings;
    size_t      *mSizeArray;
    Int32        mStringsLength;
};


//==========================================================
// RenderScript::RSMessageHandler
//==========================================================

ECode RenderScript::RSMessageHandler::Run()
{
    return NOERROR;
}


//==========================================================
// RenderScript::RSErrorHandler
//==========================================================

ECode RenderScript::RSErrorHandler::Run()
{
    return NOERROR;
}


//==========================================================
// RenderScript::StaticInitializer
//==========================================================

RenderScript::StaticInitializer::StaticInitializer()
{
    RenderScript::sInitialized = FALSE;
    AutoPtr<ISystemProperties> props;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&props);
    Boolean value;
    if (props->GetBoolean(String("config.disable_renderscript"), FALSE, &value), !value) {
        // try {
        // Class<?> vm_runtime = Class.forName("dalvik.system.VMRuntime");
        // Method get_runtime = vm_runtime.getDeclaredMethod("getRuntime");
        // sRuntime = get_runtime.invoke(null);
        // registerNativeAllocation = vm_runtime.getDeclaredMethod("registerNativeAllocation", Integer.TYPE);
        // registerNativeFree = vm_runtime.getDeclaredMethod("registerNativeFree", Integer.TYPE);
        // } catch (Exception e) {
        //     Log.e(LOG_TAG, "Error loading GC methods: " + e);
        //     throw new RSRuntimeException("Error loading GC methods: " + e);
        // }
        // try {
        // System.loadLibrary("rs_jni");
        RenderScript::_nInit();
        RenderScript::sInitialized = TRUE;
        RenderScript::sPointerSize = RenderScript::RsnSystemGetPointerSize();
        // } catch (UnsatisfiedLinkError e) {
        //     Log.e(LOG_TAG, "Error loading RS jni library: " + e);
        //     throw new RSRuntimeException("Error loading RS jni library: " + e);
        // }
    }
}


//==========================================================
// RenderScript::MessageThread
//==========================================================

RenderScript::MessageThread::MessageThread(
    /* [in] */ RenderScript* rs)
    : mRS(rs)
    , mRun(TRUE)
{
    constructor(String("RSMessageThread"));
    mAuxData = ArrayOf<Int32>::Alloc(2);
}

ECode RenderScript::MessageThread::Run()
{
    // This function is a temporary solution.  The final solution will
    // used typed allocations where the message id is the type indicator.
    AutoPtr<ArrayOf<Int32> > rbuf = ArrayOf<Int32>::Alloc(16);
    mRS->NContextInitToClient(mRS->mContext);
    while(mRun) {
        (*rbuf)[0] = 0;
        Int32 msg = mRS->NContextPeekMessage(mRS->mContext, mAuxData);
        Int32 size = (*mAuxData)[1];
        Int32 subID = (*mAuxData)[0];

        if (msg == RS_MESSAGE_TO_CLIENT_USER) {
            if ((size >> 2) >= rbuf->GetLength()) {
                rbuf = ArrayOf<Int32>::Alloc((size + 3) >> 2);
            }
            if (mRS->NContextGetUserMessage(mRS->mContext, rbuf) != RS_MESSAGE_TO_CLIENT_USER) {
                Slogger::E(LOG_TAG, "Error processing message from RenderScript.");
                return E_RS_DRIVER_EXCEPTION;
            }

            if(mRS->mMessageCallback != NULL) {
                mRS->mMessageCallback->mData = rbuf;
                mRS->mMessageCallback->mID = subID;
                mRS->mMessageCallback->mLength = size;
                mRS->mMessageCallback->Run();
            }
            else {
                Slogger::E(LOG_TAG, "Received a message from the script with no message handler installed.");
                return E_RS_INVALID_STATE_EXCEPTION;
            }
            continue;
        }

        if (msg == RS_MESSAGE_TO_CLIENT_ERROR) {
            String e = mRS->nContextGetErrorMessage(mRS->mContext);

            // Throw RSRuntimeException under the following conditions:
            //
            // 1) It is an unknown fatal error.
            // 2) It is a debug fatal error, and we are not in a
            //    debug context.
            // 3) It is a debug fatal error, and we do not have an
            //    error callback.
            if (subID >= RS_ERROR_FATAL_UNKNOWN || (subID >= RS_ERROR_FATAL_DEBUG &&
                    (mRS->mContextType != RenderScriptContextType_DEBUG || mRS->mErrorCallback == NULL))) {
                Slogger::E(LOG_TAG, "Fatal error %d, details: %s", subID, e.string());
                return E_RS_RUNTIME_EXCEPTION;
            }

            if(mRS->mErrorCallback != NULL) {
                mRS->mErrorCallback->mErrorMessage = e;
                mRS->mErrorCallback->mErrorNum = subID;
                mRS->mErrorCallback->Run();
            }
            else {
                Slogger::E(LOG_TAG, "non fatal RS error, %s", e.string());
                // Do not throw here. In these cases, we do not have
                // a fatal error.
            }
            continue;
        }

        if (msg == RS_MESSAGE_TO_CLIENT_NEW_BUFFER) {
            if (mRS->NContextGetUserMessage(mRS->mContext, rbuf) != RS_MESSAGE_TO_CLIENT_NEW_BUFFER) {
                Slogger::E(LOG_TAG, "Error processing message from RenderScript.");
                return E_RS_RUNTIME_EXCEPTION;
            }
            Int64 bufferID = ((Int64)(*rbuf)[1] << 32L) + ((Int64)(*rbuf)[0] & 0xffffffffL);
            Allocation::SendBufferNotification(bufferID);
            continue;
        }

        // 2: teardown.
        // But we want to avoid starving other threads during
        // teardown by yielding until the next line in the destructor
        // can execute to set mRun = false
        // try {
        Sleep(1, 0);
        // } catch(InterruptedException e) {
        // }
    }
    //Log.d(LOG_TAG, "MessageThread exiting.");
    return NOERROR;
}


//==========================================================
// RenderScript
//==========================================================

Int64 RenderScript::TRACE_TAG;
const String RenderScript::LOG_TAG("RenderScript_jni");
const Boolean RenderScript::DEBUG;
const Boolean RenderScript::LOG_ENABLED;
Boolean RenderScript::sInitialized = FALSE;
Int32 RenderScript::sPointerSize = 0;
AutoPtr<IFile> RenderScript::sCacheDir;
const Int64 RenderScript::sMinorID;
RenderScript::StaticInitializer RenderScript::sInitializer;

RenderScript::RenderScript()
    : mDev(0)
    , mContext(0)
{}

RenderScript::~RenderScript()
{}

ECode RenderScript::constructor(
    /* [in] */ IContext* ctx)
{
    mContextType = RenderScriptContextType_NORMAL;
    if (ctx != NULL) {
        ctx->GetApplicationContext((IContext**)&mApplicationContext);
    }
    CReentrantReadWriteLock::New((IReentrantReadWriteLock**)&mRWLock);
    return NOERROR;
}

CAR_INTERFACE_IMPL(RenderScript, Object, IRenderScript)

Int64 RenderScript::NDeviceCreate()
{
    return (Int64)(uintptr_t)rsDeviceCreate();
}

void RenderScript::NDeviceDestroy(
    /* [in] */ Int64 dev)
{
    rsDeviceDestroy((RsDevice)dev);
}

void RenderScript::NDeviceSetConfig(
    /* [in] */ Int64 dev,
    /* [in] */ Int32 param,
    /* [in] */ Int32 value)
{
    return rsDeviceSetConfig((RsDevice)dev, (RsDeviceParam)param, value);
}

Int32 RenderScript::NContextGetUserMessage(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int32>* data)
{
    Int32 len = data->GetLength();
    Slogger::D(LOG_TAG, "nContextGetMessage, con(%p), len(%i)", (RsContext)con, len);
    size_t receiveLen;
    uint32_t subID;
    int id = rsContextGetMessage((RsContext)con,
            (void*)data->GetPayload(), len * 4,
            &receiveLen, sizeof(receiveLen),
            &subID, sizeof(subID));
    if (!id && receiveLen) {
        ALOGV("message receive buffer too small.  %i", receiveLen);
    }
    return id;
}

String RenderScript::NContextGetErrorMessage(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextGetErrorMessage, con(%p)", (RsContext)con);
    char buf[1024];

    size_t receiveLen;
    uint32_t subID;
    int id = rsContextGetMessage((RsContext)con,
            buf, sizeof(buf),
            &receiveLen, sizeof(receiveLen),
            &subID, sizeof(subID));
    if (!id && receiveLen) {
        ALOGV("message receive buffer too small.  %i", receiveLen);
    }
    return String(buf);
}

Int32 RenderScript::NContextPeekMessage(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int32>* subID)
{
    Slogger::D(LOG_TAG, "nContextPeekMessage, con(%p)", (RsContext)con);
    size_t receiveLen;
    uint32_t subID;
    int id = rsContextPeekMessage((RsContext)con, &receiveLen, sizeof(receiveLen),
            &subID, sizeof(subID));
    (*subID)[0] = (Int32)subID;
    (*subID)[1] = (Int32)receiveLen;
    return id;
}

void RenderScript::NContextInitToClient(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextInitToClient, con(%p)", (RsContext)con);
    rsContextInitToClient((RsContext)con);
}

void RenderScript::NContextDeinitToClient(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextDeinitToClient, con(%p)", (RsContext)con);
    rsContextDeinitToClient((RsContext)con);
}

Int64 RenderScript::GetMinorID()
{
    return sMinorID;
}

void RenderScript::SetupDiskCache(
    /* [in] */ IFile* cacheDir)
{
    if (!sInitialized) {
        Slogger::E(LOG_TAG, "RenderScript.setupDiskCache() called when disabled");
        return;
    }

    // Defer creation of cache path to nScriptCCreate().
    mCacheDir = cacheDir;
}

Int64 RenderScript::RsnContextCreateGL(
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

    Slogger::D(LOG_TAG, "nContextCreateGL");
    return (Int64)(uintptr_t)rsContextCreateGL((RsDevice)dev, ver, sdkVer, sc, dpi);
}

Int64 RenderScript::NContextCreateGL(
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
    return RsnContextCreateGL(dev, ver, sdkVer, colorMin, colorPref,
            alphaMin, alphaPref, depthMin, depthPref,
            stencilMin, stencilPref,
            samplesMin, samplesPref, samplesQ, dpi);
}

Int64 RenderScript::RsnContextCreate(
    /* [in] */ Int64 dev,
    /* [in] */ Int32 ver,
    /* [in] */ Int32 sdkVer,
    /* [in] */ Int32 contextType)
{
    Slogger::D(LOG_TAG, "nContextCreate");
    return (Int64)(uintptr_t)rsContextCreate((RsDevice)dev, ver, sdkVer, (RsContextType)ct, 0);
}

Int64 RenderScript::NContextCreate(
    /* [in] */ Int64 dev,
    /* [in] */ Int32 ver,
    /* [in] */ Int32 sdkVer,
    /* [in] */ Int32 contextType)
{
    AutoLock lock(this);
    return RsnContextCreate(dev, ver, sdkVer, contextType);
}

void RenderScript::RsnContextDestroy(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextDestroy, con(%p)", (RsContext)con);
    rsContextDestroy((RsContext)con);
}

ECode RenderScript::NContextDestroy()
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    // take teardown lock
    // teardown lock can only be taken when no objects are being destroyed
    AutoPtr<ILock> wlock;
    mRWLock->WriteLock((ILock**)&wlock);
    wlock->Lock();

    Int64 curCon = mContext;
    // context is considered dead as of this point
    mContext = 0;

    wlock->Unlock();
    RsnContextDestroy(curCon);
    return NOERROR;
}

void RenderScript::RsnContextSetSurface(
    /* [in] */ Int64 con,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurface* wnd)
{
    Slogger::D(LOG_TAG, "nContextSetSurface, con(%p), width(%i), height(%i), surface(%p)",
            (RsContext)con, width, height, (Surface *)wnd);

    android::sp<ANativeWindow> window = NULL;
    if (wnd != NULL) {
        Int64 result;
        wnd->GetNativeWindow(&result);
        window = android_view_Surface_getNativeWindow(_env, wnd).get();
    }

    rsContextSetSurface((RsContext)con, width, height, window);
}

android::sp<ANativeWindow> RenderScript::SurfaceGetNativeWindow(
    /* [in] */ ISurface* surface)
{
    if (surface == NULL)
        return NULL;
    Surface* surfImpl = (Surface*)surface.Get();
    android::sp<android::Surface> result = reinterpret_cast<android::Surface*>(surfImpl->mNativeObject);

    return result;
}

ECode RenderScript::NContextSetSurface(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurface* sur)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextSetSurface(mContext, w, h, sur);
    return NOERROR;
}

void RenderScript::RsnContextSetSurfaceTexture(
    /* [in] */ Int64 con,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurfaceTexture* sur)
{
    assert(0);
}

ECode RenderScript::NContextSetSurfaceTexture(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurfaceTexture* sur)
{
    AutoLock lock(this);
    FAIL_RETURN(validate())
    RsnContextSetSurfaceTexture(mContext, w, h, sur);
    return NOERROR;
}

void RenderScript::RsnContextSetPriority(
    /* [in] */ Int64 con,
    /* [in] */ Int32 p)
{
    Slogger::D(LOG_TAG, "ContextSetPriority, con(%p), priority(%i)", (RsContext)con, p);
    rsContextSetPriority((RsContext)con, p);
}

ECode RenderScript::NContextSetPriority(
    /* [in] */ Int32 p)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextSetPriority(mContext, p);
    return NOERROR;
}

void RenderScript::RsnContextDump(
    /* [in] */ Int64 con,
    /* [in] */ Int32 bits)
{
    Slogger::D(LOG_TAG, "nContextDump, con(%p)  bits(%i)", (RsContext)con, bits);
    rsContextDump((RsContext)con, bits);
}

ECode RenderScript::NContextDump(
    /* [in] */ Int32 bits)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextDump(mContext, bits);
    return NOERROR;
}

void RenderScript::RsnContextFinish(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextFinish, con(%p)", (RsContext)con);
    rsContextFinish((RsContext)con);
}

ECode RenderScript::NContextFinish()
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextFinish(mContext);
    return NOERROR;
}

void RenderScript::RsnContextSendMessage(
    /* [in] */ Int64 con,
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<Int32>* data)
{
    Slogger::D(LOG_TAG, "nContextSendMessage, con(%p), id(%i), len(%i)", (RsContext)con, id, len);
    rsContextSendMessage((RsContext)con, id, (const uint8_t *)data->GetPayload(), len * sizeof(int));
}

ECode RenderScript::NContextSendMessage(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<Int32>* data)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextSendMessage(mContext, id, data);
    return NOERROR;
}

void RenderScript::RsnContextBindRootScript(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script)
{
    Slogger::D(LOG_TAG, "nContextBindRootScript, con(%p), script(%p)", (RsContext)con, (RsScript)script);
    rsContextBindRootScript((RsContext)con, (RsScript)script);
}

ECode RenderScript::NContextBindRootScript(
    /* [in] */ Int64 script)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindRootScript(mContext, script);
    return NOERROR;
}

void RenderScript::RsnContextBindSampler(
    /* [in] */ Int64 con,
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 slot)
{
    assert(0);
}

ECode RenderScript::NContextBindSampler(
    /* [in] */ Int32 sampler,
    /* [in] */ Int32 slot)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindSampler(mContext, sampler, slot);
    return NOERROR;
}

void RenderScript::RsnContextBindProgramStore(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pfs)
{
    Slogger::D(LOG_TAG, "nContextBindProgramStore, con(%p), pfs(%p)", (RsContext)con, (RsProgramStore)pfs);
    rsContextBindProgramStore((RsContext)con, (RsProgramStore)pfs);
}

ECode RenderScript::NContextBindProgramStore(
    /* [in] */ Int64 pfs)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindProgramStore(mContext, pfs);
    return NOERROR;
}

void RenderScript::RsnContextBindProgramFragment(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pf)
{
    Slogger::D(LOG_TAG, "nContextBindProgramFragment, con(%p), pf(%p)", (RsContext)con, (RsProgramFragment)pf);
    rsContextBindProgramFragment((RsContext)con, (RsProgramFragment)pf);
}

ECode RenderScript::NContextBindProgramFragment(
    /* [in] */ Int64 pf)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindProgramFragment(mContext, pf);
    return NOERROR;
}

void RenderScript::RsnContextBindProgramVertex(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pv)
{
    Slogger::D(LOG_TAG, "nContextBindProgramVertex, con(%p), pf(%p)", (RsContext)con, (RsProgramVertex)pf);
    rsContextBindProgramVertex((RsContext)con, (RsProgramVertex)pf);
}

ECode RenderScript::NContextBindProgramVertex(
    /* [in] */ Int64 pv)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindProgramVertex(mContext, pv);
    return NOERROR;
}

void RenderScript::RsnContextBindProgramRaster(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pr)
{
    Slogger::D(LOG_TAG, "nContextBindProgramRaster, con(%p), pf(%p)", (RsContext)con, (RsProgramRaster)pf);
    rsContextBindProgramRaster((RsContext)con, (RsProgramRaster)pf);
}

ECode RenderScript::NContextBindProgramRaster(
    /* [in] */ Int64 pr)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextBindProgramRaster(mContext, pr);
    return NOERROR;
}

void RenderScript::RsnContextPause(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextPause, con(%p)", (RsContext)con);
    rsContextPause((RsContext)con);
}

ECode RenderScript::NContextPause()
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextPause(mContext);
    return NOERROR;
}

void RenderScript::RsnContextResume(
    /* [in] */ Int64 con)
{
    Slogger::D(LOG_TAG, "nContextResume, con(%p)", (RsContext)con);
    rsContextResume((RsContext)con);
}

ECode RenderScript::NContextResume()
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnContextResume(mContext);
    return NOERROR;
}

void RenderScript::RsnAssignName(
    /* [in] */ Int64 con,
    /* [in] */ Int64 obj,
    /* [in] */ ArrayOf<Byte>* name)
{
    Slogger::D(LOG_TAG, "nAssignName, con(%p), obj(%p)", (RsContext)con, (void *)obj);
    rsAssignName((RsContext)con, (void *)obj, (const char *)name->GetPayload(), name->GetLength());
}

ECode RenderScript::NAssignName(
    /* [in] */ Int64 obj,
    /* [in] */ ArrayOf<Byte>* name)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAssignName(mContext, obj, name);
}

String RenderScript::RsnGetName(
    /* [in] */ Int64 con,
    /* [in] */ Int64 obj)
{
    Slogger::D(LOG_TAG, "nGetName, con(%p), obj(%p)", (RsContext)con, (void *)obj);
    const char *name = NULL;
    rsaGetName((RsContext)con, (void *)obj, &name);
    if(name == NULL || strlen(name) == 0) {
        return String(NULL);
    }
    return String(name);
}

ECode RenderScript::NGetName(
    /* [in] */ Int64 obj,
    /* [out] */ String* name)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(name)
    *name = rsnGetName(mContext, obj);
    return NOERROR;
}

void RenderScript::RsnObjDestroy(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id)
{
    Slogger::D(LOG_TAG, "nObjDestroy, con(%p) obj(%p)", (RsContext)con, (void *)obj);
    rsObjDestroy((RsContext)con, (void *)obj);
}

void RenderScript::NObjDestroy(
    /* [in] */ Int64 id)
{
    // There is a race condition here.  The calling code may be run
    // by the gc while teardown is occuring.  This protects againts
    // deleting dead objects.
    if (mContext != 0) {
        rsnObjDestroy(mContext, id);
    }
}

Int64 RenderScript::RsnElementCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 kind,
    /* [in] */ Boolean norm,
    /* [in] */ Int32 vecSize)
{
    Slogger::D(LOG_TAG, "nElementCreate, con(%p), type(%i), kind(%i), norm(%i), size(%i)", (RsContext)con, type, kind, norm, size);
    return (Int64)(uintptr_t)rsElementCreate((RsContext)con, (RsDataType)type, (RsDataKind)kind, norm, size);
}

ECode RenderScript::NElementCreate(
    /* [in] */ Int64 type,
    /* [in] */ Int32 kind,
    /* [in] */ Boolean norm,
    /* [in] */ Int32 vecSize,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = RsnElementCreate(mContext, type, kind, norm, vecSize);
    return NOERROR;
}

Int64 RenderScript::RsnElementCreate2(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int64>* elements,
    /* [in] */ ArrayOf<String>* names,
    /* [in] */ ArrayOf<Int32>* _arraySizes)
{
    Int32 fieldCount = elements->GetLength();
    Slogger::D(LOG_TAG, "nElementCreate2, con(%p)", (RsContext)con);

    // jlong *jIds = _env->GetLongArrayElements(_ids, NULL);
    // jint *jArraySizes = _env->GetIntArrayElements(_arraySizes, NULL);

    RsElement *ids = (RsElement*)malloc(fieldCount * sizeof(RsElement));
    uint32_t *arraySizes = (uint32_t *)malloc(fieldCount * sizeof(uint32_t));

    for(Int32 i = 0; i < fieldCount; i ++) {
        ids[i] = (RsElement)(*elements)[i];
        arraySizes[i] = (uint32_t)(*_arraySizes)[i];
    }

    AutoStringArrayToUTF8 names(_names, fieldCount);

    const char **nameArray = names.c_str();
    size_t *sizeArray = names.c_str_len();

    Int64 id = (Int64)(uintptr_t)rsElementCreate2((RsContext)con,
            (const RsElement *)ids, fieldCount,
            nameArray, fieldCount * sizeof(size_t),  sizeArray,
            (const uint32_t *)arraySizes, fieldCount);

    free(ids);
    free(arraySizes);

    return (Int64)(uintptr_t)id;
}

ECode RenderScript::NElementCreate2(
    /* [in] */ ArrayOf<Int64>* elements,
    /* [in] */ ArrayOf<String>* names,
    /* [in] */ ArrayOf<Int32>* arraySizes,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = RsnElementCreate2(mContext, elements, names, arraySizes);
    return NOERROR;
}

void RenderScript::RsnElementGetNativeData(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int32>* _elementData)
{
    Int32 dataSize = _elementData->GetLength();
    Slogger::D(LOG_TAG, "nElementGetNativeData, con(%p)", (RsContext)con);

    // we will pack mType; mKind; mNormalized; mVectorSize; NumSubElements
    assert(dataSize == 5);

    uintptr_t elementData[5];
    rsaElementGetNativeData((RsContext)con, (RsElement)id, elementData, dataSize);

    for(Int32 i = 0; i < dataSize; i ++) {
        const Int32 data = (Int32)elementData[i];
        (*_elementData)[i] = data;
    }
}

ECode RenderScript::NElementGetNativeData(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int32>* elementData)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnElementGetNativeData(mContext, id, elementData);
    return NOERROR;
}

void RenderScript::RsnElementGetSubElements(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* _IDs,
    /* [in] */ ArrayOf<String>* _names,
    /* [in] */ ArrayOf<Int32>* _arraySizes)
{
    Int32 dataSize = _IDs->GetLength();
    Slogger::D(LOG_TAG, "nElementGetSubElements, con(%p)", (RsContext)con);

    uintptr_t *ids = (uintptr_t*)malloc(dataSize * sizeof(uintptr_t));
    const char **names = (const char **)malloc(dataSize * sizeof(const char *));
    uint32_t *arraySizes = (uint32_t *)malloc(dataSize * sizeof(uint32_t));

    rsaElementGetSubElements((RsContext)con, (RsElement)id, ids, names, arraySizes, (uint32_t)dataSize);

    for(uint32_t i = 0; i < dataSize; i++) {
        const Int64 id = (Int64)(uintptr_t)ids[i];
        const Int32 arraySize = (Int32)arraySizes[i];
        (*_names)[i] = String(names[i]);
        (*_IDs)[i] = id;
        (*_arraySizes)[i] = arraySize;
    }

    free(ids);
    free(names);
    free(arraySizes);
}

ECode RenderScript::NElementGetSubElements(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* IDs,
    /* [in] */ ArrayOf<String>* names,
    /* [in] */ ArrayOf<Int32>* arraySizes)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnElementGetSubElements(mContext, id, IDs, names, arraySizes);
    return NOERROR;
}

Int64 RenderScript::RsnTypeCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 eid,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Boolean mips,
    /* [in] */ Boolean faces,
    /* [in] */ Int32 yuv)
{
    Slogger::D(LOG_TAG, "nTypeCreate, con(%p) eid(%p), x(%i), y(%i), z(%i), mips(%i), faces(%i), yuv(%i)",
            (RsContext)con, eid, dimx, dimy, dimz, mips, faces, yuv);

    return (Int64)(uintptr_t)rsTypeCreate((RsContext)con, (RsElement)eid, dimx, dimy, dimz, mips, faces, yuv);
}

ECode RenderScript::NTypeCreate(
    /* [in] */ Int64 eid,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Boolean mips,
    /* [in] */ Boolean faces,
    /* [in] */ Int32 yuv,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = RsnTypeCreate(mContext, eid, x, y, z, mips, faces, yuv);
    return NOERROR;
}

void RenderScript::RsnTypeGetNativeData(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* _typeData)
{
    // We are packing 6 items: mDimX; mDimY; mDimZ;
    // mDimLOD; mDimFaces; mElement; into typeData
    Int32 elementCount = _typeData->GetLength();

    assert(elementCount == 6);
    Slogger::D(LOG_TAG, "nTypeGetNativeData, con(%p)", (RsContext)con);

    uintptr_t typeData[6];
    rsaTypeGetNativeData((RsContext)con, (RsType)id, typeData, 6);

    for(Int32 i = 0; i < elementCount; i ++) {
        const Int64 data = (Int64)(uintptr_t)typeData[i];
        (*_typeData)[i] = data;
    }
}

ECode RenderScript::NTypeGetNativeData(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* typeData)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnTypeGetNativeData(mContext, id, typeData);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateTyped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 usage,
    /* [in] */ Int64 pointer)
{
    Slogger::D(LOG_TAG, "nAllocationCreateTyped, con(%p), type(%p), mip(%i), usage(%i), ptr(%p)",
            (RsContext)con, (RsElement)type, mips, usage, (void *)pointer);
    return (Int64)(uintptr_t) rsAllocationCreateTyped((RsContext)con, (RsType)type,
            (RsAllocationMipmapControl)mips, (uint32_t)usage, (uintptr_t)pointer);
}

ECode RenderScript::NAllocationCreateTyped(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 usage,
    /* [in] */ Int64 pointer,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCreateTyped(mContext, type, mip, usage, pointer);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage)
{
    SkBitmap const * nativeBitmap = reinterpret_cast<SkBitmap const *>(((CBitmap*)bmp.Get())->mNativeBitmap);
    const SkBitmap& bitmap(*nativeBitmap);

    bitmap.lockPixels();
    const void* ptr = bitmap.getPixels();
    Int64 id = (Int64)(uintptr_t)rsAllocationCreateFromBitmap((RsContext)con,
            (RsType)type, (RsAllocationMipmapControl)mip,
            ptr, bitmap.getSize(), usage);
    bitmap.unlockPixels();
    return id;
}

ECode RenderScript::NAllocationCreateFromBitmap(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCreateFromBitmap(mContext, type, mip, bmp, usage);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateBitmapBackedAllocation(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage)
{
    SkBitmap const * nativeBitmap = reinterpret_cast<SkBitmap const *>(((CBitmap*)bmp.Get())->mNativeBitmap);
    const SkBitmap& bitmap(*nativeBitmap);

    bitmap.lockPixels();
    const void* ptr = bitmap.getPixels();
    Int64 id = (Int64)(uintptr_t)rsAllocationCreateTyped((RsContext)con,
            (RsType)type, (RsAllocationMipmapControl)mip,
            (uint32_t)usage, (uintptr_t)ptr);
    bitmap.unlockPixels();
    return id;
}

ECode RenderScript::NAllocationCreateBitmapBackedAllocation(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCreateBitmapBackedAllocation(mContext, type, mip, bmp, usage);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCubeCreateFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage)
{
    SkBitmap const * nativeBitmap = reinterpret_cast<SkBitmap const *>(((CBitmap*)bmp.Get())->mNativeBitmap);
    const SkBitmap& bitmap(*nativeBitmap);

    bitmap.lockPixels();
    const void* ptr = bitmap.getPixels();
    Int64 id = (Int64)(uintptr_t)rsAllocationCubeCreateFromBitmap((RsContext)con,
            (RsType)type, (RsAllocationMipmapControl)mip,
            ptr, bitmap.getSize(), usage);
    bitmap.unlockPixels();
    return id;
}

ECode RenderScript::NAllocationCubeCreateFromBitmap(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCubeCreateFromBitmap(mContext, type, mip, bmp, usage);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateBitmapRef(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ IBitmap* bmp)
{
    assert(0);
    return -1;
}

ECode RenderScript::NAllocationCreateBitmapRef(
    /* [in] */ Int64 type,
    /* [in] */ IBitmap* bmp,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCreateBitmapRef(mContext, type, bmp);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ Int32 mips,
    /* [in] */ Int32 assetStream,
    /* [in] */ Int32 usage)
{
    assert(0);
    return -1;
}

ECode RenderScript::NAllocationCreateFromAssetStream(
    /* [in] */ Int32 mips,
    /* [in] */ Int32 assetStream,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationCreateFromAssetStream(mContext, mips, assetStream, usage);
    return NOERROR;
}

void RenderScript::RsnAllocationCopyToBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp)
{
    SkBitmap const * nativeBitmap = reinterpret_cast<SkBitmap const *>(((CBitmap*)bmp.Get())->mNativeBitmap);
    const SkBitmap& bitmap(*nativeBitmap);

    bitmap.lockPixels();
    void* ptr = bitmap.getPixels();
    rsAllocationCopyToBitmap((RsContext)con, (RsAllocation)alloc, ptr, bitmap.getSize());
    bitmap.unlockPixels();
    bitmap.notifyPixelsChanged();
}

ECode RenderScript::NAllocationCopyToBitmap(
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationCopyToBitmap(mContext, alloc, bmp);
    return NOERROR;
}

void RenderScript::RsnAllocationSyncAll(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 src)
{
    Slogger::D(LOG_TAG, "nAllocationSyncAll, con(%p), a(%p), bits(0x%08x)",
            (RsContext)con, (RsAllocation)a, bits);
    rsAllocationSyncAll((RsContext)con, (RsAllocation)a, (RsAllocationUsageType)bits);
}

ECode RenderScript::NAllocationSyncAll(
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 src)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationSyncAll(mContext, alloc, src);
    return NOERROR;
}

AutoPtr<ISurface> RsnAllocationGetSurface(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nAllocationGetSurface, con(%p), a(%p)", (RsContext)con, (RsAllocation)a);

    android::IGraphicBufferProducer *v = (android::IGraphicBufferProducer *)rsAllocationGetSurface(
            (RsContext)con, (RsAllocation)a);
    android::sp<android::IGraphicBufferProducer> bp = v;
    v->decStrong(NULL);

    android::sp<android::Surface> surface(new android::Surface(bufferProducer, TRUE));
    if (surface == NULL) {
        return NULL;
    }

    AutoPtr<ISurface> surfaceObj;
    CSurface::New((Int64)surface.get(), (ISurface**)&surfaceObj);
    return surfaceObj;
}

ECode NAllocationGetSurface(
    /* [in] */ Int64 alloc,
    /* [out] */ ISurface** surface)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;
    FAIL_RETURN(Validate())
    AutoPtr<ISurface> s = RsnAllocationGetSurface(mContext, alloc);
    *surface = s;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

void RenderScript::RsnAllocationSetSurface(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ ISurface* sur)
{
    Slogger::("nAllocationSetSurface, con(%p), alloc(%p), surface(%p)",
            (RsContext)con, (RsAllocation)alloc, (Surface *)sur);

    android::sp<android::Surface> s;
    if (sur != 0) {
        Surface* surfaceObj = (Surface*)sur;
        AutoLock lock(surfaceObj);
        s = reinterpret_cast<android::Surface*>(surfaceObj->mNativeObject);
    }

    rsAllocationSetSurface((RsContext)con, (RsAllocation)alloc, static_cast<ANativeWindow *>(s.get()));
}

ECode RenderScript::NAllocationSetSurface(
    /* [in] */ Int64 alloc,
    /* [in] */ ISurface* sur)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationSetSurface(mContext, alloc, sur);
    return NOERROR;
}

void RenderScript::RsnAllocationIoSend(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nAllocationIoSend, con(%p), alloc(%p)", (RsContext)con, alloc);
    rsAllocationIoSend((RsContext)con, (RsAllocation)alloc);
}

ECode RenderScript::NAllocationIoSend(
    /* [in] */ Int64 alloc)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationIoSend(mContext, alloc);
    return NOERROR;
}

void RenderScript::RsnAllocationIoReceive(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nAllocationIoReceive, con(%p), alloc(%p)", (RsContext)con, alloc);
    rsAllocationIoReceive((RsContext)con, (RsAllocation)alloc);
}

ECode RenderScript::NAllocationIoReceive(
    /* [in] */ Int64 alloc)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationIoReceive(mContext, alloc);
    return NOERROR;
}

void RenderScript::RsnAllocationGenerateMipmaps(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nAllocationGenerateMipmaps, con(%p), a(%p)", (RsContext)con, (RsAllocation)alloc);
    rsAllocationGenerateMipmaps((RsContext)con, (RsAllocation)alloc);
}

ECode RenderScript::NAllocationGenerateMipmaps(
    /* [in] */ Int64 alloc)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationGenerateMipmaps(mContext, alloc);
    return NOERROR;
}

void RenderScript::RsnAllocationCopyFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp)
{
    SkBitmap const * nativeBitmap = reinterpret_cast<SkBitmap const *>(((CBitmap*)bmp.Get())->mNativeBitmap);
    const SkBitmap& bitmap(*nativeBitmap);
    int w = bitmap.width();
    int h = bitmap.height();

    bitmap.lockPixels();
    const void* ptr = bitmap.getPixels();
    rsAllocation2DData((RsContext)con, (RsAllocation)alloc, 0, 0,
            0, RS_ALLOCATION_CUBEMAP_FACE_POSITIVE_X,
            w, h, ptr, bitmap.getSize(), 0);
    bitmap.unlockPixels();
}

ECode RenderScript::NAllocationCopyFromBitmap(
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationCopyFromBitmap(mContext, alloc, bmp);
    return NOERROR;
}

void RenderScript::RsnAllocationData1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    Slogger::D(LOG_TAG, "nAllocation1DData, con(%p), adapter(%p), offset(%i), count(%i), sizeBytes(%i), dataType(%i)",
            (RsContext)con, (RsAllocation)alloc, offset, count, sizeBytes, dataType);
    PER_ARRAY_TYPE(NULL, rsAllocation1DData, TRUE, (RsContext)con, alloc, offset, lod, count, ptr, sizeBytes);
}

ECode RenderScript::NAllocationData1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData1D(mContext, id, off, mip, count, d, sizeBytes, dt.mID);
    return NOERROR;
}

void RenderScript::RsnAllocationElementData1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 compIdx,
    /* [in] */ ArrayOf<Byte>* d,
    /* [in] */ Int32 sizeBytes)
{
    Int32 len = d->GetLength();
    Slogger::D(LOG_TAG, "nAllocationElementData1D, con(%p), alloc(%p), offset(%i), comp(%i), len(%i), sizeBytes(%i)",
            (RsContext)con, (RsAllocation)alloc, offset, compIdx, len, sizeBytes);
    rsAllocation1DElementData((RsContext)con, (RsAllocation)alloc, offset, lod, d->GetPayload(), sizeBytes, compIdx);
}

ECode RenderScript::NAllocationElementData1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 compIdx,
    /* [in] */ ArrayOf<Byte>* d,
    /* [in] */ Int32 sizeBytes)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationElementData1D(mContext, id, xoff, mip, compIdx, d, sizeBytes);
    return NOERROR;
}

void RenderScript::RsnAllocationData2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 dstAlloc,
    /* [in] */ Int32 dstXoff,
    /* [in] */ Int32 dstYoff,
    /* [in] */ Int32 dstMip,
    /* [in] */ Int32 dstFace,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int64 srcAlloc,
    /* [in] */ Int32 srcXoff,
    /* [in] */ Int32 srcYoff,
    /* [in] */ Int32 srcMip,
    /* [in] */ Int32 srcFace)
{
    Slogger::D(LOG_TAG, "nAllocation2DData_s, con(%p), dstAlloc(%p), dstXoff(%i), dstYoff(%i),"
            " dstMip(%i), dstFace(%i), width(%i), height(%i),"
            " srcAlloc(%p), srcXoff(%i), srcYoff(%i), srcMip(%i), srcFace(%i)",
            (RsContext)con, (RsAllocation)dstAlloc, dstXoff, dstYoff, dstMip, dstFace,
            width, height, (RsAllocation)srcAlloc, srcXoff, srcYoff, srcMip, srcFace);

    rsAllocationCopy2DRange((RsContext)con,
            (RsAllocation)dstAlloc,
            dstXoff, dstYoff,
            dstMip, dstFace,
            width, height,
            (RsAllocation)srcAlloc,
            srcXoff, srcYoff,
            srcMip, srcFace);
}

ECode RenderScript::NAllocationData2D(
    /* [in] */ Int64 dstAlloc,
    /* [in] */ Int32 dstXoff,
    /* [in] */ Int32 dstYoff,
    /* [in] */ Int32 dstMip,
    /* [in] */ Int32 dstFace,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int64 srcAlloc,
    /* [in] */ Int32 srcXoff,
    /* [in] */ Int32 srcYoff,
    /* [in] */ Int32 srcMip,
    /* [in] */ Int32 srcFace)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData2D(mContext,
            dstAlloc, dstXoff, dstYoff,
            dstMip, dstFace,
            width, height,
            srcAlloc, srcXoff, srcYoff,
            srcMip, srcFace);
    return NOERROR;
}

void RenderScript::RsnAllocationData2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    RsAllocationCubemapFace face = (RsAllocationCubemapFace)_face;
    Slogger::D(LOG_TAG, "nAllocation2DData, con(%p), adapter(%p), xoff(%i), yoff(%i), w(%i), h(%i), len(%i) type(%i)",
            (RsContext)con, alloc, xoff, yoff, w, h, sizeBytes, dataType);
    PER_ARRAY_TYPE(NULL, rsAllocation2DData, TRUE, (RsContext)con, alloc, xoff, yoff, lod, face, w, h, ptr, sizeBytes, 0);
}

ECode RenderScript::NAllocationData2D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData2D(mContext, id, xoff, yoff, mip, face, w, h, d, sizeBytes, dt);
    return NOERROR;
}

void RenderScript::RsnAllocationData2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ IBitmap* b)
{
    assert(0);
}

ECode RenderScript::NAllocationData2D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ IBitmap* b)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData2D(mContext, id, xoff, yoff, mip, face, b);
    return NOERROR;
}

void RenderScript::RsnAllocationData3D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 dstAlloc,
    /* [in] */ Int32 dstXoff,
    /* [in] */ Int32 dstYoff,
    /* [in] */ Int32 dstZoff,
    /* [in] */ Int32 dstMip,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int64 srcAlloc,
    /* [in] */ Int32 srcXoff,
    /* [in] */ Int32 srcYoff,
    /* [in] */ Int32 srcZoff,
    /* [in] */ Int32 srcMip)
{
    Slogger::D(LOG_TAG, "nAllocationData3D_alloc, con(%p), dstAlloc(%p), dstXoff(%i), dstYoff(%i),"
            " dstMip(%i), width(%i), height(%i),"
            " srcAlloc(%p), srcXoff(%i), srcYoff(%i), srcMip(%i)",
            (RsContext)con, (RsAllocation)dstAlloc, dstXoff, dstYoff, dstMip,
            width, height, (RsAllocation)srcAlloc, srcXoff, srcYoff, srcMip);

    rsAllocationCopy3DRange((RsContext)con,
            (RsAllocation)dstAlloc,
            dstXoff, dstYoff, dstZoff, dstMip,
            width, height, depth,
            (RsAllocation)srcAlloc,
            srcXoff, srcYoff, srcZoff, srcMip);
}

ECode RenderScript::NAllocationData3D(
    /* [in] */ Int64 dstAlloc,
    /* [in] */ Int32 dstXoff,
    /* [in] */ Int32 dstYoff,
    /* [in] */ Int32 dstZoff,
    /* [in] */ Int32 dstMip,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 depth,
    /* [in] */ Int64 srcAlloc,
    /* [in] */ Int32 srcXoff,
    /* [in] */ Int32 srcYoff,
    /* [in] */ Int32 srcZoff,
    /* [in] */ Int32 srcMip)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData3D(mContext,
            dstAlloc, dstXoff, dstYoff, dstZoff,
            dstMip, width, height, depth,
            srcAlloc, srcXoff, srcYoff, srcZoff, srcMip);
    return NOERROR;
}

void RenderScript::RsnAllocationData3D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 depth,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    Slogger::D(LOG_TAG, "nAllocation3DData, con(%p), alloc(%p), xoff(%i), yoff(%i), zoff(%i), lod(%i), w(%i), h(%i), d(%i), sizeBytes(%i)",
            (RsContext)con, (RsAllocation)alloc, xoff, yoff, zoff, lod, w, h, d, sizeBytes);
    PER_ARRAY_TYPE(NULL, rsAllocation3DData, TRUE, (RsContext)con, alloc, xoff, yoff, zoff, lod, w, h, d, ptr, sizeBytes, 0);
}

ECode RenderScript::NAllocationData3D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 depth,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationData3D(mContext, id, xoff, yoff, zoff, mip, w, h, depth, d, sizeBytes, dt);
    return NOERROR;
}

void RenderScript::RsnAllocationRead(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    Slogger::D(LOG_TAG, "nAllocationRead, con(%p), alloc(%p)", (RsContext)con, (RsAllocation)alloc);
    PER_ARRAY_TYPE(0, rsAllocationRead, FALSE, (RsContext)con, alloc, ptr, len * typeBytes);
}

ECode RenderScript::NAllocationRead(
    /* [in] */ Int64 id,
    /* [in] */ IInterface* d,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationRead(mContext, id, d, dt);
    return NOERROR;
}

void RenderScript::RsnAllocationRead1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    Slogger::D(LOG_TAG, "nAllocation1DRead, con(%p), adapter(%p), offset(%i), count(%i), sizeBytes(%i), dataType(%i)",
            (RsContext)con, alloc, offset, count, sizeBytes, dataType);
    PER_ARRAY_TYPE(0, rsAllocation1DRead, FALSE, (RsContext)con, alloc, offset, lod, count, ptr, sizeBytes);
}

ECode RenderScript::NAllocationRead1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationRead1D(mContext, id, off, mip, count, d, sizeBytes, dt);
    return NOERROR;
}

void RenderScript::RsnAllocationRead2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* data,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dataType)
{
    RsAllocation *alloc = (RsAllocation *)_alloc;
    RsAllocationCubemapFace face = (RsAllocationCubemapFace)_face;
    Slogger::D(LOG_TAG, "nAllocation2DRead, con(%p), adapter(%p), xoff(%i), yoff(%i), w(%i), h(%i), len(%i) type(%i)",
            (RsContext)con, alloc, xoff, yoff, w, h, sizeBytes, dataType);
    PER_ARRAY_TYPE(0, rsAllocation2DRead, FALSE, (RsContext)con, alloc, xoff, yoff, lod, face, w, h, ptr, sizeBytes, 0);
}

ECode RenderScript::NAllocationRead2D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationRead2D(mContext, id, xoff, yoff, mip, face, w, h, d, sizeBytes, dt);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationGetType(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id)
{
    Slogger::D(LOG_TAG, "nAllocationGetType, con(%p), a(%p)", (RsContext)con, (RsAllocation)a);
    return (Int64)(uintptr_t) rsaAllocationGetType((RsContext)con, (RsAllocation)a);
}

ECode RenderScript::NAllocationGetType(
    /* [in] */ Int64 id,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnAllocationGetType(mContext, id);
    return NOERROR;
}

void RenderScript::RsnAllocationResize1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 dimX)
{
    Slogger::D(LOG_TAG, "nAllocationResize1D, con(%p), alloc(%p), sizeX(%i)", (RsContext)con, (RsAllocation)alloc, dimX);
    rsAllocationResize1D((RsContext)con, (RsAllocation)alloc, dimX);
}

ECode RenderScript::NAllocationResize1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 dimX)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnAllocationResize1D(mContext, id, dimX);
    return NOERROR;
}

Int64 RenderScript::RsnFileA3DCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ Int64 native_asset)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(native_asset);
    ALOGV("______nFileA3D %p", asset);

    Int64 id = (Int64)(uintptr_t)rsaFileA3DCreateFromMemory((RsContext)con, asset->getBuffer(false), asset->getLength());
    return id;
}

ECode RenderScript::NFileA3DCreateFromAssetStream(
    /* [in] */ Int64 assetStream,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFileA3DCreateFromAssetStream(mContext, assetStream);
    return NOERROR;
}

Int64 RenderScript::RsnFileA3DCreateFromFile(
    /* [in] */ Int64 con,
    /* [in] */ const String& fileName)
{
    Int64 id = (Int64)(uintptr_t)rsaFileA3DCreateFromFile((RsContext)con, fileName.stirng());
    return id;
}

ECode RenderScript::NFileA3DCreateFromFile(
    /* [in] */ const String& path,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFileA3DCreateFromFile(mContext, path);
    return NOERROR;
}

Int64 RenderScript::RsnFileA3DCreateFromAsset(
    /* [in] */ Int64 con,
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path)
{
    android::AssetManager* mgr = (android::AssetManager*)((CAssetManager*)mgr)->Ni();
    if (mgr == NULL) {
        return 0;
    }

    android::Asset* asset = mgr->open(path.string(), android::Asset::ACCESS_BUFFER);
    if (asset == NULL) {
        return 0;
    }

    Int64 id = (Int64)(uintptr_t)rsaFileA3DCreateFromAsset((RsContext)con, asset);
    return id;
}

ECode RenderScript::NFileA3DCreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFileA3DCreateFromAsset(mContext, mgr, path);
    return NOERROR;
}

Int32 RenderScript::RsnFileA3DGetNumIndexEntries(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D)
{
    int32_t numEntries = 0;
    rsaFileA3DGetNumIndexEntries((RsContext)con, &numEntries, (RsFile)fileA3D);
    return (Int32)numEntries;
}

ECode RenderScript::NFileA3DGetNumIndexEntries(
    /* [in] */ Int64 fileA3D,
    /* [out] */ Int32* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFileA3DGetNumIndexEntries(mContext, fileA3D);
    return NOERROR;
}

void RenderScript::RsnFileA3DGetIndexEntries(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 numEntries,
    /* [in] */ ArrayOf<Int32>* IDs,
    /* [in] */ ArrayOf<String>* names)
{
    ALOGV("______nFileA3D %p", (RsFile) fileA3D);
    RsFileIndexEntry *fileEntries = (RsFileIndexEntry*)malloc((uint32_t)numEntries * sizeof(RsFileIndexEntry));

    rsaFileA3DGetIndexEntries((RsContext)con, fileEntries, (uint32_t)numEntries, (RsFile)fileA3D);

    for(Int32 i = 0; i < numEntries; i ++) {
        (*names)[i] = String(fileEntries[i].objectName);
        (*IDs)[i] = (const Int32*)&fileEntries[i].classID;
    }

    free(fileEntries);
}

ECode RenderScript::NFileA3DGetIndexEntries(
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 numEntries,
    /* [in] */ ArrayOf<Int32>* IDs,
    /* [in] */ ArrayOf<String>* names)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnFileA3DGetIndexEntries(mContext, fileA3D, numEntries, IDs, names);
    return NOERROR;
}

Int64 RenderScript::RsnFileA3DGetEntryByIndex(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 index)
{
    ALOGV("______nFileA3D %p", (RsFile) fileA3D);
    Int64 id = (Int64)(uintptr_t)rsaFileA3DGetEntryByIndex((RsContext)con, (uint32_t)index, (RsFile)fileA3D);
    return id;
}

ECode RenderScript::NFileA3DGetEntryByIndex(
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 index,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFileA3DGetEntryByIndex(mContext, fileA3D, index);
    return NOERROR;
}

Int64 RenderScript::RsnFontCreateFromFile(
    /* [in] */ Int64 con,
    /* [in] */ const String& fileName,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi)
{
    Int64 id = (Int64)(uintptr_t)rsFontCreateFromFile((RsContext)con,
            fileName.string(), fileName.GetLength(),
            fontSize, dpi);
    return id;
}

ECode RenderScript::NFontCreateFromFile(
    /* [in] */ const String& fileName,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFontCreateFromFile(mContext, fileName, size, dpi);
    return NOERROR;
}

Int64 RenderScript::RsnFontCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ const String& name,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [in] */ Int64 native_asset)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(native_asset);
    Int64 id = (Int64)(uintptr_t)rsFontCreateFromMemory((RsContext)con,
            name.string(), name.GetLength(),
            fontSize, dpi,
            asset->getBuffer(false), asset->getLength());
    return id;
}

ECode RenderScript::NFontCreateFromAssetStream(
    /* [in] */ const String& name,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [in] */ Int64 assetStream,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFontCreateFromAssetStream(mContext, name, size, dpi, assetStream);
    return NOERROR;
}

Int64 RenderScript::RsnFontCreateFromAsset(
    /* [in] */ Int64 con,
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi)
{
    android::AssetManager* mgr = (android::AssetManager*)((CAssetManager*)mgr)->Ni();
    if (mgr == NULL) {
        return 0;
    }

    android::Asset* asset = mgr->open(path.string(), android::Asset::ACCESS_BUFFER);
    if (asset == NULL) {
        return 0;
    }

    Int64 id = (Int64)(uintptr_t)rsFontCreateFromMemory((RsContext)con,
            str.string(), str.GetLength(),
            fontSize, dpi,
            asset->getBuffer(false), asset->getLength());
    delete asset;
    return id;
}

ECode RenderScript::NFontCreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnFontCreateFromAsset(mContext, mgr, path, size, dpi);
    return NOERROR;
}

void RenderScript::RsnScriptBindAllocation(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptBindAllocation, con(%p), script(%p), alloc(%p), slot(%i)",
            (RsContext)con, (RsScript)script, (RsAllocation)alloc, slot);
    rsScriptBindAllocation((RsContext)con, (RsScript)script, (RsAllocation)alloc, slot);
}

ECode RenderScript::NScriptBindAllocation(
    /* [in] */ Int64 script,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 slot)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptBindAllocation(mContext, script, alloc, slot);
    return NOERROR;
}

void RenderScript::RsnScriptSetTimeZone(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ ArrayOf<Byte>* timeZone)
{
    Slogger::D(LOG_TAG, "nScriptCSetTimeZone, con(%p), s(%p)", (RsContext)con, (void *)script);

    Int32 length = timeZone->GetLength();
    rsScriptSetTimeZone((RsContext)con, (RsScript)script, (const char *)timeZone->GetPayload(), length);
}

ECode RenderScript::NScriptSetTimeZone(
    /* [in] */ Int64 script,
    /* [in] */ ArrayOf<Byte>* timeZone)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetTimeZone(mContext, script, timeZone);
    return NOERROR;
}

void RenderScript::RsnScriptInvoke(
    /* [in] */ Int64 con,
    /* [in] */ Int64 obj,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptInvoke, con(%p), script(%p)", (RsContext)con, (void *)obj);
    rsScriptInvoke((RsContext)con, (RsScript)obj, slot);
}

ECode RenderScript::NScriptInvoke(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptInvoke(mContext, id, slot);
    return NOERROR;
}

void RenderScript::RsnScriptForEach(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params)
{
    Slogger::D(LOG_TAG, "nScriptForEach, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int32 len = params->GetLength();
    rsScriptForEach((RsContext)con, (RsScript)script, slot, (RsAllocation)ain, (RsAllocation)aout,
            params->GetPayload(), len, NULL, 0);
}

void RenderScript::RsnScriptForEach(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout)
{
    Slogger::D(LOG_TAG, "nScriptForEach, con(%p), s(%p), slot(%i)",
            (RsContext)con, (void *)script, slot);
    rsScriptForEach((RsContext)con, (RsScript)script, slot, (RsAllocation)ain,
            (RsAllocation)aout, NULL, 0, NULL, 0);
}

void RenderScript::RsnScriptForEachClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    Slogger::D(LOG_TAG, "nScriptForEachClipped, con(%p), s(%p), slot(%i)",
            (RsContext)con, (void *)script, slot);
    Int32 len = params->GetLength();
    RsScriptCall sc;
    sc.xStart = xstart;
    sc.xEnd = xend;
    sc.yStart = ystart;
    sc.yEnd = yend;
    sc.zStart = zstart;
    sc.zEnd = zend;
    sc.strategy = RS_FOR_EACH_STRATEGY_DONT_CARE;
    sc.arrayStart = 0;
    sc.arrayEnd = 0;
    rsScriptForEach((RsContext)con, (RsScript)script, slot, (RsAllocation)ain,
            (RsAllocation)aout, params->GetPayload(), len, &sc, sizeof(sc));
}

void RenderScript::RsnScriptForEachClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    Slogger::D(LOG_TAG, "nScriptForEachClipped, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    RsScriptCall sc;
    sc.xStart = xstart;
    sc.xEnd = xend;
    sc.yStart = ystart;
    sc.yEnd = yend;
    sc.zStart = zstart;
    sc.zEnd = zend;
    sc.strategy = RS_FOR_EACH_STRATEGY_DONT_CARE;
    sc.arrayStart = 0;
    sc.arrayEnd = 0;
    rsScriptForEach((RsContext)con, (RsScript)script, slot, (RsAllocation)ain,
            (RsAllocation)aout, NULL, 0, &sc, sizeof(sc));
}

ECode RenderScript::NScriptForEach(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    if (params == NULL) {
        RsnScriptForEach(mContext, id, slot, ain, aout);
    }
    else {
        RsnScriptForEach(mContext, id, slot, ain, aout, params);
    }
    return NOERROR;
}

ECode RenderScript::NScriptForEachClipped(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    if (params == NULL) {
        RsnScriptForEachClipped(mContext, id, slot, ain, aout, xstart, xend, ystart, yend, zstart, zend);
    }
    else {
        RsnScriptForEachClipped(mContext, id, slot, ain, aout, params, xstart, xend, ystart, yend, zstart, zend);
    }
    return NOERROR;
}

void RenderScript::RsnScriptForEachMultiClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Int64>* ains,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    Slogger::D("nScriptForEachMultiClippedV, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);

    Int32 in_len = ains->GetLength();
    Int64* in_ptr = ains->GetPayload();

    RsAllocation *in_allocs = NULL;

    if (sizeof(RsAllocation) == sizeof(jlong)) {
      in_allocs = (RsAllocation*)in_ptr;

    }
    else {
      // Convert from 64-bit jlong types to the native pointer type.

      in_allocs = new RsAllocation[in_len];

      for (int index = in_len; --index >= 0;) {
        in_allocs[index] = (RsAllocation)in_ptr[index];
      }
    }

    Int32 param_len = params->GetLength();

    RsScriptCall sc;
    sc.xStart = xstart;
    sc.xEnd = xend;
    sc.yStart = ystart;
    sc.yEnd = yend;
    sc.zStart = zstart;
    sc.zEnd = zend;
    sc.strategy = RS_FOR_EACH_STRATEGY_DONT_CARE;
    sc.arrayStart = 0;
    sc.arrayEnd = 0;
    rsScriptForEachMulti((RsContext)con, (RsScript)script, slot,
            in_allocs, in_len, (RsAllocation)aout, params->GetPayload(), param_len, &sc, sizeof(sc));

    if (sizeof(RsAllocation) != sizeof(jlong)) {
      delete[] in_allocs;
    }
}

void RenderScript::RsnScriptForEachMultiClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Int64>* ains,
    /* [in] */ Int64 aout,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    Slogger::D(LOG_TAG, "nScriptForEachMultiClipped, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);

    Int32 in_len = ains->GetLength();
    Int64* in_ptr = ains->GetPayload();

    RsAllocation *in_allocs = NULL;

    if (sizeof(RsAllocation) == sizeof(jlong)) {
      in_allocs = (RsAllocation*)ains->GetPayload();

    }
    else {
      // Convert from 64-bit jlong types to the native pointer type.

      in_allocs = new RsAllocation[in_len];

      for (int index = in_len; --index >= 0;) {
        in_allocs[index] = (RsAllocation)in_ptr[index];
      }
    }

    RsScriptCall sc;
    sc.xStart = xstart;
    sc.xEnd = xend;
    sc.yStart = ystart;
    sc.yEnd = yend;
    sc.zStart = zstart;
    sc.zEnd = zend;
    sc.strategy = RS_FOR_EACH_STRATEGY_DONT_CARE;
    sc.arrayStart = 0;
    sc.arrayEnd = 0;

    rsScriptForEachMulti((RsContext)con, (RsScript)script, slot, in_allocs, in_len,
            (RsAllocation)aout, NULL, 0, &sc, sizeof(sc));

    if (sizeof(RsAllocation) != sizeof(jlong)) {
      delete[] in_allocs;
    }
}

ECode RenderScript::NScriptForEachMultiClipped(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Int64>* ains,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    if (params == NULL) {
        RsnScriptForEachMultiClipped(mContext, id, slot, ains, aout, xstart, xend, ystart, yend, zstart, zend);
    }
    else {
        RsnScriptForEachMultiClipped(mContext, id, slot, ains, aout, params, xstart, xend, ystart, yend, zstart, zend);
    }
    return NOERROR;
}

void RenderScript::RsnScriptInvokeV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* data)
{
    Slogger::D(LOG_TAG, "nScriptInvokeV, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int32 len = data->GetLength();
    Byte *ptr = data->GetPayload();
    rsScriptInvokeV((RsContext)con, (RsScript)script, slot, ptr, len);
}

ECode RenderScript::NScriptInvokeV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* params)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptInvokeV(mContext, id, slot, params);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarI(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarI, con(%p), s(%p), slot(%i), val(%i)", (RsContext)con, (void *)script, slot, val);
    rsScriptSetVarI((RsContext)con, (RsScript)script, slot, val);
}

ECode RenderScript::NScriptSetVarI(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarI(mContext, id, slot, val);
    return NOERROR;
}

Int32 RenderScript::RsnScriptGetVarI(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptGetVarI, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    int value = 0;
    rsScriptGetVarV((RsContext)con, (RsScript)script, slot, &value, sizeof(value));
    return value;
}

ECode RenderScript::NScriptGetVarI(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Int32* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptGetVarI(mContext, id, slot);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarJ(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarJ, con(%p), s(%p), slot(%i), val(%lli)",
            (RsContext)con, (void *)script, slot, val);
    rsScriptSetVarJ((RsContext)con, (RsScript)script, slot, val);
}

ECode RenderScript::NScriptSetVarJ(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarJ(mContext, id, slot, val);
    return NOERROR;
}

Int64 RenderScript::RsnScriptGetVarJ(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptGetVarJ, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int64 value = 0;
    rsScriptGetVarV((RsContext)con, (RsScript)script, slot, &value, sizeof(value));
    return value;
}

ECode RenderScript::NScriptGetVarJ(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptGetVarJ(mContext, id, slot);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarF(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Float val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarF, con(%p), s(%p), slot(%i), val(%f)",
            (RsContext)con, (void *)script, slot, val);
    rsScriptSetVarF((RsContext)con, (RsScript)script, slot, val);
}

ECode RenderScript::NScriptSetVarF(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Float val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarF(mContext, id, slot, val);
    return NOERROR;
}

Float RenderScript::RsnScriptGetVarF(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptGetVarF, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Float value = 0;
    rsScriptGetVarV((RsContext)con, (RsScript)script, slot, &value, sizeof(value));
    return value;
}

ECode RenderScript::NScriptGetVarF(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Float* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptGetVarF(mContext, id, slot);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarD(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Double val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarD, con(%p), s(%p), slot(%i), val(%lf)", (RsContext)con, (void *)script, slot, val);
    rsScriptSetVarD((RsContext)con, (RsScript)script, slot, val);
}

ECode RenderScript::NScriptSetVarD(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Double val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarD(mContext, id, slot, val);
    return NOERROR;
}

Double RenderScript::RsnScriptGetVarD(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptGetVarD, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Double value = 0;
    rsScriptGetVarV((RsContext)con, (RsScript)script, slot, &value, sizeof(value));
    return value;
}

ECode RenderScript::NScriptGetVarD(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Double* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptGetVarD(mContext, id, slot);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarV, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int32 len = val->GetLength();
    rsScriptSetVarV((RsContext)con, (RsScript)script, slot, val->GetPayload(), len);
}

ECode RenderScript::NScriptSetVarV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarV(mContext, id, slot, val);
    return NOERROR;
}

void RenderScript::RsnScriptGetVarV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarV, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int32 len = val->GetLength();
    rsScriptGetVarV((RsContext)con, (RsScript)script, slot, val->GetPayload(), len);
}

ECode RenderScript::NScriptGetVarV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptGetVarV(mContext, id, slot, val);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarVE(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int64 e,
    /* [in] */ ArrayOf<Int32>* dims)
{
    Slogger::D(LOG_TAG, "nScriptSetVarVE, con(%p), s(%p), slot(%i)", (RsContext)con, (void *)script, slot);
    Int32 len = data->GetLength();
    Int32 dimsLen = dims->GetLength() * sizeof(int);
    rsScriptSetVarVE((RsContext)con, (RsScript)script, slot, data->GetPayload(), len, (RsElement)elem,
            (const uint32_t*)dims->GetPayload(), dimsLen);
}

ECode RenderScript::NScriptSetVarVE(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val,
    /* [in] */ Int64 e,
    /* [in] */ ArrayOf<Int32>* dims)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarVE(mContext, id, slot, val, e, dims);
    return NOERROR;
}

void RenderScript::RsnScriptSetVarObj(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val)
{
    Slogger::D(LOG_TAG, "nScriptSetVarObj, con(%p), s(%p), slot(%i), val(%i)", (RsContext)con, (void *)script, slot, val);
    rsScriptSetVarObj((RsContext)con, (RsScript)script, slot, (RsObjectBase)val);
}

ECode RenderScript::NScriptSetVarObj(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptSetVarObj(mContext, id, slot, val);
    return NOERROR;
}

Int64 RenderScript::RsnScriptCCreate(
    /* [in] */ Int64 con,
    /* [in] */ const String& resName,
    /* [in] */ const String& cacheDir,
    /* [in] */ ArrayOf<Byte>* scriptRef,
    /* [in] */ Int32 length)
{
    Slogger::D(LOG_TAG, "nScriptCCreate, con(%p)", (RsContext)con);

    Int64 ret = 0;
    Byte* script_ptr = NULL;
    Int32 _exception = 0;
    Int32 remaining;
    if (!scriptRef) {
        _exception = 1;
        //jniThrowException(_env, "java/lang/IllegalArgumentException", "script == null");
        return (Int64)(uintptr_t)ret;
    }
    if (length < 0) {
        _exception = 1;
        //jniThrowException(_env, "java/lang/IllegalArgumentException", "length < 0");
        return (Int64)(uintptr_t)ret;
    }
    remaining = scriptRef->GetLength();
    if (remaining < length) {
        _exception = 1;
        //jniThrowException(_env, "java/lang/IllegalArgumentException",
        //        "length > script.length - offset");
        return (Int64)(uintptr_t)ret;
    }
    script_ptr = scriptRef->GetPayload();

    //rsScriptCSetText((RsContext)con, (const char *)script_ptr, length);

    ret = (Int64)(uintptr_t)rsScriptCCreate((RsContext)con,
            resName.string(), resName.GetLength(),
            cacheDir.string(), cacheDir.GetLength(),
            (const char *)script_ptr, length);
}

ECode RenderScript::NScriptCCreate(
    /* [in] */ const String& resName,
    /* [in] */ const String& cacheDir,
    /* [in] */ ArrayOf<Byte>* script,
    /* [in] */ Int32 length,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptCCreate(mContext, resName, cacheDir, script, length);
    return NOERROR;
}

Int64 RenderScript::RsnScriptIntrinsicCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int32 id,
    /* [in] */ Int64 eid)
{
    Slogger::D(LOG_TAG, "nScriptIntrinsicCreate, con(%p) id(%i) element(%p)", (RsContext)con, id, (void *)eid);
    return (Int64)(uintptr_t)rsScriptIntrinsicCreate((RsContext)con, id, (RsElement)eid);
}

ECode RenderScript::NScriptIntrinsicCreate(
    /* [in] */ Int32 id,
    /* [in] */ Int64 eid,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptIntrinsicCreate(mContext, id, eid);
    return NOERROR;
}

Int64 RenderScript::RsnScriptKernelIDCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 sig)
{
    Slogger::D(LOG_TAG, "nScriptKernelIDCreate, con(%p) script(%p), slot(%i), sig(%i)", (RsContext)con, (void *)sid, slot, sig);
    return (Int64)(uintptr_t)rsScriptKernelIDCreate((RsContext)con, (RsScript)sid, slot, sig);
}

ECode RenderScript::NScriptKernelIDCreate(
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 sig,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptKernelIDCreate(mContext, sid, slot, sig);
    return NOERROR;
}

Int64 RenderScript::RsnScriptFieldIDCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot)
{
    Slogger::D(LOG_TAG, "nScriptFieldIDCreate, con(%p) script(%p), slot(%i)", (RsContext)con, (void *)sid, slot);
    return (Int64)(uintptr_t)rsScriptFieldIDCreate((RsContext)con, (RsScript)sid, slot);
}

ECode RenderScript::NScriptFieldIDCreate(
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptFieldIDCreate(mContext, sid, slot);
    return NOERROR;
}

Int64 RenderScript::RsnScriptGroupCreate(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int64>* kernels,
    /* [in] */ ArrayOf<Int64>* src,
    /* [in] */ ArrayOf<Int64>* dstk,
    /* [in] */ ArrayOf<Int64>* dstf,
    /* [in] */ ArrayOf<Int64>* types)
{
    Slogger::D(LOG_TAG, "nScriptGroupCreate, con(%p)", (RsContext)con);

    Int32 kernelsLen = kernels->GetLength();
    Int64 *jKernelsPtr = kernels->GetPayload();
    RsScriptKernelID* kernelsPtr = (RsScriptKernelID*) malloc(sizeof(RsScriptKernelID) * kernelsLen);
    for(int i = 0; i < kernelsLen; ++i) {
        kernelsPtr[i] = (RsScriptKernelID)jKernelsPtr[i];
    }

    Int32 srcLen = src->GetLength();
    Int64 *jSrcPtr = src->GetPayload();
    RsScriptKernelID* srcPtr = (RsScriptKernelID*) malloc(sizeof(RsScriptKernelID) * srcLen);
    for(int i = 0; i < srcLen; ++i) {
        srcPtr[i] = (RsScriptKernelID)jSrcPtr[i];
    }

    Int32 dstkLen = dstk->GetLength();
    Int64 *jDstkPtr = dstk->GetPayload();
    RsScriptKernelID* dstkPtr = (RsScriptKernelID*) malloc(sizeof(RsScriptKernelID) * dstkLen);
    for(int i = 0; i < dstkLen; ++i) {
        dstkPtr[i] = (RsScriptKernelID)jDstkPtr[i];
    }

    Int32 dstfLen = dstf->GetLength();
    Int64 *jDstfPtr = dstf->GetPayload();
    RsScriptKernelID* dstfPtr = (RsScriptKernelID*) malloc(sizeof(RsScriptKernelID) * dstfLen);
    for(int i = 0; i < dstfLen; ++i) {
        dstfPtr[i] = (RsScriptKernelID)jDstfPtr[i];
    }

    Int32 typesLen = types->GetLength();
    Int64 *jTypesPtr = types->GetPayload();
    RsType* typesPtr = (RsType*) malloc(sizeof(RsType) * typesLen);
    for(int i = 0; i < typesLen; ++i) {
        typesPtr[i] = (RsType)jTypesPtr[i];
    }

    Int64 id = (Int64)(uintptr_t)rsScriptGroupCreate((RsContext)con,
           (RsScriptKernelID *)kernelsPtr, kernelsLen * sizeof(RsScriptKernelID),
           (RsScriptKernelID *)srcPtr, srcLen * sizeof(RsScriptKernelID),
           (RsScriptKernelID *)dstkPtr, dstkLen * sizeof(RsScriptKernelID),
           (RsScriptFieldID *)dstfPtr, dstfLen * sizeof(RsScriptKernelID),
           (RsType *)typesPtr, typesLen * sizeof(RsType));

    free(kernelsPtr);
    free(srcPtr);
    free(dstkPtr);
    free(dstfPtr);
    free(typesPtr);
    return id;
}

ECode RenderScript::NScriptGroupCreate(
    /* [in] */ ArrayOf<Int64>* kernels,
    /* [in] */ ArrayOf<Int64>* src,
    /* [in] */ ArrayOf<Int64>* dstk,
    /* [in] */ ArrayOf<Int64>* dstf,
    /* [in] */ ArrayOf<Int64>* types,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnScriptGroupCreate(mContext, kernels, src, dstk, dstf, types);
    return NOERROR;
}

void RenderScript::RsnScriptGroupSetInput(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nScriptGroupSetInput, con(%p) group(%p), kernelId(%p), alloc(%p)",
            (RsContext)con, (void *)gid, (void *)kid, (void *)alloc);
    rsScriptGroupSetInput((RsContext)con, (RsScriptGroup)gid, (RsScriptKernelID)kid, (RsAllocation)alloc);
}

ECode RenderScript::NScriptGroupSetInput(
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptGroupSetInput(mContext, group, kernel, alloc);
    return NOERROR;
}

void RenderScript::RsnScriptGroupSetOutput(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc)
{
    Slogger::D(LOG_TAG, "nScriptGroupSetOutput, con(%p) group(%p), kernelId(%p), alloc(%p)",
            (RsContext)con, (void *)group, (void *)kernel, (void *)alloc);
    rsScriptGroupSetOutput((RsContext)con, (RsScriptGroup)group, (RsScriptKernelID)kernel, (RsAllocation)alloc);
}

ECode RenderScript::NScriptGroupSetOutput(
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptGroupSetOutput(mContext, group, kernel, alloc);
    return NOERROR;
}

void RenderScript::RsnScriptGroupExecute(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group)
{
    Slogger::D(LOG_TAG, "nScriptGroupSetOutput, con(%p) group(%p)", (RsContext)con, (void *)gid);
    rsScriptGroupExecute((RsContext)con, (RsScriptGroup)gid);
}

ECode RenderScript::NScriptGroupExecute(
    /* [in] */ Int64 group)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnScriptGroupExecute(mContext, group);
    return NOERROR;
}

Int64 RenderScript::RsnSamplerCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int32 magFilter,
    /* [in] */ Int32 minFilter,
    /* [in] */ Int32 wrapS,
    /* [in] */ Int32 wrapT,
    /* [in] */ Int32 wrapR,
    /* [in] */ Float aniso)
{
    Slogger::D(LOG_TAG, "nSamplerCreate, con(%p)", (RsContext)con);
    return (Int64)(uintptr_t)rsSamplerCreate((RsContext)con,
            (RsSamplerValue)magFilter,
            (RsSamplerValue)minFilter,
            (RsSamplerValue)wrapS,
            (RsSamplerValue)wrapT,
            (RsSamplerValue)wrapR,
            aniso);
}

ECode RenderScript::NSamplerCreate(
    /* [in] */ Int32 magFilter,
    /* [in] */ Int32 minFilter,
    /* [in] */ Int32 wrapS,
    /* [in] */ Int32 wrapT,
    /* [in] */ Int32 wrapR,
    /* [in] */ Float aniso,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnSamplerCreate(mContext, magFilter, minFilter, wrapS, wrapT, wrapR, aniso);
    return NOERROR;
}

Int64 RenderScript::RsnProgramStoreCreate(
    /* [in] */ Int64 con,
    /* [in] */ Boolean r,
    /* [in] */ Boolean g,
    /* [in] */ Boolean b,
    /* [in] */ Boolean a,
    /* [in] */ Boolean depthMask,
    /* [in] */ Boolean dither,
    /* [in] */ Int32 srcMode,
    /* [in] */ Int32 dstMode,
    /* [in] */ Int32 depthFunc)
{
    Slogger::D(LOG_TAG, "nProgramStoreCreate, con(%p)", (RsContext)con);
    return (Int64)(uintptr_t)rsProgramStoreCreate((RsContext)con, r, g, b, a,
            depthMask, ditherEnable, (RsBlendSrcFunc)srcFunc,
            (RsBlendDstFunc)destFunc, (RsDepthFunc)depthFunc);
}

ECode RenderScript::NProgramStoreCreate(
    /* [in] */ Boolean r,
    /* [in] */ Boolean g,
    /* [in] */ Boolean b,
    /* [in] */ Boolean a,
    /* [in] */ Boolean depthMask,
    /* [in] */ Boolean dither,
    /* [in] */ Int32 srcMode,
    /* [in] */ Int32 dstMode,
    /* [in] */ Int32 depthFunc,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnProgramStoreCreate(mContext, r, g, b, a, depthMask, dither, srcMode,
            dstMode, depthFunc);
    return NOERROR;
}

Int64 RenderScript::RsnProgramRasterCreate(
    /* [in] */ Int64 con,
    /* [in] */ Boolean pointSprite,
    /* [in] */ Int32 cullMode)
{
    Slogger::D(LOG_TAG, "nProgramRasterCreate, con(%p), pointSprite(%i), cull(%i)", (RsContext)con, pointSprite, cullMode);
    return (Int64)(uintptr_t)rsProgramRasterCreate((RsContext)con, pointSprite, (RsCullMode)cullMode);
}

ECode RenderScript::NProgramRasterCreate(
    /* [in] */ Boolean pointSprite,
    /* [in] */ Int32 cullMode,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnProgramRasterCreate(mContext, pointSprite, cullMode);
    return NOERROR;
}

void RenderScript::RsnProgramBindConstants(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pv,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 a)
{
    Slogger::D(LOG_TAG, "nProgramBindConstants, con(%p), vpf(%p), sloat(%i), a(%p)",
            (RsContext)con, (RsProgramVertex)pv, slot, (RsAllocation)a);
    rsProgramBindConstants((RsContext)con, (RsProgram)vpv, slot, (RsAllocation)a);
}

ECode RenderScript::NProgramBindConstants(
    /* [in] */ Int64 pv,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 mID)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnProgramBindConstants(mContext, pv, slot, mID);
    return NOERROR;
}

void RenderScript::RsnProgramBindTexture(
    /* [in] */ Int64 con,
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 a)
{
    Slogger::D(LOG_TAG, "nProgramBindTexture, con(%p), vpf(%p), slot(%i), a(%p)",
            (RsContext)con, (RsProgramFragment)vpf, slot, (RsAllocation)a);
    rsProgramBindTexture((RsContext)con, (RsProgramFragment)vpf, slot, (RsAllocation)a);
}

ECode RenderScript::NProgramBindTexture(
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 a)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnProgramBindTexture(mContext, vpf, slot, a);
    return NOERROR;
}

void RenderScript::RsnProgramBindSampler(
    /* [in] */ Int64 con,
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 s)
{
    Slogger::D(LOG_TAG, "nProgramBindSampler, con(%p), vpf(%p), slot(%i), a(%p)",
            (RsContext)con, (RsProgramFragment)vpf, slot, (RsSampler)s);
    rsProgramBindSampler((RsContext)con, (RsProgramFragment)vpf, slot, (RsSampler)s);
}

ECode RenderScript::NProgramBindSampler(
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 s)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnProgramBindSampler(mContext, vpf, slot, s);
    return NOERROR;
}

Int64 RenderScript::RsnProgramFragmentCreate(
    /* [in] */ Int64 con,
    /* [in] */ const String& shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params)
{
    Int64 *jParamPtr = params->GetPayload();
    Int32 paramLen = params->GetLength();

    Int32 texCount = texNames->GetLength();
    AutoStringArrayToUTF8 names(texNames, texCount);
    const char ** nameArray = names.c_str();
    size_t* sizeArray = names.c_str_len();

    Slogger::D(LOG_TAG, "nProgramFragmentCreate, con(%p), paramLen(%i)", (RsContext)con, paramLen);

    uintptr_t * paramPtr = (uintptr_t*) malloc(sizeof(uintptr_t) * paramLen);
    for(int i = 0; i < paramLen; ++i) {
        paramPtr[i] = (uintptr_t)jParamPtr[i];
    }
    Int64 ret = (Int64)(uintptr_t)rsProgramFragmentCreate((RsContext)con, shader.string(), shader.GetLength(),
            nameArray, texCount, sizeArray,
            paramPtr, paramLen);

    free(paramPtr);
    return ret;
}

ECode RenderScript::NProgramFragmentCreate(
    /* [in] */ const String& shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnProgramFragmentCreate(mContext, shader, texNames, params);
    return NOERROR;
}

Int64 RenderScript::RsnProgramVertexCreate(
    /* [in] */ Int64 con, String shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params)
{
    Int64 *jParamPtr = params->GetPayload();
    Int32 paramLen = params->GetLength();

    Slogger::D(LOG_TAG, "nProgramVertexCreate, con(%p), paramLen(%i)", (RsContext)con, paramLen);

    Int32 texCount = texNames->GetLength();
    AutoStringArrayToUTF8 names(texNames, texCount);
    const char ** nameArray = names.c_str();
    size_t* sizeArray = names.c_str_len();

    uintptr_t * paramPtr = (uintptr_t*) malloc(sizeof(uintptr_t) * paramLen);
    for(int i = 0; i < paramLen; ++i) {
        paramPtr[i] = (uintptr_t)jParamPtr[i];
    }

    Int64 ret = (Int64)(uintptr_t)rsProgramVertexCreate((RsContext)con, shaderUTF.c_str(), shaderUTF.length(),
            nameArray, texCount, sizeArray,
            paramPtr, paramLen);

    free(paramPtr);
    return ret;
}

ECode RenderScript::NProgramVertexCreate(
    /* [in] */ const String& shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnProgramVertexCreate(mContext, shader, texNames, params);
    return NOERROR;
}

Int64 RenderScript::RsnMeshCreate(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int64>* vtx,
    /* [in] */ ArrayOf<Int64>* idx,
    /* [in] */ ArrayOf<Int32>* prim)
{
    Slogger::D(LOG_TAG, "nMeshCreate, con(%p)", (RsContext)con);

    Int32 vtxLen = vtx->GetLength();
    Int64 *jVtxPtr = vtx->GetPayload();
    RsAllocation* vtxPtr = (RsAllocation*) malloc(sizeof(RsAllocation) * vtxLen);
    for(int i = 0; i < vtxLen; ++i) {
        vtxPtr[i] = (RsAllocation)(uintptr_t)jVtxPtr[i];
    }

    Int32 idxLen = idx->GetLength();
    Int64 *jIdxPtr = idx->GetPayload();
    RsAllocation* idxPtr = (RsAllocation*) malloc(sizeof(RsAllocation) * idxLen);
    for(int i = 0; i < idxLen; ++i) {
        idxPtr[i] = (RsAllocation)(uintptr_t)jIdxPtr[i];
    }

    Int32 primLen = prim->GetLength();
    Int32 *primPtr = prim->GetPayload(_prim, NULL);

    Int64 id = (Int64)(uintptr_t)rsMeshCreate((RsContext)con,
            (RsAllocation *)vtxPtr, vtxLen,
            (RsAllocation *)idxPtr, idxLen,
            (uint32_t *)primPtr, primLen);

    free(vtxPtr);
    free(idxPtr);
    return id;
}

ECode RenderScript::NMeshCreate(
    /* [in] */ ArrayOf<Int64>* vtx,
    /* [in] */ ArrayOf<Int64>* idx,
    /* [in] */ ArrayOf<Int32>* prim,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnMeshCreate(mContext, vtx, idx, prim);
    return NOERROR;
}

Int32 RenderScript::RsnMeshGetVertexBufferCount(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id)
{
    Slogger::D(LOG_TAG, "nMeshGetVertexBufferCount, con(%p), Mesh(%p)", (RsContext)con, (RsMesh)mesh);
    Int32 vtxCount = 0;
    rsaMeshGetVertexBufferCount((RsContext)con, (RsMesh)mesh, &vtxCount);
    return vtxCount;
}

ECode RenderScript::NMeshGetVertexBufferCount(
    /* [in] */ Int64 id,
    /* [out] */ Int32* count)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnMeshGetVertexBufferCount(mContext, id);
    return NOERROR;
}

Int32 RenderScript::RsnMeshGetIndexCount(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id)
{
    Slogger::D(LOG_TAG, "nMeshGetIndexCount, con(%p), Mesh(%p)", (RsContext)con, (RsMesh)mesh);
    Int32 idxCount = 0;
    rsaMeshGetIndexCount((RsContext)con, (RsMesh)mesh, &idxCount);
    return idxCount;
}

ECode RenderScript::NMeshGetIndexCount(
    /* [in] */ Int64 id,
    /* [out] */ Int32* count)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result)
    *result = 0;
    FAIL_RETURN(Validate())
    *result = RsnMeshGetIndexCount(mContext, id);
    return NOERROR;
}

void RenderScript::RsnMeshGetVertices(
    /* [in] */ Int64 con,
    /* [in] */ Int64 mesh,
    /* [in] */ ArrayOf<Int64>* vtxIds,
    /* [in] */ Int32 vtxIdCount)
{
    Slogger::D(LOG_TAG, "nMeshGetVertices, con(%p), Mesh(%p)", (RsContext)con, (RsMesh)mesh);

    RsAllocation *allocs = (RsAllocation*)malloc((uint32_t)numVtxIDs * sizeof(RsAllocation));
    rsaMeshGetVertices((RsContext)con, (RsMesh)mesh, allocs, (uint32_t)numVtxIDs);

    for(Int32 i = 0; i < numVtxIDs; i ++) {
        const Int64 alloc = (Int64)(uintptr_t)allocs[i];
        (*vtxIds)[i] = alloc;
    }

    free(allocs);
}

ECode RenderScript::NMeshGetVertices(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* vtxIds,
    /* [in] */ Int32 vtxIdCount)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnMeshGetVertices(mContext, id, vtxIds, vtxIdCount);
    return NOERROR;
}

void RenderScript::RsnMeshGetIndices(
    /* [in] */ Int64 con,
    /* [in] */ Int64 mesh,
    /* [in] */ ArrayOf<Int64>* idxIds,
    /* [in] */ ArrayOf<Int32>* primitives,
    /* [in] */ Int32 vtxIdCount)
{
    Slogger::D(LOG_TAG, "nMeshGetVertices, con(%p), Mesh(%p)", (RsContext)con, (RsMesh)mesh);

    RsAllocation *allocs = (RsAllocation*)malloc((uint32_t)numIndices * sizeof(RsAllocation));
    uint32_t *prims= (uint32_t*)malloc((uint32_t)numIndices * sizeof(uint32_t));

    rsaMeshGetIndices((RsContext)con, (RsMesh)mesh, allocs, prims, (uint32_t)numIndices);

    for(Int32 i = 0; i < numIndices; i ++) {
        const Int64 alloc = (Int64)(uintptr_t)allocs[i];
        const Int32 prim = (Int32)prims[i];
        (*idxIds)[i] = alloc;
        (*primitives)[i] = prim;
    }

    free(allocs);
    free(prims);
}

ECode RenderScript::NMeshGetIndices(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* idxIds,
    /* [in] */ ArrayOf<Int32>* primitives,
    /* [in] */ Int32 vtxIdCount)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnMeshGetIndices(mContext, id, idxIds, primitives, vtxIdCount);
    return NOERROR;
}

Int64 RenderScript::RsnPathCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int32 prim,
    /* [in] */ Boolean isStatic,
    /* [in] */ Int64 vtx,
    /* [in] */ Int64 loop,
    /* [in] */ Float q)
{
    Slogger::D(LOG_TAG, "nPathCreate, con(%p)", (RsContext)con);
    Int64 id = (Int64)(uintptr_t)rsPathCreate((RsContext)con, (RsPathPrimitive)prim, isStatic,
            (RsAllocation)vtx,
            (RsAllocation)loop, q);
    return id;
}

ECode RenderScript::NPathCreate(
    /* [in] */ Int32 prim,
    /* [in] */ Boolean isStatic,
    /* [in] */ Int64 vtx,
    /* [in] */ Int64 loop,
    /* [in] */ Float q,
    /* [out] */ Int64* result)
{
    AutoLock lock(this);
    FAIL_RETURN(Validate())
    RsnPathCreate(mContext, prim, isStatic, vtx, loop, q);
    return NOERROR;
}

void RenderScript::SetMessageHandler(
    /* [in] */ RSMessageHandler* msg)
{
    mMessageCallback = msg;
}

AutoPtr<RSMessageHandler> RenderScript::GetMessageHandler()
{
    return mMessageCallback;
}

void RenderScript::SendMessage(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<Int32>* data)
{
    NContextSendMessage(id, data);
}

void RenderScript::SetErrorHandler(
    /* [in] */ RSErrorHandler msg)
{
    mErrorCallback = msg;
}

AutoPtr<RSErrorHandler> RenderScript::GetErrorHandler()
{
    return mErrorCallback;
}

ECode RenderScript::ValidateObject(
    /* [in] */ BaseObj* o)
{
    if (o != NULL) {
        if (o->mRS.Get() != this) {
            Slogger::E(LOG_TAG, "Attempting to use an object across contexts.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RenderScript::Validate()
{
    if (mContext == 0) {
        Slogger::E(LOG_TAG, "Calling RS with no Context active.");
        return E_RS_INVALID_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode RenderScript::SetPriority(
    /* [in] */ RenderScriptPriority p)
{
    FAIL_RETURN(Validate())
    return NContextSetPriority(p);
}

ECode RenderScript::GetApplicationContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    *ctx = mApplicationContext;
    REFCOUNT_ADD(*ctx)
    return NOERROR;
}

ECode RenderScript::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 sdkVersion,
    /* [out] */ IRenderScript** rs)
{
    VALIDATE_NOT_NULL(rs)
    return Create(ctx, sdkVersion, RenderScriptContextType_NORMAL, CREATE_FLAG_NONE, rs);
}

ECode RenderScript::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 sdkVersion,
    /* [in] */ RenderScriptContextType ct,
    /* [in] */ Int32 flags,
    /* [out] */ IRenderScript** _rs)
{
    VALIDATE_NOT_NULL(_rs)
    *_rs = NULL;
    if (!sInitialized) {
        Slogger::E(LOG_TAG, "RenderScript.create() called when disabled; someone is likely to crash");
        return NOERROR;
    }

    if ((flags & ~(CREATE_FLAG_LOW_LATENCY | CREATE_FLAG_LOW_POWER)) != 0) {
        Slogger::E(LOG_TAG, "Invalid flags passed.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock<RenderScript> rs = new RenderScript(ctx);

    rs->mDev = rs->NDeviceCreate();
    rs->mContext = rs->NContextCreate(rs->mDev, flags, sdkVersion, ct);
    rs->mContextType = ct;
    if (rs->mContext == 0) {
        Slogger::E(LOG_TAG, "Failed to create RS context.");
        return E_RS_DRIVER_EXCEPTION;
    }
    rs->mMessageThread = new MessageThread(rs);
    rs->mMessageThread->Start();
    *_rs = (IRenderScript*)rs;
    REFCOUNT_ADD(*_rs)
    return NOERROR;
}

ECode RenderScript::Create(
    /* [in] */ IContext* ctx,
    /* [out] */ IRenderScript** rs)
{
    VALIDATE_NOT_NULL(rs)
    return Create(ctx, RenderScriptContextType_NORMAL, rs);
}

ECode RenderScript::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ RenderScriptContextType ct,
    /* [out] */ IRenderScript** rs)
{
    VALIDATE_NOT_NULL(rs)
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 v;
    info->GetTargetSdkVersion(&v);
    return Create(ctx, v, ct, CREATE_FLAG_NONE, rs);
}

ECode RenderScript::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ RenderScriptContextType ct,
    /* [in] */ Int32 flags,
    /* [out] */ IRenderScript** rs)
{
    VALIDATE_NOT_NULL(rs)
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 v;
    info->GetTargetSdkVersion(&v);
    return Create(ctx, v, ct, flags, rs);
}

ECode RenderScript::ContextDump()
{
    FAIL_RETURN(Validate())
    return NContextDump(0);
}

ECode RenderScript::Finish()
{
    return NContextFinish();
}

ECode RenderScript::Destroy()
{
    Validate();
    NContextFinish();

    NContextDeinitToClient(mContext);
    mMessageThread->mRun = FALSE;
    // try {
    mMessageThread->Join();
    // } catch(InterruptedException e) {
    // }
    NContextDestroy();
    NDeviceDestroy(mDev);
    mDev = 0;
    return NOERROR;
}

Boolean RenderScript::IsAlive()
{
    return mContext != 0;
}

Int64 RenderScript::SafeID(
    /* [in] */ BaseObj* o)
{
    if(o != NULL) {
        Int64 id;
        o->GetID(this, &id);
        return id;
    }
    return 0;
}

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
