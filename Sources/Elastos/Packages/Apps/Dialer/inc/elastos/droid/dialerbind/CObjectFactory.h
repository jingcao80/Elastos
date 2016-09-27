#ifndef __ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__
#define __ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__

#include "_Elastos_Droid_DialerBind_CObjectFactory.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapter;
using Elastos::Droid::Dialer::CallLog::ICallFetcher;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterCallItemExpandedListener;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterOnReportButtonClickListener;
using Elastos::Droid::Dialer::CallLog::IContactInfoHelper;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
namespace DialerBind {

CarClass(CObjectFactory)
    , public Singleton
    , public IObjectFactory
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI NewCachedNumberLookupService(
        /* [out] */ ICachedNumberLookupService** service);

    /**
     * Create a new instance of the call log adapter.
     * @param context The context to use.
     * @param callFetcher Instance of call fetcher to use.
     * @param contactInfoHelper Instance of contact info helper class to use.
     * @param isCallLog Is this call log adapter being used on the call log?
     * @return Instance of CallLogAdapter.
     */
    CARAPI NewCallLogAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ICallFetcher* callFetcher,
        /* [in] */ IContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog,
        /* [out] */ ICallLogAdapter** adapter);

    CARAPI GetReportDialogFragment(
        /* [in] */ const String& name,
        /* [out] */ IDialogFragment** fragment);

    static CARAPI_(AutoPtr<ICachedNumberLookupService>) NewCachedNumberLookupService();

    static CARAPI_(AutoPtr<ICallLogAdapter>) NewCallLogAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ICallFetcher* callFetcher,
        /* [in] */ IContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog);

    static CARAPI_(AutoPtr<IDialogFragment>) GetReportDialogFragment(
        /* [in] */ const String& name);
};

} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__
