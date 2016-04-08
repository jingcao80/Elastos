
#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "_CMainActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VideoPlayerDemo {

class CMainActivity
    : public Activity
{
public:
    class ItemOnClickListener
        : public ElRefBase
        , public IAdapterViewOnItemClickListener
    {
    public:
        ItemOnClickListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        CMainActivity* mActivity;
    };

public:
    CMainActivity();

    ~CMainActivity();

    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI GetVideoList(
        /* [in] */ const String& type,
        /* [out, callee] */ ArrayOf<IInterface*>** result);

private:
    AutoPtr<ArrayOf<IInterface*> > mDataList;
    AutoPtr<IArrayAdapter> mAdapter;
    AutoPtr<IListView> mContentListView;

    AutoPtr<ItemOnClickListener> mItemListener;
};

} // namespace VideoPlayerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CMAINACTIVITY_H__
