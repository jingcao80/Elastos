#ifndef __ELASTOS_APPS_DIALERBIND_COBJECTFACTORY_H__
#define __ELASTOS_APPS_DIALERBIND_COBJECTFACTORY_H__

#include "_Elastos_Apps_DialerBind_CObjectFactory.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos{
namespace Apps{
namespace DialerBind {

CarClass(CObjectFactory)
    , public singleton
    , public IObjectFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

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
        /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
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
        /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
        /* [in] */ IContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog);

    static CARAPI_(AutoPtr<IDialogFragment>) GetReportDialogFragment(
        /* [in] */ const String& name);
};

} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_COBJECTFACTORY_H__
