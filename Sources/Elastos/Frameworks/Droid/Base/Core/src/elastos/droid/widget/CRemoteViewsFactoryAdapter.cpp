
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/widget/CRemoteViewsFactoryAdapter.h"
#include "elastos/droid/widget/RemoteViewsService.h"
#include "elastos/droid/content/CIntentFilterComparison.h"

using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::CIntentFilterComparison;

namespace Elastos {
namespace Droid {
namespace Widget {

CRemoteViewsFactoryAdapter::CRemoteViewsFactoryAdapter()
    : mIsCreated(FALSE)
{}

ECode CRemoteViewsFactoryAdapter::constructor(
    /* [in] */ IRemoteViewsFactory* factory,
    /* [in] */ Boolean isCreated)
{
    mFactory = factory;
    mIsCreated = isCreated;
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::IsCreated(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoLock lock(this);
    *res = mIsCreated;
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::OnDataSetChanged()
{

    AutoLock lock(this);
    //try{
    mFactory->OnDataSetChanged();
    // } catch (Exception ex) {
    //     Thread t = Thread.currentThread();
    //     Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    // }
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::OnDataSetChangedAsync()
{
    AutoLock lock(this);
    return OnDataSetChanged();
}

ECode CRemoteViewsFactoryAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoLock lock(this);
    //try {
    mFactory->GetCount(count);

    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::GetViewAt(
    /* [in] */ Int32 position,
    /* [out] */ IParcelable** views)
{
    VALIDATE_NOT_NULL(views);
    AutoLock lock(this);
    //try {
        mFactory->GetViewAt(position, views);
        if (IRemoteViews::Probe(*views)) {
            IRemoteViews::Probe(*views)->SetIsWidgetCollectionChild(TRUE);
        }
        return NOERROR;
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
}

ECode CRemoteViewsFactoryAdapter::GetLoadingView(
    /* [out] */ IParcelable** views)
{
    VALIDATE_NOT_NULL(views);
    AutoLock lock(this);
    //try {
    mFactory->GetLoadingView(views);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoLock lock(this);
    //try {
    *count = 0;
    mFactory->GetViewTypeCount(count);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    AutoLock lock(this);
    *id = 0;
    //try {
    mFactory->GetItemId(position, id);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::HasStableIds(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoLock lock(this);
    *res = FALSE;
    //try {
    mFactory->HasStableIds(res);
    //} catch (Exception ex) {
    //    Thread t = Thread.currentThread();
    //    Thread.getDefaultUncaughtExceptionHandler().uncaughtException(t, ex);
    //}
    return NOERROR;
}

ECode CRemoteViewsFactoryAdapter::OnDestroy(
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

ECode CRemoteViewsFactoryAdapter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("RemoteViewsFactoryAdapter");
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
