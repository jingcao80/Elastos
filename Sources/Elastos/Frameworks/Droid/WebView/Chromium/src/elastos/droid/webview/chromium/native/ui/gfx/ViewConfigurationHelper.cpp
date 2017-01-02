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

#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/gfx/ViewConfigurationHelper.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/ViewConfigurationHelper_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/R_Ui.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Gfx {

//=====================================================================
//           ViewConfigurationHelper::InnerComponentCallbacks
//=====================================================================
CAR_INTERFACE_IMPL(ViewConfigurationHelper::InnerComponentCallbacks, Object, IComponentCallbacks)

ViewConfigurationHelper::InnerComponentCallbacks::InnerComponentCallbacks(
    /* [in] */ ViewConfigurationHelper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ViewConfigurationHelper::InnerComponentCallbacks::OnConfigurationChanged(
    /* [in] */ IConfiguration* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    // ==================before translated======================
    // updateNativeViewConfigurationIfNecessary();

    assert(NULL != mOwner);
    mOwner->UpdateNativeViewConfigurationIfNecessary();
    return NOERROR;
}

ECode ViewConfigurationHelper::InnerComponentCallbacks::OnLowMemory()
{
    return NOERROR;
}

//=====================================================================
//                       ViewConfigurationHelper
//=====================================================================
const Float ViewConfigurationHelper::MIN_SCALING_SPAN_MM = 27.0f;
const Float ViewConfigurationHelper::MIN_SCALING_TOUCH_MAJOR_DIP = 48.0f;

ViewConfigurationHelper::ViewConfigurationHelper(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mAppContext = context.getApplicationContext();
    // mViewConfiguration = ViewConfiguration.get(mAppContext);

    context->GetApplicationContext((IContext**)&mAppContext);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->Get(mAppContext, (IViewConfiguration**)&mViewConfiguration);
}

ECode ViewConfigurationHelper::RegisterListener()
{
    // ==================before translated======================
    // mAppContext.registerComponentCallbacks(
    //         new ComponentCallbacks() {
    //             @Override
    //             public void onConfigurationChanged(Configuration configuration) {
    //                 updateNativeViewConfigurationIfNecessary();
    //             }
    //
    //             @Override
    //             public void onLowMemory() {
    //             }
    //         });

    AutoPtr<InnerComponentCallbacks> componentCallBacks = new InnerComponentCallbacks(this);
    mAppContext->RegisterComponentCallbacks(componentCallBacks);
    return NOERROR;
}

ECode ViewConfigurationHelper::UpdateNativeViewConfigurationIfNecessary()
{
    // ==================before translated======================
    // // The ViewConfiguration will differ only if the density has changed.
    // ViewConfiguration configuration = ViewConfiguration.get(mAppContext);
    // if (mViewConfiguration == configuration) return;
    //
    // mViewConfiguration = configuration;
    // nativeUpdateSharedViewConfiguration(
    //         getScaledMaximumFlingVelocity(),
    //         getScaledMinimumFlingVelocity(),
    //         getScaledTouchSlop(),
    //         getScaledDoubleTapSlop(),
    //         getScaledMinScalingSpan(),
    //         getScaledMinScalingTouchMajor());


    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    AutoPtr<IViewConfiguration> configuration;
    helper->Get(mAppContext, (IViewConfiguration**)&configuration);

    if (mViewConfiguration == configuration)
        return NOERROR;

    mViewConfiguration = configuration;
    NativeUpdateSharedViewConfiguration(
        GetScaledMaximumFlingVelocity(),
        GetScaledMinimumFlingVelocity(),
        GetScaledTouchSlop(),
        GetScaledDoubleTapSlop(),
        GetScaledMinScalingSpan(),
        GetScaledMinScalingTouchMajor());

    return NOERROR;
}

Int32 ViewConfigurationHelper::GetDoubleTapTimeout()
{
    // ==================before translated======================
    // return ViewConfiguration.getDoubleTapTimeout();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    Int32 result = 0;
    helper->GetDoubleTapTimeout(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetLongPressTimeout()
{
    // ==================before translated======================
    // return ViewConfiguration.getLongPressTimeout();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    Int32 result = 0;
    helper->GetLongPressTimeout(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetTapTimeout()
{
    // ==================before translated======================
    // return ViewConfiguration.getTapTimeout();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    Int32 result = 0;
    helper->GetTapTimeout(&result);
    return result;
}

Float ViewConfigurationHelper::GetScrollFriction()
{
    // ==================before translated======================
    // return ViewConfiguration.getScrollFriction();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    Float result = 0.0f;
    helper->GetScrollFriction(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetScaledMaximumFlingVelocity()
{
    // ==================before translated======================
    // return mViewConfiguration.getScaledMaximumFlingVelocity();

    Int32 result = 0;
    mViewConfiguration->GetScaledMaximumFlingVelocity(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetScaledMinimumFlingVelocity()
{
    // ==================before translated======================
    // return mViewConfiguration.getScaledMinimumFlingVelocity();

    Int32 result = 0;
    mViewConfiguration->GetScaledMinimumFlingVelocity(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetScaledTouchSlop()
{
    // ==================before translated======================
    // return mViewConfiguration.getScaledTouchSlop();

    Int32 result = 0;
    mViewConfiguration->GetScaledTouchSlop(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetScaledDoubleTapSlop()
{
    // ==================before translated======================
    // return mViewConfiguration.getScaledDoubleTapSlop();

    Int32 result = 0;
    mViewConfiguration->GetScaledDoubleTapSlop(&result);
    return result;
}

Int32 ViewConfigurationHelper::GetScaledMinScalingSpan()
{
    // ==================before translated======================
    // final Resources res = mAppContext.getResources();
    // int id = res.getIdentifier("config_minScalingSpan", "dimen", "android");
    // // Fall back to a sensible default if the internal identifier does not exist.
    // if (id == 0) id = R.dimen.config_min_scaling_span;
    // try {
    //     return res.getDimensionPixelSize(id);
    // } catch (Resources.NotFoundException e) {
    //     assert false : "MinScalingSpan resource lookup failed.";
    //     return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, MIN_SCALING_SPAN_MM,
    //             res.getDisplayMetrics());
    // }

    AutoPtr<IResources> res;
    mAppContext->GetResources((IResources**)&res);

    Int32 id = 0;
    res->GetIdentifier(String("config_minScalingSpan"), String("dimen"), String("android"), &id);
    if (id == 0)
        id = R::dimen::config_min_scaling_span;
    //try {
        Int32 dimensionPixelSize = 0;
        res->GetDimensionPixelSize(id, &dimensionPixelSize);
        return dimensionPixelSize;
    //}
    //catch (Resources.NotFoundException e) {
    //    assert false : "MinScalingSpan resource lookup failed.";
    //    return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, MIN_SCALING_SPAN_MM,
    //                res.getDisplayMetrics());
    //}
}

Int32 ViewConfigurationHelper::GetScaledMinScalingTouchMajor()
{
    // ==================before translated======================
    // final Resources res = mAppContext.getResources();
    // int id = res.getIdentifier("config_minScalingTouchMajor", "dimen", "android");
    // // Fall back to a sensible default if the internal identifier does not exist.
    // if (id == 0) id = R.dimen.config_min_scaling_touch_major;
    // try {
    //     return res.getDimensionPixelSize(id);
    // } catch (Resources.NotFoundException e) {
    //     assert false : "MinScalingTouchMajor resource lookup failed.";
    //     return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,
    //             MIN_SCALING_TOUCH_MAJOR_DIP, res.getDisplayMetrics());
    // }

    AutoPtr<IResources> res;
    mAppContext->GetResources((IResources**)&res);

    Int32 id = 0;
    res->GetIdentifier(String("config_minScalingTouchMajor"), String("dimen"), String("android"), &id);
    if (id == 0)
        id = R::dimen::config_min_scaling_touch_major;
    //try {
        Int32 dimensionPixelSize = 0;
        res->GetDimensionPixelSize(id, &dimensionPixelSize);
        return dimensionPixelSize;
    //}
    //catch (Resources.NotFoundException e) {
    //    assert false : "MinScalingTouchMajor resource lookup failed.";
    //    return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,
    //                MIN_SCALING_TOUCH_MAJOR_DIP, res.getDisplayMetrics());
    //}
}

AutoPtr<ViewConfigurationHelper> ViewConfigurationHelper::CreateWithListener(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // ViewConfigurationHelper viewConfigurationHelper = new ViewConfigurationHelper(context);
    // viewConfigurationHelper.registerListener();
    // return viewConfigurationHelper;

    AutoPtr<ViewConfigurationHelper> viewConfigurationHelper = new ViewConfigurationHelper(context);
    viewConfigurationHelper->RegisterListener();
    return viewConfigurationHelper;
}

ECode ViewConfigurationHelper::NativeUpdateSharedViewConfiguration(
    /* [in] */ Int32 scaledMaximumFlingVelocity,
    /* [in] */ Int32 scaledMinimumFlingVelocity,
    /* [in] */ Int32 scaledTouchSlop,
    /* [in] */ Int32 scaledDoubleTapSlop,
    /* [in] */ Int32 scaledMinScalingSpan,
    /* [in] */ Int32 scaledMinScalingTouchMajor)
{
    Elastos_ViewConfigurationHelper_nativeUpdateSharedViewConfiguration(TO_IINTERFACE(this),
            scaledMaximumFlingVelocity, scaledMinimumFlingVelocity, scaledTouchSlop, scaledDoubleTapSlop, scaledMinScalingSpan, scaledMinScalingTouchMajor);
    return NOERROR;
}

Int32 ViewConfigurationHelper::GetScaledMaximumFlingVelocity(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledMaximumFlingVelocity, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledMaximumFlingVelocity();
}

Int32 ViewConfigurationHelper::GetScaledMinimumFlingVelocity(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledMinimumFlingVelocity, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledMinimumFlingVelocity();
}

Int32 ViewConfigurationHelper::GetScaledTouchSlop(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledTouchSlop, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledTouchSlop();
}

Int32 ViewConfigurationHelper::GetScaledDoubleTapSlop(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledDoubleTapSlop, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledDoubleTapSlop();
}

Int32 ViewConfigurationHelper::GetScaledMinScalingSpan(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledMinScalingSpan, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledMinScalingSpan();
}

Int32 ViewConfigurationHelper::GetScaledMinScalingTouchMajor(
    /* [in] */ IInterface* obj)
{
    ViewConfigurationHelper* mObj = (ViewConfigurationHelper*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ViewConfigurationHelper", "ViewConfigurationHelper::GetScaledMinScalingTouchMajor, mObj is NULL");
        return 0;
    }
    return mObj->GetScaledMinScalingTouchMajor();
}

AutoPtr<IInterface> ViewConfigurationHelper::CreateWithListener(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return TO_IINTERFACE(CreateWithListener(c));
}

} // namespace Gfx
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


