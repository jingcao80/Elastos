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

#include "Elastos.CoreLibrary.Utility.h"
#include "FtpURLConnection.h"
#include "ProxySelector.h"
#include "StringBuilder.h"
#include "CProxy.h"
#include "CSocket.h"
#include "CInetSocketAddress.h"
#include "CServerSocket.h"
#include "CBufferedInputStream.h"
#include "StringUtils.h"
#include "CFtpURLInputStream.h"
#include "CSocketPermission.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Etl::List;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Net::IProxySelector;
using Elastos::Net::ProxySelector;
using Elastos::Net::CProxy;
using Elastos::Net::ProxyType;
using Elastos::Net::ProxyType_HTTP;
using Elastos::Net::CSocket;
using Elastos::Net::CServerSocket;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::IInetAddress;
using Elastos::Net::CSocketPermission;
using Elastos::Net::ISocketPermission;

namespace Libcore {
namespace Net {
namespace Url {

const Int32 FtpURLConnection::FTP_PORT = 21;

// FTP Reply Constants
const Int32 FtpURLConnection::FTP_DATAOPEN = 125;

const Int32 FtpURLConnection::FTP_OPENDATA = 150;

const Int32 FtpURLConnection::FTP_OK = 200;

const Int32 FtpURLConnection::FTP_USERREADY = 220;

const Int32 FtpURLConnection::FTP_TRANSFEROK = 226;

const Int32 FtpURLConnection::FTP_LOGGEDIN = 230;

const Int32 FtpURLConnection::FTP_FILEOK = 250;

const Int32 FtpURLConnection::FTP_PASWD = 331;

const Int32 FtpURLConnection::FTP_NOTFOUND = 550;

CAR_INTERFACE_IMPL(FtpURLConnection, URLConnection, IFtpURLConnection)

ECode FtpURLConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("Libcore.Net.Url.FtpURLConnection");
    return NOERROR;
}

ECode FtpURLConnection::Connect()
{
    // Use system-wide ProxySelect to select proxy list,
    // then try to connect via elements in the proxy list.
    List< AutoPtr<Elastos::Net::IProxy> > proxyList;
    if (mProxy != NULL) {
        proxyList.PushBack(mProxy);
    }
    else {
        AutoPtr<IProxySelector> selector;
        ProxySelector::GetDefault((IProxySelector**)&selector);
        if (selector != NULL) {
            AutoPtr<IList> obj;
            selector->Select(mUri, (IList**)&obj);
            AutoPtr<IIterator> emu;
            obj->GetIterator((IIterator**)&emu);
            Boolean hasNext = FALSE;
            while(emu->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> itf;
                emu->GetNext((IInterface**)&itf);
                proxyList.PushBack(Elastos::Net::IProxy::Probe(itf));
            }
        }
    }
    if (proxyList.IsEmpty()) {
        mCurrentProxy = NULL;
        ConnectInternal();
    }
    else {
        AutoPtr<IProxySelector> selector;
        ProxySelector::GetDefault((IProxySelector**)&selector);
        List< AutoPtr<Elastos::Net::IProxy> >::Iterator iter = proxyList.Begin();
        Boolean connectOK = FALSE;
        String failureReason = String("");
        while ((++iter, iter != proxyList.End()) && !connectOK) {
            mCurrentProxy = *iter;
            // try {
            ECode ec = ConnectInternal();
            connectOK = TRUE;
            // } catch (IOException ioe) {
            if (ec != NOERROR) {
                // failureReason = ioe.getLocalizedMessage();
                // If connect failed, callback "connectFailed"
                // should be invoked.
                if (selector != NULL && CProxy::NO_PROXY != mCurrentProxy) {
                    AutoPtr<ISocketAddress> sockadd;
                    mCurrentProxy->GetAddress((ISocketAddress**)&sockadd);
                    selector->ConnectFailed(mUri, sockadd, ec);
                }
            }
            // }
        }
        if (!connectOK) {
            // throw new IOException("Unable to connect to server: " + failureReason);
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode FtpURLConnection::GetContentType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)

    String filename;
    mUrl->GetFile(&filename);
    String result = GuessContentTypeFromName(filename);
    if (result.IsNull()) {
        *type = String("content/unknown");
    }
    else {
        *type = result;
    }

    return NOERROR;
}

ECode FtpURLConnection::GetInputStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is)

    if (!mConnected) {
        FAIL_RETURN(Connect());
    }
    *is = mInputStream;
    REFCOUNT_ADD(*is)

    return NOERROR;
}

ECode FtpURLConnection::GetOutputStream(
    /* [out] */ IOutputStream** os)
{
    VALIDATE_NOT_NULL(os)

    if (!mConnected) {
        FAIL_RETURN(Connect());
    }
    AutoPtr<IOutputStream> output;
    mDataSocket->GetOutputStream((IOutputStream**)&output);
    *os = output;
    REFCOUNT_ADD(*os)

    return NOERROR;
}

ECode FtpURLConnection::GetPermission(
    /* [out] */ IPermission** permission)
{
    VALIDATE_NOT_NULL(permission);
    Int32 port = 0;
    mUrl->GetPort(&port);
    if (port <= 0) {
        port = FTP_PORT;
    }
    AutoPtr<ISocketPermission> sp;
    CSocketPermission::New(mHostName + String(":") + StringUtils::ToString(port), String("connect, resolve"),
            (ISocketPermission**)&sp);
    *permission = IPermission::Probe(sp);
    REFCOUNT_ADD(*permission);
    return NOERROR;
}

ECode FtpURLConnection::SetDoInput(
    /* [in] */ Boolean newValue)
{
    if (mConnected) {
        // throw new IllegalAccessError();
        return E_ILLEGAL_ACCESS_ERROR;
    }
    mDoInput = newValue;
    mDoOutput = !newValue;
    return NOERROR;
}

ECode FtpURLConnection::SetDoOutput(
    /* [in] */ Boolean newValue)
{
    if (mConnected) {
        // throw new IllegalAccessError();
        return E_ILLEGAL_ACCESS_ERROR;
    }
    mDoOutput = newValue;
    mDoInput = !newValue;
    return NOERROR;
}

ECode FtpURLConnection::constructor(
    /* [in] */ IURL* url)
{
    FAIL_RETURN(URLConnection::constructor(url));
    mUsername = "anonymous";
    mPassword = "";
    mUrl->GetHost(&mHostName);
    String parse;
    mUrl->GetUserInfo(&parse);
    if (!parse.IsNull()) {
        Int32 split = parse.IndexOf(':');
        if (split >= 0) {
            mUsername = parse.Substring(0, split);
            mPassword = parse.Substring(split + 1);
        }
        else {
            mUsername = parse;
        }
    }
    mUri = NULL;
    // try {
    url->ToURI((IURI**)&mUri);
    // } catch (URISyntaxException e) {
    //     // do nothing.
    // }
    return NOERROR;
}

FtpURLConnection::FtpURLConnection()
    : mDataPort(0)
{
}

ECode FtpURLConnection::Cd()
{
    String filename;
    mUrl->GetFile(&filename);

    Int32 idx = filename.LastIndexOf('/');

    if (idx > 0) {
        String dir = filename.Substring(0, idx);
        Write(String("CWD ") + dir + String("\r\n"));
        Int32 reply = 0;
        GetReply(&reply);
        if (reply != FTP_FILEOK && dir.GetLength() > 0 && dir.GetChar(0) == '/') {
            Write(String("CWD ") + dir.Substring(1) + String("\r\n"));
            GetReply(&reply);
        }
        if (reply != FTP_FILEOK) {
            // throw new IOException("Unable to change directories");
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode FtpURLConnection::ConnectInternal()
{
    Int32 port = 0;
    mUrl->GetPort(&port);
    Int32 connectTimeout = 0;
    GetConnectTimeout(&connectTimeout);
    if (port <= 0) {
        port = FTP_PORT;
    }
    ProxyType typeflag;
    mControlSocket = NULL;
    if (mCurrentProxy == NULL || ProxyType_HTTP == (mCurrentProxy->GetType(&typeflag), typeflag)) {
        FAIL_RETURN(CSocket::New((ISocket**)&mControlSocket));
    }
    else {
        FAIL_RETURN(CSocket::New(mCurrentProxy, (ISocket**)&mControlSocket));
    }
    AutoPtr<IInetSocketAddress> addr;
    FAIL_RETURN(CInetSocketAddress::New(mHostName, port, (IInetSocketAddress**)&addr));
    mControlSocket->Connect(ISocketAddress::Probe(addr), connectTimeout);
    mConnected = TRUE;
    mCtrlOutput = NULL;
    mControlSocket->GetOutputStream((IOutputStream**)&mCtrlOutput);
    mCtrlInput = NULL;
    mControlSocket->GetInputStream((IInputStream**)&mCtrlInput);
    Login();
    SetType();
    Boolean isflag = FALSE;
    if (!(GetDoInput(&isflag), isflag)) {
        Cd();
    }

    // try {
        mAcceptSocket = NULL;
        CServerSocket::New(0, (IServerSocket**)&mAcceptSocket);
        mAcceptSocket->GetLocalPort(&mDataPort);
        // Cannot set REUSEADDR so we need to send a PORT command
        Port();
        if (connectTimeout == 0) {
            // set timeout rather than zero as before
            connectTimeout = 3000;
        }
        mAcceptSocket->SetSoTimeout((GetConnectTimeout(&connectTimeout), connectTimeout));
        if (GetDoInput(&isflag), isflag) {
            GetFile();
        }
        else {
            SendFile();
        }
        mDataSocket = NULL;
        mAcceptSocket->Accept((ISocket**)&mDataSocket);
        Int32 readtime = 0;
        mDataSocket->SetSoTimeout((GetReadTimeout(&readtime), readtime));
        mAcceptSocket->Close();
    // } catch (InterruptedIOException e) {
    //     throw new IOException("Could not establish data connection");
    // }
    if (GetDoInput(&isflag), isflag) {
        AutoPtr<IInputStream> datainput;
        mDataSocket->GetInputStream((IInputStream**)&datainput);
        AutoPtr<IBufferedInputStream> bis;
        CBufferedInputStream::New(datainput, (IBufferedInputStream**)&bis);
        mInputStream = NULL;
        CFtpURLInputStream::New(IInputStream::Probe(bis), mControlSocket, (IInputStream**)&mInputStream);
    }
    return NOERROR;
}

ECode FtpURLConnection::GetFile()
{
    Int32 reply = 0;
    String file;
    mUrl->GetFile(&file);
    Write(String("RETR ") + file + String("\r\n"));
    GetReply(&reply);
    if (reply == FTP_NOTFOUND && file.GetLength() > 0 && file.GetChar(0) == '/') {
        Write(String("RETR ") + file.Substring(1) + String("\r\n"));
        GetReply(&reply);
    }
    if (!(reply == FTP_OPENDATA || reply == FTP_TRANSFEROK)) {
        // throw new FileNotFoundException("Unable to retrieve file: " + reply);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

ECode FtpURLConnection::GetReply(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr< ArrayOf<Char32> > code = ArrayOf<Char32>::Alloc(3);
    for (Int32 i = 0; i < code->GetLength(); i++) {
        Int32 tmp = 0;
        mCtrlInput->Read(&tmp);
        if (tmp == -1) {
            // throw new EOFException();
            return E_IO_EXCEPTION;
        }
        (*code)[i] = tmp;
    }
    mReplyCode = String(*code, 0, code->GetLength());

    Boolean multiline = FALSE;
    Int32 readint = 0;
    if ((mCtrlInput->Read(&readint), readint) == '-') {
        multiline = TRUE;
    }
    ReadLine(); /* Skip the rest of the first line */
    if (multiline) {
        while (ReadMultiLine()) {/* Read all of a multiline reply */
        }
    }

    // try {
    *value = StringUtils::ParseInt32(mReplyCode);
    // } catch (NumberFormatException e) {
    //     throw (IOException)(new IOException("reply code is invalid").initCause(e));
    // }
    return NOERROR;
}

ECode FtpURLConnection::Login()
{
    Int32 reply = 0;
    GetReply(&reply);
    if (reply == FTP_USERREADY) {
    }
    else {
        // throw new IOException("Unable to connect to server: " + url.getHost());
        return E_IO_EXCEPTION;
    }
    Write(String("USER ") + mUsername + String("\r\n"));
    GetReply(&reply);
    if (reply == FTP_PASWD || reply == FTP_LOGGEDIN) {
    }
    else {
        // throw new IOException("Unable to log in to server (USER): " + url.getHost());
        return E_IO_EXCEPTION;
    }
    if (reply == FTP_PASWD) {
        Write(String("PASS ") + mPassword + String("\r\n"));
        GetReply(&reply);
        if (!(reply == FTP_OK || reply == FTP_USERREADY || reply == FTP_LOGGEDIN)) {
            // throw new IOException("Unable to log in to server (PASS): " + url.getHost());
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode FtpURLConnection::Port()
{
    AutoPtr<IInetAddress> iadd;
    mControlSocket->GetLocalAddress((IInetAddress**)&iadd);
    String outstr;
    iadd->GetHostAddress(&outstr);
    StringBuilder sb("PORT ");
    sb += outstr.Replace('.',',');
    sb += ",";
    sb += StringUtils::ToString(mDataPort >> 8);
    sb += ",";
    sb += StringUtils::ToString(mDataPort & 255);
    sb += "\r\n";
    Write(sb.ToString());

    Int32 value = 0;
    if ((GetReply(&value), value) != FTP_OK) {
        // throw new IOException("Unable to configure data port");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

String FtpURLConnection::ReadLine()
{
    StringBuilder sb;
    Int32 c = 0;
    while ((mCtrlInput->Read(&c), c) != '\n') {
        sb.AppendChar((Char32) c);
    }
    return sb.ToString();
}

Boolean FtpURLConnection::ReadMultiLine()
{
    String line = ReadLine();
    if (line.GetLength() < 4) {
        return TRUE;
    }
    if (line.Substring(0, 3).Equals(mReplyCode)
            && (line.GetChar(3) == (Char32) 32)) {
        return FALSE;
    }
    return TRUE;
}

ECode FtpURLConnection::SendFile()
{
    Int32 reply = 0;
    String filename;
    mUrl->GetFile(&filename);
    Write(String("STOR ")
            + filename.Substring(filename.LastIndexOf('/') + 1, filename.GetLength())
            + String("\r\n"));
    GetReply(&reply);
    if (!(reply == FTP_OPENDATA || reply == FTP_OK || reply == FTP_DATAOPEN)) {
        // throw new IOException("Unable to store file");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode FtpURLConnection::SetType()
{
    Write(String("TYPE I\r\n"));
    Int32 reply = 0;
    if ((GetReply(&reply), reply) != FTP_OK) {
        // throw new IOException("Unable to set transfer type");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

void FtpURLConnection::Write(
    /* [in] */ const String& command)
{
    mCtrlOutput->Write(command.GetBytes(/*StandardCharsets.ISO_8859_1*/));
}

} // namespace Url
} // namespace Net
} // namespace Libcore
