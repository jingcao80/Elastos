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

#include "elastos/droid/app/backup/CBackupHelperDispatcher.h"
#include "elastos/droid/app/backup/CBackupDataInputStream.h"
#include <elastos/coredef.h>
#include <elastos/utility/logging/Slogger.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <unistd.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::Backup::IBackupDataInputStream;
using Elastos::Droid::App::Backup::CBackupDataInputStream;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

#define VERSION_1_HEADER 0x01706c48  // 'Hlp'1 little endian

const String CBackupHelperDispatcher::TAG("BackupHelperDispatcher");

CAR_INTERFACE_IMPL(CBackupHelperDispatcher, Object, IBackupHelperDispatcher)

CAR_OBJECT_IMPL(CBackupHelperDispatcher)

ECode CBackupHelperDispatcher::constructor()
{
    return NOERROR;
}

ECode CBackupHelperDispatcher::AddHelper(
    /* [in] */ const String& keyPrefix,
    /* [in] */ IBackupHelper* helper)
{
    mHelpers[keyPrefix] = helper;
    return NOERROR;
}

ECode CBackupHelperDispatcher::PerformBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    // First, do the helpers that we've already done, since they're already in the state
    // file.
    Int32 err;
    AutoPtr<Header> header = new Header();
    HashMap<String, AutoPtr<IBackupHelper> > helpers(mHelpers);
    AutoPtr<IFileDescriptor> oldStateFD;
    AutoPtr<IFileDescriptor> newStateFD;
    FAIL_RETURN(newState->GetFileDescriptor((IFileDescriptor**)&newStateFD))

    if (oldState != NULL) {
        FAIL_RETURN(oldState->GetFileDescriptor((IFileDescriptor**)&oldStateFD))
        while ((err = NativeReadHeader(header, oldStateFD)) >= 0) {
            if (err == 0) {
                AutoPtr<IBackupHelper> helper = helpers[header->keyPrefix];
                Slogger::D(TAG, String("handling existing helper '") + header->keyPrefix + String("' ")/*TODO: + helper*/);
                if (helper != NULL) {
                    FAIL_RETURN(DoOneBackup(oldState, data, newState, header, helper))
                    FAIL_RETURN(helpers.Erase(header->keyPrefix))
                }
                else {
                    NativeSkipChunk(oldStateFD, header->chunkSize);
                }
            }
        }
    }

    // Then go through and do the rest that we haven't done.
    HashMap<String, AutoPtr<IBackupHelper> >::Iterator entry;
    for (entry = helpers.Begin(); entry != helpers.End(); ++entry) {
        header->keyPrefix = entry->mFirst;
        Slogger::D(TAG, String("handling new helper '") + header->keyPrefix + String("'"));
        AutoPtr<IBackupHelper> helper = entry->mSecond;
        FAIL_RETURN(DoOneBackup(oldState, data, newState, header, helper))
    }
    return NOERROR;
}

ECode CBackupHelperDispatcher::PerformRestore(
    /* [in] */ IBackupDataInput* input,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    Boolean alreadyComplained = FALSE;

    AutoPtr<IBackupDataInputStream> stream;
    FAIL_RETURN(CBackupDataInputStream::New(input, (IBackupDataInputStream**)&stream))

    Boolean result;
    while ((input->ReadNextHeader(&result), result)) {
        String rawKey;
        FAIL_RETURN(input->GetKey(&rawKey))
        Int32 pos = rawKey.IndexOf(':');
        if (pos > 0) {
            String prefix = rawKey.Substring(0, pos);
            AutoPtr<IBackupHelper> helper = mHelpers[prefix];
            if (helper != NULL) {
                Int32 size;
                FAIL_RETURN(input->GetDataSize(&size))
                FAIL_RETURN(stream->SetSize(size))
                FAIL_RETURN(stream->SetKey(rawKey.Substring(pos+1)))
                FAIL_RETURN(helper->RestoreEntity(stream))
            }
            else {
                if (!alreadyComplained) {
                    Slogger::W(TAG, String("Couldn't find helper for: '") + rawKey + String("'"));
                    alreadyComplained = TRUE;
                }
            }
        }
        else {
            if (!alreadyComplained) {
                Slogger::W(TAG, String("Entity with no prefix: '") + rawKey + String("'"));
                alreadyComplained = TRUE;
            }
        }
        FAIL_RETURN(input->SkipEntityData()) // In case they didn't consume the data.
    }

    // Write out the state files -- mHelpers is a TreeMap, so the order is well defined.
    HashMap<String, AutoPtr<IBackupHelper> >::Iterator it;
    for (it = mHelpers.Begin(); it != mHelpers.End(); ++it) {
        AutoPtr<IBackupHelper> helper = it->mSecond;
        FAIL_RETURN(helper->WriteNewStateDescription(newState))
    }

    return NOERROR;
}

ECode CBackupHelperDispatcher::DoOneBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Header* header,
    /* [in] */ IBackupHelper* helper)
{
    AutoPtr<IFileDescriptor> newStateFD;
    FAIL_RETURN(newState->GetFileDescriptor((IFileDescriptor**)&newStateFD))

    // allocate space for the header in the file
    Int32 pos = NativeAllocateHeader(header, newStateFD);
    if (pos < 0) {
        //throw new IOException("NativeAllocateHeader failed (error " + pos + ")");
        Slogger::E(TAG, "NativeAllocateHeader failed (error %d)", pos);
        return E_IO_EXCEPTION;
    }

    FAIL_RETURN(data->SetKeyPrefix(header->keyPrefix))

    // do the backup
    FAIL_RETURN(helper->PerformBackup(oldState, data, newState))

    // fill in the header (seeking back to pos).  The file pointer will be returned to
    // where it was at the end of performBackup.  Header.chunkSize will not be filled in.
    if (FAILED(NativeWriteHeader(header, newStateFD, pos))) {
        //throw new IOException("NativeWriteHeader failed (error " + err + ")");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

Int32 Padding_Len(
    /* [in] */ Int32 len)
{
    Int32 _len = len % 4;
    return _len == 0 ? _len : 4 - _len;
}

struct chunk_header_v1 {
    Int32 headerSize;
    Int32 version;
    Int32 dataSize; // corresponds to Header.chunkSize
    Int32 nameLength; // not including the NULL terminator, which is not written to the file
};

Int32 CBackupHelperDispatcher::NativeReadHeader(
    /* [in] */ Header* h,
    /* [in] */ IFileDescriptor* fd)
{
    chunk_header_v1 flattenedHeader;
    ssize_t amt;
    String keyPrefix;
    char* buf;

    int _fd;
    fd->GetDescriptor(&_fd);
    amt = read(_fd, &flattenedHeader.headerSize, sizeof(flattenedHeader.headerSize));
    if (amt != sizeof(flattenedHeader.headerSize)) {
        return -1;
    }

    int remainingHeader = flattenedHeader.headerSize - sizeof(flattenedHeader.headerSize);

    if (flattenedHeader.headerSize < (int)sizeof(chunk_header_v1)) {
        ALOGW("Skipping unknown header: %d bytes", flattenedHeader.headerSize);
        if (remainingHeader > 0) {
            lseek(_fd, remainingHeader, SEEK_CUR);
            // >0 means skip this chunk
            return 1;
        }
    }

    amt = read(_fd, &flattenedHeader.version,
            sizeof(chunk_header_v1)-sizeof(flattenedHeader.headerSize));
    if (amt <= 0) {
        ALOGW("Failed reading chunk header");
        return -1;
    }
    remainingHeader -= sizeof(chunk_header_v1)-sizeof(flattenedHeader.headerSize);

    if (flattenedHeader.version != VERSION_1_HEADER) {
        ALOGW("Skipping unknown header version: 0x%08x, %d bytes", flattenedHeader.version,
                flattenedHeader.headerSize);
        if (remainingHeader > 0) {
            lseek(_fd, remainingHeader, SEEK_CUR);
            // >0 means skip this chunk
            return 1;
        }
    }

#if 0
    ALOGD("chunk header:");
    ALOGD("  headerSize=%d", flattenedHeader.headerSize);
    ALOGD("  version=0x%08x", flattenedHeader.version);
    ALOGD("  dataSize=%d", flattenedHeader.dataSize);
    ALOGD("  nameLength=%d", flattenedHeader.nameLength);
#endif

    if (flattenedHeader.dataSize < 0 || flattenedHeader.nameLength < 0 ||
            remainingHeader < flattenedHeader.nameLength) {
        ALOGW("Malformed V1 header remainingHeader=%d dataSize=%d nameLength=%d", remainingHeader,
                flattenedHeader.dataSize, flattenedHeader.nameLength);
        return -1;
    }

    buf = (char*)malloc(flattenedHeader.nameLength);
    if (buf == NULL) {
        ALOGW("unable to allocate %d bytes", flattenedHeader.nameLength);
        return -1;
    }

    amt = read(_fd, buf, flattenedHeader.nameLength);
    buf[flattenedHeader.nameLength] = 0;

    remainingHeader -= flattenedHeader.nameLength;

    if (remainingHeader > 0) {
        lseek(_fd, remainingHeader, SEEK_CUR);
    }

    h->chunkSize = flattenedHeader.dataSize;
    h->keyPrefix = String(buf);

    if (buf != NULL) free(buf);
    return 0;
}

Int32 CBackupHelperDispatcher::NativeSkipChunk(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 bytesToSkip)
{
    int _fd;
    fd->GetDescriptor(&_fd);
    lseek(_fd, bytesToSkip, SEEK_CUR);
    return 0;
}

Int32 CBackupHelperDispatcher::NativeAllocateHeader(
    /* [in] */ Header* h,
    /* [in] */ IFileDescriptor* fd)
{
    int pos;
    String nameObj;
    int nameLength;
    int namePadding;
    int headerSize;

    nameObj = h->keyPrefix;

    nameLength = nameObj.GetLength();
    namePadding = Padding_Len(nameLength);

    headerSize = sizeof(chunk_header_v1) + nameLength + namePadding;

    int _fd;
    fd->GetDescriptor(&_fd);
    pos = lseek(_fd, 0, SEEK_CUR);

    lseek(_fd, headerSize, SEEK_CUR);

    return pos;
}

Int32 CBackupHelperDispatcher::NativeWriteHeader(
    /* [in] */ Header* h,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 pos)
{
    int err;
    chunk_header_v1 header;
    int namePadding;
    int prevPos;
    String nameObj;
    const char* buf;

    int _fd;
    fd->GetDescriptor(&_fd);
    prevPos = lseek(_fd, 0, SEEK_CUR);

    nameObj = h->keyPrefix;
    header.nameLength = nameObj.GetLength();
    namePadding = Padding_Len(header.nameLength);

    header.headerSize = sizeof(chunk_header_v1) + header.nameLength + namePadding;
    header.version = VERSION_1_HEADER;
    header.dataSize = prevPos - (pos + header.headerSize);

    lseek(_fd, pos, SEEK_SET);
    err = write(_fd, &header, sizeof(chunk_header_v1));
    if (err != sizeof(chunk_header_v1)) {
        return errno;
    }

    buf = nameObj.string();
    err = write(_fd, buf, header.nameLength);
    if (err != header.nameLength) {
        return errno;
    }

    if (namePadding != 0) {
        int zero = 0;
        err = write(_fd, &zero, namePadding);
        if (err != namePadding) {
            return errno;
        }
    }

    lseek(_fd, prevPos, SEEK_SET);
    return 0;
}

} //namespace Backup
} //namespace App
} //namespace Droid
} //namespace Elastos
