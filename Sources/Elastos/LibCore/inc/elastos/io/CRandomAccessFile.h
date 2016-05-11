
#ifndef __ELASTOS_IO_CRANDOMACCESSFILE_H__
#define __ELASTOS_IO_CRANDOMACCESSFILE_H__

#include "_Elastos_IO_CRandomAccessFile.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::ICloseGuard;
using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {

/**
 * Allows reading from and writing to a file in a random-access manner. This is
 * different from the uni-directional sequential access that a
 * {@link FileInputStream} or {@link FileOutputStream} provides. If the file is
 * opened in read/write mode, write operations are available as well. The
 * position of the next read or write operation can be moved forwards and
 * backwards after every operation.
 */
CarClass(CRandomAccessFile)
    , public Object
    , public IRandomAccessFile
    , public IDataInput
    , public IDataOutput
    , public ICloseable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRandomAccessFile();

    ~CRandomAccessFile();

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& mode);

    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& mode);

    CARAPI Close();

    CARAPI GetChannel(
       /* [out] */ IFileChannel** channel);

    CARAPI GetFD(
        /* [out] */ IFileDescriptor** fd);

    CARAPI GetFilePointer(
        /* [out] */ Int64* offset);

    CARAPI Seek(
        /* [in] */ Int64 offset);

    CARAPI GetLength(
        /* [out] */ Int64* length);

    CARAPI SetLength(
        /* [in] */ Int64 length);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    CARAPI ReadBoolean(
        /* [out] */ Boolean* value);

    CARAPI ReadByte(
        /* [out] */ Byte* value);

    CARAPI ReadDouble(
        /* [out] */ Double* value);

    CARAPI ReadFloat(
        /* [out] */ Float* value);

    CARAPI ReadChar(
        /* [out] */ Char32* value);

    CARAPI ReadFully(
        /* [in] */ ArrayOf<byte>* buffer);

    CARAPI ReadFully(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    CARAPI ReadLine(
        /* [out] */ String* value);

    CARAPI ReadInt64(
        /* [out] */ Int64* value);

    CARAPI ReadInt16(
        /* [out] */ Int16* value);

    CARAPI ReadUnsignedByte(
        /* [out] */ Int32* value);

    CARAPI ReadUnsignedInt16(
        /* [out] */ Int32* value);

    CARAPI ReadUTF(
        /* [out] */ String* str);

    CARAPI SkipBytes(
        /* [in] */ Int32 count,
        /* [out] */ Int32* value);

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI WriteBoolean(
        /* [in] */ Boolean value);

    CARAPI WriteByte(
        /* [in] */ Int32 value);

    CARAPI WriteChar(
        /* [in] */ Int32 value);

    CARAPI WriteBytes(
        /* [in] */ const String& str);

    CARAPI WriteChars(
        /* [in] */ const String& str);

    CARAPI WriteDouble(
        /* [in] */ Double value);

    CARAPI WriteFloat(
        /* [in] */ Float value);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI WriteInt64(
        /* [in] */ Int64 value);

    CARAPI WriteInt16(
        /* [in] */ Int32 value);

    CARAPI WriteUTF(
        /* [in] */ const String& str);

private:
    /**
     * The FileDescriptor representing this RandomAccessFile.
     */
    AutoPtr<IFileDescriptor> mFd;

    Boolean mSyncMetadata;

    // The unique file channel associated with this FileInputStream (lazily
    // initialized).
    AutoPtr<IFileChannel> mChannel;

    Int32 mMode;

    AutoPtr<ICloseGuard> mGuard;
    // CloseGuard guard = CloseGuard.get();

    AutoPtr<ArrayOf<Byte> > mScratch;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CRANDOMACCESSFILE_H__
