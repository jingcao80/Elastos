//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/appwidget/AppWidgetHostView.h"
#include "elastos/droid/appwidget/AppWidgetManager.h"
#include "elastos/droid/appwidget/CParcelableSparseArray.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_ILayoutInflaterFilter;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IRemoteAdapterConnectionCallback;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace AppWidget {

const String AppWidgetHostView::TAG("AppWidgetHostView");
const Boolean AppWidgetHostView::LOGD = FALSE;
const Boolean AppWidgetHostView::CROSSFADE = FALSE;
const Int32 AppWidgetHostView::VIEW_MODE_NOINIT;
const Int32 AppWidgetHostView::VIEW_MODE_CONTENT;
const Int32 AppWidgetHostView::VIEW_MODE_ERROR;
const Int32 AppWidgetHostView::VIEW_MODE_DEFAULT;
const Int32 AppWidgetHostView::FADE_DURATION;
AutoPtr<ILayoutInflaterFilter> AppWidgetHostView::sInflaterFilter = new AppWidgetHostView::MyLayoutInflaterFilter();

//==============================================================================
//          AppWidgetHostView::MyLayoutInflaterFilter
//==============================================================================

CAR_INTERFACE_IMPL(AppWidgetHostView::MyLayoutInflaterFilter, Object, ILayoutInflaterFilter);

ECode AppWidgetHostView::MyLayoutInflaterFilter::OnLoadClass(
    /* [in] */ IClassInfo* clazz,
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    // assert(0 && "TODO");

    *allowed = TRUE;
    //return clazz.isAnnotationPresent(RemoteViews.RemoteView.class);
    return NOERROR;
}

//==============================================================================
//          AppWidgetHostView::ParcelableSparseArray
//==============================================================================

CAR_INTERFACE_IMPL(AppWidgetHostView::ParcelableSparseArray, SparseArray, IParcelable);

ECode AppWidgetHostView::ParcelableSparseArray::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    Int32 count;
    GetSize(&count);
    dest->WriteInt32(count);
    for (Int32 i = 0; i < count; i++) {
        Int32 key;
        KeyAt(i, &key);
        dest->WriteInt32(key);
        AutoPtr<IInterface> obj;
        ValueAt(i, (IInterface**)&obj);
        dest->WriteInterfacePtr(obj);
    }
    return NOERROR;
}

ECode AppWidgetHostView::ParcelableSparseArray::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 count;
    source->ReadInt32(&count);
    for (Int32 i = 0; i < count; i++) {
        Int32 key;
        source->ReadInt32(&key);
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        AutoPtr<IParcelable> parcelable = IParcelable::Probe(obj);
        Put(key, parcelable);
    }
    return NOERROR;
}

//==============================================================================
//          AppWidgetHostView
//==============================================================================

CAR_INTERFACE_IMPL(AppWidgetHostView, FrameLayout, IAppWidgetHostView);

AppWidgetHostView::AppWidgetHostView()
    : mRemoteContext(NULL)
    , mAppWidgetId(0)
    , mViewMode(VIEW_MODE_NOINIT)
    , mLayoutId(-1)
    , mFadeStartTime(-1)
{
    CPaint::New((IPaint**)&mOldPaint);
}

AppWidgetHostView::~AppWidgetHostView()
{}

ECode AppWidgetHostView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, R::anim::fade_in, R::anim::fade_out);
}

ECode AppWidgetHostView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    ECode ec = constructor(context, R::anim::fade_in, R::anim::fade_out);
    mOnClickHandler = handler;
    return ec;
}

ECode AppWidgetHostView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 animationIn,
    /* [in] */ Int32 animationOut)
{
    FrameLayout::constructor(context);
    // We want to segregate the view ids within AppWidgets to prevent
    // problems when those ids collide with view ids in the AppWidgetHost.
    SetIsRootNamespace(TRUE);
    return NOERROR;
}

ECode AppWidgetHostView::SetOnClickHandler(
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    mOnClickHandler = handler;
    return NOERROR;
}

ECode AppWidgetHostView::SetAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* info)
{
    mAppWidgetId = appWidgetId;
    mInfo = info;

    // Sometimes the AppWidgetManager returns a null AppWidgetProviderInfo object for
    // a widget, eg. for some widgets in safe mode.
    if (info != NULL) {
        // We add padding to the AppWidgetHostView if necessary
        AutoPtr<IComponentName> provider;
        info->GetProvider((IComponentName**)&provider);
        AutoPtr<IRect> padding = GetDefaultPaddingForWidget(mContext, provider, NULL);
        Int32 left, top, right, bottom;
        padding->Get(&left, &top, &right, &bottom);
        SetPadding(left, top, right, bottom);
        String labelStr;
        info->GetLabel(&labelStr);
        AutoPtr<ICharSequence> label;
        CString::New(labelStr, (ICharSequence**)&label);
        SetContentDescription(label);
    }
    return NOERROR;
}

AutoPtr<IRect> AppWidgetHostView::GetDefaultPaddingForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* component,
    /* [in] */ IRect* padding)
{
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IApplicationInfo> appInfo;

    if (padding == NULL) {
        CRect::New(0, 0, 0, 0, (IRect**)&padding);
    }
    else {
        padding->Set(0, 0, 0, 0);
    }

    //try {
    String name;
    component->GetPackageName(&name);
    ECode ec = packageManager->GetApplicationInfo(name, 0, (IApplicationInfo**)&appInfo);
    if (FAILED(ec)) {
        return padding;
    }
    //} catch (NameNotFoundException e) {
        // if we can't find the package, return 0 padding
    //    return padding;
    //}

    Int32 targetSdkVersion = 0;
    assert(appInfo != NULL);
    appInfo->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion >= Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        Int32 left, top, right, bottom;
        r->GetDimensionPixelSize(Elastos::Droid::R::dimen::default_app_widget_padding_left, &left);
        r->GetDimensionPixelSize(Elastos::Droid::R::dimen::default_app_widget_padding_right, &right);
        r->GetDimensionPixelSize(Elastos::Droid::R::dimen::default_app_widget_padding_top, &top);
        r->GetDimensionPixelSize(Elastos::Droid::R::dimen::default_app_widget_padding_bottom, &bottom);
        padding->Set(left, top, right, bottom);
    }
    return padding;
}

ECode AppWidgetHostView::GetAppWidgetId(
    /* [out] */ Int32* appWidgetId)
{
    VALIDATE_NOT_NULL(appWidgetId);
    *appWidgetId = mAppWidgetId;
    return NOERROR;
}

ECode AppWidgetHostView::GetAppWidgetInfo(
    /* [out] */ IAppWidgetProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode AppWidgetHostView::DispatchSaveInstanceState(
    /* [in] */ ISparseArray* container)
{
    AutoPtr<ISparseArray> jail;
    CParcelableSparseArray::New((ISparseArray**)&jail);
    FrameLayout::DispatchSaveInstanceState(jail);
    container->Put(GenerateId(), jail);

    return NOERROR;
}

Int32 AppWidgetHostView::GenerateId()
{
    Int32 id;
    GetId(&id);
    return id == IView::NO_ID ? mAppWidgetId : id;
}

ECode AppWidgetHostView::DispatchRestoreInstanceState(
    /* [in] */ ISparseArray* container)
{
    AutoPtr<IInterface> obj;
    container->Get(GenerateId(), (IInterface**)&obj);
    AutoPtr<IParcelable> parcelable = IParcelable::Probe(obj);

    AutoPtr<ISparseArray> jail;
    if (parcelable != NULL && ISparseArray::Probe(parcelable) != NULL) {
        jail = ISparseArray::Probe(parcelable);
    }

    if (jail == NULL) {
        CParcelableSparseArray::New((ISparseArray**)&jail);
    }

    // try  {
    ECode ec = FrameLayout::DispatchRestoreInstanceState(ISparseArray::Probe(jail));
    if (FAILED(ec)) {
        if (mInfo == NULL) {
            Logger::E(TAG, "failed to restoreInstanceState for widget id: %d, mInfo: null", mAppWidgetId);
        }
        else {
            AutoPtr<IComponentName> provider;
            mInfo->GetProvider((IComponentName**)&provider);
            Logger::E(TAG, "failed to restoreInstanceState for widget id: %d, mInfo.provider: %p, ec=%08x",
                    mAppWidgetId, provider.Get(), ec);
        }
    }
   // } catch (Exception e) {
   //     Log.e(TAG, "failed to restoreInstanceState for widget id: " + mAppWidgetId + ", "
   //             + (mInfo == null ? "null" : mInfo.provider), e);
   // }

    return NOERROR;
}

ECode AppWidgetHostView::UpdateAppWidgetSize(
    /* [in] */ IBundle* newOptions,
    /* [in] */ Int32 minWidth,
    /* [in] */ Int32 minHeight,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight)
{
    return UpdateAppWidgetSize(newOptions, minWidth, minHeight, maxWidth, maxHeight, FALSE);
}

ECode AppWidgetHostView::UpdateAppWidgetSize(
    /* [in] */ IBundle* _newOptions,
    /* [in] */ Int32 minWidth,
    /* [in] */ Int32 minHeight,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight,
    /* [in] */ Boolean ignorePadding)
{
    AutoPtr<IBundle> newOptions = _newOptions;
    if (newOptions == NULL) {
        CBundle::New((IBundle**)&newOptions);
    }

    AutoPtr<IRect> padding;
    CRect::New((IRect**)&padding);
    if (mInfo != NULL) {
        AutoPtr<IComponentName> provider;
        mInfo->GetProvider((IComponentName**)&provider);
        padding = GetDefaultPaddingForWidget(mContext, provider, padding);
    }

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);

    Int32 left, top, right, bottom;
    padding->Get(&left, &top, &right, &bottom);
    Int32 xPaddingDips = (Int32) ((left + right) / density);
    Int32 yPaddingDips = (Int32) ((top + bottom) / density);

    Int32 newMinWidth = minWidth - (ignorePadding ? 0 : xPaddingDips);
    Int32 newMinHeight = minHeight - (ignorePadding ? 0 : yPaddingDips);
    Int32 newMaxWidth = maxWidth - (ignorePadding ? 0 : xPaddingDips);
    Int32 newMaxHeight = maxHeight - (ignorePadding ? 0 : yPaddingDips);

    AutoPtr<IAppWidgetManager> widgetManager = AppWidgetManager::GetInstance(mContext);

    // We get the old options to see if the sizes have changed
    AutoPtr<IBundle> oldOptions;
    widgetManager->GetAppWidgetOptions(mAppWidgetId, (IBundle**)&oldOptions);
    Boolean needsUpdate = FALSE;
    Int32 value = 0;
    if (newMinWidth != (oldOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH, &value), value) ||
            newMinHeight != (oldOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT, &value), value) ||
            newMaxWidth != (oldOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH, &value), value) ||
            newMaxHeight != (oldOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT, &value), value)) {
        needsUpdate = TRUE;
    }

    if (needsUpdate) {
        newOptions->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH, newMinWidth);
        newOptions->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT, newMinHeight);
        newOptions->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH, newMaxWidth);
        newOptions->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT, newMaxHeight);
        UpdateAppWidgetOptions(newOptions);
    }
    return NOERROR;
}

ECode AppWidgetHostView::UpdateAppWidgetOptions(
    /* [in] */ IBundle* options)
{
    AutoPtr<IAppWidgetManager> widgetManager = AppWidgetManager::GetInstance(mContext);
    return widgetManager->UpdateAppWidgetOptions(mAppWidgetId, options);
}

ECode AppWidgetHostView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    // We're being asked to inflate parameters, probably by a LayoutInflater
    // in a remote Context. To help resolve any remote references, we
    // inflate through our last mRemoteContext when it exists.
    AutoPtr<IContext> context = mRemoteContext != NULL ? mRemoteContext.Get() : mContext;
    AutoPtr<IFrameLayoutLayoutParams> lp;
    FAIL_RETURN(CFrameLayoutLayoutParams::New(context, attrs, (IFrameLayoutLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode AppWidgetHostView::ResetAppWidget(
    /* [in] */ IAppWidgetProviderInfo* info)
{
    mInfo = info;
    mViewMode = VIEW_MODE_NOINIT;
    return UpdateAppWidget(NULL);
}

ECode AppWidgetHostView::UpdateAppWidget(
    /* [in] */ IRemoteViews* remoteViews)
{
    if (LOGD) {
        Logger::D(TAG, "updateAppWidget called mOld= %p" ,mOld.Get());
    }

    Boolean recycled = FALSE;
    AutoPtr<IView> content;

    // Capture the old view into a bitmap so we can do the crossfade.
    if (CROSSFADE) {
        if (mFadeStartTime < 0) {
            if (mView != NULL) {
                Int32 width, height;
                mView->GetWidth(&width);
                mView->GetHeight(&height);
                //try {
                mOld = NULL;
                ECode ec = CBitmap::CreateBitmap(width, height,
                        BitmapConfig_ARGB_8888, (IBitmap**)&mOld);
                if (FAILED(ec)) {
                    mOld = NULL;
                }
                // } catch (OutOfMemoryError e) {
                //     // we just won't do the fade
                //     mOld = null;
                // }
                if (mOld != NULL) {
                    //mView.drawIntoBitmap(mOld);
                }
            }
        }
    }

    // old mRemoteContext can't be destruct before old mView
    AutoPtr<IContext> oldRemoteContext = mRemoteContext;
    if (remoteViews == NULL) {
        if (mViewMode == VIEW_MODE_DEFAULT) {
            // We've already done this -- nothing to do.
            return NOERROR;
        }
        content = GetDefaultView();
        mLayoutId = -1;
        mViewMode = VIEW_MODE_DEFAULT;
    }
    else {
        // Prepare a local reference to the remote Context so we're ready to
        // inflate any requested LayoutParams.
        mRemoteContext = GetRemoteContext();
        Int32 layoutId = 0;
        remoteViews->GetLayoutId(&layoutId);

        // If our stale view has been prepared to match active, and the new
        // layout matches, try recycling it
        if (content == NULL && layoutId == mLayoutId) {
            //try {
            remoteViews->Reapply(mContext, mView, mOnClickHandler);
            content = mView;
            recycled = TRUE;
            if (LOGD) {
                Logger::D(TAG, "was able to recycled existing layout");
            }
            // } catch (RuntimeException e) {
            //     exception = e;
            // }
        }

        // Try normal RemoteView inflation
        if (content == NULL) {
            //try {
            remoteViews->Apply(mContext, this,
                mOnClickHandler, (IView**)&content);
                if (LOGD) {
                    Logger::D(TAG, "had to inflate new layout");
                }
            // } catch (RuntimeException e) {
            //     exception = e;
            // }
        }

        mLayoutId = layoutId;
        mViewMode = VIEW_MODE_CONTENT;
    }

    if (content == NULL) {
        if (mViewMode == VIEW_MODE_ERROR) {
            // We've already done this -- nothing to do.
            return NOERROR;
        }
        Logger::W(TAG, "updateAppWidget couldn't find any view, using error view");
        content = GetErrorView();
        mViewMode = VIEW_MODE_ERROR;
    }

    if (!recycled) {
        PrepareView(content);
        AddView(content);
    }

    if (mView != content) {
        RemoveView(mView);
        mView = content;
    }

    if (CROSSFADE) {
        if (mFadeStartTime < 0) {
            // if there is already an animation in progress, don't do anything --
            // the new view will pop in on top of the old one during the cross fade,
            // and that looks okay.
            mFadeStartTime = SystemClock::GetUptimeMillis();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode AppWidgetHostView::ViewDataChanged(
    /* [in] */ Int32 viewId)
{
    AutoPtr<IView> v;
    FindViewById(viewId, (IView**)&v);
    if ((v != NULL) && IAdapterView::Probe(v) != NULL) {
        AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(v);
        AutoPtr<IAdapter> adapter;
        adapterView->GetAdapter((IAdapter**)&adapter);
        if (IBaseAdapter::Probe(adapter) != NULL) {
            AutoPtr<IBaseAdapter> baseAdapter = IBaseAdapter::Probe(adapter);
            baseAdapter->NotifyDataSetChanged();
        }
        else if (adapter == NULL && IRemoteAdapterConnectionCallback::Probe(adapterView) != NULL) {
            // If the adapter is null, it may mean that the RemoteViewsAapter has not yet
            // connected to its associated service, and hence the adapter hasn't been set.
            // In this case, we need to defer the notify call until it has been set.
            IRemoteAdapterConnectionCallback::Probe(adapterView)->DeferNotifyDataSetChanged();
        }
    }
    return NOERROR;
}

AutoPtr<IContext> AppWidgetHostView::GetRemoteContext()
{
    //try {
        // Return if cloned successfully, otherwise default
    AutoPtr<IActivityInfo> providerInfo;
    mInfo->GetProviderInfo((IActivityInfo**)&providerInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(providerInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    AutoPtr<IContext> context;
    ECode ec = mContext->CreateApplicationContext(applicationInfo,
            IContext::CONTEXT_RESTRICTED, (IContext**)&context);
    if (FAILED(ec)) {
        String packageName;
        IPackageItemInfo::Probe(providerInfo)->GetPackageName(&packageName);
        Logger::E(TAG, "Package name: %s not found", packageName.string());
        return mContext;
    }
    // } catch (NameNotFoundException e) {
    //     Log.e(TAG, "Package name " + mInfo.providerInfo.packageName + " not found");
    //     return mContext;
    // }
    return context;
}

Boolean AppWidgetHostView::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    if (CROSSFADE) {
        Int32 alpha;
        Int32 l, t;
        child->GetLeft(&l);
        child->GetTop(&t);
        if (mFadeStartTime > 0) {
            alpha = (Int32)(((drawingTime - mFadeStartTime) * 255) / FADE_DURATION);
            if (alpha > 255) {
                alpha = 255;
            }
            Int32 w;
            child->GetWidth(&w);
            Logger::D(TAG, "drawChild alpha= %d, l= %d, t= %d, w= %d", alpha, l, t, w);
            if (alpha != 255 && mOld != NULL) {
                mOldPaint->SetAlpha(255 - alpha);
                //canvas.drawBitmap(mOld, l, t, mOldPaint);
            }
        }
        else {
            alpha = 255;
        }
        Int32 restoreTo;
        Int32 w, h;
        child->GetWidth(&w);
        child->GetHeight(&h);
        canvas->SaveLayerAlpha(l, t, w, h, alpha,
                ICanvas::HAS_ALPHA_LAYER_SAVE_FLAG | ICanvas::CLIP_TO_LAYER_SAVE_FLAG,
                &restoreTo);
        Boolean rv = FrameLayout::DrawChild(canvas, child, drawingTime);
        canvas->RestoreToCount(restoreTo);
        if (alpha < 255) {
            Invalidate();
        }
        else {
            mFadeStartTime = -1;
            if (mOld != NULL) {
                mOld->Recycle();
                mOld = NULL;
            }
        }
        return rv;
    }
    else {
        return FrameLayout::DrawChild(canvas, child, drawingTime);
    }
}

void AppWidgetHostView::PrepareView(
    /* [in] */ IView* view)
{
    // Take requested dimensions from child, but apply default gravity.
    AutoPtr<IViewGroupLayoutParams> vglp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    AutoPtr<IFrameLayoutLayoutParams> requested = IFrameLayoutLayoutParams::Probe(vglp);
    if (requested == NULL) {
        CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::MATCH_PARENT, (IFrameLayoutLayoutParams**)&requested);
    }

    requested->SetGravity(IGravity::CENTER);
    view->SetLayoutParams(IViewGroupLayoutParams::Probe(requested));
}

AutoPtr<IView> AppWidgetHostView::GetDefaultView()
{
    if (LOGD) {
        Logger::D(TAG, "getDefaultView");
    }
    AutoPtr<IView> defaultView;

    //try {
    if (mInfo != NULL) {
        AutoPtr<IContext> theirContext = GetRemoteContext();
        mRemoteContext = theirContext;
        AutoPtr<IInterface> service;
        theirContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
        AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
        AutoPtr<ILayoutInflater> theirInflater;
        inflater->CloneInContext(theirContext, (ILayoutInflater**)&theirInflater);
        inflater = theirInflater;
        inflater->SetFilter(sInflaterFilter);
        AutoPtr<IAppWidgetManager> widgetManager = AppWidgetManager::GetInstance(mContext);
        AutoPtr<IBundle> options;
        widgetManager->GetAppWidgetOptions(mAppWidgetId, (IBundle**)&options);

        Int32 layoutId;
        mInfo->GetInitialLayout(&layoutId);
        Boolean b = FALSE;
        options->ContainsKey(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &b);
        if (b) {
            Int32 category = 0;
            options->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &category);
            if (category == IAppWidgetProviderInfo::WIDGET_CATEGORY_KEYGUARD) {
                Int32 kgLayoutId;
                mInfo->GetInitialKeyguardLayout(&kgLayoutId);
                // If a default keyguard layout is not specified, use the standard
                // default layout.
                layoutId = kgLayoutId == 0 ? layoutId : kgLayoutId;
            }
        }
        inflater->Inflate(layoutId, this, FALSE, (IView**)&defaultView);
    }
    else {
        Logger::W(TAG, "can't inflate defaultView because mInfo is missing");
    }
    // } catch (RuntimeException e) {
    //     exception = e;
    // }

    // if (exception != null) {
    //     Log.w(TAG, "Error inflating AppWidget " + mInfo + ": " + exception.toString());
    // }

    if (defaultView == NULL) {
        if (LOGD) {
            Logger::D(TAG, "getDefaultView couldn't find any view, so inflating error");
        }
        defaultView = GetErrorView();
    }

    return defaultView;
}

AutoPtr<IView> AppWidgetHostView::GetErrorView()
{
    AutoPtr<ITextView> tv;
    CTextView::New(mContext, (ITextView**)&tv);
    tv->SetText(R::string::gadget_host_error_inflating);
    // TODO: get this color from somewhere.
    AutoPtr<IView> view = IView::Probe(tv);
    view->SetBackgroundColor(Color::Argb(127, 0, 0, 0));
    return view;
}

ECode AppWidgetHostView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CAppWidgetHostView"), (ICharSequence**)&seq);
    info->SetClassName(seq);
    return NOERROR;
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
