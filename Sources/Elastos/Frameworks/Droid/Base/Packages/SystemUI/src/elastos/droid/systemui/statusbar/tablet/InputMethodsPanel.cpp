#include "elastos/droid/systemui/statusbar/tablet/InputMethodsPanel.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


//================================================================================
//              InputMethodsPanel::InputMethodsPanelBroadcastReceiver
//================================================================================
InputMethodsPanel::InputMethodsPanelBroadcastReceiver::InputMethodsPanelBroadcastReceiver(
    /* [in] */ InputMethodsPanel* panel)
    : mHost(panel)
{}

ECode InputMethodsPanel::InputMethodsPanelBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* Int32ent)
{
    mHost->OnPackageChanged();
    return NOERROR;
}

//================================================================================
//              InputMethodsPanel::SettingsIconOnClickListener
//================================================================================
CAR_INTERFACE_IMPL(InputMethodsPanel::SettingsIconOnClickListener, IViewOnClickListener)

InputMethodsPanel::SettingsIconOnClickListener::SettingsIconOnClickListener(
    /* [in] */ const String& packageName,
    /* [in] */ const String& settingsActivity,
    /* [in] */ InputMethodsPanel* host)
    : mPackageName(packageName)
    , mSettingsActivity(settingsActivity)
    , mHost(host)
{
}

ECode InputMethodsPanel::SettingsIconOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->SetClassName(mPackageName, mSettingsActivity);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    mHost->StartActivity(intent);
    mHost->ClosePanel(true);
    return NOERROR;
}

//================================================================================
//              InputMethodsPanel::SubtypeViewOnClickListener
//================================================================================
CAR_INTERFACE_IMPL(InputMethodsPanel::SubtypeViewOnClickListener, IViewOnClickListener)

InputMethodsPanel::SubtypeViewOnClickListener::SubtypeViewOnClickListener(
    /* [in] */ InputMethodsPanel* host)
    : mHost(host)
{
}

ECode InputMethodsPanel::SubtypeViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    InputMethodPair imiAndSubtype = mHost->UpdateRadioButtonsByView(v);
    mHost->ClosePanel(FALSE);
    mHost->SetInputMethodAndSubtype(imiAndSubtype.mFirst, imiAndSubtype.mSecond);

    return NOERROR;
}

//================================================================================
//              InputMethodsPanel
//================================================================================

Int32 InputMethodsPanel::CompareInputMethodInfo(
    /* [in] */ IInputMethodInfo* imi1,
    /* [in] */ IInputMethodInfo* imi2)
{
    if (imi2 == NULL) return 0;
    if (imi1 == NULL) return 1;

    String id1, id2;
    imi1->GetId(&id1);
    imi2->GetId(&id2);

    if (mPackageManager == NULL) {
        return id1.Compare(id2);
    }

    AutoPtr<ICharSequence> imiId1, imiId2;
    imi1->LoadLabel(mPackageManager, (ICharSequence**)&imiId1);
    imi2->LoadLabel(mPackageManager, (ICharSequence**)&imiId2);

    String fullId1, fullId2;
    imiId1->ToString(&fullId1);
    imiId2->ToString(&fullId2);
    fullId1 += "/";
    fullId1 += id1;
    fullId2 += "/";
    fullId2 += id2;

    return fullId1.Compare(fullId2);
}


const String InputMethodsPanel::TAG("InputMethodsPanel");

InputMethodsPanel::InputMethodsPanel()
    : mAttached(FALSE)
    , mPackageChanged(FALSE)
    , mHardKeyboardAvailable(FALSE)
    , mHardKeyboardEnabled(FALSE)
{
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mBroadcastReceiver = new InputMethodsPanelBroadcastReceiver(this);
}

InputMethodsPanel::InputMethodsPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
    , mAttached(FALSE)
    , mPackageChanged(FALSE)
    , mHardKeyboardAvailable(FALSE)
    , mHardKeyboardEnabled(FALSE)
{
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mBroadcastReceiver = new InputMethodsPanelBroadcastReceiver(this);

    InitImpl(context, attrs, 0);
}

InputMethodsPanel::InputMethodsPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
    , mAttached(FALSE)
    , mPackageChanged(FALSE)
    , mHardKeyboardAvailable(FALSE)
    , mHardKeyboardEnabled(FALSE)
{
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mBroadcastReceiver = new InputMethodsPanelBroadcastReceiver(this);

    InitImpl(context, attrs, defStyle);
}

ECode InputMethodsPanel::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode InputMethodsPanel::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ASSERT_SUCCEEDED(LinearLayout::Init(context, attrs, defStyle));
    return InitImpl(context, attrs, defStyle);
}

ECode InputMethodsPanel::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    mContext = context;

    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&mImm);
    mImm = IInputMethodManager::Probe(tmpObj.Get());
    if (mImm == NULL) {
        Logger::E(TAG, "Failed to get input method service!");
    }
    mIntentFilter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
    mIntentFilter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    mIntentFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    mIntentFilter->AddDataScheme(String("package"));
    return NOERROR;
}

ECode InputMethodsPanel::SetHardKeyboardEnabledChangeListener(
    /* [in] */ IOnHardKeyboardEnabledChangeListener* listener)
{
    mHardKeyboardEnabledChangeListener = listener;
    return NOERROR;
}

ECode InputMethodsPanel::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    if (mAttached) {
        AutoPtr<IContext> context = GetContext();
        context->UnregisterReceiver(mBroadcastReceiver);
        mAttached = FALSE;
    }
    return NOERROR;
}

ECode InputMethodsPanel::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();
    if (!mAttached) {
        AutoPtr<IContext> context = GetContext();
        AutoPtr<IIntent> stickyIntent;
        context->RegisterReceiver(mBroadcastReceiver, mIntentFilter, (IIntent**)&stickyIntent);
        mAttached = TRUE;
    }
    return NOERROR;
}

ECode InputMethodsPanel::OnFinishInflate()
{
    AutoPtr<IView> tmpView;
    tmpView = FindViewById(SystemUIR::id::input_method_menu_list);
    mInputMethodMenuList = ILinearLayout::Probe(tmpView.Get());

    tmpView = FindViewById(SystemUIR::id::hard_keyboard_section);
    mHardKeyboardSection = ILinearLayout::Probe(tmpView.Get());

    tmpView = FindViewById(SystemUIR::id::hard_keyboard_switch);
    mHardKeyboardSwitch = ISwitch::Probe(tmpView.Get());

    tmpView = FindViewById(SystemUIR::id::ime_settings_shortcut);
    mConfigureImeShortcut = IView::Probe(tmpView.Get());
    mConfigureImeShortcut->SetOnClickListener(THIS_PROBE(IViewOnClickListener));
    // TODO: If configurations for IME are not changed, do not update
    // by checking onConfigurationChanged.
    UpdateUiElements();
    return NOERROR;
}

Boolean InputMethodsPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return FALSE;
}

ECode InputMethodsPanel::InputMethodsPanel::OnClick(
    /* [in] */ IView* view)
{
    if (view == mConfigureImeShortcut) {
        ShowConfigureInputMethods();
        ClosePanel(TRUE);
    }
    return NOERROR;
}

Boolean InputMethodsPanel::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float fx, fy;
    event->GetX(&fx);
    event->GetY(&fy);
    Int32 x = (Int32) fx;
    Int32 y = (Int32) fy;
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return LinearLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}

void InputMethodsPanel::UpdateHardKeyboardEnabled()
{
    if (mHardKeyboardAvailable) {
        Boolean checked;
        ICheckable::Probe(mHardKeyboardSwitch.Get())->IsChecked(&checked);
        if (mHardKeyboardEnabled != checked) {
            mHardKeyboardEnabled = checked;
            if (mHardKeyboardEnabledChangeListener != NULL)
                mHardKeyboardEnabledChangeListener->OnHardKeyboardEnabledChange(checked);
        }
    }
}

ECode InputMethodsPanel::OpenPanel()
{
    SetVisibility(IView::VISIBLE);
    UpdateUiElements();
    if (mInputMethodSwitchButton != NULL) {
        mInputMethodSwitchButton->SetIconImage(SystemUIR::drawable::ic_sysbar_ime_pressed);
    }
    return NOERROR;
}

ECode InputMethodsPanel::ClosePanel(
    /* [in] */ Boolean closeKeyboard)
{
    SetVisibility(IView::GONE);
    if (mInputMethodSwitchButton != NULL) {
        mInputMethodSwitchButton->SetIconImage(SystemUIR::drawable::ic_sysbar_ime);
    }
    if (closeKeyboard) {
        if (mImm != NULL) {
            Boolean result;
            AutoPtr<IBinder> token = GetWindowToken();
            mImm->HideSoftInputFromWindow(token, 0, &result);
        }
    }
    return NOERROR;
}

void InputMethodsPanel::StartActivity(
    /* [in] */ IIntent* intent)
{
    mContext->StartActivity(intent);
}

void InputMethodsPanel::ShowConfigureInputMethods()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_INPUT_METHOD_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    StartActivity(intent);
}

AutoPtr<IView> InputMethodsPanel::CreateInputMethodItem(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
   AutoPtr<ICharSequence> subtypeName;
   Boolean enabled;
    if (subtype == NULL || (subtype->OverridesImplicitlyEnabledSubtype(&enabled), enabled)) {
        subtypeName = NULL;
    }
    else {
        subtypeName = GetSubtypeName(imi, subtype);
    }

    AutoPtr<ICharSequence> imiName = GetIMIName(imi);
    AutoPtr<IDrawable> icon = GetSubtypeIcon(imi, subtype);
    AutoPtr<IView> view, tmpView;
    View::Inflate(mContext, SystemUIR::layout::system_bar_input_methods_item, NULL, (IView**)&view);

    tmpView = NULL;
    view->FindViewById(SystemUIR::id::item_icon, (IView**)&tmpView);
    AutoPtr<IImageView> subtypeIcon = IImageView::Probe(tmpView.Get());

    tmpView = NULL;
    view->FindViewById(SystemUIR::id::item_title, (IView**)&tmpView);
    AutoPtr<ITextView> itemTitle = ITextView::Probe(tmpView.Get());

    tmpView = NULL;
    view->FindViewById(SystemUIR::id::item_subtitle, (IView**)&tmpView);
    AutoPtr<ITextView> itemSubtitle = ITextView::Probe(tmpView.Get());

    tmpView = NULL;
    view->FindViewById(SystemUIR::id::item_settings_icon, (IView**)&tmpView);
    AutoPtr<IImageView> settingsIcon = IImageView::Probe(tmpView.Get());

    AutoPtr<IView> subtypeView;
    view->FindViewById(SystemUIR::id::item_subtype, (IView**)&subtypeView);
    if (subtypeName == NULL) {
        itemTitle->SetText(imiName);
        itemSubtitle->SetVisibility(IView::GONE);
    }
    else {
        itemTitle->SetText(subtypeName);
        itemSubtitle->SetVisibility(IView::VISIBLE);
        itemSubtitle->SetText(imiName);
    }

    AutoPtr<ICharSequence> itemTitleText;
    itemTitle->GetText((ICharSequence**)&itemTitleText);

    subtypeIcon->SetImageDrawable(icon);
    subtypeIcon->SetContentDescription(itemTitleText);
    String settingsActivity;
    imi->GetSettingsActivity(&settingsActivity);
    AutoPtr<ICharSequence> saSeq;
    CString::New(settingsActivity, (ICharSequence**)&saSeq);
    if (!TextUtils::IsEmpty(saSeq)) {
        String packageName;
        imi->GetPackageName(&packageName);
        AutoPtr<IViewOnClickListener> listener =
            new SettingsIconOnClickListener(packageName, settingsActivity, this);
        settingsIcon->SetOnClickListener(listener);
    }
    else {
        // Do not show the settings icon if the IME does not have a settings preference
        tmpView = NULL;
        view->FindViewById(SystemUIR::id::item_vertical_separator, (IView**)&tmpView);
        tmpView->SetVisibility(IView::GONE);
        settingsIcon->SetVisibility(IView::GONE);
    }

    AutoPtr<IInputMethodInfo> aimi = imi;
    AutoPtr<IInputMethodSubtype> asubtype = subtype;
    InputMethodPair pair(aimi, asubtype);
    mRadioViewAndImiMap[subtypeView] = pair;

    AutoPtr<IViewOnClickListener> listener = new SubtypeViewOnClickListener(this);
    subtypeView->SetOnClickListener(listener);
    return view;
}

void InputMethodsPanel::UpdateUiElements()
{
    UpdateHardKeyboardSection();

    // TODO: Reuse subtype views.
    mInputMethodMenuList->RemoveAllViews();
    mRadioViewAndImiMap.Clear();
    mPackageManager = NULL;
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);

    Int32 count;
    InputMethodCacheMap& enabledIMIs = GetEnabledInputMethodAndSubtypeList();
    InputMethodCacheMap::Iterator it = enabledIMIs.Begin();
    for (; it != enabledIMIs.End(); ++it) {
        // List<AutoPtr<IInputMethodSubtype> > subtypes;
        AutoPtr<IObjectContainer> subtypes = it->mSecond;
        if (subtypes == NULL || (subtypes->GetObjectCount(&count), count == 0)) {
            AutoPtr<IView> view = CreateInputMethodItem(it->mFirst, NULL);
            mInputMethodMenuList->AddView(view);
        }
        else {
            AutoPtr<IObjectEnumerator> emu;
            subtypes->GetObjectEnumerator((IObjectEnumerator**)&emu);
            Boolean hasNext;
            while ((emu->MoveNext(&hasNext), hasNext)) {
                AutoPtr<IInterface> tmpObj;
                emu->Current((IInterface**)&tmpObj);
                AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(tmpObj.Get());

                AutoPtr<IView> view = CreateInputMethodItem(it->mFirst, subtype);
                mInputMethodMenuList->AddView(view);
            }
        }
    }

    UpdateRadioButtons();
}

ECode InputMethodsPanel::SetImeToken(
    /* [in] */ IBinder* token)
{
    mToken = token;
    return NOERROR;
}

ECode InputMethodsPanel::SetImeSwitchButton(
    /* [in] */ IInputMethodButton* imb)
{
    mInputMethodSwitchButton = imb;
    return NOERROR;
}

void InputMethodsPanel::SetInputMethodAndSubtype(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (mToken != NULL) {
        assert(imi != NULL);
        if (mImm != NULL) {
            String id;
            imi->GetId(&id);
            mImm->SetInputMethodAndSubtype(mToken, id, subtype);
        }
    }
    else {
        // Log.w(TAG, "IME Token is not set yet.");
    }
}

ECode InputMethodsPanel::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    if (mHardKeyboardAvailable != available || mHardKeyboardEnabled != enabled) {
        mHardKeyboardAvailable = available;
        mHardKeyboardEnabled = enabled;
        UpdateHardKeyboardSection();
    }
    return NOERROR;
}

void InputMethodsPanel::UpdateHardKeyboardSection()
{
    if (mHardKeyboardAvailable) {
        mHardKeyboardSection->SetVisibility(IView::VISIBLE);
        Boolean isChecked;
        ICheckable::Probe(mHardKeyboardSwitch.Get())->IsChecked(&isChecked);
        if (isChecked != mHardKeyboardEnabled) {
            ICheckable::Probe(mHardKeyboardSwitch.Get())->SetChecked(mHardKeyboardEnabled);
            UpdateHardKeyboardEnabled();
        }
    }
    else {
        mHardKeyboardSection->SetVisibility(IView::GONE);
    }
}

InputMethodPair InputMethodsPanel::UpdateRadioButtonsByView(
    /* [in] */ IView* selectedView)
{
    AutoPtr<IView> key = selectedView;
    InputMethodPair selectedImiAndSubtype;
    HashMap<AutoPtr<IView>, InputMethodPair>::Iterator it = mRadioViewAndImiMap.Find(key);
    if (it != mRadioViewAndImiMap.End()) {
        for (it = mRadioViewAndImiMap.Begin(); it != mRadioViewAndImiMap.End(); ++it) {
            AutoPtr<IView> radioView = it->mFirst;
            AutoPtr<IView> tmpView;
            radioView->FindViewById(SystemUIR::id::item_radio, (IView**)&tmpView);
            AutoPtr<IRadioButton> subtypeRadioButton = IRadioButton::Probe(tmpView.Get());

            if (subtypeRadioButton == NULL) {
                // Log.w(TAG, "RadioButton was not found in the selected subtype view");
                return selectedImiAndSubtype;
            }

            if (radioView.Get() == selectedView) {
                selectedImiAndSubtype = it->mSecond;
                ICheckable::Probe(subtypeRadioButton.Get())->SetChecked(TRUE);
            }
            else {
                ICheckable::Probe(subtypeRadioButton.Get())->SetChecked(FALSE);
            }
        }
    }
    return selectedImiAndSubtype;
}

void InputMethodsPanel::UpdateRadioButtons()
{
    if (mImm != NULL) {
        AutoPtr<IInputMethodInfo> info = GetCurrentInputMethodInfo();
        AutoPtr<IInputMethodSubtype> subtype;
        mImm->GetCurrentInputMethodSubtype((IInputMethodSubtype**)&subtype);
        UpdateRadioButtonsByImiAndSubtype(info, subtype);
    }
}

// Turn on the selected radio button at startup
void InputMethodsPanel::UpdateRadioButtonsByImiAndSubtype(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL) return;
    // if (DEBUG) {
    //     Logger::D(TAG, "Update radio buttons by " + imi.getId() + ", " + subtype);
    // }

    String id, imiId;
    imi->GetId(&imiId);
    Boolean equals;
    HashMap<AutoPtr<IView>, InputMethodPair>::Iterator it;
    for (it = mRadioViewAndImiMap.Begin(); it != mRadioViewAndImiMap.End(); ++it) {
        AutoPtr<IView> radioView = it->mFirst;
        AutoPtr<IView> tmpView;
        radioView->FindViewById(SystemUIR::id::item_radio, (IView**)&tmpView);
        AutoPtr<IRadioButton> subtypeRadioButton = IRadioButton::Probe(tmpView.Get());
        if (subtypeRadioButton == NULL) {
            Logger::W(TAG, "RadioButton was not found in the selected subtype view");
            return;
        }

        InputMethodPair imiAndSubtype = it->mSecond;
        imiAndSubtype.mFirst->GetId(&id);
        if (id.Equals(imiId)
                && (imiAndSubtype.mSecond == NULL
                    || (imiAndSubtype.mSecond->Equals(subtype, &equals), equals))) {
            ICheckable::Probe(subtypeRadioButton.Get())->SetChecked(TRUE);
        }
        else {
            ICheckable::Probe(subtypeRadioButton.Get())->SetChecked(FALSE);
        }
    }
}

InputMethodCacheMap& InputMethodsPanel::GetEnabledInputMethodAndSubtypeList()
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context = GetContext();
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    String newEnabledIMIs;
    ss->GetString(resolver, ISettingsSecure::ENABLED_INPUT_METHODS, &newEnabledIMIs);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> configuration;
    res->GetConfiguration((IConfiguration**)&configuration);
    AutoPtr<ILocale> locale;
    configuration->GetLocale((ILocale**)&locale);
    String currentSystemLocaleString;
    locale->ToString(&currentSystemLocaleString);

    if (!TextUtils::Equals(mEnabledInputMethodAndSubtypesCacheStr, newEnabledIMIs)
            || !TextUtils::Equals(mLastSystemLocaleString, currentSystemLocaleString)
            || mPackageChanged) {
        if (mImm != NULL) {
            mEnabledInputMethodAndSubtypesCache.Clear();
            AutoPtr<IObjectContainer> imis;
            mImm->GetEnabledInputMethodList((IObjectContainer**)&imis);

            AutoPtr<IObjectEnumerator> emu;
            imis->GetObjectEnumerator((IObjectEnumerator**)&emu);
            Boolean hasNext;
            while ((emu->MoveNext(&hasNext), hasNext)) {
                AutoPtr<IInterface> tmpObj;
                emu->Current((IInterface**)&tmpObj);
                AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(tmpObj.Get());

                AutoPtr<IObjectContainer> subtypes;
                mImm->GetEnabledInputMethodSubtypeList(imi, TRUE, (IObjectContainer**)&subtypes);
                mEnabledInputMethodAndSubtypesCache[imi] = subtypes;
            }
            mEnabledInputMethodAndSubtypesCacheStr = newEnabledIMIs;
            mPackageChanged = FALSE;
            mLastSystemLocaleString = currentSystemLocaleString;
        }
    }
    return mEnabledInputMethodAndSubtypesCache;
}

AutoPtr<IInputMethodInfo> InputMethodsPanel::GetCurrentInputMethodInfo()
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context = GetContext();
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    String curInputMethodId;
    ss->GetString(resolver, ISettingsSecure::DEFAULT_INPUT_METHOD, &curInputMethodId);

    String id;

    // 1. Search IMI in cache
    InputMethodCacheMap::Iterator it = mEnabledInputMethodAndSubtypesCache.Begin();
    for (; it != mEnabledInputMethodAndSubtypesCache.End(); ++it) {
        AutoPtr<IInputMethodInfo> imi = it->mFirst;
        imi->GetId(&id);
        if (id.Equals(curInputMethodId))
            return imi;
    }

    // 2. Get current enabled IMEs and search IMI
    InputMethodCacheMap& cachedMap = GetEnabledInputMethodAndSubtypeList();
    for (it = cachedMap.Begin(); it != cachedMap.End(); ++it) {
        AutoPtr<IInputMethodInfo> imi = it->mFirst;
        imi->GetId(&id);
        if (id.Equals(curInputMethodId))
            return imi;
    }

    return NULL;
}

AutoPtr<ICharSequence> InputMethodsPanel::GetIMIName(
    /* [in] */ IInputMethodInfo* imi)
{
    if (imi == NULL) return NULL;
    AutoPtr<ICharSequence> seq;
    imi->LoadLabel(mPackageManager, (ICharSequence**)&seq);
    return seq;
}

AutoPtr<ICharSequence> InputMethodsPanel::GetSubtypeName(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL || subtype == NULL) return NULL;
    // if (DEBUG) {
    //     Log.d(TAG, "Get text from: " + imi.getPackageName() + subtype.getNameResId()
    //             + appInfo);
    // }
    String pkgName;
    imi->GetPackageName(&pkgName);
    AutoPtr<IServiceInfo> serviceInfo;
    imi->GetServiceInfo((IServiceInfo**)&serviceInfo);
    AutoPtr<IApplicationInfo> appInfo;
    serviceInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<ICharSequence> seq;
    subtype->GetDisplayName(mContext, pkgName, appInfo, (ICharSequence**)&seq);
    return seq;
}

AutoPtr<IDrawable> InputMethodsPanel::GetSubtypeIcon(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi != NULL) {
        String pkgName;
        imi->GetPackageName(&pkgName);
        AutoPtr<IServiceInfo> serviceInfo;
        imi->GetServiceInfo((IServiceInfo**)&serviceInfo);
        AutoPtr<IApplicationInfo> appInfo;
        serviceInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);

        // if (DEBUG) {
        //     Log.d(TAG, "Update icons of IME: " + pkgName);
        //     if (subtype != NULL) {
        //         Log.d(TAG, "subtype =" + subtype.getLocale() + "," + subtype.getMode());
        //     }
        // }

        AutoPtr<IDrawable> drawable;
        Int32 resId;
        if (subtype != NULL) {
            subtype->GetIconResId(&resId);
            mPackageManager->GetDrawable(pkgName, resId, appInfo, (IDrawable**)&drawable);
            return drawable;
        }

        Int32 subtypeCount;
        imi->GetSubtypeCount(&subtypeCount);
        if (subtypeCount > 0) {
            AutoPtr<IInputMethodSubtype> st;
            imi->GetSubtypeAt(0, (IInputMethodSubtype**)&st);
            st->GetIconResId(&resId);
            mPackageManager->GetDrawable(pkgName, resId, appInfo, (IDrawable**)&drawable);
            return drawable;
        }
        else {
            // try {
            appInfo = NULL;
            mPackageManager->GetApplicationInfo(pkgName, 0, (IApplicationInfo**)&appInfo);
            appInfo->LoadIcon(mPackageManager, (IDrawable**)&drawable);
            return drawable;
            // } catch (PackageManager.NameNotFoundException e) {
            //     Log.w(TAG, "IME can't be found: " + pkgName);
            // }
        }
    }
    return NULL;
}

void InputMethodsPanel::OnPackageChanged()
{
    // if (DEBUG) {
    //     Log.d(TAG, "onPackageChanged.");
    // }
    mPackageChanged = TRUE;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
