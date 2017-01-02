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

#include "ManifestReader.h"
#include "CString.h"
#include "CName.h"
#include "CByteArrayOutputStream.h"
#include "CAttributes.h"
#include <cutils/log.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::IOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::Charset::ICharset;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Utility {
namespace Jar {

ManifestReader::ManifestReader(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ IAttributes* main)
    : mEndOfMainSection(0)
    , mPos(0)
    , mConsecutiveLineBreaks(0)
{
    CByteArrayOutputStream::New(80, (IByteArrayOutputStream**)&mValueBuffer);

    mBuf = buf;
    assert(mBuf != NULL);

    Boolean result;
    while (ReadHeader(&result), result) {
        AutoPtr<ICharSequence> csq;
        CString::New(mValue, (ICharSequence**)&csq);
        IMap::Probe(main)->Put(mName, csq);
    }
    mEndOfMainSection = mPos;
}

ECode ManifestReader::ReadEntries(
    /* [in] */ IMap * entries,
    /* [in] */ HashMap<String, AutoPtr<CManifest::Chunk> > * chunks)
{
    HashMap<String, AutoPtr<IAttributes> > map;
    FAIL_RETURN(ReadEntries(&map, chunks))

    HashMap<String, AutoPtr<IAttributes> >::Iterator it;
    for (it = map.Begin(); it != map.End(); ++it) {
        AutoPtr<ICharSequence> csq;
        CString::New(it->mFirst, (ICharSequence**)&csq);
        entries->Put(csq, it->mSecond);
    }
    return NOERROR;
}

ECode ManifestReader::ReadEntries(
    /* [in] */ HashMap<String, AutoPtr<IAttributes> > * entries,
    /* [in] */ HashMap<String, AutoPtr<CManifest::Chunk> > * chunks)
{
    VALIDATE_NOT_NULL(entries)

    HashMap<String, AutoPtr<CManifest::Chunk> >::Iterator it;
    Boolean bval;
    Int32 mark = mPos;
    HashMap<String, AutoPtr<IAttributes> >::Iterator sait;
    while (ReadHeader(&bval), bval) {
        if (CName::NAME->Equals(mName, &bval), !bval) {
            ALOGE("IOException: ManifestReader::ReadEntries: Entry is not named");
            return E_IO_EXCEPTION;
        }

        String entryNameValue = mValue;
        sait = entries->Find(entryNameValue);
        AutoPtr<IAttributes> entry;
        if (sait != entries->End()) {
            entry = sait->mSecond;
        }
        if (entry == NULL) {
            CAttributes::New(12, (IAttributes**)&entry);
        }

        while (ReadHeader(&bval), bval) {
            AutoPtr<ICharSequence> cst;
            CString::New(mValue, (ICharSequence**)&cst);
            IMap::Probe(entry)->Put(mName, cst);
        }

        if (chunks != NULL) {
            it = chunks->Find(entryNameValue);
            if (it != chunks->End()) {
                // TODO A bug: there might be several verification chunks for
                // the same name. I believe they should be used to update
                // signature in order of appearance; there are two ways to fix
                // this: either use a list of chunks, or decide on used
                // signature algorithm in advance and reread the chunks while
                // updating the signature; for now a defensive error is thrown
                ALOGE("IOException: ManifestReader::ReadEntries: A jar verifier does not support more than one entry with the same name");
                return E_IO_EXCEPTION;
            }
            (*chunks)[entryNameValue] = new CManifest::Chunk(mark, mPos);
            mark = mPos;
        }

        (*entries)[entryNameValue] = entry;
    }
    return NOERROR;
}

Int32 ManifestReader::GetEndOfMainSection()
{
    return mEndOfMainSection;
}

ECode ManifestReader::ReadHeader(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mConsecutiveLineBreaks > 1) {
        // break a section on an empty line
        mConsecutiveLineBreaks = 0;
        return NOERROR;
    }

    FAIL_RETURN(ReadName())
    mConsecutiveLineBreaks = 0;
    FAIL_RETURN(ReadValue())

    // if the last line break is missed, the line
    // is ignored by the reference implementation
    *result = mConsecutiveLineBreaks > 0;
    return NOERROR;
}

ECode ManifestReader::ReadName()
{
    Int32 mark = mPos;
    HashMap<String, AutoPtr<IName> >::Iterator it;
    while (mPos < mBuf->GetLength()) {
        if ((*mBuf)[mPos++] != ':') {
            continue;
        }

        String nameString = String((const char*)mBuf->GetPayload() + mark, mPos - mark - 1/*, StandardCharsets.US_ASCII*/);

        if ((*mBuf)[mPos++] != ' ') {
            ALOGE("IOException: ManifestReader::ReadName: Invalid value for attribute '%s'", nameString.string());
            return E_IO_EXCEPTION;
        }

        // try {
        it = mAttributeNameCache.Find(nameString);
        if (it == mAttributeNameCache.End()) {
            mName = NULL;
            if (FAILED(CName::New(nameString, (IName**)&mName))) {
                ALOGE("ManifestReader::ReadName: new Attributes.Name() throws IllegalArgumentException but we declare IOException");
                return E_IO_EXCEPTION;
            }
            mAttributeNameCache[nameString] = mName;
        }
        else {
            mName = it->mSecond;
        }
        return NOERROR;
    }

    return NOERROR;
}

ECode ManifestReader::ReadValue()
{
    Boolean lastCr = FALSE;
    Int32 mark = mPos;
    Int32 last = mPos;
    mValueBuffer->Reset();
    while (mPos < mBuf->GetLength()) {
        Byte next = (*mBuf)[mPos++];
        switch (next) {
        case 0:
            ALOGE("IOException: ManifestReader::ReadValue() NULL character in a manifest");
            return E_IO_EXCEPTION;

        case '\n':
            if (lastCr) {
                lastCr = FALSE;
            } else {
                mConsecutiveLineBreaks++;
            }
            continue;

        case '\r':
            lastCr = true;
            mConsecutiveLineBreaks++;
            continue;

        case ' ':
            if (mConsecutiveLineBreaks == 1) {
                IOutputStream::Probe(mValueBuffer)->Write(mBuf, mark, last - mark);
                mark = mPos;
                mConsecutiveLineBreaks = 0;
                continue;
            }
        }

        if (mConsecutiveLineBreaks >= 1) {
            mPos--;
            break;
        }
        last = mPos;
    }

    IOutputStream::Probe(mValueBuffer)->Write(mBuf, mark, last - mark);
    // A bit frustrating that that Charset.forName will be called
    // again.
    AutoPtr<ICharset> charset;
    String charsetName;
    //StandardCharsets.UTF_8.name();
    return mValueBuffer->ToString(charsetName, &mValue);
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos