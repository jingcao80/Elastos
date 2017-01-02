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

#ifndef __ELASTOS_IO_CHANNELS_ELASTOS_IO_CCHANNELS_H__
#define __ELASTOS_IO_CHANNELS_ELASTOS_IO_CCHANNELS_H__

#include "_Elastos_IO_Channels_CChannels.h"
#include "Singleton.h"
#include "AbstractInterruptibleChannel.h"
#include "InputStream.h"
#include "OutputStream.h"

using Elastos::Core::Singleton;
using Elastos::IO::Charset::ICharsetEncoder;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::IO::Channels::Spi::AbstractInterruptibleChannel;

namespace Elastos {
namespace IO {
namespace Channels{

CarClass(CChannels)
    , public Singleton
    , public IChannels
{
private:
    /**
     * An input stream that delegates to a readable channel.
     */
    class ChannelInputStream : public InputStream
    {
    public:
        ChannelInputStream(
            /* [in] */ IReadableByteChannel* channel);

        CARAPI Read(
            /* [out] */ Int32* value);

        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 byteOffset,
            /* [in] */ Int32 byteCount,
            /* [out] */ Int32* number);

        CARAPI Available(
            /* [out] */ Int32* number);

        CARAPI Close();

    private:
        AutoPtr<IReadableByteChannel> mChannel;
    };

    /**
     * An output stream that delegates to a writable channel.
     */
    class ChannelOutputStream : public OutputStream
    {
    public:
        ChannelOutputStream(
            /* [in] */ IWritableByteChannel* channel);

        // @Override
        CARAPI Write(
            /* [in] */ Int32 oneByte);

        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        CARAPI Close();

    private:
        AutoPtr<IWritableByteChannel> mChannel;
    };

    /**
     * A readable channel that delegates to an input stream.
     */
    class InputStreamChannel
        : public AbstractInterruptibleChannel
        , public IReadableByteChannel
    {
    public:
        InputStreamChannel(
            /* [in] */ IInputStream* inputStream);

        CAR_INTERFACE_DECL()

        CARAPI Read(
            /* [in] */ IByteBuffer* target,
            /* [out] */ Int32* value);

        // @Override
        CARAPI ImplCloseChannel();

    private:
        AutoPtr<IInputStream> mInputStream;
    };

    /**
     * A writable channel that delegates to an output stream.
     */
    class OutputStreamChannel
        : public AbstractInterruptibleChannel
        , public IWritableByteChannel
    {
    public:
        OutputStreamChannel(
            /* [in] */ IOutputStream* outputStream);

        CAR_INTERFACE_DECL()

        CARAPI Write(
            /* [in] */ IByteBuffer* source,
            /* [out] */ Int32* value);

        // @Override
        CARAPI ImplCloseChannel();

    private:
        AutoPtr<IOutputStream> mOutputStream;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInputStream(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [out] */ IInputStream ** ppInputStream);

    CARAPI NewOutputStream(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [out] */ IOutputStream ** ppOutputStream);

    CARAPI NewChannel(
        /* [in] */ IInputStream * pInputStream,
        /* [out] */ IReadableByteChannel ** ppInputChannel);

    CARAPI NewChannel(
        /* [in] */ IOutputStream * pOutputStream,
        /* [out] */ IWritableByteChannel ** ppOutputChannel);

    CARAPI NewReader(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [in] */ ICharsetDecoder * pDecoder,
        /* [in] */ Int32 minBufferCapacity,
        /* [out] */ IReader ** ppReader);

    CARAPI NewReader(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [in] */ const String& charsetName,
        /* [out] */ IReader ** ppReader);

    CARAPI NewWriter(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [in] */ ICharsetEncoder * pEncoder,
        /* [in] */ Int32 minBufferCapacity,
        /* [out] */ IWriter ** ppWriter);

    CARAPI NewWriter(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [in] */ const String& charsetName,
        /* [out] */ IWriter ** ppWriter);

    static CARAPI _NewInputStream(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [out] */ IInputStream ** ppInputStream);

    static CARAPI _NewOutputStream(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [out] */ IOutputStream ** ppOutputStream);

    static CARAPI _NewChannel(
        /* [in] */ IInputStream * pInputStream,
        /* [out] */ IReadableByteChannel ** ppInputChannel);

    static CARAPI _NewChannel(
        /* [in] */ IOutputStream * pOutputStream,
        /* [out] */ IWritableByteChannel ** ppOutputChannel);

    static CARAPI _NewReader(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [in] */ ICharsetDecoder * pDecoder,
        /* [in] */ Int32 minBufferCapacity,
        /* [out] */ IReader ** ppReader);

    static CARAPI _NewReader(
        /* [in] */ IReadableByteChannel * pChannel,
        /* [in] */ const String& charsetName,
        /* [out] */ IReader ** ppReader);

    static CARAPI _NewWriter(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [in] */ ICharsetEncoder * pEncoder,
        /* [in] */ Int32 minBufferCapacity,
        /* [out] */ IWriter ** ppWriter);

    static CARAPI _NewWriter(
        /* [in] */ IWritableByteChannel * pChannel,
        /* [in] */ const String& charsetName,
        /* [out] */ IWriter ** ppWriter);

protected:
    static CARAPI CheckBlocking(
        /* [in] */ IChannel* channel);
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_ELASTOS_IO_CCHANNELS_H__
