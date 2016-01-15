
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::ElRefBase;
using Elastos::Net::IInetAddress;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::IWifiDisplay;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiDisplayDemo {


class CActivityOne : public Activity
{
public:
    class MyListener
    : public IAdapterViewOnItemClickListener
    , public IViewOnClickListener
    , public IRadioGroupOnCheckedChangeListener
    , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnCheckedChanged(
            /* [in] */  IRadioGroup* group,
            /* [in] */  Int32 checkedId);

    private:
        CActivityOne* mHost;
    };

    class DisplayListener
        : public ElRefBase
        , public IDisplayListener
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);
    private:
        CActivityOne* mHost;
    };


 protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

public:
    CActivityOne();

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI ShowToast(
        /* [in] */ const String& info,
        /* [in] */ Int32 style = IToast::LENGTH_SHORT);

    CARAPI SetupViews();

    CARAPI UpdateListView();

    CARAPI AppendInfo(
        /* [in] */ const String& info);

    CARAPI_(String) GetAddress();

    CARAPI ResetData();

public:
    CARAPI OnScanButtonClick();
    CARAPI OnRenameButtonClick();
    CARAPI OnConnectButtonClick();
    CARAPI OnDisconnectButtonClick();
    CARAPI OnForgetButtonClick();
    CARAPI OnGetStatusButtonClick();

    CARAPI ConnectToWifiDisplay(
        /* [in] */ const String& address);

private:
    static String TAG;// = "wifidirectdemo";

    AutoPtr<ITextView> mInfoTextView;
    AutoPtr<IListView> mDisplayListView;

    AutoPtr<ArrayOf<IWifiDisplay*> > mWifiDisplays;
    AutoPtr<IDisplayManager> mDisplayManager;
};

} // namespace WifiDisplayDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
