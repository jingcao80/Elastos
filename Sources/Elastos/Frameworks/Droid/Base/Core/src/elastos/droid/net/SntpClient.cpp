
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/SntpClient.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Net::CDatagramPacket;
using Elastos::Net::CDatagramSocket;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(SntpClient, Object, ISntpClient)

const String SntpClient::TAG("SntpClient");
const Int32 SntpClient::REFERENCE_TIME_OFFSET = 16;
const Int32 SntpClient::ORIGINATE_TIME_OFFSET = 24;
const Int32 SntpClient::RECEIVE_TIME_OFFSET = 32;
const Int32 SntpClient::TRANSMIT_TIME_OFFSET = 40;
const Int32 SntpClient::NTP_PACKET_SIZE = 48;
const Int32 SntpClient::NTP_PORT = 123;
const Int32 SntpClient::NTP_MODE_CLIENT = 3;
const Int32 SntpClient::NTP_VERSION = 3;
const Int64 SntpClient::OFFSET_1900_TO_1970 = ((365L * 70LL) + 17LL) * 24LL * 60LL * 60LL;

SntpClient::SntpClient()
    : mNtpTime(0)
    , mNtpTimeReference(0)
    , mRoundTripTime(0)
{}

ECode SntpClient::constructor()
{
    return NOERROR;
}

ECode SntpClient::RequestTime(
    /* [in] */ const String& host,
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IDatagramSocket> socket;
    //try {
    ECode ec = CDatagramSocket::New((IDatagramSocket**)&socket);
    if (FAILED(ec) || socket == NULL) {
        Slogger::E(TAG, "Failed to create CDatagramPacket!");
        return ec;
    }

    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);

    Int64 requestTime, requestTicks, responseTicks, responseTime;
    Int64 originateTime = 0, receiveTime = 0, transmitTime = 0, roundTripTime, clockOffset;
    Int32  length = NTP_PACKET_SIZE;
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(length);

    AutoPtr<IDatagramPacket> response;
    AutoPtr<IInetAddress> address;
    AutoPtr<IDatagramPacket> request;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    ec = socket->SetSoTimeout(timeout);
    FAIL_GOTO(ec, _EXIT_)

    ec = helper->GetByName(host, (IInetAddress**)&address);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDatagramPacket::New(buffer, length, address, NTP_PORT, (IDatagramPacket**)&request);
    FAIL_GOTO(ec, _EXIT_)

    // set mode = 3 (client) and version = 3
    // mode is in low 3 bits of first byte
    // version is in bits 3-5 of first byte
    (*buffer)[0] = NTP_MODE_CLIENT | (NTP_VERSION << 3);

    // get current time and write it to the request packet
    system->GetCurrentTimeMillis(&requestTime);
    requestTicks = SystemClock::GetElapsedRealtime();
    WriteTimeStamp(buffer, TRANSMIT_TIME_OFFSET, requestTime);

    ec = socket->Send(request);
    FAIL_GOTO(ec, _EXIT_)

    // read the response
    ec = CDatagramPacket::New(buffer, length, (IDatagramPacket**)&response);
    FAIL_GOTO(ec, _EXIT_)

    ec = socket->Receive(response);
    FAIL_GOTO(ec, _EXIT_)

    responseTicks = SystemClock::GetElapsedRealtime();
    responseTime = requestTime + (responseTicks - requestTicks);

    // extract the results
    ReadTimeStamp(buffer, ORIGINATE_TIME_OFFSET, &originateTime);
    ReadTimeStamp(buffer, RECEIVE_TIME_OFFSET, &receiveTime);
    ReadTimeStamp(buffer, TRANSMIT_TIME_OFFSET, &transmitTime);
    roundTripTime = responseTicks - requestTicks - (transmitTime - receiveTime);
    // receiveTime = originateTime + transit + skew
    // responseTime = transmitTime + transit - skew
    // clockOffset = ((receiveTime - originateTime) + (transmitTime - responseTime))/2
    //             = ((originateTime + transit + skew - originateTime) +
    //                (transmitTime - (transmitTime + transit - skew)))/2
    //             = ((transit + skew) + (transmitTime - transmitTime - transit + skew))/2
    //             = (transit + skew - transit + skew)/2
    //             = (2 * skew)/2 = skew
    clockOffset = ((receiveTime - originateTime) + (transmitTime - responseTime))/2;
    // if (false) Log.d(TAG, "round trip: " + roundTripTime + " ms");
    // if (false) Log.d(TAG, "clock offset: " + clockOffset + " ms");

    // save our results - use the times on this side of the network latency
    // (response rather than request time)
    mNtpTime = responseTime + clockOffset;
    mNtpTimeReference = responseTicks;
    mRoundTripTime = roundTripTime;

_EXIT_:
    if (FAILED(ec)) {
        Slogger::E(TAG, "request time failed: %08x", ec);
    }

    //finally...
    socket->Close();

    *result = TRUE;
    return ec;
}

ECode SntpClient::GetNtpTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNtpTime;
    return NOERROR;
}

ECode SntpClient::GetNtpTimeReference(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNtpTimeReference;
    return NOERROR;
}

ECode SntpClient::GetRoundTripTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRoundTripTime;
    return NOERROR;
}

ECode SntpClient::Read32(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    Byte b0 = (*buffer)[offset];
    Byte b1 =  (*buffer)[offset+1];
    Byte b2 =  (*buffer)[offset+2];
    Byte b3 =  (*buffer)[offset+3];

    // convert signed bytes to unsigned values
    Int32 i0 = ((b0 & 0x80) == 0x80 ? (b0 & 0x7F) + 0x80 : b0);
    Int32 i1 = ((b1 & 0x80) == 0x80 ? (b1 & 0x7F) + 0x80 : b1);
    Int32 i2 = ((b2 & 0x80) == 0x80 ? (b2 & 0x7F) + 0x80 : b2);
    Int32 i3 = ((b3 & 0x80) == 0x80 ? (b3 & 0x7F) + 0x80 : b3);

    return ((Int64)i0 << 24) + ((Int64)i1 << 16) + ((Int64)i2 << 8) + (Int64)i3;
}

ECode SntpClient::ReadTimeStamp(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 seconds = 0;
    Read32(buffer, offset, &seconds);
    Int64 fraction = 0;
    Read32(buffer, offset + 4, &fraction);
    return ((seconds - OFFSET_1900_TO_1970) * 1000) + ((fraction * 1000L) / 0x100000000L);
}

ECode SntpClient::WriteTimeStamp(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int64 time)
{
    using Elastos::Core::Math;

    Int64 seconds = time / 1000L;
    Int64 milliseconds = time - seconds * 1000L;
    seconds += OFFSET_1900_TO_1970;

    // write seconds in big endian format
    (*buffer)[offset++] = (Byte)(seconds >> 24);
    (*buffer)[offset++] = (Byte)(seconds >> 16);
    (*buffer)[offset++] = (Byte)(seconds >> 8);
    (*buffer)[offset++] = (Byte)(seconds >> 0);

    Int64 fraction = milliseconds * 0x100000000L / 1000L;
    // write fraction in big endian format
    (*buffer)[offset++] = (Byte)(fraction >> 24);
    (*buffer)[offset++] = (Byte)(fraction >> 16);
    (*buffer)[offset++] = (Byte)(fraction >> 8);
    // low order bits should be random data
    (*buffer)[offset++] = (Byte)(Math::Random() * 255.0);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
