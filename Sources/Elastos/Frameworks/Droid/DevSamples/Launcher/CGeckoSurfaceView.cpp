
#include <ext/frameworkdef.h>
#include "GeckoApp.h"
#include "GeckoAppShell.h"
#include "CGeckoSurfaceView.h"
#include "GeckoErrors.h"
#include "GeckoEvent.h"
//#include "ByteBuffer.h"
#include <elastos/utility/logging/Logger.h>
#include <stdio.h>
#include <pthread.h>
#include "elastos/droid/os/Build.h"

#include <unistd.h>

using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::CEditableFactory;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CSurfaceView;
using Elastos::Droid::View::ITouchDelegate;
using Elastos::Droid::View::EIID_ITouchDelegate;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

//==============================================================================
//      CGeckoSurfaceView::MySurfaceHoderCallback
//==============================================================================
CGeckoSurfaceView::MySurfaceHoderCallback::MySurfaceHoderCallback(
    /* [in]  */ CGeckoSurfaceView* host)
    : mHost(host)
{
}

PInterface CGeckoSurfaceView::MySurfaceHoderCallback::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ISurfaceHolderCallback) {
        return this;
    }

    return NULL;
}

UInt32 CGeckoSurfaceView::MySurfaceHoderCallback::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CGeckoSurfaceView::MySurfaceHoderCallback::Release()
{
    return ElRefBase::Release();
}

ECode CGeckoSurfaceView::MySurfaceHoderCallback::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }
    if (pObject == (IInterface*)this) {
        *pIID = EIID_ISurfaceHolderCallback;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    PRINT_FILE_LINE_EX("CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceCreated");
    AutoPtr<GeckoEvent> geckoEvent = new GeckoEvent(GeckoEvent::SURFACE_CREATED);
    assert(geckoEvent != NULL);
    GeckoAppShell::SendEventToGecko(geckoEvent);
    // if (sShowingSplashScreen) {
    //     mHost->DrawSplashScreen();
    // }
    return NOERROR;
}

ECode CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    PRINT_FILE_LINE_EX("CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceChanged");

    // On pre-Honeycomb, force exactly one frame of the previous size
    // to render because the surface change is only seen by GLES after we
    // have swapped the back buffer (i.e. the buffer size only changes
    // after the next swap buffer). We need to make sure Gecko's view
    // resizes when Android's buffer resizes.
    // In Honeycomb, the buffer size changes immediately, so rendering a
    // frame of the previous size is unnecessary (and wrong).
    if (mHost->mDrawMode == DRAW_GLES_2 &&
        (Build::VERSION::SDK_INT < Build::VERSION_CODES::HONEYCOMB)) {
        // When we get a surfaceChange event, we have 0 to n paint events
        // waiting in the Gecko event queue. We will make the first
        // succeed and the abort the others.
        mHost->mDrawSingleFrame = TRUE;
        if (!mHost->mInDrawing) {
            // Queue at least one paint event in case none are queued.
            GeckoAppShell::ScheduleRedraw();
        }
        GeckoAppShell::GeckoEventSync();
        mHost->mDrawSingleFrame = FALSE;
        mHost->mAbortDraw = FALSE;
    }

    // if (sShowingSplashScreen) {
    //     mHost->DrawSplashScreen(holder, width, height);
    // }

    ECode ec = NOERROR;
    pthread_mutex_lock(&mHost->mSurfaceLock);

    if (mHost->mInDrawing) {
        PRINT_FILE_LINE_EX("CGeckoSurfaceView surfaceChanged while mInDrawing is true!");
    }

    Boolean invalidSize = FALSE;
    if (width == 0 || height == 0) {
        mHost->mSoftwareBitmap = NULL;
        mHost->mSoftwareBuffer = NULL;
        mHost->mSoftwareBufferCopy = NULL;
        invalidSize = TRUE;
    }

    Boolean result = GeckoApp::CheckLaunchState(GeckoApp::LaunchState_GeckoRunning);
    Boolean doSyncDraw = FALSE;// mHost->mDrawMode == DRAW_2D && !invalidSize && result;
    mHost->mSyncDraw = doSyncDraw;

    mHost->mFormat = format;
    mHost->mWidth = width;
    mHost->mHeight = height;
    mHost->mSurfaceValid = TRUE;

    PRINT_FILE_LINE_EX("surfaceChanged: fmt: %d dim: %d,%d", format, width, height);

//        AutoPtr<IDisplayMetrics> metrics;
//        AutoPtr<IDisplay> display;
//        ASSERT_SUCCEEDED(GeckoApp::sAppContext->GetWindowManager()->GetDefaultDisplay((IDisplay**)&display));
//        ASSERT_SUCCEEDED(display->GetMetrics((IDisplayMetrics**)&metrics));
    Int32 widthPixels = 1280/*metrics->mWidthPixels*/;
    Int32 heightPixels = 695/*metrics->mHeightPixels*/;

    AutoPtr<GeckoEvent> geckoEvent = new GeckoEvent(
            GeckoEvent::SIZE_CHANGED, width, height, widthPixels, heightPixels);
    assert(geckoEvent != NULL);
    GeckoAppShell::SendEventToGecko(geckoEvent);

    pthread_mutex_unlock(&mHost->mSurfaceLock);

    if (doSyncDraw) {
        GeckoAppShell::ScheduleRedraw();

        AutoPtr<IInterface> syncDrawObject;
        //ec = mSyncDraws->Take(&syncDrawObject);
        if (FAILED(ec)) {
            PRINT_FILE_LINE_EX("Threw exception while getting sync draw bitmap/buffer: %08x", ec);
        }

        if (syncDrawObject != NULL) {
            IBitmap* bitmap = IBitmap::Probe(syncDrawObject);
            if (bitmap) {
                ec =  mHost->Draw(holder, bitmap);
            }
            else {
                ec = mHost->Draw(holder, IBuffer::Probe(syncDrawObject));
            }
        }
        else {
            PRINT_FILE_LINE_EX("Synchronised draw object is null");
        }
    }
    else {
        // Make sure a frame is drawn before we return
        // otherwise we see artifacts or a black screen
        GeckoAppShell::ScheduleRedraw();
        GeckoAppShell::GeckoEventSync();
    }

    return ec;
}

ECode CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* pHolder)
{
    PRINT_ENTER_LEAVE("CGeckoSurfaceView::MySurfaceHoderCallback::SurfaceDestroyed");

    mHost->mSurfaceValid = FALSE;
    mHost->mSoftwareBuffer = NULL;
    mHost->mSoftwareBufferCopy = NULL;
    mHost->mSoftwareBitmap = NULL;
    GeckoEvent* event = new GeckoEvent(GeckoEvent::SURFACE_DESTROYED);
    assert(event != NULL);
    if (mHost->mDrawMode == DRAW_GLES_2) {
        // Ensure GL cleanup occurs before we return.
        GeckoAppShell::SendEventToGeckoSync(event);
    } else {
        GeckoAppShell::SendEventToGecko(event);
    }
    return NOERROR;
}

//==============================================================================
//      CGeckoSurfaceView::MyTouchDelegate
//==============================================================================

CGeckoSurfaceView::MyTouchDelegate::MyTouchDelegate(
    /* [in] */ CGeckoSurfaceView* host)
    : mHost(host)
{}

PInterface CGeckoSurfaceView::MyTouchDelegate::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ITouchDelegate) {
        return this;
    }

    return NULL;
}

UInt32 CGeckoSurfaceView::MyTouchDelegate::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CGeckoSurfaceView::MyTouchDelegate::Release()
{
    return ElRefBase::Release();
}

ECode CGeckoSurfaceView::MyTouchDelegate::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }
    if (pObject == (IInterface*)this) {
        *pIID = EIID_ITouchDelegate;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CGeckoSurfaceView::MyTouchDelegate::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mHost->OnTouchEvent(event, result);
}

//==============================================================================
//      CGeckoSurfaceView
//==============================================================================

Boolean CGeckoSurfaceView::sShowingSplashScreen = TRUE;
String CGeckoSurfaceView::sSplashURL("");

ECode CGeckoSurfaceView::constructor(
    /* [in] */ IContext* context)
{
    ASSERT_SUCCEEDED(CSurfaceView::New(context, (ISurfaceView**)&mSurfaceView));
    AutoPtr<MyTouchDelegate> delegate = new MyTouchDelegate(this);
    mSurfaceView->SetTouchDelegate(delegate);
    mSurfaceView->SetCreateInputConnectionDelegate(this);
    mSurfaceView->SetKeyEventCallbackDelegate(this);

    AutoPtr<ISurfaceHolder> holder;
    ASSERT_SUCCEEDED(GetHolder((ISurfaceHolder**)&holder));
    AutoPtr<ISurfaceHolderCallback> callback = new MySurfaceHoderCallback(this);
    holder->AddCallback(callback);

    ASSERT_SUCCEEDED(CGeckoInputConnection::New(this, (IGeckoInputConnection**)&mInputConnection));
    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);

    AutoPtr<IDisplayMetrics> metrics;
    ASSERT_SUCCEEDED(CDisplayMetrics::New((IDisplayMetrics**)&metrics));
    AutoPtr<IDisplay> display;
    ASSERT_SUCCEEDED(GeckoApp::sAppContext->GetWindowManager()->GetDefaultDisplay((IDisplay**)&display));

    // display->GetMetrics(metrics);
    mWidth = 1280;//pIMetrics->mWidthPixels;
    mHeight = 695;//pIMetrics->mHeightPixels;

    ASSERT_SUCCEEDED(CEditableFactory::AcquireSingleton((IEditableFactory**)&mEditableFactory));

    AutoPtr<ICharSequence> str;
    CStringWrapper::New(String(""), (ICharSequence**)&str);
    InitEditable(str);
    return NOERROR;
}

CGeckoSurfaceView::CGeckoSurfaceView()
    : mInputConnection(NULL)
    , mIMEState(IME_STATE_DISABLED)
    , mIMETypeHint("")
    , mIMEActionHint("")
    , mIMELandscapeFS(FALSE)
    , mEditable(NULL)
    , mDrawSingleFrame(FALSE)
    , mAbortDraw(FALSE)
    , mSurfaceValid(FALSE)
    , mInDrawing(FALSE)
    , mSyncDraw(FALSE)
    , mDrawMode(0)
    , mFormat(0)
    , mWidth(0)
    , mHeight(0)
    , mBufferWidth(0)
    , mBufferHeight(0)
    , mKeyListener(NULL)
    , mEditableFactory(NULL)
    , mSoftwareBitmap(NULL)
    , mSoftwareBuffer(NULL)
    , mSoftwareBufferCopy(NULL)
    , mSurfaceView(NULL)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mSurfaceLock, &attr);
    pthread_mutexattr_destroy(&attr);
}

CGeckoSurfaceView::~CGeckoSurfaceView()
{
    pthread_mutex_destroy(&mSurfaceLock);
}

const InterfaceID EIID_View =
        { 0x47439045, 0x473d, 0x4d3c, { 0x87, 0x6b, 0x42, 0x42, 0x0, 0xc8, 0x9e, 0x19 } };

PInterface CGeckoSurfaceView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return mSurfaceView->Probe(riid);
    }
    return _CGeckoSurfaceView::Probe(riid);
}

ECode CGeckoSurfaceView::DrawSplashScreen()
{
    AutoPtr<ISurfaceHolder> holder;
    ASSERT_SUCCEEDED(GetHolder((ISurfaceHolder**)&holder));

    return DrawSplashScreen(holder, mWidth, mHeight);
}

ECode CGeckoSurfaceView::DrawSplashScreen(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // No splash screen for Honeycomb or greater
    if (Build::VERSION::SDK_INT >= 11) {
       return NOERROR;
    }

    AutoPtr<ICanvas> canvas;
    ASSERT_SUCCEEDED(holder->LockCanvas((ICanvas**)&canvas));
    if (canvas == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> resources;
    ASSERT_SUCCEEDED(GetResources((IResources**)&resources));

    AutoPtr<IFile> watchDir;
    ECode ec = CFile::New(GeckoApp::sGREDir, String("components"), (IFile**)&watchDir);
    if (FAILED(ec)) return ec;
    Boolean exist = FALSE;
    watchDir->Exists(&exist);
    if (TRUE){//!exist) {
        // Just show the simple splash screen for "new profile" startup
        Int32 color;
        ASSERT_SUCCEEDED(resources->GetColor(0x7f040000, &color)); //R.color.splash_background
        ASSERT_SUCCEEDED(canvas->DrawColor(color));
        AutoPtr<IDrawable> drawable;
        ASSERT_SUCCEEDED(resources->GetDrawable(0x7f020005, (IDrawable**)&drawable));//R.drawable.splash
        Int32 x, y, w, h;
        drawable->GetIntrinsicWidth(&w);
        drawable->GetIntrinsicHeight(&h);
        x = (width - w)/2;
        y = (height - h)/2 - 16;
        drawable->SetBounds(x, y, x + w, y + h);
        drawable->Draw(canvas);

        AutoPtr<IPaint> paint;
        ASSERT_SUCCEEDED(CPaint::New((IPaint**)&paint));
        paint->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
        paint->SetTextSize(32);
        paint->SetAntiAlias(TRUE);
        ASSERT_SUCCEEDED(resources->GetColor(0x7f040001, &color));//R.color.splash_msgfont TODO:change the number
        paint->SetColor(color);
        String firstRun;
        resources->GetString(0x7f050002, &firstRun); //R.string.splash_firstrun
        ASSERT_SUCCEEDED(canvas->DrawText(firstRun, width/2, y + h + 16, paint));
    }
    else {
        // Show the static UI for normal startup
        AutoPtr<IDisplayMetrics> metrics;
        ec = CDisplayMetrics::New((IDisplayMetrics**)&metrics);
        if (FAILED(ec)) return ec;
        AutoPtr<IWindowManager> wm;
        ec = GeckoApp::sAppContext->GetWindowManager((IWindowManager**)&wm);
        if (FAILED(ec)) return ec;
        AutoPtr<IDisplay> display;
        ec = wm->GetDefaultDisplay((IDisplay**)&display);
        if (FAILED(ec)) return ec;
        ec = display->GetMetrics(metrics);
        // Default to DENSITY_HIGH sizes
        Int32 toolbarHeight = 80;
        Int32 faviconOffset = 25;
        Float urlHeight = 24;
        Int32 urlOffsetX = 80;
        Int32 urlOffsetY = 48;
        Int32 densityDpi;
        //ec = metrics->GetDensityDpi(&densityDpi);
        //if (FAILED(ec)) return ec;

        if (densityDpi == IDisplayMetrics::DENSITY_MEDIUM) {
            toolbarHeight = 53;
            faviconOffset = 10;
            urlHeight = 16;
            urlOffsetX = 53;
            urlOffsetY = 32;
        }

        Int32 color;
        //ASSERT_SUCCEEDED(resources->GetColor(R.color.splash_content, &color));
        ASSERT_SUCCEEDED(canvas->DrawColor(color));

        AutoPtr<IDrawable> toolbar;
        //ec = resources->GetDrawable(Build.VERSION.SDK_INT > 8 ?
        //                           R.drawable.splash_v9 : R.drawable.splash_v8,
        //                           (IDrawable**)&toolbar);
        if (FAILED(ec)) return ec;
        toolbar->SetBounds(0, 0, width, toolbarHeight);
        toolbar->Draw(canvas);

        // XUL/CSS always uses 32px width and height for favicon
        AutoPtr<IDrawable> favicon;
        //ec = resources->GetDrawable(R.drawable.favicon32, (IDrawable**)&favicon);
        //if (FAILED(ec)) return ec;
        favicon->SetBounds(faviconOffset, faviconOffset, 32 + faviconOffset, 32 + faviconOffset);
        favicon->Draw(canvas);
        if (!CGeckoSurfaceView::sSplashURL.IsEmpty()) {
            AutoPtr<ITextPaint> textPaint;
            ec = CTextPaint::New((ITextPaint**)&textPaint);
            if (FAILED(ec)) return ec;
            textPaint->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_LEFT);
            textPaint->SetTextSize(urlHeight);
            textPaint->SetAntiAlias(TRUE);
            //ASSERT_SUCCEEDED(resources->GetColor(R.color.splash_urlfont, &color));
            textPaint->SetColor(color);

            AutoPtr<ICharSequence> text;
            ec = CStringWrapper::New(CGeckoSurfaceView::sSplashURL, (ICharSequence**)&text);
            if (FAILED(ec)) return ec;
            //AutoPtr<ICharSequence> charS = TextUtils::Ellipsize(text,
            //             textPaint, width - urlOffsetX * 2, TextUtilsTruncateAt_END);
            //assert(charS);
            String url;
            //charS->ToString(&url);
            ec = canvas->DrawText(url, urlOffsetX, urlOffsetY, textPaint);
            if (FAILED(ec)) return ec;
        }
    }
    ASSERT_SUCCEEDED(holder->UnlockCanvasAndPost(canvas));
    return NOERROR;
}

ECode CGeckoSurfaceView::Draw(
    /* [in] */ ISurfaceHolder* pHolder,
    /* [in] */ IBuffer* pBuffer)
{
    PRINT_ENTER_LEAVE("CGeckoSurfaceView::Draw IBuffer");

    if (!pHolder || !pBuffer) {
        return E_INVALID_ARGUMENT;
    }
    Int32 capacity;
    ECode ec = pBuffer->GetCapacity(&capacity);
    if (FAILED(ec)) return ec;
    if (capacity != (mWidth * mHeight * 2)) {
        return E_INVALID_ARGUMENT;
    }

    AutoLock lock(mSoftwareBufferLock);
    if (pBuffer != mSoftwareBuffer || !mSoftwareBufferCopy)
        return E_INVALID_ARGUMENT;

    AutoPtr<ICanvas> pICanvas;
    ec = pHolder->LockCanvas((ICanvas**)&pICanvas);
    if (FAILED(ec)) return ec;
    ec = mSoftwareBufferCopy->CopyPixelsFromBuffer(pBuffer);
    if (FAILED(ec)) return ec;

    ec = pICanvas->DrawBitmap(mSoftwareBufferCopy, 0, 0, NULL);
    if (FAILED(ec)) return ec;

    ec = pHolder->UnlockCanvasAndPost(pICanvas);
    return ec;
}

ECode CGeckoSurfaceView::Draw(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ IBitmap* bitmap)
{
    if (!holder || !bitmap) {
        return NOERROR;
    }
    Int32 width, height;
    ASSERT_SUCCEEDED(bitmap->GetWidth(&width));
    ASSERT_SUCCEEDED(bitmap->GetHeight(&height));

    if (width != mWidth || height != mHeight) {
        return NOERROR;
    }

    AutoLock lock(mSoftwareBitmapLock);

    if (bitmap != mSoftwareBitmap) {
        return NOERROR;
    }
    AutoPtr<ICanvas> canvas;
    ASSERT_SUCCEEDED(holder->LockCanvas((ICanvas**)&canvas));
    if (canvas == NULL) return NOERROR;
    ASSERT_SUCCEEDED(canvas->DrawBitmap(bitmap, 0, 0, NULL));
    ASSERT_SUCCEEDED(holder->UnlockCanvasAndPost(canvas));
    return NOERROR;
}

ECode CGeckoSurfaceView::GetSoftwareDrawBitmap(
    /* [out] */IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    Int32 width = mWidth;
    Int32 height = mHeight;
    if (mSoftwareBitmap) {
        ASSERT_SUCCEEDED(mSoftwareBitmap->GetHeight(&height));
        ASSERT_SUCCEEDED(mSoftwareBitmap->GetWidth(&width));
    }
    if (!mSoftwareBitmap || height != mHeight || width != mWidth) {
        mSoftwareBitmap = NULL;
        AutoPtr<IBitmapFactory> factory;
        ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory));

        ASSERT_SUCCEEDED(factory->CreateBitmap(
                mWidth, mHeight, BitmapConfig_RGB_565, (IBitmap**)&mSoftwareBitmap));
    }

    mDrawMode = DRAW_2D;
    *bitmap = mSoftwareBitmap;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CGeckoSurfaceView::GetSoftwareDrawBuffer(
    /* [out] */ IBuffer** ppBuffer)
{
    VALIDATE_NOT_NULL(ppBuffer);

    // We store pixels in 565 format, so two bytes per pixel (explaining
    // the * 2 in the following check/allocation)
    Int32 capacity = mWidth * mHeight * 2;
    ECode ec;
    if (mSoftwareBuffer) {
        mSoftwareBuffer->GetCapacity(&capacity);
    }
    if (!mSoftwareBuffer || capacity != (mWidth * mHeight * 2)) {
        if (mSoftwareBuffer) {
            mSoftwareBuffer->Release();
            mSoftwareBuffer = NULL;
        }
        // ec = ByteBuffer::AllocateDirect(mWidth * mHeight * 2, &mSoftwareBuffer);
        // if (FAILED(ec)) return ec;
    }

    Int32 width = mWidth;
    Int32 height = mHeight;
    if (mSoftwareBufferCopy) {
        mSoftwareBufferCopy->GetHeight(&height);
        mSoftwareBufferCopy->GetWidth(&width);
    }
    if (!mSoftwareBufferCopy || height != mHeight || width != mWidth) {
        mSoftwareBufferCopy = NULL;
        AutoPtr<IBitmapFactory> pIBitmapFactory;
        ec = CBitmapFactory::AcquireSingleton((IBitmapFactory**)&pIBitmapFactory);
        if (FAILED(ec)) return ec;

        ec = pIBitmapFactory->CreateBitmap(mWidth, mHeight,
                         Elastos::Droid::Graphics::BitmapConfig_RGB_565, (IBitmap**)&mSoftwareBufferCopy);
        if (FAILED(ec)) return ec;
    }

    mDrawMode = DRAW_2D;
    *ppBuffer = mSoftwareBuffer;
    REFCOUNT_ADD(*ppBuffer);
    return NOERROR;
}

ECode CGeckoSurfaceView::GetSurface(
    /* [out] */ ISurface** ppSurface)
{
    PRINT_ENTER_LEAVE("CGeckoSurfaceView::GetSurface");

    if (!ppSurface) return E_INVALID_ARGUMENT;

    AutoPtr<ISurfaceHolder> holder;
    ECode ec = GetHolder((ISurfaceHolder**)&holder);
    if (FAILED(ec)) return ec;

    return holder->GetSurface(ppSurface);
}

ECode CGeckoSurfaceView::BeginDrawing(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mInDrawing) {
        PRINT_FILE_LINE_EX("GeckoAppJava Recursive beginDrawing call!");
        *result = DRAW_ERROR;
        return NOERROR;
    }

    // Once we drawn our first frame after resize we can ignore
    // the other draw events until we handle the resize events.
    if (mAbortDraw) {
        return DRAW_DISABLED;
    }

    /* Grab the lock, which we'll hold while we're drawing.
     * It gets released in endDrawing(), and is also used in surfaceChanged
     * to make sure that we don't change our surface details while
     * we're in the middle of drawing (and especially in the middle of
     * executing beginDrawing/endDrawing).
     *
     * We might not need to hold this lock in between
     * beginDrawing/endDrawing, and might just be able to make
     * surfaceChanged, beginDrawing, and endDrawing synchronized,
     * but this way is safer for now.
     */
    pthread_mutex_lock(&mSurfaceLock);

    if (!mSurfaceValid) {
        PRINT_FILE_LINE_EX("GeckoAppJava Surface not valid");
        pthread_mutex_unlock(&mSurfaceLock);
        *result = DRAW_ERROR;
        return NOERROR;
    }

    mInDrawing = TRUE;
    mDrawMode = DRAW_GLES_2;
    *result = DRAW_GLES_2;
    return NOERROR;
}

ECode CGeckoSurfaceView::EndDrawing()
{
    if (!mInDrawing) {
        PRINT_FILE_LINE_EX("GeckoAppJava endDrawing without beginDrawing!\n");
        return NOERROR;
    }

    if (mDrawSingleFrame)
         mAbortDraw = TRUE;

    if (!mSurfaceValid) {
        PRINT_FILE_LINE_EX("GeckoAppJava endDrawing with false mSurfaceValid\n");
    }

    mInDrawing = FALSE;

//    if (!mSurfaceLock.isHeldByCurrentThread())
//        Log.e("GeckoAppJava", "endDrawing while mSurfaceLock not held by current thread!");

    pthread_mutex_unlock(&mSurfaceLock);
    return NOERROR;
}

/* How this works:
 * Whenever we want to draw, we want to be sure that we do not lock
 * the canvas unless we're sure we can draw. Locking the canvas clears
 * the canvas to black in most cases, causing a black flash.
 * At the same time, the surface can resize/disappear at any moment
 * unless the canvas is locked.
 * Draws originate from a different thread so the surface could change
 * at any moment while we try to draw until we lock the canvas.
 *
 * Also, never try to lock the canvas while holding the surface lock
 * unless you're in SurfaceChanged, in which case the canvas was already
 * locked. Surface lock -> Canvas lock will lead to AB-BA deadlocks.
 */
ECode CGeckoSurfaceView::Draw2D(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    VALIDATE_NOT_NULL(bitmap);

    // mSurfaceLock ensures that we get mSyncDraw/mSoftwareBitmap/etc.
    // set correctly before determining whether we should do a sync draw
    pthread_mutex_lock(&mSurfaceLock);

    ECode ec = NOERROR;
    if (mSyncDraw) {
        if (bitmap != mSoftwareBitmap.Get() || width != mWidth || height != mHeight) {
            pthread_mutex_unlock(&mSurfaceLock);
            return E_INVALID_ARGUMENT;
        }

        mSyncDraw = FALSE;

//        ec = mSyncDraws.put(pBitmap);
//        if (FAILED(ec)) {
//            PRINT_FILE_LINE_EX("GeckoAppJava getting sync draws queue error:%x\n", ec);
//        }
        pthread_mutex_unlock(&mSurfaceLock);
        return ec;
    }

    pthread_mutex_unlock(&mSurfaceLock);

    AutoPtr<ISurfaceHolder> holder;
    ASSERT_SUCCEEDED(GetHolder((ISurfaceHolder**)&holder));
    ASSERT_SUCCEEDED(Draw(holder, bitmap));
    return NOERROR;
}

ECode CGeckoSurfaceView::Draw2D(
    /* [in] */ IBuffer* pBuffer,
    /* [in] */ Int32 stride)
{
    assert(0);

    VALIDATE_NOT_NULL(pBuffer);

    pthread_mutex_lock(&mSurfaceLock);

    ECode ec = NOERROR;
    if (mSyncDraw) {
        if (pBuffer != mSoftwareBuffer || stride != (mWidth * 2)) {
            pthread_mutex_unlock(&mSurfaceLock);
            return E_INVALID_ARGUMENT;
        }

        mSyncDraw = FALSE;

//        ec = mSyncDraws.put(pBuffer);
//        if (FAILED(ec)) {
//            PRINT_FILE_LINE_EX("GeckoAppJava getting sync bitmaps queue error:%x\n", ec);
//        }

        pthread_mutex_unlock(&mSurfaceLock);
        return ec;
    }

    pthread_mutex_unlock(&mSurfaceLock);

    AutoPtr<ISurfaceHolder> pISurfaceHolder;
    ASSERT_SUCCEEDED(GetHolder((ISurfaceHolder**)&pISurfaceHolder));
    ASSERT_SUCCEEDED(Draw(pISurfaceHolder, pBuffer));
    return ec;
}

ECode CGeckoSurfaceView::OnCheckIsTextEditor(
    /* [out] */ Boolean * pResult)
{
    *pResult = FALSE;
    return NOERROR;
}

ECode CGeckoSurfaceView::SetEditable(
    /* [in] */ ICharSequence* contents)
{
    ASSERT_SUCCEEDED(mEditable->RemoveSpan(mInputConnection));

    Int32 length = 0;
    ASSERT_SUCCEEDED(mEditable->GetLength(&length));

    AutoPtr<IEditable> editable;
    ASSERT_SUCCEEDED(mEditable->Replace(0, length, contents, (IEditable**)&editable));

    ASSERT_SUCCEEDED(contents->GetLength(&length));
    ASSERT_SUCCEEDED(mEditable->SetSpan(mInputConnection,
                 0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE));

    //ec = Selection::SetSelection(mEditable, length);
    return NOERROR;
}

ECode CGeckoSurfaceView::InitEditable(
    /* [in] */ ICharSequence* contents)
{
    ASSERT_SUCCEEDED(mEditableFactory->NewEditable(contents, (IEditable**)&mEditable));

    Int32 length = 0;
    contents->GetLength(&length);
    ASSERT_SUCCEEDED(mEditable->SetSpan(mInputConnection,
                        0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE));
    //ec = Selection::SetSelection(mEditable, length);
    return NOERROR;
}

ECode CGeckoSurfaceView::GetVerticalFadingEdgeLength(
    /* [out] */ Int32 * pLength)
{
    return mSurfaceView->GetVerticalFadingEdgeLength(pLength);
}

ECode CGeckoSurfaceView::SetFadingEdgeLength(
    /* [in] */ Int32 length)
{
    return mSurfaceView->SetFadingEdgeLength(length);
}

ECode CGeckoSurfaceView::GetHorizontalFadingEdgeLength(
    /* [out] */ Int32 * pLength)
{
    return mSurfaceView->GetHorizontalFadingEdgeLength(pLength);
}

ECode CGeckoSurfaceView::GetVerticalScrollbarWidth(
    /* [out] */ Int32 * pWidth)
{
    return mSurfaceView->GetVerticalScrollbarWidth(pWidth);
}

ECode CGeckoSurfaceView::SetVerticalScrollbarPosition(
    /* [in] */ Int32 position)
{
    return mSurfaceView->SetVerticalScrollbarPosition(position);
}

ECode CGeckoSurfaceView::GetVerticalScrollbarPosition(
    /* [out] */ Int32* position)
{
    return mSurfaceView->GetVerticalScrollbarPosition(position);
}

ECode CGeckoSurfaceView::GetHorizontalScrollbarHeight(
    /* [out] */ Int32 * pHeight)
{
    return mSurfaceView->GetHorizontalScrollbarHeight(pHeight);
}

ECode CGeckoSurfaceView::SetOnFocusChangeListener(
    /* [in] */ IViewOnFocusChangeListener * pL)
{
    return mSurfaceView->SetOnFocusChangeListener(pL);
}

ECode CGeckoSurfaceView::AddOnLayoutChangeListener(
    /* [in] */ IViewOnLayoutChangeListener* listener)
{
    return mSurfaceView->AddOnLayoutChangeListener(listener);
}

ECode CGeckoSurfaceView::RemoveOnLayoutChangeListener(
    /* [in] */ IViewOnLayoutChangeListener* listener)
{
    return mSurfaceView->RemoveOnLayoutChangeListener(listener);
}

ECode CGeckoSurfaceView::AddOnAttachStateChangeListener(
    /* [in] */ IViewOnAttachStateChangeListener* listener)
{
    return mSurfaceView->AddOnAttachStateChangeListener(listener);
}

ECode CGeckoSurfaceView::RemoveOnAttachStateChangeListener(
    /* [in] */ IViewOnAttachStateChangeListener* listener)
{
    return mSurfaceView->RemoveOnAttachStateChangeListener(listener);
}

ECode CGeckoSurfaceView::GetOnFocusChangeListener(
    /* [out] */ IViewOnFocusChangeListener ** ppL)
{
    return mSurfaceView->GetOnFocusChangeListener(ppL);
}

ECode CGeckoSurfaceView::SetOnClickListener(
    /* [in] */ IViewOnClickListener * pL)
{
    return mSurfaceView->SetOnClickListener(pL);
}

ECode CGeckoSurfaceView::HasOnClickListeners(
        /* [out] */ Boolean* has)
{
    return mSurfaceView->HasOnClickListeners(has);
}

ECode CGeckoSurfaceView::SetOnLongClickListener(
    /* [in] */ IViewOnLongClickListener * pL)
{
    return mSurfaceView->SetOnLongClickListener(pL);
}

ECode CGeckoSurfaceView::SetOnCreateContextMenuListener(
    /* [in] */ IViewOnCreateContextMenuListener * pL)
{
    return mSurfaceView->SetOnCreateContextMenuListener(pL);
}

ECode CGeckoSurfaceView::PerformClick(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->PerformClick(pResult);
}

ECode CGeckoSurfaceView::CallOnClick(
        /* [out] */ Boolean* result)
{
    return mSurfaceView->CallOnClick(result);
}

ECode CGeckoSurfaceView::PerformLongClick(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->PerformLongClick(pResult);
}

ECode CGeckoSurfaceView::ShowContextMenu(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->ShowContextMenu(pResult);
}

ECode CGeckoSurfaceView::ShowContextMenu(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 metaState,
    /* [out] */ Boolean* result)
{
    return mSurfaceView->ShowContextMenu(x, y, metaState, result);
}

ECode CGeckoSurfaceView::StartActionMode(
    /* [in] */ IActionModeCallback* cback,
    /* [out] */ IActionMode** mode)
{
    return mSurfaceView->StartActionMode(cback, mode);
}

ECode CGeckoSurfaceView::SetOnKeyListener(
    /* [in] */ IViewOnKeyListener * pL)
{
    return mSurfaceView->SetOnKeyListener(pL);
}

ECode CGeckoSurfaceView::SetOnTouchListener(
    /* [in] */ IViewOnTouchListener * pL)
{
    return mSurfaceView->SetOnTouchListener(pL);
}

ECode CGeckoSurfaceView::SetOnGenericMotionListener(
    /* [in] */ IViewOnGenericMotionListener* l)
{
    return mSurfaceView->SetOnGenericMotionListener(l);
}

ECode CGeckoSurfaceView::SetOnHoverListener(
    /* [in] */ IViewOnHoverListener* l)
{
    return mSurfaceView->SetOnHoverListener(l);
}

ECode CGeckoSurfaceView::SetOnDragListener(
    /* [in] */ IViewOnDragListener* l)
{
    return mSurfaceView->SetOnDragListener(l);
}

ECode CGeckoSurfaceView::RequestRectangleOnScreen(
    /* [in] */ IRect * pRectangle,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestRectangleOnScreen(pRectangle, pResult);
}

ECode CGeckoSurfaceView::RequestRectangleOnScreen(
    /* [in] */ IRect * pRectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestRectangleOnScreen(
                            pRectangle, immediate, pResult);
}

ECode CGeckoSurfaceView::ClearFocus()
{
    return mSurfaceView->ClearFocus();
}

ECode CGeckoSurfaceView::HasFocus(
    /* [out] */ Boolean * pHasFocus)
{
    return mSurfaceView->HasFocus(pHasFocus);
}

ECode CGeckoSurfaceView::HasFocusable(
    /* [out] */ Boolean * pHasFocusable)
{
    return mSurfaceView->HasFocusable(pHasFocusable);
}

ECode CGeckoSurfaceView::AnnounceForAccessibility(
    /* [in] */ ICharSequence* text)
{
    return mSurfaceView->AnnounceForAccessibility(text);
}

ECode CGeckoSurfaceView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchPopulateAccessibilityEvent(
                                pEvent, pResult);
}

ECode CGeckoSurfaceView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    return mSurfaceView->OnPopulateAccessibilityEvent(event);
}

ECode CGeckoSurfaceView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    return mSurfaceView->OnInitializeAccessibilityEvent(event);
}

ECode CGeckoSurfaceView::CreateAccessibilityNodeInfo(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    return mSurfaceView->CreateAccessibilityNodeInfo(info);
}

ECode CGeckoSurfaceView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    return mSurfaceView->OnInitializeAccessibilityNodeInfo(info);
}

ECode CGeckoSurfaceView::GetAccessibilityDelegate(
    /* [out] */ IAccessibilityDelegate** accessibilityDelegate)
{
    return mSurfaceView->GetAccessibilityDelegate(accessibilityDelegate);
}

ECode CGeckoSurfaceView::SetAccessibilityDelegate(
    /* [in] */ IAccessibilityDelegate* accessibilityDelegate)
{
    return mSurfaceView->SetAccessibilityDelegate(accessibilityDelegate);
}

ECode CGeckoSurfaceView::GetAccessibilityNodeProvider(
    /* [out] */ IAccessibilityNodeProvider** provider)
{
    return mSurfaceView->GetAccessibilityNodeProvider(provider);
}

ECode CGeckoSurfaceView::GetAccessibilityViewId(
    /* [out] */ Int32* id)
{
    return mSurfaceView->GetAccessibilityViewId(id);
}

ECode CGeckoSurfaceView::GetAccessibilityWindowId(
    /* [out] */ Int32* id)
{
    return mSurfaceView->GetAccessibilityWindowId(id);
}

ECode CGeckoSurfaceView::GetLabelFor(
    /* [out] */ Int32* id)
{
    return mSurfaceView->GetLabelFor(id);
}

ECode CGeckoSurfaceView::SetLabelFor(
    /* [in] */ Int32 id)
{
    return mSurfaceView->SetLabelFor(id);
}

ECode CGeckoSurfaceView::GetContentDescription(
    /* [out, callee] */ ICharSequence ** ppDescription)
{
    return mSurfaceView->GetContentDescription(ppDescription);
}

ECode CGeckoSurfaceView::SetContentDescription(
    /* [in] */ ICharSequence* contentDescription)
{
    return mSurfaceView->SetContentDescription(contentDescription);
}

ECode CGeckoSurfaceView::IsFocused(
    /* [out] */ Boolean * pFocused)
{
    return mSurfaceView->IsFocused(pFocused);
}

ECode CGeckoSurfaceView::FindFocus(
    /* [out] */ IView ** ppView)
{
    return mSurfaceView->FindFocus(ppView);
}

ECode CGeckoSurfaceView::IsScrollContainer(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->IsScrollContainer(result);
}

ECode CGeckoSurfaceView::SetScrollContainer(
    /* [in] */ Boolean isScrollContainer)
{
    return mSurfaceView->SetScrollContainer(isScrollContainer);
}

ECode CGeckoSurfaceView::GetDrawingCacheQuality(
    /* [out] */ Int32 * pQuality)
{
    return mSurfaceView->GetDrawingCacheQuality(pQuality);
}

ECode CGeckoSurfaceView::SetDrawingCacheQuality(
    /* [in] */ Int32 quality)
{
    return mSurfaceView->SetDrawingCacheQuality(quality);
}

ECode CGeckoSurfaceView::GetKeepScreenOn(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->GetKeepScreenOn(pResult);
}

ECode CGeckoSurfaceView::SetKeepScreenOn(
    /* [in] */ Boolean keepScreenOn)
{
    return mSurfaceView->SetKeepScreenOn(keepScreenOn);
}

ECode CGeckoSurfaceView::GetNextFocusLeftId(
    /* [out] */ Int32 * pNextFocusLeftId)
{
    return mSurfaceView->GetNextFocusLeftId(pNextFocusLeftId);
}

ECode CGeckoSurfaceView::SetNextFocusLeftId(
    /* [in] */ Int32 nextFocusLeftId)
{
    return mSurfaceView->SetNextFocusLeftId(nextFocusLeftId);
}

ECode CGeckoSurfaceView::GetNextFocusRightId(
    /* [out] */ Int32 * pNextFocusRightId)
{
    return mSurfaceView->GetNextFocusRightId(pNextFocusRightId);
}

ECode CGeckoSurfaceView::SetNextFocusRightId(
    /* [in] */ Int32 nextFocusRightId)
{
    return mSurfaceView->SetNextFocusRightId(nextFocusRightId);
}

ECode CGeckoSurfaceView::GetNextFocusUpId(
    /* [out] */ Int32 * pNextFocusUpId)
{
    return mSurfaceView->GetNextFocusUpId(pNextFocusUpId);
}

ECode CGeckoSurfaceView::SetNextFocusUpId(
    /* [in] */ Int32 nextFocusUpId)
{
    return mSurfaceView->SetNextFocusUpId(nextFocusUpId);
}

ECode CGeckoSurfaceView::GetNextFocusDownId(
    /* [out] */ Int32 * pNextFocusDownId)
{
    return mSurfaceView->GetNextFocusDownId(pNextFocusDownId);
}

ECode CGeckoSurfaceView::SetNextFocusDownId(
    /* [in] */ Int32 nextFocusDownId)
{
    return mSurfaceView->SetNextFocusDownId(nextFocusDownId);
}

ECode CGeckoSurfaceView::GetNextFocusForwardId(
    /* [out] */ Int32* id)
{
    return mSurfaceView->GetNextFocusForwardId(id);
}

ECode CGeckoSurfaceView::SetNextFocusForwardId(
    /* [in] */ Int32 nextFocusForwardId)
{
    return mSurfaceView->SetNextFocusForwardId(nextFocusForwardId);
}

ECode CGeckoSurfaceView::IsShown(
    /* [out] */ Boolean * pShown)
{
    return mSurfaceView->IsShown(pShown);
}

ECode CGeckoSurfaceView::SetFitsSystemWindows(
    /* [in] */ Boolean fitSystemWindows)
{
    return mSurfaceView->SetFitsSystemWindows(fitSystemWindows);
}

ECode CGeckoSurfaceView::GetFitsSystemWindows(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->GetFitsSystemWindows(result);
}

ECode CGeckoSurfaceView::FitsSystemWindows(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->FitsSystemWindows(result);
}

ECode CGeckoSurfaceView::RequestFitSystemWindows()
{
    return mSurfaceView->RequestFitSystemWindows();
}

ECode CGeckoSurfaceView::MakeOptionalFitsSystemWindows()
{
    return mSurfaceView->MakeOptionalFitsSystemWindows();
}

ECode CGeckoSurfaceView::GetVisibility(
    /* [out] */ Int32 * pVisibility)
{
    return mSurfaceView->GetVisibility(pVisibility);
}

ECode CGeckoSurfaceView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    return mSurfaceView->SetVisibility(visibility);
}

ECode CGeckoSurfaceView::IsEnabled(
    /* [out] */ Boolean * pEnabled)
{
    return mSurfaceView->IsEnabled(pEnabled);
}

ECode CGeckoSurfaceView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetEnabled(enabled);
}

ECode CGeckoSurfaceView::SetFocusable(
    /* [in] */ Boolean focusable)
{
    return mSurfaceView->SetFocusable(focusable);
}

ECode CGeckoSurfaceView::SetFocusableInTouchMode(
    /* [in] */ Boolean focusableInTouchMode)
{
    return mSurfaceView->SetFocusableInTouchMode(focusableInTouchMode);
}

ECode CGeckoSurfaceView::SetSoundEffectsEnabled(
    /* [in] */ Boolean soundEffectsEnabled)
{
    return mSurfaceView->SetSoundEffectsEnabled(soundEffectsEnabled);
}

ECode CGeckoSurfaceView::IsSoundEffectsEnabled(
    /* [out] */ Boolean * pSoundEffectsEnabled)
{
    return mSurfaceView->IsSoundEffectsEnabled(pSoundEffectsEnabled);
}

ECode CGeckoSurfaceView::SetHapticFeedbackEnabled(
    /* [in] */ Boolean hapticFeedbackEnabled)
{
    return mSurfaceView->SetHapticFeedbackEnabled(hapticFeedbackEnabled);
}

ECode CGeckoSurfaceView::IsHapticFeedbackEnabled(
    /* [out] */ Boolean * pHapticFeedbackEnabled)
{
    return mSurfaceView->IsHapticFeedbackEnabled(pHapticFeedbackEnabled);
}

ECode CGeckoSurfaceView::GetRawLayoutDirection(
    /* [out] */ Int32* direction)
{
    return mSurfaceView->GetRawLayoutDirection(direction);
}

ECode CGeckoSurfaceView::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    return mSurfaceView->SetLayoutDirection(layoutDirection);
}

ECode CGeckoSurfaceView::GetLayoutDirection(
    /* [out] */ Int32* direction)
{
    return mSurfaceView->GetLayoutDirection(direction);
}

ECode CGeckoSurfaceView::IsLayoutRtl(
    /* [out] */ Boolean* rtl)
{
    return mSurfaceView->IsLayoutRtl(rtl);
}

ECode CGeckoSurfaceView::HasTransientState(
    /* [out] */ Boolean* has)
{
    return mSurfaceView->HasTransientState(has);
}

ECode CGeckoSurfaceView::SetHasTransientState(
    /* [in] */ Boolean hasTransientState)
{
    return mSurfaceView->SetHasTransientState(hasTransientState);
}

ECode CGeckoSurfaceView::SetWillNotDraw(
    /* [in] */ Boolean willNotDraw)
{
    return mSurfaceView->SetWillNotDraw(willNotDraw);
}

ECode CGeckoSurfaceView::WillNotDraw(
    /* [out] */ Boolean * pWillNotDraw)
{
    return mSurfaceView->WillNotDraw(pWillNotDraw);
}

ECode CGeckoSurfaceView::SetWillNotCacheDrawing(
    /* [in] */ Boolean willNotCacheDrawing)
{
    return mSurfaceView->SetWillNotCacheDrawing(willNotCacheDrawing);
}

ECode CGeckoSurfaceView::WillNotCacheDrawing(
    /* [out] */ Boolean * pCache)
{
    return mSurfaceView->WillNotCacheDrawing(pCache);
}

ECode CGeckoSurfaceView::IsClickable(
    /* [out] */ Boolean * pClickable)
{
    return mSurfaceView->IsClickable(pClickable);
}

ECode CGeckoSurfaceView::SetClickable(
    /* [in] */ Boolean clickable)
{
    return mSurfaceView->SetClickable(clickable);
}

ECode CGeckoSurfaceView::IsLongClickable(
    /* [out] */ Boolean * pLongClickable)
{
    return mSurfaceView->IsLongClickable(pLongClickable);
}

ECode CGeckoSurfaceView::SetLongClickable(
    /* [in] */ Boolean longClickable)
{
    return mSurfaceView->SetLongClickable(longClickable);
}

ECode CGeckoSurfaceView::SetPressed(
    /* [in] */ Boolean pressed)
{
    return mSurfaceView->SetPressed(pressed);
}

ECode CGeckoSurfaceView::IsPressed(
    /* [out] */ Boolean * pPressed)
{
    return mSurfaceView->IsPressed(pPressed);
}

ECode CGeckoSurfaceView::IsSaveEnabled(
    /* [out] */ Boolean * pEnabled)
{
    return mSurfaceView->IsSaveEnabled(pEnabled);
}

ECode CGeckoSurfaceView::SetSaveEnabled(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetSaveEnabled(enabled);
}

ECode CGeckoSurfaceView::GetFilterTouchesWhenObscured(
    /* [out] */ Boolean * pEnabled)
{
    return mSurfaceView->GetFilterTouchesWhenObscured(pEnabled);
}

ECode CGeckoSurfaceView::SetFilterTouchesWhenObscured(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetFilterTouchesWhenObscured(enabled);
}

ECode CGeckoSurfaceView::IsSaveFromParentEnabled(
    /* [out] */ Boolean* enabled)
{
    return mSurfaceView->IsSaveFromParentEnabled(enabled);
}

ECode CGeckoSurfaceView::SetSaveFromParentEnabled(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetSaveFromParentEnabled(enabled);
}

ECode CGeckoSurfaceView::IsFocusable(
    /* [out] */ Boolean * pFocusable)
{
    return mSurfaceView->IsFocusable(pFocusable);
}

ECode CGeckoSurfaceView::IsFocusableInTouchMode(
    /* [out] */ Boolean * pFocusable)
{
    return mSurfaceView->IsFocusableInTouchMode(pFocusable);
}

ECode CGeckoSurfaceView::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IView ** ppView)
{
    return mSurfaceView->FocusSearch(direction, ppView);
}

ECode CGeckoSurfaceView::DispatchUnhandledMove(
    /* [in] */ IView * pFocused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchUnhandledMove(pFocused, direction, pResult);
}

ECode CGeckoSurfaceView::GetFocusables(
    /* [in] */ Int32 direction,
    /* [out] */ IObjectContainer ** ppViews)
{
    return mSurfaceView->GetFocusables(direction, ppViews);
}

ECode CGeckoSurfaceView::AddFocusables(
    /* [in] */ IObjectContainer * pViews,
    /* [in] */ Int32 direction)
{
    return mSurfaceView->AddFocusables(pViews, direction);
}

ECode CGeckoSurfaceView::AddFocusables(
    /* [in] */ IObjectContainer * pViews,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 focusableMode)
{
    return mSurfaceView->AddFocusables(pViews, direction, focusableMode);
}

ECode CGeckoSurfaceView::FindViewsWithText(
    /* [in] */ IObjectContainer* outViews,
    /* [in] */ ICharSequence* searched,
    /* [in] */ Int32 flags)
{
    return mSurfaceView->FindViewsWithText(outViews, searched, flags);
}

ECode CGeckoSurfaceView::GetTouchables(
    /* [out] */ IObjectContainer ** ppViews)
{
    return mSurfaceView->GetTouchables(ppViews);
}

ECode CGeckoSurfaceView::AddTouchables(
    /* [in] */ IObjectContainer * pViews)
{
    return mSurfaceView->AddTouchables(pViews);
}

ECode CGeckoSurfaceView::RequestAccessibilityFocus(
    /* [out] */ Boolean* focus)
{
    return mSurfaceView->RequestAccessibilityFocus(focus);
}

ECode CGeckoSurfaceView::ClearAccessibilityFocus()
{
    return mSurfaceView->ClearAccessibilityFocus();
}

ECode CGeckoSurfaceView::RequestFocus(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestFocus(pResult);
}

ECode CGeckoSurfaceView::RequestFocus(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestFocus(direction, pResult);
}

ECode CGeckoSurfaceView::RequestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect * pPreviouslyFocusedRect,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestFocus(
                    direction, pPreviouslyFocusedRect, pResult);
}

ECode CGeckoSurfaceView::RequestFocusFromTouch(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->RequestFocusFromTouch(pResult);
}

ECode CGeckoSurfaceView::GetImportantForAccessibility(
    /* [out] */ Int32* mode)
{
    return mSurfaceView->GetImportantForAccessibility(mode);
}

ECode CGeckoSurfaceView::SetImportantForAccessibility(
    /* [in] */ Int32 mode)
{
    return mSurfaceView->SetImportantForAccessibility(mode);
}

ECode CGeckoSurfaceView::IsImportantForAccessibility(
    /* [out] */ Boolean* exposed)
{
    return mSurfaceView->IsImportantForAccessibility(exposed);
}

ECode CGeckoSurfaceView::GetParentForAccessibility(
    /* [out] */ IViewParent** parent)
{
    return mSurfaceView->GetParentForAccessibility(parent);
}

ECode CGeckoSurfaceView::AddChildrenForAccessibility(
    /* [in] */ IObjectContainer* children)
{
    return mSurfaceView->AddChildrenForAccessibility(children);
}

ECode CGeckoSurfaceView::IncludeForAccessibility(
    /* [out] */ Boolean* include)
{
    return mSurfaceView->IncludeForAccessibility(include);
}

ECode CGeckoSurfaceView::IsActionableForAccessibility(
    /* [out] */ Boolean* actionable)
{
    return mSurfaceView->IsActionableForAccessibility(actionable);
}

ECode CGeckoSurfaceView::NotifyAccessibilityStateChanged()
{
    return mSurfaceView->NotifyAccessibilityStateChanged();
}

ECode CGeckoSurfaceView::ResetAccessibilityStateChanged()
{
    return mSurfaceView->ResetAccessibilityStateChanged();
}

ECode CGeckoSurfaceView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* performed)
{
    return mSurfaceView->PerformAccessibilityAction(action, arguments, performed);
}

ECode CGeckoSurfaceView::GetIterableTextForAccessibility(
    /* [out] */ ICharSequence** text)
{
    return mSurfaceView->GetIterableTextForAccessibility(text);
}

ECode CGeckoSurfaceView::GetAccessibilityCursorPosition(
    /* [out] */ Int32* pos)
{
    return mSurfaceView->GetAccessibilityCursorPosition(pos);
}

ECode CGeckoSurfaceView::SetAccessibilityCursorPosition(
    /* [in] */ Int32 position)
{
    return mSurfaceView->SetAccessibilityCursorPosition(position);
}

ECode CGeckoSurfaceView::GetIteratorForGranularity(
    /* [in] */ Int32 granularity,
    /* [out] */ ITextSegmentIterator** iterator)
{
    return mSurfaceView->GetIteratorForGranularity(granularity, iterator);
}

ECode CGeckoSurfaceView::DispatchStartTemporaryDetach()
{
    return mSurfaceView->DispatchStartTemporaryDetach();
}

ECode CGeckoSurfaceView::OnStartTemporaryDetach()
{
    return mSurfaceView->OnStartTemporaryDetach();
}

ECode CGeckoSurfaceView::DispatchFinishTemporaryDetach()
{
    return mSurfaceView->DispatchFinishTemporaryDetach();
}

ECode CGeckoSurfaceView::OnFinishTemporaryDetach()
{
    return mSurfaceView->OnFinishTemporaryDetach();
}

ECode CGeckoSurfaceView::GetKeyDispatcherState(
    /* [out] */ IDispatcherState ** ppState)
{
    return mSurfaceView->GetKeyDispatcherState(ppState);
}

ECode CGeckoSurfaceView::DispatchKeyEventPreIme(
    /* [in] */ IKeyEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchKeyEventPreIme(pEvent, pResult);
}

ECode CGeckoSurfaceView::DispatchKeyEvent(
    /* [in] */ IKeyEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchKeyEvent(pEvent, pResult);
}

ECode CGeckoSurfaceView::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchKeyShortcutEvent(pEvent, pResult);
}

ECode CGeckoSurfaceView::DispatchTouchEvent(
    /* [in] */ IMotionEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchTouchEvent(pEvent, pResult);
}

ECode CGeckoSurfaceView::OnFilterTouchEventForSecurity(
    /* [in] */ IMotionEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->OnFilterTouchEventForSecurity(pEvent, pResult);
}

ECode CGeckoSurfaceView::DispatchTrackballEvent(
    /* [in] */ IMotionEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->DispatchTrackballEvent(pEvent, pResult);
}

ECode CGeckoSurfaceView::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    return mSurfaceView->DispatchGenericMotionEvent(event, handled);
}

ECode CGeckoSurfaceView::DispatchPointerEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    return mSurfaceView->DispatchPointerEvent(event, handled);
}

ECode CGeckoSurfaceView::DispatchWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return mSurfaceView->DispatchWindowFocusChanged(hasFocus);
}

ECode CGeckoSurfaceView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    return mSurfaceView->OnWindowFocusChanged(hasWindowFocus);
}

ECode CGeckoSurfaceView::HasWindowFocus(
    /* [out] */ Boolean * pFocus)
{
    return mSurfaceView->HasWindowFocus(pFocus);
}

ECode CGeckoSurfaceView::DispatchDisplayHint(
    /* [in] */ Int32 hint)
{
    return mSurfaceView->DispatchDisplayHint(hint);
}

ECode CGeckoSurfaceView::DispatchWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    return mSurfaceView->DispatchWindowVisibilityChanged(visibility);
}

ECode CGeckoSurfaceView::GetWindowVisibility(
    /* [out] */ Int32 * pVisibility)
{
    return mSurfaceView->GetWindowVisibility(pVisibility);
}

ECode CGeckoSurfaceView::GetWindowVisibleDisplayFrame(
    /* [in] */ IRect * pOutRect)
{
    return mSurfaceView->GetWindowVisibleDisplayFrame(pOutRect);
}

ECode CGeckoSurfaceView::DispatchConfigurationChanged(
    /* [in] */ IConfiguration * pNewConfig)
{
    return mSurfaceView->DispatchConfigurationChanged(pNewConfig);
}

ECode CGeckoSurfaceView::IsInTouchMode(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->IsInTouchMode(pResult);
}

ECode CGeckoSurfaceView::GetContext(
    /* [out] */ IContext ** ppCtx)
{
    return mSurfaceView->GetContext(ppCtx);
}

ECode CGeckoSurfaceView::OnKeyPreIme(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isSystem;
    event->IsSystem(&isSystem);
    if (isSystem) {
        return mSurfaceView->OnKeyPreIme(keyCode, event, result);
    }

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IKeyEvent::ACTION_DOWN:
            return ProcessKeyDown(keyCode, event, TRUE, result);
        case IKeyEvent::ACTION_UP:
            return ProcessKeyUp(keyCode, event, TRUE, result);
        case IKeyEvent::ACTION_MULTIPLE:
            Int32 repeatCount;
            event->GetRepeatCount(&repeatCount);
            return OnKeyMultiple(keyCode, repeatCount, event, result);
    }
    return mSurfaceView->OnKeyPreIme(keyCode, event, result);
}

ECode CGeckoSurfaceView::OnKeyShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mSurfaceView->OnKeyShortcut(keyCode, event, result);
}

ECode CGeckoSurfaceView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* _event,
    /* [out] */ Boolean* result)
{
    return ProcessKeyDown(keyCode, _event, FALSE, result);
}

ECode CGeckoSurfaceView::ProcessKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* _event,
    /* [in] */ Boolean isPreIme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IKeyEvent> event = _event;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_BACK: {
            Int32 count;
            event->GetRepeatCount(&count);
            if (count == 0) {
                event->StartTracking();
                *result = TRUE;
                return NOERROR;
            }
            else {
                //*result = FALSE;
                *result = TRUE;
                return NOERROR;
            }
        }
        case IKeyEvent::KEYCODE_MENU: {
            Int32 count, flags;
            event->GetRepeatCount(&count);
            if (count == 0) {
                event->StartTracking();
                break;
            }
            else if ((event->GetFlags(&flags), flags & IKeyEvent::FLAG_LONG_PRESS) != 0) {
                break;
            }
            // Ignore repeats for KEYCODE_MENU; they confuse the widget code.
            *result = FALSE;
            return NOERROR;
        }
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_SEARCH:
            *result = FALSE;
            return NOERROR;
        case IKeyEvent::KEYCODE_DEL: {
            Boolean _res;
            // See comments in GeckoInputConnection.onKeyDel
            if (mInputConnection != NULL &&
                    (mInputConnection->OnKeyDel(&_res), _res)) {
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
        case IKeyEvent::KEYCODE_ENTER: {
            Int32 flags;
            event->GetFlags(&flags);
            if ((flags & IKeyEvent::FLAG_EDITOR_ACTION) != 0 &&
                mIMEActionHint.EqualsIgnoreCase("next")) {
                Int32 action = 0;
                event->GetAction(&action);
                event = NULL;
                ASSERT_SUCCEEDED(CKeyEvent::New(action, IKeyEvent::KEYCODE_TAB, (IKeyEvent**)&event));
            }
            break;
        }
        default:
            break;
    }

    Int32 metaState;
    event->GetMetaState(&metaState);
    if (isPreIme && mIMEState != IME_STATE_DISABLED &&
        (metaState & IKeyEvent::META_ALT_ON) == 0) {
        // Let active IME process pre-IME key events
        *result = FALSE;
        return NOERROR;
    }

    // KeyListener returns true if it handled the event for us.
    Int32 flags;
    Boolean _res;
    if (mIMEState == IME_STATE_DISABLED ||
        keyCode == IKeyEvent::KEYCODE_ENTER ||
        keyCode == IKeyEvent::KEYCODE_DEL ||
        (event->GetFlags(&flags), flags & IKeyEvent::FLAG_SOFT_KEYBOARD) != 0 ||
        (mKeyListener->OnKeyDown(this, mEditable, keyCode, event, &_res), !_res)) {
        AutoPtr<GeckoEvent> geckoEvent = new GeckoEvent(event);
        assert(geckoEvent != NULL);
        GeckoAppShell::SendEventToGecko(geckoEvent);
    }
    *result = TRUE;
    return NOERROR;
}

ECode CGeckoSurfaceView::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* pEvent,
    /* [out] */ Boolean* pResult)
{
    ECode ec;
    GeckoEvent* pGeckoEvent;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_BACK:
            pGeckoEvent = new GeckoEvent(pEvent);
            if (!pGeckoEvent) return E_OUT_OF_MEMORY;
            GeckoAppShell::SendEventToGecko(pGeckoEvent);
            *pResult = TRUE;
            return NOERROR;
        case IKeyEvent::KEYCODE_MENU:
        {
            AutoPtr<IContext> pIContext;
            ec = GetContext((IContext**)&pIContext);
            if (FAILED(ec)) return ec;

            AutoPtr<IInputMethodManager> pImm;
            ec = pIContext->GetSystemService(
                            IContext::INPUT_METHOD_SERVICE, (IInterface**)&pImm);
            if (FAILED(ec)) return ec;
            AutoPtr<IBinder> pIBinder;
            ec = GetWindowToken((IBinder**)&pIBinder);
            if (FAILED(ec)) return ec;

            ec = pImm->ToggleSoftInputFromWindow(pIBinder,
                          IInputMethodManager::SHOW_FORCED, 0);
            if (FAILED(ec)) return ec;
            *pResult = TRUE;
            return NOERROR;
        }
        default:
            break;
    }
    *pResult = FALSE;
    return NOERROR;
}

ECode CGeckoSurfaceView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* pEvent,
    /* [out] */ Boolean* pResult)
{
    return ProcessKeyUp(keyCode, pEvent, FALSE, pResult);
}

ECode CGeckoSurfaceView::ProcessKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* pEvent,
    /* [in] */ Boolean isPreIme,
    /* [out] */ Boolean* pResult) {
    switch (keyCode) {
        case IKeyEvent::KEYCODE_BACK:
            Boolean isTracking;
            Boolean isCanceled;
            pEvent->IsTracking(&isTracking);
            pEvent->IsCanceled(&isCanceled);
            if (!isTracking || isCanceled) {
                *pResult = FALSE;
                return NOERROR;
            }
            break;
        default:
            break;
    }

    Int32 metaState;
    pEvent->GetMetaState(&metaState);
    if (isPreIme && mIMEState != IME_STATE_DISABLED &&
        (metaState & IKeyEvent::META_ALT_ON) == 0) {
        // Let active IME process pre-IME key events
        *pResult = FALSE;
        return NOERROR;
    }
    Int32 flags = 0;
    pEvent->GetFlags(&flags);
    if (mIMEState == IME_STATE_DISABLED ||
        keyCode == IKeyEvent::KEYCODE_ENTER ||
        keyCode == IKeyEvent::KEYCODE_DEL ||
        (flags & IKeyEvent::FLAG_SOFT_KEYBOARD) != 0 ||
        //(mKeyListener->OnKeyUp(this, mEditable, keyCode, pEvent, &result), !result)
        TRUE//TextKeyListener has not implementeded. This is a temporary implementation.
        // when achieved, we need to modify this code.
        ) {
        GeckoEvent* pGeckoEvent = new GeckoEvent(pEvent);
        GeckoAppShell::SendEventToGecko(pGeckoEvent);
    }
    *pResult = TRUE;
    return NOERROR;
}

ECode CGeckoSurfaceView::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* pEvent,
    /* [out] */ Boolean* pResult)
{
    GeckoEvent* pGeckoEvent = new GeckoEvent(pEvent);
    if (!pGeckoEvent) return E_OUT_OF_MEMORY;
    GeckoAppShell::SendEventToGecko(pGeckoEvent);

    *pResult = TRUE;
    return NOERROR;
}

ECode CGeckoSurfaceView::OnCreateInputConnection(
    /* [in] */ IEditorInfo * pOutAttrs,
    /* [out] */ IInputConnection ** ppInputConnection)
{
    /*pOutAttrs->SetInputType(InputType_TYPE_CLASS_TEXT);
    pOutAttrs->SetImeOptions(EditorInfo_IME_ACTION_NONE);
    pOutAttrs->SetActionLabel(NULL);
    mKeyListener = TextKeyListener::GetInstance();

    if (mIMEState == IME_STATE_PASSWORD) {
        outAttrs.inputType |= InputType_TYPE_TEXT_VARIATION_PASSWORD;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("url")) {
        outAttrs.inputType |= InputType_TYPE_TEXT_VARIATION_URI;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("email")) {
        outAttrs.inputType |= InputType_TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("search")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_SEARCH;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("tel")) {
        outAttrs.inputType = InputType_TYPE_CLASS_PHONE;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("number") ||
             mIMETypeHint.EqualsIgnoreCase("range")) {
        outAttrs.inputType = InputType_TYPE_CLASS_NUMBER;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("datetime") ||
             mIMETypeHint.EqualsIgnoreCase("datetime-local")) {
        outAttrs.inputType = InputType_TYPE_CLASS_DATETIME |
                             InputType_TYPE_DATETIME_VARIATION_NORMAL;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("date")) {
        outAttrs.inputType = InputType_TYPE_CLASS_DATETIME |
                             InputType_TYPE_DATETIME_VARIATION_DATE;
    }
    else if (mIMETypeHint.EqualsIgnoreCase("time")) {
        outAttrs.inputType = InputType_TYPE_CLASS_DATETIME |
                             InputType_TYPE_DATETIME_VARIATION_TIME;
    }

    if (mIMEActionHint.EqualsIgnoreCase("go")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_GO;
    }
    else if (mIMEActionHint.equalsIgnoreCase("done")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_DONE;
    }
    else if (mIMEActionHint.equalsIgnoreCase("next")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_NEXT;
    }
    else if (mIMEActionHint.equalsIgnoreCase("search")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_SEARCH;
    }
    else if (mIMEActionHint.equalsIgnoreCase("send")) {
        outAttrs.imeOptions = EditorInfo_IME_ACTION_SEND;
    }
    else if (!mIMEActionHint.IsNull() && mIMEActionHint.GetLength() != 0) {
        outAttrs.actionLabel = mIMEActionHint;
    }

    if (mIMELandscapeFS == FALSE) {
        outAttrs.imeOptions |= EditorInfo_IME_FLAG_NO_EXTRACT_UI;
    }*/

    mInputConnection->Reset();
    *ppInputConnection = mInputConnection;
    mInputConnection->AddRef();
    return NOERROR;
}

ECode CGeckoSurfaceView::CheckInputConnectionProxy(
    /* [in] */ IView * pView,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->CheckInputConnectionProxy(pView, pResult);
}

ECode CGeckoSurfaceView::CreateContextMenu(
    /* [in] */ IContextMenu * pMenu)
{
    return mSurfaceView->CreateContextMenu(pMenu);
}

ECode CGeckoSurfaceView::OnTrackballEvent(
    /* [in] */ IMotionEvent * pEvent,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->OnTrackballEvent(pEvent, pResult);
}

ECode CGeckoSurfaceView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    return mSurfaceView->OnGenericMotionEvent(event, handled);
}

ECode CGeckoSurfaceView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    return mSurfaceView->OnHoverEvent(event, handled);
}

ECode CGeckoSurfaceView::IsHovered(
    /* [out] */ Boolean* hovered)
{
    return mSurfaceView->IsHovered(hovered);
}

ECode CGeckoSurfaceView::SetHovered(
    /* [in] */ Boolean hovered)
{
    return mSurfaceView->SetHovered(hovered);
}

ECode CGeckoSurfaceView::OnHoverChanged(
    /* [in] */ Boolean hovered)
{
    return mSurfaceView->OnHoverChanged(hovered);
}

ECode CGeckoSurfaceView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    AutoPtr<GeckoEvent> geckoEvent = new GeckoEvent(event);
    assert(geckoEvent != NULL);
    GeckoAppShell::SendEventToGecko(geckoEvent);
    *result = TRUE;
    return NOERROR;
}

ECode CGeckoSurfaceView::IsInScrollingContainer(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->IsInScrollingContainer(result);
}

ECode CGeckoSurfaceView::SetTouchDelegate(
    /* [in] */ ITouchDelegate * pTouchDelegate)
{
    return mSurfaceView->SetTouchDelegate(pTouchDelegate);
}

ECode CGeckoSurfaceView::GetTouchDelegate(
    /* [out] */ ITouchDelegate ** ppTouchDelegate)
{
    return mSurfaceView->GetTouchDelegate(ppTouchDelegate);
}

ECode CGeckoSurfaceView::CancelLongPress()
{
    return mSurfaceView->CancelLongPress();
}

ECode CGeckoSurfaceView::BringToFront()
{
    return mSurfaceView->BringToFront();
}

ECode CGeckoSurfaceView::GetParent(
    /* [out] */ IViewParent ** ppParent)
{
    return mSurfaceView->GetParent(ppParent);
}

ECode CGeckoSurfaceView::SetScrollX(
    /* [in] */ Int32 value)
{
    return mSurfaceView->SetScrollX(value);
}

ECode CGeckoSurfaceView::SetScrollY(
    /* [in] */ Int32 value)
{
    return mSurfaceView->SetScrollY(value);
}

ECode CGeckoSurfaceView::GetScrollX(
    /* [out] */ Int32 * pX)
{
    return mSurfaceView->GetScrollX(pX);
}

ECode CGeckoSurfaceView::GetScrollY(
    /* [out] */ Int32 * pY)
{
    return mSurfaceView->GetScrollY(pY);
}

ECode CGeckoSurfaceView::GetWidth(
    /* [out] */ Int32 * pWidth)
{
    return mSurfaceView->GetWidth(pWidth);
}

ECode CGeckoSurfaceView::GetHeight(
    /* [out] */ Int32 * pHeight)
{
    return mSurfaceView->GetHeight(pHeight);
}

ECode CGeckoSurfaceView::GetDrawingRect(
    /* [in] */ IRect * pOutRect)
{
    return mSurfaceView->GetDrawingRect(pOutRect);
}

ECode CGeckoSurfaceView::GetMeasuredWidth(
    /* [out] */ Int32 * pWidth)
{
    return mSurfaceView->GetMeasuredWidth(pWidth);
}

ECode CGeckoSurfaceView::GetMeasuredWidthAndState(
    /* [out] */ Int32* width)
{
    return mSurfaceView->GetMeasuredWidthAndState(width);
}

ECode CGeckoSurfaceView::GetMeasuredHeight(
    /* [out] */ Int32 * pHeight)
{
    return mSurfaceView->GetMeasuredHeight(pHeight);
}

ECode CGeckoSurfaceView::GetMeasuredHeightAndState(
    /* [out] */ Int32* height)
{
    return mSurfaceView->GetMeasuredHeightAndState(height);
}

ECode CGeckoSurfaceView::GetMeasuredState(
    /* [out] */ Int32* state)
{
    return mSurfaceView->GetMeasuredState(state);
}

ECode CGeckoSurfaceView::GetMatrix(
    /* [out] */ IMatrix** matrix)
{
    return mSurfaceView->GetMatrix(matrix);
}

ECode CGeckoSurfaceView::GetCameraDistance(
    /* [out] */ Float* distance)
{
    return mSurfaceView->GetCameraDistance(distance);
}

ECode CGeckoSurfaceView::SetCameraDistance(
    /* [in] */ Float distance)
{
    return mSurfaceView->SetCameraDistance(distance);
}

ECode CGeckoSurfaceView::GetRotation(
    /* [out] */ Float* rotation)
{
    return mSurfaceView->GetRotation(rotation);
}

ECode CGeckoSurfaceView::SetRotation(
    /* [in] */ Float rotation)
{
    return mSurfaceView->SetRotation(rotation);
}

ECode CGeckoSurfaceView::GetRotationY(
    /* [out] */ Float* y)
{
    return mSurfaceView->GetRotationY(y);
}

ECode CGeckoSurfaceView::SetRotationY(
    /* [in] */ Float rotationY)
{
    return mSurfaceView->SetRotationY(rotationY);
}

ECode CGeckoSurfaceView::GetRotationX(
    /* [out] */ Float* x)
{
    return mSurfaceView->GetRotationX(x);
}

ECode CGeckoSurfaceView::SetRotationX(
    /* [in] */ Float rotationX)
{
    return mSurfaceView->SetRotationX(rotationX);
}

ECode CGeckoSurfaceView::GetScaleX(
    /* [out] */ Float* x)
{
    return mSurfaceView->GetScaleX(x);
}

ECode CGeckoSurfaceView::SetScaleX(
    /* [in] */ Float scaleX)
{
    return mSurfaceView->SetScaleX(scaleX);
}

ECode CGeckoSurfaceView::GetScaleY(
    /* [out] */ Float* y)
{
    return mSurfaceView->GetScaleY(y);
}

ECode CGeckoSurfaceView::SetScaleY(
    /* [in] */ Float scaleY)
{
    return mSurfaceView->SetScaleY(scaleY);
}

ECode CGeckoSurfaceView::GetPivotX(
    /* [out] */ Float* x)
{
    return mSurfaceView->GetPivotX(x);
}

ECode CGeckoSurfaceView::SetPivotX(
    /* [in] */ Float pivotX)
{
    return mSurfaceView->SetPivotX(pivotX);
}

ECode CGeckoSurfaceView::GetPivotY(
    /* [out] */ Float* y)
{
    return mSurfaceView->GetPivotY(y);
}

ECode CGeckoSurfaceView::SetPivotY(
    /* [in] */ Float pivotY)
{
    return mSurfaceView->SetPivotY(pivotY);
}

ECode CGeckoSurfaceView::GetAlpha(
    /* [out] */ Float* alpha)
{
    return mSurfaceView->GetAlpha(alpha);
}

ECode CGeckoSurfaceView::HasOverlappingRendering(
    /* [out] */ Boolean* overlap)
{
    return mSurfaceView->HasOverlappingRendering(overlap);
}

ECode CGeckoSurfaceView::SetAlpha(
    /* [in] */ Float alpha)
{
    return mSurfaceView->SetAlpha(alpha);
}

ECode CGeckoSurfaceView::GetTop(
    /* [out] */ Int32 * pTop)
{
    return mSurfaceView->GetTop(pTop);
}

ECode CGeckoSurfaceView::SetTop(
    /* [in] */ Int32 top)
{
    return mSurfaceView->SetTop(top);
}

ECode CGeckoSurfaceView::GetBottom(
    /* [out] */ Int32 * pBottom)
{
    return mSurfaceView->GetBottom(pBottom);
}

ECode CGeckoSurfaceView::IsDirty(
    /* [out] */ Boolean* dirty)
{
    return mSurfaceView->IsDirty(dirty);
}

ECode CGeckoSurfaceView::SetBottom(
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->SetBottom(bottom);
}

ECode CGeckoSurfaceView::GetLeft(
    /* [out] */ Int32 * pLeft)
{
    return mSurfaceView->GetLeft(pLeft);
}

ECode CGeckoSurfaceView::SetLeft(
    /* [in] */ Int32 left)
{
    return mSurfaceView->SetLeft(left);
}

ECode CGeckoSurfaceView::GetRight(
    /* [out] */ Int32 * pRight)
{
    return mSurfaceView->GetRight(pRight);
}

ECode CGeckoSurfaceView::SetRight(
    /* [in] */ Int32 right)
{
    return mSurfaceView->SetRight(right);
}

ECode CGeckoSurfaceView::GetX(
    /* [out] */ Float* x)
{
    return mSurfaceView->GetX(x);
}

ECode CGeckoSurfaceView::SetX(
    /* [in] */ Float x)
{
    return mSurfaceView->SetX(x);
}

ECode CGeckoSurfaceView::GetY(
    /* [out] */ Float* y)
{
    return mSurfaceView->GetY(y);
}

ECode CGeckoSurfaceView::SetY(
    /* [in] */ Float y)
{
    return mSurfaceView->SetY(y);
}

ECode CGeckoSurfaceView::GetTranslationX(
    /* [out] */ Float* x)
{
    return mSurfaceView->GetTranslationX(x);
}

ECode CGeckoSurfaceView::SetTranslationX(
    /* [in] */ Float translationX)
{
    return mSurfaceView->SetTranslationX(translationX);
}

ECode CGeckoSurfaceView::GetTranslationY(
    /* [out] */ Float* y)
{
    return mSurfaceView->GetTranslationY(y);
}

ECode CGeckoSurfaceView::SetTranslationY(
    /* [in] */ Float translationY)
{
    return mSurfaceView->SetTranslationY(translationY);
}

ECode CGeckoSurfaceView::GetHitRect(
    /* [in] */ IRect * pOutRect)
{
    return mSurfaceView->GetHitRect(pOutRect);
}

ECode CGeckoSurfaceView::GetFocusedRect(
    /* [in] */ IRect * pOutRect)
{
    return mSurfaceView->GetFocusedRect(pOutRect);
}

ECode CGeckoSurfaceView::GetGlobalVisibleRect(
    /* [in] */ IRect * pOutRect,
    /* [in] */ IPoint * pOutGlobalOffset,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->GetGlobalVisibleRect(
                    pOutRect, pOutGlobalOffset, pResult);
}

ECode CGeckoSurfaceView::GetGlobalVisibleRect(
    /* [in] */ IRect * pOutRect,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->GetGlobalVisibleRect(pOutRect, pResult);
}

ECode CGeckoSurfaceView::GetLocalVisibleRect(
    /* [in] */ IRect * pOutRect,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->GetLocalVisibleRect(pOutRect, pResult);
}

ECode CGeckoSurfaceView::OffsetTopAndBottom(
    /* [in] */ Int32 offset)
{
    return mSurfaceView->OffsetTopAndBottom(offset);
}

ECode CGeckoSurfaceView::OffsetLeftAndRight(
    /* [in] */ Int32 offset)
{
    return mSurfaceView->OffsetLeftAndRight(offset);
}

ECode CGeckoSurfaceView::GetLayoutParams(
    /* [out] */ IViewGroupLayoutParams ** ppParams)
{
    return mSurfaceView->GetLayoutParams(ppParams);
}

ECode CGeckoSurfaceView::SetLayoutParams(
    /* [in] */ IViewGroupLayoutParams * pParams)
{
    return mSurfaceView->SetLayoutParams(pParams);
}

ECode CGeckoSurfaceView::ResolveLayoutParams()
{
    return mSurfaceView->ResolveLayoutParams();
}

ECode CGeckoSurfaceView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return mSurfaceView->ScrollTo(x, y);
}

ECode CGeckoSurfaceView::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return mSurfaceView->ScrollBy(x, y);
}

ECode CGeckoSurfaceView::Invalidate(
    /* [in] */ IRect * pDirty)
{
    return mSurfaceView->Invalidate(pDirty);
}

ECode CGeckoSurfaceView::Invalidate(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->Invalidate(left, top, right, bottom);
}

ECode CGeckoSurfaceView::Invalidate()
{
    return mSurfaceView->Invalidate();
}

ECode CGeckoSurfaceView::IsOpaque(
    /* [out] */ Boolean * pOpaque)
{
    return mSurfaceView->IsOpaque(pOpaque);
}

ECode CGeckoSurfaceView::GetHandler(
    /* [out] */ IHandler ** handler)
{
    return mSurfaceView->GetHandler(handler);
}

ECode CGeckoSurfaceView::PostInvalidate()
{
    return mSurfaceView->PostInvalidate();
}

ECode CGeckoSurfaceView::PostInvalidate(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->PostInvalidate(left, top, right, bottom);
}

ECode CGeckoSurfaceView::PostInvalidateDelayed(
    /* [in] */ Int64 delayMilliseconds)
{
    return mSurfaceView->PostInvalidateDelayed(delayMilliseconds);
}

ECode CGeckoSurfaceView::PostInvalidateDelayed(
    /* [in] */ Int64 delayMilliseconds,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->PostInvalidateDelayed(
                delayMilliseconds, left, top, right, bottom);
}

ECode CGeckoSurfaceView::PostInvalidateOnAnimation()
{
    return mSurfaceView->PostInvalidateOnAnimation();
}

ECode CGeckoSurfaceView::PostInvalidateOnAnimation(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->PostInvalidateOnAnimation(left, top, right, bottom);
}

ECode CGeckoSurfaceView::ComputeScroll()
{
    return mSurfaceView->ComputeScroll();
}

ECode CGeckoSurfaceView::IsHorizontalFadingEdgeEnabled(
    /* [out] */ Boolean * pHorizontalFadingEdgeEnabled)
{
    return mSurfaceView->IsHorizontalFadingEdgeEnabled(
                        pHorizontalFadingEdgeEnabled);
}

ECode CGeckoSurfaceView::SetHorizontalFadingEdgeEnabled(
    /* [in] */ Boolean horizontalFadingEdgeEnabled)
{
    return mSurfaceView->SetHorizontalFadingEdgeEnabled(
                        horizontalFadingEdgeEnabled);
}

ECode CGeckoSurfaceView::IsVerticalFadingEdgeEnabled(
    /* [out] */ Boolean * pVerticalFadingEdgeEnabled)
{
    return mSurfaceView->IsVerticalFadingEdgeEnabled(
                        pVerticalFadingEdgeEnabled);
}

ECode CGeckoSurfaceView::SetVerticalFadingEdgeEnabled(
    /* [in] */ Boolean verticalFadingEdgeEnabled)
{
    return mSurfaceView->SetVerticalFadingEdgeEnabled(
                        verticalFadingEdgeEnabled);
}

ECode CGeckoSurfaceView::IsHorizontalScrollBarEnabled(
    /* [out] */ Boolean * pHorizontalScrollBarEnabled)
{
    return mSurfaceView->IsHorizontalScrollBarEnabled(
                        pHorizontalScrollBarEnabled);
}

ECode CGeckoSurfaceView::SetHorizontalScrollBarEnabled(
    /* [in] */ Boolean horizontalScrollBarEnabled)
{
    return mSurfaceView->SetHorizontalScrollBarEnabled(
                        horizontalScrollBarEnabled);
}

ECode CGeckoSurfaceView::IsVerticalScrollBarEnabled(
    /* [out] */ Boolean * pVerticalScrollBarEnabled)
{
    return mSurfaceView->IsVerticalScrollBarEnabled(
                        pVerticalScrollBarEnabled);
}

ECode CGeckoSurfaceView::SetVerticalScrollBarEnabled(
    /* [in] */ Boolean verticalScrollBarEnabled)
{
    return mSurfaceView->SetVerticalScrollBarEnabled(
                        verticalScrollBarEnabled);
}

ECode CGeckoSurfaceView::SetScrollbarFadingEnabled(
    /* [in] */ Boolean fadeScrollbars)
{
    return mSurfaceView->SetScrollbarFadingEnabled(fadeScrollbars);
}

ECode CGeckoSurfaceView::IsScrollbarFadingEnabled(
    /* [out] */ Boolean * pFadeScrollbars)
{
    return mSurfaceView->IsScrollbarFadingEnabled(pFadeScrollbars);
}

ECode CGeckoSurfaceView::GetScrollBarDefaultDelayBeforeFade(
    /* [out] */ Int32* defaultDelay)
{
    return mSurfaceView->GetScrollBarDefaultDelayBeforeFade(defaultDelay);
}

ECode CGeckoSurfaceView::SetScrollBarDefaultDelayBeforeFade(
    /* [in] */ Int32 scrollBarDefaultDelayBeforeFade)
{
    return mSurfaceView->SetScrollBarDefaultDelayBeforeFade(scrollBarDefaultDelayBeforeFade);
}

ECode CGeckoSurfaceView::GetScrollBarFadeDuration(
    /* [out] */ Int32* duration)
{
    return mSurfaceView->GetScrollBarFadeDuration(duration);
}

ECode CGeckoSurfaceView::SetScrollBarFadeDuration(
    /* [in] */ Int32 scrollBarFadeDuration)
{
    return mSurfaceView->SetScrollBarFadeDuration(scrollBarFadeDuration);
}

ECode CGeckoSurfaceView::GetScrollBarSize(
    /* [out] */ Int32* size)
{
    return mSurfaceView->GetScrollBarSize(size);
}

ECode CGeckoSurfaceView::SetScrollBarSize(
    /* [in] */ Int32 scrollBarSize)
{
    return mSurfaceView->SetScrollBarSize(scrollBarSize);
}

ECode CGeckoSurfaceView::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    return mSurfaceView->SetScrollBarStyle(style);
}

ECode CGeckoSurfaceView::GetScrollBarStyle(
    /* [out] */ Int32 * pStyle)
{
    return mSurfaceView->GetScrollBarStyle(pStyle);
}

ECode CGeckoSurfaceView::CanScrollHorizontally(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* can)
{
    return mSurfaceView->CanScrollHorizontally(direction, can);
}

ECode CGeckoSurfaceView::CanScrollVertically(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* can)
{
    return mSurfaceView->CanScrollVertically(direction, can);
}

ECode CGeckoSurfaceView::ResolveRtlPropertiesIfNeeded()
{
    return mSurfaceView->ResolveRtlPropertiesIfNeeded();
}

ECode CGeckoSurfaceView::ResetRtlProperties()
{
    return mSurfaceView->ResetRtlProperties();
}

ECode CGeckoSurfaceView::OnScreenStateChanged(
    /* [in] */ Int32 screenState)
{
    return mSurfaceView->OnScreenStateChanged(screenState);
}

ECode CGeckoSurfaceView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    return mSurfaceView->OnRtlPropertiesChanged(layoutDirection);
}

ECode CGeckoSurfaceView::ResolveLayoutDirection(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->ResolveLayoutDirection(result);
}

ECode CGeckoSurfaceView::CanResolveLayoutDirection(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->CanResolveLayoutDirection(result);
}

ECode CGeckoSurfaceView::ResetResolvedLayoutDirection()
{
    return mSurfaceView->ResetResolvedLayoutDirection();
}

ECode CGeckoSurfaceView::IsLayoutDirectionInherited(
    /* [out] */ Boolean* result)
{
    return mSurfaceView->IsLayoutDirectionInherited(result);
}

ECode CGeckoSurfaceView::ResolvePadding()
{
    return mSurfaceView->ResolvePadding();
}

ECode CGeckoSurfaceView::ResetResolvedPadding()
{
    return mSurfaceView->ResetResolvedPadding();
}

ECode CGeckoSurfaceView::AssignParent(
    /* [in] */ IViewParent * pParent)
{
    return mSurfaceView->AssignParent(pParent);
}

ECode CGeckoSurfaceView::GetWindowToken(
    /* [out] */ IBinder ** ppToken)
{
    return mSurfaceView->GetWindowToken(ppToken);
}

ECode CGeckoSurfaceView::GetApplicationWindowToken(
    /* [out] */ IBinder ** ppToken)
{
    return mSurfaceView->GetApplicationWindowToken(ppToken);
}

ECode CGeckoSurfaceView::GetDisplay(
    /* [out] */ IDisplay** display)
{
    return mSurfaceView->GetDisplay(display);
}

ECode CGeckoSurfaceView::SaveHierarchyState(
    /* [in] */ IObjectInt32Map * pContainer)
{
    return mSurfaceView->SaveHierarchyState(pContainer);
}

ECode CGeckoSurfaceView::RestoreHierarchyState(
    /* [in] */ IObjectInt32Map * pContainer)
{
    return mSurfaceView->RestoreHierarchyState(pContainer);
}

ECode CGeckoSurfaceView::GetDrawingTime(
    /* [out] */ Int64 * pTime)
{
    return mSurfaceView->GetDrawingTime(pTime);
}

ECode CGeckoSurfaceView::SetDuplicateParentStateEnabled(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetDuplicateParentStateEnabled(enabled);
}

ECode CGeckoSurfaceView::IsDuplicateParentStateEnabled(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->IsDuplicateParentStateEnabled(pResult);
}

ECode CGeckoSurfaceView::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    return mSurfaceView->SetLayerType(layerType, paint);
}

ECode CGeckoSurfaceView::SetLayerPaint(
    /* [in] */ IPaint* paint)
{
    return mSurfaceView->SetLayerPaint(paint);
}

ECode CGeckoSurfaceView::GetLayerType(
    /* [out] */ Int32* type)
{
    return mSurfaceView->GetLayerType(type);
}

ECode CGeckoSurfaceView::BuildLayer()
{
    return mSurfaceView->BuildLayer();
}

ECode CGeckoSurfaceView::SetDrawingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    return mSurfaceView->SetDrawingCacheEnabled(enabled);
}

ECode CGeckoSurfaceView::IsDrawingCacheEnabled(
    /* [out] */ Boolean * pEnabled)
{
    return mSurfaceView->IsDrawingCacheEnabled(pEnabled);
}

ECode CGeckoSurfaceView::CanHaveDisplayList(
    /* [out] */ Boolean* can)
{
    return mSurfaceView->CanHaveDisplayList(can);
}

ECode CGeckoSurfaceView::GetDisplayList(
    /* [out] */ IDisplayList** list)
{
    return GetDisplayList(list);
}

ECode CGeckoSurfaceView::GetDrawingCache(
    /* [out] */ IBitmap ** ppBitmap)
{
    return mSurfaceView->GetDrawingCache(ppBitmap);
}

ECode CGeckoSurfaceView::GetDrawingCache(
    /* [in] */ Boolean autoScale,
    /* [out] */ IBitmap ** ppBitmap)
{
    return mSurfaceView->GetDrawingCache(autoScale, ppBitmap);
}

ECode CGeckoSurfaceView::DestroyDrawingCache()
{
    return mSurfaceView->DestroyDrawingCache();
}

ECode CGeckoSurfaceView::SetDrawingCacheBackgroundColor(
    /* [in] */ Int32 color)
{
    return mSurfaceView->SetDrawingCacheBackgroundColor(color);
}

ECode CGeckoSurfaceView::GetDrawingCacheBackgroundColor(
    /* [out] */ Int32 * pColor)
{
    return mSurfaceView->GetDrawingCacheBackgroundColor(pColor);
}

ECode CGeckoSurfaceView::BuildDrawingCache()
{
    return mSurfaceView->BuildDrawingCache();
}

ECode CGeckoSurfaceView::BuildDrawingCache(
    /* [in] */ Boolean autoScale)
{
    return mSurfaceView->BuildDrawingCache(autoScale);
}

ECode CGeckoSurfaceView::CreateSnapshot(
    /* [in] */ BitmapConfig quality,
    /* [in] */ Int32 backgroundColor,
    /* [in] */ Boolean skipChildren,
    /* [out] */ IBitmap ** ppBitmap)
{
    return mSurfaceView->CreateSnapshot(
        quality, backgroundColor, skipChildren, ppBitmap);
}

ECode CGeckoSurfaceView::IsInEditMode(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->IsInEditMode(pResult);
}

ECode CGeckoSurfaceView::IsHardwareAccelerated(
    /* [out] */ Boolean* attached)
{
    return mSurfaceView->IsHardwareAccelerated(attached);
}

ECode CGeckoSurfaceView::Draw(
    /* [in] */ ICanvas * pCanvas)
{
    return mSurfaceView->Draw(pCanvas);
}

ECode CGeckoSurfaceView::GetSolidColor(
    /* [out] */ Int32 * pColor)
{
    return mSurfaceView->GetSolidColor(pColor);
}

ECode CGeckoSurfaceView::OnFinishInflate()
{
    return mSurfaceView->OnFinishInflate();
}

ECode CGeckoSurfaceView::IsLayoutRequested(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->IsLayoutRequested(pResult);
}

ECode CGeckoSurfaceView::Layout(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    return mSurfaceView->Layout(l, t, r, b);
}

ECode CGeckoSurfaceView::GetResources(
    /* [out] */ IResources ** ppResources)
{
    return mSurfaceView->GetResources(ppResources);
}

ECode CGeckoSurfaceView::UnscheduleDrawable(
    /* [in] */ IDrawable * pWho)
{
    return mSurfaceView->UnscheduleDrawable(pWho);
}

ECode CGeckoSurfaceView::OnResolveDrawables(
    /* [in] */ Int64 layoutDirection)
{
    return mSurfaceView->OnResolveDrawables(layoutDirection);
}

ECode CGeckoSurfaceView::RefreshDrawableState()
{
    return mSurfaceView->RefreshDrawableState();
}

ECode CGeckoSurfaceView::GetDrawableState(
    /* [out, callee] */ ArrayOf<Int32> ** ppState)
{
    return mSurfaceView->GetDrawableState(ppState);
}

ECode CGeckoSurfaceView::JumpDrawablesToCurrentState()
{
    return mSurfaceView->JumpDrawablesToCurrentState();
}

ECode CGeckoSurfaceView::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    return mSurfaceView->SetBackgroundColor(color);
}

ECode CGeckoSurfaceView::SetBackgroundResource(
    /* [in] */ Int32 resid)
{
    return mSurfaceView->SetBackgroundResource(resid);
}

ECode CGeckoSurfaceView::SetBackground(
    /* [in] */ IDrawable* background)
{
    return mSurfaceView->SetBackground(background);
}

ECode CGeckoSurfaceView::SetBackgroundDrawable(
    /* [in] */ IDrawable * pD)
{
    return mSurfaceView->SetBackgroundDrawable(pD);
}

ECode CGeckoSurfaceView::GetBackground(
    /* [out] */ IDrawable ** ppDrawable)
{
    return mSurfaceView->GetBackground(ppDrawable);
}

ECode CGeckoSurfaceView::SetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->SetPadding(left, top, right, bottom);
}

ECode CGeckoSurfaceView::SetPaddingRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    return mSurfaceView->SetPaddingRelative(start, top, end, bottom);
}

ECode CGeckoSurfaceView::GetPaddingTop(
    /* [out] */ Int32 * pTop)
{
    return mSurfaceView->GetPaddingTop(pTop);
}

ECode CGeckoSurfaceView::GetPaddingBottom(
    /* [out] */ Int32 * pBottom)
{
    return mSurfaceView->GetPaddingBottom(pBottom);
}

ECode CGeckoSurfaceView::GetPaddingLeft(
    /* [out] */ Int32 * pLeft)
{
    return mSurfaceView->GetPaddingLeft(pLeft);
}

ECode CGeckoSurfaceView::GetPaddingStart(
    /* [out] */ Int32* start)
{
    return mSurfaceView->GetPaddingStart(start);
}

ECode CGeckoSurfaceView::GetPaddingRight(
    /* [out] */ Int32 * pRight)
{
    return mSurfaceView->GetPaddingRight(pRight);
}

ECode CGeckoSurfaceView::GetPaddingEnd(
    /* [out] */ Int32* end)
{
    return mSurfaceView->GetPaddingEnd(end);
}

ECode CGeckoSurfaceView::IsPaddingRelative(
    /* [out] */ Boolean* relative)
{
    return mSurfaceView->IsPaddingRelative(relative);
}

ECode CGeckoSurfaceView::ResetPaddingToInitialValues()
{
    return mSurfaceView->ResetPaddingToInitialValues();
}

ECode CGeckoSurfaceView::GetOpticalInsets(
    /* [out] */ IInsets** sets)
{
    return mSurfaceView->GetOpticalInsets(sets);
}

ECode CGeckoSurfaceView::SetLayoutInsets(
    /* [in] */ IInsets* layoutInsets)
{
    return mSurfaceView->SetLayoutInsets(layoutInsets);
}

ECode CGeckoSurfaceView::SetSelected(
    /* [in] */ Boolean selected)
{
    return mSurfaceView->SetSelected(selected);
}

ECode CGeckoSurfaceView::IsSelected(
    /* [out] */ Boolean * pSelected)
{
    return mSurfaceView->IsSelected(pSelected);
}

ECode CGeckoSurfaceView::SetActivated(
    /* [in] */ Boolean activated)
{
    return mSurfaceView->SetActivated(activated);
}

ECode CGeckoSurfaceView::IsActivated(
    /* [out] */ Boolean* activated)
{
    return mSurfaceView->IsActivated(activated);
}

ECode CGeckoSurfaceView::GetViewTreeObserver(
    /* [out] */ IViewTreeObserver ** ppObserver)
{
    return mSurfaceView->GetViewTreeObserver(ppObserver);
}

ECode CGeckoSurfaceView::GetRootView(
    /* [out] */ IView ** ppView)
{
    return mSurfaceView->GetRootView(ppView);
}

ECode CGeckoSurfaceView::GetLocationOnScreen(
    /* [out] */ Int32 * pX,
    /* [out] */ Int32 * pY)
{
    return mSurfaceView->GetLocationOnScreen(pX, pY);
}

ECode CGeckoSurfaceView::GetLocationInWindow(
    /* [out] */ Int32 * pX,
    /* [out] */ Int32 * pY)
{
    return mSurfaceView->GetLocationInWindow(pX, pY);
}

ECode CGeckoSurfaceView::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView ** ppView)
{
    return mSurfaceView->FindViewById(id, ppView);
}

ECode CGeckoSurfaceView::FindViewByPredicate(
    /* [in] */ IPredicate* predicate,
    /* [out] */ IView** view)
{
    return mSurfaceView->FindViewByPredicate(predicate, view);
}

ECode CGeckoSurfaceView::FindViewByPredicateInsideOut(
    /* [in] */ IView* start,
    /* [in] */ IPredicate* predicate,
    /* [out] */ IView** view)
{
    return mSurfaceView->FindViewByPredicateInsideOut(start, predicate, view);
}

ECode CGeckoSurfaceView::SetId(
    /* [in] */ Int32 id)
{
    return mSurfaceView->SetId(id);
}

ECode CGeckoSurfaceView::GetId(
    /* [out] */ Int32 * pId)
{
    return mSurfaceView->GetId(pId);
}

ECode CGeckoSurfaceView::GetTag(
    /* [out] */ IInterface** tag)
{
    return mSurfaceView->GetTag(tag);
}

ECode CGeckoSurfaceView::SetTag(
    /* [in] */ IInterface* tag)
{
    return mSurfaceView->SetTag(tag);
}

ECode CGeckoSurfaceView::GetTag(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** tag)
{
    return mSurfaceView->GetTag(key, tag);
}

ECode CGeckoSurfaceView::SetTag(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* tag)
{
    return mSurfaceView->SetTag(key, tag);
}

ECode CGeckoSurfaceView::HackTurnOffWindowResizeAnim(
    /* [in] */ Boolean off)
{
    return mSurfaceView->HackTurnOffWindowResizeAnim(off);
}

ECode CGeckoSurfaceView::Animate(
    /* [out] */ IViewPropertyAnimator** animator)
{
    return mSurfaceView->Animate(animator);
}

ECode CGeckoSurfaceView::GetInflaterContext(
    /* [out] */ IContext** context)
{
    return mSurfaceView->GetInflaterContext(context);
}

ECode CGeckoSurfaceView::SetInflaterContext(
    /* [in] */ IContext* context)
{
    return mSurfaceView->SetInflaterContext(context);
}

ECode CGeckoSurfaceView::GetXmlPath(
    /* [out] */ String* path)
{
    return mSurfaceView->GetXmlPath(path);
}

ECode CGeckoSurfaceView::SetXmlPath(
    /* [in] */ const String& path)
{
    return mSurfaceView->SetXmlPath(path);
}

ECode CGeckoSurfaceView::IsRootNamespace(
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->IsRootNamespace(pResult);
}

ECode CGeckoSurfaceView::GetBaseline(
    /* [out] */ Int32 * pResult)
{
    return mSurfaceView->GetBaseline(pResult);
}

ECode CGeckoSurfaceView::RequestLayout()
{
    return mSurfaceView->RequestLayout();
}

ECode CGeckoSurfaceView::ForceLayout()
{
    return mSurfaceView->ForceLayout();
}

ECode CGeckoSurfaceView::Measure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    return mSurfaceView->Measure(widthMeasureSpec, heightMeasureSpec);
}

ECode CGeckoSurfaceView::GetMinimumHeight(
    /* [out] */ Int32* height)
{
    return mSurfaceView->GetMinimumHeight(height);
}

ECode CGeckoSurfaceView::SetMinimumHeight(
    /* [in] */ Int32 minHeight)
{
    return mSurfaceView->SetMinimumHeight(minHeight);
}

ECode CGeckoSurfaceView::GetMinimumWidth(
    /* [out] */ Int32* width)
{
    return mSurfaceView->GetMinimumWidth(width);
}

ECode CGeckoSurfaceView::SetMinimumWidth(
    /* [in] */ Int32 minWidth)
{
    return mSurfaceView->SetMinimumWidth(minWidth);
}

ECode CGeckoSurfaceView::GetAnimation(
    /* [out] */ IAnimation ** ppAnimation)
{
    return mSurfaceView->GetAnimation(ppAnimation);
}

ECode CGeckoSurfaceView::StartAnimation(
    /* [in] */ IAnimation * pAnimation)
{
    return mSurfaceView->StartAnimation(pAnimation);
}

ECode CGeckoSurfaceView::ClearAnimation()
{
    return mSurfaceView->ClearAnimation();
}

ECode CGeckoSurfaceView::SetAnimation(
    /* [in] */ IAnimation * pAnimation)
{
    return mSurfaceView->SetAnimation(pAnimation);
}

ECode CGeckoSurfaceView::GatherTransparentRegion(
    /* [in] */ IRegion * pRegion,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->GatherTransparentRegion(pRegion, pResult);
}

ECode CGeckoSurfaceView::PlaySoundEffect(
    /* [in] */ Int32 soundConstant)
{
    return mSurfaceView->PlaySoundEffect(soundConstant);
}

ECode CGeckoSurfaceView::PerformHapticFeedback(
    /* [in] */ Int32 feedbackConstant,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->PerformHapticFeedback(feedbackConstant, pResult);
}

ECode CGeckoSurfaceView::PerformHapticFeedback(
    /* [in] */ Int32 feedbackConstant,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean * pResult)
{
    return mSurfaceView->PerformHapticFeedback(
                        feedbackConstant, flags, pResult);
}

ECode CGeckoSurfaceView::SetSystemUiVisibility(
    /* [in] */ Int32 visibility)
{
    return mSurfaceView->SetSystemUiVisibility(visibility);
}

ECode CGeckoSurfaceView::GetSystemUiVisibility(
    /* [out] */ Int32* visibility)
{
    return mSurfaceView->GetSystemUiVisibility(visibility);
}

ECode CGeckoSurfaceView::GetWindowSystemUiVisibility(
    /* [out] */ Int32* visibility)
{
    return mSurfaceView->GetWindowSystemUiVisibility(visibility);
}

ECode CGeckoSurfaceView::OnWindowSystemUiVisibilityChanged(
    /* [in] */ Int32 visible)
{
    return mSurfaceView->OnWindowSystemUiVisibilityChanged(visible);
}

ECode CGeckoSurfaceView::DispatchWindowSystemUiVisiblityChanged(
    /* [in] */ Int32 visible)
{
    return mSurfaceView->DispatchWindowSystemUiVisiblityChanged(visible);
}

ECode CGeckoSurfaceView::SetOnSystemUiVisibilityChangeListener(
    /* [in] */ IViewOnSystemUiVisibilityChangeListener* l)
{
    return mSurfaceView->SetOnSystemUiVisibilityChangeListener(l);
}

ECode CGeckoSurfaceView::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    return mSurfaceView->DispatchSystemUiVisibilityChanged(visibility);
}

ECode CGeckoSurfaceView::SetDisabledSystemUiVisibility(
    /* [in] */ Int32 flags)
{
    return mSurfaceView->SetDisabledSystemUiVisibility(flags);
}

ECode CGeckoSurfaceView::StartDrag(
    /* [in] */ IClipData* data,
    /* [in] */ IDragShadowBuilder* shadowBuilder,
    /* [in] */ IInterface* myLocalState,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* success)
{
    return mSurfaceView->StartDrag(data, shadowBuilder, myLocalState, flags, success);
}

ECode CGeckoSurfaceView::OnDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* consumed)
{
    return mSurfaceView->OnDragEvent(event, consumed);
}

ECode CGeckoSurfaceView::DispatchDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* consumed)
{
    return mSurfaceView->DispatchDragEvent(event, consumed);
}

ECode CGeckoSurfaceView::OnCloseSystemDialogs(
    /* [in] */ const String& reason)
{
    return mSurfaceView->OnCloseSystemDialogs(reason);
}

ECode CGeckoSurfaceView::ApplyDrawableToTransparentRegion(
    /* [in] */ IDrawable * pDr,
    /* [in] */ IRegion * pRegion)
{
    return mSurfaceView->ApplyDrawableToTransparentRegion(pDr, pRegion);
}

ECode CGeckoSurfaceView::GetOverScrollMode(
    /* [out] */ Int32 * pMode)
{
    return mSurfaceView->GetOverScrollMode(pMode);
}

ECode CGeckoSurfaceView::SetOverScrollMode(
    /* [in] */ Int32 overScrollMode)
{
    return mSurfaceView->SetOverScrollMode(overScrollMode);
}

ECode CGeckoSurfaceView::GetRawTextDirection(
    /* [out] */ Int32* direction)
{
    return mSurfaceView->GetRawTextDirection(direction);
}

ECode CGeckoSurfaceView::SetTextDirection(
    /* [in] */ Int32 textDirection)
{
    return mSurfaceView->SetTextDirection(textDirection);
}

ECode CGeckoSurfaceView::GetTextDirection(
    /* [out] */ Int32* direction)
{
    return mSurfaceView->GetTextDirection(direction);
}

ECode CGeckoSurfaceView::ResolveTextDirection(
    /* [out] */ Boolean* done)
{
    return mSurfaceView->ResolveTextDirection(done);
}

ECode CGeckoSurfaceView::ResetResolvedTextDirection()
{
    return mSurfaceView->ResetResolvedTextDirection();
}

ECode CGeckoSurfaceView::IsTextDirectionInherited(
    /* [out] */ Boolean* inherited)
{
    return mSurfaceView->IsTextDirectionInherited(inherited);
}

ECode CGeckoSurfaceView::GetRawTextAlignment(
    /* [out] */ Int32* alignment)
{
    return mSurfaceView->GetRawTextAlignment(alignment);
}

ECode CGeckoSurfaceView::SetTextAlignment(
    /* [in] */ Int32 textAlignment)
{
    return mSurfaceView->SetTextAlignment(textAlignment);
}

ECode CGeckoSurfaceView::GetTextAlignment(
    /* [out] */ Int32* alignment)
{
    return mSurfaceView->GetTextAlignment(alignment);
}

ECode CGeckoSurfaceView::ResolveTextAlignment(
    /* [out] */ Boolean* done)
{
    return mSurfaceView->ResolveTextAlignment(done);
}

ECode CGeckoSurfaceView::ResetResolvedTextAlignment()
{
    return mSurfaceView->ResetResolvedTextAlignment();
}

ECode CGeckoSurfaceView::IsTextAlignmentInherited(
    /* [out] */ Boolean* inherited)
{
    return mSurfaceView->IsTextAlignmentInherited(inherited);
}

ECode CGeckoSurfaceView::GetHolder(
    /* [out] */ ISurfaceHolder ** ppHolder)
{
    return mSurfaceView->GetHolder(ppHolder);
}

ECode CGeckoSurfaceView::SetZOrderMediaOverlay(
    /* [in] */ Boolean isMediaOverlay)
{
    return mSurfaceView->SetZOrderMediaOverlay(isMediaOverlay);
}

ECode CGeckoSurfaceView::SetZOrderOnTop(
    /* [in] */ Boolean onTop)
{
    return mSurfaceView->SetZOrderOnTop(onTop);
}

ECode CGeckoSurfaceView::SetSecure(
    /* [in] */ Boolean isSecure)
{
    return mSurfaceView->SetSecure(isSecure);
}

ECode CGeckoSurfaceView::SetWindowType(
    /* [in] */ Int32 type)
{
    return mSurfaceView->SetWindowType(type);
}

ECode CGeckoSurfaceView::IsFixedSize(
    /* [out] */ Boolean * pSize)
{
    return mSurfaceView->IsFixedSize(pSize);
}

ECode CGeckoSurfaceView::SetCreateInputConnectionDelegate(
    /* [in] */ IView* view)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CGeckoSurfaceView::SetKeyEventCallbackDelegate(
    /* [in] */IKeyEventCallback* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
