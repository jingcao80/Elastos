
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/pdf/CPdfRenderer.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/Matrix.h"
#include <elastos/droid/system/OsConstants.h>
#include <elastos/core/Mutex.h>
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkMatrix.h>
#include <fpdfview.h>
#include <fsdk_rendercontext.h>
#include <utils/Log.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::Mutex;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {

const Int32 CPdfRenderer::Page::RENDER_MODE_FOR_DISPLAY = 1;
const Int32 CPdfRenderer::Page::RENDER_MODE_FOR_PRINT = 2;
CAR_INTERFACE_IMPL(CPdfRenderer::Page, Object, IPdfRendererPage);
CPdfRenderer::Page::Page()
    : mIndex(0)
    , mWidth(0)
    , mHeight(0)
    , mNativePage(0)
{}

ECode CPdfRenderer::Page::constructor(
    /* [in] */ Int32 index,
    /* [in] */ CPdfRenderer* host)
{
    mHost = host;
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);

    AutoPtr<IPoint> size = mHost->mTempPoint;
    FAIL_RETURN(NativeOpenPageAndGetSize(mHost->mNativeDocument, index, size, &mNativePage));
    mIndex = index;
    size->Get(&mWidth, &mHeight);
    return mCloseGuard->Open(String("close"));
}

CPdfRenderer::Page::~Page()
{
    mCloseGuard->WarnIfOpen();
    if (mNativePage != 0) {
        DoClose();
    }
}

ECode CPdfRenderer::Page::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index =  mIndex;
    return NOERROR;
}

ECode CPdfRenderer::Page::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode CPdfRenderer::Page::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode CPdfRenderer::Page::Render(
    /* [in] */ /*@NonNull*/ IBitmap* destination,
    /* [in] */ /*@Nullable*/ IRect* destClip,
    /* [in] */ /*@Nullable*/ IMatrix* transform,
    /* [in] */ /*@RenderMode*/ Int32 renderMode)
{
    BitmapConfig config;
    if ((destination->GetConfig(&config)) != BitmapConfig_ARGB_8888) {
        // throw new IllegalArgumentException("Unsupported pixel format");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CRect* dest = (CRect*)destClip;
    Int32 width = 0, height = 0;
    destination->GetWidth(&width);
    destination->GetHeight(&height);
    if (dest != NULL) {
        if (dest->mLeft < 0 || dest->mTop < 0
                || dest->mRight > width
                || dest->mBottom > height) {
            // throw new IllegalArgumentException("destBounds not in destination");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    Boolean value = FALSE;
    if (transform != NULL && !(transform->IsAffine(&value), value)) {
         // throw new IllegalArgumentException("transform not affine");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (renderMode != RENDER_MODE_FOR_PRINT && renderMode != RENDER_MODE_FOR_DISPLAY) {
        // throw new IllegalArgumentException("Unsupported render mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (renderMode == RENDER_MODE_FOR_PRINT && renderMode == RENDER_MODE_FOR_DISPLAY) {
        // throw new IllegalArgumentException("Only single render mode supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 contentLeft = (dest != NULL) ? dest->mLeft : 0;
    Int32 contentTop = (dest != NULL) ? dest->mTop : 0;
    Int32 contentRight = (dest != NULL) ? dest->mRight : width;
    Int32 contentBottom = (dest != NULL) ? dest->mBottom : height;

    Int64 transformPtr = (transform != NULL) ? ((Matrix*)transform)->mNativeInstance : 0;

    NativeRenderPage(mHost->mNativeDocument, mNativePage, ((CBitmap*)destination)->mNativeBitmap, contentLeft,
            contentTop, contentRight, contentBottom, transformPtr, renderMode);
    return NOERROR;
}

ECode CPdfRenderer::Page::Close()
{
    FAIL_RETURN(ThrowIfClosed());
    DoClose();
    return NOERROR;
}

void CPdfRenderer::Page::DoClose()
{
    NativeClosePage(mNativePage);
    mNativePage = 0;
    mCloseGuard->Close();
    mHost->mCurrentPage = NULL;
}

ECode CPdfRenderer::Page::ThrowIfClosed()
{
    if (mNativePage == 0) {
        // throw new IllegalStateException("Already closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}


CAR_OBJECT_IMPL(CPdfRenderer);
CAR_INTERFACE_IMPL(CPdfRenderer, Object, IPdfRenderer);
CPdfRenderer::CPdfRenderer()
    : mNativeDocument(0)
    , mPageCount(0)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    CPoint::New((IPoint**)&mTempPoint);
}

CPdfRenderer::~CPdfRenderer()
{
    mCloseGuard->WarnIfOpen();
    if (mInput != NULL) {
        DoClose();
    }
}

ECode CPdfRenderer::constructor(
    /* [in] */ /*@NonNull*/ IParcelFileDescriptor* input) /*throws IOException*/
{
    if (input == NULL) {
        // throw new NullPointerException("input cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 size = 0;
    // try {
    AutoPtr<IFileDescriptor> fd;
    input->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<ILibcore> lc;
    CLibcore::AcquireSingleton((ILibcore**)&lc);
    AutoPtr<IOs> os;
    lc->GetOs((IOs**)&os);
    if (FAILED(os->Lseek(fd, 0, OsConstants::_SEEK_SET, &size))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IStructStat> ss;
    if (FAILED(os->Fstat(fd, (IStructStat**)&ss))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ss->GetSize(&size);
    // } catch (ErrnoException ee) {
    //     // throw new IllegalArgumentException("file descriptor not seekable");
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    mInput = input;
    Int32 iFd = 0;
    FAIL_RETURN(NativeCreate((mInput->GetFd(&iFd), iFd), size, &mNativeDocument));
    mPageCount = NativeGetPageCount(mNativeDocument);
    return mCloseGuard->Open(String("close"));
}

ECode CPdfRenderer::Close()
{
    FAIL_RETURN(ThrowIfClosed());
    FAIL_RETURN(ThrowIfPageOpened());
    DoClose();
    return NOERROR;
}

ECode CPdfRenderer::GetPageCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    FAIL_RETURN(ThrowIfClosed());
    *count = mPageCount;
    return NOERROR;
}

ECode CPdfRenderer::ShouldScaleForPrinting(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    FAIL_RETURN(ThrowIfClosed());
    *state = NativeScaleForPrinting(mNativeDocument);
    return NOERROR;
}

ECode CPdfRenderer::OpenPage(
    /* [in] */ Int32 index,
    /* [out] */ IPdfRendererPage** page)
{
    VALIDATE_NOT_NULL(page);
    FAIL_RETURN(ThrowIfClosed());
    FAIL_RETURN(ThrowIfPageOpened());
    FAIL_RETURN(ThrowIfPageNotInDocument(index));
    mCurrentPage = new Page();
    ((Page*)mCurrentPage.Get())->constructor(index, this);
    *page = mCurrentPage;
    REFCOUNT_ADD(*page);
    return NOERROR;
}

void CPdfRenderer::DoClose()
{
    if (mCurrentPage != NULL) {
        ((Page*)mCurrentPage.Get())->Close();
    }
    NativeClose(mNativeDocument);
    // try {
    mInput->Close();
    // } catch (IOException ioe) {
    //      ignore - best effort
    // }
    mInput = NULL;
    mCloseGuard->Close();
}

ECode CPdfRenderer::ThrowIfClosed()
{
    if (mInput == NULL) {
        // throw new IllegalStateException("Already closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CPdfRenderer::ThrowIfPageOpened()
{
    if (mCurrentPage != NULL) {
        // throw new IllegalStateException("Current page not closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CPdfRenderer::ThrowIfPageNotInDocument(
    /* [in] */ Int32 pageIndex)
{
    if (pageIndex < 0 || pageIndex >= mPageCount) {
        // throw new IllegalArgumentException("Invalid page index");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static Mutex sLock;

static int sUnmatchedInitRequestCount = 0;

static void InitializeLibraryIfNeeded()
{
    Mutex::AutoLock _l(sLock);
    if (sUnmatchedInitRequestCount == 0) {
        FPDF_InitLibrary(NULL);
    }
    sUnmatchedInitRequestCount++;
}

static void DestroyLibraryIfNeeded()
{
    Mutex::AutoLock _l(sLock);
    sUnmatchedInitRequestCount--;
    if (sUnmatchedInitRequestCount == 0) {
       FPDF_DestroyLibrary();
    }
}

static int getBlock(
    /* [in] */ void* param,
    /* [in] */ unsigned long position,
    /* [in] */ unsigned char* outBuffer,
    /* [in] */ unsigned long size)
{
    const int fd = reinterpret_cast<intptr_t>(param);
    const int readCount = pread(fd, outBuffer, size, position);
    if (readCount < 0) {
        Logger::E(String("CPdfRenderer"), String("Cannot read from file descriptor. Error:%d"), errno);
        return 0;
    }
    return 1;
}

ECode CPdfRenderer::NativeCreate(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 size,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    InitializeLibraryIfNeeded();

    FPDF_FILEACCESS loader;
    loader.m_FileLen = size;
    loader.m_Param = reinterpret_cast<void*>(intptr_t(fd));
    loader.m_GetBlock = &getBlock;

    FPDF_DOCUMENT document = FPDF_LoadCustomDocument(&loader, NULL);

    if (!document) {
        // const long error = FPDF_GetLastError();
        // jniThrowException(env, "java/io/IOException",
        //         "cannot create document. Error:" + error);
        DestroyLibraryIfNeeded();
        *result = -1;
        return E_IO_EXCEPTION;
    }

    *result = reinterpret_cast<Int64>(document);
    return NOERROR;
}

void CPdfRenderer::NativeClose(
    /* [in] */ Int64 documentPtr)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    FPDF_CloseDocument(document);
    DestroyLibraryIfNeeded();
}

Int32 CPdfRenderer::NativeGetPageCount(
    /* [in] */ Int64 documentPtr)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    return FPDF_GetPageCount(document);
}

Boolean CPdfRenderer::NativeScaleForPrinting(
    /* [in] */ Int64 documentPtr)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    return FPDF_VIEWERREF_GetPrintScaling(document);
}

static void DropContext(
    /* [in] */ void* data)
{
    delete (CRenderContext*) data;
}

static void renderPageBitmap(
    /* [in] */ FPDF_BITMAP bitmap,
    /* [in] */ FPDF_PAGE page,
    /* [in] */ int destLeft,
    /* [in] */ int destTop,
    /* [in] */ int destRight,
    /* [in] */ int destBottom,
    /* [in] */ SkMatrix* transform,
    /* [in] */ int flags)
{
    // Note: this code ignores the currently unused RENDER_NO_NATIVETEXT,
    // FPDF_RENDER_LIMITEDIMAGECACHE, FPDF_RENDER_FORCEHALFTONE, FPDF_GRAYSCALE,
    // and FPDF_ANNOT flags. To add support for that refer to FPDF_RenderPage_Retail
    // in fpdfview.cpp

    CRenderContext* pContext = FX_NEW CRenderContext;

    CPDF_Page* pPage = (CPDF_Page*) page;
    pPage->SetPrivateData((void*) 1, pContext, DropContext);

    CFX_FxgeDevice* fxgeDevice = FX_NEW CFX_FxgeDevice;
    pContext->m_pDevice = fxgeDevice;

    // Reverse the bytes (last argument TRUE) since the Android
    // format is ARGB while the renderer uses BGRA internally.
    fxgeDevice->Attach((CFX_DIBitmap*) bitmap, 0, TRUE);

    CPDF_RenderOptions* renderOptions = pContext->m_pOptions;

    if (!renderOptions) {
        renderOptions = FX_NEW CPDF_RenderOptions;
        pContext->m_pOptions = renderOptions;
    }

    if (flags & FPDF_LCD_TEXT) {
        renderOptions->m_Flags |= RENDER_CLEARTYPE;
    } else {
        renderOptions->m_Flags &= ~RENDER_CLEARTYPE;
    }

    const CPDF_OCContext::UsageType usage = (flags & FPDF_PRINTING)
            ? CPDF_OCContext::Print : CPDF_OCContext::View;

    renderOptions->m_AddFlags = flags >> 8;
    renderOptions->m_pOCContext = new CPDF_OCContext(pPage->m_pDocument, usage);

    fxgeDevice->SaveState();

    FX_RECT clip;
    clip.left = destLeft;
    clip.right = destRight;
    clip.top = destTop;
    clip.bottom = destBottom;
    fxgeDevice->SetClip_Rect(&clip);

    CPDF_RenderContext* pageContext = FX_NEW CPDF_RenderContext;
    pContext->m_pContext = pageContext;
    pageContext->Create(pPage);

    CFX_AffineMatrix matrix;
    if (!transform) {
        pPage->GetDisplayMatrix(matrix, destLeft, destTop, destRight - destLeft,
                destBottom - destTop, 0);
    } else {
        // PDF's coordinate system origin is left-bottom while
        // in graphics it is the top-left, so remap the origin.
        matrix.Set(1, 0, 0, -1, 0, pPage->GetPageHeight());

        SkScalar transformValues[6];
        transform->asAffine(transformValues);

        matrix.Concat(transformValues[SkMatrix::kAScaleX], transformValues[SkMatrix::kASkewY],
                transformValues[SkMatrix::kASkewX], transformValues[SkMatrix::kAScaleY],
                transformValues[SkMatrix::kATransX], transformValues[SkMatrix::kATransY]);
    }
    pageContext->AppendObjectList(pPage, &matrix);

    pContext->m_pRenderer = FX_NEW CPDF_ProgressiveRenderer;
    pContext->m_pRenderer->Start(pageContext, fxgeDevice, renderOptions, NULL);

    fxgeDevice->RestoreState();

    pPage->RemovePrivateData((void*) 1);

    delete pContext;
}

void CPdfRenderer::NativeRenderPage(
    /* [in] */ Int64 documentPtr,
    /* [in] */ Int64 pagePtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Int32 destLeft,
    /* [in] */ Int32 destTop,
    /* [in] */ Int32 destRight,
    /* [in] */ Int32 destBottom,
    /* [in] */ Int64 matrixPtr,
    /* [in] */ Int32 renderMode)
{
    FPDF_DOCUMENT UNUSED(document) = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    FPDF_PAGE page = reinterpret_cast<FPDF_PAGE>(pagePtr);
    SkBitmap* skBitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    SkMatrix* skMatrix = reinterpret_cast<SkMatrix*>(matrixPtr);

    skBitmap->lockPixels();

    const int stride = skBitmap->width() * 4;

    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(skBitmap->width(), skBitmap->height(),
            FPDFBitmap_BGRA, skBitmap->getPixels(), stride);

    if (!bitmap) {
        Logger::E(String("CPdfRenderer"), String("Erorr creating bitmap"));
        return;
    }

    int renderFlags = 0;
    if (renderMode == Page::RENDER_MODE_FOR_DISPLAY) {
        renderFlags |= FPDF_LCD_TEXT;
    } else if (renderMode == Page::RENDER_MODE_FOR_PRINT) {
        renderFlags |= FPDF_PRINTING;
    }

    renderPageBitmap(bitmap, page, destLeft, destTop, destRight,
            destBottom, skMatrix, renderFlags);

    skBitmap->notifyPixelsChanged();
    skBitmap->unlockPixels();
}

ECode CPdfRenderer::NativeOpenPageAndGetSize(
    /* [in] */ Int64 documentPtr,
    /* [in] */ Int32 pageIndex,
    /* [in] */ IPoint* outSize,
    /* [out] */ Int64* resVal)
{
    VALIDATE_NOT_NULL(resVal);
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);

    FPDF_PAGE page = FPDF_LoadPage(document, pageIndex);

    if (!page) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //         "cannot load page");
        *resVal = -1;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    double width = 0;
    double height = 0;

    const int result = FPDF_GetPageSizeByIndex(document, pageIndex, &width, &height);

    if (!result) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //             "cannot get page size");
        *resVal = -1;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    outSize->Set(width, height);

    *resVal = reinterpret_cast<Int64>(page);
    return NOERROR;
}

void CPdfRenderer::NativeClosePage(
    /* [in] */ Int64 pagePtr)
{
    FPDF_PAGE page = reinterpret_cast<FPDF_PAGE>(pagePtr);
    FPDF_ClosePage(page);
}

} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
