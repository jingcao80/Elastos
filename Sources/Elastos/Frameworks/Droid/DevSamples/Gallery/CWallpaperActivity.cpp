
#include "CWallpaperActivity.h"
#include "src/data/DataSourceHelper.h"
#include "src/data/AsyncImageLoader.h"
#include "src/util/Utils.h"
#include "elastos/droid/os/SomeArgs.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::App::CWallpaperManagerHelper;
using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::App::IWallpaperManagerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CWallpaperActivity::TAG("CWallpaperActivity");
const Int32 CWallpaperActivity::MSG_IMAGE_LOADED = 0;

ECode CWallpaperActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CWallpaperActivity* mHost = (CWallpaperActivity*)strongObj.Get();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CWallpaperActivity::MSG_IMAGE_LOADED: {
            SomeArgs* args = (SomeArgs*)obj.Get();
            IDrawable* drawable = IDrawable::Probe(args->mArg1);
            IImageView* imageView = IImageView::Probe(args->mArg2);
            mHost->MyImageLoaded(drawable, imageView);
            args->Recycle();
            break;
        }
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CWallpaperActivity::MyListener, IViewOnClickListener)

CWallpaperActivity::MyListener::MyListener(
    /* [in] */ CWallpaperActivity* host)
    : mHost(host)
{}

ECode CWallpaperActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::wallpaper_back: {
            Logger::D(TAG, "OnClick()---wallpaper_back");
            mHost->OnBackPressed();
            break;
        }
        case R::id::wallpaper_ok: {
            Logger::D(TAG, "OnClick()---wallpaper_ok");
            mHost->SetWallpaper();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

CWallpaperActivity::MyLoadImageCallback::MyLoadImageCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CWallpaperActivity::MyLoadImageCallback::ImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "ImageLoaded()-----");

    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CWallpaperActivity* mHost = (CWallpaperActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = imageDrawable;
    args->mArg2 = imageView;

    AutoPtr<IMessage> msg;
    mHost->mMyHandler->ObtainMessage(CWallpaperActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    Boolean result;
    return mHost->mMyHandler->SendMessage(msg, &result);
}

CWallpaperActivity::CWallpaperActivity()
{
}

ECode CWallpaperActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()---");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_wallpaper);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mMyHandler = new MyHandler(weakHost);

    mListener = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::wallpaper_back);
    mBackButton = IImageView::Probe(view);
    assert(mBackButton != NULL);
    view = FindViewById(R::id::wallpaper_photo);
    mWallpaperView = IImageView::Probe(view);
    assert(mWallpaperView != NULL);
    mBackButton->SetOnClickListener((IViewOnClickListener*)(mListener->Probe(EIID_IViewOnClickListener)));
    view = FindViewById(R::id::wallpaper_ok);
    AutoPtr<ITextView> okButton = ITextView::Probe(view);
    assert(okButton != NULL);
    okButton->SetOnClickListener((IViewOnClickListener*)(mListener->Probe(EIID_IViewOnClickListener)));

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &mFilePath);
        if (!mFilePath.IsNullOrEmpty()) {
            Logger::D(TAG, "OnCreate()---mFilePath:", mFilePath.string());
            AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
            AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(mFilePath, TRUE, mWallpaperView, myLoadImage);
            if (drawable != NULL) {
                mWallpaperView->SetImageDrawable(IDrawable::Probe(drawable));
            }
            else {
                mWallpaperView->SetImageResource(R::color::main_layout_bg);
            }
        }
        else {
            Logger::E(TAG, "mFilePath is null or empty!");
        }
    }

    return NOERROR;
}

ECode CWallpaperActivity::OnStart()
{
    Logger::D(TAG, "OnStart()---");
    Activity::OnStart();
    return NOERROR;
}

ECode CWallpaperActivity::OnResume()
{
    Logger::D(TAG, "OnResume()---");
    Activity::OnResume();
    return NOERROR;
}

ECode CWallpaperActivity::OnPause()
{
    Logger::D(TAG, "OnPause()---");
    Activity::OnPause();
    return NOERROR;
}

ECode CWallpaperActivity::OnStop()
{
    Logger::D(TAG, "OnStop()---");
    Activity::OnStop();
    return NOERROR;
}

ECode CWallpaperActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()---");
    Activity::OnDestroy();
    return NOERROR;
}

void CWallpaperActivity::MyImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "MyImageLoaded()-----");
    IBitmapDrawable* drawable = IBitmapDrawable::Probe(imageDrawable);
    imageView->SetImageDrawable(drawable);
}

void CWallpaperActivity::SetWallpaper()
{
    Logger::D(TAG, "SetWallpaper()-----");
    AutoPtr<IDrawable> drawable;
    mWallpaperView->GetDrawable((IDrawable**)&drawable);
    IBitmapDrawable* bitmapDrawable = IBitmapDrawable::Probe(drawable);
    Boolean succeed = FALSE;
    ECode ec = NOERROR;
    if (bitmapDrawable) {
        AutoPtr<IWallpaperManagerHelper> wallpaperHelper;
        AutoPtr<IWallpaperManager> wallpaperMgr;
        CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&wallpaperHelper);
        assert(wallpaperHelper != NULL);
        wallpaperHelper->GetInstance(this, (IWallpaperManager**)&wallpaperMgr);
        assert(wallpaperMgr != NULL);
        AutoPtr<IBitmap> bitmap;
        bitmapDrawable->GetBitmap((IBitmap**)&bitmap);
        ec = wallpaperMgr->SetBitmap(bitmap);
        succeed = (ec == NOERROR);
    }

    if (!succeed) {
        Logger::W(TAG, "SetWallpaper()----failed---ECode:%d", ec);
        Utils::MakeToast(this, String("壁纸设置失败！"), IToast::LENGTH_LONG);
    }
    else {
        Utils::MakeToast(this, String("壁纸设置成功！"), IToast::LENGTH_LONG);
    }
}

} // namespace Gallery
