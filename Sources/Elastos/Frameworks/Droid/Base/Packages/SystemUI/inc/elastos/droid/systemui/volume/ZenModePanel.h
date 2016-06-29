
#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_ZENMODEPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_ZENMODEPANEL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/systemui/volume/SegmentedButtons.h"
#include "elastos/droid/systemui/volume/ZenToast.h"
#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerCallback.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::SystemUI::StatusBar::Policy::IZenModeController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ZenModeControllerCallback;
using Elastos::Droid::SystemUI::Volume::IInteractionCallback;
using Elastos::Droid::SystemUI::Volume::IZenModePanelCallback;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class ZenModePanel
    : public LinearLayout
    , public IZenModePanel
{
private:
    class ZenCallback
        : public ZenModeControllerCallback
    {
    public:
        ZenCallback(
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnZenChanged(
            /* [in] */ Int32 zen);

        // @Override
        CARAPI OnConditionsChanged(
            /* [in] */ ArrayOf<ICondition*>* conditions);

        // @Override
        CARAPI OnExitConditionChanged(
            /* [in] */ ICondition* exitCondition);

    private:
        ZenModePanel* mHost;
    };

    class H
        : public Handler
    {
    public:
        H(
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        const static Int32 UPDATE_CONDITIONS = 1;
        const static Int32 EXIT_CONDITION_CHANGED = 2;
        const static Int32 UPDATE_ZEN = 3;

    private:
        ZenModePanel* mHost;
    };

    // used as the view tag on condition rows
    class ConditionTag
        : public Object
    {
    public:
        AutoPtr<IRadioButton> mRb;
        AutoPtr<ITextView> mTitle;
        AutoPtr<ICondition> mCondition;
    };

    class Prefs
        : public Object
        , public ISharedPreferencesOnSharedPreferenceChangeListener
    {
        friend class ZenModePanel;
    public:
        CAR_INTERFACE_DECL()

        CARAPI IsNoneDangerous(
            /* [out] */ Boolean* isNoneDangerous);

        CARAPI TrackNoneSelected();

        CARAPI GetMinuteIndex(
            /* [out] */ Int32* index);

        CARAPI SetMinuteIndex(
            /* [in] */ Int32 minuteIndex);

        // @Override
        CARAPI OnSharedPreferenceChanged(
            /* [in] */ ISharedPreferences* prefs,
            /* [in] */ const String& key);

    private:
        Prefs(
            /* [in] */ ZenModePanel* host);

        CARAPI_(AutoPtr<ISharedPreferences>) prefs();

        CARAPI_(void) UpdateMinuteIndex();

        CARAPI_(Int32) ClampIndex(
            /* [in] */ Int32 index);

        CARAPI_(void) UpdateNoneSelected();

        CARAPI_(Int32) ClampNoneSelected(
            /* [in] */ Int32 noneSelected);

    public:
        const static String KEY_MINUTE_INDEX;
        const static String KEY_NONE_SELECTED;

    private:
        Int32 mNoneDangerousThreshold;

        Int32 mMinuteIndex;
        Int32 mNoneSelected;
        ZenModePanel* mHost;
    };

    class ZenButtonsCallback
        : public Object
        , public ISegmentedButtonsCallback
        , public IInteractionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        ZenButtonsCallback(
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnSelected(
            /* [in] */ IInterface* value);

        // @Override
        CARAPI OnInteraction();

    private:
        ZenModePanel* mHost;
    };

    class InteractionCallback
        : public Object
        , public IInteractionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InteractionCallback(
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnInteraction();

    private:
        ZenModePanel* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ ZenModePanel* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        ZenModePanel* mHost;
    };

    class MyViewOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnClickListener2(
            /* [in] */ ZenModePanel* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        ZenModePanel* mHost;
    };

    class MyAnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListener(
            /* [in] */ IView* noneButton);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mNoneButton;
    };

    class MyOCCL
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOCCL(
            /* [in] */ ConditionTag* tag,
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        ZenModePanel* mHost;
        AutoPtr<ConditionTag> mTag;
    };

    class MyOCL1
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOCL1(
            /* [in] */ IView* row,
            /* [in] */ ConditionTag* tag,
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<IView> mRow;
        AutoPtr<ConditionTag> mTag;
        ZenModePanel* mHost;
    };

   class MyOCL2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOCL2(
            /* [in] */ IView* row,
            /* [in] */ ConditionTag* tag,
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<IView> mRow;
        AutoPtr<ConditionTag> mTag;
        ZenModePanel* mHost;
    };

    class MyOCL3
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOCL3(
            /* [in] */ ConditionTag* tag,
            /* [in] */ ZenModePanel* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        ZenModePanel* mHost;
        AutoPtr<ConditionTag> mTag;
    };

public:
    CAR_INTERFACE_DECL()

    ZenModePanel();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetHidden(
        /* [in] */ Boolean hidden);

    CARAPI Init(
        /* [in] */ IZenModeController* controller);

    CARAPI UpdateLocale();

    CARAPI SetCallback(
        /* [in] */ IZenModePanelCallback* callback);

    CARAPI ShowSilentHint();

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) CheckForAttachedZenChange();

    CARAPI_(void) SetExpanded(
        /* [in] */ Boolean expanded);

    /** Start or stop requesting relevant zen mode exit conditions */
    CARAPI_(void) SetRequestingConditions(
        /* [in] */ Boolean requesting);

    CARAPI_(void) SetExitCondition(
        /* [in] */ ICondition* exitCondition);

    static CARAPI_(AutoPtr<IUri>) GetConditionId(
        /* [in] */ ICondition* condition);

    static CARAPI_(Boolean) SameConditionId(
        /* [in] */ ICondition* lhs,
        /* [in] */ ICondition* rhs);

    static CARAPI_(AutoPtr<ICondition>) Copy(
        /* [in] */ ICondition* condition);

    CARAPI_(void) RefreshExitConditionText();

    CARAPI_(void) HandleUpdateZen(
        /* [in] */ Int32 zen);

    CARAPI_(Int32) GetSelectedZen(
        /* [in] */ Int32 defValue);

    CARAPI_(void) UpdateWidgets();

    CARAPI_(AutoPtr<ICondition>) ParseExistingTimeCondition(
        /* [in] */ ICondition* condition);

    CARAPI_(void) HandleUpdateConditions(
        /* [in] */ ArrayOf<ICondition*>* conditions);

    CARAPI_(void) HandleUpdateConditions();

    static CARAPI_(Boolean) IsDowntime(
        /* [in] */ ICondition* c);

    CARAPI_(AutoPtr<ConditionTag>) GetConditionTagAt(
        /* [in] */ Int32 index);

    CARAPI_(void) CheckForDefault();

    CARAPI_(void) HandleExitConditionChanged(
        /* [in] */ ICondition* exitCondition);

    CARAPI_(void) Bind(
        /* [in] */ ICondition* condition,
        /* [in] */ IView* convertView);

    CARAPI_(void) AnnounceConditionSelection(
        /* [in] */ ConditionTag* tag);

    CARAPI_(void) OnClickTimeButton(
        /* [in] */ IView* row,
        /* [in] */ ConditionTag* tag,
        /* [in] */ Boolean up);

    CARAPI_(void) Select(
        /* [in] */ ICondition* condition);

    CARAPI_(void) FireMoreSettings();

    CARAPI_(void) FireInteraction();

    CARAPI_(void) FireExpanded();

public:
    const static AutoPtr<IIntent> ZEN_SETTINGS;

    const static AutoPtr<ArrayOf<Int32> > MINUTE_BUCKETS;
private:
    const static String TAG;
    const static Boolean DEBUG;

    const static Int32 SECONDS_MS = 1000;
    const static Int32 MINUTES_MS = 60 * SECONDS_MS;

    const static Int32 MIN_BUCKET_MINUTES;
    const static Int32 MAX_BUCKET_MINUTES;
    const static Int32 DEFAULT_BUCKET_INDEX;
    const static Int32 FOREVER_CONDITION_INDEX = 0;
    const static Int32 TIME_CONDITION_INDEX = 1;
    const static Int32 FIRST_CONDITION_INDEX = 2;
    const static Float SILENT_HINT_PULSE_SCALE;

    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<H> mHandler;
    AutoPtr<Prefs> mPrefs;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    Int32 mSubheadWarningColor;
    Int32 mSubheadColor;
    AutoPtr<ZenToast> mZenToast;

    String mTag;

    AutoPtr<SegmentedButtons> mZenButtons;
    AutoPtr<IView> mZenSubhead;
    AutoPtr<ITextView> mZenSubheadCollapsed;
    AutoPtr<ITextView> mZenSubheadExpanded;
    AutoPtr<IView> mMoreSettings;
    AutoPtr<ILinearLayout> mZenConditions;

    AutoPtr<IZenModePanelCallback> mCallback;
    AutoPtr<IZenModeController> mController;
    Boolean mRequestingConditions;
    AutoPtr<ICondition> mExitCondition;
    String mExitConditionText;
    Int32 mBucketIndex;
    Boolean mExpanded;
    Boolean mHidden;
    Int32 mSessionZen;
    Int32 mAttachedZen;
    AutoPtr<ICondition> mSessionExitCondition;
    AutoPtr<ArrayOf<ICondition*> > mConditions;
    AutoPtr<ICondition> mTimeCondition;

    AutoPtr<ZenCallback> mZenCallback;
    AutoPtr<ZenButtonsCallback> mZenButtonsCallback;
    AutoPtr<InteractionCallback> mInteractionCallback;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_ZENMODEPANEL_H__