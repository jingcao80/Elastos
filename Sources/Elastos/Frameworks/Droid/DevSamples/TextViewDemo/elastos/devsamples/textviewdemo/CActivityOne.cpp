
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Wifi.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "CActivityOne.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::Animation::IRotateAnimation;
using Elastos::Droid::View::Animation::CRotateAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::ITranslateAnimation;
using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::View::Animation::IScaleAnimation;
using Elastos::Droid::View::Animation::CScaleAnimation;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ITextView;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConfigurationGroupCipher;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::IWifiConfigurationPairwiseCipher;
using Elastos::Droid::Wifi::IWifiConfigurationProtocol;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace TextViewDemo {

static const String DBG_TAG("CActivityOne");

//=======================================================================
// CActivityOne::MyListener
//=======================================================================

CAR_INTERFACE_IMPL_7(CActivityOne::MyListener, Object, IViewOnTouchListener, IViewOnKeyListener,
        IViewOnClickListener, IAdapterViewOnItemClickListener, IAdapterViewOnItemLongClickListener,
        IDialogInterfaceOnClickListener, IRadioGroupOnCheckedChangeListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::MyListener::~MyListener()
{
    Logger::D("CActivityOne::MyListener", "destory ~MyListener(): %p", this);
}

ECode CActivityOne::MyListener::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Logger::D("CActivityOne", "OnTouch");
    Int32 id;
    view->GetId(&id);

    if (id == R::id::myTextView) {
        ITextView::Probe(view)->SetText(CoreUtils::Convert("中文"));
    }

    if (result) {
        *result = FALSE;;
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Logger::D("CActivityOne", "OnItemClick position = %d, id = %lld", position, id);

    StringBuilder sb("Choosed item ");
    sb += position;
    mHost->ShowAlert(sb.ToString());
    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Logger::D("CActivityOne", "OnItemLongClick position = %d, id = %lld", position, id);
    *result = TRUE;
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    Logger::D("CActivityOne", "CActivityOne::MyListener::OnClick %08x", id);

    if (id == R::id::DialogButton) {
        Logger::D("CActivityOne", "Show Dilaog!");
        mHost->ShowDialog(0);
    }
    else if (id == R::id::PopupWindowButton) {
        Logger::D("CActivityOne", "Show PopupWindow!");
        mHost->OnCreatePopupWindow();
    }
    else if (id == R::id::PowerManagerButton) {
        Logger::D("CActivityOne", "Test PowerManager!");
        mHost->OnTestPowerManager();
    }
    else if (id == R::id::ConnectivityManagerButton) {
        Logger::D("CActivityOne", "Test ConnectivityManager!");
        mHost->OnTestConnectivityManager();
    }
    else if (id == R::id::btn_close_popup) {
        Logger::D("CActivityOne", "Dismiss PopupWindow!");
        mHost->mPopupWindow->Dismiss();
        mHost->mPopupWindow = NULL;
        return NOERROR;
    }
    else if (id == R::id::AnamtionButton) {
        Logger::D("CActivityOne", "AnimationButton");

        static Int32 count = 0;
        count = count % 4;
        if (count == 0) {
            mHost->mDialogButton->StartAnimation(mHost->mAlphaAnimation);
        }
        else if (count == 1) {
            mHost->mDialogButton->StartAnimation(mHost->mRotateAnimation);
        }
        else if (count == 2) {
            mHost->mDialogButton->StartAnimation(mHost->mScaleAnimation);
        }
        else {
            mHost->mDialogButton->StartAnimation(mHost->mTranslateAnimation);
        }
        count++;
    }
    else if (id == R::id::chkAndroid) {
        Logger::D("CActivityOne", "Click Android CheckBox");
        ICheckable* checkable = ICheckable::Probe(mHost->mAndroidCheckBox);
        checkable->SetChecked(TRUE);

        checkable = ICheckable::Probe(mHost->mIosCheckBox);
        checkable->SetChecked(FALSE);
    }
    else if (id == R::id::chkIos) {
        Logger::D("CActivityOne", "Click iOS CheckBox");
        ICheckable* checkable = ICheckable::Probe(mHost->mAndroidCheckBox);
        checkable->SetChecked(FALSE);

        checkable = ICheckable::Probe(mHost->mIosCheckBox);
        checkable->SetChecked(TRUE);
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::D("CActivityOne", "CActivityOne::MyListener::OnClick with IDialogInterface");

    switch (which) {
        case IDialogInterface::BUTTON_POSITIVE:
            Logger::D("CActivityOne", "点击了确定按钮");
            break;
        case IDialogInterface::BUTTON_NEGATIVE:
            Logger::D("CActivityOne", "点击了取消按钮");
            break;
        case IDialogInterface::BUTTON_NEUTRAL:
            Logger::D("CActivityOne", "点击了中立按钮");
            break;
        default:
            break;
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnKey(
    /* [in] */ IView * view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    view->GetId(&id);

    Int32 action;
    event->GetAction(&action);
    Char32 label;
    event->GetDisplayLabel(&label);

    Logger::D("CActivityOne", "CActivityOne::MyListener::OnKey: view %08x, keyCode: %d, '%c'",
        id, keyCode, (char)label);

    if (action == IKeyEvent::ACTION_DOWN) {
        if (keyCode == IKeyEvent::KEYCODE_0) {
            Logger::D("CActivityOne", "Key 0 is down.");
        }
    }
    else if (action == IKeyEvent::ACTION_UP) {
    }

    if (result) {
        *result = FALSE;
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
    /* [in] */  IRadioGroup* group,
    /* [in] */  Int32 checkedId)
{
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(DBG_TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);

    // Setup ContentView
    //
    SetContentView(R::layout::main);

    // Setup TextView
    //
    AutoPtr<IView> temp = FindViewById(R::id::myTextView);
    mTextView = ITextView::Probe(temp);
    assert(mTextView != NULL);

    temp = FindViewById(R::id::myEditText);
    mEditText = IEditText::Probe(temp);
    assert(mEditText != NULL);
    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnKeyListener* keyListener = (IViewOnKeyListener*)l.Get();
    IView::Probe(mEditText)->SetOnKeyListener(keyListener);

    // Setup ListView
    //
    temp = FindViewById(R::id::myListView);
    mListView = IListView::Probe(temp);
    assert(mListView != NULL);

    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    for (Int32 i = 0; i < 15; ++i) {
        StringBuilder sb("Item ");
        sb += i;
        list->Add(sb.ToCharSequence());
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, list, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    AutoPtr<IAdapterView> ad = IAdapterView::Probe(mListView);
    ad->SetAdapter(IAdapter::Probe(adapter));

    //TEST SimpleAdapter
    {
        AutoPtr<IList> listData;
        CArrayList::New((IList**)&listData);

        String key1("key1");
        String key2("key2");
        for (Int32 i = 0; i < 5; i++) {
            AutoPtr<IMap> map;
            CHashMap::New((IMap**)&map);

            StringBuilder sb1("Itemkey1 ");
            sb1 += i;
            map->Put(CoreUtils::Convert(key1), sb1.ToCharSequence());

            StringBuilder sb2("Itemkey2 ");
            sb2 += i;
            map->Put(CoreUtils::Convert(key2), sb2.ToCharSequence());
            listData->Add(map);
        }

        AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(2);
        (*from)[0] = key1;
        (*from)[1] = key2;

        AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(2);
        (*to)[0] = R::id::textview1;
        (*to)[1] = R::id::textview2;

        AutoPtr<ISimpleAdapter> siAdapter;
        CSimpleAdapter::New(this, listData, R::layout::view, from, to,
                (ISimpleAdapter**)&siAdapter);
        assert(siAdapter != NULL);
        ad->SetAdapter(IAdapter::Probe(siAdapter));
    }
    //TEST SimpleAdapter

    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mListView->SetDivider(IDrawable::Probe(drawable));
    mListView->SetDividerHeight(1);

    IAdapterViewOnItemClickListener* itemClickListener = (IAdapterViewOnItemClickListener*)l.Get();
    ad->SetOnItemClickListener(itemClickListener);

    IAdapterViewOnItemLongClickListener* itemLongClickListener = (IAdapterViewOnItemLongClickListener*)l.Get();
    ad->SetOnItemLongClickListener(itemLongClickListener);

    // Setup Buttons
    //
    mAnimationButton = FindViewById(R::id::AnamtionButton);
    assert(mAnimationButton != NULL);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)l.Get();
    mAnimationButton->SetOnClickListener(clickListener);

    mDialogButton = FindViewById(R::id::DialogButton);
    assert(mDialogButton != NULL);
    mDialogButton->SetOnClickListener(clickListener);
    mDialogButton->SetOnKeyListener(keyListener);

    mPopupWindowButton = FindViewById(R::id::PopupWindowButton);
    assert(mPopupWindowButton != NULL);
    mPopupWindowButton->SetOnClickListener(clickListener);

    mPowerManagerButton = FindViewById(R::id::PowerManagerButton);
    assert(mPowerManagerButton != NULL);
    mPowerManagerButton->SetOnClickListener(clickListener);

    mConnectivityManagerButton = FindViewById(R::id::ConnectivityManagerButton);
    assert(mConnectivityManagerButton != NULL);
    mConnectivityManagerButton->SetOnClickListener(clickListener);

    // Setup CheckBox
    //
    temp = FindViewById(R::id::chkAndroid);
    mAndroidCheckBox = ICheckBox::Probe(temp);
    IView::Probe(mAndroidCheckBox)->SetOnClickListener(clickListener);

    temp = FindViewById(R::id::chkIos);
    mIosCheckBox = ICheckBox::Probe(temp);
    IView::Probe(mIosCheckBox)->SetOnClickListener(clickListener);

   // RegisterForContextMenu(view);

   // AutoPtr<IViewParent> parent;
   // view->GetParent((IViewParent**)&parent);
   // mContent = IView::Probe(parent);
   // mContent->SetOnKeyListener(keyListener);

    // Setup Animations
    //
    CAlphaAnimation::New(0.3f, 1.0f, (IAnimation**)&mAlphaAnimation);
    mAlphaAnimation->SetDuration(3000);

    CRotateAnimation::New(0.0f, +350.0f, IAnimation::RELATIVE_TO_SELF,
            0.5f,IAnimation::RELATIVE_TO_SELF, 0.5f,
            (IAnimation**)&mRotateAnimation);
    mRotateAnimation->SetDuration(3000);

    CScaleAnimation::New(0.2f, 1.4f, 0.2f, 1.4f, IAnimation::RELATIVE_TO_SELF,
            0.5f, IAnimation::RELATIVE_TO_SELF, 0.5f,
            (IAnimation**)&mScaleAnimation);
    mScaleAnimation->SetDuration(3000);

    CTranslateAnimation::New(300.0f, -20.0f, -10.0f, 30.0f,
            (IAnimation**)&mTranslateAnimation);
    mTranslateAnimation->SetDuration(3000);

//    CreateNavigationBar();

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(DBG_TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

ECode CActivityOne::ShowAlert(
    /* [in] */ const String& info)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);

    builder->SetTitle(CoreUtils::Convert("Bingo!"));

    builder->SetMessage(CoreUtils::Convert(info));

    // Setup Button
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IDialogInterfaceOnClickListener* clickListener = (IDialogInterfaceOnClickListener*)l.Get();

    builder->SetPositiveButton(CoreUtils::Convert("确定"), clickListener);

    // Create Dialog
    //
    AutoPtr<IAlertDialog> dlg;
    builder->Create((IAlertDialog**)&dlg);

    // Show Dialog
    //
    IDialog::Probe(dlg)->Show();
    return NOERROR;
}

AutoPtr<IDialog> CActivityOne::OnCreateDialog(
   /* [in] */ Int32 id)
{
    Logger::D("CActivityOne", "CActivityOne::OnCreateDialog");

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);

    builder->SetTitle(CoreUtils::Convert("普通对话框"));

    builder->SetMessage(CoreUtils::Convert("这是一个普通对话框"));

    // Setup Button
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IDialogInterfaceOnClickListener* clickListener = (IDialogInterfaceOnClickListener*)l.Get();

    builder->SetPositiveButton(CoreUtils::Convert("确定"), clickListener);

    builder->SetNeutralButton(CoreUtils::Convert("中立"), clickListener);

    builder->SetNegativeButton(CoreUtils::Convert("取消"), clickListener);

    // Create Dialog
    //
    AutoPtr<IAlertDialog> dlg;
    builder->Create((IAlertDialog**)&dlg);

//    IDialogInterface* di = IDialogInterface::Probe(dlg);
    return IDialog::Probe(dlg);
}

ECode CActivityOne::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    Logger::D("CActivityOne", "CActivityOne::OnCreateContextMenu");

    AutoPtr<IMenu> menuObj = IMenu::Probe(menu);

    AutoPtr<IMenuItem> item;
    menuObj->Add(0, 1, 1, CoreUtils::Convert("ctxItem1"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 2, 2, CoreUtils::Convert("ctxItem2"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 3, 3, CoreUtils::Convert("ctxItem3"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 4, 4, CoreUtils::Convert("ctxItem4"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 5, 5, CoreUtils::Convert("ctxItem5"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 6, 6, CoreUtils::Convert("ctxItem6"), (IMenuItem**)&item);

    item = NULL;
    menuObj->Add(0, 7, 7, CoreUtils::Convert("ctxItem7"), (IMenuItem**)&item);

    return NOERROR;
}

ECode CActivityOne::CreateNavigationBar()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> navigationBar;
    inflater->Inflate(R::layout::navigationbar, NULL, (IView**)&navigationBar);

    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR,
            0 | IWindowManagerLayoutParams::FLAG_TOUCHABLE_WHEN_WAKING
            | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IPixelFormat::OPAQUE, (IWindowManagerLayoutParams**)&lp);

    lp->SetGravity(IGravity::BOTTOM | IGravity::FILL_HORIZONTAL);
    lp->SetTitle(CoreUtils::Convert("NavigationBar"));

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)l.Get();

    navigationBar->FindViewById(R::id::menu, (IView**)&mBack);
    assert(mBack != NULL);
    mBack->SetOnClickListener(clickListener);

    navigationBar->FindViewById(R::id::back, (IView**)&mHome);
    assert(mHome != NULL);
    mHome->SetOnClickListener(clickListener);

    navigationBar->FindViewById(R::id::home, (IView**)&mMenu);
    assert(mMenu != NULL);
    mMenu->SetOnClickListener(clickListener);

//    AutoPtr<IWindowManager> wm;
//    CWindowManagerImpl::AcquireSingleton((IWindowManager**)&wm);
//    wm->AddView(navigationBar, lp);

    return NOERROR;
}

ECode CActivityOne::OnCreatePopupWindow()
{
    Logger::D("CActivityOne", "CActivityOne::OnCreatePopupWindow()");
    mPopupWindow = NULL;

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::popupwindow, NULL, (IView**)&layout);

    CPopupWindow::New(layout, 350, 350, TRUE, (IPopupWindow**)&mPopupWindow);
    mPopupWindow->ShowAtLocation(layout, IGravity::CENTER, 0, 0);

    // Setup TextView
    //
    AutoPtr<IView> temp;
    layout->FindViewById(R::id::txtView, (IView**)&temp);
    AutoPtr<ITextView> textView = ITextView::Probe(temp);
    assert(textView != NULL);

    textView->SetText(CoreUtils::Convert("PopupWindow 测试程序!"));

    // Setup Button
    //
    AutoPtr<IView> dismissButton;
    layout->FindViewById(R::id::btn_close_popup, (IView**)&dismissButton);
    assert(dismissButton != NULL);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)l.Get();
    dismissButton->SetOnClickListener(clickListener);

    return NOERROR;
}

ECode CActivityOne::OnTestPowerManager()
{
    // AutoPtr<IInterface> obj;
    // GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    // AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    // assert(pm != NULL);
    // AutoPtr<IPowerManagerWakeLock> wl;
    // pm->NewWakeLock(IPowerManager::SCREEN_DIM_WAKE_LOCK, String("My Tag"), (IPowerManagerWakeLock**)&wl);
    // assert(wl != NULL);
    // wl->AcquireLock();
    // //  ..screen will stay on during this section..
    // wl->ReleaseLock();

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    AutoPtr<IWifiManager> wifi = IWifiManager::Probe(obj);
    assert(wifi != NULL);

    Boolean result;
    wifi->SetWifiEnabled(TRUE, &result);
    Logger::D("CActivityOne", "SetWifiEnabled %s", result ? "succeeded!" : "failed!");

    // sleep(1);

    wifi->StartScan(&result);
    Logger::D("CActivityOne", "StartScan %s", result ? "succeeded!" : "failed!");

    sleep(5);

    AutoPtr<IList> list;
    wifi->GetScanResults((IList**)&list);
    Logger::D("CActivityOne", "GetScanResults %s", list != NULL ? "succeeded!" : "failed!");

    if (list != NULL) {
        Int32 size;
        list->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            list->Get(i, (IInterface**)&obj);
            IScanResult* result = IScanResult::Probe(obj);

            String ssid, bssid;
            result->GetSSID(&ssid);
            result->GetBSSID(&bssid);
            Logger::D("CActivityOne", "ssid: %s, bssid: %s", ssid.string(), bssid.string());
        }
    }

    AutoPtr<IWifiConfiguration> wifiConfig;
    CWifiConfiguration::New((IWifiConfiguration**)&wifiConfig);
    wifiConfig->SetSSID(String("\"Wireless-Kortide\""));
    wifiConfig->SetPreSharedKey(String("\"Elastos2011\""));
    AutoPtr<IBitSet> groupCiphers, keyMgmt, pairwiseCiphers, protocols;
    wifiConfig->GetAllowedGroupCiphers((IBitSet**)&groupCiphers);
    groupCiphers->Set(IWifiConfigurationGroupCipher::TKIP);
    groupCiphers->Set(IWifiConfigurationGroupCipher::CCMP);
    wifiConfig->GetAllowedKeyManagement((IBitSet**)&keyMgmt);
    keyMgmt->Set(IWifiConfigurationKeyMgmt::WPA_PSK);
    wifiConfig->GetAllowedPairwiseCiphers((IBitSet**)&pairwiseCiphers);
    pairwiseCiphers->Set(IWifiConfigurationPairwiseCipher::TKIP);
    pairwiseCiphers->Set(IWifiConfigurationPairwiseCipher::CCMP);
    wifiConfig->GetAllowedProtocols((IBitSet**)&protocols);
    protocols->Set(IWifiConfigurationProtocol::RSN);
    Int32 netId;
    ECode ec = wifi->AddNetwork(wifiConfig, &netId);
    Logger::D("CActivityOne", "AddNetwork %s, netId: %d", SUCCEEDED(ec) ? "succeeded!" : "failed!", netId);

    wifi->EnableNetwork(netId, TRUE, &result);
    Logger::D("CActivityOne", "EnableNetwork %s", result ? "succeeded!" : "failed!");

    return NOERROR;
}

ECode CActivityOne::OnTestConnectivityManager()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> conn = IConnectivityManager::Probe(obj);
    assert(conn != NULL);

    AutoPtr<INetworkInfo> info;
    conn->GetNetworkInfo(IConnectivityManager::TYPE_WIFI, (INetworkInfo**)&info);

    Logger::D("CActivityOne", "NetworkInfo: %s", TO_CSTR(info));
    return NOERROR;
}

} // namespace TextViewDemo
} // namespace DevSamples
} // namespace Elastos
