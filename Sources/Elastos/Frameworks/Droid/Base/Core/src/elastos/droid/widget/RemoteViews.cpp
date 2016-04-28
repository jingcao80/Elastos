
#include "elastos/droid/widget/RemoteViews.h"

#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/app/CActivityOptionsHelper.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/widget/CRemoteViews.h"
#include "elastos/droid/widget/CRemoteViewsListAdapter.h"
#include "elastos/droid/R.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Animation::PropertyValuesHolder;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::PorterDuffMode_NONE;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ALPHA_8;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_4444;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::ILayoutInflaterFilter;
using Elastos::Droid::View::EIID_ILayoutInflaterFilter;
using Elastos::Droid::View::EIID_IViewOnClickListener;

using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IChar32;
using Elastos::Core::CChar32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//                  RemoteViewsOnClickHandler
//==============================================================================
CAR_INTERFACE_IMPL(RemoteViews::RemoteViewsOnClickHandler, Object, IRemoteViewsOnClickHandler)

ECode RemoteViews::RemoteViewsOnClickHandler::OnClickHandler(
    /* [in] */ IView* view,
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* fillInIntent,
    /* [out] */ Boolean* succeed)
{
    VALIDATE_NOT_NULL(succeed);

    assert(pendingIntent != NULL);

    // try {
        // TODO: Unregister this handler if PendingIntent.FLAG_ONE_SHOT?
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    Int32 w, h;
    view->GetMeasuredWidth(&w);
    view->GetMeasuredHeight(&h);
    AutoPtr<IActivityOptionsHelper> helper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
    AutoPtr<IActivityOptions> opts;
    helper->MakeScaleUpAnimation(view, 0, 0, w, h, (IActivityOptions**)&opts);
    AutoPtr<IBundle> bundle;
    opts->ToBundle((IBundle**)&bundle);
    AutoPtr<IIntentSender> sender;
    pendingIntent->GetIntentSender((IIntentSender**)&sender);
    ECode ec = context->StartIntentSender(sender, fillInIntent,
        IIntent::FLAG_ACTIVITY_NEW_TASK,
        IIntent::FLAG_ACTIVITY_NEW_TASK, 0, bundle);
    if (FAILED(ec)) {
        *succeed = FALSE;
        return NOERROR;
    }
    // } catch (IntentSender.SendIntentException e) {
    //     android.util.Log.e(LOG_TAG, "Cannot send pending intent: ", e);
    //     return false;
    // } catch (Exception e) {
    //     android.util.Log.e(LOG_TAG, "Cannot send pending intent due to " +
    //             "unknown exception: ", e);
    //     return false;
    // }
    *succeed = TRUE;
    return NOERROR;
}

//==============================================================================
//                  MemoryUsageCounter
//==============================================================================
CAR_INTERFACE_IMPL(RemoteViews::MemoryUsageCounter, Object, IMemoryUsageCounter)

RemoteViews::MemoryUsageCounter::MemoryUsageCounter()
    : mMemoryUsage(0)
{
}

ECode RemoteViews::MemoryUsageCounter::Clear()
{
    mMemoryUsage = 0;
    return NOERROR;
}

ECode RemoteViews::MemoryUsageCounter::Increment(
    /* [in */ Int32 numBytes)
{
    mMemoryUsage += numBytes;
    return NOERROR;
}

ECode RemoteViews::MemoryUsageCounter::GetMemoryUsage(
    /* [out] */ Int32* usage)
{
    VALIDATE_NOT_NULL(usage)

    *usage = mMemoryUsage;
    return NOERROR;
}

ECode RemoteViews::MemoryUsageCounter::AddBitmapMemory(
    /* [in */ IBitmap* b)
{
    BitmapConfig c;
    b->GetConfig(&c);
    // If we don't know, be pessimistic and assume 4
    int bpp = 4;
    switch (c) {
    case BitmapConfig_ALPHA_8:
        bpp = 1;
        break;
    case BitmapConfig_RGB_565:
    case BitmapConfig_ARGB_4444:
        bpp = 2;
        break;
    case BitmapConfig_ARGB_8888:
        bpp = 4;
        break;
    }

    Int32 w, h;
    b->GetWidth(&w);
    b->GetHeight(&h);
    Increment(w * h * bpp);
    return NOERROR;
}

CAR_INTERFACE_IMPL(RemoteViews::MutablePair, Object, IMutablePair)

RemoteViews::MutablePair::MutablePair(
    /* [in] */ const String& first,
    /* [in] */ const String& second)
    : mFirst(first)
    , mSecond(second)
{}

ECode RemoteViews::MutablePair::Equals(
    /* [in] */ IInterface* oth,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)

    AutoPtr<IMutablePair> pair = IMutablePair::Probe(oth);
    if (pair == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    MutablePair* pairImpl = (MutablePair*)pair.Get();
    *equals =mSecond.Equals(pairImpl->mSecond) && mFirst.Equals(pairImpl->mFirst);
    return NOERROR;
}

ECode RemoteViews::MutablePair::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = mFirst.GetHashCode() ^ mSecond.GetHashCode();
    return NOERROR;
}

ECode RemoteViews::MutablePair::GetFirst(
    /* [out] */ String* fir)
{
    VALIDATE_NOT_NULL(fir)

    *fir = mFirst;
    return NOERROR;
}

ECode RemoteViews::MutablePair::SetFirst(
    /* [in] */ const String& fir)
{
    mFirst = fir;
    return NOERROR;
}

ECode RemoteViews::MutablePair::GetSecond(
    /* [out] */ String* sec)
{
    VALIDATE_NOT_NULL(sec)

    *sec = mSecond;
    return NOERROR;
}

ECode RemoteViews::MutablePair::SetSecond(
    /* [in] */ const String& sec)
{
    mSecond = sec;
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::Action
//==============================================================================
const Int32 RemoteViews::Action::MERGE_REPLACE;
const Int32 RemoteViews::Action::MERGE_APPEND;
const Int32 RemoteViews::Action::MERGE_IGNORE;

CAR_INTERFACE_IMPL(RemoteViews::Action, Object, IParcelable)

RemoteViews::Action::Action()
    : mViewId(0)
{}

RemoteViews::Action::Action(
    /* [in] */ Int32 viewId)
    : mViewId(viewId)
{}

ECode RemoteViews::Action::UpdateMemoryUsageEstimate(
    /* [in] */ IMemoryUsageCounter* counter)
{
    // We currently only calculate Bitmap memory usage, so by default, don't do anything
    // here
    return NOERROR;
}

ECode RemoteViews::Action::SetBitmapCache(
    /* [in] */ IBitmapCache* bitmapCache)
{
    // Do nothing
    return NOERROR;
}

ECode RemoteViews::Action::SetViewId(
    /* [in] */ Int32 viewId)
{
    mViewId = viewId;
    return NOERROR;
}

ECode RemoteViews::Action::GetViewId(
    /* [out] */ Int32* viewId)
{
    VALIDATE_NOT_NULL(viewId)

    *viewId = mViewId;
    return NOERROR;
}

ECode RemoteViews::Action::MergeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = MERGE_REPLACE;
    return NOERROR;
}

ECode RemoteViews::Action::GetUniqueKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)

    String subName;
    GetActionName(&subName);
    StringBuilder sb(subName);
    sb += "[viewId:0x";
    sb += StringUtils::ToString(mViewId, 16);
    sb += "]";
    *key = sb.ToString();
    return NOERROR;
}


//==============================================================================
//                  RemoteViews::_SetEmptyView
//==============================================================================
const Int32 RemoteViews::_SetEmptyView::TAG;

RemoteViews::_SetEmptyView::_SetEmptyView()
    : mEmptyViewId(0)
{}

RemoteViews::_SetEmptyView::_SetEmptyView(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 emptyViewId)
    : Action(viewId)
    , mEmptyViewId(emptyViewId)
{}

ECode RemoteViews::_SetEmptyView::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> view;
    root->FindViewById(mViewId, (IView**)&view);
    AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(view);
    if (adapterView == NULL) {
        return NOERROR;
    }

    AutoPtr<IView> emptyView;
    root->FindViewById(mEmptyViewId, (IView**)&emptyView);
    if (emptyView == NULL) {
        return NOERROR;
    }

    return adapterView->SetEmptyView(emptyView);
}

ECode RemoteViews::_SetEmptyView::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetEmptyView";
    return NOERROR;
}

ECode RemoteViews::_SetEmptyView::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    source->ReadInt32(&mEmptyViewId);
    return NOERROR;
}

ECode RemoteViews::_SetEmptyView::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mEmptyViewId);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::_SetOnClickFillInIntent
//==============================================================================
const Int32 RemoteViews::_SetOnClickFillInIntent::TAG;

RemoteViews::_SetOnClickFillInIntent::_SetOnClickFillInIntent(
    /* [in] */ RemoteViews* host)
    : mHost(host)
{}

RemoteViews::_SetOnClickFillInIntent::_SetOnClickFillInIntent(
    /* [in] */ Int32 id,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ RemoteViews* host)
    : Action(id)
    , mFillInIntent(fillInIntent)
    , mHost(host)
{}

ECode RemoteViews::_SetOnClickFillInIntent::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    if (!mHost->mIsWidgetCollectionChild) {
        return NOERROR;
    }
    if (root == target) {
        target->SetTagInternal(R::id::fillInIntent, mFillInIntent);
    }
    else if (mFillInIntent) {
        AutoPtr<FillInIntentClickListener> listener = new FillInIntentClickListener(mFillInIntent, handler);
        target->SetOnClickListener(listener);
    }
    return NOERROR;
}

ECode RemoteViews::_SetOnClickFillInIntent::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetOnClickFillInIntent";
    return NOERROR;
}

ECode RemoteViews::_SetOnClickFillInIntent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    mFillInIntent = NULL;
    source->ReadInterfacePtr((Handle32*)&mFillInIntent);
    return NOERROR;
}

ECode RemoteViews::_SetOnClickFillInIntent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInterfacePtr(mFillInIntent);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::_SetPendingIntentTemplate
//==============================================================================
const Int32 RemoteViews::_SetPendingIntentTemplate::TAG;

RemoteViews::_SetPendingIntentTemplate::_SetPendingIntentTemplate(
    /* [in] */ RemoteViews* host)
    : mHost(host)
{}

RemoteViews::_SetPendingIntentTemplate::_SetPendingIntentTemplate(
    /* [in] */ Int32 id,
    /* [in] */ IPendingIntent* pendingIntentTemplate,
    /* [in] */ RemoteViews* host)
    : Action(id)
    , mPendingIntentTemplate(pendingIntentTemplate)
    , mHost(host)
{}

ECode RemoteViews::_SetPendingIntentTemplate::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    AutoPtr<IAdapterView> av = IAdapterView::Probe(target);
    if (av) {
        AutoPtr<IntentTemplateOnItemClickListener> listener = new IntentTemplateOnItemClickListener(mPendingIntentTemplate, handler);
        av->SetOnItemClickListener(listener);
        IView::Probe(av)->SetTag(mPendingIntentTemplate);
    }
    else {
        SLOGGERE("RemoteViews", String("Cannot setPendingIntentTemplate on a view which is not") +
                        String("an AdapterView (id: ") + StringUtils::ToString(mViewId) + ")")
    }
    return NOERROR;
}

ECode RemoteViews::_SetPendingIntentTemplate::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetPendingIntentTemplate";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::_SetPendingIntentTemplate::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    mPendingIntentTemplate = NULL;
    source->ReadInterfacePtr((Handle32*)&mPendingIntentTemplate);
    return NOERROR;
}

ECode RemoteViews::_SetPendingIntentTemplate::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInterfacePtr(mPendingIntentTemplate);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::SetRemoteViewsAdapterList
//==============================================================================
const Int32 RemoteViews::SetRemoteViewsAdapterList::TAG;

RemoteViews::SetRemoteViewsAdapterList::SetRemoteViewsAdapterList()
    : Action(0)
    ,mViewTypeCount(0)
{}

RemoteViews::SetRemoteViewsAdapterList::SetRemoteViewsAdapterList(
    /* [in] */ Int32 id,
    /* [in] */ IArrayList* list,
    /* [in] */ Int32 viewTypeCount)
    : Action(id)
    , mViewTypeCount(viewTypeCount)
    , mList(list)
{}

ECode RemoteViews::SetRemoteViewsAdapterList::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    // Ensure that we are applying to an AppWidget root
    if (IAppWidgetHostView::Probe(rootParent) == NULL) {
        SLOGGERE("RemoteViews", String("SetRemoteViewsAdapterIntent action can only be used for ") +
                "AppWidgets (root id: " + StringUtils::ToString(mViewId) + ")")
        return NOERROR;
    }
    // Ensure that we are calling setRemoteAdapter on an AdapterView that supports it
    if (IAbsListView::Probe(target) == NULL && IAdapterViewAnimator::Probe(target) == NULL) {
        SLOGGERE("RemoteViews", String("Cannot setRemoteViewsAdapter on a view which is not ") +
                "an AbsListView or AdapterViewAnimator (id: " + StringUtils::ToString(mViewId) + ")");
        return NOERROR;
    }

    if (IAbsListView::Probe(target) != NULL) {
        AutoPtr<IAbsListView> v = IAbsListView::Probe(target);
        AutoPtr<IAdapter> a;
        IAdapterView::Probe(v)->GetAdapter((IAdapter**)&a);
        Int32 typeCount;
        a->GetViewTypeCount(&typeCount);
        if (IRemoteViewsListAdapter::Probe(a) && mViewTypeCount <= typeCount) {
            IRemoteViewsListAdapter::Probe(a)->SetViewsList(mList);
        } else {
            AutoPtr<IContext> ctx;
            IView::Probe(v)->GetContext((IContext**)&ctx);
            AutoPtr<IAdapter> apt;
            CRemoteViewsListAdapter::New(ctx, mList, mViewTypeCount, (IAdapter**)&apt);
            IAdapterView::Probe(v)->SetAdapter(apt);
        }
    } else if (IAdapterViewAnimator::Probe(target) != NULL) {
        AutoPtr<IAdapterViewAnimator> v = IAdapterViewAnimator::Probe(target);
        AutoPtr<IAdapter> a;
        IAdapterView::Probe(v)->GetAdapter((IAdapter**)&a);
        Int32 typeCount;
        a->GetViewTypeCount(&typeCount);
        if (IRemoteViewsListAdapter::Probe(a) && mViewTypeCount <= typeCount) {
            IRemoteViewsListAdapter::Probe(a)->SetViewsList(mList);
        } else {
            AutoPtr<IContext> ctx;
            IView::Probe(v)->GetContext((IContext**)&ctx);
            AutoPtr<IAdapter> apt;
            CRemoteViewsListAdapter::New(ctx, mList, mViewTypeCount, (IAdapter**)&apt);
            IAdapterView::Probe(v)->SetAdapter(apt);
        }
    }
    return NOERROR;
}

ECode RemoteViews::SetRemoteViewsAdapterList::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetRemoteViewsAdapterList";
    return NOERROR;
}

ECode RemoteViews::SetRemoteViewsAdapterList::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mViewId);
    source->ReadInt32(&mViewTypeCount);
    Int32 count;
    source->ReadInt32(&count);
    CArrayList::New((IArrayList**)&mList);

    for (int i = 0; i < count; i++) {
        AutoPtr<IRemoteViews> rv;
        CRemoteViews::New((IRemoteViews**)&rv);
        IParcelable::Probe(rv)->ReadFromParcel(source);
        mList->Add(rv);
    }
    return NOERROR;
}

ECode RemoteViews::SetRemoteViewsAdapterList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mViewTypeCount);

    Int32 size;
    if (mList == NULL || (mList->GetSize(&size), size) == 0) {
        dest->WriteInt32(0);
    } else {
        mList->GetSize(&size);
        dest->WriteInt32(size);
        for (int i = 0; i < size; i++) {
            AutoPtr<IInterface> tmp;
            mList->Get(i, (IInterface**)&tmp);
            AutoPtr<IParcelable> p = IParcelable::Probe(tmp);
            p->WriteToParcel(dest);
        }
    }
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::SetRemoteViewsAdapterIntent
//==============================================================================
const Int32 RemoteViews::SetRemoteViewsAdapterIntent::TAG;

RemoteViews::SetRemoteViewsAdapterIntent::SetRemoteViewsAdapterIntent()
{}

RemoteViews::SetRemoteViewsAdapterIntent::SetRemoteViewsAdapterIntent(
    /* [in] */ Int32 id,
    /* [in] */ IIntent* intent)
    : Action(id)
    , mIntent(intent)
{}

ECode RemoteViews::SetRemoteViewsAdapterIntent::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr <IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    if (IAppWidgetHostView::Probe(rootParent) == NULL) {
        SLOGGERE("RemoteViews", String("SetRemoteViewsAdapterIntent action can only be used for ") +
                "AppWidgets (root id: " + StringUtils::ToString(mViewId)  + ")");
        return NOERROR;
    }
    if (IAbsListView::Probe(target) == NULL && IAdapterViewAnimator::Probe(target) == NULL) {
        SLOGGERE("RemoteViews", String("Cannot setRemoteViewsAdapter on a view which is not ") +
                "an AbsListView or AdapterViewAnimator (id: " + StringUtils::ToString(mViewId)  + ")");
        return NOERROR;
    }
    AutoPtr<IAppWidgetHostView> host = IAppWidgetHostView::Probe(rootParent);
    Int32 hostId = 0;
    host->GetAppWidgetId(&hostId);
    mIntent->PutExtra(EXTRA_REMOTEADAPTER_APPWIDGET_ID, hostId);
    AutoPtr<IAbsListView> absListView = IAbsListView::Probe(target);
    AutoPtr<IAdapterViewAnimator> viewAnimator = IAdapterViewAnimator::Probe(target);
    if (absListView) {
        absListView->SetRemoteViewsAdapter(mIntent);
        absListView->SetRemoteViewsOnClickHandler(handler);
    } else if (viewAnimator) {
        viewAnimator->SetRemoteViewsAdapter(mIntent);
        viewAnimator->SetRemoteViewsOnClickHandler(handler);
    }
    return NOERROR;
}

ECode RemoteViews::SetRemoteViewsAdapterIntent::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetRemoteViewsAdapterIntent";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::SetRemoteViewsAdapterIntent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    mIntent = NULL;
    source->ReadInterfacePtr((Handle32*)&mIntent);
    return NOERROR;
}

ECode RemoteViews::SetRemoteViewsAdapterIntent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInterfacePtr(mIntent);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::_SetOnClickPendingIntent
//==============================================================================
const Int32 RemoteViews::_SetOnClickPendingIntent::TAG;

RemoteViews::_SetOnClickPendingIntent::_SetOnClickPendingIntent(
    /* [in] */ RemoteViews* host)
    : mHost(host)
{}

RemoteViews::_SetOnClickPendingIntent::_SetOnClickPendingIntent(
    /* [in] */ Int32 id,
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ RemoteViews* host)
    : Action(id)
    , mPendingIntent(pendingIntent)
    , mHost(host)
{}

ECode RemoteViews::_SetOnClickPendingIntent::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    if (mHost->mIsWidgetCollectionChild) {
        SLOGGERW("RemoteViews", String("Cannot setOnClickPendingIntent for collection item ") +
            "(id: " + StringUtils::ToString(mViewId) + ")");
        AutoPtr<IContext> context;
        root->GetContext((IContext**)&context);
        AutoPtr<IApplicationInfo> appInfo;
        context->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version = 0;
        if (appInfo != NULL &&
                (appInfo->GetTargetSdkVersion(&version), version >= Build::VERSION_CODES::JELLY_BEAN)) {
            return NOERROR;
        }
    }

    if (target != NULL) {
        if (mPendingIntent) {
            AutoPtr<PendingIntentClickListener> listener;
            listener = new PendingIntentClickListener(mPendingIntent, handler);
            target->SetOnClickListener(listener);
        }
    }
    return NOERROR;
}

ECode RemoteViews::_SetOnClickPendingIntent::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetOnClickPendingIntent";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::_SetOnClickPendingIntent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);

    Int32 temp = 0;
    source->ReadInt32(&temp);
    if (temp == 1) {
        mPendingIntent = NULL;
        source->ReadInterfacePtr((Handle32*)&mPendingIntent);
    }

    return NOERROR;
}

ECode RemoteViews::_SetOnClickPendingIntent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    if (mPendingIntent != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mPendingIntent);
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::_SetDrawableParameters
//==============================================================================
const Int32 RemoteViews::_SetDrawableParameters::TAG;

RemoteViews::_SetDrawableParameters::_SetDrawableParameters()
    : mTargetBackground(FALSE)
    , mAlpha(0)
    , mColorFilter(0)
    , mFilterMode(PorterDuffMode_NONE)
    , mLevel(0)
{}

RemoteViews::_SetDrawableParameters::_SetDrawableParameters(
    /* [in] */ Int32 id,
    /* [in] */ Boolean targetBackground,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 colorFilter,
    /* [in] */ PorterDuffMode mode,
    /* [in] */ Int32 level)
    : Action(id)
    , mTargetBackground(targetBackground)
    , mAlpha(alpha)
    , mColorFilter(colorFilter)
    , mFilterMode(mode)
    , mLevel(level)
{}

ECode RemoteViews::_SetDrawableParameters::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return NOERROR;
    }

    AutoPtr<IDrawable> targetDrawable;
    if (mTargetBackground) {
        target->GetBackground((IDrawable**)&targetDrawable);
    }
    else if (IImageView::Probe(target) !=  NULL) {
        AutoPtr<IImageView> imageview = IImageView::Probe(target);
        imageview->GetDrawable((IDrawable**)&targetDrawable);
    }

    if (targetDrawable != NULL) {
        if (mAlpha != -1) {
            targetDrawable->SetAlpha(mAlpha);
        }
        if (mFilterMode != PorterDuffMode_NONE) {
            targetDrawable->SetColorFilter(mColorFilter, mFilterMode);
        }
        if (mLevel != -1) {
            Boolean res = FALSE;
            targetDrawable->SetLevel(mLevel, &res);
        }
    }
    return NOERROR;
}

ECode RemoteViews::_SetDrawableParameters::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "SetDrawableParameters";
    return NOERROR;
}

ECode RemoteViews::_SetDrawableParameters::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    Int32 background = 0;
    source->ReadInt32(&background);
    mTargetBackground = background != 0;
    source->ReadInt32(&mAlpha);
    source->ReadInt32(&mColorFilter);
    Int32 mode = 0;
    source->ReadInt32(&mode);
    Boolean hasMode = mode != 0;
    if (hasMode) {
        source->ReadInt32(&mFilterMode);
        //mFilterMode = PorterDuff.Mode.valueOf(parcel.readString());
    }
    else {
        mFilterMode = PorterDuffMode_NONE;
    }
    source->ReadInt32(&mLevel);
    return NOERROR;
}

ECode RemoteViews::_SetDrawableParameters::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mTargetBackground ? 1 : 0);
    dest->WriteInt32(mAlpha);
    dest->WriteInt32(mColorFilter);
    if (mFilterMode != PorterDuffMode_NONE) {
        dest->WriteInt32(1);
        dest->WriteInt32(mFilterMode);
    }
    else {
        dest->WriteInt32(0);
    }
    dest->WriteInt32(mLevel);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::ReflectionActionWithoutParams
//==============================================================================
const Int32 RemoteViews::ReflectionActionWithoutParams::TAG;

RemoteViews::ReflectionActionWithoutParams::ReflectionActionWithoutParams(
    /* [in] */ RemoteViews* host)
    : mHost(host)
{}

RemoteViews::ReflectionActionWithoutParams::ReflectionActionWithoutParams(
    /* [in] */ Int32 id,
    /* [in] */ const String& methodName,
    /* [in] */ RemoteViews* host)
    : Action(id)
    , mMethodName(methodName)
    , mHost(host)
{}

ECode RemoteViews::ReflectionActionWithoutParams::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr <IView> view;
    root->FindViewById(mViewId, (IView**)&view);
    if (view == NULL) {
        return NOERROR;
    }

    AutoPtr<IMethodInfo> methodInfo;
    ECode ec = mHost->GetMethod(view, mMethodName, String("()E"), (IMethodInfo**)&methodInfo);
    if (!FAILED(ec) && methodInfo != NULL) {
        return methodInfo->Invoke(view, NULL);
    } else {
        return ec;
    }
}

ECode RemoteViews::ReflectionActionWithoutParams::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "ReflectionActionWithoutParams";
    return NOERROR;
}

ECode RemoteViews::ReflectionActionWithoutParams::MergeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mMethodName.Equals("ShowNext") || mMethodName.Equals("ShowPrevious")) {
        *result = MERGE_IGNORE;
    }
    else {
        *result = MERGE_REPLACE;
    }
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::ReflectionActionWithoutParams::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    source->ReadString(&mMethodName);
    return NOERROR;
}

ECode RemoteViews::ReflectionActionWithoutParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteString(mMethodName);
    return NOERROR;
}

CAR_INTERFACE_IMPL(RemoteViews::BitmapCache, Object, IBitmapCache)

RemoteViews::BitmapCache::BitmapCache()
{}

RemoteViews::BitmapCache::BitmapCache(
    /* [in] */ IParcel* parcel)
{
    constructor(parcel);
}

ECode RemoteViews::BitmapCache::constructor()
{
    return NOERROR;
}

ECode RemoteViews::BitmapCache::constructor(
    /* [in] */ IParcel* parcel)
{
    Int32 count;
    parcel->ReadInt32(&count);
    for (Int32 i = 0; i< count; i++) {
        AutoPtr<IBitmap> bmp;
        CBitmap::New((IBitmap**)&bmp);
        IParcelable::Probe(bmp)->ReadFromParcel(parcel);
        mBitmaps.PushBack(bmp);
    }
    return NOERROR;
}

ECode RemoteViews::BitmapCache::GetBitmapId(
    /* [in] */ IBitmap* b,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    if (b == NULL) {
        *id = -1;
        return NOERROR;
    } else {
        List<AutoPtr<IBitmap> >::Iterator it = mBitmaps.Begin();
        Int32 index = 0;
        for (; it != mBitmaps.End(); it++) {
            if (b == *it) {
                *id = index;
                return NOERROR;
            }
            index++;
        }
        if (it == mBitmaps.End()) {
            AutoPtr<IBitmap> tmp = b;
            mBitmaps.PushBack(tmp);
        }
        *id = mBitmaps.GetSize() - 1;
        return NOERROR;
    }
}

ECode RemoteViews::BitmapCache::GetBitmapForId(
    /* [in] */ Int32 id,
    /* [out] */ IBitmap** bmp)
{
    if (id < 0 || id > mBitmaps.GetSize()) {
        *bmp = NULL;
    } else {
        *bmp = mBitmaps[id];
        REFCOUNT_ADD(*bmp)
    }
    return NOERROR;
}

ECode RemoteViews::BitmapCache::WriteBitmapsToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 count = mBitmaps.GetSize();
    dest->WriteInt32(count);
    List<AutoPtr<IBitmap> >::Iterator it = mBitmaps.Begin();
    for (; it != mBitmaps.End(); it++) {
        AutoPtr<IBitmap> bmp = *it;
        IParcelable::Probe(bmp)->WriteToParcel(dest);
    }
    return NOERROR;
}

ECode RemoteViews::BitmapCache::Assimilate(
    /* [in] */ IBitmapCache* bitmapCache)
{
    BitmapCache* impl = (BitmapCache*)bitmapCache;
    List<AutoPtr<IBitmap> >::Iterator it = impl->mBitmaps.Begin();
    for (; it != impl->mBitmaps.End(); it++) {
        AutoPtr<IBitmap> b = *it;
        List<AutoPtr<IBitmap> >::Iterator it2 = mBitmaps.Begin();
        for (; it2 != mBitmaps.End(); it2++) {
            if (b == *it2) {
                break;
            }
        }
        if (it2 ==mBitmaps.End()) {
            mBitmaps.PushBack(b);
        }
    }
    return NOERROR;
}

ECode RemoteViews::BitmapCache::AddBitmapMemory(
    /* [in] */ IMemoryUsageCounter* memoryCounter)
{
    List<AutoPtr<IBitmap> >::Iterator it = mBitmaps.Begin();
    for (; it != mBitmaps.End(); it++) {
        memoryCounter->AddBitmapMemory(*it);
    }
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::BitmapReflectionAction
//==============================================================================
const Int32 RemoteViews::BitmapReflectionAction::TAG;

RemoteViews::BitmapReflectionAction::BitmapReflectionAction(
    /* [in] */ RemoteViews* host)
    : mBitmapId(0)
    , mHost(host)
{}

RemoteViews::BitmapReflectionAction::BitmapReflectionAction(
    /* [in] */ Int32 id,
    /* [in] */ const String& methodName,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ RemoteViews* host)
    : Action(id)
    , mBitmapId(0)
    , mBitmap(bitmap)
    , mMethodName(methodName)
    , mHost(host)
{
    mHost->mBitmapCache->GetBitmapId(bitmap, &mBitmapId);
}

ECode RemoteViews::BitmapReflectionAction::SetBitmapCache(
    /* [in] */ IBitmapCache * bitmapCache)
{
    return bitmapCache->GetBitmapId(mBitmap, &mBitmapId);
}

ECode RemoteViews::BitmapReflectionAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<ReflectionAction> ra = new ReflectionAction(mViewId, mMethodName, ReflectionAction::BITMAP, mBitmap);
    return ra->Apply(root, rootParent, handler);
}

ECode RemoteViews::BitmapReflectionAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "BitmapReflectionAction";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::BitmapReflectionAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    source->ReadString(&mMethodName);
    source->ReadInt32(&mBitmapId);
    mBitmap = NULL;
    return mHost->mBitmapCache->GetBitmapForId(mBitmapId, (IBitmap**)&mBitmap);
}

ECode RemoteViews::BitmapReflectionAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteString(mMethodName);
    dest->WriteInt32(mBitmapId);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::ReflectionAction
//==============================================================================
const Int32 RemoteViews::ReflectionAction::TAG;
const Int32 RemoteViews::ReflectionAction::BOOLEAN;
const Int32 RemoteViews::ReflectionAction::BYTE;
const Int32 RemoteViews::ReflectionAction::SHORT;
const Int32 RemoteViews::ReflectionAction::INT;
const Int32 RemoteViews::ReflectionAction::LONG;
const Int32 RemoteViews::ReflectionAction::FLOAT;
const Int32 RemoteViews::ReflectionAction::DOUBLE;
const Int32 RemoteViews::ReflectionAction::CHAR;
const Int32 RemoteViews::ReflectionAction::STRING;
const Int32 RemoteViews::ReflectionAction::CHAR_SEQUENCE;
const Int32 RemoteViews::ReflectionAction::URI;
const Int32 RemoteViews::ReflectionAction::BITMAP;
const Int32 RemoteViews::ReflectionAction::BUNDLE;
const Int32 RemoteViews::ReflectionAction::INTENT;

RemoteViews::ReflectionAction::ReflectionAction()
    : mType(0)
{}

RemoteViews::ReflectionAction::ReflectionAction(
    /* [in] */ Int32 id,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 type,
    /* [in] */ IInterface* value)
    : Action(id)
    , mMethodName(methodName)
    , mType(type)
    , mValue(value)
{}

String RemoteViews::ReflectionAction::GetParameterType()
{
    switch (mType) {
        case BOOLEAN:
        {
            return String("(Z)E)");
        }
        case BYTE:
        {
            return String("(B)E)");
        }
        case SHORT:
        {
            return String("(I16)E)");
        }
        case INT:
        {
            return String("(I32)E)");
        }
        case LONG:
        {
            return String("(I64)E)");
        }
        case FLOAT:
        {
            return String("(F)E)");
        }
        case DOUBLE:
        {
            return String("(D)E)");
        }
        case CHAR:
        {
            return String("(C32)E)");
        }
        case STRING:
        {
            return String("(LElastos/String;)E)");
        }
        case CHAR_SEQUENCE:
        {
            return String("(LElastos/Core/ICharSequence;*)E");
        }
        case URI:
        {
            return String("(LElastos/Droid/Net/IUri;*)E");
        }
        case BITMAP:
        {
            return String("(LElastos/Droid/Graphics/IBitmap;*)E");
        }
        case BUNDLE:
        {
            return String("(LElastos/Droid/Os/IBundle;*)E");
        }
        case INTENT:
        {
            return String("(LElastos/Droid/Content/IIntent;*)E");
        }
        default:
            return String("()E");
    }
}

ECode RemoteViews::ReflectionAction::MergeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mMethodName.Equals(String("SmoothScrollBy"))) {
        *result = MERGE_APPEND;
    }
    else {
        *result = MERGE_REPLACE;
    }
    return NOERROR;
}

ECode RemoteViews::ReflectionAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    // PEL("RemoteViews::_ReflectionAction::Apply()")
    assert(root != NULL);
    AutoPtr<IView> view;
    root->FindViewById(mViewId, (IView**)&view);
    if (view == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String param = GetParameterType();
    AutoPtr<IClassInfo> klass = PropertyValuesHolder::TransformClassInfo(view);
    if (klass == NULL) {
        SLOGGERE("RemoteViews", "can not find view's class infomations")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String className;
    klass->GetName(&className);
    AutoPtr<IMethodInfo> methodInfo;
    if (FAILED(klass->GetMethodInfo(mMethodName, param, (IMethodInfo**)&methodInfo)) || methodInfo == NULL)
    {
        SLOGGERE("RemoteViews ", String("can not find method named: ") + mMethodName + "in view:" + className)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IArgumentList> argumentList = WrapArg(methodInfo);
    methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    assert(argumentList != NULL);

    switch (mType) {
        case BOOLEAN:
        {
            AutoPtr<IBoolean> value = IBoolean::Probe(mValue);
            assert(value != NULL);
            Boolean v = FALSE;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfBoolean(0, v);
            break;
        }
        case BYTE:
        {
            AutoPtr<IByte> value = IByte::Probe(mValue);
            assert(value != NULL);
            Byte v = 0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfByte(0, v);
            break;
        }
        case SHORT:
        {
            AutoPtr<IInteger16> value = IInteger16::Probe(mValue);
            assert(value != NULL);
            Int16 v = 0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfInt16(0, v);
            break;
        }
        case INT:
        {
            AutoPtr<IInteger32> value = IInteger32::Probe(mValue);
            assert(value != NULL);
            Int32 v = 0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfInt32(0, v);
            break;
        }
        case LONG:
        {
            AutoPtr<IInteger64> value = IInteger64::Probe(mValue);
            assert(value != NULL);
            Int64 v = 0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfInt64(0, v);
            break;
        }
        case FLOAT:
        {
            AutoPtr<IFloat> value = IFloat::Probe(mValue);
            assert(value != NULL);
            Float v = 0.f;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfFloat(0, v);
            break;
        }
        case DOUBLE:
        {
            AutoPtr<IDouble> value = IDouble::Probe(mValue);
            assert(value != NULL);
            Double v = 0.0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfDouble(0, v);
            break;
        }
        case CHAR:
        {
            AutoPtr<IChar32> value = IChar32::Probe(mValue);
            assert(value != NULL);
            Char32 v = 0;
            value->GetValue(&v);
            argumentList->SetInputArgumentOfInt32(0, v);
            break;
        }
        case STRING:
        {
            String v;
            if (mValue != NULL) {
                AutoPtr<ICharSequence> value = ICharSequence::Probe(mValue);
                value->ToString(&v);
            }
            argumentList->SetInputArgumentOfString(0, v);
            break;
        }
        case CHAR_SEQUENCE:
        {
            AutoPtr<ICharSequence> value = ICharSequence::Probe(mValue);
            argumentList->SetInputArgumentOfObjectPtr(0, value);
            break;
        }
        case URI:
        case BITMAP:
        case BUNDLE:
        case INTENT:
            argumentList->SetInputArgumentOfObjectPtr(0, mValue);
            break;
        default:
            break;
    }

    ECode ec = methodInfo->Invoke(view, argumentList);
    if (FAILED(ec)) {
        SLOGGERE("RemoteViews", String("method: ") + mMethodName + "in class: " + className + "invoke error")
    }
    return ec;
}

ECode RemoteViews::ReflectionAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    String str("ReflectionAction");
    str += mMethodName;
    str += StringUtils::ToString(mType);
    *name = str;
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::ReflectionAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    source->ReadString(&mMethodName);
    source->ReadInt32(&mType);
    // Logger::D("RemoteViews", "+ RemoteViews::_ReflectionAction::ReadFromParcel(), got type = %d; viewid = %d; methodName = %s", mType, mViewId, mMethodName.string());

    switch (mType) {
        case BOOLEAN:
        {
            Int32 res = 0;
            source->ReadInt32(&res);
            Boolean result = FALSE;
            result = res != 0;
            AutoPtr<IBoolean> b;
            CBoolean::New(result, (IBoolean**)&b);
            mValue = b;
            break;
        }
        case BYTE:
        {
            Byte byte;
            source->ReadByte(&byte);
            AutoPtr<IByte> ibyte;
            CByte::New(byte, (IByte**)&ibyte);
            mValue = ibyte;
            break;
        }
        case SHORT:
        {
            //this is right?
            Int16 s = 0;
            source->ReadInt16(&s);
            AutoPtr<IInteger16> integer16;
            CInteger16::New(s, (IInteger16**)&integer16);
            mValue = integer16;
            break;
        }
        case INT:
        {
            Int32 i = 0;
            source->ReadInt32(&i);
            AutoPtr<IInteger32> integer32;
            CInteger32::New(i, (IInteger32**)&integer32);
            mValue = integer32;
            break;
        }
        case LONG:
        {
            Int64 l = 0;
            source->ReadInt64(&l);
            AutoPtr<IInteger64> integer64;
            CInteger64::New(l, (IInteger64**)&integer64);
            mValue = integer64;
            break;
        }
        case FLOAT:
        {
            Float f = 0;
            source->ReadFloat(&f);
            AutoPtr<IFloat> fl;
            CFloat::New(f, (IFloat**)&fl);
            mValue = fl;
            break;
        }
        case DOUBLE:
        {
            Double d = 0;
            source->ReadDouble(&d);
            AutoPtr<IDouble> dou;
            CDouble::New(d, (IDouble**)&dou);
            mValue = dou;
            break;
        }
        case CHAR:
        {
            // this is right?
            Int32 c;
            source->ReadInt32(&c);
            AutoPtr<IChar32> ch;
            CChar32::New((Char32)c, (IChar32**)&ch);
            mValue = ch;
            break;
        }
        case STRING:
        case CHAR_SEQUENCE:
        {
            AutoPtr<ICharSequence> charCsq;
            FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&charCsq));
            mValue = charCsq;
            break;
        }
        case URI:
        {
            Int32 flag = 0;
            source->ReadInt32(&flag);
            if (flag != 0) {
                AutoPtr<IUri> uri;
                source->ReadInterfacePtr((Handle32*)&uri);

                if(uri != NULL){
                    mValue = uri;
                }
            }
            break;
        }
        case BITMAP:
        {
            Int32 flag = 0;
            source->ReadInt32(&flag);
            if(flag != 0) {
                AutoPtr<IBitmap> bmp;
                CBitmap::New((IBitmap**)&bmp);
                AutoPtr<IParcelable> p = IParcelable::Probe(bmp);
                p->ReadFromParcel(source);
                mValue = bmp;
            }
            break;
        }
        case BUNDLE:
        {
            Int32 flag = 0;
            source->ReadInt32(&flag);
            if(flag != 0) {
                AutoPtr<IBundle> bundle;
                CBundle::New((IBundle**)&bundle);
                AutoPtr<IParcelable> p = IParcelable::Probe(bundle);
                p->ReadFromParcel(source);
                mValue = bundle;
                break;
            }
        }
        case INTENT:
        {
            Int32 flag = 0;
            source->ReadInt32(&flag);
            if (flag) {
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                AutoPtr<IParcelable> p = IParcelable::Probe(intent);
                p->ReadFromParcel(source);
                mValue = intent;
            }
            break;
        }
        default:
            break;
    }
    // Logger::D("RemoteViews", "- RemoteViews::_ReflectionAction::ReadFromParcel()");
    return NOERROR;
}

ECode RemoteViews::ReflectionAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    // Logger::D("RemoteViews", "+ RemoteViews::_ReflectionAction::WriteToParcel(), type = %d; viewid = %d; method = %s", mType, mViewId, mMethodName.string());
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteString(mMethodName);
    dest->WriteInt32(mType);

    switch (mType) {
        case BOOLEAN:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type BOOLEAN");
            AutoPtr<IBoolean> b = IBoolean::Probe(mValue);
            Boolean res = FALSE;
            b->GetValue(&res);
            dest->WriteInt32(res ? 1 : 0);
            break;
        }
        case BYTE:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type BYTE");
            AutoPtr<IByte> ib = IByte::Probe(mValue);
            Byte by;
            ib->GetValue(&by);
            dest->WriteByte(by);
            break;
        }
        case SHORT:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type SHORT");
            AutoPtr<IInteger16> integer16 = IInteger16::Probe(mValue);
            Int16 i16 = 0;
            integer16->GetValue(&i16);
            dest->WriteInt16(i16);
            break;
        }
        case INT:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type INT");
            AutoPtr<IInteger32> integer32 = IInteger32::Probe(mValue);
            Int32 i32 = 0;
            integer32->GetValue(&i32);
            dest->WriteInt32(i32);
            break;
        }
        case LONG:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type LONG");
            AutoPtr<IInteger64> integer64 = IInteger64::Probe(mValue);
            Int64 i64 = 0;
            integer64->GetValue(&i64);
            dest->WriteInt64(i64);
            break;
        }
        case FLOAT:
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type FLOAT");
        {
            AutoPtr<IFloat> f = IFloat::Probe(mValue);
            Float fl = 0;
            f->GetValue(&fl);
            dest->WriteFloat(fl);
            break;
        }
        case DOUBLE:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type DOUBLE");
            AutoPtr<IDouble> d = IDouble::Probe(mValue);
            Double dou = 0;
            d->GetValue(&dou);
            dest->WriteDouble(dou);
            break;
        }
        case CHAR:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type CHAR");
            AutoPtr<IChar32> c = IChar32::Probe(mValue);
            Char32 ch;
            c->GetValue(&ch);
            dest->WriteInt32((Int32)ch);
            break;
        }
        case STRING:
        case CHAR_SEQUENCE:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type CHAR_SEQUENCE");
            AutoPtr<ICharSequence> charSeq = ICharSequence::Probe(mValue);
            TextUtils::WriteToParcel(charSeq, dest);
            break;
        }
        case URI:
        case BITMAP:
        case BUNDLE:
        case INTENT:
        {
            // Logger::D("RemoteViews", "RemoteViews::_ReflectionAction::WriteToParcel(), write type INTENT");
            if(mValue) {
                dest->WriteInt32(1);
                IParcelable::Probe(mValue)->WriteToParcel(dest);
            } else {
                dest->WriteInt32(0);
            }
            break;
        }
        default:
            SLOGGERW("RemoteViews", "RemoteViews::ReflectionAction::WriteToParcel(), write type unknown");
            break;
    }

    // Logger::D("RemoteViews", "- RemoteViews::_ReflectionAction::WriteToParcel()");
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::ViewGroupAction
//==============================================================================
const Int32 RemoteViews::ViewGroupAction::TAG;

RemoteViews::ViewGroupAction::ViewGroupAction(
    /* [in] */ RemoteViews* host)
    : mHost(host)
{}

RemoteViews::ViewGroupAction::ViewGroupAction(
    /* [in] */ Int32 viewId,
    /* [in] */ IRemoteViews* nestedViews,
    /* [in] */ RemoteViews* host)
    : Action(viewId)
    , mNestedViews(nestedViews)
    , mHost(host)
{
    if (nestedViews != NULL) {
        mHost->ConfigureRemoteViewsAsChild(nestedViews);
    }
}

ECode RemoteViews::ViewGroupAction::MergeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = MERGE_APPEND;
    return NOERROR;
}

ECode RemoteViews::ViewGroupAction::UpdateMemoryUsageEstimate(
    /* [in] */ IMemoryUsageCounter* counter)
{
    if (mNestedViews != NULL) {
        Int32 usage;
        mNestedViews->EstimateMemoryUsage(&usage);
        counter->Increment(usage);
    }
    return NOERROR;
}

ECode RemoteViews::ViewGroupAction::SetBitmapCache(
    /* [in] */ IBitmapCache* bitmapCache)
{
    if (mNestedViews != NULL) {
        RemoteViews* v = (RemoteViews*)(mNestedViews.Get());
        v->SetBitmapCache(bitmapCache);
    }
    return NOERROR;
}

ECode RemoteViews::ViewGroupAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IContext> context;
    root->GetContext((IContext**)&context);
    AutoPtr<IView> v;
    root->FindViewById(mViewId, (IView**)&v);
    AutoPtr<IViewGroup> target = IViewGroup::Probe(v);
    if (target == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mNestedViews != NULL) {
        AutoPtr<IView> view;
        mNestedViews->Apply(context, target, handler, (IView**)&view);
        target->AddView(view);
    }
    else {
        target->RemoveAllViews();
    }
    return NOERROR;
}

ECode RemoteViews::ViewGroupAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    String str("ViewGroupAction");
    if (mNestedViews) {
        str += "Add";
    }
    else {
        str += "Remove";
    }
    *name = str;
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::ViewGroupAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return ReadFromParcel(source, NULL);
}

ECode RemoteViews::ViewGroupAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    if (mNestedViews) {
        dest->WriteInt32(1);
        IParcelable::Probe(mNestedViews)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

ECode RemoteViews::ViewGroupAction::ReadFromParcel(
    /* [in] */ IParcel* source,
    /* [in] */ IBitmapCache* bitmapCache)
{
    source->ReadInt32(&mViewId);
    Int32 res = 0;
    source->ReadInt32(&res);
    Boolean nestedViewsNull = res == 0;
    if (!nestedViewsNull) {
        CRemoteViews::New((IRemoteViews**)&mNestedViews);
        ((RemoteViews*)mNestedViews.Get())->constructor(source, bitmapCache);
    }
    else {
        mNestedViews = NULL;
    }
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::TextViewDrawableAction
//==============================================================================
const Int32 RemoteViews::TextViewDrawableAction::TAG;

RemoteViews::TextViewDrawableAction::TextViewDrawableAction()
    : mIsRelative(FALSE)
    , mD1(0)
    , mD2(0)
    , mD3(0)
    , mD4(0)
{}

RemoteViews::TextViewDrawableAction::TextViewDrawableAction(
    /* [in] */ Int32 viewId,
    /* [in] */ Boolean isRelative,
    /* [in] */ Int32 d1,
    /* [in] */ Int32 d2,
    /* [in] */ Int32 d3,
    /* [in] */ Int32 d4)
    : Action(viewId)
    , mIsRelative(isRelative)
    , mD1(d1)
    , mD2(d2)
    , mD3(d3)
    , mD4(d4)
{}

ECode RemoteViews::TextViewDrawableAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> v;
    root->FindViewById(mViewId, (IView**)&v);
    AutoPtr<ITextView> target = ITextView::Probe(v);
    if (target == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsRelative) {
        return target->SetCompoundDrawablesRelativeWithIntrinsicBounds(mD1, mD2, mD3, mD4);
    }
    else {
        return target->SetCompoundDrawablesWithIntrinsicBounds(mD1, mD2, mD3, mD4);
    }
}

ECode RemoteViews::TextViewDrawableAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "TextViewDrawableAction";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::TextViewDrawableAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // need not to read TAG again!
    source->ReadInt32(&mViewId);
    Int32 res = 0;
    source->ReadInt32(&res);
    mIsRelative = res != 0;
    source->ReadInt32(&mD1);
    source->ReadInt32(&mD2);
    source->ReadInt32(&mD3);
    source->ReadInt32(&mD4);
    return NOERROR;
}

ECode RemoteViews::TextViewDrawableAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mIsRelative ? 1 : 0);
    dest->WriteInt32(mD1);
    dest->WriteInt32(mD2);
    dest->WriteInt32(mD3);
    dest->WriteInt32(mD4);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::TextViewSizeAction
//==============================================================================
const Int32 RemoteViews::TextViewSizeAction::TAG;

RemoteViews::TextViewSizeAction::TextViewSizeAction()
    : mUnits(0)
    , mSize(0.0)
{}

RemoteViews::TextViewSizeAction::TextViewSizeAction(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 units,
    /* [in] */ Float size)
    : Action(viewId)
    , mUnits(units)
    , mSize(size)
{}

ECode RemoteViews::TextViewSizeAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> v;
    root->FindViewById(mViewId, (IView**)&v);
    AutoPtr<ITextView> target = ITextView::Probe(v);
    if (target == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return target->SetTextSize(mUnits, mSize);
}

ECode RemoteViews::TextViewSizeAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "TextViewSizeAction";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::TextViewSizeAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mViewId);
    source->ReadInt32(&mUnits);
    source->ReadFloat(&mSize);
    return NOERROR;
}

ECode RemoteViews::TextViewSizeAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mUnits);
    dest->WriteFloat(mSize);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::ViewPaddingAction
//==============================================================================
const Int32 RemoteViews::ViewPaddingAction::TAG;

RemoteViews::ViewPaddingAction::ViewPaddingAction()
    : mLeft(0)
    , mTop(0)
    , mRight(0)
    , mBottom(0)
{}

RemoteViews::ViewPaddingAction::ViewPaddingAction(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
    : Action(viewId)
    , mLeft(left)
    , mTop(top)
    , mRight(right)
    , mBottom(bottom)
{}

ECode RemoteViews::ViewPaddingAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> target;
    root->FindViewById(mViewId, (IView**)&target);
    if (target == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    target->SetPadding(mLeft, mTop, mRight, mBottom);
    return NOERROR;
}

ECode RemoteViews::ViewPaddingAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "ViewPaddingAction";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::ViewPaddingAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mViewId);
    source->ReadInt32(&mLeft);
    source->ReadInt32(&mTop);
    source->ReadInt32(&mRight);
    source->ReadInt32(&mBottom);
    return NOERROR;
}

ECode RemoteViews::ViewPaddingAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mLeft);
    dest->WriteInt32(mTop);
    dest->WriteInt32(mRight);
    dest->WriteInt32(mBottom);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::TextViewDrawableColorFilterAction
//==============================================================================

const Int32 RemoteViews::TextViewDrawableColorFilterAction::TAG;

RemoteViews::TextViewDrawableColorFilterAction::TextViewDrawableColorFilterAction()
    : Action(0)
    , mIsRelative(FALSE)
    , mIndex(0)
    , mColor(0)
    , mMode(PorterDuffMode_NONE)
{}

RemoteViews::TextViewDrawableColorFilterAction::TextViewDrawableColorFilterAction(
    /* [in] */ Int32 viewId,
    /* [in] */ Boolean isRelative,
    /* [in] */ Int32 index,
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
    : Action(viewId)
    , mIsRelative(isRelative)
    , mIndex(index)
    , mColor(color)
    , mMode(mode)
{}

ECode RemoteViews::TextViewDrawableColorFilterAction::Apply(
    /* [in] */ IView* root,
    /* [in] */ IViewGroup* rootParent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IView> v;
    root->FindViewById(mViewId, (IView**)&v);
    AutoPtr<ITextView> target = ITextView::Probe(v);
    if (target == NULL) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<IDrawable*> > drawables;
    if (mIsRelative) {
            target->GetCompoundDrawablesRelative((ArrayOf<IDrawable*>**)&drawables);
    } else {
            target->GetCompoundDrawables((ArrayOf<IDrawable*>**)&drawables);
    }
    if (mIndex < 0 || mIndex >= 4) {
        SLOGGERE("RemoteViews", "index must be in range [0, 3].")
        return NOERROR;
    }
    AutoPtr<IDrawable> d = (*drawables)[mIndex];
    if (d != NULL) {
        d->Mutate();
        d->SetColorFilter(mColor, mMode);
    }
    return NOERROR;
}

ECode RemoteViews::TextViewDrawableColorFilterAction::GetActionName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = "TextViewDrawableColorFilterAction";
    return NOERROR;
}

// interface IParcelable
ECode RemoteViews::TextViewDrawableColorFilterAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mViewId);
    Int32 isRelative;
    source->ReadInt32(&isRelative);
    mIsRelative =  isRelative == 1;
    source->ReadInt32(&mIndex);
    source->ReadInt32(&mColor);
    source->ReadInt32(&mMode);
    return NOERROR;
}

ECode RemoteViews::TextViewDrawableColorFilterAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(TAG);
    dest->WriteInt32(mViewId);
    dest->WriteInt32(mIsRelative ? 1 : 0);
    dest->WriteInt32(mIndex);
    dest->WriteInt32(mColor);
    dest->WriteInt32(mMode);
    return NOERROR;
}

//==============================================================================
//                  RemoteViews::FillInIntentClickListener
//==============================================================================
CAR_INTERFACE_IMPL(RemoteViews::FillInIntentClickListener, Object, IViewOnClickListener)

RemoteViews::FillInIntentClickListener::FillInIntentClickListener(
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
    : mIntent(fillInIntent)
    , mHandler(handler)
{}

ECode RemoteViews::FillInIntentClickListener::FillInIntentClickListener::OnClick(
    /* [in] */ IView* v)
{
    // Insure that this view is a child of an AdapterView
    AutoPtr<IViewParent> pa;
    v->GetParent((IViewParent**)&pa);
    AutoPtr<IView> parent = IView::Probe(pa);
    while (parent != NULL && IAppWidgetHostView::Probe(parent) == NULL) {
        pa = NULL;
        parent->GetParent((IViewParent**)&pa);
        parent = IView::Probe(pa);
    }

    if (IAppWidgetHostView::Probe(parent) != NULL || parent == NULL) {
        // Somehow they've managed to get this far without having
        // and AdapterView as a parent.
        SLOGGERE("RemoteViews", "Collection item doesn't have AdapterView parent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Insure that a template pending intent has been set on an ancestor

    AutoPtr<IInterface> tag;
    parent->GetTag((IInterface**)&tag);
    if (IPendingIntent::Probe(tag) == NULL) {
        SLOGGERE("RemoteViews", "Attempting setOnClickFillInIntent without calling setPendingIntentTemplate on parent.")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IPendingIntent> pendingIntent = IPendingIntent::Probe(tag);

    AutoPtr<IRect> rect = GetSourceBounds(v);

    mIntent->SetSourceBounds(rect);
    Boolean succeed = FALSE;
    mHandler->OnClickHandler(v, pendingIntent, mIntent, &succeed);

    return NOERROR;
}

//==============================================================================
//                  RemoteViews::PendingIntentClickListener
//==============================================================================
CAR_INTERFACE_IMPL(RemoteViews::PendingIntentClickListener, Object, IViewOnClickListener)

RemoteViews::PendingIntentClickListener::PendingIntentClickListener(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
    : mIntent(intent)
    , mHandler(handler)
{}

ECode RemoteViews::PendingIntentClickListener::OnClick(
    /* [in] */ IView* v)
{
    // Find target view location in screen coordinates and
    // fill into PendingIntent before sending.

    AutoPtr<IRect> rect = GetSourceBounds(v);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetSourceBounds(rect);
    Boolean succeed = FALSE;
    mHandler->OnClickHandler(v, mIntent, intent, &succeed);

    return NOERROR;
}

//==============================================================================
//                  RemoteViews::IntentTemplateOnItemClickListener
//==============================================================================
CAR_INTERFACE_IMPL(RemoteViews::IntentTemplateOnItemClickListener, Object, IAdapterViewOnItemClickListener)

RemoteViews::IntentTemplateOnItemClickListener::IntentTemplateOnItemClickListener(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
    : mIntent(intent)
    , mHandler(handler)
{}

ECode RemoteViews::IntentTemplateOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (IViewGroup::Probe(view)) {
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(view);
        if (IAdapterViewAnimator::Probe(parent)) {
            AutoPtr<IView> v;
            vg->GetChildAt(0, (IView**)&v);
            vg = IViewGroup::Probe(v);
        }
        if (!vg) return NOERROR;
        AutoPtr<IIntent> fillInIntent;
        Int32 childCount = 0;
        vg->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> iview;
            vg->GetChildAt(i, (IView**)&iview);
            AutoPtr<IInterface> tag;
            iview->GetTag(R::id::fillInIntent, (IInterface**)&tag);
            if (IIntent::Probe(tag)) {
                fillInIntent = IIntent::Probe(tag);
                break;
            }
        }

        if (!fillInIntent) return NOERROR;

        AutoPtr<IRect> rect = GetSourceBounds(view);
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetSourceBounds(rect);
        Boolean succeed = FALSE;
        mHandler->OnClickHandler(view, mIntent, intent, &succeed);
    }
    return NOERROR;
}

RemoteViews::MyContextWrapper::MyContextWrapper(
    /* [in] */ IContext* ctx,
    /* [in] */ IContext* contextForResources)
{
    ContextWrapper::constructor(ctx);
    mContextForResources = contextForResources;
}


ECode RemoteViews::MyContextWrapper::GetResources(
    /* [out] */ IResources** res)
{
    return mContextForResources->GetResources(res);
}

RemoteViews::MyContextWrapper::GetTheme(
    /* [out] */ IResourcesTheme** theme)
{
    return mContextForResources->GetTheme(theme);
}
//==============================================================================
//                  RemoteViews
//==============================================================================
CAR_INTERFACE_IMPL_3(RemoteViews, Object, IRemoteViews, IParcelable, ILayoutInflaterFilter)

static void ThreadDestructor(void* st)
{
    AutoPtr<IArgumentList> argument = static_cast<IArgumentList*>(st);
    if (argument) {
        argument->Release();
    }
}

static Boolean InitTls()
{
    Int32 result = pthread_key_create(&RemoteViews::sInvokeArgsTls, ThreadDestructor);
    return result == 0 ;
}

static AutoPtr<IRemoteViewsOnClickHandler> InitHandler()
{
    AutoPtr<IRemoteViewsOnClickHandler> temp = new RemoteViews::RemoteViewsOnClickHandler();
    return temp;
}

const String RemoteViews::TAG("RemoteViews");
const String RemoteViews::EXTRA_REMOTEADAPTER_APPWIDGET_ID("remoteAdapterAppWidgetId");
const Int32 RemoteViews::MODE_NORMAL;
const Int32 RemoteViews::MODE_HAS_LANDSCAPE_AND_PORTRAIT;
Object RemoteViews::sMethodsLock;
AutoPtr<IArrayMap> RemoteViews::sMethods;
pthread_key_t RemoteViews::sInvokeArgsTls;
Boolean RemoteViews::sHaveInitTls = InitTls();
const AutoPtr<IRemoteViewsOnClickHandler> RemoteViews::DEFAULT_ON_CLICK_HANDLER = InitHandler();

RemoteViews::RemoteViews()
    : mLayoutId(0)
    , mIsRoot(TRUE)
    , mIsWidgetCollectionChild(FALSE)
{}

ECode RemoteViews::constructor()
{
    return NOERROR;
}

ECode RemoteViews::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 layoutId)
{
    AutoPtr<IApplicationInfo> info;
    GetApplicationInfo(packageName, UserHandle::GetMyUserId(), (IApplicationInfo**)&info);
    return constructor(info, layoutId);
}

ECode RemoteViews::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 layoutId)
{
    AutoPtr<IApplicationInfo> info;
    GetApplicationInfo(packageName, userId, (IApplicationInfo**)&info);
    return constructor(info, layoutId);
}

    /**
     * Create a new RemoteViews object that will display the views contained
     * in the specified layout file.
     *
     * @param application The application whose content is shown by the views.
     * @param layoutId The id of the layout resource.
     *
     * @hide
     */
ECode RemoteViews::constructor(
    /* [in] */ IApplicationInfo* application,
    /* [in] */ Int32 layoutId)
{
    mApplication = application;
    mLayoutId = layoutId;
    mBitmapCache = new BitmapCache();
    // setup the memory usage statistics

    mMemoryUsageCounter = new MemoryUsageCounter();
    RecalculateMemoryUsage();
    return NOERROR;
}

    /**
     * Create a new RemoteViews object that will inflate as the specified
     * landspace or portrait RemoteViews, depending on the current configuration.
     *
     * @param landscape The RemoteViews to inflate in landscape configuration
     * @param portrait The RemoteViews to inflate in portrait configuration
     */
ECode RemoteViews::constructor(
        /* [in] */ IRemoteViews* landscape,
        /* [in] */ IRemoteViews* portrait)
{
    if (landscape == NULL || portrait == NULL) {
        SLOGGERE("RemoteViews", "Both RemoteViews must be non-null")
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IApplicationInfo> landApp = ((RemoteViews*)landscape)->mApplication;
    AutoPtr<IApplicationInfo> portraitApp = ((RemoteViews*)portrait)->mApplication;
    Int32 landUid, portraitUid;
    landApp->GetUid(&landUid);
    portraitApp->GetUid(&portraitUid);
    String landAppName, portraitAppName;
    IPackageItemInfo::Probe(landApp)->GetPackageName(&landAppName);
    IPackageItemInfo::Probe(portraitApp)->GetPackageName(&portraitAppName);

    if (landUid != portraitUid|| ! landAppName.Equals(portraitAppName))
    {
        SLOGGERE("RemoteViews", "Both RemoteViews must share the same package and user")
        return E_RUNTIME_EXCEPTION;
    }
    mApplication = portraitApp;
    portrait->GetLayoutId(&mLayoutId);

    mLandscape = landscape;
    mPortrait = portrait;

    // setup the memory usage statistics
    mMemoryUsageCounter = new MemoryUsageCounter();

    mBitmapCache = new BitmapCache();
    ConfigureRemoteViewsAsChild(landscape);
    ConfigureRemoteViewsAsChild(portrait);

    RecalculateMemoryUsage();
    return NOERROR;
}

ECode RemoteViews::constructor(
    /* [in] */ IParcel* parcel,
    /* [in] */ IBitmapCache* bitmapCache)
{
    Int32 mode;
    parcel->ReadInt32(&mode);

    // We only store a bitmap cache in the root of the RemoteViews.
    if (bitmapCache == NULL) {
        mBitmapCache = new BitmapCache(parcel);
    } else {
        SetBitmapCache(bitmapCache);
        SetNotRoot();
    }

    if (mode == MODE_NORMAL) {
        parcel->ReadInterfacePtr((Handle32*)&mApplication);
        parcel->ReadInt32(&mLayoutId);
        Int32 tmp;
        parcel->ReadInt32(&tmp);
        mIsWidgetCollectionChild = tmp == 1;

        Int32 count;
        parcel->ReadInt32(&count);
        if (count > 0) {
            for (int i=0; i<count; i++) {
                Int32 tag;
                parcel->ReadInt32(&tag);
                AutoPtr<IRemoteViewsAction> actionImpl;
                switch (tag) {
                    case _SetOnClickPendingIntent::TAG:
                        actionImpl = new _SetOnClickPendingIntent(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case _SetDrawableParameters::TAG:
                        actionImpl = new _SetDrawableParameters();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case ReflectionAction::TAG:
                        actionImpl = new ReflectionAction();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case ViewGroupAction::TAG:
                        actionImpl = new ViewGroupAction(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case ReflectionActionWithoutParams::TAG:
                        actionImpl = new ReflectionActionWithoutParams(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case _SetEmptyView::TAG:
                        actionImpl = new _SetEmptyView();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case _SetPendingIntentTemplate::TAG:
                        actionImpl = new _SetPendingIntentTemplate(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case _SetOnClickFillInIntent::TAG:
                        actionImpl = new _SetOnClickFillInIntent(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case SetRemoteViewsAdapterIntent::TAG:
                        actionImpl = new SetRemoteViewsAdapterIntent();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case TextViewDrawableAction::TAG:
                        actionImpl = new TextViewDrawableAction();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case TextViewSizeAction::TAG:
                        actionImpl = new TextViewDrawableAction();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case ViewPaddingAction::TAG:
                        actionImpl = new ViewPaddingAction();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case BitmapReflectionAction::TAG:
                        actionImpl = new BitmapReflectionAction(this);
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case SetRemoteViewsAdapterList::TAG:
                        actionImpl = new SetRemoteViewsAdapterList();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    case TextViewDrawableColorFilterAction::TAG:
                        actionImpl = new TextViewDrawableColorFilterAction();
                        IParcelable::Probe(actionImpl)->ReadFromParcel(parcel);
                        mActions.PushBack(actionImpl);
                        break;
                    default:
                        SLOGGERE(TAG, String("tag not found: ") + StringUtils::ToString(tag))
                        return E_RUNTIME_EXCEPTION;
                }
            }
        }
    } else {
        // MODE_HAS_LANDSCAPE_AND_PORTRAIT
        CRemoteViews::New((IRemoteViews**)&mLandscape);
        CRemoteViews::New((IRemoteViews**)&mPortrait);
        ((RemoteViews*)mLandscape.Get())->constructor(parcel, mBitmapCache);
        ((RemoteViews*)mLandscape.Get())->constructor(parcel, mBitmapCache);
        mApplication = ((RemoteViews*)mPortrait.Get())->mApplication;
        mPortrait->GetLayoutId(&mLayoutId);
    }

    // setup the memory usage statistics
    mMemoryUsageCounter = new MemoryUsageCounter();
    RecalculateMemoryUsage();
    return NOERROR;
}

ECode RemoteViews::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return constructor(source, NULL);
}

ECode RemoteViews::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (!HasLandscapeAndPortraitLayouts()) {
        dest->WriteInt32(MODE_NORMAL);
        // We only write the bitmap cache if we are the root RemoteViews, as this cache
        // is shared by all children.
        if (mIsRoot) {
            mBitmapCache->WriteBitmapsToParcel(dest);
        }
        IParcelable::Probe(mApplication)->WriteToParcel(dest);
        dest->WriteInt32(mLayoutId);
        dest->WriteInt32(mIsWidgetCollectionChild ? 1 : 0);
        Int32 count = mActions.GetSize();
        dest->WriteInt32(count);
        for (int i=0; i<count; i++) {
            AutoPtr<IRemoteViewsAction> a = mActions[i];
            IParcelable::Probe(a)->WriteToParcel(dest);
        }
    } else {
        dest->WriteInt32(MODE_HAS_LANDSCAPE_AND_PORTRAIT);
        // We only write the bitmap cache if we are the root RemoteViews, as this cache
        // is shared by all children.
        if (mIsRoot) {
            mBitmapCache->WriteBitmapsToParcel(dest);
        }
        IParcelable::Probe(mLandscape)->WriteToParcel(dest);
        IParcelable::Probe(mLandscape)->WriteToParcel(dest);
    }
    return NOERROR;
}

ECode RemoteViews::SetNotRoot()
{
    mIsRoot = FALSE;
    return NOERROR;
}

void RemoteViews::ConfigureRemoteViewsAsChild(
    /* [in] */ IRemoteViews* rv)
{
    RemoteViews* remote = (RemoteViews*)rv;
    mBitmapCache->Assimilate(remote->mBitmapCache);
    remote->SetBitmapCache(mBitmapCache);
    remote->SetNotRoot();
}

Boolean RemoteViews::HasLandscapeAndPortraitLayouts()
{
    return (mLandscape != NULL) && (mPortrait != NULL);
}

AutoPtr<IRemoteViews> RemoteViews::GetRemoteViewsToApply(
    /* [in] */ IContext* context)
{
    if (HasLandscapeAndPortraitLayouts()) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> config;
        resources->GetConfiguration((IConfiguration**)&config);
        Int32 orientation = 0;
        config->GetOrientation(&orientation);
        if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
            return mLandscape;
        } else {
            return mPortrait;
        }
    }

    return this;
}

void RemoteViews::PerformApply(
    /* [in] */ IView* v,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    if (!mActions.IsEmpty()) {
        if (!handler) {
            handler = DEFAULT_ON_CLICK_HANDLER;
        }
        List< AutoPtr<IRemoteViewsAction> >::Iterator it;
        for (it = mActions.Begin(); it != mActions.End(); ++it) {
            AutoPtr<IRemoteViewsAction> a = *it;
            a->Apply(v, parent, handler);
        }
    }
}

AutoPtr<IContext> RemoteViews::GetContextForResources(
    /* [in] */ IContext* context,
    /* [in] */ const String& themePackageName)
{
    if (mApplication != NULL) {
        Int32 appUid;
        mApplication->GetUid(&appUid);
        Int32 userHandleId = CUserHandle::GetUserId(appUid);
        Int32 ctxUid;
        context->GetUserId(&ctxUid);
        String pkgName;
        context->GetPackageName(&pkgName);
        String appPkgName;
        IPackageItemInfo::Probe(mApplication)->GetPackageName(&appPkgName);
        if (ctxUid == userHandleId && pkgName.Equals(appPkgName)) {
            return context;
        }
        // try {
            AutoPtr<IContext> result;
            if (FAILED(context->CreateApplicationContext(mApplication, themePackageName,
                    IContext::CONTEXT_RESTRICTED, (IContext**)&result)))
            {
                SLOGGERE(TAG, String("Package name ") + appPkgName + " not found");
            }
        // } catch (NameNotFoundException e) {
        // }
    }

    return context;
}

ECode RemoteViews::MergeRemoteViews(
    /* [in] */ IRemoteViews* newRv)
{
    if (newRv == NULL) {
        return NOERROR;
    }
    AutoPtr<IRemoteViews> copy;
    newRv->Clone((IRemoteViews**)&copy);;

    HashMap<String, AutoPtr<IRemoteViewsAction> > map;

    // We first copy the new RemoteViews, as the process of merging modifies the way the actions
    // reference the bitmap cache. We don't want to modify the object as it may need to
    // be merged and applied multiple times.
    List<AutoPtr<IRemoteViewsAction> >::Iterator it = mActions.Begin();

    for (; it != mActions.End(); it++) {
        AutoPtr<IRemoteViewsAction> a = *it;
        String uniqueKey;
        a->GetUniqueKey(&uniqueKey);
        map[uniqueKey] = a;
    }

    List<AutoPtr<IRemoteViewsAction> > newActions = ((RemoteViews*)copy.Get())->mActions;
    List<AutoPtr<IRemoteViewsAction> >::Iterator newIt = newActions.Begin();
    for (; newIt != newActions.End(); newIt++) {
        AutoPtr<IRemoteViewsAction> a = *newIt;
        String key;
        a->GetUniqueKey(&key);
        Int32 mergeBehavior;
        (*newIt)->MergeBehavior(&mergeBehavior);
        if (map.Find(key) != map.End() && mergeBehavior == Action::MERGE_REPLACE) {
            mActions.Remove(map[key]);
            map.Erase(key);
        }

        // If the merge behavior is ignore, we don't bother keeping the extra action
        if (mergeBehavior == Action::MERGE_REPLACE || mergeBehavior == Action::MERGE_APPEND) {
            mActions.PushBack(a);
        }
    }

    // Because pruning can remove the need for bitmaps, we reconstruct the bitmap cache
    mBitmapCache = new BitmapCache();
    SetBitmapCache(mBitmapCache);
    return NOERROR;
}

ECode RemoteViews::Clone(
    /* [out] */ IRemoteViews** remoteViews)
{
    AutoPtr<IParcel> source;
    CParcel::New((IParcel**)&source);
    WriteToParcel(source);
    source->SetDataPosition(0);
    CRemoteViews::New(source, NULL, remoteViews);
    // source->Recycle();
    return NOERROR;
}

ECode RemoteViews::GetPackage(
    /* [out] */ String* pkg)
{
    VALIDATE_NOT_NULL(pkg)

    if (mApplication != NULL) {
        return IPackageItemInfo::Probe(mApplication)->GetPackageName(pkg);
    } else {
        *pkg = String(NULL);
    }

    return NOERROR;
}

ECode RemoteViews::GetLayoutId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mLayoutId;
    return NOERROR;
}

ECode RemoteViews::SetIsWidgetCollectionChild(
    /* [in] */ Boolean isWidgetCollectionChild)
{
    mIsWidgetCollectionChild = isWidgetCollectionChild;
    return NOERROR;
}

ECode RemoteViews::GetSequenceNumber(
    /* [out] */ Int32* number)
{
    *number = mActions.GetSize();
    return NOERROR;
}

void RemoteViews::RecalculateMemoryUsage()
{
    mMemoryUsageCounter->Clear();
    if (!HasLandscapeAndPortraitLayouts()) {
        if (!mActions.IsEmpty()) {
            List< AutoPtr<IRemoteViewsAction> >::Iterator it;
            for (it = mActions.Begin(); it != mActions.End(); ++it) {
                (*it)->UpdateMemoryUsageEstimate(mMemoryUsageCounter);
            }
        }
        if (mIsRoot) {
            mBitmapCache->AddBitmapMemory(mMemoryUsageCounter);
        }
    }
    else {
        Int32 land = 0, port = 0;
        mLandscape->EstimateMemoryUsage(&land);
        mPortrait->EstimateMemoryUsage(&port);
        mMemoryUsageCounter->Increment(land);
        mMemoryUsageCounter->Increment(port);
        mBitmapCache->AddBitmapMemory(mMemoryUsageCounter);
    }
}

void RemoteViews::SetBitmapCache(
    /* [in] */ IBitmapCache* bitmapCache)
{
    mBitmapCache = bitmapCache;
    if (!HasLandscapeAndPortraitLayouts()) {
        if (!mActions.IsEmpty()) {
            List< AutoPtr<IRemoteViewsAction> >::Iterator it;
            for (it = mActions.Begin(); it != mActions.End(); ++it) {
                (*it)->SetBitmapCache(bitmapCache);
            }
        }
    }
    else {
        RemoteViews* land = (RemoteViews*)mLandscape.Get();
        RemoteViews* prot = (RemoteViews*)mLandscape.Get();
        land->SetBitmapCache(bitmapCache);
        prot->SetBitmapCache(bitmapCache);
    }
}

ECode RemoteViews::EstimateMemoryUsage(
    /* [out] */ Int32* usage)
{
    return mMemoryUsageCounter->GetMemoryUsage(usage);
}

ECode RemoteViews::AddAction(
    /* [in] */ IRemoteViewsAction* a)
{
    if (HasLandscapeAndPortraitLayouts()) {
       SLOGGERE("RemoteViews", String("RemoteViews specifying separate landscape and portrait") +
            " layouts cannot be modified. Instead, fully configure the landscape and" +
            " portrait layouts individually before constructing the combined layout.");
        return E_RUNTIME_EXCEPTION;
    }
    mActions.PushBack(a);
    a->UpdateMemoryUsageEstimate(mMemoryUsageCounter);
    return NOERROR;
}

AutoPtr<IRect> RemoteViews::GetSourceBounds(
    /* [out] */ IView* v)
{
    AutoPtr<IContext> ctx;
    v->GetContext((IContext**)&ctx);
    AutoPtr<IResources> resources;
    ctx->GetResources((IResources**)&resources);
    AutoPtr<ICompatibilityInfo> compatibilityInfo;
    resources->GetCompatibilityInfo((ICompatibilityInfo**)&compatibilityInfo);
    Float appScale;
    compatibilityInfo->GetApplicationScale(&appScale);
    AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
    v->GetLocationOnScreen(pos);

    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    Int32 w, h;
    v->GetWidth(&w);
    v->GetHeight(&h);
    rect->SetLeft((Int32) ((*pos)[0] * appScale + 0.5f));
    rect->SetTop((Int32) ((*pos)[1] * appScale + 0.5f));
    rect->SetRight((Int32) (((*pos)[0] + w) * appScale + 0.5f));
    rect->SetBottom((Int32) (((*pos)[1] + h) * appScale + 0.5f));
    return rect;
}

ECode RemoteViews::GetMethod(
    /* [in] */ IView* view,
    /* [in] */ const String& methodName,
    /* [in] */ const String& paramType,
    /* [out] */ IMethodInfo** info)
{
    AutoPtr<IMethodInfo> method;
    AutoPtr<IClassInfo> klass = PropertyValuesHolder::TransformClassInfo(view);
    if (klass == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock lock(sMethodsLock);
        AutoPtr<IInterface> obj;
        sMethods->Get(klass, (IInterface**)&obj);;
        AutoPtr<IArrayMap> methods = IArrayMap::Probe(obj);
        if (methods == NULL) {
            CArrayMap::New((IArrayMap**)&methods);
            sMethods->Put(klass, methods);
        }

        mPair->SetFirst(methodName);
        mPair->SetSecond(paramType);

        obj = NULL;
        methods->Get(mPair, (IInterface**)&obj);
        method = IMethodInfo::Probe(obj);
        if (method == NULL) {
            FAIL_RETURN(klass->GetMethodInfo(methodName, paramType, (IMethodInfo**)&method))
        }

        String className;
        klass->GetName(&className);
        if (method == NULL) {
            SLOGGERE("RemoteViews", String("view: ") + className + " doesn't have method: " + methodName + paramType)
        } else {
            String annotation;
            method->GetAnnotation(&annotation);
            if (!annotation.Equals(String("RemotableViewMethod"))) {
                SLOGGERE("RemoteViews", String("view: ") + className + " can't use method with RemoteViews: " + methodName + paramType)
            }

            AutoPtr<IMutablePair> pair = new MutablePair(methodName, paramType);
            methods->Put(pair, method);
        }
    }
    *info = method;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

AutoPtr<IArgumentList> RemoteViews::WrapArg(
    /* [in] */ IMethodInfo* method)
{
    AutoPtr<IArgumentList> argument = (IArgumentList*)pthread_getspecific(sInvokeArgsTls);
    if (argument != NULL) {
        argument->Release();
    }
    argument == NULL;
    method->CreateArgumentList((IArgumentList**)&argument);
    pthread_setspecific(sInvokeArgsTls, argument.Get());
    argument->AddRef();
    return argument;
}

ECode RemoteViews::GetApplicationInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ IApplicationInfo** info)
{
        if (packageName == NULL) {
            *info = NULL;
            return NOERROR;
        }

        // Get the application for the passed in package and user.
        AutoPtr<IApplication> application = CActivityThread::GetCurrentApplication();
        if (application == NULL) {
            SLOGGERE(TAG, "Cannot create remote views out of an aplication.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        AutoPtr<IApplicationInfo> applicationInfo;
        IContext::Probe(application)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 uid;
        applicationInfo->GetUid(&uid);
        String pkgName;
        IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&pkgName);

        if (UserHandle::GetUserId(uid) != userId
                || !pkgName.Equals(packageName)) {
            // try {
                AutoPtr<IContext> baseContext;
                IContextWrapper::Probe(application)->GetBaseContext((IContext**)&baseContext);
                AutoPtr<IUserHandle> uhandle;
                CUserHandle::New(userId, (IUserHandle**)&uhandle);
                AutoPtr<IContext> context;
                ECode ec = baseContext->CreatePackageContextAsUser(
                        packageName, 0, uhandle, (IContext**)&context);
                if (FAILED(ec) || context == NULL) {
                    // throw new IllegalArgumentException("No such package " + packageName);
                    SLOGGERE(TAG, String("No such package ") + packageName)
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                return context->GetApplicationInfo(info);
            // } catch (NameNotFoundException nnfe) {
            // }
        }
        return NOERROR;
}

ECode RemoteViews::AddView(
    /* [in] */ Int32 viewId,
    /* [in] */ IRemoteViews* nestedView)
{
    AutoPtr<IRemoteViewsAction> action = new ViewGroupAction(viewId, nestedView, this);
    return AddAction(action);
}

ECode RemoteViews::RemoveAllViews(
    /* [in] */ Int32 viewId)
{
    AutoPtr<IRemoteViewsAction> action = new ViewGroupAction(viewId, NULL, this);
    return AddAction(action);
}

ECode RemoteViews::ShowNext(
    /* [in] */ Int32 viewId)
{
    AutoPtr<IRemoteViewsAction> action = new ReflectionActionWithoutParams(viewId, String("ShowNext"), this);
    return AddAction(action);
}

ECode RemoteViews::ShowPrevious(
    /* [in] */ Int32 viewId)
{
    AutoPtr<IRemoteViewsAction> action = new ReflectionActionWithoutParams(viewId, String("ShowPrevious"), this);
    return AddAction(action);
}

ECode RemoteViews::SetDisplayedChild(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 childIndex)
{
    return SetInt32(viewId, String("SetDisplayedChild"), childIndex);
}

ECode RemoteViews::SetViewVisibility(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 visibility)
{
    return SetInt32(viewId, String("SetVisibility"), visibility);
}

ECode RemoteViews::SetTextViewText(
    /* [in] */ Int32 viewId,
    /* [in] */ ICharSequence* text)
{
    return SetCharSequence(viewId, String("SetText"), text);
}

ECode RemoteViews::SetTextViewTextSize(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 units,
    /* [in] */ Float size)
{
    AutoPtr<IRemoteViewsAction> action = new TextViewSizeAction(viewId, units, size);
    return AddAction(action);
}

ECode RemoteViews::SetTextViewCompoundDrawables(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRemoteViewsAction> action = new TextViewDrawableAction(viewId, FALSE, left, top, right, bottom);
    return AddAction(action);
}

ECode RemoteViews::SetTextViewCompoundDrawablesRelative(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRemoteViewsAction> action = new TextViewDrawableAction(viewId, TRUE, start, top, end, bottom);
    return AddAction(action);
}

ECode RemoteViews::SetTextViewCompoundDrawablesRelativeColorFilter(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 index,
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    if (index < 0 || index >= 4) {
        SLOGGERE(TAG, "index must be in range [0, 3].")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IRemoteViewsAction> action = new TextViewDrawableColorFilterAction(viewId, TRUE, index, color, mode);
    return AddAction(action);
}

ECode RemoteViews::SetImageViewResource(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 srcId)
{
    return SetInt32(viewId, String("SetImageResource"), srcId);
}

ECode RemoteViews::SetImageViewUri(
    /* [in] */ Int32 viewId,
    /* [in] */ IUri* uri)
{
    return SetUri(viewId, String("SetImageURI"), uri);
}

ECode RemoteViews::SetImageViewBitmap(
    /* [in] */ Int32 viewId,
    /* [in] */ IBitmap* bitmap)
{
    return SetBitmap(viewId, String("SetImageBitmap"), bitmap);
}

ECode RemoteViews::SetEmptyView(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 emptyViewId)
{
    AutoPtr<IRemoteViewsAction> action = new _SetEmptyView(viewId, emptyViewId);
    return AddAction(action);
}

ECode RemoteViews::SetChronometer(
    /* [in] */ Int32 viewId,
    /* [in] */ Int64 base,
    /* [in] */ const String& format,
    /* [in] */ Boolean started)
{
    SetInt64(viewId, String("SetBase"), base);
    SetString(viewId, String("SetFormat"), format);
    SetBoolean(viewId, String("SetStarted"), started);
    return NOERROR;
}

ECode RemoteViews::SetProgressBar(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 max,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean indeterminate)
{
    SetBoolean(viewId, String("SetIndeterminate"), indeterminate);
    if (!indeterminate) {
        SetInt32(viewId, String("SetMax"), max);
        SetInt32(viewId, String("SetProgress"), progress);
    }
    return NOERROR;
}

ECode RemoteViews::SetOnClickPendingIntent(
    /* [in] */ Int32 viewId,
    /* [in] */ IPendingIntent* pendingIntent)
{
    AutoPtr<IRemoteViewsAction> action = new _SetOnClickPendingIntent(viewId, pendingIntent, this);
    return AddAction(action);
}

ECode RemoteViews::SetPendingIntentTemplate(
    /* [in] */ Int32 viewId,
    /* [in] */ IPendingIntent* pendingIntentTemplate)
{
    AutoPtr<IRemoteViewsAction> action = new _SetPendingIntentTemplate(viewId, pendingIntentTemplate, this);
    return AddAction(action);
}

ECode RemoteViews::SetOnClickFillInIntent(
    /* [in] */ Int32 viewId,
    /* [in] */ IIntent* fillInIntent)
{
    AutoPtr<IRemoteViewsAction> action = new _SetOnClickFillInIntent(viewId, fillInIntent, this);
    return AddAction(action);
}

ECode RemoteViews::SetDrawableParameters(
    /* [in] */ Int32 viewId,
    /* [in] */ Boolean targetBackground,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 colorFilter,
    /* [in] */ PorterDuffMode mode,
    /* [in] */ Int32 level)
{
    AutoPtr<IRemoteViewsAction> action = new _SetDrawableParameters(viewId, targetBackground, alpha, colorFilter, mode, level);
    return AddAction(action);
}

ECode RemoteViews::SetTextColor(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 color)
{
    return SetInt32(viewId, String("SetTextColor"), color);
}

ECode RemoteViews::SetRemoteAdapter(
    /* [in] */ Int32 viewId,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IRemoteViewsAction> action = new SetRemoteViewsAdapterIntent(viewId, intent);
    return AddAction(action);
}

ECode RemoteViews::SetRemoteAdapter(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId,
    /* [in] */ IIntent* intent)
{
    return SetRemoteAdapter(viewId, intent);
}

ECode RemoteViews::SetRemoteAdapter(
    /* [in] */ Int32 viewId,
    /* [in] */ IArrayList* list,
    /* [in] */ Int32 viewTypeCount)
{
    AutoPtr<IRemoteViewsAction> action = new SetRemoteViewsAdapterList(viewId, list, viewTypeCount);
    return AddAction(action);
}

ECode RemoteViews::SetScrollPosition(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 position)
{
    return SetInt32(viewId, String("SmoothScrollToPosition"), position);
}

ECode RemoteViews::SetRelativeScrollPosition(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 offset)
{
    return SetInt32(viewId, String("SmoothScrollByOffset"), offset);
}

ECode RemoteViews::SetViewPadding(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRemoteViewsAction> action = new ViewPaddingAction(viewId, left, top, right, bottom);
    return AddAction(action);
}

ECode RemoteViews::SetBoolean(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Boolean value)
{
    AutoPtr<IBoolean> pValue;
    CBoolean::New(value, (IBoolean**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::BOOLEAN, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetByte(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Byte value)
{
    AutoPtr<IByte> pValue;
    CByte::New(value, (IByte**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::BYTE, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetInt16(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Int16 value)
{
    AutoPtr<IInteger16> pValue;
    CInteger16::New(value, (IInteger16**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::SHORT, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetInt32(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> pValue;
    CInteger32::New(value, (IInteger32**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::INT, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetInt64(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Int64 value)
{
    AutoPtr<IInteger64> pValue;
    CInteger64::New(value, (IInteger64**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::LONG, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetFloat(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Float value)
{
    AutoPtr<IFloat> pValue;
    CFloat::New(value, (IFloat**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::FLOAT, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetDouble(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Double value)
{
    AutoPtr<IDouble> pValue;
    CDouble::New(value, (IDouble**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::DOUBLE, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetChar(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ Char32 value)
{
    AutoPtr<IChar32> pValue;
    CChar32::New(value, (IChar32**)&pValue);
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::CHAR, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetString(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> pValue = CoreUtils::Convert(value);
    AutoPtr<Action> action = new ReflectionAction(viewId, methodName, ReflectionAction::STRING, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetCharSequence(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ ICharSequence* value)
{
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::CHAR_SEQUENCE, value);

    return AddAction(action);
}

ECode RemoteViews::SetUri(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ IUri* value)
{
    AutoPtr<IUri> pValue = value;
    if (value != NULL)
    {
        value->GetCanonicalUri((IUri**)&pValue);
        // if (StrictMode.vmFileUriExposureEnabled()) {
        //     value.checkFileUriExposed("RemoteViews.setUri()");
        // }
    }
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::URI, pValue);
    return AddAction(action);
}

ECode RemoteViews::SetBitmap(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ IBitmap* value)
{
    AutoPtr<IRemoteViewsAction> action = new BitmapReflectionAction(viewId, methodName, value, this);
    return AddAction(action);
}

ECode RemoteViews::SetBundle(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ IBundle* value)
{
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::BUNDLE, value);
    return AddAction(action);
}

ECode RemoteViews::SetIntent(
    /* [in] */ Int32 viewId,
    /* [in] */ const String& methodName,
    /* [in] */ IIntent* value)
{
    AutoPtr<IRemoteViewsAction> action = new ReflectionAction(viewId, methodName, ReflectionAction::INTENT, value);
    return AddAction(action);
}

ECode RemoteViews::SetContentDescription(
    /* [in] */ Int32 viewId,
    /* [in] */ ICharSequence* contentDescription)
{
    return SetCharSequence(viewId, String("SetContentDescription"), contentDescription);
}

ECode RemoteViews::SetLabelFor(
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 labeledId)
{
    return SetInt32(viewId, String("SetLabelFor"), labeledId);
}

ECode RemoteViews::Apply(
    /* [in] */ IContext* ctx,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return Apply(ctx, parent, NULL, view);
}

/** @hide */
ECode RemoteViews::Apply(
    /* [in] */ IContext* ctx,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IRemoteViewsOnClickHandler* handler,
    /* [out] */ IView** view)
{
    String str;
    return Apply(ctx, parent, handler, str, view);
}

/** @hide */
ECode RemoteViews::Apply(
    /* [in] */ IContext* ctx,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IRemoteViewsOnClickHandler* handler,
    /* [in] */ const String& themePackageName,
    /* [out] */ IView** view)
{
    AutoPtr<IRemoteViews> rvToApply = GetRemoteViewsToApply(ctx);
    // RemoteViews may be built by an application installed in another
    // user. So build a context that loads resources from that user but
    // still returns the current users userId so settings like data / time formats
    // are loaded without requiring cross user persmissions.
    AutoPtr<IView> result;
    AutoPtr<IContext> contextForResources = GetContextForResources(ctx, themePackageName);
    AutoPtr<IContext> inflationContext = new MyContextWrapper(ctx, contextForResources);

    AutoPtr<IInterface> tmp;
    ctx->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmp);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(tmp);

    // Clone inflater so we load resources from correct context and
    // we don't add a filter to the static version returned by getSystemService.
    AutoPtr<ILayoutInflater> newInflater;
    inflater->CloneInContext(inflationContext, (ILayoutInflater**)&newInflater);
    inflater->SetFilter(this);
    Int32 layoutId;
    rvToApply->GetLayoutId(&layoutId);
    inflater->Inflate(layoutId, parent, FALSE, (IView**)&result);

    RemoteViews* impl = (RemoteViews*)rvToApply.Get();
    impl->PerformApply(result, parent, handler);

    *view = result;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode RemoteViews::Reapply(
    /* [in] */ IContext* context,
    /* [in] */ IView* v)
{
    return Reapply(context, v, NULL);
}

ECode RemoteViews::Reapply(
    /* [in] */ IContext* context,
    /* [in] */ IView* v,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    AutoPtr<IRemoteViews> rvToApply = GetRemoteViewsToApply(context);
    // In the case that a view has this RemoteViews applied in one orientation, is persisted
    // across orientation change, and has the RemoteViews re-applied in the new orientation,
    // we throw an exception, since the layouts may be completely unrelated.

    if (HasLandscapeAndPortraitLayouts()) {
        Int32 vId = 0, rvId = 0;
        v->GetId(&vId);
        rvToApply->GetLayoutId(&rvId);
        if (vId != rvId) {
            SLOGGERE(TAG, String("Attempting to re-apply RemoteViews to a view that") +
                   " that does not share the same root layout id.")
            return E_RUNTIME_EXCEPTION;
        }
    }

    AutoPtr<IViewParent> parent;
    v->GetParent((IViewParent**)&parent);
    RemoteViews* remote = (RemoteViews*)rvToApply.Get();
    remote->PerformApply(v, IViewGroup::Probe(parent), handler);

    return NOERROR;
}

ECode RemoteViews::OnLoadClass(
    /* [in] */ IClassInfo* clazz,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
//    return clazz.isAnnotationPresent(RemoteView.class);
    *res = FALSE;
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
