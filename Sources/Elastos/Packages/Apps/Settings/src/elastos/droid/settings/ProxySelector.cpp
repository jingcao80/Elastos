#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/ProxySelector.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Net::CProxy;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
// using Elastos::Droid::App::Admin::IDevicePolicyManager;
// using Elastos::Droid::Content::IContentResolver;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Net::IConnectivityManager;
// using Elastos::Droid::Net::IProxyInfo;
// using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::Provider::ISettings;
// using Elastos::Droid::Text::ISelection;
// using Elastos::Droid::Text::ISpannable;
// using Elastos::Droid::Text::TextUtils;
// using Elastos::Droid::View::ILayoutInflater;
// using Elastos::Droid::View::IView;
// using Elastos::Droid::View::View::IOnClickListener;
// using Elastos::Droid::View::View::IOnFocusChangeListener;
// using Elastos::Droid::View::IViewGroup;
// using Elastos::Droid::Widget::IButton;
// using Elastos::Net::IInetSocketAddress;

namespace Elastos {
namespace Droid {
namespace Settings {

const String ProxySelector::TAG("ProxySelector");

const Int32 ProxySelector::ERROR_DIALOG_ID = 0;

CAR_INTERFACE_IMPL(ProxySelector, Fragment, IDialogCreatable)

ProxySelector::ProxySelector()
{}

ProxySelector::~ProxySelector()
{}

ECode ProxySelector::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    if (id == ERROR_DIALOG_ID) {
        AutoPtr<ICharSequence> seq;
        ITextView::Probe(mHostnameField)->GetText((ICharSequence**)&seq);
        String hostname = TO_STR(seq).Trim();
        seq = NULL;
        ITextView::Probe(mPortField)->GetText((ICharSequence**)&seq);
        String portStr = TO_STR(seq).Trim();
        seq = NULL;
        ITextView::Probe(mExclusionListField)->GetText((ICharSequence**)&seq);
        String exclList = TO_STR(seq).Trim();
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        String msg;
        IContext::Probe(activity)->GetString(Validate(hostname, portStr, exclList), &msg);

        AutoPtr<IAlertDialogBuilder> adb;
        CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&adb);
        adb->SetTitle(R::string::proxy_error);
        adb->SetPositiveButton(R::string::proxy_error_dismiss, NULL);
        adb->SetMessage(CoreUtils::Convert(msg));
        AutoPtr<IAlertDialog> ad;
        adb->Create((IAlertDialog**)&ad);
        *dialog = IDialog::Probe(ad);
        REFCOUNT_ADD(*dialog)
        return NOERROR;
    }
    return NOERROR;
}

Int32 ProxySelector::Validate(
    /* [in] */ const String& hostname,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList)
{
    using Elastos::Droid::Net::IProxy;
    AutoPtr<IProxy> proxy;
    CProxy::AcquireSingleton((IProxy**)&proxy);
    Int32 result;
    proxy->Validate(hostname, port, exclList, &result);
    switch (result) {
        case IProxy::PROXY_VALID:
            return 0;
        case IProxy::PROXY_HOSTNAME_EMPTY:
            return R::string::proxy_error_empty_host_set_port;
        case IProxy::PROXY_HOSTNAME_INVALID:
            return R::string::proxy_error_invalid_host;
        case IProxy::PROXY_PORT_EMPTY:
            return R::string::proxy_error_empty_port;
        case IProxy::PROXY_PORT_INVALID:
            return R::string::proxy_error_invalid_port;
        case IProxy::PROXY_EXCLLIST_INVALID:
            return R::string::proxy_error_invalid_exclusion_list;
        default:
            // should neven happen
            Logger::E(TAG, "Unknown proxy settings error");
            return -1;
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
