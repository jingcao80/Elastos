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

#include "Math.h"
#include "FileURLConnection.h"
#include "UriCodec.h"
#include "CFile.h"
#include "CBufferedInputStream.h"
#include "CFileInputStream.h"
#include "CPrintStream.h"
#include "CByteArrayInputStream.h"
#include "CByteArrayOutputStream.h"
#include "StringUtils.h"
#include "CTreeMap.h"
#include "CString.h"
#include "Collections.h"
#include "CFilePermission.h"

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::EIID_IComparator;
using Elastos::IO::CFile;
using Elastos::IO::CFilePermission;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IPrintStream;
using Elastos::IO::CPrintStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::ITreeMap;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::IList;
using Elastos::Utility::Collections;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FileURLConnection::Comparator, Object, IComparator)

ECode FileURLConnection::Comparator::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (a == b) {
        *result = 0;
        return NOERROR;
    } else if (a == NULL) {
        *result = -1;
        return NOERROR;
    } else if (b == NULL) {
        *result = 1;
        return NOERROR;
    }

    String as = Object::ToString(a);
    String bs = Object::ToString(b);
    *result = as.CompareIgnoreCase(bs);
    return NOERROR;
}

CAR_INTERFACE_IMPL(FileURLConnection, URLConnection, IFileURLConnection)
AutoPtr<IComparator> FileURLConnection::HEADER_COMPARATOR = FileURLConnection::InitComparator();
const Int32 FileURLConnection::CONTENT_TYPE_VALUE_IDX = 1;
const Int32 FileURLConnection::CONTENT_LENGTH_VALUE_IDX = 3;
const Int32 FileURLConnection::LAST_MODIFIED_VALUE_IDX = 5;

AutoPtr<IComparator> FileURLConnection::InitComparator()
{
    return new Comparator();
}

FileURLConnection::FileURLConnection()
    : mLength(-1)
    , mLastModified(-1)
    , mIsDir(FALSE)
{}

ECode FileURLConnection::constructor(
    /* [in] */ IURL* url)
{
    FAIL_RETURN(URLConnection::constructor(url))
    url->GetFile(&mFilename);
    if (mFilename.IsNull()) {
        mFilename = String("");
    }

    mHeaderKeysAndValues = ArrayOf<String>::Alloc(6);
    mHeaderKeysAndValues->Set(0, String("content-type"));
    mHeaderKeysAndValues->Set(1, String(NULL));
    mHeaderKeysAndValues->Set(2, String("content-length"));
    mHeaderKeysAndValues->Set(3, String(NULL));
    mHeaderKeysAndValues->Set(4, String("last-modified"));
    mHeaderKeysAndValues->Set(5, String(NULL));

    return UriCodec::Decode(mFilename, &mFilename);
}

ECode FileURLConnection::Connect()
{
    AutoPtr<IFile> f;
    CFile::New(mFilename, (IFile**)&f);
    ECode error = NOERROR;
    Boolean isDirectory = FALSE;
    if (f->IsDirectory(&isDirectory), isDirectory) {
        mIsDir = true;
        mIs = GetDirectoryListing(f);
        // use -1 for the contentLength
        f->GetLastModified(&mLastModified);
        (*mHeaderKeysAndValues)[CONTENT_TYPE_VALUE_IDX] = "text/html";
    } else {
        // try {
        AutoPtr<IFileInputStream> fs;
        error = CFileInputStream::New(f, (IFileInputStream**)&fs);
        if (error != (ECode)E_IO_EXCEPTION) {
            error = CBufferedInputStream::New(IInputStream::Probe(fs), (IInputStream**)&mIs);
        }
        // } catch (IOException ioe) {
        //     error = ioe;
        // }

        if (error == NOERROR) {
            f->GetLength(&mLength);
            f->GetLastModified(&mLastModified);
            (*mHeaderKeysAndValues)[CONTENT_TYPE_VALUE_IDX] = GetContentTypeForPlainFiles();
        } else {
            (*mHeaderKeysAndValues)[CONTENT_TYPE_VALUE_IDX] = "content/unknown";
        }
    }

    (*mHeaderKeysAndValues)[CONTENT_LENGTH_VALUE_IDX] = StringUtils::ToString(mLength);
    (*mHeaderKeysAndValues)[LAST_MODIFIED_VALUE_IDX] = StringUtils::ToString(mLastModified);

    mConnected = TRUE;
    // if (error != null) {
    //     throw error;
    // }
    return error;
}

ECode FileURLConnection::GetHeaderField(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    if (!mConnected) {
        // try {
        if (FAILED(Connect())) {
            *value = String(NULL);
            return NOERROR;
        }
        // } catch (IOException ioe) {
        //     return null;
        // }
    }

    for (Int32 i = 0; i < mHeaderKeysAndValues->GetLength(); i += 2) {
        if ((*mHeaderKeysAndValues)[i].EqualsIgnoreCase(key)) {
            *value = (*mHeaderKeysAndValues)[i + 1];
            return NOERROR;
        }
    }

    *value = String(NULL);
    return NOERROR;
}

ECode FileURLConnection::GetHeaderFieldKey(
    /* [in] */ Int32 position,
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    if (!mConnected) {
        // try {
        if (FAILED(Connect())) {
            *key = String(NULL);
            return NOERROR;
        }
        // } catch (IOException ioe) {
        //     return null;
        // }
    }

    if (position < 0 || position > mHeaderKeysAndValues->GetLength() / 2) {
        *key = String(NULL);
        return NOERROR;
    }

    *key = (*mHeaderKeysAndValues)[position * 2];
    return NOERROR;
}

ECode FileURLConnection::GetHeaderField(
    /* [in] */ Int32 position,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (!mConnected) {
        // try {
        if (FAILED(Connect())) {
            *value = String(NULL);
            return NOERROR;
        }
        // } catch (IOException ioe) {
        //     return null;
        // }
    }

    if (position < 0 || position > mHeaderKeysAndValues->GetLength() / 2) {
        *value = String(NULL);
        return NOERROR;
    }

    *value = (*mHeaderKeysAndValues)[(position * 2) + 1];
    return NOERROR;
}

ECode FileURLConnection::GetHeaderFields(
    /* [out] */ IMap** headerFields)
{
    VALIDATE_NOT_NULL(headerFields);

    if (mHeaderFields == NULL) {
        AutoPtr<ITreeMap> headerFieldsMap;
        CTreeMap::New(HEADER_COMPARATOR, (ITreeMap**)&headerFieldsMap);

        for (Int32 i = 0; i < mHeaderKeysAndValues->GetLength(); i += 2) {
            AutoPtr<ICharSequence> key;
            CString::New((*mHeaderKeysAndValues)[i], (ICharSequence**)&key);

            AutoPtr<ICharSequence> key2;
            CString::New((*mHeaderKeysAndValues)[i + 1], (ICharSequence**)&key2);
            AutoPtr<IList> list;
            Collections::SingletonList(key2, (IList**)&list);
            IMap::Probe(headerFieldsMap)->Put(key, list);
        }

        Collections::UnmodifiableMap(IMap::Probe(headerFieldsMap), (IMap**)&mHeaderFields);
    }

    *headerFields = mHeaderFields;
    REFCOUNT_ADD(*headerFields);
    return NOERROR;
}

ECode FileURLConnection::GetContentLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)

    mLength = GetContentLengthInt64();
    *length = mLength <= Elastos::Core::Math::INT32_MAX_VALUE ? (Int32) mLength : -1;
    return NOERROR;
}

Int64 FileURLConnection::GetContentLengthInt64()
{
    // try {
    if (!mConnected) {
        Connect();
    }
    // } catch (IOException e) {
    //     // default is -1
    // }
    return mLength;
}

ECode FileURLConnection::GetContentType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    // The content-type header field is always at position 0.
    return GetHeaderField(0, type);
}

String FileURLConnection::GetContentTypeForPlainFiles()
{
    String result;
    mUrl->GetFile(&result);
    result = GuessContentTypeFromName(result);
    if (result != NULL) {
        return result;
    }

    // try {
    result = GuessContentTypeFromStream(mIs);
    // } catch (IOException e) {
    //     // Ignore
    // }
    if (result != NULL) {
        return result;
    }

    return String("content/unknown");
}

AutoPtr<IInputStream> FileURLConnection::GetDirectoryListing(
    /* [in] */ IFile* f)
{
    AutoPtr< ArrayOf<String> > fileList;
    f->List((ArrayOf<String>**)&fileList);
    AutoPtr<IByteArrayOutputStream> bytes;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bytes);
    AutoPtr<IPrintStream> out;
    CPrintStream::New(IOutputStream::Probe(bytes), (IPrintStream**)&out);
    out->Print(String("<title>Directory Listing</title>\n"));
    out->Print(String("<base href=\"file:"));
    String pathname;
    out->Print((f->GetPath(&pathname), pathname).Replace('\\', '/')
        + String("/\"><h1>")
        + (f->GetPath(&pathname), pathname)
        + String("</h1>\n<hr>\n"));
    Int32 i = 0;
    for (i = 0; i < fileList->GetLength(); i++) {
        out->Print((*fileList)[i] + String("<br>\n"));
    }
    ICloseable::Probe(out)->Close();
    AutoPtr<IInputStream> outinput;
    AutoPtr< ArrayOf<Byte> > outbytes;
    bytes->ToByteArray((ArrayOf<Byte>**)&outbytes);
    CByteArrayInputStream::New(outbytes, (IInputStream**)&outinput);

    return outinput;
}

ECode FileURLConnection::GetInputStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is)

    if (!mConnected) {
        Connect();
    }
    *is = mIs;
    REFCOUNT_ADD(*is)
    return NOERROR;
}

ECode FileURLConnection::GetPermission(
    /* [out] */ IPermission** permission)
{
    VALIDATE_NOT_NULL(permission);
    if (mPermission == NULL) {
        String path = mFilename;
        if (CFile::sSeparatorChar != '/') {
            path = path.Replace('/', CFile::sSeparatorChar);
        }
        CFilePermission::New(path, String("read"), (IFilePermission**)&mPermission);
    }

    *permission = IPermission::Probe(mPermission);
    REFCOUNT_ADD(*permission);
    return NOERROR;
}

} // namespace Url
} // namespace Net
} // namespace Libcore
