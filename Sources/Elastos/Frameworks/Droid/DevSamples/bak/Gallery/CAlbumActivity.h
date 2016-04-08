
#ifndef __CALBUMACTIVITY_H__
#define __CALBUMACTIVITY_H__

#include "_CAlbumActivity.h"
#include "elastos/droid/app/Activity.h"
#include "src/data/AsyncImageLoader.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Core::IRunnable;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IGridView;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapterViewBinder;
using Elastos::Droid::Widget::ITextView;

namespace Gallery {

class CAlbumActivity
    : public Activity
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ IWeakReference* host)
            : mWeakHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
        , public IAdapterViewOnItemClickListener
        , public IPopupWindowOnDismissListener
        , public IViewOnTouchListener
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        MyListener(
            /* [in] */ CAlbumActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnDismiss();

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CAlbumActivity* mHost;
    };

    class MyViewBinder
        : public ElRefBase
        , public ISimpleAdapterViewBinder
    {
    public:
        MyViewBinder(
            /* [in] */ CAlbumActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI SetViewValue(
            /* [in] */ IView* view,
            /* [in] */ IInterface* data,
            /* [in] */ const String& textRepresentation,
            /* [out] */ Boolean* result);

    private:
        CAlbumActivity* mHost;
    };

    class LoadImageInfoRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        LoadImageInfoRunnable(
            /* [in] */ IWeakReference* host);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class MyLoadImageCallback
        : public ILoadImageCallback
    {
    public:
        MyLoadImageCallback(
            /* [in] */ IWeakReference* host);

        CARAPI ImageLoaded(
            /* [in] */ IDrawable* imageDrawable,
            /* [in] */ IImageView* imageView);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class AlbumEntry
        : public ElRefBase
    {
    public:
        String sourcePath;
        String desc;
        String num;
    };

friend class MyListener;
friend class MyViewBinder;
friend class LoadImageInfoRunnable;
friend class MyLoadImageCallback;

public:
    CAlbumActivity();
    ~CAlbumActivity();

    CARAPI_(void) MyImageLoaded(
        /* [in] */ IDrawable* imageDrawable,
        /* [in] */ IImageView* imageView);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnBackPressed();

private:
    CARAPI_(AutoPtr<ISimpleAdapter>) GetSimpleAdapter();

    CARAPI_(AutoPtr<IArrayAdapter>) GetArrayAdapter();

    CARAPI OpenAlbumPopupWindow(
        /* [in] */ IView* v);

    CARAPI CloseAlbumPopupWindow();

    CARAPI_(void) LoadImageInfos();

    CARAPI_(void) ShowProgressDialog(
        /* [in] */ Boolean show);

    CARAPI_(void) LoadAlbumView();

private:
    static const String TAG;
    static const Int32 MSG_IMAGE_LOADED;
    static const Int32 MSG_LOAD_ALBUM_VIEW;

private:
    Boolean mShowAlbum;

    AutoPtr<MyListener> mMyListener;
    AutoPtr<ITextView> mAboutButton;
    AutoPtr<IButton> mCheckAll;
    // AutoPtr<IButton> mDetailButton;
    AutoPtr<ILinearLayout> mDetailLayout;
    AutoPtr<IImageButton> mDeleteButton;
    AutoPtr<IButton> mPopupButton;
    AutoPtr<IListView> mListView;
    AutoPtr<IGridView> mGridView;
    AutoPtr<ITextView> mEmptyView;
    AutoPtr<IPopupWindow> mAlbumPopupWindow;
    AutoPtr<ISimpleAdapter> mSimpleAdapter;
    AutoPtr<IArrayAdapter> mArrayAdapter;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IProgressBar> mProgressBar;
    List< AutoPtr<AlbumEntry> > mAlbumEntryList;
    AutoPtr<IHandler> mMyHandler;
};

} // namespace Gallery

#endif // __CALBUMACTIVITY_H__
