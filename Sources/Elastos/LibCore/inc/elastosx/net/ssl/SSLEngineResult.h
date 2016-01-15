
#ifndef __ELASTOSX_NET_SSL_SSLENGINERESULT_H__
#define __ELASTOSX_NET_SSL_SSLENGINERESULT_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The result object describing the state of the {@code SSLEngine} produced
 * by the {@code wrap()} and {@code unwrap()} operations.
 */
class SSLEngineResult
    : public Object
    , public ISSLEngineResult
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code SSLEngineResult} instance with the specified state
     * values.
     *
     * @param status
     *            the return value of the {@code SSLEngine} operation.
     * @param handshakeStatus
     *            the status of the current handshake
     * @param bytesConsumed
     *            the number of bytes retrieved from the source buffer(s).
     * @param bytesProduced
     *            the number of bytes transferred to the destination buffer(s).
     * @throws IllegalArgumentException
     *             if {@code status} or {@code handshakeStatus} is {@code null},
     *             or if {@code bytesConsumed} or {@code bytesProduces} are
     *             negative.
     */
    CARAPI constructor(
        /* [in] */ SSLEngineResult_Status status,
        /* [in] */ SSLEngineResult_HandshakeStatus handshakeStatus,
        /* [in] */ Int32 bytesConsumed,
        /* [in] */ Int32 bytesProduced);

    /**
     * Returns the return value of the {@code SSLEngine} operation.
     *
     * @return the return value of the {@code SSLEngine} operation.
     */
    CARAPI GetStatus(
        /* [out] */ SSLEngineResult_Status* status);

    /**
     * Returns the status of the current handshake.
     *
     * @return the status of the current handshake.
     */
    CARAPI GetHandshakeStatus(
        /* [out] */ SSLEngineResult_HandshakeStatus* status);

    /**
     * Returns the number of bytes retrieved from the source buffer(s).
     *
     * @return the number of bytes retrieved from the source buffer(s).
     */
    CARAPI BytesConsumed(
        /* [out] */ Int32* bytesConsumed);

    /**
     * Returns the number of bytes transferred to the destination buffer(s).
     *
     * @return the number of bytes transferred to the destination buffer(s).
     */
    CARAPI BytesProduced(
        /* [out] */ Int32* bytesProduced);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    // Store Status object
    /*const*/ SSLEngineResult_Status mStatus;

    // Store HandshakeStatus object
    /*const*/ SSLEngineResult_HandshakeStatus mHandshakeStatus;

    // Store bytesConsumed
    /*const*/ Int32 mBytesConsumed;

    // Store bytesProduced
    /*const*/ Int32 mBytesProduced;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLENGINERESULT_H__
