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

#include "elastos/droid/server/CAssetAtlasService.h"
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/Process.h>
#include <elastos/core/Math.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/system/OsConstants.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Libcore.h>

#include <cutils/log.h>
#include <utils/StrongPointer.h>

#include <ui/GraphicBuffer.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <SkCanvas.h>
#include <SkBitmap.h>

using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IIAssetAtlas;
using Elastos::Droid::View::IGraphicBufferHelper;
using Elastos::Droid::View::CGraphicBufferHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::AtlasType_SliceMinArea;
using Elastos::Droid::Graphics::AtlasType_COUNT;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableConstantState;
using Elastos::Droid::Graphics::CAtlas;
using Elastos::Droid::Graphics::CAtlasEntry;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparator;
using Elastos::IO::IWriter;
using Elastos::IO::IReader;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::CBufferedReader;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Name of the <code>CAssetAtlasService</code>.
 */
const String CAssetAtlasService::ASSET_ATLAS_SERVICE("assetatlas");
const String CAssetAtlasService::TAG("CAssetAtlasService");

// Turns debug logs on/off. Debug logs are kept to a minimum and should
// remain on to diagnose issues
const Boolean CAssetAtlasService::DEBUG_ATLAS = FALSE;

// When set to TRUE the content of the atlas will be saved to disk
// in /data/system/atlas.png. The shared GraphicBuffer may be empty
const Boolean CAssetAtlasService::DEBUG_ATLAS_TEXTURE = FALSE;

// Minimum size in pixels to consider for the resulting texture
const Int32 CAssetAtlasService::MIN_SIZE = 768;
// Maximum size in pixels to consider for the resulting texture
const Int32 CAssetAtlasService::MAX_SIZE = 2048;
// Increment in number of pixels between size variants when looking
// for the best texture dimensions
const Int32 CAssetAtlasService::STEP = 64;

// This percentage of the total number of pixels represents the minimum
// number of pixels we want to be able to pack in the atlas
const Float CAssetAtlasService::PACKING_THRESHOLD = 0.8f;

// Defines the number of Int32 fields used to represent a single entry
// in the atlas map. This number defines the size of the array returned
// by the GetMap(). See the mAtlasMap field for more information
const Int32 CAssetAtlasService::ATLAS_MAP_ENTRY_FIELD_COUNT = 4;

// Specifies how our GraphicBuffer will be used. To get proper swizzling
// the buffer will be written to using OpenGL (from JNI) so we can leave
// the software flag set to "never"
const Int32 CAssetAtlasService::GRAPHIC_BUFFER_USAGE =
    IGraphicBuffer::USAGE_SW_READ_NEVER |
    IGraphicBuffer::USAGE_SW_WRITE_NEVER |
    IGraphicBuffer::USAGE_HW_TEXTURE;

//===============================================================================
// CAssetAtlasService::Renderer
//===============================================================================

CAssetAtlasService::Renderer::Renderer(
    /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
    /* [in] */ Int32 pixelCount,
    /* [in] */ CAssetAtlasService* host)
{
    mBitmaps = bitmaps;
    mPixelCount = pixelCount;
    mNativeBitmap = 0;
    mHost = host;
}

ECode CAssetAtlasService::Renderer::Run()
{
    AutoPtr<Configuration> config = mHost->ChooseConfiguration(
        mBitmaps, mPixelCount, mHost->mVersionName);
    if (DEBUG_ATLAS) Logger::D(CAssetAtlasService::TAG, "Loaded configuration: %s", TO_CSTR(config));

    if (config != NULL) {
        AutoPtr<IGraphicBufferHelper> helper;
        CGraphicBufferHelper::AcquireSingleton((IGraphicBufferHelper**)&helper);

        mHost->mBuffer = NULL;
        helper->Create(config->mWidth, config->mHeight,
            IPixelFormat::RGBA_8888,
            CAssetAtlasService::GRAPHIC_BUFFER_USAGE, (IGraphicBuffer**)&mHost->mBuffer);

        if (mHost->mBuffer != NULL) {
            AutoPtr<IAtlas> atlas;
            CAtlas::New(config->mType, config->mWidth, config->mHeight, config->mFlags, (IAtlas**)&atlas);
            if (RenderAtlas(mHost->mBuffer, atlas, config->mCount)) {
                mHost->mAtlasReady->Set(TRUE);
            }
        }
    }
    return NOERROR;
}

Boolean CAssetAtlasService::Renderer::RenderAtlas(
    /* [in] */ IGraphicBuffer* buffer,
    /* [in] */ IAtlas* atlas,
    /* [in] */ Int32 packCount)
{
    if (DEBUG_ATLAS) Logger::I(TAG, " RenderAtlas: packCount=%d, buffer:%s", packCount, TO_CSTR(buffer));
    // Use a Source blend mode to improve performance, the target bitmap
    // will be zero'd out so there's no need to waste time applying blending
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    AutoPtr<IPorterDuffXfermode> pdm;
    CPorterDuffXfermode::New(PorterDuffMode_SRC, (IPorterDuffXfermode**)&pdm);
    paint->SetXfermode(IXfermode::Probe(pdm));

    // We always render the atlas into a bitmap. This bitmap is then
    // uploaded into the GraphicBuffer using OpenGL to swizzle the content
    Int32 w, h;
    buffer->GetWidth(&w);
    buffer->GetHeight(&h);
    AutoPtr<ICanvas> canvas = AcquireCanvas(w, h);
    if (canvas == NULL) return FALSE;

    AutoPtr<IAtlasEntry> entry;
    CAtlasEntry::New((IAtlasEntry**)&entry);

    mHost->mAtlasMap = ArrayOf<Int64>::Alloc(packCount * ATLAS_MAP_ENTRY_FIELD_COUNT);
    AutoPtr< ArrayOf<Int64> > atlasMap = mHost->mAtlasMap;
    Int32 mapIndex = 0;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Boolean result = FALSE;

    Int64 startRender;
    system->GetNanoTime(&startRender);

    Int32 x, y, ival;
    Handle64 bmp;
    Boolean rotated;
    List<AutoPtr<IBitmap> >::Iterator it;
    Int32 index = 0;
    for (it = mBitmaps->Begin(); it != mBitmaps->End(); ++it) {
        AutoPtr<IBitmap> bitmap = *it;
        bitmap->GetWidth(&w);
        bitmap->GetHeight(&h);
        AutoPtr<IAtlasEntry> ae;
        atlas->Pack(w, h, entry, (IAtlasEntry**)&ae);
        if (ae != NULL) {
            // We have more bitmaps to pack than the current configuration
            // says, we were most likely not able to detect a change in the
            // list of preloaded drawables, abort and delete the configuration
            if (mapIndex >= mHost->mAtlasMap->GetLength()) {
                DeleteDataFile();
                break;
            }

            entry->GetX(&x);
            entry->GetY(&y);
            entry->GetRotated(&rotated);
            canvas->Save(&ival);
            canvas->Translate(x, y);
            if (rotated) {
                bitmap->GetHeight(&h);
                canvas->Translate(h, 0.0f);
                canvas->Rotate(90.0f);
            }
            canvas->DrawBitmap(bitmap, 0.0f, 0.0f, NULL);
            canvas->Restore();

            bitmap->GetNativeBitmap(&bmp);
            atlasMap->Set(mapIndex++, bmp);
            atlasMap->Set(mapIndex++, x);
            atlasMap->Set(mapIndex++, y);
            atlasMap->Set(mapIndex++, rotated ? 1 : 0);
        }
        else {
            if (DEBUG_ATLAS) Logger::W(TAG, " RenderAtlas: pack bitmap at %d failed, bitmap:%s, w:h=(%d, %d)",
                index, TO_CSTR(bitmap), w, h);
        }
        index++;
    }

    Int64 endRender;
    system->GetNanoTime(&endRender);
    if (mNativeBitmap != 0) {
        result = mHost->NavtiveUploadAtlas(buffer, mNativeBitmap);
    }

    Int64 endUpload;
    system->GetNanoTime(&endUpload);
    if (DEBUG_ATLAS) {
        Float renderDuration = (endRender - startRender) / 1000.0f / 1000.0f;
        Float uploadDuration = (endUpload - endRender) / 1000.0f / 1000.0f;
        Logger::D(CAssetAtlasService::TAG, "Rendered atlas in %.2fms (%.2f+%.2fms)",
            renderDuration + uploadDuration, renderDuration, uploadDuration);
    }

    ReleaseCanvas(canvas);
    return result;
}

AutoPtr<ICanvas> CAssetAtlasService::Renderer::AcquireCanvas(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<ICanvas> canvas;

    if (DEBUG_ATLAS_TEXTURE) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&mAtlasBitmap);
        CCanvas::New(mAtlasBitmap, (ICanvas**)&canvas);
    }
    else {
        CCanvas::New((ICanvas**)&canvas);
        mNativeBitmap = mHost->NativeAcquireAtlasCanvas(canvas, width, height);
    }
    return canvas;
}

void CAssetAtlasService::Renderer::ReleaseCanvas(
    /* [in] */ ICanvas* canvas)
{
    if (DEBUG_ATLAS_TEXTURE) {
        canvas->SetBitmap(NULL);

        AutoPtr<IFile> systemDirectory;
        CFile::New(Environment::GetDataDirectory(), String("system"), (IFile**)&systemDirectory);
        AutoPtr<IFile> dataFile;
        CFile::New(systemDirectory, String("atlas.png"), (IFile**)&dataFile);

        // try {
        AutoPtr<IOutputStream> out;
        CFileOutputStream::New(dataFile, (IOutputStream**)&out);
        Boolean bval;
        mAtlasBitmap->Compress(BitmapCompressFormat_PNG, 100, out, &bval);
        ICloseable::Probe(out)->Close();
        // } catch (FileNotFoundException e) {
        //     // Ignore
        // } catch (IOException e) {
        //     // Ignore
        // }

        mAtlasBitmap->Recycle();
        mAtlasBitmap = NULL;
    }
    else {
        mHost->NativeReleaseAtlasCanvas(canvas, mNativeBitmap);
    }
}

//===============================================================================
// CAssetAtlasService::Configuration
//===============================================================================

CAssetAtlasService::Configuration::Configuration(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 count)
    : mType(type)
    , mWidth(width)
    , mHeight(height)
    , mCount(count)
    , mFlags(IAtlas::FLAG_DEFAULTS)
{
}

CAssetAtlasService::Configuration::Configuration(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 count,
    /* [in] */ Int32 flags)
    : mType(type)
    , mWidth(width)
    , mHeight(height)
    , mCount(count)
    , mFlags(flags)
{}

//@Override
ECode CAssetAtlasService::Configuration::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(128);
    sb += "configuration{type=";
    sb += mType;
    sb += ", (";
    sb += mWidth;
    sb += "x";
    sb += mHeight;
    sb += ")";
    sb += ", flags=0x";
    sb += StringUtils::ToHexString(mFlags);
    sb += ", count=";
    sb += mCount;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//===============================================================================
// CAssetAtlasService::WorkerResult
//===============================================================================

CAssetAtlasService::WorkerResult::WorkerResult(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 count)
    : mType(type)
    , mWidth(width)
    , mHeight(height)
    , mCount(count)
{
}

//@Override
ECode CAssetAtlasService::WorkerResult::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(128);
    sb += mType;
    sb += "(";
    sb += mWidth;
    sb += "x";
    sb += mHeight;
    sb += ")";
    *str = sb.ToString();
    return NOERROR;
}

//===============================================================================
// CAssetAtlasService::ComputeWorker
//===============================================================================

CAssetAtlasService::ComputeWorker::ComputeWorker(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 step,
    /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
    /* [in] */ Int32 pixelCount,
    /* [in] */ IList* results,
    /* [in] */ ICountDownLatch* signal)
    : mStart(start)
    , mEnd(end)
    , mStep(step)
    , mBitmaps(bitmaps)
    , mResults(results)
    , mSignal(signal)
{
    // Minimum number of pixels we want to be able to pack
    Int32 threshold = (Int32) (pixelCount * PACKING_THRESHOLD);
    // Make sure we can find at least one configuration
    while (threshold > MAX_SIZE * MAX_SIZE) {
        threshold >>= 1;
    }
    mThreshold = threshold;
}

//@Override
ECode CAssetAtlasService::ComputeWorker::Run()
{
    if (DEBUG_ATLAS) {
        String name;
        Thread::GetCurrentThread()->GetName(&name);
        Logger::D(TAG, "Running %s", name.string());
    }

    AutoPtr<IAtlasEntry> entry;
    CAtlasEntry::New((IAtlasEntry**)&entry);
    for (Int32 type = AtlasType_SliceMinArea; type < AtlasType_COUNT; ++type) {
        for (Int32 width = mStart; width < mEnd; width += mStep) {
            for (Int32 height = MIN_SIZE; height < MAX_SIZE; height += STEP) {
                // If the atlas is not big enough, skip it
                if (width * height <= mThreshold) continue;
                Int32 count = PackBitmaps(type, width, height, entry);
                if (count > 0) {
                    AutoPtr<WorkerResult> wr = new WorkerResult(type, width, height, count);
                    mResults->Add(TO_IINTERFACE(wr));
                    // If we were able to pack everything let's stop here
                    // Increasing the height further won't make things better
                    if (count == (Int32)mBitmaps->GetSize()) {
                        break;
                    }
                }
            }
        }
    }

    if (mSignal != NULL) {
        mSignal->CountDown();
    }
    return NOERROR;
}

Int32 CAssetAtlasService::ComputeWorker::PackBitmaps(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IAtlasEntry* entry)
{
    Int32 total = 0;
    AutoPtr<IAtlas> atlas;
    CAtlas::New(type, width, height, (IAtlas**)&atlas);

    Int32 w, h;
    List<AutoPtr<IBitmap> >::Iterator it;
    for (it = mBitmaps->Begin(); it != mBitmaps->End(); ++it) {
        AutoPtr<IBitmap> bitmap = *it;
        if (bitmap) {
            bitmap->GetWidth(&w);
            bitmap->GetHeight(&h);
            AutoPtr<IAtlasEntry> ae;
            atlas->Pack(w, h, entry, (IAtlasEntry**)&ae);
            if (ae != NULL) {
                total++;
            }
        }
    }

    return total;
}

//===============================================================================
// CAssetAtlasService::BitmapComparator
//===============================================================================

CAR_INTERFACE_IMPL(CAssetAtlasService::BitmapComparator, Object, IComparator)

//@Override
ECode CAssetAtlasService::BitmapComparator::Compare(
    /* [in] */ IInterface* o1,
    /* [in] */ IInterface* o2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    IBitmap* b1 = IBitmap::Probe(o1);
    IBitmap* b2 = IBitmap::Probe(o2);

    Int32 w1, w2, h1, h2;
    b1->GetWidth(&w1);
    b2->GetWidth(&w2);
    b1->GetHeight(&h1);
    b2->GetHeight(&h2);

    if (w1 == w2) {
        *result = h2 - h1;
        return NOERROR;
    }
    *result = w2 - w1;
    return NOERROR;
}

//===============================================================================
// CAssetAtlasService::BitmapComparator
//===============================================================================

CAR_INTERFACE_IMPL(CAssetAtlasService::WorkerResultComparator, Object, IComparator)

CARAPI CAssetAtlasService::WorkerResultComparator::Compare(
    /* [in] */ IInterface* o1,
    /* [in] */ IInterface* o2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    WorkerResult* r1 = (WorkerResult*)IObject::Probe(o1);
    WorkerResult* r2 = (WorkerResult*)IObject::Probe(o2);

    Int32 delta = r2->mCount - r1->mCount;
    if (delta != 0) {
        *result = delta;
        return NOERROR;
    }
    *result = r1->mWidth * r1->mHeight - r2->mWidth * r2->mHeight;
    return NOERROR;
}

//===============================================================================
// CAssetAtlasService
//===============================================================================

CAR_INTERFACE_IMPL_2(CAssetAtlasService, Object, IIAssetAtlas, IBinder)

CAR_OBJECT_IMPL(CAssetAtlasService)

CAssetAtlasService::CAssetAtlasService()
{}

ECode CAssetAtlasService::constructor(
    /* [in] */ IContext* context)
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mAtlasReady);
    mContext = context;
    mVersionName = QueryVersionName(context);

    AutoPtr<IArrayList> bitmaps;
    CArrayList::New(300, (IArrayList**)&bitmaps);
    Int32 totalPixelCount = 0;

    // We only care about drawables that hold bitmaps
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IInt64SparseArray> drawables;
    resources->GetPreloadedDrawables((IInt64SparseArray**)&drawables);

    Int32 count, w, h;
    drawables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        drawables->ValueAt(i, (IInterface**)&obj);
        IDrawableConstantState* dcs = IDrawableConstantState::Probe(obj);
        AutoPtr<IBitmap> bitmap;
        dcs->GetBitmap((IBitmap**)&bitmap);
        if (bitmap != NULL) {
            BitmapConfig config;
            bitmap->GetConfig(&config);
            if (config == BitmapConfig_ARGB_8888) {
                bitmaps->Add(bitmap.Get());
                bitmap->GetWidth(&w);
                bitmap->GetHeight(&h);
                totalPixelCount += w * h;
            }
        }
    }

    // Our algorithms perform better when the bitmaps are first sorted
    // The comparator will sort the bitmap by width first, then by height
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    AutoPtr<IComparator> cpt = new BitmapComparator();
    collections->Sort(IList::Probe(bitmaps), cpt);

    // Kick off the packing work on a worker thread
    AutoPtr<List<AutoPtr<IBitmap> > > bmps = new List<AutoPtr<IBitmap> >();
    AutoPtr<IIterator> it;
    bitmaps->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IBitmap> bmp = IBitmap::Probe(obj);
        bmps->PushBack(bmp);
    }

    AutoPtr<IRunnable> runnable = new Renderer(bmps, totalPixelCount, this);
    AutoPtr<IThread> thread;
    CThread::New(runnable, (IThread**)&thread);
    return thread->Start();
}

String CAssetAtlasService::QueryVersionName(
    /* [in] */ IContext* context)
{
    String packageName;
    context->GetPackageName(&packageName);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IPackageInfo> info;
    ECode ec = pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
    if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        Logger::W(TAG, "Could not get package info");
        return String(NULL);
    }
    String str;
    info->GetVersionName(&str);
    return str;
}

ECode CAssetAtlasService::SystemRunning()
{
    return NOERROR;
}

ECode CAssetAtlasService::IsCompatible(
    /* [in] */ Int32 ppid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ppid == Process::MyPpid();
    return NOERROR;
}

ECode CAssetAtlasService::GetBuffer(
    /* [out] */ IGraphicBuffer** gb)
{
    VALIDATE_NOT_NULL(gb)
    *gb = NULL;
    Boolean ready;
    mAtlasReady->Get(&ready);
    if (ready) {
        *gb = mBuffer;
        REFCOUNT_ADD(*gb)
    }
    return NOERROR;
}

ECode CAssetAtlasService::GetMap(
    /* [out, callee] */ ArrayOf<Int64>** map)
{
    VALIDATE_NOT_NULL(map)
    *map = NULL;
    Boolean ready;
    mAtlasReady->Get(&ready);
    if (ready) {
        *map = mAtlasMap;
        REFCOUNT_ADD(*map)
    }
    return NOERROR;
}

AutoPtr<CAssetAtlasService::Configuration> CAssetAtlasService::ComputeBestConfiguration(
    /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
    /* [in] */ Int32 pixelCount)
{
    if (DEBUG_ATLAS) {
        Logger::D(TAG, "Computing best atlas configuration...bitmaps:%d, pixelCount:%d",
            bitmaps ? bitmaps->GetSize() : 0, pixelCount);
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 begin;
    system->GetNanoTime(&begin);

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);

    AutoPtr<IList> wrList;
    CArrayList::New((IList**)&wrList);
    AutoPtr<IList> results;
    collections->SynchronizedList(wrList, (IList**)&results);

    // Don't bother with an extra thread if there's only one processor
    AutoPtr<IOs> os;
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    Int64 numCpu = 1;
    os->Sysconf(OsConstants::__SC_NPROCESSORS_CONF, &numCpu);
    Int32 cpuCount = numCpu;
    assert(cpuCount > 0);

    if (cpuCount == 1) {
        AutoPtr<ComputeWorker> cw = new ComputeWorker(
            MIN_SIZE, MAX_SIZE, STEP, bitmaps, pixelCount, results, NULL);
        cw->Run();
    }
    else {
        Int32 start = MIN_SIZE;
        Int32 end = MAX_SIZE - (cpuCount - 1) * STEP;
        Int32 step = STEP * cpuCount;
        Boolean bval;

        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> seconds;
        helper->GetSECONDS((ITimeUnit**)&seconds);
        AutoPtr<ICountDownLatch> sig;
        CCountDownLatch::New(cpuCount, (ICountDownLatch**)&sig);

        for (Int32 i = 0; i < cpuCount; i++, start += STEP, end += STEP) {
            AutoPtr<ComputeWorker> worker = new ComputeWorker(start, end, step,
                    bitmaps, pixelCount, results, sig);
            StringBuilder sb("Atlas Worker #");
            sb += (i + 1);
            AutoPtr<IThread> thread;
            CThread::New((IRunnable*)worker.Get(), sb.ToString(), (IThread**)&thread);
            thread->Start();
        }

        ECode ec = sig->Await(10, seconds, &bval);
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
            Logger::W(TAG, "Could not complete configuration computation");
            return NULL;
        }
    }

    Int32 rSize = 0;
    results->GetSize(&rSize);
    if (rSize == 0) {
        if (DEBUG_ATLAS) {
            Logger::W(TAG, "No atlas configuration found!");
        }
        return NULL;
    }

    // Maximize the number of packed bitmaps, minimize the texture size
    AutoPtr<IComparator> wrCmp = new WorkerResultComparator();
    collections->Sort(results, wrCmp);

    if (DEBUG_ATLAS) {
        Int64 nt;
        system->GetNanoTime(&nt);
        Float delay = (nt - begin) / 1000.0f / 1000.0f / 1000.0f;
        Logger::D(TAG, "Found best atlas configuration in %.2fs", delay);
    }

    AutoPtr<IInterface> obj;
    results->Get(0, (IInterface**)&obj);
    AutoPtr<WorkerResult> result = (WorkerResult*)IObject::Probe(obj);
    AutoPtr<Configuration> cfg = new Configuration(result->mType, result->mWidth,
        result->mHeight, result->mCount);
    return cfg;
}

AutoPtr<IFile> CAssetAtlasService::GetDataFile()
{
    AutoPtr<IFile> dir = Environment::GetDataDirectory();
    AutoPtr<IFile> systemDirectory;
    CFile::New(dir, String("system"), (IFile**)&systemDirectory);
    AutoPtr<IFile> result;
    CFile::New(systemDirectory, String("framework_atlas.config"), (IFile**)&result);
    return result;
}

void CAssetAtlasService::DeleteDataFile()
{
    Logger::W(TAG, "Current configuration inconsistent with assets list");
    AutoPtr<IFile> file = GetDataFile();
    Boolean bval;
    file->Delete(&bval);
    if (!bval) {
        Logger::W(TAG, "Could not delete the current configuration");
    }
}

AutoPtr<IFile> CAssetAtlasService::GetFrameworkResourcesFile()
{
    AutoPtr<IApplicationInfo> ai;
    mContext->GetApplicationInfo((IApplicationInfo**)&ai);
    String sourceDir;
    ai->GetSourceDir(&sourceDir);
    AutoPtr<IFile> result;
    CFile::New(sourceDir, (IFile**)&result);
    return result;
}

AutoPtr<CAssetAtlasService::Configuration> CAssetAtlasService::ChooseConfiguration(
    /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
    /* [in] */ Int32 pixelCount,
    /* [in] */ const String& versionName)
{
    AutoPtr<Configuration> config;

    AutoPtr<IFile> dataFile = GetDataFile();
    Boolean exists;
    dataFile->Exists(&exists);
    if (exists) {
        config = ReadConfiguration(dataFile, versionName);
    }

    if (config == NULL) {
        config = ComputeBestConfiguration(bitmaps, pixelCount);
        if (config != NULL) WriteConfiguration(config, dataFile, versionName);
    }

    return config;
}

void CAssetAtlasService::WriteConfiguration(
    /* [in] */ Configuration* config,
    /* [in] */ IFile* file,
    /* [in] */ const String& versionName)
{
    AutoPtr<IBufferedWriter> writer;
    AutoPtr<IOutputStream> out;
    CFileOutputStream::New(file, (IOutputStream**)&out);
    AutoPtr<IWriter> wr;
    COutputStreamWriter::New(out, (IWriter**)&wr);
    CBufferedWriter::New(wr, (IBufferedWriter**)&writer);
    IWriter* bw = IWriter::Probe(writer);

    bw->Write(GetBuildIdentifier(versionName));
    writer->NewLine();
    bw->Write(StringUtils::ToString((Int32)config->mType));
    writer->NewLine();
    bw->Write(StringUtils::ToString(config->mWidth));
    writer->NewLine();
    bw->Write(StringUtils::ToString(config->mHeight));
    writer->NewLine();
    bw->Write(StringUtils::ToString(config->mCount));
    writer->NewLine();
    bw->Write(StringUtils::ToString(config->mFlags));
    writer->NewLine();

    // } catch (FileNotFoundException e) {
    //     Logger::W(TAG, "Could not write " + file, e);
    // } catch (IOException e) {
    //     Logger::W(TAG, "Could not write " + file, e);
    // } finally
    if (writer != NULL) {
        ICloseable::Probe(writer)->Close();
    }
}

AutoPtr<CAssetAtlasService::Configuration> CAssetAtlasService::ReadConfiguration(
    /* [in] */ IFile* file,
    /* [in] */ const String& versionName)
{
    AutoPtr<IBufferedReader> reader;
    AutoPtr<Configuration> config;

    AutoPtr<IInputStream> in;
    CFileInputStream::New(file, (IInputStream**)&in);
    AutoPtr<IReader> rd;
    CInputStreamReader::New(in, (IReader**)&rd);
    CBufferedReader::New(rd, (IBufferedReader**)&reader);

    if (CheckBuildIdentifier(reader, versionName)) {
        String line;
        reader->ReadLine(&line);
        AtlasType type = (AtlasType)StringUtils::ParseInt32(line);
        Int32 width = ReadInt(reader, MIN_SIZE, MAX_SIZE);
        Int32 height = ReadInt(reader, MIN_SIZE, MAX_SIZE);
        Int32 count = ReadInt(reader, 0, Elastos::Core::Math::INT32_MAX_VALUE);
        Int32 flags = ReadInt(reader, Elastos::Core::Math::INT32_MIN_VALUE,
            Elastos::Core::Math::INT32_MAX_VALUE);

        config = new Configuration(type, width, height, count, flags);
    }
    // } catch (IllegalArgumentException e) {
    //     Logger::W(TAG, "Invalid parameter value in " + file, e);
    // } catch (FileNotFoundException e) {
    //     Logger::W(TAG, "Could not read " + file, e);
    // } catch (IOException e) {
    //     Logger::W(TAG, "Could not read " + file, e);
    // } finally {
    if (reader != NULL) {
        ICloseable::Probe(reader)->Close();
    }

    return config;
}

Int32 CAssetAtlasService::ReadInt(
    /* [in] */ IBufferedReader* reader,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    String line;
    reader->ReadLine(&line);
    Int32 value = StringUtils::ParseInt32(line);
    value = Elastos::Core::Math::Min(max, value);
    value = Elastos::Core::Math::Max(min, value);
    return value;
}

Boolean CAssetAtlasService::CheckBuildIdentifier(
    /* [in] */ IBufferedReader* reader,
    /* [in] */ const String& versionName)
{
    String deviceBuildId = GetBuildIdentifier(versionName);
    String buildId;
    reader->ReadLine(&buildId);
    return deviceBuildId.Equals(buildId);
}

String CAssetAtlasService::GetBuildIdentifier(
    /* [in] */ const String& versionName)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String id;
    sysProp->Get(String("ro.build.fingerprint"), String(""), &id);
    StringBuilder sb(id);
    sb += '/';
    sb += versionName;
    sb += '/';

    AutoPtr<IFile> file = GetFrameworkResourcesFile();
    Int64 length;
    file->GetLength(&length);
    sb += StringUtils::ToString(length);
    return sb.ToString();
}

//=======================================================================
// Native
//=======================================================================

Int64 CAssetAtlasService::NativeAcquireAtlasCanvas(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    SkBitmap* bitmap = new SkBitmap;
    bitmap->allocN32Pixels(width, height);
    bitmap->eraseColor(0);
    Int64 handle = reinterpret_cast<Int64>(bitmap);
    canvas->SetNativeBitmap(handle);

    return handle;
}

void CAssetAtlasService::NativeReleaseAtlasCanvas(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int64 bitmapHandle)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapHandle);
    canvas->SetNativeBitmap(0);
    delete bitmap;
}

// Defines how long to wait for the GPU when uploading the atlas
// This timeout is defined in nanoseconds (see EGL_KHR_fence_sync extension)
#define FENCE_TIMEOUT 2000000000

#define CLEANUP_GL_AND_RETURN(result) \
    if (fence != EGL_NO_SYNC_KHR) eglDestroySyncKHR(display, fence); \
    if (image) eglDestroyImageKHR(display, image); \
    if (texture) glDeleteTextures(1, &texture); \
    if (surface != EGL_NO_SURFACE) eglDestroySurface(display, surface); \
    if (context != EGL_NO_CONTEXT) eglDestroyContext(display, context); \
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT); \
    eglReleaseThread(); \
    eglTerminate(display); \
    return result;

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

static android::sp<android::GraphicBuffer> graphicBufferForJavaObject(IGraphicBuffer* obj)
{
    if (obj) {
        Int64 nativeObject;
        obj->GetNativeObject(&nativeObject);
        if (nativeObject != 0) {
            android::sp<android::GraphicBuffer> gb = reinterpret_cast<android::GraphicBuffer*>(nativeObject);
            return gb;
        }
    }
    return NULL;
}

Boolean CAssetAtlasService::NavtiveUploadAtlas(
    /* [in] */ IGraphicBuffer* graphicBuffer,
    /* [in] */ Int64 bitmapHandle)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapHandle);
    // The goal of this method is to copy the bitmap into the GraphicBuffer
    // using the GPU to swizzle the texture content
    android::sp<android::GraphicBuffer> buffer(graphicBufferForJavaObject(graphicBuffer));

    if (buffer != NULL) {
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) return FALSE;

        EGLint major;
        EGLint minor;
        if (!eglInitialize(display, &major, &minor)) {
            Logger::W(TAG, "Could not initialize EGL");
            return FALSE;
        }

        // We're going to use a 1x1 pbuffer surface later on
        // The configuration doesn't really matter for what we're trying to do
        EGLint configAttrs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 0,
                EGL_DEPTH_SIZE, 0,
                EGL_STENCIL_SIZE, 0,
                EGL_NONE
        };
        EGLConfig configs[1];
        EGLint configCount;
        if (!eglChooseConfig(display, configAttrs, configs, 1, &configCount)) {
            Logger::W(TAG, "Could not select EGL configuration");
            eglReleaseThread();
            eglTerminate(display);
            return FALSE;
        }
        if (configCount <= 0) {
            Logger::W(TAG, "Could not find EGL configuration");
            eglReleaseThread();
            eglTerminate(display);
            return FALSE;
        }

        // These objects are initialized below but the default "null"
        // values are used to cleanup properly at any point in the
        // initialization sequence
        GLuint texture = 0;
        EGLImageKHR image = EGL_NO_IMAGE_KHR;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLSyncKHR fence = EGL_NO_SYNC_KHR;

        EGLint attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        EGLContext context = eglCreateContext(display, configs[0], EGL_NO_CONTEXT, attrs);
        if (context == EGL_NO_CONTEXT) {
            Logger::W(TAG, "Could not create EGL context");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        // Create the 1x1 pbuffer
        EGLint surfaceAttrs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
        surface = eglCreatePbufferSurface(display, configs[0], surfaceAttrs);
        if (surface == EGL_NO_SURFACE) {
            Logger::W(TAG, "Could not create EGL surface");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        if (!eglMakeCurrent(display, surface, surface, context)) {
            Logger::W(TAG, "Could not change current EGL context");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        // We use an EGLImage to access the content of the GraphicBuffer
        // The EGL image is later bound to a 2D texture
        EGLClientBuffer clientBuffer = (EGLClientBuffer) buffer->getNativeBuffer();
        EGLint imageAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
        image = eglCreateImageKHR(display, EGL_NO_CONTEXT,
                EGL_NATIVE_BUFFER_ANDROID, clientBuffer, imageAttrs);
        if (image == EGL_NO_IMAGE_KHR) {
            Logger::W(TAG, "Could not create EGL image");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
        if (glGetError() != GL_NO_ERROR) {
            Logger::W(TAG, "Could not create/bind texture");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        // Upload the content of the bitmap in the GraphicBuffer
        glPixelStorei(GL_UNPACK_ALIGNMENT, bitmap->bytesPerPixel());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap->width(), bitmap->height(),
                GL_RGBA, GL_UNSIGNED_BYTE, bitmap->getPixels());
        if (glGetError() != GL_NO_ERROR) {
            Logger::W(TAG, "Could not upload to texture");
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        // The fence is used to wait for the texture upload to finish
        // properly. We cannot rely on glFlush() and glFinish() as
        // some drivers completely ignore these API calls
        fence = eglCreateSyncKHR(display, EGL_SYNC_FENCE_KHR, NULL);
        if (fence == EGL_NO_SYNC_KHR) {
            Logger::W(TAG, "Could not create sync fence %#x", eglGetError());
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        // The flag EGL_SYNC_FLUSH_COMMANDS_BIT_KHR will trigger a
        // pipeline flush (similar to what a glFlush() would do.)
        EGLint waitStatus = eglClientWaitSyncKHR(display, fence,
                EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, FENCE_TIMEOUT);
        if (waitStatus != EGL_CONDITION_SATISFIED_KHR) {
            Logger::W(TAG, "Failed to wait for the fence %#x", eglGetError());
            CLEANUP_GL_AND_RETURN(FALSE);
        }

        CLEANUP_GL_AND_RETURN(TRUE);
    }

    return FALSE;
}


} // Server
} // Droid
} // Elastos
