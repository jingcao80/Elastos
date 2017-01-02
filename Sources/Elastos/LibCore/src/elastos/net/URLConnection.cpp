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

#include "Elastos.CoreLibrary.IO.h"
#include "URLConnection.h"
#include "DefaultFileNameMap.h"
#include "CDateHelper.h"
#include "CSystem.h"
#include "Character.h"
#include "StringBuffer.h"
#include "StringUtils.h"
#include "Collections.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Core::ISystem;
using Elastos::Utility::Collections;
using Elastos::Utility::IDateHelper;
using Elastos::Utility::CDateHelper;

namespace Elastos {
namespace Net {

//=============================================================
// URLConnection::DefaultContentHandler
//=============================================================
Object URLConnection::sLock;

CAR_INTERFACE_IMPL(URLConnection::DefaultContentHandler, Object, IContentHandler)

ECode URLConnection::DefaultContentHandler::GetContent(
    /* [in] */ IURLConnection* uConn,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(uConn)
    AutoPtr<IInputStream> o;
    FAIL_RETURN(uConn->GetInputStream((IInputStream**)&o))
    *obj = o.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode URLConnection::DefaultContentHandler::GetContent(
    /* [in] */ IURLConnection* uConn,
    /* [in] */ ArrayOf<InterfaceID> * types,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(types)

    FAIL_RETURN(GetContent(uConn, obj))
    for (Int32 i =0; i < types->GetLength(); ++i) {
        if ((*obj)->Probe((*types)[i]) != NULL) {
            return NOERROR;
        }
    }

    return NOERROR;
}

//=============================================================
// URLConnection
//=============================================================

CAR_INTERFACE_IMPL(URLConnection, Object, IURLConnection)

Boolean URLConnection::sDefaultAllowUserInteraction = FALSE;
Boolean URLConnection::sDefaultUseCaches = TRUE;
HashMap<String, AutoPtr<IContentHandler> > URLConnection::sContentHandlers;
AutoPtr<IContentHandlerFactory> URLConnection::sContentHandlerFactory;
AutoPtr<IFileNameMap> URLConnection::sFileNameMap;

URLConnection::URLConnection()
    : mIfModifiedSince(0)
    , mUseCaches(sDefaultUseCaches)
    , mConnected(FALSE)
    , mDoOutput(FALSE)
    , mDoInput(TRUE)
    , mAllowUserInteraction(sDefaultAllowUserInteraction)
    , mLastModified(-1)
    , mReadTimeout(0)
    , mConnectTimeout(0)
{
    mDefaultHandler = new DefaultContentHandler();
}

URLConnection::~URLConnection()
{}

ECode URLConnection::constructor(
    /* [in] */ IURL* url)
{
    mUrl = url;
    return NOERROR;
}

ECode URLConnection::GetAllowUserInteraction(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mAllowUserInteraction;
    return NOERROR;
}

ECode URLConnection::GetContent(
    /* [out] */ IInterface** content)
{
    VALIDATE_NOT_NULL(content)

    if (!mConnected) {
        Connect();
    }

    GetContentType(&mContentType);
    if (mContentType.IsNull()) {
        String fileName;
        mUrl->GetFile(&fileName);
        mContentType = GuessContentTypeFromName(fileName);
        if (mContentType.IsNull()) {
            AutoPtr<IInputStream> inputStream;
            GetInputStream((IInputStream**)&inputStream);
            mContentType = GuessContentTypeFromStream(inputStream);
        }
    }
    if (!mContentType.IsNull()) {
        AutoPtr<IContentHandler> handler;
        GetContentHandler(mContentType, (IContentHandler**)&handler);
        return handler->GetContent(this, content);
    }

    *content = NULL;
    return NOERROR;
}

ECode URLConnection::GetContent(
    /* [in] */ ArrayOf<InterfaceID> * types,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(types)

    if (!mConnected) {
        Connect();
    }

    GetContentType(&mContentType);
    if (mContentType.IsNull()) {
        String fileName;
        mUrl->GetFile(&fileName);
        mContentType = GuessContentTypeFromName(fileName);
        if (mContentType.IsNull()) {
            AutoPtr<IInputStream> inputStream;
            GetInputStream((IInputStream**)&inputStream);
            mContentType = GuessContentTypeFromStream(inputStream);
        }
    }

    if (!mContentType.IsNull()) {
        AutoPtr<IContentHandler> handler;
        GetContentHandler(mContentType, (IContentHandler**)&handler);

        return handler->GetContent(this, types, obj);
    }

    return NOERROR;
}

ECode URLConnection::GetContentEncoding(
    /* [out] */ String* encoding)
{
    return GetHeaderFieldByKey(String("Content-Encoding"), encoding);
}

ECode URLConnection::GetContentHandler(
    /* [in] */ const String& type,
    /* [out] */ IContentHandler** contentHandler)
{
    VALIDATE_NOT_NULL(contentHandler)

    // Replace all non-alphanumeric character by '_'
    String typeString = ParseTypeString(type.Replace('/', '.'));

    // if there's a cached content handler, use it
    HashMap<String, AutoPtr<IContentHandler> >::Iterator it = sContentHandlers.Find(type);
    if (it != sContentHandlers.End()) {
        *contentHandler = it->mSecond;
        REFCOUNT_ADD(*contentHandler);
        return NOERROR;
    }

    AutoPtr<IContentHandler> cHandler;
    if (sContentHandlerFactory != NULL) {
        sContentHandlerFactory->CreateContentHandler(type, (IContentHandler**)&cHandler);
        sContentHandlers[type] = cHandler;
        *contentHandler = cHandler;
        REFCOUNT_ADD(*contentHandler);
        return NOERROR;
    }

    // search through the package list for the right class for the Content
    // Type
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String packageList;
    system->GetEnv(String("java.content.handler.pkgs"), &packageList);

   if (!packageList.IsNull()) {
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split(packageList, String("|"), (ArrayOf<String>**)&values);
        Int32 count = (values ? values->GetLength() : 0);
        for (Int32 i = 0; i < count; ++i) {
            // try {
            String className((*values)[i]);
            className += ".";
            className += typeString;

            assert(0);
            //TODO reflection
            //
               // Class<?> cl = Class.forName((*values)[i] + "."
               //         + typeString, true, ClassLoader
               //         .getSystemClassLoader());
               // cHandler = cl.newInstance();
           // } catch (ClassNotFoundException e) {
           // } catch (IllegalAccessException e) {
           // } catch (InstantiationException e) {
           // }
       }
   }

    if (cHandler == NULL) {
        assert(0);
        // try {
        //     // Try looking up AWT image content handlers
        //     String className = "org.apache.harmony.awt.www.content." + typeString;
        //     cHandler = Class.forName(className).newInstance();
        // } catch (ClassNotFoundException e) {
        // } catch (IllegalAccessException e) {
        // } catch (InstantiationException e) {
        // }
    }
    if (cHandler != NULL) {
        if (IContentHandler::Probe(cHandler) == NULL) {
            // throw new UnknownServiceException();
            return E_UNKNOWN_SERVICE_EXCEPTION;
        }
        sContentHandlers[type] = cHandler; // if we got the handler,
        // cache it for next time
        *contentHandler = cHandler;
        REFCOUNT_ADD(*contentHandler);
        return NOERROR;
    }

    *contentHandler = mDefaultHandler;
    REFCOUNT_ADD(*contentHandler);
    return NOERROR;
}

ECode URLConnection::GetContentLength(
    /* [out] */ Int32* length)
{
    return GetHeaderFieldInt32(String("Content-Length"), -1, length);
}

ECode URLConnection::GetContentType(
    /* [out] */ String* type)
{
    return GetHeaderFieldByKey(String("Content-Type"), type);
}

ECode URLConnection::GetDate(
    /* [out] */ Int64* date)
{
    return GetHeaderFieldDate(String("Date"), 0, date);
}

Boolean URLConnection::GetDefaultAllowUserInteraction()
{
    return sDefaultAllowUserInteraction;
}

String URLConnection::GetDefaultRequestProperty(
    /* [in] */ const String& field)
{
    return String(NULL);
}

ECode URLConnection::GetDefaultUseCaches(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = sDefaultUseCaches;
    return NOERROR;
}

ECode URLConnection::GetDoInput(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    *allowed = mDoInput;
    return NOERROR;
}

ECode URLConnection::GetDoOutput(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    *allowed = mDoOutput;
    return NOERROR;
}

ECode URLConnection::GetExpiration(
    /* [out] */ Int64* expiration)
{
    return GetHeaderFieldDate(String("Expires"), 0, expiration);
}

AutoPtr<IFileNameMap> URLConnection::GetFileNameMap()
{
    {    AutoLock syncLock(sLock);
        if (sFileNameMap == NULL) {
            sFileNameMap = new DefaultFileNameMap();
        }
    }
    return sFileNameMap;
}

ECode URLConnection::GetHeaderField(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    *value = NULL;
    return NOERROR;
}

ECode URLConnection::GetHeaderField(
    /* [in] */ Int32 pos,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    *value = NULL;
    return NOERROR;
}

ECode URLConnection::GetHeaderFields(
    /* [out] */ IMap** headerFields)
{
    VALIDATE_NOT_NULL(headerFields)
    *headerFields = Collections::EMPTY_MAP;
    REFCOUNT_ADD(*headerFields)
    return NOERROR;
}

ECode URLConnection::GetRequestProperties(
    /* [out] */ IMap** properties)
{
    VALIDATE_NOT_NULL(properties)
    *properties = NULL;

    FAIL_RETURN(CheckNotConnected())
    *properties = Collections::EMPTY_MAP;
    REFCOUNT_ADD(*properties)
    return NOERROR;
    return NOERROR;
}

ECode URLConnection::CheckNotConnected()
{
    if (mConnected) {
//        throw new IllegalStateException("Already connected");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode URLConnection::AddRequestProperty(
    /* [in] */ const String& field,
    /* [in] */ const String& newValue)
{
    FAIL_RETURN(CheckNotConnected());
    if (field.IsNull()) {
//        throw new NullPointerException("field == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode URLConnection::GetHeaderFieldByKey(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    *value = NULL;
    return NOERROR;
}

ECode URLConnection::GetHeaderFieldDate(
    /* [in] */ const String& field,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = defaultValue;

    String date;
    FAIL_RETURN(GetHeaderFieldByKey(field, &date));
    if (date.IsNull()) {
        *value = defaultValue;
        return NOERROR;
    }

    AutoPtr<IDateHelper> dateHelper;
    CDateHelper::AcquireSingleton((IDateHelper**)&dateHelper);
    ECode ec = dateHelper->Parse(date, value);
    if (FAILED(ec)) {
        *value = defaultValue;
    }
    return NOERROR;
}

ECode URLConnection::GetHeaderFieldInt32(
    /* [in] */ const String& field,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
//    try {
    String s;
    GetHeaderFieldByKey(field, &s);
    ECode ec = StringUtils::Parse(s, 10, value);
    if (FAILED(ec)) {
        *value = defaultValue;
    }
    return NOERROR;
//    } catch (NumberFormatException e) {
//        return defaultValue;
//    }
}

ECode URLConnection::GetHeaderFieldKey(
    /* [in] */ Int32 posn,
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)

    *key = NULL;
    return NOERROR;
}

ECode URLConnection::GetIfModifiedSince(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)

    *time = mIfModifiedSince;
    return NOERROR;
}

ECode URLConnection::GetInputStream(
    /* [out] */ IInputStream** is)
{
//    throw new UnknownServiceException("Does not support writing to the input stream");
    return E_UNKNOWN_SERVICE_EXCEPTION;
}

ECode URLConnection::GetLastModified(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (mLastModified != -1) {
        *value = mLastModified;
        return NOERROR;
    }
    GetHeaderFieldDate(String("Last-Modified"), 0, &mLastModified);
    *value = mLastModified;
    return NOERROR;
}

ECode URLConnection::GetOutputStream(
    /* [out] */ IOutputStream** os)
{
//    throw new UnknownServiceException("Does not support writing to the output stream");
    return E_UNKNOWN_SERVICE_EXCEPTION;
}

ECode URLConnection::GetPermission(
    /* [out] */ IPermission** perm)
{
    VALIDATE_NOT_NULL(perm);
    *perm = NULL;
    //TODO
    assert(0);
    // return new java.security.AllPermission();
    return NOERROR;
}

ECode URLConnection::GetRequestProperty(
    /* [in] */ const String& field,
    /* [out] */ String* property)
{
    VALIDATE_NOT_NULL(property)

    FAIL_RETURN(CheckNotConnected());
    *property = NULL;
    return NOERROR;
}

ECode URLConnection::GetURL(
    /* [out] */ IURL** url)
{
    VALIDATE_NOT_NULL(url)

    *url = mUrl;
    REFCOUNT_ADD(*url);
    return NOERROR;
}

ECode URLConnection::GetUseCaches(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed)

    *allowed = mUseCaches;
    return NOERROR;
}

String URLConnection::GuessContentTypeFromName(
    /* [in] */ const String& url)
{
    String s;
    GetFileNameMap()->GetContentTypeFor(url, &s);
    return s;
}

String URLConnection::GuessContentTypeFromStream(
    /* [in] */ IInputStream* is)
{
    Boolean supported;
    if (is->IsMarkSupported(&supported), !supported) {
        return String(NULL);
    }
    // Look ahead up to 64 bytes for the longest encoded header
    is->Mark(64);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(64);
    Int32 length;
    is->Read(bytes, &length);
    is->Reset();

    // If there is no data from the input stream, we can't determine content type.
    if (length == -1) {
        return String(NULL);
    }

    // Check for Unicode BOM encoding indicators
    String encoding("US-ASCII");
    Int32 start = 0;
    if (length > 1) {
        if (((*bytes)[0] == (Byte)0xFF) && ((*bytes)[1] == (Byte)0xFE)) {
            encoding = String("UTF-16LE");
            start = 2;
            length -= length & 1;
        }
        if (((*bytes)[0] == (Byte)0xFE) && ((*bytes)[1] == (Byte)0xFF)) {
            encoding = String("UTF-16BE");
            start = 2;
            length -= length & 1;
        }
        if (length > 2) {
            if (((*bytes)[0] == (Byte)0xEF) && ((*bytes)[1] == (Byte)0xBB)
                    && ((*bytes)[2] == (Byte)0xBF)) {
                encoding = String("UTF-8");
                start = 3;
            }
            if (length > 3) {
                if (((*bytes)[0] == (Byte)0x00) && ((*bytes)[1] == (Byte)0x00)
                        && ((*bytes)[2] == (Byte)0xFE)
                        && ((*bytes)[3] == (Byte)0xFF)) {
                    encoding = String("UTF-32BE");
                    start = 4;
                    length -= length & 3;
                }
                if (((*bytes)[0] == (Byte)0xFF) && ((*bytes)[1] == (Byte)0xFE)
                        && ((*bytes)[2] == (Byte)0x00)
                        && ((*bytes)[3] == (Byte)0x00)) {
                    encoding = String("UTF-32LE");
                    start = 4;
                    length -= length & 3;
                }
            }
        }
    }

    String header((const char*)bytes->GetPayload() + start, length - start/*, encoding*/);

    // Check binary types
    if (header.StartWith("PK")) {
        return String("application/zip");
    }
    if (header.StartWith("GI")) {
        return String("image/gif");
    }

    // Check text types
    String textHeader = header.Trim().ToUpperCase();
    if (textHeader.StartWith("<!DOCTYPE HTML") ||
            textHeader.StartWith("<HTML") ||
            textHeader.StartWith("<HEAD") ||
            textHeader.StartWith("<BODY") ||
            textHeader.StartWith("<HEAD")) {
        return String("text/html");
    }

    if (textHeader.StartWith("<?XML")) {
        return String("application/xml");
    }

    // Give up
    return String(NULL);
}

String URLConnection::ParseTypeString(
    /* [in] */ const String& typeString)
{
    StringBuffer typeStringBuffer(typeString);
    for (Int32 i = 0; i < typeStringBuffer.GetByteCount(); i++) {
        // if non-alphanumeric, replace it with '_'
        Char32 c;
        typeStringBuffer.GetCharAt(i, &c);
        if (!(Character::IsLetter(c) || Character::IsDigit(c) || c == '.')) {
            typeStringBuffer.SetCharAt(i, '_');
        }
    }
    String result;
    typeStringBuffer.ToString(&result);
    return result;
}

ECode URLConnection::SetAllowUserInteraction(
    /* [in] */ Boolean newValue)
{
    FAIL_RETURN(CheckNotConnected());
    mAllowUserInteraction = newValue;

    return NOERROR;
}

ECode URLConnection::SetContentHandlerFactory(
    /* [in] */ IContentHandlerFactory* contentFactory)
{
    {    AutoLock syncLock(sLock);
        if (sContentHandlerFactory != NULL) {
            //throw new Error("Factory already set");
            return NOERROR;
        }
        // SecurityManager sManager = System.getSecurityManager();
        // if (sManager != null) {
        //    sManager.checkSetFactory();
        // }
        sContentHandlerFactory = contentFactory;
    }
    return NOERROR;
}

void URLConnection::SetDefaultAllowUserInteraction(
    /* [in] */ Boolean allows)
{
    sDefaultAllowUserInteraction = allows;
}

void URLConnection::SetDefaultRequestProperty(
    /* [in] */ const String& field,
    /* [in] */ const String& value)
{
}

ECode URLConnection::SetDefaultUseCaches(
    /* [in] */ Boolean newValue)
{
    sDefaultUseCaches = newValue;
    return NOERROR;
}

ECode URLConnection::SetDoInput(
    /* [in] */ Boolean newValue)
{
    FAIL_RETURN(CheckNotConnected());
    mDoInput = newValue;

    return NOERROR;
}

ECode URLConnection::SetDoOutput(
    /* [in] */ Boolean newValue)
{
    FAIL_RETURN(CheckNotConnected());
    mDoOutput = newValue;

    return NOERROR;
}

void URLConnection::SetFileNameMap(
    /* [in] */ IFileNameMap* map)
{
    {    AutoLock syncLock(sLock);
        sFileNameMap = map;
    }
    return;
}

ECode URLConnection::SetIfModifiedSince(
    /* [in] */ Int64 newValue)
{
    FAIL_RETURN(CheckNotConnected());
    mIfModifiedSince = newValue;

    return NOERROR;
}

ECode URLConnection::SetRequestProperty(
    /* [in] */ const String& field,
    /* [in] */ const String& newValue)
{
    FAIL_RETURN(CheckNotConnected());
    if (field.IsNull()) {
//        throw new NullPointerException("field == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode URLConnection::SetUseCaches(
    /* [in] */ Boolean newValue)
{
    FAIL_RETURN(CheckNotConnected());
    mUseCaches = newValue;

    return NOERROR;
}

ECode URLConnection::SetConnectTimeout(
    /* [in] */ Int32 timeout)
{
    if (timeout < 0) {
//        throw new IllegalArgumentException("timeout < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConnectTimeout = timeout;
    return NOERROR;
}

ECode URLConnection::GetConnectTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)

    *timeout = mConnectTimeout;
    return NOERROR;
}

ECode URLConnection::SetReadTimeout(
    /* [in] */ Int32 timeout)
{
    if (timeout < 0) {
//        throw new IllegalArgumentException("timeout < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    mReadTimeout = timeout;
    return NOERROR;
}

ECode URLConnection::GetReadTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)

    *timeout = mReadTimeout;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
