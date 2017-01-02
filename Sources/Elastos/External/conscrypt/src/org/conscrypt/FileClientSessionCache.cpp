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

#include "org/conscrypt/FileClientSessionCache.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::ITreeSet;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

static AutoPtr<IMap> InitCaches()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return IMap::Probe(map);
}

AutoPtr<IMap/*<File, FileClientSessionCache.Impl>*/> FileClientSessionCache::caches = InitCaches();

//==============================================================================
//  FileClientSessionCache::Impl
//==============================================================================

CAR_INTERFACE_IMPL(FileClientSessionCache::Impl, Object, ISSLClientSessionCache)

FileClientSessionCache::Impl::Impl(
    /* [in] */ IFile* directory)
{
    mAccessOrder = NewAccessOrder();

    Boolean exists;
    directory->Exists(&exists);
    Boolean b;
    if (exists && (directory->IsDirectory(&b), !b)) {
        // throw new IOException(directory + " exists but is not a directory.");
        return;
    }

    if (exists) {
        // Read and sort initial list of files. We defer adding
        // information about these files to accessOrder until necessary
        // (see indexFiles()). Sorting the list enables us to detect
        // cache misses in getSessionData().
        // Note: Sorting an array here was faster than creating a
        // HashSet on Dalvik.
        directory->List((ArrayOf<String>**)&mInitialFiles);
        if (mInitialFiles == NULL) {
            // File.list() will return NULL in error cases without throwing IOException
            // http://b/3363561
            // throw new IOException(directory + " exists but cannot list contents.");
            return;
        }
// TODO: Need Arrays::Sort(ArrayOf<String>)
        // Arrays::Sort(mInitialFiles);
        mSize = mInitialFiles->GetLength();
    }
    else {
        // Create directory.
        Boolean b;
        if (directory->Mkdirs(&b), !b) {
            // throw new IOException("Creation of " + directory + " directory failed.");
            return;
        }
        mSize = 0;
    }

    mDirectory = directory;
}

ECode FileClientSessionCache::Impl::GetSessionData(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * Note: This method is only called when the in-memory cache
     * in SSLSessionContext misses, so it would be unnecessarily
     * redundant for this cache to store data in memory.
     */

    String name;
    FileName(host, port, &name);
    AutoPtr<IInterface> obj;
    mAccessOrder->Get(CoreUtils::Convert(name), (IInterface**)&obj);
    AutoPtr<IFile> file = IFile::Probe(obj);

    if (file == NULL) {
        // File wasn't in access order. Check initialFiles...
        if (mInitialFiles == NULL) {
            // All files are in accessOrder, so it doesn't exist.
            *result = NULL;
            return NOERROR;
        }

        // Look in initialFiles.
        Int32 index;
        Arrays::BinarySearch(mInitialFiles, name, &index);
        if (index < 0) {
            // Not found.
            *result = NULL;
            return NOERROR;
        }

        // The file is on disk but not in accessOrder yet.
        CFile::New(mDirectory, name, (IFile**)&file);
        mAccessOrder->Put(CoreUtils::Convert(name), file);
    }

    AutoPtr<IFileInputStream> in;
    // try {
    CFileInputStream::New(file, (IFileInputStream**)&in);
    // } catch (FileNotFoundException e) {
    //     logReadError(host, file, e);
    //     *result = NULL;
    //     return NOERROR;
    // }
    // try {
    Int64 size;
    file->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc((Int32)size);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(IInputStream::Probe(in), (IDataInputStream**)&dis);
    IDataInput::Probe(dis)->ReadFully(data);

    // } catch (IOException e) {
    //     logReadError(host, file, e);
    //     return NULL;
    // } finally {
    if (in != NULL) {
        // try {
        ICloseable::Probe(in)->Close();
        // } catch (RuntimeException rethrown) {
        //     throw rethrown;
        // } catch (Exception ignored) {
        // }
    }
    // }
    *result = data;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode FileClientSessionCache::Impl::LogReadError(
    /* [in] */ const String& host,
    /* [in] */ IFile* file,
    /* [in] */ IThrowable* t)
{
    String str;
    IObject::Probe(file)->ToString(&str);
    Logger::E("FileClientSessionCache::Impl", "FileClientSessionCache: Error reading session data for %s from %s."
            , host.string(), str.string());
    // t->PrintStackTrace();
    return NOERROR;
}

ECode FileClientSessionCache::Impl::PutSessionData(
    /* [in] */ ISSLSession* session,
    /* [in] */ ArrayOf<Byte>* sessionData)
{
    String host;
    session->GetPeerHost(&host);
    if (sessionData == NULL) {
        // throw new NULLPointerException("sessionData == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 port;
    session->GetPeerPort(&port);
    String name;
    FileName(host, port, &name);
    AutoPtr<IFile> file;
    CFile::New(mDirectory, name, (IFile**)&file);

    // Used to keep track of whether or not we're expanding the cache.
    Boolean existedBefore;
    file->Exists(&existedBefore);

    AutoPtr<IFileOutputStream> out;
    // try {
    CFileOutputStream::New(file, (IFileOutputStream**)&out);
    // } catch (FileNotFoundException e) {
    //     // We can't write to the file.
    //     logWriteError(host, file, e);
    //     return;
    // }

    // If we expanded the cache (by creating a new file)...
    if (!existedBefore) {
        mSize++;

        // Delete an old file if necessary.
        MakeRoom();
    }

    Boolean writeSuccessful = FALSE;
    // try {
    IOutputStream::Probe(out)->Write(sessionData);
    writeSuccessful = TRUE;
    // } catch (IOException e) {
    //     logWriteError(host, file, e);
    // } finally {
    Boolean closeSuccessful = FALSE;
    // try {
    ICloseable::Probe(out)->Close();
    closeSuccessful = TRUE;
    // } catch (IOException e) {
    //     logWriteError(host, file, e);
    // } finally {
    if (!writeSuccessful || !closeSuccessful) {
        // Storage failed. Clean up.
        Delete(file);
    }
    else {
        // Success!
        mAccessOrder->Put(CoreUtils::Convert(name), file);
    }
    // }
    // }
    return NOERROR;
}

ECode FileClientSessionCache::Impl::LogWriteError(
    /* [in] */ const String& host,
    /* [in] */ IFile* file,
    /* [in] */ IThrowable* t)
{
    // System.err.println("FileClientSessionCache: Error writing session data for " + host + " to " + file + ".");
    String str;
    IObject::Probe(file)->ToString(&str);
    Logger::E("FileClientSessionCache::Impl", "FileClientSessionCache: Error writing session data for %s to %s.", host.string(), str.string());
    // t->PrintStackTrace();
    return NOERROR;
}

AutoPtr<IMap/*<String, File>*/> FileClientSessionCache::Impl::NewAccessOrder()
{
    AutoPtr<ILinkedHashMap> map;
    CLinkedHashMap::New(IFileClientSessionCache::MAX_SIZE, 0.75, TRUE /* access order */,
            (ILinkedHashMap**)&map);
    return IMap::Probe(map);
}

ECode FileClientSessionCache::Impl::FileName(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (host == NULL) {
        // throw new NULLPointerException("host == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = host + "." + port;
    return NOERROR;
}

void FileClientSessionCache::Impl::MakeRoom()
{
    if (mSize <= IFileClientSessionCache::MAX_SIZE) {
        return;
    }

    IndexFiles();

    // Delete LRUed files.
    Int32 removals = mSize - IFileClientSessionCache::MAX_SIZE;
    AutoPtr<ICollection> collection;
    mAccessOrder->GetValues((ICollection**)&collection);
    AutoPtr<IIterator> i;
    collection->GetIterator((IIterator**)&i);

    do {
        AutoPtr<IInterface> obj;
        i->GetNext((IInterface**)&obj);
        Delete(IFile::Probe(obj));
        i->Remove();
    } while (--removals > 0);
}

void FileClientSessionCache::Impl::IndexFiles()
{
    AutoPtr<ArrayOf<String> > initialFiles = mInitialFiles;
    if (initialFiles != NULL) {
        mInitialFiles = NULL;

        // Files on disk only, sorted by last modified time.
        // TODO: Use last access time.
        AutoPtr<ISet/*<CacheFile>*/> diskOnly;
        CTreeSet::New((ISet**)&diskOnly);

        Boolean b;
        for (Int32 i = 0; i < initialFiles->GetLength(); i++) {
            String name = (*initialFiles)[i];
            // If the file hasn't been accessed in this process...
            if (mAccessOrder->ContainsKey(CoreUtils::Convert(name), &b), !b) {
                AutoPtr<CacheFile> cf = new CacheFile(mDirectory, name);
                diskOnly->Add((IInterface*)(IObject*)cf.Get());
            }
        }

        if (diskOnly->IsEmpty(&b), !b) {
            // Add files not accessed in this process to the beginning
            // of accessOrder.
            AutoPtr<IMap/*<String, File>*/> newOrder = NewAccessOrder();

            AutoPtr<ArrayOf<IInterface*> > array;
            diskOnly->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); ++i) {
                AutoPtr<CacheFile> cacheFile = (CacheFile*)(IObject*)(*array)[i];
                newOrder->Put(CoreUtils::Convert(cacheFile->mName),
                        (IInterface*)(IObject*)cacheFile.Get());
            }
            newOrder->PutAll(mAccessOrder);

            mAccessOrder = newOrder;
        }
    }
}

ECode FileClientSessionCache::Impl::Delete(
    /* [in] */ IFile* file)
{
    Boolean b;
    if (file->Delete(&b), !b) {
        // new IOException("FileClientSessionCache: Failed to delete " + file + ".").printStackTrace();
        return E_IO_EXCEPTION;
    }
    mSize--;
    return NOERROR;
}

//==============================================================================
//  FileClientSessionCache::CacheFile
//==============================================================================

FileClientSessionCache::CacheFile::CacheFile(
    /* [in] */ IFile* dir,
    /* [in] */ const String& name)
    : mName(name)
    , mLastModified(-1)
{
    File::constructor(dir, name);
}

ECode FileClientSessionCache::CacheFile::LastModified(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 lastModified = mLastModified;
    if (lastModified == -1) {
        File::GetLastModified(&mLastModified);
        lastModified = mLastModified;
    }
    *result = lastModified;
    return NOERROR;
}

ECode FileClientSessionCache::CacheFile::CompareTo(
    /* [in] */ IFile* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // Sort by last modified time.
    Int64 lm;
    another->GetLastModified(&lm);
    Int64 val;
    LastModified(&val);
    Int64 ret = val - lm;
    if (ret == 0) {
        return File::CompareTo(another, result);
    }
    *result = ret < 0 ? -1 : 1;
    return NOERROR;
}

//==============================================================================
//  FileClientSessionCache
//==============================================================================

ECode FileClientSessionCache::UsingDirectory(
    /* [in] */ IFile* directory,
    /* [out] */ ISSLClientSessionCache** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    caches->Get(directory, (IInterface**)&obj);
    AutoPtr<Impl> cache = (Impl*)(IObject*)obj.Get();
    if (cache == NULL) {
        cache = new Impl(directory);
        caches->Put(directory, (IInterface*)(IObject*)cache.Get());
    }
    *result = ISSLClientSessionCache::Probe(cache);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/** For testing. */
ECode FileClientSessionCache::Reset()
{
    return caches->Clear();
}

} // namespace Conscrypt
} // namespace Org
