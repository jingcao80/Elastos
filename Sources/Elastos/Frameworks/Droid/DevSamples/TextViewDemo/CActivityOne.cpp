
#include "CActivityOne.h"
#include "R.h"
#include <R.h>
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <unistd.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IBitSet;
using Elastos::Utility::CObjectStringMap;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConfigurationGroupCipher;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::IWifiConfigurationPairwiseCipher;
using Elastos::Droid::Wifi::IWifiConfigurationProtocol;
using Elastos::Droid::Net::Ethernet::IEthernetManager;
using Elastos::Droid::Net::Ethernet::CEthernetManager;
using Elastos::Droid::Net::Ethernet::IEthernetDevInfo;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
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
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ITextView;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TextViewDemo {


CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

CActivityOne::MyListener::~MyListener()
{
    Slogger::D("CActivityOne::MyListener", "destory ~MyListener(): %p", this);
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnTouchListener) {
        return (IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnKeyListener) {
        return (IViewOnKeyListener*)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return (IAdapterViewOnItemClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_IDialogInterfaceOnClickListener) {
        return (IDialogInterfaceOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Slogger::D("CActivityOne", "OnTouch");
    Int32 id;
    view->GetId(&id);

    if (id == R::id::myTextView) {
        AutoPtr<ICharSequence> csq;
        CStringWrapper::New(String("中文"), (ICharSequence**)&csq);
        ITextView::Probe(view)->SetText(csq);
    }

    if (result) {
        *result = false;;
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Slogger::D("CActivityOne", "OnItemClick position = %d, id = %lld", position, id);

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
    Slogger::D("CActivityOne", "OnItemLongClick position = %d, id = %lld", position, id);
    *result = TRUE;
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    Slogger::D("CActivityOne", "CActivityOne::MyListener::OnClick %08x", id);

    if (id == R::id::DialogButton) {
        Slogger::D("CActivityOne", "Show Dilaog!");
        mHost->ShowDialog(0);
    }
    else if (id == R::id::PopupWindowButton) {
        Slogger::D("CActivityOne", "Show PopupWindow!");
        mHost->OnCreatePopupWindow();
    }
    else if (id == R::id::PowerManagerButton) {
        Slogger::D("CActivityOne", "Test PowerManager!");
        mHost->OnTestPowerManager();
    }
    else if (id == R::id::ConnectivityManagerButton) {
        Slogger::D("CActivityOne", "Test ConnectivityManager!");
        mHost->OnTestConnectivityManager();
    }
    else if (id == R::id::btn_close_popup) {
        Slogger::D("CActivityOne", "Dismiss PopupWindow!");
        mHost->mPopupWindow->Dismiss();
        mHost->mPopupWindow = NULL;
        return NOERROR;
    }
    else if (id == R::id::AnamtionButton) {
        Slogger::D("CActivityOne", "AnimationButton");

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
        Slogger::D("CActivityOne", "Click Android CheckBox");
        ICheckable* checkable = ICheckable::Probe(mHost->mAndroidCheckBox.Get());
        checkable->SetChecked(TRUE);

        checkable = ICheckable::Probe(mHost->mIosCheckBox.Get());
        checkable->SetChecked(FALSE);
    }
    else if (id == R::id::chkIos) {
        Slogger::D("CActivityOne", "Click iOS CheckBox");
        ICheckable* checkable = ICheckable::Probe(mHost->mAndroidCheckBox.Get());
        checkable->SetChecked(FALSE);

        checkable = ICheckable::Probe(mHost->mIosCheckBox.Get());
        checkable->SetChecked(TRUE);
    }

    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Slogger::D("CActivityOne", "CActivityOne::MyListener::OnClick with IDialogInterface");

    switch (which) {
    case IDialogInterface::BUTTON_POSITIVE:
        Slogger::D("CActivityOne", "点击了确定按钮");
        break;
    case IDialogInterface::BUTTON_NEGATIVE:
        Slogger::D("CActivityOne", "点击了取消按钮");
        break;
    case IDialogInterface::BUTTON_NEUTRAL:
        Slogger::D("CActivityOne", "点击了中立按钮");
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
    Int32 id;
    view->GetId(&id);

    Int32 action;
    event->GetAction(&action);
    Char32 label;
    event->GetDisplayLabel(&label);

    Slogger::D("CActivityOne", "CActivityOne::MyListener::OnKey: view %08x, keyCode: %d, '%c'",
        id, keyCode, (char)label);

    if (action == IKeyEvent::ACTION_DOWN) {
        if (keyCode == IKeyEvent::KEYCODE_0) {
            Slogger::D("CActivityOne", "Key 0 is down.");
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

CActivityOne::CActivityOne()
{
    Slogger::D("CActivityOne", "create CActivityOne(): %p", this);
}

CActivityOne::~CActivityOne()
{
    Slogger::D("CActivityOne", "destory ~CActivityOne(): %p", this);
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Slogger::D("CActivityOne", "CActivityOne::OnCreate");

    // Create Listener
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IAdapterViewOnItemClickListener* itemClickListener = (IAdapterViewOnItemClickListener*)(l->Probe(EIID_IAdapterViewOnItemClickListener));
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));
    IViewOnKeyListener* keyListener = (IViewOnKeyListener*)(l->Probe(EIID_IViewOnKeyListener));

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
    mEditText->SetOnKeyListener(keyListener);

    // Setup ListView
    //
    temp = FindViewById(R::id::myListView);
    mListView = IListView::Probe(temp);
    assert(mListView != NULL);

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    for (Int32 i = 0; i < 15; ++i) {
        StringBuilder sb("Item ");
        sb += i;
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    mListView->SetAdapter(adapter.Get());


    //TEST SimpleAdapter
    #if 0
    {
        AutoPtr<IObjectContainer> container;
        CParcelableObjectContainer::New((IObjectContainer**)&container);

        String key1("key1");
        String key2("key2");
        for (Int32 i = 0; i < 5; i++) {
            AutoPtr<IObjectStringMap> item;
            CObjectStringMap::New((IObjectStringMap**)&item);

            StringBuilder sb("Itemkey1 ");
            sb += i;
            AutoPtr<ICharSequence> seq = sb.ToCharSequence();
            item->Put(key1, seq);

            StringBuilder sb2("Itemkey2 ");
            sb2 += i;
            AutoPtr<ICharSequence> seq2 = sb2.ToCharSequence();
            item->Put(key2, seq2);
            container->Add(item);
        }

        AutoPtr<ISimpleAdapter> adapter;

        AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(2);
        (*from)[0] = key1;
        (*from)[1] = key2;

        AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(2);
        (*to)[0] = R::id::textview1;
        (*to)[1] = R::id::textview2;
        CSimpleAdapter::New(this, container, R::layout::view, from, to, (ISimpleAdapter**)&adapter);
        assert(adapter != NULL);
        mListView->SetAdapter(adapter.Get());
    }
    #endif //TEST SimpleAdapter


    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mListView->SetDivider(drawable);
    mListView->SetDividerHeight(1);
    mListView->SetOnItemClickListener(itemClickListener);
    // mListView->SetOnItemLongClickListener(l);

    // Setup Buttons
    //
    mAnimationButton = FindViewById(R::id::AnamtionButton);
    assert(mAnimationButton != NULL);
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
    mAndroidCheckBox = ICheckBox::Probe(temp.Get());
    mAndroidCheckBox->SetOnClickListener(clickListener);

    temp = FindViewById(R::id::chkIos);
    mIosCheckBox = ICheckBox::Probe(temp.Get());
    mIosCheckBox->SetOnClickListener(clickListener);

//
//    RegisterForContextMenu(view.Get());
//
//    AutoPtr<IViewParent> parent;
//    view->GetParent((IViewParent**)&parent);
//    mContent = IView::Probe(parent);
//    mContent->SetOnKeyListener(keyListener);

    // Setup Animations
    //
    CAlphaAnimation::New(0.3f, 1.0f, (IAlphaAnimation**)&mAlphaAnimation);
    mAlphaAnimation->SetDuration(3000);

    CRotateAnimation::New(0.0f, +350.0f, IAnimation::RELATIVE_TO_SELF,
        0.5f,IAnimation::RELATIVE_TO_SELF, 0.5f, (IRotateAnimation**)&mRotateAnimation);
    mRotateAnimation->SetDuration(3000);

    CScaleAnimation::New(0.2f, 1.4f, 0.2f, 1.4f, IAnimation::RELATIVE_TO_SELF,
        0.5f, IAnimation::RELATIVE_TO_SELF, 0.5f, (IScaleAnimation**)&mScaleAnimation);
    mScaleAnimation->SetDuration(3000);

    CTranslateAnimation::New(300.0f, -20.0f, -10.0f, 30.0f,
        (ITranslateAnimation**)&mTranslateAnimation);
    mTranslateAnimation->SetDuration(3000);

//    CreateNavigationBar();

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Slogger::D("CActivityOne", "CActivityOne::OnStart");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    Slogger::D("CActivityOne", "CActivityOne::OnResume");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Slogger::D("CActivityOne", "CActivityOne::OnPause");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Slogger::D("CActivityOne", "CActivityOne::OnStop");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Slogger::D("CActivityOne", "CActivityOne::OnDestroy");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Slogger::D("CActivityOne", "CActivityOne::OnActivityResult");
    return NOERROR;
}

ECode CActivityOne::ShowAlert(
    /* [in] */ const String& info)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(THIS_PROBE(IContext), (IAlertDialogBuilder**)&builder);

    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("Bingo!"), (ICharSequence**)&cs);
    builder->SetTitle(cs);

    cs = NULL;
    CStringWrapper::New(info, (ICharSequence**)&cs);
    builder->SetMessage(cs);

    // Setup Button
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IDialogInterfaceOnClickListener* clickListener = (IDialogInterfaceOnClickListener*)l.Get();

    cs = NULL;
    CStringWrapper::New(String("确定"), (ICharSequence**)&cs);
    builder->SetPositiveButton(cs, clickListener);

    // Create Dialog
    //
    AutoPtr<IAlertDialog> dlg;
    builder->Create((IAlertDialog**)&dlg);

    // Show Dialog
    //
    dlg->Show();
    return NOERROR;
}

AutoPtr<IDialog> CActivityOne::OnCreateDialog(
   /* [in] */ Int32 id)
{
    Slogger::D("CActivityOne", "CActivityOne::OnCreateDialog");
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(THIS_PROBE(IContext), (IAlertDialogBuilder**)&builder);

    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("普通对话框"), (ICharSequence**)&cs);
    builder->SetTitle(cs);

    cs = NULL;
    CStringWrapper::New(String("这是一个普通对话框"), (ICharSequence**)&cs);
    builder->SetMessage(cs);

    // Setup Button
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IDialogInterfaceOnClickListener* clickListener = (IDialogInterfaceOnClickListener*)l.Get();

    cs = NULL;
    CStringWrapper::New(String("确定"), (ICharSequence**)&cs);
    builder->SetPositiveButton(cs, clickListener);

    cs = NULL;
    CStringWrapper::New(String("中立"), (ICharSequence**)&cs);
    builder->SetNeutralButton(cs, clickListener);

    cs = NULL;
    CStringWrapper::New(String("取消"), (ICharSequence**)&cs);
    builder->SetNegativeButton(cs, clickListener);

    // Create Dialog
    //
    AutoPtr<IAlertDialog> dlg;
    builder->Create((IAlertDialog**)&dlg);

//    IDialogInterface* di = IDialogInterface::Probe(dlg);
    return dlg;
}

ECode CActivityOne::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    Slogger::D("CActivityOne", "CActivityOne::OnCreateContextMenu");

    AutoPtr<IMenuItem> item = NULL;
    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(String("ctxItem1"), (ICharSequence**)&csq);
    menu->Add(0, 1, 1, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem2"), (ICharSequence**)&csq);
    menu->Add(0, 2, 2, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem3"), (ICharSequence**)&csq);
    menu->Add(0, 3, 3, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem4"), (ICharSequence**)&csq);
    menu->Add(0, 4, 4, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem5"), (ICharSequence**)&csq);
    menu->Add(0, 5, 5, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem6"), (ICharSequence**)&csq);
    menu->Add(0, 6, 6, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem7"), (ICharSequence**)&csq);
    menu->Add(0, 7, 7, csq, (IMenuItem**)&item);

    return NOERROR;
}

ECode CActivityOne::CreateNavigationBar()
{
    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

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
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("NavigationBar"), (ICharSequence**)&cs);
    lp->SetTitle(cs);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

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
    Slogger::D("CActivityOne", "CActivityOne::OnCreatePopupWindow()");
    mPopupWindow = NULL;

    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::popupwindow, NULL, (IView**)&layout);

    CPopupWindow::New(layout, 350, 350, TRUE, (IPopupWindow**)&mPopupWindow);
    mPopupWindow->ShowAtLocation(layout, IGravity::CENTER, 0, 0);

    // Setup TextView
    //
    AutoPtr<IView> temp;
    layout->FindViewById(R::id::txtView, (IView**)&temp);
    AutoPtr<ITextView> textView = ITextView::Probe(temp.Get());
    assert(textView != NULL);

    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("PopupWindow 测试程序!"), (ICharSequence**)&cs);
    textView->SetText(cs);

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
    // AutoPtr<IPowerManager> pm;
    // GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    // assert(pm != NULL);
    // AutoPtr<IPowerManagerWakeLock> wl;
    // pm->NewWakeLock(IPowerManager::SCREEN_DIM_WAKE_LOCK, String("My Tag"), (IPowerManagerWakeLock**)&wl);
    // assert(wl != NULL);
    // wl->AcquireLock();
    // //  ..screen will stay on during this section..
    // wl->ReleaseLock();

    AutoPtr<IWifiManager> wifi;
    GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&wifi);
    assert(wifi != NULL);

    Boolean result;
    wifi->SetWifiEnabled(TRUE, &result);
    Slogger::D("CActivityOne", "SetWifiEnabled %s", result ? "succeeded!" : "failed!");

    // sleep(1);

    wifi->StartScan(&result);
    Slogger::D("CActivityOne", "StartScan %s", result ? "succeeded!" : "failed!");

    sleep(5);

    AutoPtr<IObjectContainer> scanResults;
    wifi->GetScanResults((IObjectContainer**)&scanResults);
    Slogger::D("CActivityOne", "GetScanResults %s", scanResults != NULL ? "succeeded!" : "failed!");

    // if (scanResults != NULL) {
    //     AutoPtr<IObjectEnumerator> it;
    //     scanResults->GetObjectEnumerator((IObjectEnumerator**)&it);
    //     Boolean hasNext;
    //     while (it->MoveNext(&hasNext), hasNext) {
    //         AutoPtr<IInterface> obj;
    //         it->Current((IInterface**)&obj);
    //         IScanResult* result = IScanResult::Probe(obj);

    //         String ssid, bssid;
    //         result->GetSSID(&ssid);
    //         result->GetBSSID(&bssid);
    //         Slogger::D("CActivityOne", "ssid: %s, bssid: %s", ssid.string(), bssid.string())
    //     }
    // }

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
    Slogger::D("CActivityOne", "AddNetwork %s, netId: %d", SUCCEEDED(ec) ? "succeeded!" : "failed!", netId);

    wifi->EnableNetwork(netId, TRUE, &result);
    Slogger::D("CActivityOne", "EnableNetwork %s", result ? "succeeded!" : "failed!");

    return NOERROR;
}

ECode CActivityOne::OnTestConnectivityManager()
{
    AutoPtr<IConnectivityManager> conn;
    GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&conn);
    assert(conn != NULL);

    AutoPtr<INetworkInfo> info;
    conn->GetNetworkInfo(IConnectivityManager::TYPE_WIFI, (INetworkInfo**)&info);

    String infoStr;
    info->ToString(&infoStr);
    Slogger::D("CActivityOne", "NetworkInfo: %s", infoStr.string());
    return NOERROR;
}

} // namespace TextViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
