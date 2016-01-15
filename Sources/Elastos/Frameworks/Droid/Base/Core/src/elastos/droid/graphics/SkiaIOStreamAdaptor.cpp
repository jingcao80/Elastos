
#include "elastos/droid/graphics/SkiaIOStreamAdaptor.h"

using Elastos::IO::IFlushable;

namespace Elastos {
namespace Droid {
namespace Graphics {

class SkiaInputStreamAdaptor : public SkStream
{
public:
    SkiaInputStreamAdaptor(IInputStream* is, ArrayOf<Byte>* ar)
        : mInputStream(is)
        , mByteArray(ar)
    {
        SkASSERT(ar);
        mCapacity   = ar->GetLength();
        SkASSERT(mCapacity > 0);
        mBytesRead  = 0;
    }

    virtual bool rewind()
    {
        mBytesRead = 0;

        if (FAILED(mInputStream->Reset())) {
            return FALSE;
        }
        return TRUE;
    }

    size_t doRead(void* buffer, size_t size)
    {
        size_t bytesRead = 0;
        // read the bytes
        do {
            size_t requested = size;
            if (requested > mCapacity) {
                requested = mCapacity;
            }

            Int32 n;
            ECode ec = mInputStream->Read(mByteArray, 0, requested, &n);
            if (FAILED(ec)) {
                return 0;
            }

            if (n < 0) { // n == 0 should not be possible, see InputStream read() specifications.
                break;  // eof
            }

            memcpy(buffer, mByteArray->GetPayload(), n);

            buffer = (void*)((char*)buffer + n);
            bytesRead += n;
            size -= n;
            mBytesRead += n;
        } while (size != 0);

        return bytesRead;
    }

    size_t doSkip(size_t size)
    {
        Int64 skipped;

        if (FAILED(mInputStream->Skip(size, &skipped))) {
            return 0;
        }

        if (skipped < 0) {
            skipped = 0;
        }

        return (size_t)skipped;
    }

    size_t doSize()
    {
        Int32 avail;

        if (FAILED(mInputStream->Available(&avail))) {
            avail = 0;
        }
        return avail;
    }

    virtual size_t read(void* buffer, size_t size)
    {
        if (NULL == buffer) {
            if (0 == size) {
                return this->doSize();
            }
            else {
                /*  InputStream.skip(n) can return <=0 but still not be at EOF
                    If we see that value, we need to call read(), which will
                    block if waiting for more data, or return -1 at EOF
                 */
                size_t amountSkipped = 0;
                do {
                    size_t amount = this->doSkip(size - amountSkipped);
                    if (0 == amount) {
                        char tmp;
                        amount = this->doRead(&tmp, 1);
                        if (0 == amount) {
                            // if read returned 0, we're at EOF
                            break;
                        }
                    }
                    amountSkipped += amount;
                } while (amountSkipped < size);
                return amountSkipped;
            }
        }
        return this->doRead(buffer, size);
    }

private:
    AutoPtr<IInputStream> mInputStream;
    AutoPtr< ArrayOf<Byte> > mByteArray;          // the caller owns this object
    size_t mCapacity;
    size_t mBytesRead;
};

SkStream* CreateSkiaInputStreamAdaptor(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage,
    /* [in] */ Int32 markSize)
{
    if (markSize) {
        stream->Mark(markSize);
    }

    return new SkiaInputStreamAdaptor(stream, storage);
}

class SkiaOutputStreamAdaptor : public SkWStream
{
public:
    SkiaOutputStreamAdaptor(IOutputStream* stream, ArrayOf<Byte>* storage)
        : mOutputStream(stream)
        , mByteArray(storage)
    {
        mCapacity = storage->GetLength();
    }

    virtual bool write(const void* buffer, size_t size)
    {
        ArrayOf<Byte>* storage = mByteArray;

        while (size > 0) {
            size_t requested = size;
            if (requested > mCapacity) {
                requested = mCapacity;
            }

            memcpy(storage->GetPayload(), buffer, requested);

            ECode ec = mOutputStream->WriteBytes(*storage, 0, requested);
            if (FAILED(ec)) {
                return FALSE;
            }

            buffer = (void*)((char*)buffer + requested);
            size -= requested;
        }
        return TRUE;
    }

    virtual void flush()
    {
        assert(IFlushable::Probe(mOutputStream) != NULL);
        IFlushable::Probe(mOutputStream)->Flush();
    }

private:
    AutoPtr<IOutputStream> mOutputStream;
    AutoPtr< ArrayOf<Byte> > mByteArray;     // the caller owns this object
    size_t mCapacity;
};

SkWStream* CreateSkiaOutputStreamAdaptor(
    /* [in] */ IOutputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage)
{
    return new SkiaOutputStreamAdaptor(stream, storage);
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
