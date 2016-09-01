
#ifndef __ELASTOS_DROID_DIALERBIND_OBJECTFACTORY_H__
#define __ELASTOS_DROID_DIALERBIND_OBJECTFACTORY_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapter;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterCallFetcher;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterCallItemExpandedListener;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterOnReportButtonClickListener;
// using Elastos::Droid::Dialer::CallLog::ContactInfoHelper;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
namespace DialerBind {

/**
 * Default static binding for various objects.
 */
class ObjectFactory
{
public:
    static CARAPI_(AutoPtr<ICachedNumberLookupService>) NewCachedNumberLookupService();

    // static CARAPI_(AutoPtr<CallLogAdapter>) NewCallLogAdapter(
    //     /* [in] */ IContext* context,
    //     /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
    //     /* [in] */ ContactInfoHelper* contactInfoHelper,
    //     /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    //     /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    //     /* [in] */ Boolean isCallLog);

    static CARAPI_(AutoPtr<IDialogFragment>) GetReportDialogFragment(
        /* [in] */ const String& name);
};

} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_OBJECTFACTORY_H__
