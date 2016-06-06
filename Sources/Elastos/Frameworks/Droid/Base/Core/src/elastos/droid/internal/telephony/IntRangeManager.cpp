
#include "elastos/droid/internal/telephony/IntRangeManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 IntRangeManager::INITIAL_CLIENTS_ARRAY_SIZE = 4;

IntRangeManager::IntRange::IntRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ const String& client)
    : mStartId(0)
    , mEndId(0)
{
    mStartId = startId;
    mEndId = endId;
    CArrayList::New(INITIAL_CLIENTS_ARRAY_SIZE, (IArrayList**)&mClients);
    AutoPtr<ClientRange> cr = new ClientRange(startId, endId, client);
    mClients->Add(cr->Probe(EIID_IInterface));
}

IntRangeManager::IntRange::IntRange(
    /* [in] */ ClientRange* clientRange)
    : mStartId(0)
    , mEndId(0)
{
    mStartId = clientRange->mStartId;
    mEndId = clientRange->mEndId;
    CArrayList::New(INITIAL_CLIENTS_ARRAY_SIZE, (IArrayList**)&mClients);
    mClients->Add(clientRange->Probe(EIID_IInterface));
}

IntRangeManager::IntRange::IntRange(
    /* [in] */ IntRange* intRange,
    /* [in] */ Int32 numElements)
    : mStartId(0)
    , mEndId(0)
{
    mStartId = intRange->mStartId;
    mEndId = intRange->mEndId;
    Int32 size = 0;
    intRange->mClients->GetSize(&size);
    CArrayList::New(size, (IArrayList**)&mClients);
    for (Int32 i = 0; i < numElements; i++) {
        AutoPtr<IInterface> obj;
        intRange->mClients->Get(i, (IInterface**)&obj);
        mClients->Add(obj);
    }
}

void IntRangeManager::IntRange::Insert(
    /* [in] */ ClientRange* range)
{
    Int32 len = 0;
    mClients->GetSize(&len);
    Int32 insert = -1;
    for (Int32 i=0; i < len; i++) {
        AutoPtr<IInterface> obj;
        mClients->Get(i, (IInterface**)&obj);
        AutoPtr<ClientRange> nextRange = (ClientRange*)IClientRange::Probe(obj);
        if (range->mStartId <= nextRange->mStartId) {
            // ignore duplicate ranges from the same client
            Boolean equal = FALSE;
            if (range->Equals(nextRange->Probe(EIID_IInterface), &equal), !equal) {
                // check if same startId, then order by endId
                if (range->mStartId == nextRange->mStartId
                        && range->mEndId > nextRange->mEndId) {
                    insert = i + 1;
                    if (insert < len) {
                        // there may be more client following with same startId
                        // new [1, 5] existing [1, 2] [1, 4] [1, 7]
                        continue;
                    }
                    break;
                }
                mClients->Add(i, range->Probe(EIID_IInterface));
            }
            return;
        }
    }
    if (insert != -1 && insert < len) {
        mClients->Add(insert, range->Probe(EIID_IInterface));
        return;
    }
    mClients->Add(range->Probe(EIID_IInterface));    // append to end of list
}

CAR_INTERFACE_IMPL(IntRangeManager::ClientRange, Object, IClientRange)
IntRangeManager::ClientRange::ClientRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ const String& client)
    : mStartId(startId)
    , mEndId(endId)
{
    mClient = client;
}

ECode IntRangeManager::ClientRange::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (o != NULL && IClientRange::Probe(o)) {
        ClientRange* other = (ClientRange*)IClientRange::Probe(o);
        *result = mStartId == other->mStartId &&
                mEndId == other->mEndId &&
                mClient.Equals(other->mClient);
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode IntRangeManager::ClientRange::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = (mStartId * 31 + mEndId) * 31 + mClient.GetHashCode();
    return NOERROR;
}

CAR_INTERFACE_IMPL(IntRangeManager, Object, IIntRangeManager)
IntRangeManager::IntRangeManager()
{
    CArrayList::New((IArrayList**)&mRanges);
}

/**
 * Enable a range for the specified client and update ranges
 * if necessary. If {@link #finishUpdate} returns failure,
 * FALSE is returned and the range is not added.
 *
 * @param startId the first id included in the range
 * @param endId the last id included in the range
 * @param client the client requesting the enabled range
 * @return TRUE if successful, FALSE otherwise
 */
ECode IntRangeManager::EnableRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ const String& client,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);

    Int32 len = 0;
    mRanges->GetSize(&len);

    // empty range list: add the initial IntRange
    if (len == 0) {
        if (TryAddRanges(startId, endId, TRUE)) {
            AutoPtr<IntRange> _ir = new IntRange(startId, endId, client);
            mRanges->Add(_ir->Probe(EIID_IInterface));
            *result = TRUE;
            return NOERROR;
        }
        else {
            *result = FALSE;   // failed to update radio
            return NOERROR;
        }
    }

    for (Int32 startIndex = 0; startIndex < len; startIndex++) {
        AutoPtr<IInterface> obj;
        mRanges->Get(startIndex, (IInterface**)&obj);
        IntRange* range = (IntRange*)IObject::Probe(obj);
        if ((startId) >= range->mStartId && (endId) <= range->mEndId) {
            // exact same range:  new [1, 1] existing [1, 1]
            // range already enclosed in existing: new [3, 3], [1,3]
            // no radio update necessary.
            // duplicate "client" check is done in insert, attempt to insert.
            AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
            range->Insert(_cr);
            *result = TRUE;
            return NOERROR;
        }
        else if ((startId - 1) == range->mEndId) {
            // new [3, x] existing [1, 2]  OR new [2, 2] existing [1, 1]
            // found missing link? check if next range can be joined
            Int32 newRangeEndId = endId;
            AutoPtr<IntRange> nextRange;
            if ((startIndex + 1) < len) {
                AutoPtr<IInterface> o;
                mRanges->Get(startIndex + 1, (IInterface**)&o);
                nextRange = (IntRange*)IObject::Probe(o);
                if ((nextRange->mStartId - 1) <= endId) {
                    // new [3, x] existing [1, 2] [5, 7] OR  new [2 , 2] existing [1, 1] [3, 5]
                    if (endId <= nextRange->mEndId) {
                        // new [3, 6] existing [1, 2] [5, 7]
                        newRangeEndId = nextRange->mStartId - 1; // need to enable [3, 4]
                    }
                }
                else {
                    // mark nextRange to be joined as NULL.
                    nextRange = NULL;
                }
            }
            if (TryAddRanges(startId, newRangeEndId, TRUE)) {
                range->mEndId = endId;
                AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                range->Insert(_cr);

                // found missing link? check if next range can be joined
                if (nextRange != NULL) {
                    if (range->mEndId < nextRange->mEndId) {
                        // new [3, 6] existing [1, 2] [5, 10]
                        range->mEndId = nextRange->mEndId;
                    }
                    range->mClients->AddAll(ICollection::Probe(nextRange->mClients));
                    mRanges->Remove(nextRange->Probe(EIID_IInterface));
                }
                *result = TRUE;
                return NOERROR;
            }
            else {
                *result = FALSE;   // failed to update radio
                return NOERROR;
            }
        }
        else if (startId < range->mStartId) {
            // new [1, x] , existing [5, y]
            // test if new range completely precedes this range
            // note that [1, 4] and [5, 6] coalesce to [1, 6]
            if ((endId + 1) < range->mStartId) {
                // new [1, 3] existing [5, 6] non contiguous case
                // insert new Int32 range before previous first range
                if (TryAddRanges(startId, endId, TRUE)) {
                    AutoPtr<IntRange> _ir = new IntRange(startId, endId, client);
                    mRanges->Add(startIndex, _ir->Probe(EIID_IInterface));
                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    *result = FALSE;   // failed to update radio
                    return NOERROR;
                }
            }
            else if (endId <= range->mEndId) {
                // new [1, 4] existing [5, 6]  or  new [1, 1] existing [2, 2]
                // extend the start of this range
                if (TryAddRanges(startId, range->mStartId - 1, TRUE)) {
                    range->mStartId = startId;
                    AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                    range->mClients->Add(0, _cr->Probe(EIID_IInterface));
                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    *result = FALSE;   // failed to update radio
                    return NOERROR;
                }
            }
            else {
                // find last range that can coalesce into the new combined range
                for (Int32 endIndex = startIndex+1; endIndex < len; endIndex++) {
                    AutoPtr<IInterface> e;
                    mRanges->Get(endIndex, (IInterface**)&e);
                    IntRange* endRange = (IntRange*)IObject::Probe(e);
                    if ((endId + 1) < endRange->mStartId) {
                        // new [1, 10] existing [2, 3] [14, 15]
                        // try to add entire new range
                        if (TryAddRanges(startId, endId, TRUE)) {
                            range->mStartId = startId;
                            range->mEndId = endId;
                            // insert new ClientRange before existing ranges
                            AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                            range->mClients->Add(0, _cr->Probe(EIID_IInterface));
                            // coalesce range with following ranges up to endIndex-1
                            // remove each range after adding its elements, so the index
                            // of the next range to join is always startIndex+1.
                            // i is the index if no elements were removed: we only care
                            // about the number of loop iterations, not the value of i.
                            Int32 joinIndex = startIndex + 1;
                            for (Int32 i = joinIndex; i < endIndex; i++) {
                                // new [1, 10] existing [2, 3] [5, 6] [14, 15]
                                AutoPtr<IInterface> j;
                                mRanges->Get(joinIndex, (IInterface**)&j);
                                IntRange* joinRange = (IntRange*)IObject::Probe(j);
                                range->mClients->AddAll(ICollection::Probe(joinRange->mClients));
                                mRanges->Remove(joinRange->Probe(EIID_IInterface));
                            }
                            *result = TRUE;
                            return NOERROR;
                        }
                        else {
                            *result = FALSE;   // failed to update radio
                            return NOERROR;
                        }
                    }
                    else if (endId <= endRange->mEndId) {
                        // new [1, 10] existing [2, 3] [5, 15]
                        // add range from start id to start of last overlapping range,
                        // values from endRange.startId to endId are already enabled
                        if (TryAddRanges(startId, endRange->mStartId - 1, TRUE)) {
                            range->mStartId = startId;
                            range->mEndId = endRange->mEndId;
                            // insert new ClientRange before existing ranges
                            AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                            range->mClients->Add(0, _cr->Probe(EIID_IInterface));
                            // coalesce range with following ranges up to endIndex
                            // remove each range after adding its elements, so the index
                            // of the next range to join is always startIndex+1.
                            // i is the index if no elements were removed: we only care
                            // about the number of loop iterations, not the value of i.
                            Int32 joinIndex = startIndex + 1;
                            for (Int32 i = joinIndex; i <= endIndex; i++) {
                                AutoPtr<IInterface> j;
                                mRanges->Get(joinIndex, (IInterface**)&j);
                                IntRange* joinRange = (IntRange*)IObject::Probe(j);
                                range->mClients->AddAll(ICollection::Probe(joinRange->mClients));
                                mRanges->Remove(joinRange->Probe(EIID_IInterface));
                            }
                            *result = TRUE;
                            return NOERROR;
                        }
                        else {
                            *result = FALSE;   // failed to update radio
                            return NOERROR;
                        }
                    }
                }

                // new [1, 10] existing [2, 3]
                // endId extends past all existing IntRanges: combine them all together
                if (TryAddRanges(startId, endId, TRUE)) {
                    range->mStartId = startId;
                    range->mEndId = endId;
                    // insert new ClientRange before existing ranges
                    AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                    range->mClients->Add(0, _cr->Probe(EIID_IInterface));
                    // coalesce range with following ranges up to len-1
                    // remove each range after adding its elements, so the index
                    // of the next range to join is always startIndex+1.
                    // i is the index if no elements were removed: we only care
                    // about the number of loop iterations, not the value of i.
                    Int32 joinIndex = startIndex + 1;
                    for (Int32 i = joinIndex; i < len; i++) {
                        // new [1, 10] existing [2, 3] [5, 6]
                        AutoPtr<IInterface> r;
                        mRanges->Get(joinIndex, (IInterface**)&r);
                        IntRange* joinRange = (IntRange*)IObject::Probe(r);
                        range->mClients->AddAll(ICollection::Probe(joinRange->mClients));
                        mRanges->Remove(joinRange->Probe(EIID_IInterface));
                    }
                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    *result = FALSE;   // failed to update radio
                    return NOERROR;
                }
            }
        }
        else if ((startId + 1) <= range->mEndId) {
            // new [2, x] existing [1, 4]
            if (endId <= range->mEndId) {
                // new [2, 3] existing [1, 4]
                // completely contained in existing range; no radio changes
                AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                range->Insert(_cr);
                *result = TRUE;
                return NOERROR;
            }
            else {
                // new [2, 5] existing [1, 4]
                // find last range that can coalesce into the new combined range
                Int32 endIndex = startIndex;
                for (Int32 testIndex = startIndex + 1; testIndex < len; testIndex++) {
                    AutoPtr<IInterface> t;
                    mRanges->Get(testIndex, (IInterface**)&t);
                    IntRange* testRange = (IntRange*)IObject::Probe(t);
                    if ((endId + 1) < testRange->mStartId) {
                        break;
                    }
                    else {
                        endIndex = testIndex;
                    }
                }
                // no adjacent IntRanges to combine
                if (endIndex == startIndex) {
                    // new [2, 5] existing [1, 4]
                    // add range from range.endId+1 to endId,
                    // values from startId to range.endId are already enabled
                    if (TryAddRanges(range->mEndId + 1, endId, TRUE)) {
                        range->mEndId = endId;
                        AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                        range->Insert(_cr);
                        *result = TRUE;
                        return NOERROR;
                    }
                    else {
                        *result = FALSE;   // failed to update radio
                        return NOERROR;
                    }
                }
                // get last range to coalesce into start range
                AutoPtr<IInterface> e;
                mRanges->Get(endIndex, (IInterface**)&e);
                IntRange* endRange = (IntRange*)IObject::Probe(e);
                // Values from startId to range.endId have already been enabled.
                // if endId > endRange.endId, then enable range from range.endId+1 to endId,
                // else enable range from range.endId+1 to endRange.startId-1, because
                // values from endRange.startId to endId have already been added.
                Int32 newRangeEndId = (endId <= endRange->mEndId) ? endRange->mStartId - 1 : endId;
                // new [2, 10] existing [1, 4] [7, 8] OR
                // new [2, 10] existing [1, 4] [7, 15]
                if (TryAddRanges(range->mEndId + 1, newRangeEndId, TRUE)) {
                    newRangeEndId = (endId <= endRange->mEndId) ? endRange->mEndId : endId;
                    range->mEndId = newRangeEndId;
                    // insert new ClientRange in place
                    AutoPtr<ClientRange> _cr = new ClientRange(startId, endId, client);
                    range->Insert(_cr);
                    // coalesce range with following ranges up to endIndex
                    // remove each range after adding its elements, so the index
                    // of the next range to join is always startIndex+1 (joinIndex).
                    // i is the index if no elements had been removed: we only care
                    // about the number of loop iterations, not the value of i.
                    Int32 joinIndex = startIndex + 1;
                    for (Int32 i = joinIndex; i <= endIndex; i++) {
                        AutoPtr<IInterface> j;
                        mRanges->Get(joinIndex, (IInterface**)&j);
                        IntRange* joinRange = (IntRange*)IObject::Probe(j);
                        range->mClients->AddAll(ICollection::Probe(joinRange->mClients));
                        mRanges->Remove(joinRange->Probe(EIID_IInterface));
                    }
                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    *result = FALSE;   // failed to update radio
                    return NOERROR;
                }
            }
        }
    }

    // new [5, 6], existing [1, 3]
    // append new range after existing IntRanges
    if (TryAddRanges(startId, endId, TRUE)) {
        AutoPtr<IntRange> _ir = new IntRange(startId, endId, client);
        mRanges->Add(_ir->Probe(EIID_IInterface));
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;   // failed to update radio
    return NOERROR;
}

/**
 * Disable a range for the specified client and update ranges
 * if necessary. If {@link #finishUpdate} returns failure,
 * FALSE is returned and the range is not removed.
 *
 * @param startId the first id included in the range
 * @param endId the last id included in the range
 * @param client the client requesting to disable the range
 * @return TRUE if successful, FALSE otherwise
 */
ECode IntRangeManager::DisableRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ const String& client,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);

    Int32 len = 0;
    mRanges->GetSize(&len);

    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        mRanges->Get(i, (IInterface**)&obj);
        IntRange* range = (IntRange*)IObject::Probe(obj);
        if (startId < range->mStartId) {
            *result = FALSE;   // not found
            return NOERROR;
        }
        else if (endId <= range->mEndId) {
            // found the IntRange that encloses the client range, if any
            // search for it in the clients list
            AutoPtr<IArrayList/*<ClientRange*/> clients = range->mClients;

            // handle common case of IntRange containing one ClientRange
            Int32 crLength = 0;
            clients->GetSize(&crLength);
            if (crLength == 1) {
                AutoPtr<IInterface> o;
                clients->Get(0, (IInterface**)&o);
                ClientRange* cr = (ClientRange*)IClientRange::Probe(o);
                Boolean tmp = FALSE;
                if (cr->mStartId == startId && cr->mEndId == endId && cr->mClient.Equals(client)) {
                    // mRange contains only what's enabled.
                    // remove the range from mRange then update the radio
                    mRanges->Remove(i);
                    if (UpdateRanges(&tmp), tmp) {
                        *result = TRUE;
                        return NOERROR;
                    }
                    else {
                        // failed to update radio.  insert back the range
                        mRanges->Add(i, range->Probe(EIID_IInterface));
                        *result = FALSE;
                        return NOERROR;
                    }
                }
                else {
                    *result = FALSE;   // not found
                    return NOERROR;
                }
            }

            // several ClientRanges: remove one, potentially splitting into many IntRanges.
            // Save the original start and end id for the original IntRange
            // in case the radio update fails and we have to revert it. If the
            // update succeeds, we remove the client range and insert the new IntRanges.
            // clients are ordered by startId then by endId, so client with largest endId
            // can be anywhere.  Need to loop thru to find largestEndId.
            Int32 largestEndId = Elastos::Core::Math::INT32_MIN_VALUE;  // largest end identifier found
            Boolean updateStarted = FALSE;

            // crlength >= 2
            for (Int32 crIndex=0; crIndex < crLength; crIndex++) {
                AutoPtr<IInterface> c;
                clients->Get(crIndex, (IInterface**)&c);
                ClientRange* cr = (ClientRange*)IClientRange::Probe(c);
                Boolean tmp = FALSE;
                if (cr->mStartId == startId && cr->mEndId == endId && cr->mClient.Equals(client)) {
                    // found the ClientRange to remove, check if it's the last in the list
                    if (crIndex == crLength - 1) {
                        if (range->mEndId == largestEndId) {
                            // remove [2, 5] from [1, 7] [2, 5]
                            // no channels to remove from radio; return success
                            clients->Remove(crIndex);
                            *result = TRUE;
                            return NOERROR;
                        }
                        else {
                            // disable the channels at the end and lower the end id
                            clients->Remove(crIndex);
                            range->mEndId = largestEndId;
                            if (UpdateRanges(&tmp), tmp) {
                                *result = TRUE;
                                return NOERROR;
                            }
                            else {
                                clients->Add(crIndex, cr->Probe(EIID_IInterface));
                                range->mEndId = cr->mEndId;
                                *result = FALSE;
                                return NOERROR;
                            }
                        }
                    }

                    // copy the IntRange so that we can remove elements and modify the
                    // start and end id's in the copy, leaving the original unmodified
                    // until after the radio update succeeds
                    AutoPtr<IntRange> rangeCopy = new IntRange(range, crIndex);

                    if (crIndex == 0) {
                        // removing the first ClientRange, so we may need to increase
                        // the start id of the IntRange.
                        // We know there are at least two ClientRanges in the list,
                        // because check for just one ClientRanges case is already handled
                        // so clients.get(1) should always succeed.
                        AutoPtr<IInterface> n;
                        clients->Get(1, (IInterface**)&n);
                        ClientRange* crt = (ClientRange*)IClientRange::Probe(n);
                        Int32 nextStartId = crt->mStartId;
                        if (nextStartId != range->mStartId) {
                            updateStarted = TRUE;
                            rangeCopy->mStartId = nextStartId;
                        }
                        // init largestEndId
                        largestEndId = crt->mEndId;
                    }

                    // go through remaining ClientRanges, creating new IntRanges when
                    // there is a gap in the sequence. After radio update succeeds,
                    // remove the original IntRange and append newRanges to mRanges.
                    // Otherwise, leave the original IntRange in mRanges and return FALSE.
                    AutoPtr<IArrayList/*<IntRange*/> newRanges;
                    CArrayList::New((IArrayList**)&newRanges);

                    IntRange* currentRange = rangeCopy;
                    for (Int32 nextIndex = crIndex + 1; nextIndex < crLength; nextIndex++) {
                        AutoPtr<IInterface> ncr;
                        clients->Get(nextIndex, (IInterface**)&ncr);
                        ClientRange* nextCr = (ClientRange*)IClientRange::Probe(ncr);
                        if (nextCr->mStartId > largestEndId + 1) {
                            updateStarted = TRUE;
                            currentRange->mEndId = largestEndId;
                            newRanges->Add(currentRange->Probe(EIID_IInterface));
                            currentRange = new IntRange(nextCr);
                        }
                        else {
                            if (currentRange->mEndId < nextCr->mEndId) {
                                currentRange->mEndId = nextCr->mEndId;
                            }
                            currentRange->mClients->Add(nextCr->Probe(EIID_IInterface));
                        }
                        if (nextCr->mEndId > largestEndId) {
                            largestEndId = nextCr->mEndId;
                        }
                    }

                    // remove any channels between largestEndId and endId
                    if (largestEndId < endId) {
                        updateStarted = TRUE;
                        currentRange->mEndId = largestEndId;
                    }
                    newRanges->Add(currentRange->Probe(EIID_IInterface));

                    // replace the original IntRange with newRanges
                    mRanges->Remove(i);
                    mRanges->AddAll(i, ICollection::Probe(newRanges));
                    Boolean tmp = FALSE;
                    if (updateStarted && (UpdateRanges(&tmp), !tmp)) {
                        // failed to update radio.  revert back mRange.
                        mRanges->RemoveAll(ICollection::Probe(newRanges));
                        mRanges->Add(i, range->Probe(EIID_IInterface));
                        *result = FALSE;
                        return NOERROR;
                    }

                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    // not the ClientRange to remove; save highest end ID seen so far
                    if (cr->mEndId > largestEndId) {
                        largestEndId = cr->mEndId;
                    }
                }
            }
        }
    }

    *result = FALSE;   // not found
    return NOERROR;
}

/**
 * Perform a complete update operation (enable all ranges). Useful
 * after a radio reset. Calls {@link #startUpdate}, followed by zero or
 * more calls to {@link #addRange}, followed by {@link #finishUpdate}.
 * @return TRUE if successful, FALSE otherwise
 */
ECode IntRangeManager::UpdateRanges(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    StartUpdate();

    PopulateAllRanges();
    *result = FinishUpdate();
    return NOERROR;
}

/**
 * Enable or disable a single range of message identifiers.
 * @param startId the first id included in the range
 * @param endId the last id included in the range
 * @param selected TRUE to enable range, FALSE to disable range
 * @return TRUE if successful, FALSE otherwise
 */
Boolean IntRangeManager::TryAddRanges(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ Boolean selected)
{
    StartUpdate();
    PopulateAllRanges();
    // This is the new range to be enabled
    AddRange(startId, endId, selected); // adds to mConfigList
    return FinishUpdate();
}

/**
 * Returns whether the list of ranges is completely empty.
 * @return TRUE if there are no enabled ranges
 */
ECode IntRangeManager::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mRanges->IsEmpty(result);
}

/**
 * Called when attempting to add a single range of message identifiers
 * Populate all ranges of message identifiers.
 */
void IntRangeManager::PopulateAllRanges()
{
    AutoPtr<IIterator> itr;
    mRanges->GetIterator((IIterator**)&itr);
    Boolean hasNext;
    // Populate all ranges from mRanges
    while (itr->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        itr->GetNext((IInterface**)&obj);
        IntRange* currRange = (IntRange*) IObject::Probe(obj);
        AddRange(currRange->mStartId, currRange->mEndId, TRUE);
    }
}

/**
 * Called when attempting to add a single range of message identifiers
 * Populate all ranges of message identifiers using clients' ranges.
 */
void IntRangeManager::PopulateAllClientRanges()
{
    Int32 len = 0;
    mRanges->GetSize(&len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        mRanges->Get(i, (IInterface**)&obj);
        IntRange* range = (IntRange*)IObject::Probe(obj);

        Int32 clientLen = 0;
        range->mClients->GetSize(&clientLen);
        for (Int32 j=0; j < clientLen; j++) {
            AutoPtr<IInterface> o;
            range->mClients->Get(j, (IInterface**)&o);
            ClientRange* nextRange = (ClientRange*)IClientRange::Probe(o);
            AddRange(nextRange->mStartId, nextRange->mEndId, TRUE);
        }
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
