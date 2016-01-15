
#include "elastos/droid/widget/RemoteViewsService.h"
#include "elastos/droid/content/CIntentFilterComparison.h"
#include "elastos/droid/widget/CRemoteViewsFactoryAdapter.h"
#include "elastos/droid/widget/RemoteViewsService.h"

#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Widget::EIID_IIRemoteViewsFactory;

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL_2(RemoteViewsService::RemoteViewsFactoryAdapter,Object, IIRemoteViewsFactory, IBinder)

RemoteViewsService::RemoteViewsFactoryAdapter::RemoteViewsFactoryAdapter()
    : mIsCreated(FALSE)
{}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::constructor(
    /* [in] */ IRemoteViewsFactory* factory,
    /* [in] */ Boolean isCreated)
{
    mFactory = factory;
    mIsCreated = isCreated;
    return NOERROR;
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::IsCreated(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoLock lock(this);
    *res = mIsCreated;
    return NOERROR;
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::OnDataSetChanged()
{

    AutoLock lock(this);
    //try{
    return mFactory->OnDataSetChanged();
    // } catch (Exception ex) {
    //     Thread t = Thread.currentThread();
    //     Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    // }
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::OnDataSetChangedAsync()
{
    AutoLock lock(this);
    return OnDataSetChanged();
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoLock lock(this);
    //try {
    return mFactory->GetCount(count);

    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::GetViewAt(
    /* [in] */ Int32 position,
    /* [out] */ IParcelable** views)
{
    VALIDATE_NOT_NULL(views);
    AutoLock lock(this);
    //try {
        mFactory->GetViewAt(position, views);
        if (IRemoteViews::Probe(*views)) {
            return IRemoteViews::Probe(*views)->SetIsWidgetCollectionChild(TRUE);
        }
        return NOERROR;
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::GetLoadingView(
    /* [out] */ IParcelable** views)
{
    VALIDATE_NOT_NULL(views);
    AutoLock lock(this);
    //try {
    return mFactory->GetLoadingView(views);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoLock lock(this);
    //try {
    *count = 0;
    return mFactory->GetViewTypeCount(count);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    AutoLock lock(this);
    *id = 0;
    //try {
    return mFactory->GetItemId(position, id);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::HasStableIds(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoLock lock(this);
    *res = FALSE;
    //try {
    return mFactory->HasStableIds(res);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::OnDestroy(
    /* [in] */ IIntent* intent)
{
    AutoLock lock(RemoteViewsService::sLock);
    AutoPtr<IIntentFilterComparison> fc;
    CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&fc);
    if (RemoteViewsService::sRemoteViewFactories.Find(fc) != RemoteViewsService::sRemoteViewFactories.End()) {
        AutoPtr<IRemoteViewsFactory> factory = RemoteViewsService::sRemoteViewFactories[fc];
        //try {
            factory->OnDestroy();
        //} catch (Exception ex) {
        //    Thread t = Thread.currentThread();
        //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
        //}
        RemoteViewsService::sRemoteViewFactories.Erase(fc);
    }
    return NOERROR;
}

ECode RemoteViewsService::RemoteViewsFactoryAdapter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("RemoteViewsFactoryAdapter");
    return NOERROR;
}


const String RemoteViewsService::TAG("RemoteViewsService");
Object RemoteViewsService::sLock;
HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IRemoteViewsFactory> > RemoteViewsService::sRemoteViewFactories;

CAR_INTERFACE_IMPL(RemoteViewsService, Service, IRemoteViewsService)

ECode RemoteViewsService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    AutoLock lock(sLock);

    AutoPtr<IIntentFilterComparison> fc;
    CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&fc);
    AutoPtr<IRemoteViewsFactory> factory;
    Boolean isCreated = FALSE;
    if (sRemoteViewFactories.Find(fc) == sRemoteViewFactories.End()) {
        FAIL_RETURN(OnGetViewFactory(intent, (IRemoteViewsFactory**)&factory));
        sRemoteViewFactories[fc] = factory;
        factory->OnCreate();
        isCreated = FALSE;
    }
    else {
        factory = sRemoteViewFactories[fc];
        isCreated = TRUE;
    }

    return CRemoteViewsFactoryAdapter::New(factory, isCreated, binder);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
