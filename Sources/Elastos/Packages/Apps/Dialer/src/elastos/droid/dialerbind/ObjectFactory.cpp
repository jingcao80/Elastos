
#include "elastos/droid/dialerbind/ObjectFactory.h"
// #include "elastos/droid/dialer/calllog/CallLogAdapter.h"

// using Elastos::Droid::Dialer::Calllog::CallLogAdapter;

namespace Elastos {
namespace Droid {
namespace DialerBind {

AutoPtr<ICachedNumberLookupService> ObjectFactory::NewCachedNumberLookupService()
{
    // no-op
    return NULL;
}

// AutoPtr<CallLogAdapter> ObjectFactory::NewCallLogAdapter(
//     /* [in] */ IContext* context,
//     /* [in] */ ICallFetcher* callFetcher,
//     /* [in] */ IContactInfoHelper* contactInfoHelper,
//     /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
//     /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
//     /* [in] */ Boolean isCallLog)
// {
//     AutoPtr<ICallLogAdapter> adapter;
//     CCallLogAdapter::New(context, callFetcher, contactInfoHelper,
//             callItemExpandedListener, onReportButtonClickListener,
//             isCallLog, (ICallLogAdapter**)&adapter);
//     return adapter;
// }

AutoPtr<IDialogFragment> ObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name)
{
    return NULL;
}

} // DialerBind
} // Droid
} // Elastos

