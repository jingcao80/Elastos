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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__
#define __ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__

#include "Elastos.Droid.RenderScript.h"
#include "elastos/droid/RenderScript/ProgramStore.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Core::Runnable;
using Elastos::Core::Thread;
using Elastos::Utility::Concurrent::Locks::IReentrantReadWriteLock;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace RenderScript {

class Allocation;
class Element;

class RenderScript
    : public Object
    , public IRenderScript
{
public:
    ///////////////////////////////////////////////////////////////////////////////////
    //

    /**
     * The base class from which an application should derive in order
     * to receive RS messages from scripts. When a script calls {@code
     * rsSendToClient}, the data fields will be filled, and the run
     * method will be called on a separate thread.  This will occur
     * some time after {@code rsSendToClient} completes in the script,
     * as {@code rsSendToClient} is asynchronous. Message handlers are
     * not guaranteed to have completed when {@link
     * android.renderscript.RenderScript#finish} returns.
     *
     */
    class RSMessageHandler : public Runnable
    {
    public:
        RSMessageHandler()
            : mID(0)
            , mLength(0)
        {}

        CARAPI Run();

    protected:
        AutoPtr<ArrayOf<Int32> > mData;
        Int32 mID;
        Int32 mLength;
    };

    /**
     * The runtime error handler base class.  An application should derive from this class
     * if it wishes to install an error handler.  When errors occur at runtime,
     * the fields in this class will be filled, and the run method will be called.
     *
     */
    class RSErrorHandler : public Runnable
    {
    public:
        RSErrorHandler()
            : mErrorNum(0)
        {}

        CARAPI Run();

    protected:
        String mErrorMessage;
        Int32 mErrorNum;
    };

protected:
    class MessageThread : public Thread
    {
    public:
        MessageThread(
            /* [in] */ RenderScript* rs);

        CARAPI Run();

    public:
        AutoPtr<RenderScript> mRS;
        Boolean mRun;
        AutoPtr<ArrayOf<Int32> > mAuxData;

        const Int32 RS_MESSAGE_TO_CLIENT_NONE = 0;
        const Int32 RS_MESSAGE_TO_CLIENT_EXCEPTION = 1;
        const Int32 RS_MESSAGE_TO_CLIENT_RESIZE = 2;
        const Int32 RS_MESSAGE_TO_CLIENT_ERROR = 3;
        const Int32 RS_MESSAGE_TO_CLIENT_USER = 4;
        const Int32 RS_MESSAGE_TO_CLIENT_NEW_BUFFER = 5;

        const Int32 RS_ERROR_FATAL_DEBUG = 0x0800;
        const Int32 RS_ERROR_FATAL_UNKNOWN = 0x1000;
    };

private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    RenderScript();

    virtual ~RenderScript();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CAR_INTERFACE_DECL()

    /**
     * Returns an identifier that can be used to identify a particular
     * minor version of RS.
     *
     * @hide
     */
    static CARAPI_(Int64) GetMinorID();

     /**
     * Sets the directory to use as a persistent storage for the
     * renderscript file cache.
     *
     * @hide
     * @param cacheDir A directory the current process can write to
     */
    static CARAPI_(void) SetupDiskCache(
        /* [in] */ IFile* cacheDir);

    CARAPI_(void) SetMessageHandler(
        /* [in] */ RSMessageHandler* msg);

    CARAPI_(AutoPtr<RSMessageHandler>) GetMessageHandler();

    /**
     * Place a message into the message queue to be sent back to the message
     * handler once all previous commands have been executed.
     *
     * @param id
     * @param data
     */
    CARAPI_(void) SendMessage(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<Int32>* data);

    CARAPI_(void) SetErrorHandler(
        /* [in] */ RSErrorHandler msg);

    CARAPI_(AutoPtr<RSErrorHandler>) GetErrorHandler();

    CARAPI SetPriority(
        /* [in] */ RenderScriptPriority p);

    /**
     * Gets the application context associated with the RenderScript context.
     *
     * @return The application context.
     */
    CARAPI GetApplicationContext(
        /* [out] */ IContext** ctx);

    /**
     * @hide
     */
    static CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 sdkVersion,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     * @hide
     * @param ctx The context.
     * @return RenderScript
     */
    static CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 sdkVersion,
        /* [in] */ RenderScriptContextType ct,
        /* [in] */ Int32 flags,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     * @param ctx The context.
     * @return RenderScript
     */
    static CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [out] */ IRenderScript** rs);

    /**
     * Create a RenderScript context.
     *
     *
     * @param ctx The context.
     * @param ct The type of context to be created.
     * @return RenderScript
     */
    static CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ RenderScriptContextType ct,
        /* [out] */ IRenderScript** rs);

     /**
     * Create a RenderScript context.
     *
     *
     * @param ctx The context.
     * @param ct The type of context to be created.
     * @param flags The OR of the CREATE_FLAG_* options desired
     * @return RenderScript
     */
    static CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ RenderScriptContextType ct,
        /* [in] */ Int32 flags,
        /* [out] */ IRenderScript** rs);

    /**
     * Print the currently available debugging information about the state of
     * the RS context to the log.
     *
     */
    CARAPI ContextDump();

    /**
     * Wait for any pending asynchronous opeations (such as copies to a RS
     * allocation or RS script executions) to complete.
     *
     */
    CARAPI Finish();

    /**
     * Destroys this RenderScript context.  Once this function is called,
     * using this context or any objects belonging to this context is
     * illegal.
     *
     */
    CARAPI Destroy();

protected:
    static CARAPI_(void) _NInit();

    /*
     * Detect the bitness of the VM to allow FieldPacker to do the right thing.
     */
    static CARAPI_(Int32) RsnSystemGetPointerSize();

    // Non-threadsafe functions.
    CARAPI_(Int64) NDeviceCreate();

    CARAPI_(void) NDeviceDestroy(
        /* [in] */ Int64 dev);

    CARAPI_(void) NDeviceSetConfig(
        /* [in] */ Int64 dev,
        /* [in] */ Int32 param,
        /* [in] */ Int32 value);

    CARAPI_(Int32) NContextGetUserMessage(
        /* [in] */ Int64 con,
        /* [in] */ ArrayOf<Int32>* data);

    CARAPI_(String) NContextGetErrorMessage(
        /* [in] */ Int64 con);

    CARAPI_(Int32) NContextPeekMessage(
        /* [in] */ Int64 con,
        /* [in] */ ArrayOf<Int32>* subID);

    CARAPI_(void) NContextInitToClient(
        /* [in] */ Int64 con);

    CARAPI_(void) NContextDeinitToClient(
        /* [in] */ Int64 con);

    // Methods below are wrapped to protect the non-threadsafe
    // lockless fifo.
    CARAPI_(Int64) RsnContextCreateGL(
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
        /* [in] */ Int32 dpi);

    CARAPI_(Int64) NContextCreateGL(
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
        /* [in] */ Int32 dpi);

    CARAPI_(Int64) RsnContextCreate(
        /* [in] */ Int64 dev,
        /* [in] */ Int32 ver,
        /* [in] */ Int32 sdkVer,
        /* [in] */ Int32 contextType);

    CARAPI_(Int64) NContextCreate(
        /* [in] */ Int64 dev,
        /* [in] */ Int32 ver,
        /* [in] */ Int32 sdkVer,
        /* [in] */ Int32 contextType);

    CARAPI_(void) RsnContextDestroy(
        /* [in] */ Int64 con);

    CARAPI NContextDestroy();

    CARAPI_(void) RsnContextSetSurface(
        /* [in] */ Int64 con,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ISurface* sur);

    CARAPI NContextSetSurface(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ISurface* sur);

    CARAPI_(void) RsnContextSetSurfaceTexture(
        /* [in] */ Int64 con,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ISurfaceTexture* sur);

    CARAPI NContextSetSurfaceTexture(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ISurfaceTexture* sur);

    CARAPI_(void) RsnContextSetPriority(
        /* [in] */ Int64 con,
        /* [in] */ Int32 p);

    CARAPI NContextSetPriority(
        /* [in] */ Int32 p);

    CARAPI_(void) RsnContextDump(
        /* [in] */ Int64 con,
        /* [in] */ Int32 bits);

    CARAPI NContextDump(
        /* [in] */ Int32 bits);

    CARAPI_(void) RsnContextFinish(
        /* [in] */ Int64 con);

    CARAPI NContextFinish();

    CARAPI_(void) RsnContextSendMessage(
        /* [in] */ Int64 con,
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<Int32>* data);

    CARAPI NContextSendMessage(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<Int32>* data);

    CARAPI_(void) RsnContextBindRootScript(
        /* [in] */ Int64 con,
        /* [in] */ Int64 script);

    CARAPI NContextBindRootScript(
        /* [in] */ Int64 script);

    CARAPI_(void) RsnContextBindSampler(
        /* [in] */ Int64 con,
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 slot);

    CARAPI NContextBindSampler(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 slot);

    CARAPI_(void) RsnContextBindProgramStore(
        /* [in] */ Int64 con,
        /* [in] */ Int64 pfs);

    CARAPI NContextBindProgramStore(
        /* [in] */ Int64 pfs);

    CARAPI_(void) RsnContextBindProgramFragment(
        /* [in] */ Int64 con,
        /* [in] */ Int64 pf);

    CARAPI NContextBindProgramFragment(
        /* [in] */ Int64 pf);

    CARAPI_(void) RsnContextBindProgramVertex(
        /* [in] */ Int64 con,
        /* [in] */ Int64 pv);

    CARAPI NContextBindProgramVertex(
        /* [in] */ Int64 pv);

    CARAPI_(void) RsnContextBindProgramRaster(
        /* [in] */ Int64 con,
        /* [in] */ Int64 pr);

    CARAPI NContextBindProgramRaster(
        /* [in] */ Int64 pr);

    CARAPI_(void) RsnContextPause(
        /* [in] */ Int64 con);

    CARAPI NContextPause();

    CARAPI_(void) RsnContextResume(
        /* [in] */ Int64 con);

    CARAPI NContextResume();

    CARAPI_(void) RsnAssignName(
        /* [in] */ Int64 con,
        /* [in] */ Int64 obj,
        /* [in] */ ArrayOf<Byte>* name);

    CARAPI NAssignName(
        /* [in] */ Int64 obj,
        /* [in] */ ArrayOf<Byte>* name);

    CARAPI_(String) RsnGetName(
        /* [in] */ Int64 con,
        /* [in] */ Int64 obj);

    CARAPI NGetName(
        /* [in] */ Int64 obj,
        /* [out] */ String* name);

    // nObjDestroy is explicitly _not_ synchronous to prevent crashes in finalizers
    CARAPI_(void) RsnObjDestroy(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id);

    CARAPI_(void) NObjDestroy(
        /* [in] */ Int64 id);

    CARAPI_(Int64) RsnElementCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ Int32 kind,
        /* [in] */ Boolean norm,
        /* [in] */ Int32 vecSize);

    CARAPI NElementCreate(
        /* [in] */ Int64 type,
        /* [in] */ Int32 kind,
        /* [in] */ Boolean norm,
        /* [in] */ Int32 vecSize,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnElementCreate2(
        /* [in] */ Int64 con,
        /* [in] */ ArrayOf<Int64>* elements,
        /* [in] */ ArrayOf<String>* names,
        /* [in] */ ArrayOf<Int32>* arraySizes);

    CARAPI NElementCreate2(
        /* [in] */ ArrayOf<Int64>* elements,
        /* [in] */ ArrayOf<String>* names,
        /* [in] */ ArrayOf<Int32>* arraySizes,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnElementGetNativeData(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int32>* elementData);

    CARAPI NElementGetNativeData(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int32>* elementData);

    CARAPI_(void) RsnElementGetSubElements(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* IDs,
        /* [in] */ ArrayOf<String>* names,
        /* [in] */ ArrayOf<Int32>* arraySizes);

    CARAPI NElementGetSubElements(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* IDs,
        /* [in] */ ArrayOf<String>* names,
        /* [in] */ ArrayOf<Int32>* arraySizes);

    CARAPI_(Int64) RsnTypeCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int64 eid,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Boolean mips,
        /* [in] */ Boolean faces,
        /* [in] */ Int32 yuv);

    CARAPI NTypeCreate(
        /* [in] */ Int64 eid,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Boolean mips,
        /* [in] */ Boolean faces,
        /* [in] */ Int32 yuv,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnTypeGetNativeData(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* typeData);

    CARAPI NTypeGetNativeData(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* typeData);

    CARAPI_(Int64) RsnAllocationCreateTyped(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 usage,
        /* [in] */ Int64 pointer);

    CARAPI NAllocationCreateTyped(
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 usage,
        /* [in] */ Int64 pointer,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnAllocationCreateFromBitmap(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage);

    CARAPI NAllocationCreateFromBitmap(
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnAllocationCreateBitmapBackedAllocation(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage);

    CARAPI NAllocationCreateBitmapBackedAllocation(
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnAllocationCubeCreateFromBitmap(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage);

    CARAPI NAllocationCubeCreateFromBitmap(
        /* [in] */ Int64 type,
        /* [in] */ Int32 mip,
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 usage,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnAllocationCreateBitmapRef(
        /* [in] */ Int64 con,
        /* [in] */ Int64 type,
        /* [in] */ IBitmap* bmp);

    CARAPI NAllocationCreateBitmapRef(
        /* [in] */ Int64 type,
        /* [in] */ IBitmap* bmp,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnAllocationCreateFromAssetStream(
        /* [in] */ Int64 con,
        /* [in] */ Int32 mips,
        /* [in] */ Int32 assetStream,
        /* [in] */ Int32 usage);

    CARAPI NAllocationCreateFromAssetStream(
        /* [in] */ Int32 mips,
        /* [in] */ Int32 assetStream,
        /* [in] */ Int32 usage,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnAllocationCopyToBitmap(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc,
        /* [in] */ IBitmap* bmp);

    CARAPI NAllocationCopyToBitmap(
        /* [in] */ Int64 alloc,
        /* [in] */ IBitmap* bmp);

    CARAPI_(void) RsnAllocationSyncAll(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc,
        /* [in] */ Int32 src);

    CARAPI NAllocationSyncAll(
        /* [in] */ Int64 alloc,
        /* [in] */ Int32 src);

    CARAPI_(AutoPtr<ISurface>) RsnAllocationGetSurface(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc);

    CARAPI NAllocationGetSurface(
        /* [in] */ Int64 alloc,
        /* [out] */ ISurface** surface);

    CARAPI_(void) RsnAllocationSetSurface(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc,
        /* [in] */ ISurface* sur);

    CARAPI NAllocationSetSurface(
        /* [in] */ Int64 alloc,
        /* [in] */ ISurface* sur);

    CARAPI_(void) RsnAllocationIoSend(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc);

    CARAPI NAllocationIoSend(
        /* [in] */ Int64 alloc);

    CARAPI_(void) RsnAllocationIoReceive(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc);

    CARAPI NAllocationIoReceive(
        /* [in] */ Int64 alloc);

    CARAPI_(void) RsnAllocationGenerateMipmaps(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc);

    CARAPI NAllocationGenerateMipmaps(
        /* [in] */ Int64 alloc);

    CARAPI_(void) RsnAllocationCopyFromBitmap(
        /* [in] */ Int64 con,
        /* [in] */ Int64 alloc,
        /* [in] */ IBitmap* bmp);

    CARAPI NAllocationCopyFromBitmap(
        /* [in] */ Int64 alloc,
        /* [in] */ IBitmap* bmp);

    CARAPI_(void) RsnAllocationData1D(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 off,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 count,
        /* [in] */ IInterface* d,
        /* [in] */ Int32 sizeBytes,
        /* [in] */ Int32 dt);

    CARAPI NAllocationData1D(
        /* [in] */ Int64 id,
        /* [in] */ Int32 off,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 count,
        /* [in] */ IInterface* d,
        /* [in] */ Int32 sizeBytes,
        /* [in] */ ElementDataType dt);

    CARAPI_(void) RsnAllocationElementData1D(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 compIdx,
        /* [in] */ ArrayOf<Byte>* d,
        /* [in] */ Int32 sizeBytes);

    CARAPI NAllocationElementData1D(
        /* [in] */ Int64 id,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 compIdx,
        /* [in] */ ArrayOf<Byte>* d,
        /* [in] */ Int32 sizeBytes);

    CARAPI_(void) RsnAllocationData2D(
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

    CARAPI NAllocationData2D(
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

    CARAPI_(void) RsnAllocationData2D(
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

    CARAPI NAllocationData2D(
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

    CARAPI_(void) RsnAllocationData2D(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 face,
        /* [in] */ IBitmap* b);

    CARAPI NAllocationData2D(
        /* [in] */ Int64 id,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 face,
        /* [in] */ IBitmap* b);

    CARAPI_(void) RsnAllocationData3D(
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

    CARAPI NAllocationData3D(
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

    CARAPI_(void) RsnAllocationData3D(
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

    CARAPI NAllocationData3D(
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

    CARAPI_(void) RsnAllocationRead(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ IInterface* d,
        /* [in] */ Int32 dt);

    CARAPI NAllocationRead(
        /* [in] */ Int64 id,
        /* [in] */ IInterface* d,
        /* [in] */ ElementDataType dt);

    CARAPI_(void) RsnAllocationRead1D(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 off,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 count,
        /* [in] */ IInterface* d,
        /* [in] */ Int32 sizeBytes,
        /* [in] */ Int32 dt);

    CARAPI NAllocationRead1D(
        /* [in] */ Int64 id,
        /* [in] */ Int32 off,
        /* [in] */ Int32 mip,
        /* [in] */ Int32 count,
        /* [in] */ IInterface* d,
        /* [in] */ Int32 sizeBytes,
        /* [in] */ ElementDataType dt);

    CARAPI_(void) RsnAllocationRead2D(
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

    CARAPI NAllocationRead2D(
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

    CARAPI_(Int64) RsnAllocationGetType(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id);

    CARAPI NAllocationGetType(
        /* [in] */ Int64 id,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnAllocationResize1D(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 dimX);

    CARAPI NAllocationResize1D(
        /* [in] */ Int64 id,
        /* [in] */ Int32 dimX);

    CARAPI_(Int64) RsnFileA3DCreateFromAssetStream(
        /* [in] */ Int64 con,
        /* [in] */ Int64 assetStream);

    CARAPI NFileA3DCreateFromAssetStream(
        /* [in] */ Int64 assetStream,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnFileA3DCreateFromFile(
        /* [in] */ Int64 con,
        /* [in] */ const String& path);

    CARAPI NFileA3DCreateFromFile(
        /* [in] */ const String& path,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnFileA3DCreateFromAsset(
        /* [in] */ Int64 con,
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path);

    CARAPI NFileA3DCreateFromAsset(
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [out] */ Int64* result);

    CARAPI_(Int32) RsnFileA3DGetNumIndexEntries(
        /* [in] */ Int64 con,
        /* [in] */ Int64 fileA3D);

    CARAPI NFileA3DGetNumIndexEntries(
        /* [in] */ Int64 fileA3D,
        /* [out] */ Int32* result);

    CARAPI_(void) RsnFileA3DGetIndexEntries(
        /* [in] */ Int64 con,
        /* [in] */ Int64 fileA3D,
        /* [in] */ Int32 numEntries,
        /* [in] */ ArrayOf<Int32>* IDs,
        /* [in] */ ArrayOf<String>* names);

    CARAPI NFileA3DGetIndexEntries(
        /* [in] */ Int64 fileA3D,
        /* [in] */ Int32 numEntries,
        /* [in] */ ArrayOf<Int32>* IDs,
        /* [in] */ ArrayOf<String>* names);

    CARAPI_(Int64) RsnFileA3DGetEntryByIndex(
        /* [in] */ Int64 con,
        /* [in] */ Int64 fileA3D,
        /* [in] */ Int32 index);

    CARAPI NFileA3DGetEntryByIndex(
        /* [in] */ Int64 fileA3D,
        /* [in] */ Int32 index,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnFontCreateFromFile(
        /* [in] */ Int64 con,
        /* [in] */ const String& fileName,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi);

    CARAPI NFontCreateFromFile(
        /* [in] */ const String& fileName,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnFontCreateFromAssetStream(
        /* [in] */ Int64 con,
        /* [in] */ const String& name,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi,
        /* [in] */ Int64 assetStream);

    CARAPI NFontCreateFromAssetStream(
        /* [in] */ const String& name,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi,
        /* [in] */ Int64 assetStream,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnFontCreateFromAsset(
        /* [in] */ Int64 con,
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi);

    CARAPI NFontCreateFromAsset(
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [in] */ Float size,
        /* [in] */ Int32 dpi,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnScriptBindAllocation(
        /* [in] */ Int64 con,
        /* [in] */ Int64 script,
        /* [in] */ Int64 alloc,
        /* [in] */ Int32 slot);

    CARAPI NScriptBindAllocation(
        /* [in] */ Int64 script,
        /* [in] */ Int64 alloc,
        /* [in] */ Int32 slot);

    CARAPI_(void) RsnScriptSetTimeZone(
        /* [in] */ Int64 con,
        /* [in] */ Int64 script,
        /* [in] */ ArrayOf<Byte>* timeZone);

    CARAPI NScriptSetTimeZone(
        /* [in] */ Int64 script,
        /* [in] */ ArrayOf<Byte>* timeZone);

    CARAPI_(void) RsnScriptInvoke(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI NScriptInvoke(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI_(void) RsnScriptForEach(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 ain,
        /* [in] */ Int64 aout,
        /* [in] */ ArrayOf<Byte>* params);

    CARAPI_(void) RsnScriptForEach(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 ain,
        /* [in] */ Int64 aout);

    CARAPI_(void) RsnScriptForEachClipped(
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

    CARAPI_(void) RsnScriptForEachClipped(
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

    CARAPI NScriptForEach(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 ain,
        /* [in] */ Int64 aout,
        /* [in] */ ArrayOf<Byte>* params);

    CARAPI NScriptForEachClipped(
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
    CARAPI_(void) RsnScriptForEachMultiClipped(
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
    CARAPI_(void) RsnScriptForEachMultiClipped(
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
    CARAPI NScriptForEachMultiClipped(
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

    CARAPI_(void) RsnScriptInvokeV(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* params);

    CARAPI NScriptInvokeV(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* params);

    CARAPI_(void) RsnScriptSetVarI(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int32 val);

    CARAPI NScriptSetVarI(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int32 val);

    CARAPI_(Int32) RsnScriptGetVarI(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI NScriptGetVarI(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [out] */ Int32* result);

    CARAPI_(void) RsnScriptSetVarJ(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 val);

    CARAPI NScriptSetVarJ(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 val);

    CARAPI_(Int64) RsnScriptGetVarJ(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI NScriptGetVarJ(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnScriptSetVarF(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Float val);

    CARAPI NScriptSetVarF(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Float val);

    CARAPI_(Float) RsnScriptGetVarF(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI NScriptGetVarF(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [out] */ Float* result);

    CARAPI_(void) RsnScriptSetVarD(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Double val);

    CARAPI NScriptSetVarD(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Double val);

    CARAPI_(Double) RsnScriptGetVarD(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot);

    CARAPI NScriptGetVarD(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [out] */ Double* result);

    CARAPI_(void) RsnScriptSetVarV(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val);

    CARAPI NScriptSetVarV(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val);

    CARAPI_(void) RsnScriptGetVarV(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val);

    CARAPI NScriptGetVarV(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val);

    CARAPI_(void) RsnScriptSetVarVE(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val,
        /* [in] */ Int64 e,
        /* [in] */ ArrayOf<Int32>* dims);

    CARAPI NScriptSetVarVE(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ ArrayOf<Byte>* val,
        /* [in] */ Int64 e,
        /* [in] */ ArrayOf<Int32>* dims);

    CARAPI_(void) RsnScriptSetVarObj(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 val);

    CARAPI NScriptSetVarObj(
        /* [in] */ Int64 id,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 val);

    CARAPI_(Int64) RsnScriptCCreate(
        /* [in] */ Int64 con,
        /* [in] */ const String& resName,
        /* [in] */ const String& cacheDir,
        /* [in] */ ArrayOf<Byte>* script,
        /* [in] */ Int32 length);

    CARAPI NScriptCCreate(
        /* [in] */ const String& resName,
        /* [in] */ const String& cacheDir,
        /* [in] */ ArrayOf<Byte>* script,
        /* [in] */ Int32 length,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnScriptIntrinsicCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int32 id,
        /* [in] */ Int64 eid);

    CARAPI NScriptIntrinsicCreate(
        /* [in] */ Int32 id,
        /* [in] */ Int64 eid,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnScriptKernelIDCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int64 sid,
        /* [in] */ Int32 slot,
        /* [in] */ Int32 sig);

    CARAPI NScriptKernelIDCreate(
        /* [in] */ Int64 sid,
        /* [in] */ Int32 slot,
        /* [in] */ Int32 sig,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnScriptFieldIDCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int64 sid,
        /* [in] */ Int32 slot);

    CARAPI NScriptFieldIDCreate(
        /* [in] */ Int64 sid,
        /* [in] */ Int32 slot,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnScriptGroupCreate(
        /* [in] */ Int64 con,
        /* [in] */ ArrayOf<Int64>* kernels,
        /* [in] */ ArrayOf<Int64>* src,
        /* [in] */ ArrayOf<Int64>* dstk,
        /* [in] */ ArrayOf<Int64>* dstf,
        /* [in] */ ArrayOf<Int64>* types);

    CARAPI NScriptGroupCreate(
        /* [in] */ ArrayOf<Int64>* kernels,
        /* [in] */ ArrayOf<Int64>* src,
        /* [in] */ ArrayOf<Int64>* dstk,
        /* [in] */ ArrayOf<Int64>* dstf,
        /* [in] */ ArrayOf<Int64>* types,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnScriptGroupSetInput(
        /* [in] */ Int64 con,
        /* [in] */ Int64 group,
        /* [in] */ Int64 kernel,
        /* [in] */ Int64 alloc);

    CARAPI NScriptGroupSetInput(
        /* [in] */ Int64 group,
        /* [in] */ Int64 kernel,
        /* [in] */ Int64 alloc);

    CARAPI_(void) RsnScriptGroupSetOutput(
        /* [in] */ Int64 con,
        /* [in] */ Int64 group,
        /* [in] */ Int64 kernel,
        /* [in] */ Int64 alloc);

    CARAPI NScriptGroupSetOutput(
        /* [in] */ Int64 group,
        /* [in] */ Int64 kernel,
        /* [in] */ Int64 alloc);

    CARAPI_(void) RsnScriptGroupExecute(
        /* [in] */ Int64 con,
        /* [in] */ Int64 group);

    CARAPI NScriptGroupExecute(
        /* [in] */ Int64 group);

    CARAPI_(Int64) RsnSamplerCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int32 magFilter,
        /* [in] */ Int32 minFilter,
        /* [in] */ Int32 wrapS,
        /* [in] */ Int32 wrapT,
        /* [in] */ Int32 wrapR,
        /* [in] */ Float aniso);

    CARAPI NSamplerCreate(
        /* [in] */ Int32 magFilter,
        /* [in] */ Int32 minFilter,
        /* [in] */ Int32 wrapS,
        /* [in] */ Int32 wrapT,
        /* [in] */ Int32 wrapR,
        /* [in] */ Float aniso,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnProgramStoreCreate(
        /* [in] */ Int64 con,
        /* [in] */ Boolean r,
        /* [in] */ Boolean g,
        /* [in] */ Boolean b,
        /* [in] */ Boolean a,
        /* [in] */ Boolean depthMask,
        /* [in] */ Boolean dither,
        /* [in] */ Int32 srcMode,
        /* [in] */ Int32 dstMode,
        /* [in] */ Int32 depthFunc);

    CARAPI NProgramStoreCreate(
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

    CARAPI_(Int64) RsnProgramRasterCreate(
        /* [in] */ Int64 con,
        /* [in] */ Boolean pointSprite,
        /* [in] */ Int32 cullMode);

    CARAPI NProgramRasterCreate(
        /* [in] */ Boolean pointSprite,
        /* [in] */ Int32 cullMode,
        /* [out] */ Int64* result);

    CARAPI_(void) RsnProgramBindConstants(
        /* [in] */ Int64 con,
        /* [in] */ Int64 pv,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 mID);

    CARAPI NProgramBindConstants(
        /* [in] */ Int64 pv,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 mID);

    CARAPI_(void) RsnProgramBindTexture(
        /* [in] */ Int64 con,
        /* [in] */ Int64 vpf,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 a);

    CARAPI NProgramBindTexture(
        /* [in] */ Int64 vpf,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 a);

    CARAPI_(void) RsnProgramBindSampler(
        /* [in] */ Int64 con,
        /* [in] */ Int64 vpf,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 s);

    CARAPI NProgramBindSampler(
        /* [in] */ Int64 vpf,
        /* [in] */ Int32 slot,
        /* [in] */ Int64 s);

    CARAPI_(Int64) RsnProgramFragmentCreate(
        /* [in] */ Int64 con,
        /* [in] */ const String& shader,
        /* [in] */ ArrayOf<String>* texNames,
        /* [in] */ ArrayOf<Int64>* params);

    CARAPI NProgramFragmentCreate(
        /* [in] */ const String& shader,
        /* [in] */ ArrayOf<String>* texNames,
        /* [in] */ ArrayOf<Int64>* params,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnProgramVertexCreate(
        /* [in] */ Int64 con, String shader,
        /* [in] */ ArrayOf<String>* texNames,
        /* [in] */ ArrayOf<Int64>* params);

    CARAPI NProgramVertexCreate(
        /* [in] */ const String& shader,
        /* [in] */ ArrayOf<String>* texNames,
        /* [in] */ ArrayOf<Int64>* params,
        /* [out] */ Int64* result);

    CARAPI_(Int64) RsnMeshCreate(
        /* [in] */ Int64 con,
        /* [in] */ ArrayOf<Int64>* vtx,
        /* [in] */ ArrayOf<Int64>* idx,
        /* [in] */ ArrayOf<Int32>* prim);

    CARAPI NMeshCreate(
        /* [in] */ ArrayOf<Int64>* vtx,
        /* [in] */ ArrayOf<Int64>* idx,
        /* [in] */ ArrayOf<Int32>* prim,
        /* [out] */ Int64* result);

    CARAPI_(Int32) RsnMeshGetVertexBufferCount(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id);

    CARAPI NMeshGetVertexBufferCount(
        /* [in] */ Int64 id,
        /* [out] */ Int32* count);

    CARAPI_(Int32) RsnMeshGetIndexCount(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id);

    CARAPI NMeshGetIndexCount(
        /* [in] */ Int64 id,
        /* [out] */ Int32* count);

    CARAPI_(void) RsnMeshGetVertices(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* vtxIds,
        /* [in] */ Int32 vtxIdCount);

    CARAPI NMeshGetVertices(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* vtxIds,
        /* [in] */ Int32 vtxIdCount);

    CARAPI_(void) RsnMeshGetIndices(
        /* [in] */ Int64 con,
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* idxIds,
        /* [in] */ ArrayOf<Int32>* primitives,
        /* [in] */ Int32 vtxIdCount);

    CARAPI NMeshGetIndices(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* idxIds,
        /* [in] */ ArrayOf<Int32>* primitives,
        /* [in] */ Int32 vtxIdCount);

    CARAPI_(Int64) RsnPathCreate(
        /* [in] */ Int64 con,
        /* [in] */ Int32 prim,
        /* [in] */ Boolean isStatic,
        /* [in] */ Int64 vtx,
        /* [in] */ Int64 loop,
        /* [in] */ Float q);

    CARAPI NPathCreate(
        /* [in] */ Int32 prim,
        /* [in] */ Boolean isStatic,
        /* [in] */ Int64 vtx,
        /* [in] */ Int64 loop,
        /* [in] */ Float q,
        /* [out] */ Int64* result);

    CARAPI ValidateObject(
        /* [in] */ BaseObj* o);

    CARAPI Validate();

    CARAPI_(Boolean) IsAlive();

    CARAPI_(Int64) SafeID(
        /* [in] */ BaseObj* o);

private:
    CARAPI_(android::sp<ANativeWindow>) SurfaceGetNativeWindow(
        /* [in] */ ISurface* surfaceObj);

protected:
    static Int64 TRACE_TAG = 1L << 15;// = Trace.TRACE_TAG_RS;

    static const String LOG_TAG;
    static const Boolean DEBUG  = FALSE;
    // @SuppressWarnings({"UnusedDeclaration", "deprecation"})
    static const Boolean LOG_ENABLED = FALSE;

    /*
     * We use a class initializer to allow the native code to cache some
     * field offsets.
     */
    // @SuppressWarnings({"FieldCanBeLocal", "UnusedDeclaration"}) // TODO: now used locally; remove?
    static Boolean sInitialized;

    static Int32 sPointerSize;

    static AutoPtr<IFile> sCacheDir;

    // this should be a monotonically increasing ID
    // used in conjunction with the API version of a device
    static const Int64 sMinorID = 1;

    ContextType mContextType;
    AutoPtr<IReentrantReadWriteLock> mRWLock;

    Int64 mDev;
    Int64 mContext;
    // @SuppressWarnings({"FieldCanBeLocal"})
    AutoPtr<MessageThread> mMessageThread;

    AutoPtr<IElement> mElement_U8;
    AutoPtr<IElement> mElement_I8;
    AutoPtr<IElement> mElement_U16;
    AutoPtr<IElement> mElement_I16;
    AutoPtr<IElement> mElement_U32;
    AutoPtr<IElement> mElement_I32;
    AutoPtr<IElement> mElement_U64;
    AutoPtr<IElement> mElement_I64;
    AutoPtr<IElement> mElement_F32;
    AutoPtr<IElement> mElement_F64;
    AutoPtr<IElement> mElement_BOOLEAN;

    AutoPtr<IElement> mElement_ELEMENT;
    AutoPtr<IElement> mElement_TYPE;
    AutoPtr<IElement> mElement_ALLOCATION;
    AutoPtr<IElement> mElement_SAMPLER;
    AutoPtr<IElement> mElement_SCRIPT;
    AutoPtr<IElement> mElement_MESH;
    AutoPtr<IElement> mElement_PROGRAM_FRAGMENT;
    AutoPtr<IElement> mElement_PROGRAM_VERTEX;
    AutoPtr<IElement> mElement_PROGRAM_RASTER;
    AutoPtr<IElement> mElement_PROGRAM_STORE;
    AutoPtr<IElement> mElement_FONT;

    AutoPtr<IElement> mElement_A_8;
    AutoPtr<IElement> mElement_RGB_565;
    AutoPtr<IElement> mElement_RGB_888;
    AutoPtr<IElement> mElement_RGBA_5551;
    AutoPtr<IElement> mElement_RGBA_4444;
    AutoPtr<IElement> mElement_RGBA_8888;

    AutoPtr<IElement> mElement_FLOAT_2;
    AutoPtr<IElement> mElement_FLOAT_3;
    AutoPtr<IElement> mElement_FLOAT_4;

    AutoPtr<IElement> mElement_DOUBLE_2;
    AutoPtr<IElement> mElement_DOUBLE_3;
    AutoPtr<IElement> mElement_DOUBLE_4;

    AutoPtr<IElement> mElement_UCHAR_2;
    AutoPtr<IElement> mElement_UCHAR_3;
    AutoPtr<IElement> mElement_UCHAR_4;

    AutoPtr<IElement> mElement_CHAR_2;
    AutoPtr<IElement> mElement_CHAR_3;
    AutoPtr<IElement> mElement_CHAR_4;

    AutoPtr<IElement> mElement_USHORT_2;
    AutoPtr<IElement> mElement_USHORT_3;
    AutoPtr<IElement> mElement_USHORT_4;

    AutoPtr<IElement> mElement_SHORT_2;
    AutoPtr<IElement> mElement_SHORT_3;
    AutoPtr<IElement> mElement_SHORT_4;

    AutoPtr<IElement> mElement_UINT_2;
    AutoPtr<IElement> mElement_UINT_3;
    AutoPtr<IElement> mElement_UINT_4;

    AutoPtr<IElement> mElement_INT_2;
    AutoPtr<IElement> mElement_INT_3;
    AutoPtr<IElement> mElement_INT_4;

    AutoPtr<IElement> mElement_ULONG_2;
    AutoPtr<IElement> mElement_ULONG_3;
    AutoPtr<IElement> mElement_ULONG_4;

    AutoPtr<IElement> mElement_LONG_2;
    AutoPtr<IElement> mElement_LONG_3;
    AutoPtr<IElement> mElement_LONG_4;

    AutoPtr<IElement> mElement_YUV;

    AutoPtr<IElement> mElement_MATRIX_4X4;
    AutoPtr<IElement> mElement_MATRIX_3X3;
    AutoPtr<IElement> mElement_MATRIX_2X2;

    AutoPtr<ISampler> mSampler_CLAMP_NEAREST;
    AutoPtr<ISampler> mSampler_CLAMP_LINEAR;
    AutoPtr<ISampler> mSampler_CLAMP_LINEAR_MIP_LINEAR;
    AutoPtr<ISampler> mSampler_WRAP_NEAREST;
    AutoPtr<ISampler> mSampler_WRAP_LINEAR;
    AutoPtr<ISampler> mSampler_WRAP_LINEAR_MIP_LINEAR;
    AutoPtr<ISampler> mSampler_MIRRORED_REPEAT_NEAREST;
    AutoPtr<ISampler> mSampler_MIRRORED_REPEAT_LINEAR;
    AutoPtr<ISampler> mSampler_MIRRORED_REPEAT_LINEAR_MIP_LINEAR;

    AutoPtr<ProgramStore> mProgramStore_BLEND_NONE_DEPTH_TEST;
    AutoPtr<ProgramStore> mProgramStore_BLEND_NONE_DEPTH_NO_DEPTH;
    AutoPtr<ProgramStore> mProgramStore_BLEND_ALPHA_DEPTH_TEST;
    AutoPtr<ProgramStore> mProgramStore_BLEND_ALPHA_DEPTH_NO_DEPTH;

    AutoPtr<ProgramRaster> mProgramRaster_CULL_BACK;
    AutoPtr<ProgramRaster> mProgramRaster_CULL_FRONT;
    AutoPtr<ProgramRaster> mProgramRaster_CULL_NONE;

    /**
     * If an application is expecting messages, it should set this
     * field to an instance of {@link RSMessageHandler}.  This
     * instance will receive all the user messages sent from {@code
     * sendToClient} by scripts from this context.
     *
     */
    AutoPtr<RSMessageHandler> mMessageCallback;

    /**
     * Application Error handler.  All runtime errors will be dispatched to the
     * instance of RSAsyncError set here.  If this field is null a
     * {@link RSRuntimeException} will instead be thrown with details about the error.
     * This will cause program termaination.
     *
     */
    AutoPtr<RSErrorHandler> mErrorCallback;

private:
    AutoPtr<IContext> mApplicationContext;

    static StaticInitializer sInitializer;

    friend class Allocation;
    friend class Element;
    friend class StaticInitializer;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__
