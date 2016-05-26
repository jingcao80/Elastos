
#ifndef __ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__
#define __ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__

#include "Elastos.Droid.RenderScript.h"

using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Utility::Concurrent::Locks::IReentrantReadWriteLock;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace RenderScript {

class Allocation;

class RenderScript
    : public Object
    , public IRenderScript
{
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

private:
    AutoPtr<IContext> mApplicationContext;

    static StaticInitializer sInitializer;

    friend class Allocation;
    friend class StaticInitializer;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_RENDERSCRIPT_RENDERSCEIPT_H__
