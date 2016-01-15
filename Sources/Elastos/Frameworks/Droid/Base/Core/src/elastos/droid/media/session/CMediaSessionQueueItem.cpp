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
    source->ReadInterfacePtr((Handle32*)&obj);
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
