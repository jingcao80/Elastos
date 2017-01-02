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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/pdf/CPdfDocument.h"
#include "elastos/droid/graphics/NativeCanvas.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include <skia/core/SkDocument.h>
#include <skia/core/SkPicture.h>
#include <skia/core/SkPictureRecorder.h>
#include <skia/core/SkStream.h>
#include <skia/core/SkRect.h>
#include <vector>

using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {

///////////////////////////////////////////// CPdfDocument::PageInfo::Builder /////////
CAR_INTERFACE_IMPL(CPdfDocument::PageInfo::Builder, Object, IPdfDocumentPageInfoBuilder);
CPdfDocument::PageInfo::Builder::Builder(
    /* [in] */ Int32 pageWidth,
    /* [in] */ Int32 pageHeight,
    /* [in] */ Int32 pageNumber)
{
    mPageInfo = new PageInfo();

    if (pageWidth <= 0) {
        // throw new IllegalArgumentException("page width must be positive");
        assert(0);
    }
    if (pageHeight <= 0) {
        // throw new IllegalArgumentException("page width must be positive");
        assert(0);
    }
    if (pageNumber < 0) {
        // throw new IllegalArgumentException("pageNumber must be non negative");
        assert(0);
    }
    mPageInfo->mPageWidth = pageWidth;
    mPageInfo->mPageHeight = pageHeight;
    mPageInfo->mPageNumber = pageNumber;
}

ECode CPdfDocument::PageInfo::Builder::SetContentRect(
    /* [in] */ IRect* contentRect)
{
    if (contentRect != NULL) {
        Int32 left = 0, right = 0, top = 0, bottom = 0;
        contentRect->Get(&left, &top, &right, &bottom);
        if (left < 0 || top < 0
            || right > mPageInfo->mPageWidth
            || bottom > mPageInfo->mPageHeight) {
            // throw new IllegalArgumentException("contentRect does not fit the page");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mPageInfo->mContentRect = contentRect;
    return NOERROR;
}

ECode CPdfDocument::PageInfo::Builder::Create(
    /* [out] */ IPdfDocumentPageInfo** info)
{
    VALIDATE_NOT_NULL(info);
    if (mPageInfo->mContentRect == NULL) {
        CRect::New(0, 0, mPageInfo->mPageWidth, mPageInfo->mPageHeight, (IRect**)&mPageInfo->mContentRect);
    }
    *info = mPageInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

///////////////////////////////////////////// CPdfDocument::PageInfo /////////
CAR_INTERFACE_IMPL(CPdfDocument::PageInfo, Object, IPdfDocumentPageInfo);
CPdfDocument::PageInfo::PageInfo()
    : mPageWidth(0)
    , mPageHeight(0)
    , mPageNumber(0)
{
    /* do nothing */
}

ECode CPdfDocument::PageInfo::GetPageWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mPageWidth;
    return NOERROR;
}

ECode CPdfDocument::PageInfo::GetPageHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mPageHeight;
    return NOERROR;
}

ECode CPdfDocument::PageInfo::GetContentRect(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = mContentRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CPdfDocument::PageInfo::GetPageNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mPageNumber;
    return NOERROR;
}


///////////////////////////////////////////// CPdfDocument::Page /////////
CAR_INTERFACE_IMPL(CPdfDocument::Page, Object, IPdfDocumentPage);
CPdfDocument::Page::Page(
    /* [in] */ ICanvas* canvas,
    /* [in] */ PageInfo* pageInfo)
    : mPageInfo(pageInfo)
    , mCanvas(canvas)
{
}

ECode CPdfDocument::Page::GetCanvas(
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas);
    *canvas = mCanvas;
    REFCOUNT_ADD(*canvas);
    return NOERROR;
}

ECode CPdfDocument::Page::GetInfo(
    /* [out] */ IPdfDocumentPageInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mPageInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CPdfDocument::Page::IsFinished(
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);
    *finished = mCanvas == NULL;
    return NOERROR;
}

void CPdfDocument::Page::Finish()
{
    if (mCanvas != NULL) {
        mCanvas->ReleaseResources();
        mCanvas = NULL;
    }
}


///////////////////////////////////////////// CPdfDocument::PdfCanvas /////////
CPdfDocument::PdfCanvas::PdfCanvas()
{
}

ECode CPdfDocument::PdfCanvas::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

///////////////////////////////////////////// CPdfDocument /////////


struct PageRecord
{

    PageRecord(
        /* [in] */ int width,
        /* [in] */ int height,
        /* [in] */ const SkRect& contentRect)
        : mPictureRecorder(new SkPictureRecorder())
        , mPicture(NULL)
        , mWidth(width)
        , mHeight(height) {
        mContentRect = contentRect;
    }

    ~PageRecord() {
        delete mPictureRecorder;
        if (NULL != mPicture) {
            mPicture->unref();
        }
    }

    SkPictureRecorder* mPictureRecorder;
    SkPicture* mPicture;
    const int mWidth;
    const int mHeight;
    SkRect mContentRect;
};

class PdfDocument
{
public:
    PdfDocument() {
        mCurrentPage = NULL;
    }

    SkCanvas* startPage(
        /* [in] */ int width,
        /* [in] */ int height,
        /* [in] */ int contentLeft,
        /* [in] */ int contentTop,
        /* [in] */ int contentRight,
        /* [in] */ int contentBottom)
    {
        assert(mCurrentPage == NULL);

        SkRect contentRect = SkRect::MakeLTRB(
                contentLeft, contentTop, contentRight, contentBottom);
        PageRecord* page = new PageRecord(width, height, contentRect);
        mPages.push_back(page);
        mCurrentPage = page;

        SkCanvas* canvas = page->mPictureRecorder->beginRecording(
                contentRect.width(), contentRect.height(), NULL, 0);

        // We pass this canvas to Java where it is used to construct
        // a Java Canvas object which dereferences the pointer when it
        // is destroyed, so we have to bump up the reference count.
        canvas->ref();

        return canvas;
    }

    void finishPage() {
        assert(mCurrentPage != NULL);
        assert(mCurrentPage->mPictureRecorder != NULL);
        assert(mCurrentPage->mPicture == NULL);
        mCurrentPage->mPicture = mCurrentPage->mPictureRecorder->endRecording();
        delete mCurrentPage->mPictureRecorder;
        mCurrentPage->mPictureRecorder = NULL;
        mCurrentPage = NULL;
    }

    void write(SkWStream* stream) {
        SkDocument* document = SkDocument::CreatePDF(stream);
        for (unsigned i = 0; i < mPages.size(); i++) {
            PageRecord* page =  mPages[i];

            SkCanvas* canvas = document->beginPage(page->mWidth, page->mHeight,
                    &(page->mContentRect));

            canvas->clipRect(page->mContentRect);
            canvas->translate(page->mContentRect.left(), page->mContentRect.top());
            canvas->drawPicture(page->mPicture);

            document->endPage();
        }
        document->close();
    }

    void close() {
        assert(NULL == mCurrentPage);
        for (unsigned i = 0; i < mPages.size(); i++) {
            delete mPages[i];
        }
    }

private:
    ~PdfDocument() {
        close();
    }

    std::vector<PageRecord*> mPages;
    PageRecord* mCurrentPage;
};


CAR_OBJECT_IMPL(CPdfDocument);

CAR_INTERFACE_IMPL(CPdfDocument, Object, IPdfDocument);

CPdfDocument::CPdfDocument()
    : mNativeDocument(0)
{
    mChunk = ArrayOf<Byte>::Alloc(4096);
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    CArrayList::New((IList**)&mPages);
}

CPdfDocument::~CPdfDocument()
{
    mCloseGuard->WarnIfOpen();
    Dispose();
}

ECode CPdfDocument::constructor()
{
    mNativeDocument = NativeCreateDocument();
    mCloseGuard->Open(String("CPdfDocument::Close"));
    return NOERROR;
}

ECode CPdfDocument::StartPage(
    /* [in] */ IPdfDocumentPageInfo* pageInfo,
    /* [out] */ IPdfDocumentPage** page)
{
    VALIDATE_NOT_NULL(page);
    FAIL_RETURN(ThrowIfClosed());
    FAIL_RETURN(ThrowIfCurrentPageNotFinished());
    if (pageInfo == NULL) {
        // throw new IllegalArgumentException("page cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    PageInfo* info = (PageInfo*)pageInfo;
    Int32 left = 0, right = 0, top = 0, bottom = 0;
    info->mContentRect->Get(&left, &top, &right, &bottom);
    AutoPtr<PdfCanvas> pc = new PdfCanvas();
    pc->constructor(NativeStartPage(mNativeDocument, info->mPageWidth, info->mPageHeight,
        left, top, right, bottom));
    mCurrentPage = new Page(pc.Get(), info);
    *page = mCurrentPage;
    REFCOUNT_ADD(*page);
    return NOERROR;
}

ECode CPdfDocument::FinishPage(
    /* [in] */ IPdfDocumentPage* page)
{
    FAIL_RETURN(ThrowIfClosed());
    if (page == NULL) {
        // throw new IllegalArgumentException("page cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (page != mCurrentPage) {
        // throw new IllegalStateException("invalid page");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean value = FALSE;
    if (page->IsFinished(&value), value) {
        // throw new IllegalStateException("page already finished");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IPdfDocumentPageInfo> info;
    page->GetInfo((IPdfDocumentPageInfo**)&info);
    mPages->Add(info);
    mCurrentPage = NULL;
    NativeFinishPage(mNativeDocument);
    ((Page*)page)->Finish();
    return NOERROR;
}

ECode CPdfDocument::WriteTo(
    /* [in] */ IOutputStream* out) /*throws IOException*/
{
    FAIL_RETURN(ThrowIfClosed());
    FAIL_RETURN(ThrowIfCurrentPageNotFinished());
    if (out == NULL) {
        // throw new IllegalArgumentException("out cannot be NULL!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeWriteTo(mNativeDocument, out, mChunk);
    return NOERROR;
}

ECode CPdfDocument::GetPages(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(mPages, list);
}

ECode CPdfDocument::Close()
{
    FAIL_RETURN(ThrowIfCurrentPageNotFinished());
    return Dispose();
}

ECode CPdfDocument::Dispose()
{
    if (mNativeDocument != 0) {
        NativeClose(mNativeDocument);
        mCloseGuard->Close();
        mNativeDocument = 0;
    }
    return NOERROR;
}

ECode CPdfDocument::ThrowIfClosed()
{
    if (mNativeDocument == 0) {
        // throw new IllegalStateException("document is closed!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CPdfDocument::ThrowIfCurrentPageNotFinished()
{
    if (mCurrentPage != NULL) {
        // throw new IllegalStateException("Current page not finished!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int64 CPdfDocument::NativeCreateDocument()
{
    return reinterpret_cast<Int64>(new PdfDocument());
}

void CPdfDocument::NativeClose(
    /* [in] */ Int64 documentPtr)
{
    PdfDocument* document = reinterpret_cast<PdfDocument*>(documentPtr);
    document->close();
}

void CPdfDocument::NativeFinishPage(
    /* [in] */ Int64 documentPtr)
{
    PdfDocument* document = reinterpret_cast<PdfDocument*>(documentPtr);
    document->finishPage();
}

void CPdfDocument::NativeWriteTo(
    /* [in] */ Int64 documentPtr,
    /* [in] */ IOutputStream* out,
    /* [in] */ ArrayOf<Byte>* chunk)
{
    PdfDocument* document = reinterpret_cast<PdfDocument*>(documentPtr);
    SkWStream* skWStream = CreateOutputStreamAdaptor(out, chunk);
    document->write(skWStream);
    delete skWStream;
}

Int64 CPdfDocument::NativeStartPage(
    /* [in] */ Int64 documentPtr,
    /* [in] */ Int32 pageWidth,
    /* [in] */ Int32 pageHeight,
    /* [in] */ Int32 contentLeft,
    /* [in] */ Int32 contentTop,
    /* [in] */ Int32 contentRight,
    /* [in] */ Int32 contentBottom)
{
    PdfDocument* document = reinterpret_cast<PdfDocument*>(documentPtr);
    SkCanvas* canvas = document->startPage(pageWidth, pageHeight,
            contentLeft, contentTop, contentRight, contentBottom);
    return reinterpret_cast<Int64>(NativeCanvas::create_canvas(canvas));
}

} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
