#ifndef __CSEARCHACTIVITY_H__
#define __CSEARCHACTIVITY_H__

#include "_CSearchActivity.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IBaseAdapter;

namespace MusicPlayer {

class CSearchActivity: public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CSearchActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
        /* [in] */ IView* v);

    private:
        CSearchActivity* mActivity;
    };


    class ContentAdapter
        : public ElRefBase
        , public IBaseAdapter
    {
    public:
        ContentAdapter(
            /* [in] */ CSearchActivity* activity);

        CAR_INTERFACE_DECL()

        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI GetCount(
            /* [in] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* pItemId);

        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI IsEmpty(
            /* [out] */ Boolean* isEmpty);

        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        CARAPI NotifyDataSetChanged();

        CARAPI NotifyDataSetInvalidated();

        CARAPI GetDropDownView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        CSearchActivity* mActivity;
        static Int32 mImage[];
        static Int32 mImageLength;
    };

    class MediaScannerReceiver : public BroadcastReceiver
    {
    public:
        CARAPI OnReceive(
            /* [in] */IContext* context,
            /* [in] */IIntent* intent);

        TO_STRING_IMPL("MediaScannerReceiver: ")
    };

protected:
    CARAPI OnCreate(
        /* [in] */IBundle* savedInstanceState);

    CARAPI InitView();

    CARAPI SetupListener();

private:
    AutoPtr<IView>  mBtnBack;
    AutoPtr<IButton> mBtnSearch;
    AutoPtr<IButton> mBtnEdit;

    AutoPtr<IButton> mBtnPlay;
    AutoPtr<IButton> mBtnPause;

    AutoPtr<IListView> mContentListView;

    AutoPtr<MyListener> mActionListener;
};

} // namespace MusicPlayer

#endif // __CMAINACTIVITY_H__
