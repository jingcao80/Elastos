#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WRITEWIFICONFIGTONFCDIALOG_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WRITEWIFICONFIGTONFCDIALOG_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Nfc::ITag;
using Elastos::Droid::Nfc::INfcAdapterReaderCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Wifi::IWifiManager;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class WriteWifiConfigToNfcDialog
    : public AlertDialog
    , public ITextWatcher
    , public INoCopySpan
    , public IViewOnClickListener
    , public ICompoundButtonOnCheckedChangeListener
{
public:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ WriteWifiConfigToNfcDialog* host,
            /* [in] */ Int32 id);

        ~MyRunnable();

        //@Override
        CARAPI Run();

    private:
        WriteWifiConfigToNfcDialog* mHost;
        Int32 mId;
    };

    class NfcAdapterReaderCallback
        : public Object
        , public INfcAdapterReaderCallback
    {
    public:
        CAR_INTERFACE_DECL();

        NfcAdapterReaderCallback(
            /* [in] */ WriteWifiConfigToNfcDialog* host);

        ~NfcAdapterReaderCallback();

        //@Override
        CARAPI OnTagDiscovered(
            /* [in] */ ITag* tag);

    private:
        WriteWifiConfigToNfcDialog* mHost;
    };

    class SetViewTextRunnable
        : public Runnable
    {
    public:
        SetViewTextRunnable(
            /* [in] */ ITextView* textView,
            /* [in] */ Int32 resid);

        ~SetViewTextRunnable();

        //@Override
        CARAPI Run();

    private:
        AutoPtr<ITextView> mTextView;
        Int32 mResid;
    };

public:
    CAR_INTERFACE_DECL();

    WriteWifiConfigToNfcDialog();

    ~WriteWifiConfigToNfcDialog();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ AccessPoint* accessPoint,
        /* [in] */ IWifiManager* wifiManager);

    // //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI Dismiss();

    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    //@Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    //@Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

private:
    CARAPI_(void) HandleWriteNfcEvent(
        /* [in] */ ITag* tag);

    CARAPI_(void) EnableSubmitIfAppropriate();

    CARAPI_(void) SetViewText(
        /* [in] */ ITextView* view,
        /* [in] */ Int32 resid);

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) HexStringToByteArray(
        /* [in] */ const String& s);

    static CARAPI_(String) ByteArrayToHexString(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    static const String NFC_TOKEN_MIME_TYPE;

    static const String TAG;
    static const String PASSWORD_FORMAT;
    static const Int32 HEX_RADIX;
    static const AutoPtr< ArrayOf<Char32> > hexArray;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<AccessPoint> mAccessPoint;
    AutoPtr<IView> mView;
    AutoPtr<IButton> mSubmitButton;
    AutoPtr<IButton> mCancelButton;
    AutoPtr<IHandler> mOnTextChangedHandler;
    AutoPtr<ITextView> mPasswordView;
    AutoPtr<ITextView> mLabelView;
    AutoPtr<ICheckBox> mPasswordCheckBox;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IWifiManager> mWifiManager;
    String mWpsNfcConfigurationToken;
    AutoPtr<IContext> mContext;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WRITEWIFICONFIGTONFCDIALOG_H__
