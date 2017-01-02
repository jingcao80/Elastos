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

#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FILEURLCONNECTION_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FILEURLCONNECTION_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "URLConnection.h"

using Elastos::Core::IComparator;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::URLConnection;
using Elastos::IO::IInputStream;
using Elastos::IO::IFilePermission;
using Elastos::IO::IFile;

namespace Libcore {
namespace Net {
namespace Url {

/**
 * This subclass extends <code>URLConnection</code>.
 * <p>
 * This class is responsible for connecting, getting content and input stream of
 * the file.
 */
class FileURLConnection
    : public URLConnection
    , public IFileURLConnection
{
private:
    class Comparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* a,
            /* [in] */ IInterface* b,
            /* [out] */ Int32* result);
};

public:
    CAR_INTERFACE_DECL()

    FileURLConnection();

    /**
     * Creates an instance of <code>FileURLConnection</code> for establishing
     * a connection to the file pointed by this <code>URL<code>
     *
     * @param url The URL this connection is connected to
     */
    CARAPI constructor(
        /* [in] */ IURL* url);

    /**
     * This methods will attempt to obtain the input stream of the file pointed
     * by this <code>URL</code>. If the file is a directory, it will return
     * that directory listing as an input stream.
     *
     * @throws IOException
     *             if an IO error occurs while connecting
     */
    CARAPI Connect();

    CARAPI GetHeaderField(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    CARAPI GetHeaderFieldKey(
        /* [in] */ Int32 posn,
        /* [out] */ String* key);

    CARAPI GetHeaderField(
        /* [in] */ Int32 pos,
        /* [out] */ String* value);

    virtual CARAPI GetHeaderFields(
        /* [out] */ IMap** headerFields);

    /**
     * Returns the length of the file in bytes, or {@code -1} if the length cannot be
     * represented as an {@code int}. See {@link #getContentLengthLong()} for a method that can
     * handle larger files.
     */
    CARAPI GetContentLength(
        /* [out] */ Int32* length);

    /**
     * Returns the content type of the resource. Just takes a guess based on the
     * name.
     *
     * @return the content type
     */
    CARAPI GetContentType(
        /* [out] */ String* type);


    /**
     * Returns the input stream of the object referred to by this
     * <code>URLConnection</code>
     *
     * File Sample : "/ZIP211/+/harmony/tools/javac/resources/javac.properties"
     * Invalid File Sample:
     * "/ZIP/+/harmony/tools/javac/resources/javac.properties"
     * "ZIP211/+/harmony/tools/javac/resources/javac.properties"
     *
     * @return input stream of the object
     *
     * @throws IOException
     *             if an IO error occurs
     */
    CARAPI GetInputStream(
        /* [out] */ IInputStream** is);

    /**
     * Returns the permission, in this case the subclass, FilePermission object
     * which represents the permission necessary for this URLConnection to
     * establish the connection.
     *
     * @return the permission required for this URLConnection.
     *
     * @throws IOException
     *             if an IO exception occurs while creating the permission.
     */
    CARAPI GetPermission(
        /* [out] */ IPermission** permission);

private:
    CARAPI_(AutoPtr<IInputStream>) GetDirectoryListing(
        /* [in] */ IFile* f);

    CARAPI_(String) GetContentTypeForPlainFiles();

    /**
     * Returns the length of the file in bytes.
     */
    CARAPI_(Int64) GetContentLengthInt64();

    static CARAPI_(AutoPtr<IComparator>) InitComparator();

private:
    String mFilename;

    AutoPtr<IInputStream> mIs;

    Int64 mLength; // = -1;
    Int64 mLastModified/* = -1*/;

    Boolean mIsDir;

    AutoPtr<IFilePermission> mPermission;
    /**
     * A set of three key value pairs representing the headers we support.
     */
    AutoPtr<ArrayOf<String> > mHeaderKeysAndValues;

    static const Int32 CONTENT_TYPE_VALUE_IDX;
    static const Int32 CONTENT_LENGTH_VALUE_IDX;
    static const Int32 LAST_MODIFIED_VALUE_IDX;

    AutoPtr<IMap> mHeaderFields;

    static AutoPtr<IComparator> HEADER_COMPARATOR;
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FILEURLCONNECTION_H__
