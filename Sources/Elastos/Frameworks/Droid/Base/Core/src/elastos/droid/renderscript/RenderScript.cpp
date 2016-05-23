
#include "elastos/droid/RenderScript/RenderScript.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/view/Surface.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <gui/Surface.h>
#include <ui/ANativeObjectBase.h>
#include <rs/rs.h>

using Elastos::Droid::RenderScript::EIID_IRenderScript;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::View::Surface;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

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
{}

RenderScript::~RenderScript()
{}

ECode RenderScript::constructor(
    /* [in] */ IContext* ctx)
{}

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
            ptr, len * 4,
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
    rsContextSendMessage((RsContext)con, id, (const uint8_t *)data.GetPayload(), len * sizeof(int));
}

ECode RenderScript::NContextSendMessage(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<Int32>* data)
{
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
    rsAssignName((RsContext)con, (void *)obj, (const char *)name.GetPayload(), name->GetLength());
}

ECode RenderScript::NAssignName(
    /* [in] */ Int64 obj,
    /* [in] */ ArrayOf<Byte>* name)
{
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
    VALIDATE_NOT_NULL(result)
    *result = rsnElementCreate(mContext, type, kind, norm, vecSize);
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
    VALIDATE_NOT_NULL(result)
    *result = rsnElementCreate2(mContext, elements, names, arraySizes);
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
    VALIDATE_NOT_NULL(result)
    *result = rsnTypeCreate(mContext, eid, x, y, z, mips, faces, yuv);
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
    VALIDATE_NOT_NULL(result)
    *result = rsnAllocationCreateTyped(mContext, type, mip, usage, pointer);
    return NOERROR;
}

Int64 RenderScript::RsnAllocationCreateFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage)
{
    // begin from this
    SkBitmap const * nativeBitmap =
            (SkBitmap const *)_env->GetLongField(jbitmap, gNativeBitmapID);
    const SkBitmap& bitmap(*nativeBitmap);

    bitmap.lockPixels();
    const void* ptr = bitmap.getPixels();
    jlong id = (jlong)(uintptr_t)rsAllocationCreateFromBitmap((RsContext)con,
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
    /* [out] */ Int64* result);

Int64 RenderScript::RsnAllocationCreateBitmapBackedAllocation(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage);

ECode RenderScript::NAllocationCreateBitmapBackedAllocation(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnAllocationCubeCreateFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage);

ECode RenderScript::NAllocationCubeCreateFromBitmap(
    /* [in] */ Int64 type,
    /* [in] */ Int32 mip,
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnAllocationCreateBitmapRef(
    /* [in] */ Int64 con,
    /* [in] */ Int64 type,
    /* [in] */ IBitmap* bmp);

ECode RenderScript::NAllocationCreateBitmapRef(
    /* [in] */ Int64 type,
    /* [in] */ IBitmap* bmp,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnAllocationCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ Int32 mips,
    /* [in] */ Int32 assetStream,
    /* [in] */ Int32 usage);

ECode RenderScript::NAllocationCreateFromAssetStream(
    /* [in] */ Int32 mips,
    /* [in] */ Int32 assetStream,
    /* [in] */ Int32 usage,
    /* [out] */ Int64* result);

void RenderScript::RsnAllocationCopyToBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp);

ECode RenderScript::NAllocationCopyToBitmap(
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp);

void RenderScript::RsnAllocationSyncAll(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 src);

ECode RenderScript::NAllocationSyncAll(
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 src);

native Surface rsnAllocationGetSurface(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc);

synchronized Surface nAllocationGetSurface(
    /* [in] */ Int64 alloc);

void RenderScript::RsnAllocationSetSurface(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ ISurface* sur);

ECode RenderScript::NAllocationSetSurface(
    /* [in] */ Int64 alloc,
    /* [in] */ ISurface* sur);

void RenderScript::RsnAllocationIoSend(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc);

ECode RenderScript::NAllocationIoSend(
    /* [in] */ Int64 alloc);

void RenderScript::RsnAllocationIoReceive(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc);

ECode RenderScript::NAllocationIoReceive(
    /* [in] */ Int64 alloc);

void RenderScript::RsnAllocationGenerateMipmaps(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc);

ECode RenderScript::NAllocationGenerateMipmaps(
    /* [in] */ Int64 alloc);

void RenderScript::RsnAllocationCopyFromBitmap(
    /* [in] */ Int64 con,
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp);

ECode RenderScript::NAllocationCopyFromBitmap(
    /* [in] */ Int64 alloc,
    /* [in] */ IBitmap* bmp);

void RenderScript::RsnAllocationData1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dt);

ECode RenderScript::NAllocationData1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt);

void RenderScript::RsnAllocationElementData1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 compIdx,
    /* [in] */ ArrayOf<Byte>* d,
    /* [in] */ Int32 sizeBytes);

ECode RenderScript::NAllocationElementData1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 compIdx,
    /* [in] */ ArrayOf<Byte>* d,
    /* [in] */ Int32 sizeBytes);

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
    /* [in] */ Int32 srcFace);

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
    /* [in] */ Int32 srcFace);

void RenderScript::RsnAllocationData2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dt);

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
    /* [in] */ ElementDataType dt);

void RenderScript::RsnAllocationData2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ IBitmap* b);

ECode RenderScript::NAllocationData2D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ IBitmap* b);

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
    /* [in] */ Int32 srcMip);

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
    /* [in] */ Int32 srcMip);

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
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dt);

ECode RenderScript::NAllocationData3D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 depth,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt);

void RenderScript::RsnAllocationRead(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 dt);

ECode RenderScript::NAllocationRead(
    /* [in] */ Int64 id,
    /* [in] */ IInterface* d,
    /* [in] */ ElementDataType dt);

void RenderScript::RsnAllocationRead1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dt);

ECode RenderScript::NAllocationRead1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 off,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ ElementDataType dt);

void RenderScript::RsnAllocationRead2D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 mip,
    /* [in] */ Int32 face,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* d,
    /* [in] */ Int32 sizeBytes,
    /* [in] */ Int32 dt);

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
    /* [in] */ ElementDataType dt);

Int64 RenderScript::RsnAllocationGetType(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id);

ECode RenderScript::NAllocationGetType(
    /* [in] */ Int64 id,
    /* [out] */ Int64* result);

void RenderScript::RsnAllocationResize1D(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 dimX);

ECode RenderScript::NAllocationResize1D(
    /* [in] */ Int64 id,
    /* [in] */ Int32 dimX);

Int64 RenderScript::RsnFileA3DCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ Int64 assetStream);

ECode RenderScript::NFileA3DCreateFromAssetStream(
    /* [in] */ Int64 assetStream,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnFileA3DCreateFromFile(
    /* [in] */ Int64 con,
    /* [in] */ const String& path);

ECode RenderScript::NFileA3DCreateFromFile(
    /* [in] */ const String& path,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnFileA3DCreateFromAsset(
    /* [in] */ Int64 con,
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path);

ECode RenderScript::NFileA3DCreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [out] */ Int64* result);

CARAPI_(Int32) RsnFileA3DGetNumIndexEntries(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D);

ECode RenderScript::NFileA3DGetNumIndexEntries(
    /* [in] */ Int64 fileA3D,
    /* [out] */ Int32* result);

void RenderScript::RsnFileA3DGetIndexEntries(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 numEntries,
    /* [in] */ ArrayOf<Int32>* IDs,
    /* [in] */ ArrayOf<String>* names);

ECode RenderScript::NFileA3DGetIndexEntries(
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 numEntries,
    /* [in] */ ArrayOf<Int32>* IDs,
    /* [in] */ ArrayOf<String>* names);

Int64 RenderScript::RsnFileA3DGetEntryByIndex(
    /* [in] */ Int64 con,
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 index);

ECode RenderScript::NFileA3DGetEntryByIndex(
    /* [in] */ Int64 fileA3D,
    /* [in] */ Int32 index,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnFontCreateFromFile(
    /* [in] */ Int64 con,
    /* [in] */ const String& fileName,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi);

ECode RenderScript::NFontCreateFromFile(
    /* [in] */ const String& fileName,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnFontCreateFromAssetStream(
    /* [in] */ Int64 con,
    /* [in] */ const String& name,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [in] */ Int64 assetStream);

ECode RenderScript::NFontCreateFromAssetStream(
    /* [in] */ const String& name,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [in] */ Int64 assetStream,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnFontCreateFromAsset(
    /* [in] */ Int64 con,
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi);

ECode RenderScript::NFontCreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [in] */ Float size,
    /* [in] */ Int32 dpi,
    /* [out] */ Int64* result);

void RenderScript::RsnScriptBindAllocation(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptBindAllocation(
    /* [in] */ Int64 script,
    /* [in] */ Int64 alloc,
    /* [in] */ Int32 slot);

void RenderScript::RsnScriptSetTimeZone(
    /* [in] */ Int64 con,
    /* [in] */ Int64 script,
    /* [in] */ ArrayOf<Byte>* timeZone);

ECode RenderScript::NScriptSetTimeZone(
    /* [in] */ Int64 script,
    /* [in] */ ArrayOf<Byte>* timeZone);

void RenderScript::RsnScriptInvoke(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptInvoke(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

void RenderScript::RsnScriptForEach(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params);

void RenderScript::RsnScriptForEach(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout);

void RenderScript::RsnScriptForEachClipped(
    /* [in] */ Int64 con,
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
    /* [in] */ Int32 zend);

void RenderScript::RsnScriptForEachClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend);

ECode RenderScript::NScriptForEach(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 ain,
    /* [in] */ Int64 aout,
    /* [in] */ ArrayOf<Byte>* params);

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
    /* [in] */ Int32 zend);

/**
 * Multi-input code.
 *
 */

// @hide
void RenderScript::RsnScriptForEachMultiClipped(
    /* [in] */ Int64 con,
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
    /* [in] */ Int32 zend);

// @hide
void RenderScript::RsnScriptForEachMultiClipped(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Int64>* ains,
    /* [in] */ Int64 aout,
    /* [in] */ Int32 xstart,
    /* [in] */ Int32 xend,
    /* [in] */ Int32 ystart,
    /* [in] */ Int32 yend,
    /* [in] */ Int32 zstart,
    /* [in] */ Int32 zend);

// @hide
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
    /* [in] */ Int32 zend);

void RenderScript::RsnScriptInvokeV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* params);

ECode RenderScript::NScriptInvokeV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* params);

void RenderScript::RsnScriptSetVarI(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 val);

ECode RenderScript::NScriptSetVarI(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 val);

CARAPI_(Int32) RsnScriptGetVarI(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptGetVarI(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Int32* result);

void RenderScript::RsnScriptSetVarJ(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val);

ECode RenderScript::NScriptSetVarJ(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val);

Int64 RenderScript::RsnScriptGetVarJ(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptGetVarJ(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Int64* result);

void RenderScript::RsnScriptSetVarF(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Float val);

ECode RenderScript::NScriptSetVarF(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Float val);

CARAPI_(Float) RsnScriptGetVarF(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptGetVarF(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Float* result);

void RenderScript::RsnScriptSetVarD(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Double val);

ECode RenderScript::NScriptSetVarD(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Double val);

CARAPI_(Double) RsnScriptGetVarD(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptGetVarD(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [out] */ Double* result);

void RenderScript::RsnScriptSetVarV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val);

ECode RenderScript::NScriptSetVarV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val);

void RenderScript::RsnScriptGetVarV(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val);

ECode RenderScript::NScriptGetVarV(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val);

void RenderScript::RsnScriptSetVarVE(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val,
    /* [in] */ Int64 e,
    /* [in] */ ArrayOf<Int32>* dims);

ECode RenderScript::NScriptSetVarVE(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ ArrayOf<Byte>* val,
    /* [in] */ Int64 e,
    /* [in] */ ArrayOf<Int32>* dims);

void RenderScript::RsnScriptSetVarObj(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val);

ECode RenderScript::NScriptSetVarObj(
    /* [in] */ Int64 id,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 val);

Int64 RenderScript::RsnScriptCCreate(
    /* [in] */ Int64 con,
    /* [in] */ const String& resName,
    /* [in] */ const String& cacheDir,
    /* [in] */ ArrayOf<Byte>* script,
    /* [in] */ Int32 length);

ECode RenderScript::NScriptCCreate(
    /* [in] */ const String& resName,
    /* [in] */ const String& cacheDir,
    /* [in] */ ArrayOf<Byte>* script,
    /* [in] */ Int32 length,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnScriptIntrinsicCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int32 id,
    /* [in] */ Int64 eid);

ECode RenderScript::NScriptIntrinsicCreate(
    /* [in] */ Int32 id,
    /* [in] */ Int64 eid,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnScriptKernelIDCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 sig);

ECode RenderScript::NScriptKernelIDCreate(
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [in] */ Int32 sig,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnScriptFieldIDCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot);

ECode RenderScript::NScriptFieldIDCreate(
    /* [in] */ Int64 sid,
    /* [in] */ Int32 slot,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnScriptGroupCreate(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int64>* kernels,
    /* [in] */ ArrayOf<Int64>* src,
    /* [in] */ ArrayOf<Int64>* dstk,
    /* [in] */ ArrayOf<Int64>* dstf,
    /* [in] */ ArrayOf<Int64>* types);

ECode RenderScript::NScriptGroupCreate(
    /* [in] */ ArrayOf<Int64>* kernels,
    /* [in] */ ArrayOf<Int64>* src,
    /* [in] */ ArrayOf<Int64>* dstk,
    /* [in] */ ArrayOf<Int64>* dstf,
    /* [in] */ ArrayOf<Int64>* types,
    /* [out] */ Int64* result);

void RenderScript::RsnScriptGroupSetInput(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc);

ECode RenderScript::NScriptGroupSetInput(
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc);

void RenderScript::RsnScriptGroupSetOutput(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc);

ECode RenderScript::NScriptGroupSetOutput(
    /* [in] */ Int64 group,
    /* [in] */ Int64 kernel,
    /* [in] */ Int64 alloc);

void RenderScript::RsnScriptGroupExecute(
    /* [in] */ Int64 con,
    /* [in] */ Int64 group);

ECode RenderScript::NScriptGroupExecute(
    /* [in] */ Int64 group);

Int64 RenderScript::RsnSamplerCreate(
    /* [in] */ Int64 con,
    /* [in] */ Int32 magFilter,
    /* [in] */ Int32 minFilter,
    /* [in] */ Int32 wrapS,
    /* [in] */ Int32 wrapT,
    /* [in] */ Int32 wrapR,
    /* [in] */ Float aniso);

ECode RenderScript::NSamplerCreate(
    /* [in] */ Int32 magFilter,
    /* [in] */ Int32 minFilter,
    /* [in] */ Int32 wrapS,
    /* [in] */ Int32 wrapT,
    /* [in] */ Int32 wrapR,
    /* [in] */ Float aniso,
    /* [out] */ Int64* result);

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
    /* [in] */ Int32 depthFunc,
    /* [out] */ Int64* result);

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
    /* [out] */ Int64* result);

Int64 RenderScript::RsnProgramRasterCreate(
    /* [in] */ Int64 con,
    /* [in] */ Boolean pointSprite,
    /* [in] */ Int32 cullMode);

ECode RenderScript::NProgramRasterCreate(
    /* [in] */ Boolean pointSprite,
    /* [in] */ Int32 cullMode,
    /* [out] */ Int64* result);

void RenderScript::RsnProgramBindConstants(
    /* [in] */ Int64 con,
    /* [in] */ Int64 pv,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 mID);

ECode RenderScript::NProgramBindConstants(
    /* [in] */ Int64 pv,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 mID);

void RenderScript::RsnProgramBindTexture(
    /* [in] */ Int64 con,
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 a);

ECode RenderScript::NProgramBindTexture(
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 a);

void RenderScript::RsnProgramBindSampler(
    /* [in] */ Int64 con,
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 s);

ECode RenderScript::NProgramBindSampler(
    /* [in] */ Int64 vpf,
    /* [in] */ Int32 slot,
    /* [in] */ Int64 s);

Int64 RenderScript::RsnProgramFragmentCreate(
    /* [in] */ Int64 con, String shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params);

ECode RenderScript::NProgramFragmentCreate(
    /* [in] */ const String& shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnProgramVertexCreate(
    /* [in] */ Int64 con, String shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params);

ECode RenderScript::NProgramVertexCreate(
    /* [in] */ const String& shader,
    /* [in] */ ArrayOf<String>* texNames,
    /* [in] */ ArrayOf<Int64>* params,
    /* [out] */ Int64* result);

Int64 RenderScript::RsnMeshCreate(
    /* [in] */ Int64 con,
    /* [in] */ ArrayOf<Int64>* vtx,
    /* [in] */ ArrayOf<Int64>* idx,
    /* [in] */ ArrayOf<Int32>* prim);

ECode RenderScript::NMeshCreate(
    /* [in] */ ArrayOf<Int64>* vtx,
    /* [in] */ ArrayOf<Int64>* idx,
    /* [in] */ ArrayOf<Int32>* prim,
    /* [out] */ Int64* result);

CARAPI_(Int32) RsnMeshGetVertexBufferCount(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id);

ECode RenderScript::NMeshGetVertexBufferCount(
    /* [in] */ Int64 id,
    /* [out] */ Int32* count);

CARAPI_(Int32) RsnMeshGetIndexCount(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id);

ECode RenderScript::NMeshGetIndexCount(
    /* [in] */ Int64 id,
    /* [out] */ Int32* count);

void RenderScript::RsnMeshGetVertices(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* vtxIds,
    /* [in] */ Int32 vtxIdCount);

ECode RenderScript::NMeshGetVertices(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* vtxIds,
    /* [in] */ Int32 vtxIdCount);

void RenderScript::RsnMeshGetIndices(
    /* [in] */ Int64 con,
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* idxIds,
    /* [in] */ ArrayOf<Int32>* primitives,
    /* [in] */ Int32 vtxIdCount);

ECode RenderScript::NMeshGetIndices(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* idxIds,
    /* [in] */ ArrayOf<Int32>* primitives,
    /* [in] */ Int32 vtxIdCount);

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
