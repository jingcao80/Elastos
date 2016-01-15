
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "URLConnection.h"

using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::URLConnection;
using Elastos::Net::ISocket;
using Elastos::Net::IServerSocket;
using Elastos::Net::IURI;

namespace Libcore {
namespace Net {
namespace Url {

class FtpURLConnection
    : public URLConnection
    , public IFtpURLConnection
{
public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Connect();

    CARAPI GetContentType(
        /* [out] */ String* type);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** is);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** os);

    /**
     * Returns the permission object (in this case, SocketPermission) with the
     * host and the port number as the target name and "resolve, connect" as the
     * action list.
     *
     * @return the permission object required for this connection
     * @throws IOException
     *             thrown when an IO exception occurs during the creation of the
     *             permission object.
     */
    CARAPI GetPermission(
        /* [out] */ IPermission** permission);

    CARAPI SetDoInput(
        /* [in] */ Boolean newValue);

    CARAPI SetDoOutput(
        /* [in] */ Boolean newValue);

    CARAPI constructor(
        /* [in] */ IURL* url);

    FtpURLConnection();

private:
    CARAPI Cd();

    CARAPI ConnectInternal();

    CARAPI GetFile();

    CARAPI GetReply(
        /* [out] */ Int32* value);

    CARAPI Login();

    CARAPI Port();

    CARAPI_(String) ReadLine();

    CARAPI_(Boolean) ReadMultiLine();

    CARAPI SendFile();

    CARAPI SetType();

    CARAPI_(void) Write(
        /* [in] */ const String& command);

private:

    static const Int32 FTP_PORT; // = 21;

    // FTP Reply Constants
    static const Int32 FTP_DATAOPEN; // = 125;

    static const Int32 FTP_OPENDATA; // = 150;

    static const Int32 FTP_OK; // = 200;

    static const Int32 FTP_USERREADY; // = 220;

    static const Int32 FTP_TRANSFEROK; // = 226;

    // static const Int32 FTP_PASV = 227;

    static const Int32 FTP_LOGGEDIN; // = 230;

    static const Int32 FTP_FILEOK; // = 250;

    static const Int32 FTP_PASWD; // = 331;

    // static const Int32 FTP_DATAERROR = 451;

    // static const Int32 FTP_ERROR = 500;

    static const Int32 FTP_NOTFOUND; // = 550;

    AutoPtr<ISocket> mControlSocket;

    AutoPtr<ISocket> mDataSocket;

    AutoPtr<IServerSocket> mAcceptSocket;

    AutoPtr<IInputStream> mCtrlInput;

    AutoPtr<IInputStream> mInputStream;

    AutoPtr<IOutputStream> mCtrlOutput;

    Int32 mDataPort;

    String mUsername; // = "anonymous";

    String mPassword; // = "";

    String mReplyCode;

    String mHostName;

    AutoPtr<Elastos::Net::IProxy> mProxy;

    AutoPtr<Elastos::Net::IProxy> mCurrentProxy;

    AutoPtr<IURI> mUri;
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
