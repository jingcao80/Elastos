#ifndef __Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__

#include <elastos/droid/app/Activity.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_DevSamples_ProgressBarDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICalendarView;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IGridLayout;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageSwitcher;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITextSwitcher;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::IViewFactory;
using Elastos::Droid::Widget::IViewSwitcher;
using Elastos::Utility::IList;

namespace Elastos {
namespace DevSamples {
namespace ProgressBarDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class ActivityOneCheckedChangeListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityOneCheckedChangeListener();

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);
    };

    class Switcher
        : public Object
        , public IViewFactory
    {
    public:
        CAR_INTERFACE_DECL()

        Switcher(
            /* [in] */ IContext* context);

        CARAPI MakeView(
            /* [out] */ IView** view);

    private:
        AutoPtr<IContext> mContext;
    };

    class ImageSwitcherListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ImageSwitcherListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActivityOne* mHost;
    };

    class TextSwitcherListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TextSwitcherListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActivityOne* mHost;
    };

    class ActivityListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityListener(
            /* [in] */ CActivityOne* host);

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
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

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

    AutoPtr<IList> mList;
    static Int32 count;
    static Int32 index;
};

} // namespace ProgressBarDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__