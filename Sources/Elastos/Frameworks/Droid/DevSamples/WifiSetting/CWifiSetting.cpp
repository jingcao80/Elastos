
#include "CWifiSetting.h"
// #include "Uri.h"
#include <elautoptr.h>
#include <stdio.h>
#include <elastos/core/StringBuffer.h>
#include <utils/SystemClock.h>
#include <unistd.h>
// #include "MyThread.h"
#include "R.h"

using namespace Elastos::Core;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiSetting {

CWifiSetting::MyListener::MyListener(
    /* [in] */ CWifiSetting* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CWifiSetting::MyListener::Probe(
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
    else if (riid == EIID_ICompoundButtonOnCheckedChangeListener) {
        return (ICompoundButtonOnCheckedChangeListener*)this;
    }

    return NULL;
}

UInt32 CWifiSetting::MyListener::AddRef()
{
    // atomic_inc of android bionic C library will return
    // the old value of mRef before it is increased
    Int32 ref = atomic_inc(&mRef);
    // so we should increase ref before return
    return ++ref;
}

UInt32 CWifiSetting::MyListener::Release()
{
    // atomic_inc of android bionic C library will return
    // the old value of mRef before it is decreased
    Int32 ref = atomic_dec(&mRef);
    // so we should decrease ref
    if (--ref == 0) {
        delete this;
    }
    assert(ref >= 0);
    return ref;
}

ECode CWifiSetting::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
        *pIID = EIID_IViewOnTouchListener;
    }
    else if (pObject == (IInterface*)(IViewOnKeyListener*)this) {
        *pIID = EIID_IViewOnKeyListener;
    }
    else if (pObject == (IInterface*)(IAdapterViewOnItemClickListener*)this) {
        *pIID = EIID_IAdapterViewOnItemClickListener;
    }
    else if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else if (pObject == (IInterface*)(IDialogInterfaceOnClickListener*)this) {
        *pIID = EIID_IDialogInterfaceOnClickListener;
    }
    else if (pObject == (IInterface*)(ICompoundButtonOnCheckedChangeListener*)this) {
        *pIID = EIID_ICompoundButtonOnCheckedChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CWifiSetting::MyListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    printf("OnTouch\n");
    return NOERROR;
}

ECode CWifiSetting::MyListener::OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id)
{
    printf("OnItemClick position = %d, id = %lld\n", position, id);
    return NOERROR;
}

ECode CWifiSetting::MyListener::OnClick(
        /* [in] */ IView* view)
{
printf("==[%s]--==[%s]--==[%d] \n", __FILE__, __FUNCTION__, __LINE__);
    if (view == mHost->mEthernetButton.Get()) {
printf("==[%s]--==[%s]--==[%d] Try to connect ethernet.\n", __FILE__, __FUNCTION__, __LINE__);
        AutoPtr<ICharSequence> state;
        String addition = String("Ethernet State: ");

        if (mHost->CheckState(CONNECTWAY_ETH)) {
            CStringWrapper::New(addition + String(" Ethernet has been connected."), (ICharSequence**)&state);
        }
        else {
            CStringWrapper::New(addition + String(" Ethernet can't be connected."), (ICharSequence**)&state);
        }

        mHost->mEthernetText->SetText(state);
    }
    else if (view == mHost->mWifiButton.Get()) {
printf("==[%s]--==[%s]--==[%d] Check wifi state.\n", __FILE__, __FUNCTION__, __LINE__);
        String addition = String("Wi-Fi State: ");
        AutoPtr<ICharSequence> state;

        if (mHost->CheckState(CONNECTWAY_WIFI)) {
            CStringWrapper::New(addition + String(" Have wifi's hot node."), (ICharSequence**)&state);
        }
        else {
            CStringWrapper::New(addition + String(" There is no wifi's hot node."), (ICharSequence**)&state);
        }

        mHost->mWifiText->SetText(state);
    }
    else if (view == mHost->mCancelButton.Get()) {
        mHost->DismissDialog(0);
    }

    return NOERROR;
}

ECode CWifiSetting::MyListener::OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which)
{
    switch (which) {
    case IDialogInterface::BUTTON_POSITIVE:
            printf("点击了确定按钮\n");
        break;
    case IDialogInterface::BUTTON_NEGATIVE:
            printf("点击了取消按钮\n");
        break;
    case IDialogInterface::BUTTON_NEUTRAL:
            printf("点击了中立按钮\n");
        break;
    default:
        break;
    }

    return NOERROR;
}

ECode CWifiSetting::MyListener::OnKey(
    /* [in] */ IView * v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 action;
    event->GetAction(&action);
    Char32 label = 0;
    event->GetDisplayLabel(&label);

    if (action == IKeyEvent::ACTION_DOWN) {
        printf("Key '%c' is down on ImageView\n", (char)label);
    }
    else if (action == IKeyEvent::ACTION_UP) {
        printf("Key '%c' is up on ImageView\n", (char)label);
    }

    if (result) {
        *result = false;
    }

    return NOERROR;
}

ECode CWifiSetting::MyListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return NOERROR;
}

CWifiSetting::CWifiSetting()
    : mScanList(NULL)
    , mIsInited(FALSE)
    , mIsConnecting(FALSE)
    , mFocusScanRes(NULL)
{}

CWifiSetting::~CWifiSetting()
{
    if (mScanList != NULL) {
        delete mScanList;
        mScanList = NULL;
    }

    if (mFocusScanRes != NULL) {
        delete mFocusScanRes;
        mFocusScanRes = NULL;
    }
}

Boolean CWifiSetting::CheckState(
    /* [in] */ CONNECTWAY way)
{
    //Try to connect ethernet.
    if (way == CONNECTWAY_ETH) {
        printf("==[%s]--==[%s]--==[%d] [Check Net State---Ethernet, please wait...]\n", __FILE__, __FUNCTION__, __LINE__);
        //1. check ethernet
        Boolean connected = Ethernet::RunDhcpEthe();
        if (connected) {
            printf("==[%s]--==[%s]--==[%d] Ethernet Connected.\n", __FILE__, __FUNCTION__, __LINE__);
        }
        else {
            printf("==[%s]--==[%s]--==[%d] Ethernet Net Connected.\n", __FILE__, __FUNCTION__, __LINE__);
        }
        return connected;
    }
    //Check wifi net state
    else if (way == CONNECTWAY_WIFI) {
        printf("\n\n==[%s]--==[%s]--==[%d] [Check Net State---Wifi, please wait...]\n", __FILE__, __FUNCTION__, __LINE__);
        ElWifi_Start();

        std::vector<ScanResult*>* scanList = ElWifi_ScanResults();
        if (scanList != NULL) {
            printf("==[%s]--==[%s]--==[%d], [WIFI]--Has hot node.\n", __FILE__, __FUNCTION__, __LINE__);

            std::vector<ScanResult*>::iterator node = scanList->begin();
            for (; node != scanList->end(); ++node) {
                ScanResult* res = *node;
                assert(res != NULL);
                printf("==[%s]--==[%s]--==[%d], ssid = [%s] \n", __FILE__, __FUNCTION__, __LINE__, res->ssid);
            }
            delete scanList;
            return TRUE;
        }
        else {
            printf("==[%s]--==[%s]--==[%d], [WIFI]--Doesn't has hot node.\n", __FILE__, __FUNCTION__, __LINE__);
            return FALSE;
        }
    }
    else {
        printf("==[%s]--==[%s]--==[%d] [Check Net State---Ethernet, please wait...]\n", __FILE__, __FUNCTION__, __LINE__);
        //1. check ethernet
        Boolean connected = Ethernet::RunDhcpEthe();
        if (connected) {
            printf("==[%s]--==[%s]--==[%d] Ethernet Connected.\n", __FILE__, __FUNCTION__, __LINE__);
            return TRUE;
        }
        else {
            printf("==[%s]--==[%s]--==[%d] Ethernet Net Connected.\n", __FILE__, __FUNCTION__, __LINE__);
        }

        printf("\n\n==[%s]--==[%s]--==[%d] [Check Net State---Wifi, please wait...]\n", __FILE__, __FUNCTION__, __LINE__);
        //2. check wifi
        ElWifi_Start();

        std::vector<ScanResult*>* scanList = ElWifi_ScanResults();
        if (scanList != NULL) {
            printf("==[%s]--==[%s]--==[%d], [WIFI]--Has hot node.\n", __FILE__, __FUNCTION__, __LINE__);
            return TRUE;
        }
        else {
            printf("==[%s]--==[%s]--==[%d], [WIFI]--Doesn't has hot node.\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    return FALSE;
}

void CWifiSetting::Connect(
    /* [in] */ CONNECTWAY way,
    /* [in] */ const char* ssid,
    /* [in] */ int security,
    /* [in] */ const char* password)
{
printf("==[%s]--==[%s]--==[%d] Try to connect, please wait...\n", __FILE__, __FUNCTION__, __LINE__);
    if (way == CONNECTWAY_ETH) {
        Boolean connected = Ethernet::RunDhcpEthe();
        if (connected) {
            printf("==[%s]--==[%s]--==[%d] Ethernet Connected.\n", __FILE__, __FUNCTION__, __LINE__);
        }
        else {
            printf("==[%s]--==[%s]--==[%d] Ethernet Net Connected.\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
    else if (way == CONNECTWAY_WIFI) {
        //1. Scan wifi hot node.
        std::vector<ScanResult*>* scanList = ElWifi_ScanResults();
        assert(scanList != NULL);

        //2. print the node's information.
        std::vector<ScanResult*>::iterator node = scanList->begin();
        for (; node != scanList->end(); ++node) {
            ScanResult* res = *node;
            assert(res != NULL);
            printf("==[%s]--==[%s]--==[%d], ssid = [%s] \n", __FILE__, __FUNCTION__, __LINE__, res->ssid);
        }

        //3. connect?
        int flag = ElWifi_AddNetwork(ssid, security, password);
        printf("==[%s]--==[%s]--==[%d], WIFI flag = [%d] \n", __FILE__, __FUNCTION__, __LINE__, flag);
    }
    else {
        assert(0);
    }
}

ECode CWifiSetting::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    //Ethernet
    mEthernetButton = FindViewById(R::id::ethernetbutton);
    assert(mEthernetButton != NULL);
    AutoPtr<MyListener> l = new MyListener(this);

    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));
    mEthernetButton->SetOnClickListener(clickListener);
    mEthernetText = ITextView::Probe(FindViewById(R::id::ethernetstatetextview));
    assert(mEthernetText != NULL);

    //Wifi
    mWifiButton = FindViewById(R::id::wifibutton);
    assert(mWifiButton != NULL);
    mWifiButton->SetOnClickListener(clickListener);
    mWifiText = ITextView::Probe(FindViewById(R::id::wifistatetextview));
    assert(mWifiText != NULL);

    return NOERROR;
}

ECode CWifiSetting::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CWifiSetting::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CWifiSetting::OnPause()
{
    return NOERROR;
}

ECode CWifiSetting::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CWifiSetting::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CWifiSetting::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace WifiSetting
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
