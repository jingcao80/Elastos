#ifndef __ELASTOS_IO_CHANNELS_SPI_SELECTORPROVIDER_H__
#define __ELASTOS_IO_CHANNELS_SPI_SELECTORPROVIDER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

/**
 * {@code SelectorProvider} is an abstract base class that declares methods for
 * providing instances of {@link DatagramChannel}, {@link Pipe},
 * {@link java.nio.channels.Selector} , {@link ServerSocketChannel}, and
 * {@link SocketChannel}. All the methods of this class are thread-safe.
 *
 * <p>A provider instance can be retrieved through a system property or the
 * configuration file in a jar file; if no provider is available that way then
 * the system default provider is returned.
 */
class SelectorProvider
    : public ISelectorProvider
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    virtual ~SelectorProvider();

    /**
     * Gets a provider instance by executing the following steps when called for
     * the first time:
     * <ul>
     * <li> if the system property "java.nio.channels.spi.SelectorProvider" is
     * set, the value of this property is the class name of the provider
     * returned; </li>
     * <li>if there is a provider-configuration file named
     * "java.nio.channels.spi.SelectorProvider" in META-INF/services of a jar
     * file valid in the system class loader, the first class name is the
     * provider's class name; </li>
     * <li> otherwise, a system default provider will be returned.</li>
     * </ul>
     *
     * @return the provider.
     */
    static CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider);

    /**
     * Creates a new open {@code DatagramChannel}.
     *
     * @return the new channel.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI OpenDatagramChannel(
        /* [out] */ IDatagramChannel** channel) = 0;

    /**
     * Creates a new {@code Pipe}.
     *
     * @return the new pipe.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI OpenPipe(
        /* [out] */ IPipe** pipe) = 0;

    /**
     * Creates a new selector.
     *
     * @return the new selector.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI OpenSelector(
        /* [out] */ IAbstractSelector** selector) = 0;

    /**
     * Creates a new open {@code ServerSocketChannel}.
     *
     * @return the new channel.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI OpenServerSocketChannel(
        /* [out] */ IServerSocketChannel** channel) = 0;

    /**
     * Create a new open {@code SocketChannel}.
     *
     * @return the new channel.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI OpenSocketChannel(
        /* [out] */ ISocketChannel** channel) = 0;

    /**
     * Returns the channel inherited from the process that created this VM.
     * On Android, this method always returns null because stdin and stdout are
     * never connected to a socket.
     *
     * @return the channel.
     * @throws IOException
     *             if an I/O error occurs.
     */
    CARAPI InheriteChannel(
        /* [out] */ IChannel** cannel);

protected:
    SelectorProvider();

private:
    CARAPI LoadProviderByJar(
        /* [out] */ ISelectorProvider** provider);

private:
    static AutoPtr<ISelectorProvider> sProvider;
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SPI_SELECTORPROVIDER_H__
