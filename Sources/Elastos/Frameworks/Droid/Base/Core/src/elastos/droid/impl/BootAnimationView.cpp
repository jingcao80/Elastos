
#include "elastos/droid/impl/BootAnimationView.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::SystemProperties;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::EIID_IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

const Boolean BootAnimationView::DEBUG = FALSE;
const String BootAnimationView::TAG("BootAnimationView");
const String BootAnimationView::BOOT_LOGO_PATH = String("/system/media/bootlogo.bmp");
const String BootAnimationView::INIT_LOGO_PATH = String("/system/media/initlogo.bmp");

const Int32 BootAnimationView::nsize = 11;

const Int32 BootAnimationView::FRAME_RATE = 300;
const Int32 BootAnimationView::MSG_UPDATE_FRAME = 0;
const Int32 BootAnimationView::MSG_UPDATE = 1;


BootAnimationView::BootHandler::BootHandler(
    /* [in] */ BootAnimationView* host)
    : mHost(host)
{
}

ECode BootAnimationView::BootHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case BootAnimationView::MSG_UPDATE_FRAME:
            {
                if (mHost->cframe >= mHost->nsize) {
                    mHost->cframe = mHost->sframe;
                }
                mHost->DrawBatteryFrame();
                mHost->cframe++;
                mHost->mSlef->Invalidate();
                AutoPtr<IMessage> message;
                ObtainMessage(BootAnimationView::MSG_UPDATE_FRAME, (IMessage**)&message);
                Boolean isSuccess = FALSE;
                SendMessageDelayed(message,
                        BootAnimationView::FRAME_RATE, &isSuccess);
                break;
            }
        case BootAnimationView::MSG_UPDATE:
            mHost->mSlef->Invalidate();
            break;

    }
    return NOERROR;
}

AutoPtr<IBitmap> BootAnimationView::GetImageFromAssetsFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& filepath)
{
    AutoPtr<IBitmap> image;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IAssetManager> am;
    resources->GetAssets((IAssetManager**)&am);
    // try{
    AutoPtr<IInputStream> is;
    am->Open(filepath, (IInputStream**)&is);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    factory->DecodeStream(is, (IBitmap**)&image);
    is->Close();
    // }catch (IOException e){
        // e.printStackTrace();
    // }
    return image;
}

AutoPtr<IBitmap> BootAnimationView::GetImageFromSystemPath(
    /* [in] */ IContext* context,
    /* [in] */ const String& filepath)
{
    AutoPtr<IBitmap> image;
    AutoPtr<IFile> fp;
    CFile::New(filepath, (IFile**)&fp);
    // try{
    AutoPtr<IFileInputStream> is;
    CFileInputStream::New(fp, (IFileInputStream**)&is);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    factory->DecodeStream(is, (IBitmap**)&image);
    is->Close();
    // }catch (IOException e){
        // e.printStackTrace();
    // }
    return image;
}

BootAnimationView::BootAnimationView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : View(context, attrs)
    , cframe(0)
    , sframe(0)
    , drawWhat(0)
{
    // Init global variable.
    mHandler = new BootHandler(this);

    mSlef = this;
    mBat = ArrayOf< AutoPtr<IBitmap> >::Alloc(nsize);
    mBootLogo = GetImageFromSystemPath(context, BOOT_LOGO_PATH);
    mInitLogo = GetImageFromSystemPath(context, INIT_LOGO_PATH);
    for (int i = 0; i < mBat->GetLength(); i ++) {
        (*mBat)[i] = GetImageFromAssetsFile(context, String("images/bat") + StringUtils::Int32ToString(i) + String(".bmp"));
    }
    Slogger::D(TAG, String("BootAnimationView init"));
    (*mBat)[0]->GetWidth(&mW);
    (*mBat)[0]->GetHeight(&mH);
    InitMemCanvas();
}

CAR_INTERFACE_IMPL(BootAnimationView, IView)

IVIEW_METHODS_IMPL(BootAnimationView, View)

void BootAnimationView::StartShowBatteryCharge(
    /* [in] */ Int32 percent)
{
    Slogger::D(TAG, "startShowBatteryCharge percent = %d", percent);
    if (percent <= 100 && percent > 0) {
        sframe = percent/10;
    } else {
        sframe = 0;
    }
    cframe = sframe;
    if(DEBUG)
        Slogger::D(TAG, "cframe = %d", cframe);
    drawWhat = 1;
    mHandler->RemoveMessages(MSG_UPDATE_FRAME);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_UPDATE_FRAME, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessage(message, &isSuccess);
}

AutoPtr<IBitmap> BootAnimationView::ZoomBitmap(
    /* [in] */ IBitmap* bmp,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    Int32 width = 0;
    bmp->GetWidth(&width);
    Int32 height= 0;
    bmp->GetHeight(&height);
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    Float scaleWidth = ((Float)w / width);
    Float scaleHeight = ((Float)h / height);
    Boolean isSuccess = FALSE;
    matrix->PostScale(scaleWidth, scaleHeight, &isSuccess);
    AutoPtr<IBitmap> image;
    CBitmap::CreateBitmap(bmp, 0, 0, width, height, matrix, TRUE, (IBitmap**)&image);
    return image;
}

void BootAnimationView::DrawBatteryFrame()
{
    mMemCanvas->DrawBitmap((*mBat)[cframe], 0, 0, mMemPaint);
}

void BootAnimationView::HideScreen(
    /* [in] */ Boolean enable)
{
    if (enable) {
        drawWhat = 2;
    } else {
        drawWhat = 0;
    }

    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_UPDATE, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessage(message, &isSuccess);
}

void BootAnimationView::ShowBootInitLogo(
    /* [in] */ Int32 logo)
{
    drawWhat = 3;
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_UPDATE, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessage(message, &isSuccess);
}

void BootAnimationView::ShowInitLogo()
{
    drawWhat = 5;
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_UPDATE, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessage(message, &isSuccess);
}

void BootAnimationView::HideBootInitLogo()
{
    drawWhat = 0;
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_UPDATE, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    mHandler->SendMessage(message, &isSuccess);
}

void BootAnimationView::InitMemCanvas()
{
    Slogger::D(TAG, "initMemCanvas");
    CBitmap::CreateBitmap(mW, mH, BitmapConfig_ARGB_8888, (IBitmap**)&mMemBitmap);
    CCanvas::New((ICanvas**)&mMemCanvas);
    mMemCanvas->SetBitmap(mMemBitmap);
    CPaint::New((IPaint**)&mMemPaint);
    mMemPaint->SetColor(IColor::BLACK);
    mMemPaint->SetStyle(PaintStyle_FILL);
}

void BootAnimationView::ShowBootAnimation()
{
    if (DEBUG) {
        Slogger::D(TAG, "showBootAnimation");
    }
    SystemProperties::Set(String("service.bootanim.exit"), String("0"));
    SystemProperties::Set(String("ctl.start"), String("bootanim"));
}

void BootAnimationView::HideBootAnimation()
{
    if (DEBUG) {
        Slogger::D(TAG, "hideBootAnimation");
    }
    SystemProperties::Set(String("service.bootanim.exit"), String("1"));
    SystemProperties::Set(String("ctl.stop"), String("bootanim"));
}

void BootAnimationView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);
    switch (drawWhat) {
    case 1:
        {
            Int32 width = 0;
            Int32 height = 0;
            canvas->GetWidth(&width);
            canvas->GetHeight(&height);
            canvas->DrawRect(0, 0, width, height, mMemPaint);
            canvas->DrawBitmap(mMemBitmap, width/2 - mW/2, height/2 - mH/2, mMemPaint);
            break;
        }
    case 5:
        {
            canvas->DrawBitmap(mInitLogo, 0, 0, mMemPaint);
            break;
        }
    case 2:
        {
            Int32 width = 0;
            Int32 height = 0;
            canvas->GetWidth(&width);
            canvas->GetHeight(&height);
            canvas->DrawRect(0, 0, width, height, mMemPaint);
            break;
        }
    case 3:
        {
            Int32 width = 0;
            Int32 height = 0;
            Int32 bootLogoWidth = 0;
            Int32 bootLogoHeight = 0;
            canvas->GetWidth(&width);
            canvas->GetHeight(&height);
            mBootLogo->GetWidth(&bootLogoWidth);
            mBootLogo->GetHeight(&bootLogoHeight);
            canvas->DrawRect(0, 0, width, height, mMemPaint);
            canvas->DrawBitmap(mBootLogo, width/2 - bootLogoWidth/2, height/2 - bootLogoHeight/2, mMemPaint);
            break;
        }
    }
}

} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl
