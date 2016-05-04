/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;

/**
 * Clients can enable reception of SMS-CB messages for specific ranges of
 * message Identifiers (channels). This class keeps track of the currently
 * enabled message identifiers and calls abstract methods to update the
 * radio when the range of enabled message identifiers changes.
 *
 * An update is a call to {@link #startUpdate} followed by zero or more
 * calls to {@link #addRange} followed by a call to {@link #finishUpdate}.
 * Calls to {@link #enableRange} and {@link #disableRange} will perform
 * an incremental update operation if the enabled ranges have changed.
 * A full update Operation (i.e. after a radio reset) can be performed
 * by a call to {@link #updateRanges}.
 *
 * Clients are identified by String (the name associated with the User ID
 * of the caller) so that a call to remove a range can be mapped to the
 * client that enabled that Range (or else rejected).
 */
public abstract class IntRangeManager {

    /**
     * Initial capacity for IntRange clients array list. There will be
     * few cell broadcast listeners on a typical device, so this can be small.
     */
    private static const Int32 INITIAL_CLIENTS_ARRAY_SIZE = 4;

    /**
     * One or more clients forming the continuous range [startId, endId].
     * <p>When a client is added, the IntRange may merge with one or more
     * adjacent IntRanges to form a single combined IntRange.
     * <p>When a client is removed, the IntRange may divide into several
     * non-contiguous IntRanges.
     */
    private class IntRange {
        Int32 mStartId;
        Int32 mEndId;
        // sorted by earliest start id
        final ArrayList<ClientRange> mClients;

        /**
         * Create a new IntRange with a single client.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         * @param client the client requesting the enabled range
         */
        IntRange(Int32 startId, Int32 endId, String client) {
            mStartId = startId;
            mEndId = endId;
            mClients = new ArrayList<ClientRange>(INITIAL_CLIENTS_ARRAY_SIZE);
            mClients->Add(new ClientRange(startId, endId, client));
        }

        /**
         * Create a new IntRange for an existing ClientRange.
         * @param clientRange the initial ClientRange to add
         */
        IntRange(ClientRange clientRange) {
            mStartId = clientRange.mStartId;
            mEndId = clientRange.mEndId;
            mClients = new ArrayList<ClientRange>(INITIAL_CLIENTS_ARRAY_SIZE);
            mClients->Add(clientRange);
        }

        /**
         * Create a new IntRange from an existing IntRange. This is used for
         * removing a ClientRange, because new IntRanges may need to be created
         * for any gaps that open up after the ClientRange is removed. A copy
         * is made of the elements of the original IntRange preceding the element
         * that is being removed. The following elements will be added to this
         * IntRange or to a new IntRange when a gap is found.
         * @param intRange the original IntRange to copy elements from
         * @param numElements the number of elements to copy from the original
         */
        IntRange(IntRange intRange, Int32 numElements) {
            mStartId = intRange.mStartId;
            mEndId = intRange.mEndId;
            mClients = new ArrayList<ClientRange>(intRange.mClients->Size());
            For (Int32 i=0; i < numElements; i++) {
                mClients->Add(intRange.mClients->Get(i));
            }
        }

        /**
         * Insert new ClientRange in order by start id, then by end id
         * <p>If the new ClientRange is known to be sorted before or after the
         * existing ClientRanges, or at a particular index, it can be added
         * to the clients array list directly, instead of via this method.
         * <p>Note that this can be changed from linear to binary search if the
         * number of clients grows large enough that it would make a difference.
         * @param range the new ClientRange to insert
         */
        void Insert(ClientRange range) {
            Int32 len = mClients->Size();
            Int32 insert = -1;
            For (Int32 i=0; i < len; i++) {
                ClientRange nextRange = mClients->Get(i);
                If (range.mStartId <= nextRange.mStartId) {
                    // ignore duplicate ranges from the same client
                    If (!range->Equals(nextRange)) {
                        // check if same startId, then order by endId
                        If (range.mStartId == nextRange.mStartId
                                && range.mEndId > nextRange.mEndId) {
                            insert = i + 1;
                            If (insert < len) {
                                // there may be more client following with same startId
                                // new [1, 5] existing [1, 2] [1, 4] [1, 7]
                                continue;
                            }
                            break;
                        }
                        mClients->Add(i, range);
                    }
                    return;
                }
            }
            If (insert != -1 && insert < len) {
                mClients->Add(insert, range);
                return;
            }
            mClients->Add(range);    // append to end of list
        }
    }

    /**
     * The message id range for a single client.
     */
    private class ClientRange {
        final Int32 mStartId;
        final Int32 mEndId;
        final String mClient;

        ClientRange(Int32 startId, Int32 endId, String client) {
            mStartId = startId;
            mEndId = endId;
            mClient = client;
        }

        //@Override
        public Boolean Equals(Object o) {
            If (o != NULL && o instanceof ClientRange) {
                ClientRange other = (ClientRange) o;
                return mStartId == other.mStartId &&
                        mEndId == other.mEndId &&
                        mClient->Equals(other.mClient);
            } else {
                return FALSE;
            }
        }

        //@Override
        public Int32 HashCode() {
            Return (mStartId * 31 + mEndId) * 31 + mClient->HashCode();
        }
    }

    /**
     * List of integer ranges, one per client, sorted by start id.
     */
    private ArrayList<IntRange> mRanges = new ArrayList<IntRange>();

    protected IntRangeManager() {}

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
    public synchronized Boolean EnableRange(Int32 startId, Int32 endId, String client) {
        Int32 len = mRanges->Size();

        // empty range list: add the initial IntRange
        If (len == 0) {
            If (TryAddRanges(startId, endId, TRUE)) {
                mRanges->Add(new IntRange(startId, endId, client));
                return TRUE;
            } else {
                return FALSE;   // failed to update radio
            }
        }

        For (Int32 startIndex = 0; startIndex < len; startIndex++) {
            IntRange range = mRanges->Get(startIndex);
            If ((startId) >= range.mStartId && (endId) <= range.mEndId) {
                // exact same range:  new [1, 1] existing [1, 1]
                // range already enclosed in existing: new [3, 3], [1,3]
                // no radio update necessary.
                // duplicate "client" check is done in insert, attempt to insert.
                range->Insert(new ClientRange(startId, endId, client));
                return TRUE;
            } else If ((startId - 1) == range.mEndId) {
                // new [3, x] existing [1, 2]  OR new [2, 2] existing [1, 1]
                // found missing link? check if next range can be joined
                Int32 newRangeEndId = endId;
                IntRange nextRange = NULL;
                If ((startIndex + 1) < len) {
                    nextRange = mRanges->Get(startIndex + 1);
                    If ((nextRange.mStartId - 1) <= endId) {
                        // new [3, x] existing [1, 2] [5, 7] OR  new [2 , 2] existing [1, 1] [3, 5]
                        If (endId <= nextRange.mEndId) {
                            // new [3, 6] existing [1, 2] [5, 7]
                            newRangeEndId = nextRange.mStartId - 1; // need to enable [3, 4]
                        }
                    } else {
                        // mark nextRange to be joined as NULL.
                        nextRange = NULL;
                    }
                }
                If (TryAddRanges(startId, newRangeEndId, TRUE)) {
                    range.mEndId = endId;
                    range->Insert(new ClientRange(startId, endId, client));

                    // found missing link? check if next range can be joined
                    If (nextRange != NULL) {
                        If (range.mEndId < nextRange.mEndId) {
                            // new [3, 6] existing [1, 2] [5, 10]
                            range.mEndId = nextRange.mEndId;
                        }
                        range.mClients->AddAll(nextRange.mClients);
                        mRanges->Remove(nextRange);
                    }
                    return TRUE;
                } else {
                    return FALSE;   // failed to update radio
                }
            } else If (startId < range.mStartId) {
                // new [1, x] , existing [5, y]
                // test if new range completely precedes this range
                // note that [1, 4] and [5, 6] coalesce to [1, 6]
                If ((endId + 1) < range.mStartId) {
                    // new [1, 3] existing [5, 6] non contiguous case
                    // insert new Int32 range before previous first range
                    If (TryAddRanges(startId, endId, TRUE)) {
                        mRanges->Add(startIndex, new IntRange(startId, endId, client));
                        return TRUE;
                    } else {
                        return FALSE;   // failed to update radio
                    }
                } else If (endId <= range.mEndId) {
                    // new [1, 4] existing [5, 6]  or  new [1, 1] existing [2, 2]
                    // extend the start of this range
                    If (TryAddRanges(startId, range.mStartId - 1, TRUE)) {
                        range.mStartId = startId;
                        range.mClients->Add(0, new ClientRange(startId, endId, client));
                        return TRUE;
                    } else {
                        return FALSE;   // failed to update radio
                    }
                } else {
                    // find last range that can coalesce into the new combined range
                    For (Int32 endIndex = startIndex+1; endIndex < len; endIndex++) {
                        IntRange endRange = mRanges->Get(endIndex);
                        If ((endId + 1) < endRange.mStartId) {
                            // new [1, 10] existing [2, 3] [14, 15]
                            // try to add entire new range
                            If (TryAddRanges(startId, endId, TRUE)) {
                                range.mStartId = startId;
                                range.mEndId = endId;
                                // insert new ClientRange before existing ranges
                                range.mClients->Add(0, new ClientRange(startId, endId, client));
                                // coalesce range with following ranges up to endIndex-1
                                // remove each range after adding its elements, so the index
                                // of the next range to join is always startIndex+1.
                                // i is the index if no elements were removed: we only care
                                // about the number of loop iterations, not the value of i.
                                Int32 joinIndex = startIndex + 1;
                                For (Int32 i = joinIndex; i < endIndex; i++) {
                                    // new [1, 10] existing [2, 3] [5, 6] [14, 15]
                                    IntRange joinRange = mRanges->Get(joinIndex);
                                    range.mClients->AddAll(joinRange.mClients);
                                    mRanges->Remove(joinRange);
                                }
                                return TRUE;
                            } else {
                                return FALSE;   // failed to update radio
                            }
                        } else If (endId <= endRange.mEndId) {
                            // new [1, 10] existing [2, 3] [5, 15]
                            // add range from start id to start of last overlapping range,
                            // values from endRange.startId to endId are already enabled
                            If (TryAddRanges(startId, endRange.mStartId - 1, TRUE)) {
                                range.mStartId = startId;
                                range.mEndId = endRange.mEndId;
                                // insert new ClientRange before existing ranges
                                range.mClients->Add(0, new ClientRange(startId, endId, client));
                                // coalesce range with following ranges up to endIndex
                                // remove each range after adding its elements, so the index
                                // of the next range to join is always startIndex+1.
                                // i is the index if no elements were removed: we only care
                                // about the number of loop iterations, not the value of i.
                                Int32 joinIndex = startIndex + 1;
                                For (Int32 i = joinIndex; i <= endIndex; i++) {
                                    IntRange joinRange = mRanges->Get(joinIndex);
                                    range.mClients->AddAll(joinRange.mClients);
                                    mRanges->Remove(joinRange);
                                }
                                return TRUE;
                            } else {
                                return FALSE;   // failed to update radio
                            }
                        }
                    }

                    // new [1, 10] existing [2, 3]
                    // endId extends past all existing IntRanges: combine them all together
                    If (TryAddRanges(startId, endId, TRUE)) {
                        range.mStartId = startId;
                        range.mEndId = endId;
                        // insert new ClientRange before existing ranges
                        range.mClients->Add(0, new ClientRange(startId, endId, client));
                        // coalesce range with following ranges up to len-1
                        // remove each range after adding its elements, so the index
                        // of the next range to join is always startIndex+1.
                        // i is the index if no elements were removed: we only care
                        // about the number of loop iterations, not the value of i.
                        Int32 joinIndex = startIndex + 1;
                        For (Int32 i = joinIndex; i < len; i++) {
                            // new [1, 10] existing [2, 3] [5, 6]
                            IntRange joinRange = mRanges->Get(joinIndex);
                            range.mClients->AddAll(joinRange.mClients);
                            mRanges->Remove(joinRange);
                        }
                        return TRUE;
                    } else {
                        return FALSE;   // failed to update radio
                    }
                }
            } else If ((startId + 1) <= range.mEndId) {
                // new [2, x] existing [1, 4]
                If (endId <= range.mEndId) {
                    // new [2, 3] existing [1, 4]
                    // completely contained in existing range; no radio changes
                    range->Insert(new ClientRange(startId, endId, client));
                    return TRUE;
                } else {
                    // new [2, 5] existing [1, 4]
                    // find last range that can coalesce into the new combined range
                    Int32 endIndex = startIndex;
                    For (Int32 testIndex = startIndex+1; testIndex < len; testIndex++) {
                        IntRange testRange = mRanges->Get(testIndex);
                        If ((endId + 1) < testRange.mStartId) {
                            break;
                        } else {
                            endIndex = testIndex;
                        }
                    }
                    // no adjacent IntRanges to combine
                    If (endIndex == startIndex) {
                        // new [2, 5] existing [1, 4]
                        // add range from range.endId+1 to endId,
                        // values from startId to range.endId are already enabled
                        If (TryAddRanges(range.mEndId + 1, endId, TRUE)) {
                            range.mEndId = endId;
                            range->Insert(new ClientRange(startId, endId, client));
                            return TRUE;
                        } else {
                            return FALSE;   // failed to update radio
                        }
                    }
                    // get last range to coalesce into start range
                    IntRange endRange = mRanges->Get(endIndex);
                    // Values from startId to range.endId have already been enabled.
                    // if endId > endRange.endId, then enable range from range.endId+1 to endId,
                    // else enable range from range.endId+1 to endRange.startId-1, because
                    // values from endRange.startId to endId have already been added.
                    Int32 newRangeEndId = (endId <= endRange.mEndId) ? endRange.mStartId - 1 : endId;
                    // new [2, 10] existing [1, 4] [7, 8] OR
                    // new [2, 10] existing [1, 4] [7, 15]
                    If (TryAddRanges(range.mEndId + 1, newRangeEndId, TRUE)) {
                        newRangeEndId = (endId <= endRange.mEndId) ? endRange.mEndId : endId;
                        range.mEndId = newRangeEndId;
                        // insert new ClientRange in place
                        range->Insert(new ClientRange(startId, endId, client));
                        // coalesce range with following ranges up to endIndex
                        // remove each range after adding its elements, so the index
                        // of the next range to join is always startIndex+1 (joinIndex).
                        // i is the index if no elements had been removed: we only care
                        // about the number of loop iterations, not the value of i.
                        Int32 joinIndex = startIndex + 1;
                        For (Int32 i = joinIndex; i <= endIndex; i++) {
                            IntRange joinRange = mRanges->Get(joinIndex);
                            range.mClients->AddAll(joinRange.mClients);
                            mRanges->Remove(joinRange);
                        }
                        return TRUE;
                    } else {
                        return FALSE;   // failed to update radio
                    }
                }
            }
        }

        // new [5, 6], existing [1, 3]
        // append new range after existing IntRanges
        If (TryAddRanges(startId, endId, TRUE)) {
            mRanges->Add(new IntRange(startId, endId, client));
            return TRUE;
        } else {
            return FALSE;   // failed to update radio
        }
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
    public synchronized Boolean DisableRange(Int32 startId, Int32 endId, String client) {
        Int32 len = mRanges->Size();

        For (Int32 i=0; i < len; i++) {
            IntRange range = mRanges->Get(i);
            If (startId < range.mStartId) {
                return FALSE;   // not found
            } else If (endId <= range.mEndId) {
                // found the IntRange that encloses the client range, if any
                // search for it in the clients list
                ArrayList<ClientRange> clients = range.mClients;

                // handle common case of IntRange containing one ClientRange
                Int32 crLength = clients->Size();
                If (crLength == 1) {
                    ClientRange cr = clients->Get(0);
                    If (cr.mStartId == startId && cr.mEndId == endId && cr.mClient->Equals(client)) {
                        // mRange contains only what's enabled.
                        // remove the range from mRange then update the radio
                        mRanges->Remove(i);
                        If (UpdateRanges()) {
                            return TRUE;
                        } else {
                            // failed to update radio.  insert back the range
                            mRanges->Add(i, range);
                            return FALSE;
                        }
                    } else {
                        return FALSE;   // not found
                    }
                }

                // several ClientRanges: remove one, potentially splitting into many IntRanges.
                // Save the original start and end id for the original IntRange
                // in case the radio update fails and we have to revert it. If the
                // update succeeds, we remove the client range and insert the new IntRanges.
                // clients are ordered by startId then by endId, so client with largest endId
                // can be anywhere.  Need to loop thru to find largestEndId.
                Int32 largestEndId = Integer.MIN_VALUE;  // largest end identifier found
                Boolean updateStarted = FALSE;

                // crlength >= 2
                For (Int32 crIndex=0; crIndex < crLength; crIndex++) {
                    ClientRange cr = clients->Get(crIndex);
                    If (cr.mStartId == startId && cr.mEndId == endId && cr.mClient->Equals(client)) {
                        // found the ClientRange to remove, check if it's the last in the list
                        If (crIndex == crLength - 1) {
                            If (range.mEndId == largestEndId) {
                                // remove [2, 5] from [1, 7] [2, 5]
                                // no channels to remove from radio; return success
                                clients->Remove(crIndex);
                                return TRUE;
                            } else {
                                // disable the channels at the end and lower the end id
                                clients->Remove(crIndex);
                                range.mEndId = largestEndId;
                                If (UpdateRanges()) {
                                    return TRUE;
                                } else {
                                    clients->Add(crIndex, cr);
                                    range.mEndId = cr.mEndId;
                                    return FALSE;
                                }
                            }
                        }

                        // copy the IntRange so that we can remove elements and modify the
                        // start and end id's in the copy, leaving the original unmodified
                        // until after the radio update succeeds
                        IntRange rangeCopy = new IntRange(range, crIndex);

                        If (crIndex == 0) {
                            // removing the first ClientRange, so we may need to increase
                            // the start id of the IntRange.
                            // We know there are at least two ClientRanges in the list,
                            // because check for just one ClientRanges case is already handled
                            // so clients->Get(1) should always succeed.
                            Int32 nextStartId = clients->Get(1).mStartId;
                            If (nextStartId != range.mStartId) {
                                updateStarted = TRUE;
                                rangeCopy.mStartId = nextStartId;
                            }
                            // init largestEndId
                            largestEndId = clients->Get(1).mEndId;
                        }

                        // go through remaining ClientRanges, creating new IntRanges when
                        // there is a gap in the sequence. After radio update succeeds,
                        // remove the original IntRange and append newRanges to mRanges.
                        // Otherwise, leave the original IntRange in mRanges and return FALSE.
                        ArrayList<IntRange> newRanges = new ArrayList<IntRange>();

                        IntRange currentRange = rangeCopy;
                        For (Int32 nextIndex = crIndex + 1; nextIndex < crLength; nextIndex++) {
                            ClientRange nextCr = clients->Get(nextIndex);
                            If (nextCr.mStartId > largestEndId + 1) {
                                updateStarted = TRUE;
                                currentRange.mEndId = largestEndId;
                                newRanges->Add(currentRange);
                                currentRange = new IntRange(nextCr);
                            } else {
                                If (currentRange.mEndId < nextCr.mEndId) {
                                    currentRange.mEndId = nextCr.mEndId;
                                }
                                currentRange.mClients->Add(nextCr);
                            }
                            If (nextCr.mEndId > largestEndId) {
                                largestEndId = nextCr.mEndId;
                            }
                        }

                        // remove any channels between largestEndId and endId
                        If (largestEndId < endId) {
                            updateStarted = TRUE;
                            currentRange.mEndId = largestEndId;
                        }
                        newRanges->Add(currentRange);

                        // replace the original IntRange with newRanges
                        mRanges->Remove(i);
                        mRanges->AddAll(i, newRanges);
                        If (updateStarted && !UpdateRanges()) {
                            // failed to update radio.  revert back mRange.
                            mRanges->RemoveAll(newRanges);
                            mRanges->Add(i, range);
                            return FALSE;
                        }

                        return TRUE;
                    } else {
                        // not the ClientRange to remove; save highest end ID seen so far
                        If (cr.mEndId > largestEndId) {
                            largestEndId = cr.mEndId;
                        }
                    }
                }
            }
        }

        return FALSE;   // not found
    }

    /**
     * Perform a complete update Operation (enable all ranges). Useful
     * after a radio reset. Calls {@link #startUpdate}, followed by zero or
     * more calls to {@link #addRange}, followed by {@link #finishUpdate}.
     * @return TRUE if successful, FALSE otherwise
     */
    public Boolean UpdateRanges() {
        StartUpdate();

        PopulateAllRanges();
        return FinishUpdate();
    }

    /**
     * Enable or disable a single range of message identifiers.
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param selected TRUE to enable range, FALSE to disable range
     * @return TRUE if successful, FALSE otherwise
     */
    protected Boolean TryAddRanges(Int32 startId, Int32 endId, Boolean selected) {

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
    public Boolean IsEmpty() {
        return mRanges->IsEmpty();
    }

    /**
     * Called when attempting to add a single range of message identifiers
     * Populate all ranges of message identifiers.
     */
    private void PopulateAllRanges() {
        Iterator<IntRange> itr = mRanges->Iterator();
        // Populate all ranges from mRanges
        While (itr->HasNext()) {
            IntRange currRange = (IntRange) itr->Next();
            AddRange(currRange.mStartId, currRange.mEndId, TRUE);
        }
    }

    /**
     * Called when attempting to add a single range of message identifiers
     * Populate all ranges of message identifiers using clients' ranges.
     */
    private void PopulateAllClientRanges() {
        Int32 len = mRanges->Size();
        For (Int32 i = 0; i < len; i++) {
            IntRange range = mRanges->Get(i);

            Int32 clientLen = range.mClients->Size();
            For (Int32 j=0; j < clientLen; j++) {
                ClientRange nextRange = range.mClients->Get(j);
                AddRange(nextRange.mStartId, nextRange.mEndId, TRUE);
            }
        }
    }

    /**
     * Called when the list of enabled ranges has changed. This will be
     * followed by zero or more calls to {@link #addRange} followed by
     * a call to {@link #finishUpdate}.
     */
    protected abstract void StartUpdate();

    /**
     * Called after {@link #startUpdate} to indicate a range of enabled
     * or disabled values.
     *
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param selected TRUE to enable range, FALSE to disable range
     */
    protected abstract void AddRange(Int32 startId, Int32 endId, Boolean selected);

    /**
     * Called to indicate the end of a range update started by the
     * previous call to {@link #startUpdate}.
     * @return TRUE if successful, FALSE otherwise
     */
    protected abstract Boolean FinishUpdate();
}
