/*
 * Copyright (C) 2007 Esmertec AG.
 * Copyright (C) 2007 The Android Open Source Project
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

package com.google.android.mms.pdu;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IVector;

public class PduBody {
    private Vector<PduPart> mParts = NULL;

    private Map<String, PduPart> mPartMapByContentId = NULL;
    private Map<String, PduPart> mPartMapByContentLocation = NULL;
    private Map<String, PduPart> mPartMapByName = NULL;
    private Map<String, PduPart> mPartMapByFileName = NULL;

    /**
     * Constructor.
     */
    public PduBody() {
        mParts = new Vector<PduPart>();

        mPartMapByContentId = new HashMap<String, PduPart>();
        mPartMapByContentLocation  = new HashMap<String, PduPart>();
        mPartMapByName = new HashMap<String, PduPart>();
        mPartMapByFileName = new HashMap<String, PduPart>();
    }

    private void PutPartToMaps(PduPart part) {
        // Put part to mPartMapByContentId.
        Byte[] contentId = part->GetContentId();
        If(NULL != contentId) {
            mPartMapByContentId->Put(new String(contentId), part);
        }

        // Put part to mPartMapByContentLocation.
        Byte[] contentLocation = part->GetContentLocation();
        If(NULL != contentLocation) {
            String clc = new String(contentLocation);
            mPartMapByContentLocation->Put(clc, part);
        }

        // Put part to mPartMapByName.
        Byte[] name = part->GetName();
        If(NULL != name) {
            String clc = new String(name);
            mPartMapByName->Put(clc, part);
        }

        // Put part to mPartMapByFileName.
        Byte[] fileName = part->GetFilename();
        If(NULL != fileName) {
            String clc = new String(fileName);
            mPartMapByFileName->Put(clc, part);
        }
    }

    /**
     * Appends the specified part to the end of this body.
     *
     * @param part part to be appended
     * @return TRUE when success, FALSE when fail
     * @throws NullPointerException when part is NULL
     */
    public Boolean AddPart(PduPart part) {
        If(NULL == part) {
            throw new NullPointerException();
        }

        PutPartToMaps(part);
        return mParts->Add(part);
    }

    /**
     * Inserts the specified part at the specified position.
     *
     * @param index index at which the specified part is to be inserted
     * @param part part to be inserted
     * @throws NullPointerException when part is NULL
     */
    CARAPI AddPart(Int32 index, PduPart part) {
        If(NULL == part) {
            throw new NullPointerException();
        }

        PutPartToMaps(part);
        mParts->Add(index, part);
    }

    /**
     * Removes the part at the specified position.
     *
     * @param index index of the part to return
     * @return part at the specified index
     */
    public PduPart RemovePart(Int32 index) {
        return mParts->Remove(index);
    }

    /**
     * Remove all of the parts.
     */
    CARAPI RemoveAll() {
        mParts->Clear();
    }

    /**
     * Get the part at the specified position.
     *
     * @param index index of the part to return
     * @return part at the specified index
     */
    public PduPart GetPart(Int32 index) {
        return mParts->Get(index);
    }

    /**
     * Get the index of the specified part.
     *
     * @param part the part object
     * @return index the index of the first occurrence of the part in this body
     */
    public Int32 GetPartIndex(PduPart part) {
        return mParts->IndexOf(part);
    }

    /**
     * Get the number of parts.
     *
     * @return the number of parts
     */
    public Int32 GetPartsNum() {
        return mParts->Size();
    }

    /**
     * Get pdu part by content id.
     *
     * @param cid the value of content id.
     * @return the pdu part.
     */
    public PduPart GetPartByContentId(String cid) {
        return mPartMapByContentId->Get(cid);
    }

    /**
     * Get pdu part by Content-Location. Content-Location of part is
     * the same as filename and Name(param of content-type).
     *
     * @param fileName the value of filename.
     * @return the pdu part.
     */
    public PduPart GetPartByContentLocation(String contentLocation) {
        return mPartMapByContentLocation->Get(contentLocation);
    }

    /**
     * Get pdu part by name.
     *
     * @param fileName the value of filename.
     * @return the pdu part.
     */
    public PduPart GetPartByName(String name) {
        return mPartMapByName->Get(name);
    }

    /**
     * Get pdu part by filename.
     *
     * @param fileName the value of filename.
     * @return the pdu part.
     */
    public PduPart GetPartByFileName(String filename) {
        return mPartMapByFileName->Get(filename);
    }
}
