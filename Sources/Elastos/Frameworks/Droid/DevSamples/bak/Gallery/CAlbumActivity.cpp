
#include "CAlbumActivity.h"
#include "src/data/BucketHelper.h"
#include "src/data/DataSourceHelper.h"
#include "src/data/AsyncImageLoader.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/os/SomeArgs.h"

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::CStringWrapper;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CThread;
using Elastos::Core::IThread;

using Elastos::IO::CFile;
using Elastos::IO::IFile;

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IGridView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::EIID_ISimpleAdapterViewBinder;
using Elastos::Droid::Widget::IRelativeLayout;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Utility::CParcelableObjectContainer;

using Elastos::Utility::CObjectStringMap;
using Elastos::Utility::IObjectStringMap;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CAlbumActivity::TAG("CAlbumActivity");
const Int32 CAlbumActivity::MSG_IMAGE_LOADED = 0;
const Int32 CAlbumActivity::MSG_LOAD_ALBUM_VIEW = 1;

ECode CAlbumActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CAlbumActivity::MSG_IMAGE_LOADED: {
            SomeArgs* args = (SomeArgs*)obj.Get();
            IDrawable* drawable = IDrawable::Probe(args->mArg1);
            IImageView* imageView = IImageView::Probe(args->mArg2);
            mHost->MyImageLoaded(drawable, imageView);
            args->Recycle();
            break;
        }
        case CAlbumActivity::MSG_LOAD_ALBUM_VIEW: {
            mHost->LoadAlbumView();
            break;
        }
    }

    return NOERROR;
}

CAlbumActivity::MyListener::MyListener(
    /* [in] */ CAlbumActivity* host)
    : mHost(host)
{}

PInterface CAlbumActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return (IAdapterViewOnItemClickListener*)this;
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

UInt32 CAlbumActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CAlbumActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CAlbumActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else if (pObject == (IInterface*)(IAdapterViewOnItemClickListener*)this) {
        *pIID = EIID_IAdapterViewOnItemClickListener;
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

ECode CAlbumActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::album_btn_detail: {
            Logger::D(TAG, "OnClick()---album_btn_detail");
            break;
        }
        case R::id::album_about: {
            Logger::D(TAG, "OnClick()---album_about");
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("Gallery"), String("Gallery.CAboutActivity"));
            if (FAILED(mHost->StartActivity(intent))) {
                Logger::E(TAG, "OnClick()---StartActivity CAboutActivity failed!");
            }
            break;
        }
        case R::id::album_check_all: {
            Logger::D(TAG, "OnClick()---album_check_all");
            // Boolean isChecked = FALSE;
            // ICheckable::Probe(v)->IsChecked(&isChecked);
            // Logger::D("CAlbumActivity", "OnClick---check_all---isChecked:%d", isChecked);
            // if (isChecked) {
            //     mHost->mDetailButton->SetEnabled(TRUE);
            //     mHost->mDeleteButton->SetEnabled(TRUE);
            // }
            // else {
            //     mHost->mDetailButton->SetEnabled(FALSE);
            //     mHost->mDeleteButton->SetEnabled(FALSE);
            // }
            break;
        }
        case R::id::album_btn_delete: {
            Logger::D(TAG, "OnClick()---album_btn_delete");
            break;
        }
        case R::id::album_btn_popup: {
            Logger::D(TAG, "OnClick()---album_btn_popup");
            if (mHost->mShowAlbum) {
                mHost->CloseAlbumPopupWindow();
            }
            else {
                mHost->OpenAlbumPopupWindow(v);
            }
            break;
        }
        case R::id::pop_album: {
            Logger::D(TAG, "OnClick()---pop_album");
            mHost->CloseAlbumPopupWindow();
            break;
        }
        case R::id::pop_time: {
            Logger::D(TAG, "OnClick()---pop_time");
            mHost->CloseAlbumPopupWindow();
            break;
        }
        case R::id::pop_location: {
            Logger::D(TAG, "OnClick()---pop_location");
            mHost->CloseAlbumPopupWindow();
            break;
        }

        default:
            break;
    }

    return NOERROR;
}

ECode CAlbumActivity::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Logger::D(TAG, "OnItemClick()---position = %d, id = %lld", position, id);
    return NOERROR;
}

ECode CAlbumActivity::MyListener::OnDismiss()
{
    Logger::D(TAG, "OnDismiss()---");
    mHost->mAlbumPopupWindow = NULL;
    mHost->mShowAlbum = FALSE;
    return NOERROR;
}

ECode CAlbumActivity::MyListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    AutoPtr<IView> view;
    event->GetAction(&action);
    AutoPtr<IViewParent> viewParent;
    v->GetParent((IViewParent**)&viewParent);
    AutoPtr<IRelativeLayout> itemLayout = IRelativeLayout::Probe(viewParent) ;
    if (IMotionEvent::ACTION_DOWN == action) {
        Logger::D(TAG, "onTouch()--ACTION_DOWN");
        itemLayout->SetBackgroundResource(R::drawable::tools_bg_d);
    }
    else {
        Logger::D(TAG, "onTouch()--ACTION:%d", action);
        AutoPtr<IResources> res;
        mHost->GetResources((IResources**)&res);
        AutoPtr<IBitmapDrawable> bitmapDrawable;
        CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        itemLayout->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        AutoPtr<IViewParent> viewParent2;
        viewParent->GetParent((IViewParent**)&viewParent2);
        AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(viewParent2);
        Int32 index = 0;
        adapterView->GetPositionForView(v, &index);
        Logger::D(TAG, "onTouch()--index:%d", index);
        if (IMotionEvent::ACTION_UP == action) {
            AutoPtr<AlbumEntry> entry = mHost->mAlbumEntryList[index];
            String folderPath = entry->sourcePath.Substring(0, entry->sourcePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
            Logger::D(TAG, "onTouch()--ACTION_UP--path:%s", folderPath.string());
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("Gallery"), String("Gallery.CBrowserActivity"));
            intent->PutExtra(DataSourceHelper::SOURCE_PATH, folderPath);
            intent->PutExtra(DataSourceHelper::SOURCE_DESC, entry->desc);
            if (FAILED(mHost->StartActivity(intent))) {
                Logger::E(TAG, "OnClick()---StartActivity CBrowserActivity failed!");
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CAlbumActivity::MyListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    Logger::D(TAG, "OnCheckedChanged()--isChecked:%d", isChecked);
    return NOERROR;
}

CAlbumActivity::MyViewBinder::MyViewBinder(
    /* [in] */ CAlbumActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CAlbumActivity::MyViewBinder, ISimpleAdapterViewBinder)

ECode CAlbumActivity::MyViewBinder::SetViewValue(
    /* [in] */ IView* view,
    /* [in] */ IInterface* data,
    /* [in] */ const String& textRepresentation,
    /* [out] */ Boolean* result)
{
    // Logger::D(TAG, "MyViewBinder---SetViewValue");
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (ILinearLayout::Probe(view) != NULL) {
        // Logger::D(TAG, "MyViewBinder---SetViewValue---SetOnTouchListener");
        AutoPtr<ILinearLayout> folderLayout = ILinearLayout::Probe(view);
        folderLayout->SetOnTouchListener((IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener)));
        *result = TRUE;
    }
    else if (IImageView::Probe(view) != NULL) {
        Logger::D(TAG, "MyViewBinder()---SetViewValue:%s", textRepresentation.string());
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
    return NOERROR;
}

CAlbumActivity::LoadImageInfoRunnable::LoadImageInfoRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

CAR_INTERFACE_IMPL(CAlbumActivity::LoadImageInfoRunnable, IRunnable)

ECode CAlbumActivity::LoadImageInfoRunnable::Run()
{
    Logger::D(TAG, "Run()-----");

    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    // DataSourceHelper::SetRootPath(DataSourceHelper::ROOT_PATH);
    AutoPtr<HashMap<String, AutoPtr<List<String> > > > pathMap = DataSourceHelper::GetImageFileMap();
    AutoPtr<List<String> > pathList = DataSourceHelper::GetImagePathList();
    if (pathList != NULL) {
        Logger::D(TAG, "Run()-----1");
        AutoPtr<List<String> > fileList;
        AutoPtr<AlbumEntry> entry;
        List<String>::Iterator it = pathList->Begin();
        for (Int32 i = 0; it != pathList->End(); ++it, ++i) {
            entry = new AlbumEntry();
            String path(*it);
            HashMap<String, AutoPtr<List<String> > >::Iterator iter = pathMap->Find(path);
            fileList = iter->mSecond;
            entry->sourcePath = path;
            entry->sourcePath += DataSourceHelper::PATH_SPLITE;
            entry->sourcePath += fileList->GetFront();
            Int32 lastIndex = path.LastIndexOf(DataSourceHelper::PATH_SPLITE);
            entry->desc = path.Substring(lastIndex + 1, path.GetLength());
            Int32 size = fileList->GetSize();
            entry->num = StringUtils::ToString(size);
            mHost->mAlbumEntryList.PushBack(entry);

            // Logger::D(TAG, "Album %d: sourcePath:%s, desc:%s, num:%s",
            //     i, entry->sourcePath.string(), entry->desc.string(), entry->num.string());
        }

        Boolean result = FALSE;
        mHost->mMyHandler->SendEmptyMessage(MSG_LOAD_ALBUM_VIEW, &result);
    }
    return NOERROR;
}

CAlbumActivity::MyLoadImageCallback::MyLoadImageCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CAlbumActivity::MyLoadImageCallback::ImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "ImageLoaded()-----");

    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = imageDrawable;
    args->mArg2 = imageView;

    AutoPtr<IMessage> msg;
    mHost->mMyHandler->ObtainMessage(CAlbumActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    Boolean result;
    return mHost->mMyHandler->SendMessage(msg, &result);
}

void CAlbumActivity::MyImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "MyImageLoaded()-----");
    imageView->SetImageDrawable(imageDrawable);
}

CAlbumActivity::CAlbumActivity()
{
    Logger::D(TAG, "CAlbumActivity(): %p", this);
}

CAlbumActivity::~CAlbumActivity()
{
    Logger::D(TAG, "~CAlbumActivity(): %p", this);
}

ECode CAlbumActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()-----");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_album);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mMyHandler = new MyHandler(weakHost);

    // Init image file map.
    DataSourceHelper::SetRootPath(DataSourceHelper::ROOT_PATH);

    mMyListener = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::album_btn_popup);
    mPopupButton = IButton::Probe(view);
    assert(mPopupButton != NULL);

    view = FindViewById(R::id::album_gridview);
    mGridView = IGridView::Probe(view);
    assert(mGridView != NULL);

    view = FindViewById(R::id::album_empty);
    mEmptyView = ITextView::Probe(view);
    assert(mEmptyView != NULL);

    view = FindViewById(R::id::album_about);
    mAboutButton = ITextView::Probe(view);
    assert(mAboutButton != NULL);

    view = FindViewById(R::id::album_check_all);
    mCheckAll = IButton::Probe(view);
    assert(mCheckAll != NULL);

    view = FindViewById(R::id::album_btn_detail);
    mDetailLayout = ILinearLayout::Probe(view);
    assert(mDetailLayout != NULL);

    view = FindViewById(R::id::album_btn_delete);
    mDeleteButton = IImageButton::Probe(view);
    assert(mDeleteButton != NULL);

    view = FindViewById(R::id::album_progress_bar);
    mProgressBar = IProgressBar::Probe(view);
    assert(mProgressBar != NULL);

    LoadImageInfos();
    return NOERROR;
}

ECode CAlbumActivity::OnStart()
{
    Logger::D(TAG, "OnStart()-----");
    Activity::OnStart();
    return NOERROR;
}

ECode CAlbumActivity::OnResume()
{
    Logger::D(TAG, "OnResume()-----");
    Activity::OnResume();
    return NOERROR;
}

ECode CAlbumActivity::OnPause()
{
    Logger::D(TAG, "OnPause()-----");
    Activity::OnPause();
    return NOERROR;
}

ECode CAlbumActivity::OnStop()
{
    Logger::D(TAG, "OnStop()-----");
    Activity::OnStop();
    return NOERROR;
}

ECode CAlbumActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()-----");
    Activity::OnDestroy();
    AutoPtr<AlbumEntry> entry;
    AutoPtr< ArrayOf<String> > albumArray = ArrayOf<String>::Alloc(mAlbumEntryList.GetSize());
    List< AutoPtr<AlbumEntry> >::Iterator it = mAlbumEntryList.Begin();
    for (Int32 i = 0; it != mAlbumEntryList.End(); ++it, ++i) {
        entry = *it;
        (*albumArray)[i] = entry->sourcePath;
    }
    AsyncImageLoader::UpdateImageCache(albumArray);
    return NOERROR;
}

ECode CAlbumActivity::OnBackPressed()
{
    Logger::D(TAG, "OnBackPressed()---");
    Activity::OnBackPressed();
    return NOERROR;
}

AutoPtr<ISimpleAdapter> CAlbumActivity::GetSimpleAdapter()
{
    Logger::D(TAG, "GetSimpleAdapter()-----");
    AutoPtr<ISimpleAdapter> simpleAdapter;
    if (!mAlbumEntryList.IsEmpty()) {
        Logger::D(TAG, "GetSimpleAdapter()---AlbumEntry count: %d", mAlbumEntryList.GetSize());
        AutoPtr<IObjectContainer> dataList;
        CParcelableObjectContainer::New((IObjectContainer**)&dataList);
        AutoPtr<IObjectStringMap> map;
        AutoPtr<IBoolean> boolValue;
        AutoPtr<AlbumEntry> entry;
        AutoPtr<ICharSequence> cs;
        String key1("cover");
        String key2("count");
        String key3("checkbox");
        String key4("name");
        String key5("folderlayout");
        List< AutoPtr<AlbumEntry> >::Iterator it = mAlbumEntryList.Begin();
        for (Int32 i = 0; it != mAlbumEntryList.End(); ++it, ++i) {
            entry = *it;
            map = NULL;
            CObjectStringMap::New((IObjectStringMap**)&map);

            cs = NULL;
            CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
            map->Put(key1, cs);

            cs = NULL;
            CStringWrapper::New(entry->num, (ICharSequence**)&cs);
            map->Put(key2, cs);

            boolValue = NULL;
            CBoolean::New(FALSE, (IBoolean**)&boolValue);
            map->Put(key3, boolValue);

            cs = NULL;
            CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
            map->Put(key4, cs);

            dataList->Add(map);

            // Logger::D(TAG, " > album %d, path:%s, num:%s, desc:%s",
            //     i, entry->sourcePath.string(), entry->num.string(), entry->desc.string());
        }

        AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(5);
        (*from)[0] = key1;
        (*from)[1] = key2;
        (*from)[2] = key3;
        (*from)[3] = key4;
        (*from)[4] = key5;
        AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(5);
        (*to)[0] = R::id::album_set_cover;
        (*to)[1] = R::id::album_count;
        (*to)[2] = R::id::album_set_check;
        (*to)[3] = R::id::album_set_desc;
        (*to)[4] = R::id::album_cover_layout;

        CSimpleAdapter::New(this, dataList, R::layout::album_item, from, to, (ISimpleAdapter**)&simpleAdapter);
        AutoPtr<MyViewBinder> myViewBinder = new MyViewBinder(this);
        simpleAdapter->SetViewBinder(ISimpleAdapterViewBinder::Probe(myViewBinder));
    }
    return simpleAdapter;
}

AutoPtr<IArrayAdapter> CAlbumActivity::GetArrayAdapter()
{
    Logger::D(TAG, "GetArrayAdapter()-----");
    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    for (Int32 i = 0; i < 100; ++i) {
        StringBuilder sb("Item ");
        sb += i;
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq);
    }

    AutoPtr<IArrayAdapter> arrayAdapter;
    CArrayAdapter::New(this, R::layout::test_item, dataList, (IArrayAdapter**)&arrayAdapter);
    return arrayAdapter;
}

ECode CAlbumActivity::OpenAlbumPopupWindow(
    /* [in] */ IView* v)
{
    Logger::D(TAG, "OpenAlbumPopupWindow()-----");
    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();

    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::album_popup_dialog, NULL, (IView**)&layout);
    AutoPtr<IView> view;
    layout->FindViewById(R::id::pop_album, (IView**)&view);
    AutoPtr<ITextView> popupAlbum = ITextView::Probe(view);
    assert(popupAlbum != NULL);
    popupAlbum->SetOnClickListener(clickListener);

    view = NULL;
    layout->FindViewById(R::id::pop_time, (IView**)&view);
    AutoPtr<ITextView> popupTime = ITextView::Probe(view);
    assert(popupTime != NULL);
    popupTime->SetOnClickListener(clickListener);

    view = NULL;
    layout->FindViewById(R::id::pop_location, (IView**)&view);
    AutoPtr<ITextView> popupLocation = ITextView::Probe(view);
    assert(popupLocation != NULL);
    popupLocation->SetOnClickListener(clickListener);

    assert(mAlbumPopupWindow.Get() == NULL);
    CPopupWindow::New(layout, 80, 93, TRUE, (IPopupWindow**)&mAlbumPopupWindow);
    mAlbumPopupWindow->SetTouchable(TRUE);
    mAlbumPopupWindow->SetOutsideTouchable(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
    mAlbumPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
    mAlbumPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mMyListener.Get());
    mAlbumPopupWindow->ShowAsDropDown(v);
    mShowAlbum = TRUE;

    return NOERROR;
}

ECode CAlbumActivity::CloseAlbumPopupWindow()
{
    Boolean isShow = FALSE;
    if (mAlbumPopupWindow != NULL && (mAlbumPopupWindow->IsShowing(&isShow), isShow)) {
        Logger::D(TAG, "CloseAlbumPopupWindow()---");
        mAlbumPopupWindow->Dismiss();
        mShowAlbum = FALSE;
    }

    return NOERROR;
}

void CAlbumActivity::LoadImageInfos()
{
    Logger::D(TAG, "LoadImageInfos()----");

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);

    AutoPtr<IThread> loadThread;
    AutoPtr<LoadImageInfoRunnable> loadRunnable = new LoadImageInfoRunnable(weakHost);
    CThread::New(IRunnable::Probe(loadRunnable), (IThread**)&loadThread);
    assert(loadThread != NULL);
    loadThread->Start();
}

void CAlbumActivity::LoadAlbumView()
{
    Logger::D(TAG, "LoadAlbumView()-----");
    mProgressBar->SetVisibility(IView::GONE);
    mSimpleAdapter = GetSimpleAdapter();

    if (mSimpleAdapter != NULL) {
        mGridView->SetVisibility(IView::VISIBLE);
        mGridView->SetAdapter(IAdapter::Probe(mSimpleAdapter));
        // mSimpleAdapter->NotifyDataSetChanged();
    }
    else {
        Logger::D(TAG, "LoadAlbumView()----mSimpleAdapter is empty");
        mEmptyView->SetVisibility(IView::VISIBLE);
        mGridView->SetVisibility(IView::GONE);
    }

    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
    mPopupButton->SetOnClickListener(clickListener);
    mDeleteButton->SetOnClickListener(clickListener);
    mCheckAll->SetOnClickListener(clickListener);
    mDetailLayout->SetOnClickListener(clickListener);
    mDeleteButton->SetOnClickListener(clickListener);
    mAboutButton->SetOnClickListener(clickListener);
}

} // namespace Gallery
