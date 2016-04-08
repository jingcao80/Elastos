
#include "CBrowserActivity.h"
#include "src/data/DataSourceHelper.h"
#include "src/data/AsyncImageLoader.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/os/SomeArgs.h"

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;

using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::EIID_ISimpleAdapterViewBinder;
using Elastos::Droid::Widget::IRelativeLayout;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Utility::CParcelableObjectContainer;

using Elastos::Utility::CObjectStringMap;
using Elastos::Utility::IObjectStringMap;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CBrowserActivity::TAG("CBrowserActivity");
const Int32 CBrowserActivity::MSG_IMAGE_LOADED = 0;

ECode CBrowserActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CBrowserActivity* mHost = (CBrowserActivity*)strongObj.Get();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CBrowserActivity::MSG_IMAGE_LOADED: {
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

CBrowserActivity::MyListener::MyListener(
    /* [in] */ CBrowserActivity* host)
    : mHost(host)
{}

PInterface CBrowserActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_IPopupWindowOnDismissListener) {
        return (IPopupWindowOnDismissListener*)this;
    }
    else if (riid == EIID_IViewOnTouchListener) {
        return (IViewOnTouchListener*)this;
    }
    else if (riid == EIID_ICompoundButtonOnCheckedChangeListener) {
        return (ICompoundButtonOnCheckedChangeListener*)this;
    }

    return NULL;
}

UInt32 CBrowserActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CBrowserActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CBrowserActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else if (pObject == (IInterface*)(IPopupWindowOnDismissListener*)this) {
        *pIID = EIID_IPopupWindowOnDismissListener;
    }
    else if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
        *pIID = EIID_IViewOnTouchListener;
    }
    else if (pObject == (IInterface*)(ICompoundButtonOnCheckedChangeListener*)this) {
        *pIID = EIID_ICompoundButtonOnCheckedChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CBrowserActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::pic_back: {
            Logger::D(TAG, "OnClick()---pic_back");
            mHost->OnBackPressed();
            break;
        }
        case R::id::pic_btn_wallpaper: {
            Logger::D(TAG, "OnClick()---pic_btn_wallpaper");
            break;
        }
        case R::id::pic_btn_detail: {
            Logger::D(TAG, "OnClick()---pic_btn_detail");
            break;
        }
        case R::id::pic_btn_share: {
            Logger::D(TAG, "OnClick()---pic_btn_share");
            break;
        }
        case R::id::pic_btn_popup: {
            Logger::D(TAG, "OnClick()---pic_btn_popup");
            if (mHost->mShowPicPop) {
                mHost->ClosePicPopupWindow();
            }
            else {
                mHost->OpenPicPopupWindow(v);
            }
            break;
        }
        case R::id::pic_pop_time: {
            Logger::D(TAG, "OnClick()---pic_pop_time");
            mHost->ClosePicPopupWindow();
            break;
        }
        case R::id::pic_pop_location: {
            Logger::D(TAG, "OnClick()---pic_pop_location");
            mHost->ClosePicPopupWindow();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

ECode CBrowserActivity::MyListener::OnDismiss()
{
    Logger::D(TAG, "OnDismiss()---");
    mHost->mShowPicPop = FALSE;
    return NOERROR;
}

ECode CBrowserActivity::MyListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    AutoPtr<IView> view;
    event->GetAction(&action);
    v->FindViewById(R::id::pic_thumb_overlay, (IView**)&view);
    AutoPtr<IImageView> overlayView = IImageView::Probe(view);
    if (IMotionEvent::ACTION_DOWN == action) {
        Logger::D(TAG, "onTouch()--ACTION_DOWN");
        overlayView->SetVisibility(IView::VISIBLE);
    }
    else {
        Logger::D(TAG, "onTouch()--ACTION:%d", action);
        overlayView->SetVisibility(IView::GONE);
        AutoPtr<IViewParent> viewParent;
        AutoPtr<IViewParent> viewParent2;
        v->GetParent((IViewParent**)&viewParent);
        viewParent->GetParent((IViewParent**)&viewParent2);
        AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(viewParent2);
        Int32 index = 0;
        adapterView->GetPositionForView(v, &index);
        Logger::D(TAG, "onTouch()--index:%d", index);
        if (IMotionEvent::ACTION_UP == action) {
            AutoPtr<PictureEntry> entry = mHost->mPictureEntryList[index];
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("Gallery"), String("Gallery.CPhotoActivity"));
            intent->PutExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
            intent->PutExtra(DataSourceHelper::SOURCE_INDEX, index);
            if (FAILED(mHost->StartActivity(intent))) {
                Logger::E(TAG, "onTouch()--StartActivity CPhotoActivity failed!");
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CBrowserActivity::MyListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    Logger::D(TAG, "OnCheckedChanged()--isChecked:%d", isChecked);
    return NOERROR;
}

CBrowserActivity::MyViewBinder::MyViewBinder(
    /* [in] */ CBrowserActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CBrowserActivity::MyViewBinder, ISimpleAdapterViewBinder)

ECode CBrowserActivity::MyViewBinder::SetViewValue(
    /* [in] */ IView* view,
    /* [in] */ IInterface* data,
    /* [in] */ const String& textRepresentation,
    /* [out] */ Boolean* result)
{
    // Logger::D(TAG, "MyViewBinder()---SetViewValue---");
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (IRelativeLayout::Probe(view) != NULL) {
        // Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnTouchListener");
        AutoPtr<IRelativeLayout> thumbLayout = IRelativeLayout::Probe(view);
        thumbLayout->SetOnTouchListener((IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener)));
        *result = TRUE;
    }
    else if (IImageView::Probe(view) != NULL) {
        // Logger::D(TAG, "MyViewBinder()---SetViewValue---text:%s", textRepresentation.string());
        AutoPtr<IImageView> imageView = IImageView::Probe(view);
        AutoPtr<IWeakReference> weakHost;
        mHost->GetWeakReference((IWeakReference**)&weakHost);
        assert(weakHost != NULL);
        AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
        AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(textRepresentation, FALSE, imageView, myLoadImage);
        if (drawable != NULL) {
            imageView->SetImageDrawable(IDrawable::Probe(drawable));
        }
        else {
            imageView->SetImageResource(R::color::divide_line_bg);
        }
        *result = TRUE;
    }
//    else if (ICheckable::Probe(view) != NULL) {
//        Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnCheckedChangeListener");
//        AutoPtr<ICheckBox> checkBox = ICheckBox::Probe(view);
//        checkBox->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)
//                        (mHost->mMyListener->Probe(EIID_ICompoundButtonOnCheckedChangeListener)));
//        *result = FALSE;
//    }
    return NOERROR;
}

CBrowserActivity::MyLoadImageCallback::MyLoadImageCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CBrowserActivity::MyLoadImageCallback::ImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "ImageLoaded()-----");
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CBrowserActivity* mHost = (CBrowserActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = imageDrawable;
    args->mArg2 = imageView;

    AutoPtr<IMessage> msg;
    mHost->mMyHandler->ObtainMessage(CBrowserActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    Boolean result;
    return mHost->mMyHandler->SendMessage(msg, &result);
}

CBrowserActivity::CBrowserActivity()
{
}

ECode CBrowserActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()---");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_browser);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mMyHandler = new MyHandler(weakHost);

    mMyListener = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::pic_back);
    mBackButton = IImageView::Probe(view);
    assert(mBackButton != NULL);

    view = FindViewById(R::id::pic_folder_name);
    AutoPtr<ITextView> albumName = ITextView::Probe(view);
    assert(albumName != NULL);

    view = FindViewById(R::id::pic_btn_wallpaper);
    mWallpaperLayout = ILinearLayout::Probe(view);
    assert(mWallpaperLayout != NULL);

    view = FindViewById(R::id::pic_btn_detail);
    mDetailLayout = ILinearLayout::Probe(view);
    assert(mDetailLayout != NULL);

    view = FindViewById(R::id::pic_btn_share);
    mShareLayout = ILinearLayout::Probe(view);
    assert(mShareLayout != NULL);

    view = FindViewById(R::id::pic_btn_popup);
    mPopButton = IButton::Probe(view);
    assert(mPopButton != NULL);

    view = FindViewById(R::id::pic_gridview);
    mGridView = IGridView::Probe(view);
    assert(mGridView != NULL);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        String path;
        intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &path);
        if (!path.IsNullOrEmpty()) {
            String folderName;
            intent->GetStringExtra(DataSourceHelper::SOURCE_DESC, &folderName);
            AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(path);
            if (imageItems != NULL) {
                AutoPtr<PictureEntry> entry;
                List<String>::Iterator it = imageItems->Begin();
                for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
                    entry = new PictureEntry();
                    entry->sourcePath = path;
                    entry->sourcePath += DataSourceHelper::PATH_SPLITE;
                    entry->sourcePath += *it;
                    entry->desc = folderName;
                    // Logger::D(TAG, " > %d, path:%s, folderName:%s", i, entry->sourcePath.string(), entry->desc.string());
                    mPictureEntryList.PushBack(entry);
                }
                AutoPtr<ICharSequence> cs;
                CStringWrapper::New(folderName, (ICharSequence**)&cs);
                albumName->SetText(cs);
                mSimpleAdapter = GetSimpleAdapter();
                if (mSimpleAdapter == NULL) {
                    Logger::W(TAG, "mSimpleAdapter is null!");
                }
                mGridView->SetAdapter(IAdapter::Probe(mSimpleAdapter));
            }
            else {
                Logger::W(TAG, "imageItems is null!");
            }
        }
    }

    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
    mBackButton->SetOnClickListener(clickListener);
    mWallpaperLayout->SetOnClickListener(clickListener);
    mDetailLayout->SetOnClickListener(clickListener);
    mShareLayout->SetOnClickListener(clickListener);
    mPopButton->SetOnClickListener(clickListener);
    return NOERROR;
}

ECode CBrowserActivity::OnStart()
{
    Logger::D(TAG, "OnStart()---");
    Activity::OnStart();
    return NOERROR;
}

ECode CBrowserActivity::OnResume()
{
    Logger::D(TAG, "OnResume()---");
    Activity::OnResume();
    return NOERROR;
}

ECode CBrowserActivity::OnPause()
{
    Logger::D(TAG, "OnPause()---");
    Activity::OnPause();
    return NOERROR;
}

ECode CBrowserActivity::OnStop()
{
    Logger::D(TAG, "OnStop()---");
    Activity::OnStop();
    return NOERROR;
}

ECode CBrowserActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()---");
    Activity::OnDestroy();
    mSimpleAdapter = NULL;
    return NOERROR;
}

ECode CBrowserActivity::OnBackPressed()
{
    Logger::D(TAG, "OnBackPressed()---");

    if (mShowPicPop) {
        ClosePicPopupWindow();
    }
    else {
        Activity::OnBackPressed();
    }

    return NOERROR;
}

ECode CBrowserActivity::OpenPicPopupWindow(
    /* [in] */ IView* v)
{
    Logger::D(TAG, "OpenPicPopupWindow()-----");

    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();

    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::pic_popup_dialog, NULL, (IView**)&layout);
    AutoPtr<IView> view;
    layout->FindViewById(R::id::pic_pop_time, (IView**)&view);
    AutoPtr<ITextView> textTime = ITextView::Probe(view);
    assert(textTime != NULL);
    textTime->SetOnClickListener(clickListener);

    view = NULL;
    layout->FindViewById(R::id::pic_pop_location, (IView**)&view);
    AutoPtr<ITextView> textLocation = ITextView::Probe(view);
    assert(textLocation != NULL);
    textLocation->SetOnClickListener(clickListener);

    mPicPopupWindow = NULL;
    CPopupWindow::New(layout, 80, 62, TRUE, (IPopupWindow**)&mPicPopupWindow);
    mPicPopupWindow->SetTouchable(TRUE);
    mPicPopupWindow->SetOutsideTouchable(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
    mPicPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
    mPicPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mMyListener.Get());
    mPicPopupWindow->ShowAsDropDown(v);
    mShowPicPop = TRUE;

    return NOERROR;
}

ECode CBrowserActivity::ClosePicPopupWindow()
{
    Boolean isShow = FALSE;
    if (mPicPopupWindow != NULL && (mPicPopupWindow->IsShowing(&isShow), isShow)) {
        Logger::D(TAG, "ClosePicPopupWindow()---");
        mPicPopupWindow->Dismiss();
        mPicPopupWindow = NULL;
    }
    mShowPicPop = FALSE;
    return NOERROR;
}

AutoPtr<ISimpleAdapter> CBrowserActivity::GetSimpleAdapter()
{
    Logger::D(TAG, "GetSimpleAdapter()---");
    AutoPtr<ISimpleAdapter> simpleAdapter;
    if (!mPictureEntryList.IsEmpty()) {
        Logger::D(TAG, "GetSimpleAdapter()---PictureEntry count: %d", mPictureEntryList.GetSize());
        AutoPtr<IObjectContainer> dataList;
        CParcelableObjectContainer::New((IObjectContainer**)&dataList);

        AutoPtr<PictureEntry> entry;
        String key1("image");
        String key2("checkbox");
        String key3("thumbLayout");

        List< AutoPtr<PictureEntry> >::Iterator it = mPictureEntryList.Begin();
        for (; it != mPictureEntryList.End(); ++it) {
            entry = *it;
            AutoPtr<IObjectStringMap> map;
            CObjectStringMap::New((IObjectStringMap**)&map);
            AutoPtr<IFile> file;
            CFile::New(entry->sourcePath, (IFile**)&file);
            Int64 last = 0;
            file->LastModified(&last);
            Logger::D(TAG, "GetSimpleAdapter()---sort--last:%lld,--path:%s", last, entry->sourcePath.string());
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
            map->Put(key1, cs->Probe(EIID_ICharSequence));
            AutoPtr<IBoolean> boolValue;
            CBoolean::New(FALSE, (IBoolean**)&boolValue);
            map->Put(key2, boolValue->Probe(EIID_IBoolean));
            dataList->Add(map);
        }

        AutoPtr< ArrayOf<String> > from = ArrayOf<String>::Alloc(3);
        (*from)[0] = key1;
        (*from)[1] = key2;
        (*from)[2] = key3;
        AutoPtr< ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(3);
        (*to)[0] = R::id::pic_thumb;
        (*to)[1] = R::id::pic_check;
        (*to)[2] = R::id::pic_thumb_layout;
        CSimpleAdapter::New(this, dataList, R::layout::pic_item, from, to, (ISimpleAdapter**)&simpleAdapter);
        AutoPtr<MyViewBinder> myViewBinder = new MyViewBinder(this);
        simpleAdapter->SetViewBinder(ISimpleAdapterViewBinder::Probe(myViewBinder));
    }

    return simpleAdapter;
}

void CBrowserActivity::MyImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "MyImageLoaded()-----");
    imageView->SetImageDrawable(imageDrawable);
}

} // namespace Gallery
