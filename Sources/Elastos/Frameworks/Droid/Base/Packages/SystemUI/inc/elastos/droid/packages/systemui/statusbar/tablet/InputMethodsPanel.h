#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODPANEL_H__

#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include "elastos/Pair.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Pair;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISwitch;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


typedef Pair<AutoPtr<IInputMethodInfo>, AutoPtr<IInputMethodSubtype> > InputMethodPair;
typedef HashMap<AutoPtr<IInputMethodInfo>, AutoPtr<IObjectContainer> > InputMethodCacheMap;


class InputMethodsPanel : public LinearLayout
{
private:
    class InputMethodsPanelBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
         InputMethodsPanelBroadcastReceiver(
            /* [in] */ InputMethodsPanel* host);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("InputMethodsPanel::InputMethodsPanelBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        InputMethodsPanel* mHost;
    };

    class SettingsIconOnClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SettingsIconOnClickListener(
            /* [in] */ const String& packageName,
            /* [in] */ const String& settingsActivity,
            /* [in] */ InputMethodsPanel* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        String mPackageName;
        String mSettingsActivity;
        InputMethodsPanel* mHost;
    };

    class SubtypeViewOnClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SubtypeViewOnClickListener(
            /* [in] */ InputMethodsPanel* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        InputMethodsPanel* mHost;
    };

public:
    InputMethodsPanel();

    InputMethodsPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    InputMethodsPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetHardKeyboardEnabledChangeListener(
        /* [in] */ IOnHardKeyboardEnabledChangeListener* listener);

    CARAPI OpenPanel();

    CARAPI ClosePanel(
        /* [in] */ Boolean closeKeyboard);

    CARAPI SetImeToken(
        /* [in] */ IBinder* token);

    CARAPI SetImeSwitchButton(
        /* [in] */ IInputMethodButton* imb);

    CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    Int32 CompareInputMethodInfo(
        /* [in] */ IInputMethodInfo* lhs,
        /* [in] */ IInputMethodInfo* rhs);

    //@Override

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI OnFinishInflate();

    virtual Boolean IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI OnClick(
        /* [in] */ IView* view);

    virtual Boolean DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    virtual InputMethodPair UpdateRadioButtonsByView(
        /* [in] */ IView* selectedView);

private:
    void UpdateHardKeyboardEnabled();

    void StartActivity(
        /* [in] */ IIntent* intent);

    void ShowConfigureInputMethods();

    AutoPtr<IView> CreateInputMethodItem(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    void UpdateUiElements();

    void SetInputMethodAndSubtype(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */IInputMethodSubtype* subtype);

    void UpdateHardKeyboardSection();

    void UpdateRadioButtons();

    // Turn on the selected radio button at startup
    void UpdateRadioButtonsByImiAndSubtype(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    InputMethodCacheMap& GetEnabledInputMethodAndSubtypeList();

    AutoPtr<IInputMethodInfo> GetCurrentInputMethodInfo();

    AutoPtr<ICharSequence> GetIMIName(
        /* [in] */ IInputMethodInfo* imi);

    AutoPtr<ICharSequence> GetSubtypeName(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    AutoPtr<IDrawable> GetSubtypeIcon(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    void OnPackageChanged();

private:
    static const String TAG;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<IInputMethodManager> mImm;
    AutoPtr<IIntentFilter> mIntentFilter;

    HashMap<AutoPtr<IView>, InputMethodPair> mRadioViewAndImiMap;
    //TreeMap<InputMethodInfo, List<InputMethodSubtype> >
    InputMethodCacheMap mEnabledInputMethodAndSubtypesCache;

    Boolean mAttached;// = false;
    Boolean mPackageChanged;// = false;
    AutoPtr<IContext> mContext;
    AutoPtr<IBinder> mToken;
    AutoPtr<IInputMethodButton> mInputMethodSwitchButton;
    AutoPtr<ILinearLayout> mInputMethodMenuList;
    Boolean mHardKeyboardAvailable;
    Boolean mHardKeyboardEnabled;
    AutoPtr<IOnHardKeyboardEnabledChangeListener> mHardKeyboardEnabledChangeListener;
    AutoPtr<ILinearLayout> mHardKeyboardSection;
    AutoPtr<ISwitch> mHardKeyboardSwitch;
    AutoPtr<IPackageManager> mPackageManager;
    String mEnabledInputMethodAndSubtypesCacheStr;
    String mLastSystemLocaleString;
    AutoPtr<IView> mConfigureImeShortcut;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODPANEL_H__
