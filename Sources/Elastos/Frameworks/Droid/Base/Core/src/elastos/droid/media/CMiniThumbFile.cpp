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

#include "elastos/droid/media/CMiniThumbFile.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Environment;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CFile;
using Elastos::IO::Channels::IFileLock;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

/*static*/ const String CMiniThumbFile::TAG("MiniThumbFile");

/*static*/ HashMap< String, AutoPtr<IMiniThumbFile> > CMiniThumbFile::sThumbFiles;

/*static*/ Object CMiniThumbFile::mStaticThislock;

const Int32 CMiniThumbFile::MINI_THUMB_DATA_FILE_VERSION = 3;
const Int32 CMiniThumbFile::HEADER_SIZE = 1 + 8 + 4;

CAR_OBJECT_IMPL(CMiniThumbFile)

CAR_INTERFACE_IMPL(CMiniThumbFile, Object, IMiniThumbFile)

CMiniThumbFile::CMiniThumbFile()
{}

ECode CMiniThumbFile::constructor(
    /* [in] */ IUri* uri)
{
    mUri = uri;
    AutoPtr<IByteBufferHelper> byteBufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteBufferHelper);
    byteBufferHelper->AllocateDirect(BYTES_PER_MINTHUMB, (IByteBuffer**)&mBuffer);
    return NOERROR;
}

/*synchronized*/
ECode CMiniThumbFile::Deactivate()
{
    AutoLock lock(mThislock);

    if (mMiniThumbFile != NULL) {
//        try {
            ICloseable::Probe(mMiniThumbFile)->Close();
            mMiniThumbFile = NULL;
//        } catch (IOException ex) {
//            // ignore exception
//        }
    }
    return NOERROR;
}

/*synchronized*/
ECode CMiniThumbFile::GetMagic(
    /* [in] */ Int64 id,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThislock);
    // check the mini thumb file for the right data.  Right is
    // defined as having the right magic number at the offset
    // reserved for this "id".
    AutoPtr<IRandomAccessFile> r = MiniThumbDataFile();
    if (r != NULL) {
        Int64 pos = id * BYTES_PER_MINTHUMB;
        AutoPtr<IFileLock> fileLock = NULL;
//        try {
            IBuffer::Probe(mBuffer)->Clear();
            IBuffer::Probe(mBuffer)->SetLimit(1 + 8);

            mChannel->Lock(pos, 1 + 8, TRUE, (IFileLock**)&fileLock);
            // check that we can read the following 9 bytes
            // (1 for the "status" and 8 for the long)
            Int32 tempValue1;
            mChannel->Read(mBuffer.Get(), pos, &tempValue1);
            if (tempValue1 == 9) {
                IBuffer::Probe(mBuffer)->SetPosition(0);
                Byte tempValue2;
                mBuffer->Get(&tempValue2);
                if (tempValue2 == 1) {
                    mBuffer->GetInt64(result);
                    return NOERROR;
                }
            }
//        } catch (IOException ex) {
//            Logger::V(TAG, "Got exception checking file magic: "/*, ex*/);
//        } catch (RuntimeException ex) {
//            // Other NIO related exception like disk full, read only channel..etc
//            Logger::E(TAG, String("Got exception when reading magic, id = ") + StringUtils::Int64ToString(id) +
//                    String(", disk full or mount read-only? ") /*+ ex.getClass()*/);
//        } finally {
//            try {
                if (fileLock != NULL) {
                    fileLock->ReleaseLock();
                }
//            }
//            catch (IOException ex) {
//                // ignore it.
//            }
//        }
    }
    *result = 0;
    return NOERROR;
}

/*synchronized*/
ECode CMiniThumbFile::SaveMiniThumbToFile( // throws IOException
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int64 id,
    /* [in] */ Int64 magic)
{
    AutoLock lock(mThislock);

    AutoPtr<IRandomAccessFile> r = MiniThumbDataFile();
    if (r == NULL) {
        return NOERROR;
    }

    Int64 pos = id * BYTES_PER_MINTHUMB;
    AutoPtr<IFileLock> fileLock;
//    try {
        if (data != NULL) {
            if (data->GetLength() > BYTES_PER_MINTHUMB - HEADER_SIZE) {
                // not enough space to store it.
                return NOERROR;
            }
            IBuffer::Probe(mBuffer)->Clear();
            mBuffer->Put((Byte) 1);
            mBuffer->PutInt64(magic);
            mBuffer->PutInt32(data->GetLength());
            mBuffer->Put(data);
            IBuffer::Probe(mBuffer)->Flip();

            mChannel->Lock(pos, BYTES_PER_MINTHUMB, FALSE, (IFileLock**)&fileLock);
            Int32 tempValue;
            mChannel->Write(mBuffer.Get(), pos, &tempValue);
        }
//    } catch (IOException ex) {
//        Logger::E(TAG, String("couldn't save mini thumbnail data for ")
//                + StringUtils::Int64ToString(id) + String("; ")/*, ex*/);
//        throw ex;
//    } catch (RuntimeException ex) {
//        // Other NIO related exception like disk full, read only channel..etc
//        Logger::E(TAG, String("couldn't save mini thumbnail data for ")
//                + StringUtils::Int64ToString(id) + String("; disk full or mount read-only? ") /*+ ex.getClass()*/);
//    } finally {
//        try {
            if (fileLock != NULL) {
                fileLock->ReleaseLock();
            }
//        }
//        catch (IOException ex) {
//            // ignore it.
//        }
//    }
    return NOERROR;
}

/*synchronized*/
ECode CMiniThumbFile::GetMiniThumbFromFile(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mThislock);
    AutoPtr<IRandomAccessFile> r = MiniThumbDataFile();
    if (r == NULL) {
        return NOERROR;
    }

    Int64 pos = id * BYTES_PER_MINTHUMB;
    AutoPtr<IFileLock> fileLock = NULL;
//    try {
        IBuffer::Probe(mBuffer)->Clear();
        mChannel->Lock(pos, BYTES_PER_MINTHUMB, TRUE, (IFileLock**)&fileLock);
        Int32 size;
        mChannel->Read(mBuffer.Get(), pos, &size);
        if (size > 1 + 8 + 4) { // flag, magic, length
            IBuffer::Probe(mBuffer)->SetPosition(0);
            Byte flag;
            mBuffer->Get(&flag);
            Int64 magic;
            mBuffer->GetInt64(&magic);
            Int32 length;
            mBuffer->GetInt32(&length);

            if (size >= 1 + 8 + 4 + length && data->GetLength() >= length) {
                mBuffer->Get(data, 0, length);
                *result = data;
            }
        }
//    } catch (IOException ex) {
//        Logger::W(TAG, String("got exception when reading thumbnail id=") + StringUtils::Int64ToString(id) + String(", exception: ") /*+ ex*/);
//    } catch (RuntimeException ex) {
//        // Other NIO related exception like disk full, read only channel..etc
//        Logger::W(TAG, String("Got exception when reading thumbnail, id = ") + StringUtils::Int64ToString(id) +
//                String(", disk full or mount read-only? ") /*+ ex.getClass()*/);
//    } finally {
//        try {
            if (fileLock != NULL) {
                fileLock->ReleaseLock();
            }
//        }
//        catch (IOException ex) {
//            // ignore it.
//        }
//    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static synchronized*/
ECode CMiniThumbFile::Reset()
{
    AutoLock lock(mStaticThislock);

    AutoPtr<IMiniThumbFile> file;
    HashMap< String, AutoPtr<IMiniThumbFile> >::Iterator it = sThumbFiles.Begin();

    for (; it != sThumbFiles.End(); ++it) {
        file = it->mSecond;
        file->Deactivate();
    }
    sThumbFiles.Clear();
    return NOERROR;
}

/*static synchronized*/
ECode CMiniThumbFile::Instance(
    /* [in] */ IUri* uri,
    /* [out] */ IMiniThumbFile** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mStaticThislock);

    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    list->Get(1, (IInterface**)&obj);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
    String type;
    cs->ToString(&type);

    HashMap< String, AutoPtr<IMiniThumbFile> >::Iterator it = sThumbFiles.Find(type);
    AutoPtr<IMiniThumbFile> file = it != sThumbFiles.End() ? it->mSecond : NULL;
    // Logger::V(TAG, String("get minithumbfile for type: ") + type);
    if (file == NULL) {
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        AutoPtr<IUri> tempUri;
        uriHelper->Parse(String("content://media/external/") + type + String("/media"), (IUri**)&tempUri);
        AutoPtr<CMiniThumbFile> mtf;
        CMiniThumbFile::NewByFriend(tempUri, (CMiniThumbFile**)&mtf);
        file = (CMiniThumbFile*)mtf.Get();
        sThumbFiles[type] = file;
    }

    *result = file;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String CMiniThumbFile::RandomAccessFilePath(
    /* [in] */ Int32 version)
{
    String tempText;
    AutoPtr<IFile> file = Environment::GetExternalStorageDirectory();
    file->ToString(&tempText);
    String directoryName = tempText + "/DCIM/.thumbnails";

    Int32 tempValue = 0;
    IObject* obj = IObject::Probe(mUri.Get());
    if (obj != NULL) {
        obj->GetHashCode(&tempValue);
    }
    String result = directoryName + "/thumbdata" + StringUtils::ToString(version) + "-" + StringUtils::ToString(tempValue);
    return result;
}

void CMiniThumbFile::RemoveOldFile()
{
    String oldPath = RandomAccessFilePath(MINI_THUMB_DATA_FILE_VERSION - 1);
    AutoPtr<IFile> oldFile ;
    CFile::New(oldPath,(IFile**)&oldFile);

    Boolean tempState;
    oldFile->Exists(&tempState);
    if (tempState) {
//        try {
            oldFile->Delete(&tempState);
//        } catch (SecurityException ex) {
//            // ignore
//        }
    }
}

AutoPtr<IRandomAccessFile> CMiniThumbFile::MiniThumbDataFile()
{
    if (mMiniThumbFile == NULL) {
        RemoveOldFile();
        String path = RandomAccessFilePath(MINI_THUMB_DATA_FILE_VERSION);

        AutoPtr<IFile> dir, directory;
        CFile::New(path, (IFile**)&dir);
        dir->GetParentFile((IFile**)&directory);

        Boolean tempState;
        directory->IsDirectory(&tempState);
        if (!tempState) {
            directory->Mkdirs(&tempState);
            if (!tempState) {
                String tempText;
                directory->ToString(&tempText);
                Logger::E(TAG, String("Unable to create .thumbnails directory ") + tempText);
            }
        }
        AutoPtr<IFile> f;
        CFile::New(path,(IFile**)&f);
        ECode ec;
//        try {
            ec = CRandomAccessFile::New(f, String("rw"), (IRandomAccessFile**)&mMiniThumbFile);
//        } catch (IOException ex) {
//            // Open as read-only so we can at least read the existing
//            // thumbnails.
//            try {
                if(FAILED(ec)) {
                    mMiniThumbFile = NULL;
                    CRandomAccessFile::New(f, String("r"), (IRandomAccessFile**)&mMiniThumbFile);
                }
//            } catch (IOException ex2) {
//                // ignore exception
//            }
//        }

        if (mMiniThumbFile != NULL) {
            mChannel = NULL;
            mMiniThumbFile->GetChannel((IFileChannel**)&mChannel);
        }
    }
    return mMiniThumbFile;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
