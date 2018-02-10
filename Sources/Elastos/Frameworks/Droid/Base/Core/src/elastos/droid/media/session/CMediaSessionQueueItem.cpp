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

#include "elastos/droid/media/session/CMediaSessionQueueItem.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL_2(CMediaSessionQueueItem, Object, IMediaSessionQueueItem, IParcelable)

CAR_OBJECT_IMPL(CMediaSessionQueueItem)

CMediaSessionQueueItem::CMediaSessionQueueItem()
    : mId(0)
{
}

CMediaSessionQueueItem::~CMediaSessionQueueItem()
{
}

ECode CMediaSessionQueueItem::constructor()
{
    return NOERROR;
}

ECode CMediaSessionQueueItem::constructor(
    /* [in] */ IMediaDescription * description,
    /* [in] */ Int64 id)
{
    if (description == NULL) {
        // throw new IllegalArgumentException("Description cannot be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (id == UNKNOWN_ID) {
        // throw new IllegalArgumentException("Id cannot be QueueItem.UNKNOWN_ID");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDescription = description;
    mId = id;
    return NOERROR;
}

ECode CMediaSessionQueueItem::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mId);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mDescription = IMediaDescription::Probe(obj);
    return NOERROR;
}

ECode CMediaSessionQueueItem::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mId);
    dest->WriteInterfacePtr(mDescription);
    return NOERROR;
}

/**
 * Get the description for this item.
 */
ECode CMediaSessionQueueItem::GetDescription(
    /* [out] */ IMediaDescription ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDescription;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/**
 * Get the queue id for this item.
 */
ECode CMediaSessionQueueItem::GetQueueId(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CMediaSessionQueueItem::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    // *result = String("MediaSession.QueueItem {") +
    //         "Description=" + mDescription +
    //         ", Id=" + mId + " }";
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
