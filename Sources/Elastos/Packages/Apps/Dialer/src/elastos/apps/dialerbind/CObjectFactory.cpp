
#include "CObjectFactory.h"

namespace Elastos {
namespace Apps {
namespace DialerBind {

CAR_INTERFACE_IMPL(CObjectFactory, Singleton, IObjectFactory);

CAR_SINGLETON_IMPL(CObjectFactory);

ECode CObjectFactory::NewCachedNumberLookupService(
        /* [out] */ ICachedNumberLookupService** service)
{
    VALIDATE_NOT_NULL(service);
    AutoPtr<ICachedNumberLookupService> temp = NewCachedNumberLookupService();
    *service = temp;
    if (*service) {
        REFCOUNT_ADD(*service);
    }
    return NOERROR;
}

ECode CObjectFactory::NewCallLogAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
    /* [in] */ IContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog,
    /* [out] */ ICallLogAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    AutoPtr<ICallLogAdapter> temp = NewCallLogAdapter(context, callFetcher,
            contactInfoHelper, callItemExpandedListener,
            onReportButtonClickListener, isCallLog);
    *adapter = temp;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

ECode CObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name,
    /* [out] */ IDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialogFragment> temp = GetReportDialogFragment(name);
    *fragment = temp;
    if (*fragment) {
        REFCOUNT_ADD(*fragment);
    }
    return NOERROR;
}

AutoPtr<ICachedNumberLookupService> CObjectFactory::NewCachedNumberLookupService()
{
    // no-op
    return NULL;
}

AutoPtr<ICallLogAdapter> CObjectFactory::NewCallLogAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
    /* [in] */ IContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog)
{
    AutoPtr<ICallLogAdapter> adapter;
    CCallLogAdapter::New(context, callFetcher, contactInfoHelper,
            callItemExpandedListener, onReportButtonClickListener,
            isCallLog, (ICallLogAdapter**)&adapter);
    return adapter;
}

AutoPtr<IDialogFragment> CObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name)
{
    return NULL;
}

} // DialerBind
} // Apps
} // Elastos

