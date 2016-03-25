
#include "CEditActivity.h"
#include "src/data/DataSourceHelper.h"
#include "src/data/AsyncImageLoader.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/SomeArgs.h"

using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CEditActivity::TAG("CEditActivity");
const Int32 CEditActivity::MSG_IMAGE_LOADED = 0;

ECode CEditActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CEditActivity* mHost = (CEditActivity*)strongObj.Get();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CEditActivity::MSG_IMAGE_LOADED: {
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

CEditActivity::MyListener::MyListener(
    /* [in] */ CEditActivity* host)
    : mHost(host)
{}

PInterface CEditActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CEditActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEditActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CEditActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CEditActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::edit_back: {
            Logger::D(TAG, "OnClick()---edit_back");
            mHost->OnBackPressed();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

CEditActivity::MyLoadImageCallback::MyLoadImageCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CEditActivity::MyLoadImageCallback::ImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "ImageLoaded()-----");
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CEditActivity* mHost = (CEditActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = imageDrawable;
    args->mArg2 = imageView;

    AutoPtr<IMessage> msg;
    mHost->mMyHandler->ObtainMessage(CEditActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    Boolean result;
    return mHost->mMyHandler->SendMessage(msg, &result);
}

CEditActivity::CEditActivity()
{
}

ECode CEditActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()---");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_edit);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mMyHandler = new MyHandler(weakHost);

    AutoPtr<MyListener> l = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::edit_back);
    mBackButton = IImageView::Probe(view);
    assert(mBackButton != NULL);
    view = FindViewById(R::id::edit_photo);
    mEditView = IImageView::Probe(view);
    assert(mEditView != NULL);
    mBackButton->SetOnClickListener(l.Get());

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &mFilePath);
        if (!mFilePath.IsNullOrEmpty()) {
            Logger::D(TAG, "OnCreate()---mFilePath:", mFilePath.string());
            AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
            AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(mFilePath, TRUE, mEditView, myLoadImage);
            if (drawable != NULL) {
                mEditView->SetImageDrawable(IDrawable::Probe(drawable));
            }
            else {
                mEditView->SetImageResource(R::color::main_layout_bg);
            }
        }
        else {
            Logger::E(TAG, "mFilePath is null or empty!");
        }
    }

    return NOERROR;
}

ECode CEditActivity::OnStart()
{
    Logger::D(TAG, "OnStart()---");
    Activity::OnStart();
    return NOERROR;
}

ECode CEditActivity::OnResume()
{
    Logger::D(TAG, "OnResume()---");
    Activity::OnResume();
    return NOERROR;
}

ECode CEditActivity::OnPause()
{
    Logger::D(TAG, "OnPause()---");
    Activity::OnPause();
    return NOERROR;
}

ECode CEditActivity::OnStop()
{
    Logger::D(TAG, "OnStop()---");
    Activity::OnStop();
    return NOERROR;
}

ECode CEditActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()---");
    Activity::OnDestroy();
    return NOERROR;
}

void CEditActivity::MyImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "MyImageLoaded()-----");
    imageView->SetImageDrawable(imageDrawable);
}

} // namespace Gallery
