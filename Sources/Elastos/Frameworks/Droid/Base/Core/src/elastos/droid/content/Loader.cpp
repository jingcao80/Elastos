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

#include "elastos/droid/content/Loader.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(Loader, Object, ILoader)

Loader::Loader()
    : mContentChanged(FALSE)
    , mId(0)
    , mStarted(FALSE)
    , mAbandoned(FALSE)
    , mReset(TRUE)
    , mProcessingChange(FALSE)
{}

Loader::~Loader()
{}

ECode Loader::constructor(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context)
    FAIL_RETURN(context->GetApplicationContext((IContext**)&mContext));
    return NOERROR;
}

ECode Loader::DeliverResult(
    /* [in] */ IInterface* data)
{
    if (NULL != mListener) {
        FAIL_RETURN(mListener->OnLoadComplete((ILoader*) this, data));
    }
    return NOERROR;
}

ECode Loader::DeliverCancellation()
{
    if (NULL != mOnLoadCanceledListener) {
        FAIL_RETURN(mOnLoadCanceledListener->OnLoadCanceled((ILoader*) this));
    }
    return NOERROR;
}

ECode Loader::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode Loader::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode Loader::RegisterListener(
    /* [in] */ Int32 id,
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    if (NULL != mListener) {
        //throw new IllegalStateException("There is already a listener registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mListener = listener;
    mId = id;
    return NOERROR;
}

ECode Loader::UnregisterListener(
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    if (NULL == mListener) {
        //throw new IllegalStateException("No listener register");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!_CObject_Compare(mListener, listener)) {
        //throw new IllegalArgumentException("Attempting to unregister the wrong listener");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mListener = NULL;
    return NOERROR;
}

ECode Loader::RegisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    if (NULL != mOnLoadCanceledListener) {
        //throw new IllegalStateException("There is already a listener registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mOnLoadCanceledListener = listener;
    return NOERROR;
}

ECode Loader::UnregisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    if (NULL == mOnLoadCanceledListener) {
        //throw new IllegalStateException("No listener register");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!_CObject_Compare(mOnLoadCanceledListener, listener)) {
        //throw new IllegalArgumentException("Attempting to unregister the wrong listener");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mOnLoadCanceledListener = NULL;
    return NOERROR;
}

ECode Loader::IsStarted(
    /* [out] */ Boolean* isStarted)
{
    VALIDATE_NOT_NULL(isStarted)
    *isStarted = mStarted;
    return NOERROR;
}

ECode Loader::IsAbandoned(
    /* [out] */ Boolean* isAbandoned)
{
    VALIDATE_NOT_NULL(isAbandoned)
    *isAbandoned = mAbandoned;
    return NOERROR;
}

ECode Loader::IsReset(
    /* [out] */ Boolean* isReset)
{
    VALIDATE_NOT_NULL(isReset)
    *isReset = mReset;
    return NOERROR;
}

ECode Loader::StartLoading()
{
    mStarted = TRUE;
    mReset = FALSE;
    mAbandoned = FALSE;
    FAIL_RETURN(OnStartLoading());
    return NOERROR;
}

ECode Loader::CancelLoad(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    FAIL_RETURN(OnCancelLoad(succeeded));
    return NOERROR;
}

ECode Loader::ForceLoad()
{
    FAIL_RETURN(OnForceLoad());
    return NOERROR;
}

ECode Loader::StopLoading()
{
    mStarted = FALSE;
    FAIL_RETURN(OnStopLoading());
    return NOERROR;
}

ECode Loader::Abandon()
{
    mAbandoned = TRUE;
    FAIL_RETURN(OnAbandon());
    return NOERROR;
}

ECode Loader::Reset()
{
    FAIL_RETURN(OnReset());
    mReset = TRUE;
    mStarted = FALSE;
    mAbandoned = FALSE;
    mContentChanged = FALSE;
    mProcessingChange = FALSE;
    return NOERROR;
}

ECode Loader::TakeContentChanged(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = mContentChanged;
    mContentChanged = FALSE;
    mProcessingChange |= *succeeded;
    return NOERROR;
}

ECode Loader::CommitContentChanged()
{
    mProcessingChange = FALSE;
    return NOERROR;
}

ECode Loader::RollbackContentChanged()
{
    if (mProcessingChange) {
        mContentChanged = TRUE;
    }
    return NOERROR;
}

ECode Loader::OnContentChanged()
{
    if (mStarted) {
        FAIL_RETURN(ForceLoad());
    }
    else {
        // This loader has been stopped, so we don't want to load
        // new data right now...  but keep track of it changing to
        // refresh later if we start again.
        mContentChanged = TRUE;
    }

    return NOERROR;
}

ECode Loader::DataToString(
    /* [in] */ IInterface* data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(64);
//***    DebugUtils::BuildShortClassTag(data, sb);
    sb.Append("}");
    return sb.ToString(str);
}

ECode Loader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(64);
//***    DebugUtils::BuildShortClassTag(this, sb);
    sb.Append(" id=");
    sb.Append(mId);
    sb.Append("}");
    return sb.ToString(str);
}

ECode Loader::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // if (mStarted || mContentChanged || mProcessingChange) {
    //     writer.print(prefix); writer.print("mStarted="); writer.print(mStarted);
    //             writer.print(" mContentChanged="); writer.print(mContentChanged);
    //             writer.print(" mProcessingChange="); writer.println(mProcessingChange);
    // }
    // if (mAbandoned || mReset) {
    //     writer.print(prefix); writer.print("mAbandoned="); writer.print(mAbandoned);
    //             writer.print(" mReset="); writer.println(mReset);
    // }
    return NOERROR;
}

ECode Loader::OnStartLoading()
{
    return NOERROR;
}

ECode Loader::OnCancelLoad(
    /* [out] */ Boolean* isCanceled)
{
    return NOERROR;
}

ECode Loader::OnForceLoad()
{
    return NOERROR;
}

ECode Loader::OnStopLoading()
{
    return NOERROR;
}

ECode Loader::OnAbandon()
{
    return NOERROR;
}

ECode Loader::OnReset()
{
    return NOERROR;
}

}
}
}
