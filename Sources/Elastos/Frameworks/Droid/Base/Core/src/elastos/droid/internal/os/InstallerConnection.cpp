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

#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/internal/os/InstallerConnection.h"
#include "elastos/droid/net/CLocalSocket.h"
#include "elastos/droid/net/CLocalSocketAddress.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Libcore::IO::CStreams;
using Libcore::IO::IStreams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

const String InstallerConnection::TAG("InstallerConnection");
const Boolean InstallerConnection::LOCAL_DEBUG = FALSE;

ECode InstallerConnection::constructor()
{
    mBuf = ArrayOf<Byte>::Alloc(1024);
    return NOERROR;
}

CAR_INTERFACE_IMPL(InstallerConnection, Object, IInstallerConnection)

ECode InstallerConnection::Transact(
    /* [in] */ const String& cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    if (!Connect()) {
        Slogger::E(TAG, "connection failed");
        *result = String("-1");
        return NOERROR;
    }

    if (!WriteCommand(cmd)) {
        /*
         * If installd died and restarted in the background (unlikely but
         * possible) we'll fail on the next write (this one). Try to
         * reconnect and write the command one more time before giving up.
         */
        Slogger::E(TAG, "write command failed? reconnect!");
        if (!Connect() || !WriteCommand(cmd)) {
            *result = String("-1");
            return NOERROR;
        }
    }
    if (LOCAL_DEBUG) {
        Slogger::I(TAG, "send: '%s'", cmd.string());
    }

    Int32 replyLength = ReadReply();
    if (replyLength > 0) {
        String s(*mBuf, 0, replyLength);
        if (LOCAL_DEBUG) {
            Slogger::I(TAG, "recv: '%s'", s.string());
        }
        *result = s;
    }
    else {
        if (LOCAL_DEBUG) {
            Slogger::I(TAG, "fail");
        }
        *result = String("-1");
    }
    return NOERROR;
}

ECode InstallerConnection::Execute(
    /* [in] */ const String& cmd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String res;
    Transact(cmd, &res);
    *result = StringUtils::ParseInt32(res, 10 , -1);
    return NOERROR;
}

ECode InstallerConnection::Dexopt(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& instructionSet,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return Dexopt(apkPath, uid, isPublic, String("*"), instructionSet, FALSE, result);
}

ECode InstallerConnection::Dexopt(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& pkgName,
    /* [in] */ const String& instructionSet,
    /* [in] */ Boolean vmSafeMode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder("dexopt");
    builder.AppendChar(' ');
    builder.Append(apkPath);
    builder.AppendChar(' ');
    builder.Append(uid);
    builder.Append(isPublic ? " 1" : " 0");
    builder.AppendChar(' ');
    builder.Append(pkgName);
    builder.AppendChar(' ');
    builder.Append(instructionSet);
    builder.AppendChar(' ');
    builder.Append(vmSafeMode ? " 1" : " 0");
    return Execute(builder.ToString(), result);
}

ECode InstallerConnection::Patchoat(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& instructionSet,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return Patchoat(apkPath, uid, isPublic, String("*"), instructionSet, result);
}

ECode InstallerConnection::Patchoat(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& pkgName,
    /* [in] */ const String& instructionSet,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder("patchoat");
    builder.AppendChar(' ');
    builder.Append(apkPath);
    builder.AppendChar(' ');
    builder.Append(uid);
    builder.Append(isPublic ? " 1" : " 0");
    builder.AppendChar(' ');
    builder.Append(pkgName);
    builder.AppendChar(' ');
    builder.Append(instructionSet);
    return Execute(builder.ToString(), result);
}

Boolean InstallerConnection::Connect()
{
    if (mSocket != NULL) {
        return TRUE;
    }
    Slogger::I(TAG, "connecting...");
    ECode ec = NOERROR;
    do {
        CLocalSocket::New((ILocalSocket**)&mSocket);

        AutoPtr<ILocalSocketAddress> address;
        CLocalSocketAddress::New(String("installd"), LocalSocketAddressNamespace_RESERVED,
            (ILocalSocketAddress**)&address);

        ec = mSocket->Connect(address);
        if (FAILED(ec))
            break;

        mIn = NULL;
        ec = mSocket->GetInputStream((IInputStream**)&mIn);
        if (FAILED(ec))
            break;

        mOut = NULL;
        ec = mSocket->GetOutputStream((IOutputStream**)&mOut);
    } while (0);

    if (FAILED(ec)) { //E_IO_EXCEPTION
        Disconnect();
        return FALSE;
    }
    return TRUE;
}

ECode InstallerConnection::Disconnect()
{
    Slogger::I(TAG, "disconnecting...");
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(mSocket));
    ioUtils->CloseQuietly(ICloseable::Probe(mIn));
    ioUtils->CloseQuietly(ICloseable::Probe(mOut));

    mSocket = NULL;
    mIn = NULL;
    mOut = NULL;
    return NOERROR;
}

Boolean InstallerConnection::ReadFully(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 len)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    if (FAILED(streams->ReadFully(mIn, buffer, 0, len))) { //E_IO_EXCEPTION
        Slogger::E(TAG, "read exception");
        Disconnect();
        return FALSE;
    }

    if (LOCAL_DEBUG) {
        Slogger::I(TAG, "read %d bytes", len);
    }

    return TRUE;
}

Int32 InstallerConnection::ReadReply()
{
    if (!ReadFully(mBuf, 2)) {
        return -1;
    }

    Int32 len = (((Int32)(*mBuf)[0]) & 0xff) | ((((Int32)(*mBuf)[1]) & 0xff) << 8);
    if ((len < 1) || (len > mBuf->GetLength())) {
        Slogger::E(TAG, "invalid reply length (%d)", len);
        Disconnect();
        return -1;
    }

    if (!ReadFully(mBuf, len)) {
        return -1;
    }

    return len;
}

Boolean InstallerConnection::WriteCommand(
    /* [in] */ const String& cmdString)
{
    AutoPtr<ArrayOf<Byte> > cmd = cmdString.GetBytes();
    Int32 len = cmd->GetLength();
    if ((len < 1) || (len > mBuf->GetLength())) {
        return FALSE;
    }

    (*mBuf)[0] = (Byte)(len & 0xff);
    (*mBuf)[1] = (Byte)((len >> 8) & 0xff);
    ECode ec = NOERROR;
    do {
        ec = mOut->Write(mBuf, 0, 2);
        if (FAILED(ec))
            break;
        ec = mOut->Write(cmd, 0, len);
    } while (0);

    if (FAILED(ec)) { //E_IO_EXCEPTION
        Slogger::E(TAG, "write error");
        Disconnect();
        return FALSE;
    }
    return TRUE;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
