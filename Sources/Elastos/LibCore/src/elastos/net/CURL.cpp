
#include "CURL.h"
#include "CURI.h"
#include "Thread.h"
#include "CSystem.h"
#include "StringUtils.h"
#include "AutoLock.h"
#include "CJarHandler.h"
#include "CFileHandler.h"
#include "CFtpHandler.h"
#include "UrlUtils.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::IClassLoader;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Libcore::Net::Url::IJarHandler;
using Libcore::Net::Url::CJarHandler;
using Libcore::Net::Url::IFileHandler;
using Libcore::Net::Url::CFileHandler;
using Libcore::Net::Url::IFtpHandler;
using Libcore::Net::Url::CFtpHandler;
using Libcore::Net::Url::UrlUtils;

namespace Elastos {
namespace Net {

HashMap<String, AutoPtr<IURLStreamHandler> > CURL::sStreamHandlers;
AutoPtr<IURLStreamHandlerFactory> CURL::sStreamHandlerFactory;
Object CURL::sLock;

CAR_INTERFACE_IMPL_2(CURL, Object, IURL, ISerializable)

CAR_OBJECT_IMPL(CURL)

CURL::CURL()
    : mPort(-1)
    , mHashCode(0)
{}

ECode CURL::constructor(
    /* [in] */ const String& spec)
{
    return constructor(NULL, spec, NULL);
}

ECode CURL::constructor(
    /* [in] */ IURL* context,
    /* [in] */ const String& spec)
{
    return constructor(context, spec, NULL);
}

ECode CURL::constructor(
    /* [in] */ IURL* context,
    /* [in] */ const String& spec,
    /* [in] */ IURLStreamHandler* handler)
{
    if (spec.IsNull()) {
        return E_MALFORMED_URL_EXCEPTION;
    }
    if (handler != NULL) {
        mStreamHandler = handler;
    }

    String spectemp = spec.Trim();

    mProtocol = UrlUtils::GetSchemePrefix(spectemp);
    Int32 schemeSpecificPartStart = !mProtocol.IsNull() ? (mProtocol.GetLength() + 1) : 0;

    String s;
    // If the context URL has a different protocol, discard it because we can't use it.
    if (!mProtocol.IsNull() && context != NULL && (context->GetProtocol(&s), !mProtocol.Equals(s))) {
        context = NULL;
    }

    // Inherit from the context URL if it exists.
    if (context != NULL) {
        String protocol, hostName, authority, userInfo, path, query, ref;
        Int32 port;
        context->GetProtocol(&protocol);
        context->GetHost(&hostName);
        context->GetPort(&port);
        context->GetAuthority(&authority);
        context->GetUserInfo(&userInfo);
        context->GetPath(&path);
        context->GetQuery(&query);
        context->GetRef(&ref);
        Set(protocol, hostName, port, authority, userInfo, path, query, ref);

        if (mStreamHandler == NULL) {
            mStreamHandler = ((CURL*)context)->mStreamHandler;
        }
    } else if (mProtocol.IsNull()) {
        return E_MALFORMED_URL_EXCEPTION;
    }

    if (mStreamHandler == NULL) {
        SetupStreamHandler();
        if (mStreamHandler == NULL) {
            return E_MALFORMED_URL_EXCEPTION;
        }
    }

    // Parse the URL. If the handler throws any exception, throw MalformedURLException instead.
    ECode ec = mStreamHandler->ParseURL(this, spectemp, schemeSpecificPartStart, spectemp.GetLength());
    if (FAILED(ec)) {
        return E_MALFORMED_URL_EXCEPTION;
    }

    return NOERROR;
}

ECode CURL::constructor(
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ const String& file)
{
    return constructor(protocol, host, -1, file, NULL);
}

ECode CURL::constructor(
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& file)
{
    return constructor(protocol, host, port, file, NULL);
}

ECode CURL::constructor(
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& file,
    /* [in] */ IURLStreamHandler* handler)
{
    if (port < -1) {
//        throw new MalformedURLException("Port out of range: " + port);
        return E_MALFORMED_URL_EXCEPTION;
    }

    if (protocol.IsNull()) {
//        throw new NullPointerException("Unknown protocol: null");
        return E_NULL_POINTER_EXCEPTION;
    }

    String hostNew = host;
    // Wrap IPv6 addresses in square brackets if they aren't already.
    if (!host.IsNull() && host.IndexOf(":") != -1 && host.GetChar(0) != '[') {
        hostNew = String("[") + host + String("]");
    }

    mProtocol = protocol;
    mHost = hostNew;
    mPort = port;

    String fileNew = UrlUtils::AuthoritySafePath(host, file);

    // Set the fields from the arguments. Handle the case where the
    // passed in "file" includes both a file and a reference part.
    Int32 index = -1;
    index = fileNew.IndexOf("#");
    if (index >= 0) {
        mFile = fileNew.Substring(0, index);
        mRef = fileNew.Substring(index + 1);
    }
    else {
        mFile = fileNew;
    }
    FixURL(FALSE);

    // Set the stream handler for the URL either to the handler
    // argument if it was specified, or to the default for the
    // receiver's protocol if the handler was null.
    if (handler == NULL) {
        SetupStreamHandler();
        if (mStreamHandler == NULL) {
//            throw new MalformedURLException("Unknown protocol: " + protocol);
            return E_MALFORMED_URL_EXCEPTION;
        }
    }
    else {
//        SecurityManager sm = System.getSecurityManager();
//        if (sm != null) {
//            sm.checkPermission(specifyStreamHandlerPermission);
//        }
        mStreamHandler = handler;
    }
    return NOERROR;
}

ECode CURL::SetURLStreamHandlerFactory(
    /* [in] */ IURLStreamHandlerFactory* streamFactory)
{
    AutoLock lock(sLock);

    if (sStreamHandlerFactory != NULL) {
//        throw new Error("Factory already set");
        return E_FAIL;
    }
//    SecurityManager sm = System.getSecurityManager();
//    if (sm != null) {
//        sm.checkSetFactory();
//    }
    sStreamHandlers.Clear();
    sStreamHandlerFactory = streamFactory;

    return NOERROR;
}

ECode CURL::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (o == NULL) {
        return NOERROR;
    }
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    IURL * obj = IURL::Probe(o);
    if (obj == NULL) {
        return NOERROR;
    }

    return mStreamHandler->Equals(this, obj, result);
}

void CURL::FixURL(
    /* [in] */ Boolean fixHost)
{
    Int32 index;
    if (!mHost.IsNullOrEmpty()) {
        mAuthority = mHost;
        if (mPort != -1) {
            StringBuilder sb(mAuthority);
            sb += ":";
            sb += mPort;
            mAuthority = sb.ToString();
        }
    }
    if (fixHost) {
        if (!mHost.IsNull() && (index = mHost.LastIndexOf('@')) > -1) {
            mUserInfo = mHost.Substring(0, index);
            mHost = mHost.Substring(index + 1);
        }
        else {
            mUserInfo = NULL;
        }
    }
    if (!mFile.IsNull() && (index = mFile.IndexOf('?')) > -1) {
        mQuery = mFile.Substring(index + 1);
        mPath = mFile.Substring(0, index);
    }
    else {
        mQuery = NULL;
        mPath = mFile;
    }
}

ECode CURL::Set(
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& file,
    /* [in] */ const String& ref)
{
    if (mProtocol.IsNull()) {
        mProtocol = protocol;
    }
    mHost = host;
    mFile = file;
    mPort = port;
    mRef = ref;
    mHashCode = 0;
    FixURL(TRUE);
    return NOERROR;
}

ECode CURL::SameFile(
    /* [in] */ IURL* otherURL,
    /* [out] */ Boolean* isSame)
{
    VALIDATE_NOT_NULL(isSame);
    return mStreamHandler->SameFile(this, otherURL, isSame);
}

ECode CURL::GetHashCode(
    /* [out] */ Int32 * hash)
{
    VALIDATE_NOT_NULL(hash)
    if (mHashCode == 0) {
        mStreamHandler->GetHashCode(this, &mHashCode);
    }
    *hash = mHashCode;
    return NOERROR;
}

void CURL::SetupStreamHandler()
{
    // Check for a cached (previously looked up) handler for
    // the requested protocol.
    mStreamHandler = NULL;
    HashMap<String, AutoPtr<IURLStreamHandler> >::Iterator it =
        sStreamHandlers.Find(mProtocol);
    if (it != sStreamHandlers.End()) {
        mStreamHandler = it->mSecond;
        return;
    }

    // If there is a stream handler factory, then attempt to
    // use it to create the handler.
    if (sStreamHandlerFactory != NULL) {
        sStreamHandlerFactory->CreateURLStreamHandler(mProtocol, (IURLStreamHandler**)&mStreamHandler);
        if (mStreamHandler != NULL) {
            sStreamHandlers[mProtocol] = mStreamHandler;
            return;
        }
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    // Check if there is a list of packages which can provide handlers.
    // If so, then walk this list looking for an applicable one.
    String packageList;
    system->GetProperty(String("java.protocol.handler.pkgs"), &packageList);
    AutoPtr<IThread> ct = Thread::GetCurrentThread();

    AutoPtr<IClassLoader> contextClassLoader;
    ct->GetContextClassLoader((IClassLoader**)&contextClassLoader);
    if (packageList != NULL && contextClassLoader != NULL) {
        AutoPtr<ArrayOf<String> > array;
        StringUtils::Split(packageList, String("\\|"), (ArrayOf<String>**)&array);
        if (array != NULL) {
            for (Int32 i = 0; i < array->GetLength(); i++) {
                String packageName = (*array)[i];
                String className = packageName + "." + mProtocol + ".Handler";

                AutoPtr<IClassInfo> c;
                if (FAILED(contextClassLoader->LoadClass(className, (IClassInfo**)&c))) {
                    continue;
                }

                AutoPtr<IInterface> obj;
                if (FAILED(c->CreateObject((IInterface**)&obj))) {
                    continue;
                }
                mStreamHandler = IURLStreamHandler::Probe(obj);

                if (mStreamHandler != NULL) {
                    sStreamHandlers[mProtocol] = mStreamHandler;
                }
                return;
            }
        }
    }

    mStreamHandler = NULL;
    // Fall back to a built-in stream handler if the user didn't supply one
    if (mProtocol.Equals("file")) {
        CFileHandler::New((IURLStreamHandler**)&mStreamHandler);
    }
    else if (mProtocol.Equals("ftp")) {
        CFtpHandler::New((IURLStreamHandler**)&mStreamHandler);
    }
    else if (mProtocol.Equals("http")) {
        assert(0);
        // try {
        //     String name = "com.android.okhttp.HttpHandler";
        //     streamHandler = (URLStreamHandler) Class.forName(name).newInstance();
        // } catch (Exception e) {
        //     throw new AssertionError(e);
        // }
    }
    else if (mProtocol.Equals("https")) {
        assert(0);
        // try {
        //     String name = "com.android.okhttp.HttpsHandler";
        //     streamHandler = (URLStreamHandler) Class.forName(name).newInstance();
        // } catch (Exception e) {
        //     throw new AssertionError(e);
        // }
    }
    else if (mProtocol.Equals("jar")) {
        CJarHandler::New((IURLStreamHandler**)&mStreamHandler);
    }

    if (mStreamHandler != NULL) {
        sStreamHandlers[mProtocol] = mStreamHandler;
    }
}

ECode CURL::GetContent(
    /* [out] */ IInterface** content)
{
    VALIDATE_NOT_NULL(content);

    AutoPtr<IURLConnection> connection;
    FAIL_RETURN(OpenConnection((IURLConnection**)&connection));

    return connection->GetContent(content);
}

//        public final Object getContent(Class[] types)

ECode CURL::OpenStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is);

    AutoPtr<IURLConnection> connection;
    FAIL_RETURN(OpenConnection((IURLConnection**)&connection));

    return connection->GetInputStream(is);
}

ECode CURL::OpenConnection(
    /* [out] */ IURLConnection** connection)
{
    VALIDATE_NOT_NULL(connection);

    return mStreamHandler->OpenConnection(this, connection);
}

ECode CURL::ToURI(
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri);

    String url;
    ToExternalForm(&url);
    return CURI::New(url, uri);
}

ECode CURL::OpenConnection(
    /* [in] */ IProxy* proxy,
    /* [out] */ IURLConnection** connection)
{
    VALIDATE_NOT_NULL(connection);

    if (proxy == NULL) {
//        throw new IllegalArgumentException("proxy == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mStreamHandler->OpenConnection(this, proxy, connection);
}

ECode CURL::ToExternalForm(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);

    if (mStreamHandler == NULL) {
        *url = String("unknown protocol(") + mProtocol + String(")://") + mHost + mFile;
        return NOERROR;
    }
    return mStreamHandler->ToExternalForm(this, url);
}

ECode CURL::GetFile(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    *fileName = mFile;

    return NOERROR;
}

ECode CURL::GetHost(
    /* [out] */ String* hostName)
{
    VALIDATE_NOT_NULL(hostName);
    *hostName = mHost;

    return NOERROR;
}

ECode CURL::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mPort;

    return NOERROR;
}

ECode CURL::GetEffectivePort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port =  CURI::GetEffectivePort(mProtocol, mPort);
    return NOERROR;
}

ECode CURL::GetProtocol(
    /* [out] */ String* protocol)
{
    VALIDATE_NOT_NULL(protocol);
    *protocol = mProtocol;

    return NOERROR;
}

ECode CURL::GetRef(
    /* [out] */ String* ref)
{
    VALIDATE_NOT_NULL(ref);
    *ref = mRef;

    return NOERROR;
}

ECode CURL::GetQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);
    *query = mQuery;

    return NOERROR;
}

ECode CURL::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mPath;

    return NOERROR;
}

ECode CURL::GetUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    *userInfo = mUserInfo;

    return NOERROR;
}

ECode CURL::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);
    *authority = mAuthority;

    return NOERROR;
}

ECode CURL::ToURILenient(
    /* [out] */ IURI** uri)
{
    if (mStreamHandler == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String handlerStr;
    mStreamHandler->ToExternalForm(this, TRUE, &handlerStr);
    return CURI::New(handlerStr, uri);
}

ECode CURL::Set(
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& authority,
    /* [in] */ const String& userInfo,
    /* [in] */ const String& path,
    /* [in] */ const String& query,
    /* [in] */ const String& ref)
{
    String filePart = path;
    if (!query.IsNull() && !query.IsEmpty()) {
        filePart += String("?") + query;
    }
    Set(protocol, host, port, filePart, ref);
    mAuthority = authority;
    mUserInfo = userInfo;
    mPath = path;
    mQuery = query;
    return NOERROR;
}

ECode CURL::GetDefaultPort(
    /* [out] */ Int32* port)
{
    return mStreamHandler->GetDefaultPort(port);
}

ECode CURL::ToString(
    /* [out] */ String* str)
{
    return ToExternalForm(str);
}

} // namespace Net
} // namespace Elastos
