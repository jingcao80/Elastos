
#ifndef __ELASTOS_DROID_APP_LISTGRAGMENT_H__
#define __ELASTOS_DROID_APP_LISTGRAGMENT_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::EIID_IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC ListFragment
    : public Fragment
    , public IListFragment
{
public:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ListFragment* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        ListFragment* mHost;
    };

    class MyOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnItemClickListener(
            /* [in] */ ListFragment* host)
            : mHost(host)
        {}

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        ListFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ListFragment();

    virtual ~ListFragment();

    CARAPI constructor();

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnDestroyView();

    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI SetListAdapter(
        /* [in] */ IListAdapter* adapter);

    CARAPI SetSelection(
        /* [in] */ Int32 position);

    CARAPI GetSelectedItemPosition(
        /* [out] */ Int32* position);

    CARAPI GetSelectedItemId(
        /* [out] */ Int64* id);

    CARAPI GetListView(
        /* [out] */ IListView** listview);

    CARAPI SetEmptyText(
        /* [in] */ ICharSequence* text);

    CARAPI SetListShown(
        /* [in] */ Boolean shown);

    CARAPI SetListShownNoAnimation(
        /* [in] */ Boolean shown);

    CARAPI GetListAdapter(
        /* [out] */ IListAdapter** listadapter);

private:
    ECO_LOCAL CARAPI SetListShown(
        /* [in] */ Boolean shown,
        /* [in] */ Boolean animate);

    ECO_LOCAL CARAPI EnsureList();

public:
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<IListView> mList;
    AutoPtr<IView> mEmptyView;
    AutoPtr<ITextView> mStandardEmptyView;
    AutoPtr<IView> mProgressContainer;
    AutoPtr<IView> mListContainer;
    AutoPtr<ICharSequence> mEmptyText;
    Boolean mListShown;

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<MyRunnable> mRequestFocus;
    AutoPtr<MyOnItemClickListener> mOnClickListener;
};

} //namespace App
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_APP_LISTGRAGMENT_H__
