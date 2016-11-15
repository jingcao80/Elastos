
#ifndef __CWIFISETTING_H__
#define __CWIFISETTING_H__

#include "elastos/droid/app/Activity.h"
#include "_CWifiSetting.h"
// #include "ConnectTimerTask.h"
#include "WifiFun.h"
#include "Ethernet.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;


class MyThread;

enum CONNECTWAY {
    CONNECTWAY_NONE,
    CONNECTWAY_WIFI,
    CONNECTWAY_ETH
};

enum WIFISTATUS {
    WIFI_STARTING,
    WIFI_STARTING_SUCCESS,
    WIFI_STARTING_FAILED,
    WIFI_LINKED_SUCCESS,
    WIFI_LINKED_FAILED,
    WIFI_STOPING,
    WIFI_NONE
};

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiSetting {

class CWifiSetting : public Activity
{
public:
    class MyListener
    : public IViewOnTouchListener
    , public IViewOnKeyListener
    , public IAdapterViewOnItemClickListener
    , public IViewOnClickListener
    , public IDialogInterfaceOnClickListener
    , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        MyListener(
            /* [in] */ CWifiSetting* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnKey(
            /* [in] */ IView * v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CWifiSetting* mHost;
        Int32 mRef;
    };

public:
    CWifiSetting();

    ~CWifiSetting();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI_(Boolean) CheckState(
        /* [in] */ CONNECTWAY way = CONNECTWAY_NONE);

    CARAPI_(void) Connect(
        /* [in] */ CONNECTWAY way,
        /* [in] */ const char* ssid = NULL,
        /* [in] */ int security = 0,
        /* [in] */ const char* password = NULL);

private:
    AutoPtr<IView> mEthernetButton;
    AutoPtr<IView> mWifiButton;
    AutoPtr<IView> mCancelButton;

    //TextView
    AutoPtr<ITextView> mEthernetText;
    AutoPtr<ITextView> mWifiText;

    AutoPtr<ILayoutInflater> mInflater;

    std::vector<ScanResult*>*  mScanList;

    Boolean  mIsInited;
    Boolean  mIsConnecting;
    ScanResult*  mFocusScanRes;

    AutoPtr<IThread> mConnectThread;

friend class MyListener;
// friend class ConnectTimerTask;
friend class MyThread;
};

} // namespace WifiSetting
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CWIFISETTING_H__
