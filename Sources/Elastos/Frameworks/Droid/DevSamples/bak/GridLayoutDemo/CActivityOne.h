#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IViewSwitcher;
using Elastos::Droid::Widget::ITextSwitcher;
using Elastos::Droid::Widget::IImageSwitcher;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IViewFactory;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICalendarView;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::IGridLayout;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::Internal::IActionBarView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ImageButtonDemo {

class CActivityOne : public Activity
{
public:
    class ActivityOneCheckedChangeListener
        : public ICompoundButtonOnCheckedChangeListener
        , public ElRefBase
    {
    public:
        ActivityOneCheckedChangeListener();

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);
    };

    class Switcher
        : public IViewFactory
        , public ElRefBase
    {
    public:
        Switcher(
            /* [in] */ IContext* context);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI MakeView(
            /* [out] */ IView** view);

    private:
        AutoPtr<IContext> mContext;
    };

    class ImageSwitcherListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        ImageSwitcherListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        AutoPtr<CActivityOne> mHost;
    };

    class TextSwitcherListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        TextSwitcherListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        AutoPtr<CActivityOne> mHost;
    };


    class ActivityListener
        : public ISeekBarOnSeekBarChangeListener
        , public IViewOnClickListener
        , public ElRefBase
    {
    public:
        ActivityListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        AutoPtr<CActivityOne> mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IImageButton> mPrev;
    AutoPtr<IImageButton> mNext;
    AutoPtr<IImageButton> mImageButton;
    AutoPtr<ISeekBar> mBar;
    AutoPtr<IProgressBar> mProgress;
    AutoPtr<INumberPicker> mNumber;
    AutoPtr<ITimePicker> mTime;
    AutoPtr<IViewSwitcher> mSwitcher;
    AutoPtr<IImageSwitcher> mImageSwitch;
    AutoPtr<IImageButton> mImagePrev;
    AutoPtr<IImageButton> mImageNext;
    AutoPtr<ITextSwitcher> mTextSwitch;
    AutoPtr<IImageButton> mTextPrev;
    AutoPtr<IImageButton> mTextNext;
    AutoPtr<ISwitch> mSwitch;
    AutoPtr<ICalendarView> mCalendar;
    AutoPtr<IDatePicker> mDate;
    AutoPtr<ITabHost> mTabHost;
    AutoPtr<IGridLayout> mLayout;
    AutoPtr<IButton> mButton;
    AutoPtr<IActionBarView> mActionBarView;
    AutoPtr<IActionBar> mActionBar;

    List< AutoPtr<IDrawable> > mList;
    static Int32 count;
    static Int32 index;
};

} // namespace SurfaceViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CACTIVITYONE_H__