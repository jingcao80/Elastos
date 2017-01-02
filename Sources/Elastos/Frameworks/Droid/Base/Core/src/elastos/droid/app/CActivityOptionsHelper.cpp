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

#include "elastos/droid/app/CActivityOptionsHelper.h"
#include "elastos/droid/app/CActivityOptions.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CActivityOptionsHelper, Singleton, IActivityOptionsHelper)

CAR_SINGLETON_IMPL(CActivityOptionsHelper)

ECode CActivityOptionsHelper::MakeCustomAnimation(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 enterResId,
    /* [in] */ Int32 exitResId,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeCustomAnimation(
        ctx, enterResId, exitResId, options);
}

ECode CActivityOptionsHelper::MakeCustomAnimation(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 enterResId,
    /* [in] */ Int32 exitResId,
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeCustomAnimation(
        ctx, enterResId, exitResId, handler, listener, options);
}

ECode CActivityOptionsHelper::MakeScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 startWidth,
    /* [in] */ Int32 startHeight,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeScaleUpAnimation(
        source, startX, startY, startWidth, startHeight, options);
}

ECode CActivityOptionsHelper::MakeThumbnailScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeThumbnailScaleUpAnimation(
        source, thumbnail, startX, startY, options);
}

ECode CActivityOptionsHelper::MakeThumbnailScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeThumbnailScaleUpAnimation(
        source, thumbnail, startX, startY, listener, options);
}

ECode CActivityOptionsHelper::MakeThumbnailScaleDownAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeThumbnailScaleDownAnimation(
        source, thumbnail, startX, startY, listener, options);
}

ECode CActivityOptionsHelper::MakeThumbnailAspectScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeThumbnailAspectScaleUpAnimation(
        source, thumbnail, startX, startY, targetWidth, targetHeight, listener, options);
}

ECode CActivityOptionsHelper::MakeThumbnailAspectScaleDownAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeThumbnailAspectScaleDownAnimation(
        source, thumbnail, startX, startY, targetWidth, targetHeight, listener, options);
}

ECode CActivityOptionsHelper::MakeSceneTransitionAnimation(
    /* [in] */ IActivity* activity,
    /* [in] */ IView* sharedElement,
    /* [in] */ const String& sharedElementName,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeSceneTransitionAnimation(
        activity, sharedElement, sharedElementName, options);
}

ECode CActivityOptionsHelper::MakeSceneTransitionAnimation(
    /* [in] */ IActivity* activity,
    /* [in] */ ArrayOf<IPair*>* sharedElements,
    /* [out] */ IActivityOptions** options)
{
    return CActivityOptions::MakeSceneTransitionAnimation(activity, sharedElements, options);
}

ECode CActivityOptionsHelper::MakeTaskLaunchBehind(
    /* [in] */ IActivityOptions** options)
{
    return CActivityOptions::MakeTaskLaunchBehind(options);
}

ECode CActivityOptionsHelper::Abort(
    /* [in] */ IBundle* options)
{
    return CActivityOptions::Abort(options);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
