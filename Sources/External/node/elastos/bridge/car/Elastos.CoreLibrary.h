#ifndef __CAR_ELASTOS_CORELIBRARY_H__
#define __CAR_ELASTOS_CORELIBRARY_H__

#ifndef _NO_INCLIST
#include <elastos.h>
using namespace Elastos;
#endif // !_NO_INCLIST


namespace Elastos {
namespace Core {
interface ICharSequence;
EXTERN const _ELASTOS InterfaceID EIID_ICharSequence;
interface IAppendable;
EXTERN const _ELASTOS InterfaceID EIID_IAppendable;
interface ICloneable;
EXTERN const _ELASTOS InterfaceID EIID_ICloneable;
interface IComparable;
EXTERN const _ELASTOS InterfaceID EIID_IComparable;
interface IComparator;
EXTERN const _ELASTOS InterfaceID EIID_IComparator;
interface IStringBuilder;
EXTERN const _ELASTOS InterfaceID EIID_IStringBuilder;
interface IStringBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IStringBuffer;
interface IArrayOf;
EXTERN const _ELASTOS InterfaceID EIID_IArrayOf;
interface INumber;
EXTERN const _ELASTOS InterfaceID EIID_INumber;
interface IBoolean;
EXTERN const _ELASTOS InterfaceID EIID_IBoolean;
interface IByte;
EXTERN const _ELASTOS InterfaceID EIID_IByte;
interface IChar32;
EXTERN const _ELASTOS InterfaceID EIID_IChar32;
interface IInteger16;
EXTERN const _ELASTOS InterfaceID EIID_IInteger16;
interface IInteger32;
EXTERN const _ELASTOS InterfaceID EIID_IInteger32;
interface IInteger64;
EXTERN const _ELASTOS InterfaceID EIID_IInteger64;
interface IFloat;
EXTERN const _ELASTOS InterfaceID EIID_IFloat;
interface IDouble;
EXTERN const _ELASTOS InterfaceID EIID_IDouble;
interface IString;
EXTERN const _ELASTOS InterfaceID EIID_IString;
interface IRunnable;
EXTERN const _ELASTOS InterfaceID EIID_IRunnable;
interface ISynchronize;
EXTERN const _ELASTOS InterfaceID EIID_ISynchronize;
interface IThreadUncaughtExceptionHandler;
EXTERN const _ELASTOS InterfaceID EIID_IThreadUncaughtExceptionHandler;
interface IThread;
EXTERN const _ELASTOS InterfaceID EIID_IThread;
interface IThreadGroup;
EXTERN const _ELASTOS InterfaceID EIID_IThreadGroup;
interface IClassLoader;
EXTERN const _ELASTOS InterfaceID EIID_IClassLoader;
interface IThrowable;
EXTERN const _ELASTOS InterfaceID EIID_IThrowable;
interface IStackTraceElement;
EXTERN const _ELASTOS InterfaceID EIID_IStackTraceElement;
interface IBlockGuardPolicy;
EXTERN const _ELASTOS InterfaceID EIID_IBlockGuardPolicy;
interface IBlockGuard;
EXTERN const _ELASTOS InterfaceID EIID_IBlockGuard;
interface ICloseGuardReporter;
EXTERN const _ELASTOS InterfaceID EIID_ICloseGuardReporter;
interface ICloseGuard;
EXTERN const _ELASTOS InterfaceID EIID_ICloseGuard;
interface ICloseGuardHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICloseGuardHelper;
interface IEnum;
EXTERN const _ELASTOS InterfaceID EIID_IEnum;
interface ISystem;
EXTERN const _ELASTOS InterfaceID EIID_ISystem;
interface IThreadLocal;
EXTERN const _ELASTOS InterfaceID EIID_IThreadLocal;
interface ICPathClassLoaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPathClassLoaderClassObject;
interface ICStringClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringClassObject;
interface ICBooleanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBooleanClassObject;
interface ICByteClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteClassObject;
interface ICChar32ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICChar32ClassObject;
interface ICInteger16ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInteger16ClassObject;
interface ICInteger32ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInteger32ClassObject;
interface ICInteger64ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInteger64ClassObject;
interface ICFloatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFloatClassObject;
interface ICDoubleClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDoubleClassObject;
interface ICArrayOfClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICArrayOfClassObject;
interface ICThrowableClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThrowableClassObject;
interface ICBlockGuardClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBlockGuardClassObject;
interface ICCloseGuardClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCloseGuardClassObject;
interface ICCloseGuardHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCloseGuardHelperClassObject;
interface ICObjectClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICObjectClassObject;
interface ICSystemClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSystemClassObject;
interface ICThreadClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThreadClassObject;
interface ICThreadGroupClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThreadGroupClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPathClassLoader;
EXTERN const _ELASTOS ClassID ECLSID_CPathClassLoaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CString;
EXTERN const _ELASTOS ClassID ECLSID_CStringClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBoolean;
EXTERN const _ELASTOS ClassID ECLSID_CBooleanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByte;
EXTERN const _ELASTOS ClassID ECLSID_CByteClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CChar32;
EXTERN const _ELASTOS ClassID ECLSID_CChar32ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInteger16;
EXTERN const _ELASTOS ClassID ECLSID_CInteger16ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInteger32;
EXTERN const _ELASTOS ClassID ECLSID_CInteger32ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInteger64;
EXTERN const _ELASTOS ClassID ECLSID_CInteger64ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFloat;
EXTERN const _ELASTOS ClassID ECLSID_CFloatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDouble;
EXTERN const _ELASTOS ClassID ECLSID_CDoubleClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CArrayOf;
EXTERN const _ELASTOS ClassID ECLSID_CArrayOfClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThrowable;
EXTERN const _ELASTOS ClassID ECLSID_CThrowableClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBlockGuard;
EXTERN const _ELASTOS ClassID ECLSID_CBlockGuardClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCloseGuard;
EXTERN const _ELASTOS ClassID ECLSID_CCloseGuardClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCloseGuardHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCloseGuardHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CObject;
EXTERN const _ELASTOS ClassID ECLSID_CObjectClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSystem;
EXTERN const _ELASTOS ClassID ECLSID_CSystemClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThread;
EXTERN const _ELASTOS ClassID ECLSID_CThreadClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThreadGroup;
EXTERN const _ELASTOS ClassID ECLSID_CThreadGroupClassObject;
}
}

namespace Elastos {
namespace IO {
interface ISerializable;
EXTERN const _ELASTOS InterfaceID EIID_ISerializable;
interface ICloseable;
EXTERN const _ELASTOS InterfaceID EIID_ICloseable;
interface IFlushable;
EXTERN const _ELASTOS InterfaceID EIID_IFlushable;
interface IInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IInputStream;
interface IFilterInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IFilterInputStream;
interface IBufferedInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IBufferedInputStream;
interface IOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IOutputStream;
interface IFilterOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IFilterOutputStream;
interface IBufferedOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IBufferedOutputStream;
interface IReadable;
EXTERN const _ELASTOS InterfaceID EIID_IReadable;
interface IReader;
EXTERN const _ELASTOS InterfaceID EIID_IReader;
interface IBufferedReader;
EXTERN const _ELASTOS InterfaceID EIID_IBufferedReader;
interface IWriter;
EXTERN const _ELASTOS InterfaceID EIID_IWriter;
interface IBufferedWriter;
EXTERN const _ELASTOS InterfaceID EIID_IBufferedWriter;
interface IByteArrayInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IByteArrayInputStream;
interface IByteArrayOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IByteArrayOutputStream;
interface ICharArrayReader;
EXTERN const _ELASTOS InterfaceID EIID_ICharArrayReader;
interface ICharArrayWriter;
EXTERN const _ELASTOS InterfaceID EIID_ICharArrayWriter;
interface IDataInput;
EXTERN const _ELASTOS InterfaceID EIID_IDataInput;
interface IDataInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDataInputStream;
interface IDataInputStreamHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDataInputStreamHelper;
interface IDataOutput;
EXTERN const _ELASTOS InterfaceID EIID_IDataOutput;
interface IDataOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDataOutputStream;
interface IFile;
EXTERN const _ELASTOS InterfaceID EIID_IFile;
interface IFileHelper;
EXTERN const _ELASTOS InterfaceID EIID_IFileHelper;
interface IFileDescriptor;
EXTERN const _ELASTOS InterfaceID EIID_IFileDescriptor;
interface IFileDescriptorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IFileDescriptorHelper;
interface IFileFilter;
EXTERN const _ELASTOS InterfaceID EIID_IFileFilter;
interface IFileInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IFileInputStream;
interface IFileOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IFileOutputStream;
interface IPermission;
EXTERN const _ELASTOS InterfaceID EIID_IPermission;
interface IPermissionCollection;
EXTERN const _ELASTOS InterfaceID EIID_IPermissionCollection;
interface IFilePermission;
EXTERN const _ELASTOS InterfaceID EIID_IFilePermission;
interface IInputStreamReader;
EXTERN const _ELASTOS InterfaceID EIID_IInputStreamReader;
interface IFileReader;
EXTERN const _ELASTOS InterfaceID EIID_IFileReader;
interface IOutputStreamWriter;
EXTERN const _ELASTOS InterfaceID EIID_IOutputStreamWriter;
interface IFileWriter;
EXTERN const _ELASTOS InterfaceID EIID_IFileWriter;
interface IFilenameFilter;
EXTERN const _ELASTOS InterfaceID EIID_IFilenameFilter;
interface IFilterReader;
EXTERN const _ELASTOS InterfaceID EIID_IFilterReader;
interface IFilterWriter;
EXTERN const _ELASTOS InterfaceID EIID_IFilterWriter;
interface ILineNumberInputStream;
EXTERN const _ELASTOS InterfaceID EIID_ILineNumberInputStream;
interface ILineNumberReader;
EXTERN const _ELASTOS InterfaceID EIID_ILineNumberReader;
interface IObjectInput;
EXTERN const _ELASTOS InterfaceID EIID_IObjectInput;
interface IObjectInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IObjectInputStream;
interface IObjectInputStreamGetField;
EXTERN const _ELASTOS InterfaceID EIID_IObjectInputStreamGetField;
interface IObjectInputValidation;
EXTERN const _ELASTOS InterfaceID EIID_IObjectInputValidation;
interface IObjectOutput;
EXTERN const _ELASTOS InterfaceID EIID_IObjectOutput;
interface IObjectOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IObjectOutputStream;
interface IObjectOutputStreamPutField;
EXTERN const _ELASTOS InterfaceID EIID_IObjectOutputStreamPutField;
interface IObjectStreamClass;
EXTERN const _ELASTOS InterfaceID EIID_IObjectStreamClass;
interface IObjectStreamClassHelper;
EXTERN const _ELASTOS InterfaceID EIID_IObjectStreamClassHelper;
interface IObjectStreamField;
EXTERN const _ELASTOS InterfaceID EIID_IObjectStreamField;
interface IPipedInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IPipedInputStream;
interface IPrintStream;
EXTERN const _ELASTOS InterfaceID EIID_IPrintStream;
interface IPrintWriter;
EXTERN const _ELASTOS InterfaceID EIID_IPrintWriter;
interface IPushbackInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IPushbackInputStream;
interface IPushbackReader;
EXTERN const _ELASTOS InterfaceID EIID_IPushbackReader;
interface IRandomAccessFile;
EXTERN const _ELASTOS InterfaceID EIID_IRandomAccessFile;
interface ISequenceInputStream;
EXTERN const _ELASTOS InterfaceID EIID_ISequenceInputStream;
interface IStreamTokenizer;
EXTERN const _ELASTOS InterfaceID EIID_IStreamTokenizer;
interface IStringBufferInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IStringBufferInputStream;
interface IStringReader;
EXTERN const _ELASTOS InterfaceID EIID_IStringReader;
interface IStringWriter;
EXTERN const _ELASTOS InterfaceID EIID_IStringWriter;
interface IPipedReader;
EXTERN const _ELASTOS InterfaceID EIID_IPipedReader;
interface IPipedWriter;
EXTERN const _ELASTOS InterfaceID EIID_IPipedWriter;
interface IByteOrderHelper;
EXTERN const _ELASTOS InterfaceID EIID_IByteOrderHelper;
interface IBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IBuffer;
interface IByteBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IByteBuffer;
interface IByteBufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IByteBufferHelper;
interface IByteArrayBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IByteArrayBuffer;
interface ICharBuffer;
EXTERN const _ELASTOS InterfaceID EIID_ICharBuffer;
interface ICharBufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICharBufferHelper;
interface IDoubleBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IDoubleBuffer;
interface IDoubleBufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDoubleBufferHelper;
interface IFloatBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IFloatBuffer;
interface IFloatBufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IFloatBufferHelper;
interface IInt16Buffer;
EXTERN const _ELASTOS InterfaceID EIID_IInt16Buffer;
interface IInt16BufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInt16BufferHelper;
interface IInt32Buffer;
EXTERN const _ELASTOS InterfaceID EIID_IInt32Buffer;
interface IInt32BufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInt32BufferHelper;
interface IInt64Buffer;
EXTERN const _ELASTOS InterfaceID EIID_IInt64Buffer;
interface IInt64BufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInt64BufferHelper;
interface IMappedByteBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IMappedByteBuffer;
interface IDirectByteBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IDirectByteBuffer;
interface IDirectByteBufferHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDirectByteBufferHelper;
interface INIOAccess;
EXTERN const _ELASTOS InterfaceID EIID_INIOAccess;
interface IFileDescriptorChannel;
EXTERN const _ELASTOS InterfaceID EIID_IFileDescriptorChannel;
interface IFileChannelImpl;
EXTERN const _ELASTOS InterfaceID EIID_IFileChannelImpl;
interface INioUtils;
EXTERN const _ELASTOS InterfaceID EIID_INioUtils;
interface ICBufferedInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedInputStreamClassObject;
interface ICSelectorProviderImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSelectorProviderImplClassObject;
interface ICByteArrayOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteArrayOutputStreamClassObject;
interface ICStringWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringWriterClassObject;
interface ICFilterInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFilterInputStreamClassObject;
interface ICBufferedOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedOutputStreamClassObject;
interface ICFilterOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFilterOutputStreamClassObject;
interface ICBufferedReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedReaderClassObject;
interface ICBufferedWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedWriterClassObject;
interface ICByteArrayInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteArrayInputStreamClassObject;
interface ICCharArrayReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharArrayReaderClassObject;
interface ICCharArrayWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharArrayWriterClassObject;
interface ICCharBufferHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharBufferHelperClassObject;
interface ICDataInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDataInputStreamClassObject;
interface ICPushbackInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPushbackInputStreamClassObject;
interface ICDataOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDataOutputStreamClassObject;
interface ICByteBufferHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteBufferHelperClassObject;
interface ICByteOrderHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteOrderHelperClassObject;
interface ICSelectorImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSelectorImplClassObject;
interface ICFileDescriptorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileDescriptorClassObject;
interface ICFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileClassObject;
interface ICFileHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileHelperClassObject;
interface ICInputStreamReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInputStreamReaderClassObject;
interface ICDirectByteBufferHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDirectByteBufferHelperClassObject;
interface ICPrintWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPrintWriterClassObject;
interface ICLineNumberInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLineNumberInputStreamClassObject;
interface ICLineNumberReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLineNumberReaderClassObject;
interface ICOutputStreamWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICOutputStreamWriterClassObject;
interface ICPrintStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPrintStreamClassObject;
interface ICPushbackReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPushbackReaderClassObject;
interface ICRandomAccessFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRandomAccessFileClassObject;
interface ICSequenceInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSequenceInputStreamClassObject;
interface ICStringBufferInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringBufferInputStreamClassObject;
interface ICStringReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringReaderClassObject;
interface ICFileInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileInputStreamClassObject;
interface ICFileOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileOutputStreamClassObject;
interface ICFilePermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFilePermissionClassObject;
interface ICFileReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileReaderClassObject;
interface ICFileWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileWriterClassObject;
interface ICNioUtilsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNioUtilsClassObject;
interface ICPipedInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPipedInputStreamClassObject;
interface ICPipedOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPipedOutputStreamClassObject;
interface ICPipedReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPipedReaderClassObject;
interface ICPipedWriterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPipedWriterClassObject;
interface ICNIOAccessClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNIOAccessClassObject;
interface ICStreamTokenizerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStreamTokenizerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorProviderImpl;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorProviderImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringWriter;
EXTERN const _ELASTOS ClassID ECLSID_CStringWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFilterInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CFilterInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFilterOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CFilterOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedReader;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedWriter;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayReader;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayWriter;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharBufferHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCharBufferHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDataInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDataInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPushbackInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CPushbackInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDataOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDataOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteBufferHelper;
EXTERN const _ELASTOS ClassID ECLSID_CByteBufferHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteOrderHelper;
EXTERN const _ELASTOS ClassID ECLSID_CByteOrderHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorImpl;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileDescriptor;
EXTERN const _ELASTOS ClassID ECLSID_CFileDescriptorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFile;
EXTERN const _ELASTOS ClassID ECLSID_CFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileHelper;
EXTERN const _ELASTOS ClassID ECLSID_CFileHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInputStreamReader;
EXTERN const _ELASTOS ClassID ECLSID_CInputStreamReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDirectByteBufferHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDirectByteBufferHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPrintWriter;
EXTERN const _ELASTOS ClassID ECLSID_CPrintWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLineNumberInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CLineNumberInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLineNumberReader;
EXTERN const _ELASTOS ClassID ECLSID_CLineNumberReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_COutputStreamWriter;
EXTERN const _ELASTOS ClassID ECLSID_COutputStreamWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPrintStream;
EXTERN const _ELASTOS ClassID ECLSID_CPrintStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPushbackReader;
EXTERN const _ELASTOS ClassID ECLSID_CPushbackReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRandomAccessFile;
EXTERN const _ELASTOS ClassID ECLSID_CRandomAccessFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSequenceInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CSequenceInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringBufferInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CStringBufferInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringReader;
EXTERN const _ELASTOS ClassID ECLSID_CStringReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CFileInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CFileOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFilePermission;
EXTERN const _ELASTOS ClassID ECLSID_CFilePermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileReader;
EXTERN const _ELASTOS ClassID ECLSID_CFileReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileWriter;
EXTERN const _ELASTOS ClassID ECLSID_CFileWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNioUtils;
EXTERN const _ELASTOS ClassID ECLSID_CNioUtilsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPipedInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CPipedInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPipedOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CPipedOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPipedReader;
EXTERN const _ELASTOS ClassID ECLSID_CPipedReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPipedWriter;
EXTERN const _ELASTOS ClassID ECLSID_CPipedWriterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNIOAccess;
EXTERN const _ELASTOS ClassID ECLSID_CNIOAccessClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStreamTokenizer;
EXTERN const _ELASTOS ClassID ECLSID_CStreamTokenizerClassObject;
}
}

namespace Elastos {
namespace Math {
interface IMathContext;
EXTERN const _ELASTOS InterfaceID EIID_IMathContext;
interface IMathContextHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMathContextHelper;
interface IBigDecimal;
EXTERN const _ELASTOS InterfaceID EIID_IBigDecimal;
interface IBigDecimalHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBigDecimalHelper;
interface IBigInteger;
EXTERN const _ELASTOS InterfaceID EIID_IBigInteger;
interface IBigIntegerHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBigIntegerHelper;
interface ICBigDecimalClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBigDecimalClassObject;
interface ICBigDecimalHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBigDecimalHelperClassObject;
interface ICBigIntegerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBigIntegerClassObject;
interface ICBigIntegerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBigIntegerHelperClassObject;
interface ICMathContextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMathContextClassObject;
interface ICMathContextHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMathContextHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBigDecimal;
EXTERN const _ELASTOS ClassID ECLSID_CBigDecimalClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBigDecimalHelper;
EXTERN const _ELASTOS ClassID ECLSID_CBigDecimalHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBigInteger;
EXTERN const _ELASTOS ClassID ECLSID_CBigIntegerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBigIntegerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CBigIntegerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMathContext;
EXTERN const _ELASTOS ClassID ECLSID_CMathContextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMathContextHelper;
EXTERN const _ELASTOS ClassID ECLSID_CMathContextHelperClassObject;
}
}

namespace Elastos {
namespace Droid {
namespace System {
interface IStructAddrinfo;
EXTERN const _ELASTOS InterfaceID EIID_IStructAddrinfo;
interface IStructFlock;
EXTERN const _ELASTOS InterfaceID EIID_IStructFlock;
interface IStructGroupReq;
EXTERN const _ELASTOS InterfaceID EIID_IStructGroupReq;
interface IStructGroupSourceReq;
EXTERN const _ELASTOS InterfaceID EIID_IStructGroupSourceReq;
interface IStructLinger;
EXTERN const _ELASTOS InterfaceID EIID_IStructLinger;
interface IStructPasswd;
EXTERN const _ELASTOS InterfaceID EIID_IStructPasswd;
interface IStructPollfd;
EXTERN const _ELASTOS InterfaceID EIID_IStructPollfd;
interface IStructStat;
EXTERN const _ELASTOS InterfaceID EIID_IStructStat;
interface IStructStatVfs;
EXTERN const _ELASTOS InterfaceID EIID_IStructStatVfs;
interface IStructTimeval;
EXTERN const _ELASTOS InterfaceID EIID_IStructTimeval;
interface IStructTimevalHelper;
EXTERN const _ELASTOS InterfaceID EIID_IStructTimevalHelper;
interface IStructUcred;
EXTERN const _ELASTOS InterfaceID EIID_IStructUcred;
interface IStructUtsname;
EXTERN const _ELASTOS InterfaceID EIID_IStructUtsname;
interface IZygoteHooks;
EXTERN const _ELASTOS InterfaceID EIID_IZygoteHooks;
interface ICStructAddrinfoClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructAddrinfoClassObject;
interface ICStructGroupReqClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructGroupReqClassObject;
interface ICStructLingerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructLingerClassObject;
interface ICStructPasswdClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructPasswdClassObject;
interface ICStructPollfdClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructPollfdClassObject;
interface ICStructStatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructStatClassObject;
interface ICStructStatVfsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructStatVfsClassObject;
interface ICStructFlockClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructFlockClassObject;
interface ICStructTimevalClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructTimevalClassObject;
interface ICStructTimevalHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructTimevalHelperClassObject;
interface ICStructUcredClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructUcredClassObject;
interface ICStructUtsnameClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructUtsnameClassObject;
interface ICStructGroupSourceReqClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStructGroupSourceReqClassObject;
interface ICZygoteHooksClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZygoteHooksClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructAddrinfo;
EXTERN const _ELASTOS ClassID ECLSID_CStructAddrinfoClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructGroupReq;
EXTERN const _ELASTOS ClassID ECLSID_CStructGroupReqClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructLinger;
EXTERN const _ELASTOS ClassID ECLSID_CStructLingerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructPasswd;
EXTERN const _ELASTOS ClassID ECLSID_CStructPasswdClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructPollfd;
EXTERN const _ELASTOS ClassID ECLSID_CStructPollfdClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructStat;
EXTERN const _ELASTOS ClassID ECLSID_CStructStatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructStatVfs;
EXTERN const _ELASTOS ClassID ECLSID_CStructStatVfsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructFlock;
EXTERN const _ELASTOS ClassID ECLSID_CStructFlockClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructTimeval;
EXTERN const _ELASTOS ClassID ECLSID_CStructTimevalClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructTimevalHelper;
EXTERN const _ELASTOS ClassID ECLSID_CStructTimevalHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructUcred;
EXTERN const _ELASTOS ClassID ECLSID_CStructUcredClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructUtsname;
EXTERN const _ELASTOS ClassID ECLSID_CStructUtsnameClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStructGroupSourceReq;
EXTERN const _ELASTOS ClassID ECLSID_CStructGroupSourceReqClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZygoteHooks;
EXTERN const _ELASTOS ClassID ECLSID_CZygoteHooksClassObject;
}
}
}

namespace Elastos {
namespace Security {
interface IKey;
EXTERN const _ELASTOS InterfaceID EIID_IKey;
interface IPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IPublicKey;
interface IPrivateKey;
EXTERN const _ELASTOS InterfaceID EIID_IPrivateKey;
interface IPrincipal;
EXTERN const _ELASTOS InterfaceID EIID_IPrincipal;
interface IAccessControlContext;
EXTERN const _ELASTOS InterfaceID EIID_IAccessControlContext;
interface IAccessController;
EXTERN const _ELASTOS InterfaceID EIID_IAccessController;
interface IAlgorithmParameterGenerator;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmParameterGenerator;
interface IAlgorithmParameterGeneratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmParameterGeneratorHelper;
interface IAlgorithmParameters;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmParameters;
interface IAlgorithmParametersHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmParametersHelper;
interface IProviderService;
EXTERN const _ELASTOS InterfaceID EIID_IProviderService;
interface IProvider;
EXTERN const _ELASTOS InterfaceID EIID_IProvider;
interface IAuthProvider;
EXTERN const _ELASTOS InterfaceID EIID_IAuthProvider;
interface ICertificate;
EXTERN const _ELASTOS InterfaceID EIID_ICertificate;
interface ITimestamp;
EXTERN const _ELASTOS InterfaceID EIID_ITimestamp;
interface ICodeSigner;
EXTERN const _ELASTOS InterfaceID EIID_ICodeSigner;
interface ICodeSource;
EXTERN const _ELASTOS InterfaceID EIID_ICodeSource;
interface IDigestInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDigestInputStream;
interface IDigestOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDigestOutputStream;
interface IDomainCombiner;
EXTERN const _ELASTOS InterfaceID EIID_IDomainCombiner;
interface IGuard;
EXTERN const _ELASTOS InterfaceID EIID_IGuard;
interface IGuardedObject;
EXTERN const _ELASTOS InterfaceID EIID_IGuardedObject;
interface IIdentity;
EXTERN const _ELASTOS InterfaceID EIID_IIdentity;
interface IIdentityScope;
EXTERN const _ELASTOS InterfaceID EIID_IIdentityScope;
interface IIdentityScopeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IIdentityScopeHelper;
interface IKeyFactory;
EXTERN const _ELASTOS InterfaceID EIID_IKeyFactory;
interface IKeyFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyFactoryHelper;
interface IKeyFactorySpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyFactorySpi;
interface IKeyPair;
EXTERN const _ELASTOS InterfaceID EIID_IKeyPair;
interface IKeyPairGenerator;
EXTERN const _ELASTOS InterfaceID EIID_IKeyPairGenerator;
interface IKeyPairGeneratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyPairGeneratorHelper;
interface IKeyPairGeneratorSpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyPairGeneratorSpi;
interface IKeyStoreBuilder;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreBuilder;
interface IKeyStoreBuilderHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreBuilderHelper;
interface IKeyStoreEntry;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreEntry;
interface IKeyStorePrivateKeyEntry;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStorePrivateKeyEntry;
interface IKeyStoreSecretKeyEntry;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreSecretKeyEntry;
interface IKeyStoreTrustedCertificateEntry;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreTrustedCertificateEntry;
interface IKeyStoreLoadStoreParameter;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreLoadStoreParameter;
interface IKeyStoreProtectionParameter;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreProtectionParameter;
interface IKeyStoreCallbackHandlerProtection;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreCallbackHandlerProtection;
interface IKeyStorePasswordProtection;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStorePasswordProtection;
interface IKeyStore;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStore;
interface IKeyStoreHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreHelper;
interface IKeyStoreSpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreSpi;
interface IMessageDigest;
EXTERN const _ELASTOS InterfaceID EIID_IMessageDigest;
interface IMessageDigestHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMessageDigestHelper;
interface IPermission;
EXTERN const _ELASTOS InterfaceID EIID_IPermission;
interface IPermissionCollection;
EXTERN const _ELASTOS InterfaceID EIID_IPermissionCollection;
interface IPolicyParameters;
EXTERN const _ELASTOS InterfaceID EIID_IPolicyParameters;
interface IPolicy;
EXTERN const _ELASTOS InterfaceID EIID_IPolicy;
interface IPolicyHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPolicyHelper;
interface IPrivilegedAction;
EXTERN const _ELASTOS InterfaceID EIID_IPrivilegedAction;
interface IPrivilegedExceptionAction;
EXTERN const _ELASTOS InterfaceID EIID_IPrivilegedExceptionAction;
interface IProtectionDomain;
EXTERN const _ELASTOS InterfaceID EIID_IProtectionDomain;
interface ISecureRandom;
EXTERN const _ELASTOS InterfaceID EIID_ISecureRandom;
interface ISecureRandomHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISecureRandomHelper;
interface ISecureRandomSpi;
EXTERN const _ELASTOS InterfaceID EIID_ISecureRandomSpi;
interface ISecurity;
EXTERN const _ELASTOS InterfaceID EIID_ISecurity;
interface ISignatureSpi;
EXTERN const _ELASTOS InterfaceID EIID_ISignatureSpi;
interface ISignature;
EXTERN const _ELASTOS InterfaceID EIID_ISignature;
interface ISignatureHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISignatureHelper;
interface ISignedObject;
EXTERN const _ELASTOS InterfaceID EIID_ISignedObject;
interface ISigner;
EXTERN const _ELASTOS InterfaceID EIID_ISigner;
interface IUnresolvedPermission;
EXTERN const _ELASTOS InterfaceID EIID_IUnresolvedPermission;
interface ICProviderServiceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICProviderServiceClassObject;
interface ICSecurityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSecurityClassObject;
interface ICMessageDigestHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMessageDigestHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CProviderService;
EXTERN const _ELASTOS ClassID ECLSID_CProviderServiceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSecurity;
EXTERN const _ELASTOS ClassID ECLSID_CSecurityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMessageDigestHelper;
EXTERN const _ELASTOS ClassID ECLSID_CMessageDigestHelperClassObject;
}
}

namespace Elastos {
namespace Security {
namespace Acl {
interface IOwner;
EXTERN const _ELASTOS InterfaceID EIID_IOwner;
interface IAcl;
EXTERN const _ELASTOS InterfaceID EIID_IAcl;
interface IAclEntry;
EXTERN const _ELASTOS InterfaceID EIID_IAclEntry;
interface IGroup;
EXTERN const _ELASTOS InterfaceID EIID_IGroup;
interface IPermission;
EXTERN const _ELASTOS InterfaceID EIID_IPermission;
}
}
}

namespace Elastos {
namespace Security {
namespace Cert {
interface IExtension;
EXTERN const _ELASTOS InterfaceID EIID_IExtension;
interface ICertificate;
EXTERN const _ELASTOS InterfaceID EIID_ICertificate;
interface IX509Certificate;
EXTERN const _ELASTOS InterfaceID EIID_IX509Certificate;
interface ICRL;
EXTERN const _ELASTOS InterfaceID EIID_ICRL;
interface ICRLSelector;
EXTERN const _ELASTOS InterfaceID EIID_ICRLSelector;
interface ICertPath;
EXTERN const _ELASTOS InterfaceID EIID_ICertPath;
interface ICertPathParameters;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathParameters;
interface ICertPathValidatorResult;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathValidatorResult;
interface ICertPathBuilderResult;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathBuilderResult;
interface ICertPathBuilderSpi;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathBuilderSpi;
interface ICertPathBuilder;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathBuilder;
interface ICertPathBuilderHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathBuilderHelper;
interface ICertPathValidator;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathValidator;
interface ICertPathValidatorHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathValidatorHelper;
interface ICertPathValidatorSpi;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathValidatorSpi;
interface ICertSelector;
EXTERN const _ELASTOS InterfaceID EIID_ICertSelector;
interface ICertStore;
EXTERN const _ELASTOS InterfaceID EIID_ICertStore;
interface ICertStoreHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICertStoreHelper;
interface ICertStoreParameters;
EXTERN const _ELASTOS InterfaceID EIID_ICertStoreParameters;
interface ICertStoreSpi;
EXTERN const _ELASTOS InterfaceID EIID_ICertStoreSpi;
interface ICertificateFactory;
EXTERN const _ELASTOS InterfaceID EIID_ICertificateFactory;
interface ICertificateFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICertificateFactoryHelper;
interface ICertificateFactorySpi;
EXTERN const _ELASTOS InterfaceID EIID_ICertificateFactorySpi;
interface ICollectionCertStoreParameters;
EXTERN const _ELASTOS InterfaceID EIID_ICollectionCertStoreParameters;
interface ILDAPCertStoreParameters;
EXTERN const _ELASTOS InterfaceID EIID_ILDAPCertStoreParameters;
interface IPKIXParameters;
EXTERN const _ELASTOS InterfaceID EIID_IPKIXParameters;
interface IPKIXBuilderParameters;
EXTERN const _ELASTOS InterfaceID EIID_IPKIXBuilderParameters;
interface IPolicyNode;
EXTERN const _ELASTOS InterfaceID EIID_IPolicyNode;
interface ITrustAnchor;
EXTERN const _ELASTOS InterfaceID EIID_ITrustAnchor;
interface IPKIXCertPathValidatorResult;
EXTERN const _ELASTOS InterfaceID EIID_IPKIXCertPathValidatorResult;
interface IPKIXCertPathBuilderResult;
EXTERN const _ELASTOS InterfaceID EIID_IPKIXCertPathBuilderResult;
interface IPKIXCertPathChecker;
EXTERN const _ELASTOS InterfaceID EIID_IPKIXCertPathChecker;
interface IPolicyQualifierInfo;
EXTERN const _ELASTOS InterfaceID EIID_IPolicyQualifierInfo;
interface IX509CRL;
EXTERN const _ELASTOS InterfaceID EIID_IX509CRL;
interface IX509Extension;
EXTERN const _ELASTOS InterfaceID EIID_IX509Extension;
interface IX509CRLEntry;
EXTERN const _ELASTOS InterfaceID EIID_IX509CRLEntry;
interface IX509CRLSelector;
EXTERN const _ELASTOS InterfaceID EIID_IX509CRLSelector;
interface IX509CertSelector;
EXTERN const _ELASTOS InterfaceID EIID_IX509CertSelector;
interface ICCertificateFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCertificateFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCertificateFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCertificateFactoryHelperClassObject;
}
}
}

namespace Elastos {
namespace Security {
namespace Spec {
interface IRSAOtherPrimeInfo;
EXTERN const _ELASTOS InterfaceID EIID_IRSAOtherPrimeInfo;
interface IAlgorithmParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmParameterSpec;
interface IKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IKeySpec;
interface IDSAPrivateKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDSAPrivateKeySpec;
interface IDSAPublicKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDSAPublicKeySpec;
interface IECField;
EXTERN const _ELASTOS InterfaceID EIID_IECField;
interface IECFieldF2m;
EXTERN const _ELASTOS InterfaceID EIID_IECFieldF2m;
interface IECFieldFp;
EXTERN const _ELASTOS InterfaceID EIID_IECFieldFp;
interface IECGenParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IECGenParameterSpec;
interface IECParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IECParameterSpec;
interface IECPoint;
EXTERN const _ELASTOS InterfaceID EIID_IECPoint;
interface IECPrivateKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IECPrivateKeySpec;
interface IECPublicKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IECPublicKeySpec;
interface IEllipticCurve;
EXTERN const _ELASTOS InterfaceID EIID_IEllipticCurve;
interface IEncodedKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IEncodedKeySpec;
interface IMGF1ParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IMGF1ParameterSpec;
interface IPKCS8EncodedKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IPKCS8EncodedKeySpec;
interface IPSSParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IPSSParameterSpec;
interface IPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IPublicKey;
interface IRSAKeyGenParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IRSAKeyGenParameterSpec;
interface IRSAPrivateKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPrivateKeySpec;
interface IRSAMultiPrimePrivateCrtKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IRSAMultiPrimePrivateCrtKeySpec;
interface IRSAPrivateCrtKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPrivateCrtKeySpec;
interface IRSAPublicKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPublicKeySpec;
interface IX509EncodedKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IX509EncodedKeySpec;
}
}
}

namespace Elastos {
namespace Security {
namespace Interfaces {
interface IDSAKey;
EXTERN const _ELASTOS InterfaceID EIID_IDSAKey;
interface IDSAKeyPairGenerator;
EXTERN const _ELASTOS InterfaceID EIID_IDSAKeyPairGenerator;
interface IDSAParams;
EXTERN const _ELASTOS InterfaceID EIID_IDSAParams;
interface IDSAPrivateKey;
EXTERN const _ELASTOS InterfaceID EIID_IDSAPrivateKey;
interface IDSAPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IDSAPublicKey;
interface IECKey;
EXTERN const _ELASTOS InterfaceID EIID_IECKey;
interface IECPrivateKey;
EXTERN const _ELASTOS InterfaceID EIID_IECPrivateKey;
interface IECPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IECPublicKey;
interface IRSAKey;
EXTERN const _ELASTOS InterfaceID EIID_IRSAKey;
interface IRSAPrivateKey;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPrivateKey;
interface IRSAMultiPrimePrivateCrtKey;
EXTERN const _ELASTOS InterfaceID EIID_IRSAMultiPrimePrivateCrtKey;
interface IRSAPrivateCrtKey;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPrivateCrtKey;
interface IRSAPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IRSAPublicKey;
}
}
}

interface IPipedOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IPipedOutputStream;

namespace Elastos {
namespace IO {
namespace Channels {
interface IChannel;
EXTERN const _ELASTOS InterfaceID EIID_IChannel;
interface IReadableByteChannel;
EXTERN const _ELASTOS InterfaceID EIID_IReadableByteChannel;
interface IWritableByteChannel;
EXTERN const _ELASTOS InterfaceID EIID_IWritableByteChannel;
interface IChannels;
EXTERN const _ELASTOS InterfaceID EIID_IChannels;
interface IScatteringByteChannel;
EXTERN const _ELASTOS InterfaceID EIID_IScatteringByteChannel;
interface IGatheringByteChannel;
EXTERN const _ELASTOS InterfaceID EIID_IGatheringByteChannel;
interface IInterruptibleChannel;
EXTERN const _ELASTOS InterfaceID EIID_IInterruptibleChannel;
interface IFileChannel;
EXTERN const _ELASTOS InterfaceID EIID_IFileChannel;
interface ISelectableChannel;
EXTERN const _ELASTOS InterfaceID EIID_ISelectableChannel;
interface ISelectionKey;
EXTERN const _ELASTOS InterfaceID EIID_ISelectionKey;
interface ISelector;
EXTERN const _ELASTOS InterfaceID EIID_ISelector;
interface ISocketChannel;
EXTERN const _ELASTOS InterfaceID EIID_ISocketChannel;
interface ISocketChannelHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISocketChannelHelper;
interface IServerSocketChannel;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocketChannel;
interface IServerSocketChannelHelper;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocketChannelHelper;
interface IDatagramChannel;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramChannel;
interface IDatagramChannelHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramChannelHelper;
interface ISinkChannel;
EXTERN const _ELASTOS InterfaceID EIID_ISinkChannel;
interface ISourceChannel;
EXTERN const _ELASTOS InterfaceID EIID_ISourceChannel;
interface IPipe;
EXTERN const _ELASTOS InterfaceID EIID_IPipe;
interface IPipeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPipeHelper;
interface IFileLock;
EXTERN const _ELASTOS InterfaceID EIID_IFileLock;
interface IByteChannel;
EXTERN const _ELASTOS InterfaceID EIID_IByteChannel;
interface ICChannelsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICChannelsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CChannels;
EXTERN const _ELASTOS ClassID ECLSID_CChannelsClassObject;
}
}
}

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {
interface ISelectorProvider;
EXTERN const _ELASTOS InterfaceID EIID_ISelectorProvider;
interface ISelectorProviderHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISelectorProviderHelper;
interface IAbstractSelector;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractSelector;
interface IAbstractSelectionKey;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractSelectionKey;
interface IAbstractInterruptibleChannel;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractInterruptibleChannel;
interface IAbstractSelectableChannel;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractSelectableChannel;
interface ICSelectorProviderHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSelectorProviderHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorProviderHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSelectorProviderHelperClassObject;
}
}
}
}

namespace Elastos {
namespace IO {
namespace Charset {
interface ICharset;
EXTERN const _ELASTOS InterfaceID EIID_ICharset;
interface ICharsetHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICharsetHelper;
interface ICoderResult;
EXTERN const _ELASTOS InterfaceID EIID_ICoderResult;
interface ICoderResultHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICoderResultHelper;
interface ICharsetDecoder;
EXTERN const _ELASTOS InterfaceID EIID_ICharsetDecoder;
interface ICharsetEncoder;
EXTERN const _ELASTOS InterfaceID EIID_ICharsetEncoder;
interface ICodingErrorAction;
EXTERN const _ELASTOS InterfaceID EIID_ICodingErrorAction;
interface ICodingErrorActionHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICodingErrorActionHelper;
interface ICharsets;
EXTERN const _ELASTOS InterfaceID EIID_ICharsets;
interface IModifiedUtf8;
EXTERN const _ELASTOS InterfaceID EIID_IModifiedUtf8;
interface IStandardCharsets;
EXTERN const _ELASTOS InterfaceID EIID_IStandardCharsets;
interface ICCodingErrorActionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCodingErrorActionClassObject;
interface ICCodingErrorActionHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCodingErrorActionHelperClassObject;
interface ICCharsetHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharsetHelperClassObject;
interface ICCharsetsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharsetsClassObject;
interface ICCoderResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCoderResultClassObject;
interface ICCoderResultHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCoderResultHelperClassObject;
interface ICModifiedUtf8ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICModifiedUtf8ClassObject;
interface ICStandardCharsetsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStandardCharsetsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCodingErrorAction;
EXTERN const _ELASTOS ClassID ECLSID_CCodingErrorActionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCodingErrorActionHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCodingErrorActionHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharsetHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCharsetHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharsets;
EXTERN const _ELASTOS ClassID ECLSID_CCharsetsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCoderResult;
EXTERN const _ELASTOS ClassID ECLSID_CCoderResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCoderResultHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCoderResultHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CModifiedUtf8;
EXTERN const _ELASTOS ClassID ECLSID_CModifiedUtf8ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStandardCharsets;
EXTERN const _ELASTOS ClassID ECLSID_CStandardCharsetsClassObject;
}
}
}

namespace Elastos {
namespace IO {
namespace Charset {
namespace Spi {
interface ICharsetProvider;
EXTERN const _ELASTOS InterfaceID EIID_ICharsetProvider;
}
}
}
}

namespace Elastos {
namespace Net {
interface IAuthenticator;
EXTERN const _ELASTOS InterfaceID EIID_IAuthenticator;
interface IAuthenticatorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAuthenticatorHelper;
interface ICacheRequest;
EXTERN const _ELASTOS InterfaceID EIID_ICacheRequest;
interface ICacheResponse;
EXTERN const _ELASTOS InterfaceID EIID_ICacheResponse;
interface IContentHandler;
EXTERN const _ELASTOS InterfaceID EIID_IContentHandler;
interface IContentHandlerFactory;
EXTERN const _ELASTOS InterfaceID EIID_IContentHandlerFactory;
interface ICookieHandler;
EXTERN const _ELASTOS InterfaceID EIID_ICookieHandler;
interface ICookieHandlerHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICookieHandlerHelper;
interface IURLStreamHandler;
EXTERN const _ELASTOS InterfaceID EIID_IURLStreamHandler;
interface IURLStreamHandlerFactory;
EXTERN const _ELASTOS InterfaceID EIID_IURLStreamHandlerFactory;
interface IURI;
EXTERN const _ELASTOS InterfaceID EIID_IURI;
interface IURIHelper;
EXTERN const _ELASTOS InterfaceID EIID_IURIHelper;
interface IURL;
EXTERN const _ELASTOS InterfaceID EIID_IURL;
interface IURLHelper;
EXTERN const _ELASTOS InterfaceID EIID_IURLHelper;
interface ICookieManager;
EXTERN const _ELASTOS InterfaceID EIID_ICookieManager;
interface ICookiePolicy;
EXTERN const _ELASTOS InterfaceID EIID_ICookiePolicy;
interface ICookiePolicyHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICookiePolicyHelper;
interface ICookieStore;
EXTERN const _ELASTOS InterfaceID EIID_ICookieStore;
interface IDatagramPacket;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramPacket;
interface IDatagramSocket;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramSocket;
interface IDatagramSocketHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramSocketHelper;
interface ISocketOptions;
EXTERN const _ELASTOS InterfaceID EIID_ISocketOptions;
interface IDatagramSocketImpl;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramSocketImpl;
interface IDatagramSocketImplFactory;
EXTERN const _ELASTOS InterfaceID EIID_IDatagramSocketImplFactory;
interface IResponseCache;
EXTERN const _ELASTOS InterfaceID EIID_IResponseCache;
interface IResponseCacheHelper;
EXTERN const _ELASTOS InterfaceID EIID_IResponseCacheHelper;
interface IURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IURLConnection;
interface IURLConnectionHelper;
EXTERN const _ELASTOS InterfaceID EIID_IURLConnectionHelper;
interface IHttpURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpURLConnection;
interface IHttpURLConnectionHelper;
EXTERN const _ELASTOS InterfaceID EIID_IHttpURLConnectionHelper;
interface IFileNameMap;
EXTERN const _ELASTOS InterfaceID EIID_IFileNameMap;
interface IHttpCookie;
EXTERN const _ELASTOS InterfaceID EIID_IHttpCookie;
interface IHttpCookieHelper;
EXTERN const _ELASTOS InterfaceID EIID_IHttpCookieHelper;
interface IIDN;
EXTERN const _ELASTOS InterfaceID EIID_IIDN;
interface IInetAddress;
EXTERN const _ELASTOS InterfaceID EIID_IInetAddress;
interface IInetAddressHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInetAddressHelper;
interface IInet4Address;
EXTERN const _ELASTOS InterfaceID EIID_IInet4Address;
interface IInet4AddressHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInet4AddressHelper;
interface IInet6Address;
EXTERN const _ELASTOS InterfaceID EIID_IInet6Address;
interface IInet6AddressHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInet6AddressHelper;
interface ISocketAddress;
EXTERN const _ELASTOS InterfaceID EIID_ISocketAddress;
interface IInetSocketAddress;
EXTERN const _ELASTOS InterfaceID EIID_IInetSocketAddress;
interface IInetSocketAddressHelper;
EXTERN const _ELASTOS InterfaceID EIID_IInetSocketAddressHelper;
interface IInterfaceAddress;
EXTERN const _ELASTOS InterfaceID EIID_IInterfaceAddress;
interface IMulticastSocket;
EXTERN const _ELASTOS InterfaceID EIID_IMulticastSocket;
interface INetworkInterface;
EXTERN const _ELASTOS InterfaceID EIID_INetworkInterface;
interface INetworkInterfaceHelper;
EXTERN const _ELASTOS InterfaceID EIID_INetworkInterfaceHelper;
interface IPasswordAuthentication;
EXTERN const _ELASTOS InterfaceID EIID_IPasswordAuthentication;
interface IPlainDatagramSocketImpl;
EXTERN const _ELASTOS InterfaceID EIID_IPlainDatagramSocketImpl;
interface ISocketImpl;
EXTERN const _ELASTOS InterfaceID EIID_ISocketImpl;
interface IPlainSocketImpl;
EXTERN const _ELASTOS InterfaceID EIID_IPlainSocketImpl;
interface IPlainServerSocketImpl;
EXTERN const _ELASTOS InterfaceID EIID_IPlainServerSocketImpl;
interface IProxy;
EXTERN const _ELASTOS InterfaceID EIID_IProxy;
interface IProxyHelper;
EXTERN const _ELASTOS InterfaceID EIID_IProxyHelper;
interface IProxySelector;
EXTERN const _ELASTOS InterfaceID EIID_IProxySelector;
interface IProxySelectorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IProxySelectorHelper;
interface ISecureCacheResponse;
EXTERN const _ELASTOS InterfaceID EIID_ISecureCacheResponse;
interface IServerSocket;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocket;
interface IServerSocketHelper;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocketHelper;
interface ISocket;
EXTERN const _ELASTOS InterfaceID EIID_ISocket;
interface ISocketHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISocketHelper;
interface ISocketImplFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISocketImplFactory;
interface ISocks4Message;
EXTERN const _ELASTOS InterfaceID EIID_ISocks4Message;
interface IURLEncoder;
EXTERN const _ELASTOS InterfaceID EIID_IURLEncoder;
interface IURLDecoder;
EXTERN const _ELASTOS InterfaceID EIID_IURLDecoder;
interface IJarURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IJarURLConnection;
interface INetPermission;
EXTERN const _ELASTOS InterfaceID EIID_INetPermission;
interface ISocketPermission;
EXTERN const _ELASTOS InterfaceID EIID_ISocketPermission;
interface ICCookieManagerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieManagerClassObject;
interface ICDatagramPacketClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatagramPacketClassObject;
interface ICDatagramSocketClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatagramSocketClassObject;
interface ICHttpCookieClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpCookieClassObject;
interface ICHttpCookieHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpCookieHelperClassObject;
interface ICInet4AddressClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInet4AddressClassObject;
interface ICInet4AddressHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInet4AddressHelperClassObject;
interface ICInet6AddressClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInet6AddressClassObject;
interface ICInet6AddressHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInet6AddressHelperClassObject;
interface ICInetUnixAddressClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInetUnixAddressClassObject;
interface ICInetSocketAddressClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInetSocketAddressClassObject;
interface ICInetSocketAddressHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInetSocketAddressHelperClassObject;
interface ICMulticastSocketClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMulticastSocketClassObject;
interface ICNetPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNetPermissionClassObject;
interface ICPasswordAuthenticationClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPasswordAuthenticationClassObject;
interface ICPlainDatagramSocketImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainDatagramSocketImplClassObject;
interface ICPlainServerSocketImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainServerSocketImplClassObject;
interface ICPlainSocketImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainSocketImplClassObject;
interface ICPlainSocketInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainSocketInputStreamClassObject;
interface ICPlainSocketOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainSocketOutputStreamClassObject;
interface ICProxyClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICProxyClassObject;
interface ICProxyHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICProxyHelperClassObject;
interface ICResponseCacheHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseCacheHelperClassObject;
interface ICServerSocketClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICServerSocketClassObject;
interface ICServerSocketHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICServerSocketHelperClassObject;
interface ICSocketClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocketClassObject;
interface ICSocketHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocketHelperClassObject;
interface ICSocketPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocketPermissionClassObject;
interface ICSocks4MessageClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocks4MessageClassObject;
interface ICURIClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICURIClassObject;
interface ICURIHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICURIHelperClassObject;
interface ICURLClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICURLClassObject;
interface ICCookieHandlerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieHandlerHelperClassObject;
interface ICCookiePolicyHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookiePolicyHelperClassObject;
interface ICIDNClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICIDNClassObject;
interface ICInetAddressHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInetAddressHelperClassObject;
interface ICNetworkInterfaceHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNetworkInterfaceHelperClassObject;
interface ICURLEncoderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICURLEncoderClassObject;
interface ICURLDecoderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICURLDecoderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieManager;
EXTERN const _ELASTOS ClassID ECLSID_CCookieManagerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatagramPacket;
EXTERN const _ELASTOS ClassID ECLSID_CDatagramPacketClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatagramSocket;
EXTERN const _ELASTOS ClassID ECLSID_CDatagramSocketClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpCookie;
EXTERN const _ELASTOS ClassID ECLSID_CHttpCookieClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpCookieHelper;
EXTERN const _ELASTOS ClassID ECLSID_CHttpCookieHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInet4Address;
EXTERN const _ELASTOS ClassID ECLSID_CInet4AddressClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInet4AddressHelper;
EXTERN const _ELASTOS ClassID ECLSID_CInet4AddressHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInet6Address;
EXTERN const _ELASTOS ClassID ECLSID_CInet6AddressClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInet6AddressHelper;
EXTERN const _ELASTOS ClassID ECLSID_CInet6AddressHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInetUnixAddress;
EXTERN const _ELASTOS ClassID ECLSID_CInetUnixAddressClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInetSocketAddress;
EXTERN const _ELASTOS ClassID ECLSID_CInetSocketAddressClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInetSocketAddressHelper;
EXTERN const _ELASTOS ClassID ECLSID_CInetSocketAddressHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMulticastSocket;
EXTERN const _ELASTOS ClassID ECLSID_CMulticastSocketClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNetPermission;
EXTERN const _ELASTOS ClassID ECLSID_CNetPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPasswordAuthentication;
EXTERN const _ELASTOS ClassID ECLSID_CPasswordAuthenticationClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainDatagramSocketImpl;
EXTERN const _ELASTOS ClassID ECLSID_CPlainDatagramSocketImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainServerSocketImpl;
EXTERN const _ELASTOS ClassID ECLSID_CPlainServerSocketImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketImpl;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CProxy;
EXTERN const _ELASTOS ClassID ECLSID_CProxyClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CProxyHelper;
EXTERN const _ELASTOS ClassID ECLSID_CProxyHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseCacheHelper;
EXTERN const _ELASTOS ClassID ECLSID_CResponseCacheHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocket;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocketClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocketHelper;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocketHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocket;
EXTERN const _ELASTOS ClassID ECLSID_CSocketClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocketHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSocketHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocketPermission;
EXTERN const _ELASTOS ClassID ECLSID_CSocketPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocks4Message;
EXTERN const _ELASTOS ClassID ECLSID_CSocks4MessageClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CURI;
EXTERN const _ELASTOS ClassID ECLSID_CURIClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CURIHelper;
EXTERN const _ELASTOS ClassID ECLSID_CURIHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CURL;
EXTERN const _ELASTOS ClassID ECLSID_CURLClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieHandlerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCookieHandlerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookiePolicyHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCookiePolicyHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CIDN;
EXTERN const _ELASTOS ClassID ECLSID_CIDNClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInetAddressHelper;
EXTERN const _ELASTOS ClassID ECLSID_CInetAddressHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNetworkInterfaceHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNetworkInterfaceHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CURLEncoder;
EXTERN const _ELASTOS ClassID ECLSID_CURLEncoderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CURLDecoder;
EXTERN const _ELASTOS ClassID ECLSID_CURLDecoderClassObject;
}
}

namespace Elastos {
namespace Text {
interface IAnnotation;
EXTERN const _ELASTOS InterfaceID EIID_IAnnotation;
interface IAttributedCharacterIteratorAttribute;
EXTERN const _ELASTOS InterfaceID EIID_IAttributedCharacterIteratorAttribute;
interface IAttributedCharacterIteratorAttributeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAttributedCharacterIteratorAttributeHelper;
interface IAttributedCharacterIterator;
EXTERN const _ELASTOS InterfaceID EIID_IAttributedCharacterIterator;
interface IAttributedString;
EXTERN const _ELASTOS InterfaceID EIID_IAttributedString;
interface IBidi;
EXTERN const _ELASTOS InterfaceID EIID_IBidi;
interface IBidiHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBidiHelper;
interface IBreakIterator;
EXTERN const _ELASTOS InterfaceID EIID_IBreakIterator;
interface IBreakIteratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBreakIteratorHelper;
interface ICharacterIterator;
EXTERN const _ELASTOS InterfaceID EIID_ICharacterIterator;
interface IChoiceFormat;
EXTERN const _ELASTOS InterfaceID EIID_IChoiceFormat;
interface IChoiceFormatHelper;
EXTERN const _ELASTOS InterfaceID EIID_IChoiceFormatHelper;
interface ICollationElementIterator;
EXTERN const _ELASTOS InterfaceID EIID_ICollationElementIterator;
interface ICollationElementIteratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICollationElementIteratorHelper;
interface ICollationKey;
EXTERN const _ELASTOS InterfaceID EIID_ICollationKey;
interface ICollator;
EXTERN const _ELASTOS InterfaceID EIID_ICollator;
interface ICollatorHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICollatorHelper;
interface IDateFormatField;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormatField;
interface IDateFormatFieldHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormatFieldHelper;
interface IDateFormat;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormat;
interface IDateFormatHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormatHelper;
interface IDateFormatSymbols;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormatSymbols;
interface IDateFormatSymbolsHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDateFormatSymbolsHelper;
interface IDecimalFormat;
EXTERN const _ELASTOS InterfaceID EIID_IDecimalFormat;
interface IDecimalFormatSymbols;
EXTERN const _ELASTOS InterfaceID EIID_IDecimalFormatSymbols;
interface IDecimalFormatSymbolsHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDecimalFormatSymbolsHelper;
interface IFieldPosition;
EXTERN const _ELASTOS InterfaceID EIID_IFieldPosition;
interface IFormatField;
EXTERN const _ELASTOS InterfaceID EIID_IFormatField;
interface IFormat;
EXTERN const _ELASTOS InterfaceID EIID_IFormat;
interface IMessageFormatField;
EXTERN const _ELASTOS InterfaceID EIID_IMessageFormatField;
interface IMessageFormat;
EXTERN const _ELASTOS InterfaceID EIID_IMessageFormat;
interface INormalizer;
EXTERN const _ELASTOS InterfaceID EIID_INormalizer;
interface INumberFormatField;
EXTERN const _ELASTOS InterfaceID EIID_INumberFormatField;
interface INumberFormat;
EXTERN const _ELASTOS InterfaceID EIID_INumberFormat;
interface INumberFormatHelper;
EXTERN const _ELASTOS InterfaceID EIID_INumberFormatHelper;
interface IParsePosition;
EXTERN const _ELASTOS InterfaceID EIID_IParsePosition;
interface IRuleBasedCollator;
EXTERN const _ELASTOS InterfaceID EIID_IRuleBasedCollator;
interface ISimpleDateFormat;
EXTERN const _ELASTOS InterfaceID EIID_ISimpleDateFormat;
interface IStringCharacterIterator;
EXTERN const _ELASTOS InterfaceID EIID_IStringCharacterIterator;
interface ICAnnotationClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAnnotationClassObject;
interface ICAttributedCharacterIteratorAttributeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributedCharacterIteratorAttributeClassObject;
interface ICBidiClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBidiClassObject;
interface ICBidiHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBidiHelperClassObject;
interface ICBreakIteratorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBreakIteratorHelperClassObject;
interface ICRuleBasedBreakIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRuleBasedBreakIteratorClassObject;
interface ICFieldPositionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFieldPositionClassObject;
interface ICParsePositionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICParsePositionClassObject;
interface ICAttributedStringClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributedStringClassObject;
interface ICChoiceFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICChoiceFormatClassObject;
interface ICChoiceFormatHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICChoiceFormatHelperClassObject;
interface ICNumberFormatFieldClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNumberFormatFieldClassObject;
interface ICNumberFormatHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNumberFormatHelperClassObject;
interface ICDecimalFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDecimalFormatClassObject;
interface ICDecimalFormatSymbolsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDecimalFormatSymbolsClassObject;
interface ICDecimalFormatSymbolsHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDecimalFormatSymbolsHelperClassObject;
interface ICMessageFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMessageFormatClassObject;
interface ICSimpleDateFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSimpleDateFormatClassObject;
interface ICDateFormatHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateFormatHelperClassObject;
interface ICNormalizerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNormalizerHelperClassObject;
interface ICMessageFormatFieldClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMessageFormatFieldClassObject;
interface ICDateFormatSymbolsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateFormatSymbolsClassObject;
interface ICDateFormatFieldClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateFormatFieldClassObject;
interface ICDateFormatSymbolsHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateFormatSymbolsHelperClassObject;
interface ICCollationElementIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollationElementIteratorClassObject;
interface ICCollationElementIteratorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollationElementIteratorHelperClassObject;
interface ICRuleBasedCollatorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRuleBasedCollatorClassObject;
interface ICStringCharacterIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringCharacterIteratorClassObject;
interface ICCollatorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollatorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAnnotation;
EXTERN const _ELASTOS ClassID ECLSID_CAnnotationClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributedCharacterIteratorAttribute;
EXTERN const _ELASTOS ClassID ECLSID_CAttributedCharacterIteratorAttributeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBidi;
EXTERN const _ELASTOS ClassID ECLSID_CBidiClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBidiHelper;
EXTERN const _ELASTOS ClassID ECLSID_CBidiHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBreakIteratorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CBreakIteratorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedBreakIterator;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedBreakIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFieldPosition;
EXTERN const _ELASTOS ClassID ECLSID_CFieldPositionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CParsePosition;
EXTERN const _ELASTOS ClassID ECLSID_CParsePositionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributedString;
EXTERN const _ELASTOS ClassID ECLSID_CAttributedStringClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CChoiceFormat;
EXTERN const _ELASTOS ClassID ECLSID_CChoiceFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CChoiceFormatHelper;
EXTERN const _ELASTOS ClassID ECLSID_CChoiceFormatHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNumberFormatField;
EXTERN const _ELASTOS ClassID ECLSID_CNumberFormatFieldClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNumberFormatHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNumberFormatHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormat;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormatSymbols;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormatSymbolsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormatSymbolsHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDecimalFormatSymbolsHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMessageFormat;
EXTERN const _ELASTOS ClassID ECLSID_CMessageFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSimpleDateFormat;
EXTERN const _ELASTOS ClassID ECLSID_CSimpleDateFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNormalizerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNormalizerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMessageFormatField;
EXTERN const _ELASTOS ClassID ECLSID_CMessageFormatFieldClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatSymbols;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatSymbolsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatField;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatFieldClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatSymbolsHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDateFormatSymbolsHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIterator;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIteratorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIteratorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedCollator;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedCollatorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringCharacterIterator;
EXTERN const _ELASTOS ClassID ECLSID_CStringCharacterIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollatorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCollatorHelperClassObject;
}
}

namespace Elastos {
namespace Utility {
interface IRandom;
EXTERN const _ELASTOS InterfaceID EIID_IRandom;
interface IBitSet;
EXTERN const _ELASTOS InterfaceID EIID_IBitSet;
interface IBitSetHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBitSetHelper;
interface IDate;
EXTERN const _ELASTOS InterfaceID EIID_IDate;
interface IDateHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDateHelper;
interface ICalendar;
EXTERN const _ELASTOS InterfaceID EIID_ICalendar;
interface ICalendarHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICalendarHelper;
interface IIterable;
EXTERN const _ELASTOS InterfaceID EIID_IIterable;
interface ICollection;
EXTERN const _ELASTOS InterfaceID EIID_ICollection;
interface IList;
EXTERN const _ELASTOS InterfaceID EIID_IList;
interface ILocale;
EXTERN const _ELASTOS InterfaceID EIID_ILocale;
interface IBuilder;
EXTERN const _ELASTOS InterfaceID EIID_IBuilder;
interface ILocaleHelper;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleHelper;
interface ILocaleBuilder;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleBuilder;
interface IMapEntry;
EXTERN const _ELASTOS InterfaceID EIID_IMapEntry;
interface IMap;
EXTERN const _ELASTOS InterfaceID EIID_IMap;
interface IHashMap;
EXTERN const _ELASTOS InterfaceID EIID_IHashMap;
interface IHashSet;
EXTERN const _ELASTOS InterfaceID EIID_IHashSet;
interface IHashTable;
EXTERN const _ELASTOS InterfaceID EIID_IHashTable;
interface IIdentityHashMap;
EXTERN const _ELASTOS InterfaceID EIID_IIdentityHashMap;
interface ILinkedHashMap;
EXTERN const _ELASTOS InterfaceID EIID_ILinkedHashMap;
interface ILinkedHashSet;
EXTERN const _ELASTOS InterfaceID EIID_ILinkedHashSet;
interface ILinkedList;
EXTERN const _ELASTOS InterfaceID EIID_ILinkedList;
interface IDictionary;
EXTERN const _ELASTOS InterfaceID EIID_IDictionary;
interface IProperties;
EXTERN const _ELASTOS InterfaceID EIID_IProperties;
interface IEnumeration;
EXTERN const _ELASTOS InterfaceID EIID_IEnumeration;
interface IGregorianCalendar;
EXTERN const _ELASTOS InterfaceID EIID_IGregorianCalendar;
interface IIterator;
EXTERN const _ELASTOS InterfaceID EIID_IIterator;
interface IListIterator;
EXTERN const _ELASTOS InterfaceID EIID_IListIterator;
interface IQueue;
EXTERN const _ELASTOS InterfaceID EIID_IQueue;
interface IDeque;
EXTERN const _ELASTOS InterfaceID EIID_IDeque;
interface IWeakHashMap;
EXTERN const _ELASTOS InterfaceID EIID_IWeakHashMap;
interface ITreeMap;
EXTERN const _ELASTOS InterfaceID EIID_ITreeMap;
interface ITreeSet;
EXTERN const _ELASTOS InterfaceID EIID_ITreeSet;
interface IRandomAccess;
EXTERN const _ELASTOS InterfaceID EIID_IRandomAccess;
interface ITimeZone;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZone;
interface ITimeZoneHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZoneHelper;
interface ISimpleTimeZone;
EXTERN const _ELASTOS InterfaceID EIID_ISimpleTimeZone;
interface ITimerTask;
EXTERN const _ELASTOS InterfaceID EIID_ITimerTask;
interface ITimer;
EXTERN const _ELASTOS InterfaceID EIID_ITimer;
interface ICurrency;
EXTERN const _ELASTOS InterfaceID EIID_ICurrency;
interface ICurrencyHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICurrencyHelper;
interface IUUID;
EXTERN const _ELASTOS InterfaceID EIID_IUUID;
interface IUUIDHelper;
EXTERN const _ELASTOS InterfaceID EIID_IUUIDHelper;
interface ITimeZoneGetter;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZoneGetter;
interface ITimeZoneGetterHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZoneGetterHelper;
interface IArrayList;
EXTERN const _ELASTOS InterfaceID EIID_IArrayList;
interface IFormattableFlags;
EXTERN const _ELASTOS InterfaceID EIID_IFormattableFlags;
interface IFormatter;
EXTERN const _ELASTOS InterfaceID EIID_IFormatter;
interface IFormattable;
EXTERN const _ELASTOS InterfaceID EIID_IFormattable;
interface IArrayDeque;
EXTERN const _ELASTOS InterfaceID EIID_IArrayDeque;
interface IEventListener;
EXTERN const _ELASTOS InterfaceID EIID_IEventListener;
interface IEventListenerProxy;
EXTERN const _ELASTOS InterfaceID EIID_IEventListenerProxy;
interface IEventObject;
EXTERN const _ELASTOS InterfaceID EIID_IEventObject;
interface IResourceBundle;
EXTERN const _ELASTOS InterfaceID EIID_IResourceBundle;
interface IResourceBundleControl;
EXTERN const _ELASTOS InterfaceID EIID_IResourceBundleControl;
interface IResourceBundleHelper;
EXTERN const _ELASTOS InterfaceID EIID_IResourceBundleHelper;
interface IListResourceBundle;
EXTERN const _ELASTOS InterfaceID EIID_IListResourceBundle;
interface ISortedMap;
EXTERN const _ELASTOS InterfaceID EIID_ISortedMap;
interface INavigableMap;
EXTERN const _ELASTOS InterfaceID EIID_INavigableMap;
interface ISet;
EXTERN const _ELASTOS InterfaceID EIID_ISet;
interface ISortedSet;
EXTERN const _ELASTOS InterfaceID EIID_ISortedSet;
interface INavigableSet;
EXTERN const _ELASTOS InterfaceID EIID_INavigableSet;
interface IObserver;
EXTERN const _ELASTOS InterfaceID EIID_IObserver;
interface IObservable;
EXTERN const _ELASTOS InterfaceID EIID_IObservable;
interface IPriorityQueue;
EXTERN const _ELASTOS InterfaceID EIID_IPriorityQueue;
interface IEnumSet;
EXTERN const _ELASTOS InterfaceID EIID_IEnumSet;
interface IEnumSetHelper;
EXTERN const _ELASTOS InterfaceID EIID_IEnumSetHelper;
interface IHugeEnumSet;
EXTERN const _ELASTOS InterfaceID EIID_IHugeEnumSet;
interface IMiniEnumSet;
EXTERN const _ELASTOS InterfaceID EIID_IMiniEnumSet;
interface IPropertyResourceBundle;
EXTERN const _ELASTOS InterfaceID EIID_IPropertyResourceBundle;
interface IScanner;
EXTERN const _ELASTOS InterfaceID EIID_IScanner;
interface IServiceLoader;
EXTERN const _ELASTOS InterfaceID EIID_IServiceLoader;
interface IServiceLoaderHelper;
EXTERN const _ELASTOS InterfaceID EIID_IServiceLoaderHelper;
interface IVector;
EXTERN const _ELASTOS InterfaceID EIID_IVector;
interface IStack;
EXTERN const _ELASTOS InterfaceID EIID_IStack;
interface IStringTokenizer;
EXTERN const _ELASTOS InterfaceID EIID_IStringTokenizer;
interface IUnsafeArrayList;
EXTERN const _ELASTOS InterfaceID EIID_IUnsafeArrayList;
interface IPropertyPermission;
EXTERN const _ELASTOS InterfaceID EIID_IPropertyPermission;
interface ICollections;
EXTERN const _ELASTOS InterfaceID EIID_ICollections;
interface IEnumMap;
EXTERN const _ELASTOS InterfaceID EIID_IEnumMap;
interface ICRandomClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRandomClassObject;
interface ICStringTokenizerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringTokenizerClassObject;
interface ICArrayDequeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICArrayDequeClassObject;
interface ICArrayListClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICArrayListClassObject;
interface ICArrayListIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICArrayListIteratorClassObject;
interface ICBitSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBitSetClassObject;
interface ICBitSetHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBitSetHelperClassObject;
interface ICEnumMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEnumMapClassObject;
interface ICHashMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHashMapClassObject;
interface ICHashSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHashSetClassObject;
interface ICHashTableClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHashTableClassObject;
interface ICIdentityHashMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICIdentityHashMapClassObject;
interface ICLinkedHashMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedHashMapClassObject;
interface ICLinkedHashSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedHashSetClassObject;
interface ICLinkedListClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedListClassObject;
interface ICVectorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICVectorClassObject;
interface ICStackClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStackClassObject;
interface ICWeakHashMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICWeakHashMapClassObject;
interface ICTreeMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTreeMapClassObject;
interface ICTreeSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTreeSetClassObject;
interface ICPriorityQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPriorityQueueClassObject;
interface ICHugeEnumSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHugeEnumSetClassObject;
interface ICMiniEnumSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMiniEnumSetClassObject;
interface ICUnsafeArrayListClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUnsafeArrayListClassObject;
interface ICCollectionsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollectionsClassObject;
interface ICEnumSetHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEnumSetHelperClassObject;
interface ICCalendarHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCalendarHelperClassObject;
interface ICCurrencyHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCurrencyHelperClassObject;
interface ICDateHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateHelperClassObject;
interface ICGregorianCalendarClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICGregorianCalendarClassObject;
interface ICTimerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimerClassObject;
interface ICUUIDClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUUIDClassObject;
interface ICUUIDHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUUIDHelperClassObject;
interface ICTimeZoneHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimeZoneHelperClassObject;
interface ICLocaleClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocaleClassObject;
interface ICLocaleHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocaleHelperClassObject;
interface ICLocaleBuilderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocaleBuilderClassObject;
interface ICDateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateClassObject;
interface ICPropertiesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPropertiesClassObject;
interface ICPropertyResourceBundleClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPropertyResourceBundleClassObject;
interface ICFormatterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFormatterClassObject;
interface ICScannerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICScannerClassObject;
interface ICSimpleTimeZoneClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSimpleTimeZoneClassObject;
interface ICObservableClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICObservableClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRandom;
EXTERN const _ELASTOS ClassID ECLSID_CRandomClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringTokenizer;
EXTERN const _ELASTOS ClassID ECLSID_CStringTokenizerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CArrayDeque;
EXTERN const _ELASTOS ClassID ECLSID_CArrayDequeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CArrayList;
EXTERN const _ELASTOS ClassID ECLSID_CArrayListClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CArrayListIterator;
EXTERN const _ELASTOS ClassID ECLSID_CArrayListIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBitSet;
EXTERN const _ELASTOS ClassID ECLSID_CBitSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBitSetHelper;
EXTERN const _ELASTOS ClassID ECLSID_CBitSetHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEnumMap;
EXTERN const _ELASTOS ClassID ECLSID_CEnumMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHashMap;
EXTERN const _ELASTOS ClassID ECLSID_CHashMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHashSet;
EXTERN const _ELASTOS ClassID ECLSID_CHashSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHashTable;
EXTERN const _ELASTOS ClassID ECLSID_CHashTableClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CIdentityHashMap;
EXTERN const _ELASTOS ClassID ECLSID_CIdentityHashMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedHashMap;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedHashMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedHashSet;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedHashSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedList;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedListClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CVector;
EXTERN const _ELASTOS ClassID ECLSID_CVectorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStack;
EXTERN const _ELASTOS ClassID ECLSID_CStackClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CWeakHashMap;
EXTERN const _ELASTOS ClassID ECLSID_CWeakHashMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTreeMap;
EXTERN const _ELASTOS ClassID ECLSID_CTreeMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTreeSet;
EXTERN const _ELASTOS ClassID ECLSID_CTreeSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPriorityQueue;
EXTERN const _ELASTOS ClassID ECLSID_CPriorityQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHugeEnumSet;
EXTERN const _ELASTOS ClassID ECLSID_CHugeEnumSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMiniEnumSet;
EXTERN const _ELASTOS ClassID ECLSID_CMiniEnumSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUnsafeArrayList;
EXTERN const _ELASTOS ClassID ECLSID_CUnsafeArrayListClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollections;
EXTERN const _ELASTOS ClassID ECLSID_CCollectionsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEnumSetHelper;
EXTERN const _ELASTOS ClassID ECLSID_CEnumSetHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCalendarHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCalendarHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCurrencyHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCurrencyHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDateHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CGregorianCalendar;
EXTERN const _ELASTOS ClassID ECLSID_CGregorianCalendarClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTimer;
EXTERN const _ELASTOS ClassID ECLSID_CTimerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUUID;
EXTERN const _ELASTOS ClassID ECLSID_CUUIDClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUUIDHelper;
EXTERN const _ELASTOS ClassID ECLSID_CUUIDHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTimeZoneHelper;
EXTERN const _ELASTOS ClassID ECLSID_CTimeZoneHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocale;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleHelper;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleBuilder;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleBuilderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDate;
EXTERN const _ELASTOS ClassID ECLSID_CDateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CProperties;
EXTERN const _ELASTOS ClassID ECLSID_CPropertiesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPropertyResourceBundle;
EXTERN const _ELASTOS ClassID ECLSID_CPropertyResourceBundleClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFormatter;
EXTERN const _ELASTOS ClassID ECLSID_CFormatterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CScanner;
EXTERN const _ELASTOS ClassID ECLSID_CScannerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSimpleTimeZone;
EXTERN const _ELASTOS ClassID ECLSID_CSimpleTimeZoneClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CObservable;
EXTERN const _ELASTOS ClassID ECLSID_CObservableClassObject;
}
}

namespace Elastos {
namespace Utility {
namespace Regex {
interface IMatchResult;
EXTERN const _ELASTOS InterfaceID EIID_IMatchResult;
interface IMatcher;
EXTERN const _ELASTOS InterfaceID EIID_IMatcher;
interface IMatcherHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMatcherHelper;
interface ISplitter;
EXTERN const _ELASTOS InterfaceID EIID_ISplitter;
interface IPattern;
EXTERN const _ELASTOS InterfaceID EIID_IPattern;
interface IPatternHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPatternHelper;
interface ICMatcherClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMatcherClassObject;
interface ICMatcherHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMatcherHelperClassObject;
interface ICSplitterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSplitterClassObject;
interface ICPatternHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPatternHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMatcher;
EXTERN const _ELASTOS ClassID ECLSID_CMatcherClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMatcherHelper;
EXTERN const _ELASTOS ClassID ECLSID_CMatcherHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSplitter;
EXTERN const _ELASTOS ClassID ECLSID_CSplitterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPatternHelper;
EXTERN const _ELASTOS ClassID ECLSID_CPatternHelperClassObject;
}
}
}

namespace Elastos {
namespace Utility {
namespace Logging {
interface ILevel;
EXTERN const _ELASTOS InterfaceID EIID_ILevel;
interface ILevelHelper;
EXTERN const _ELASTOS InterfaceID EIID_ILevelHelper;
interface IErrorManager;
EXTERN const _ELASTOS InterfaceID EIID_IErrorManager;
interface IFilter;
EXTERN const _ELASTOS InterfaceID EIID_IFilter;
interface ILogFormatter;
EXTERN const _ELASTOS InterfaceID EIID_ILogFormatter;
interface ILogHandler;
EXTERN const _ELASTOS InterfaceID EIID_ILogHandler;
interface ILogger;
EXTERN const _ELASTOS InterfaceID EIID_ILogger;
interface IErrorManagerHelper;
EXTERN const _ELASTOS InterfaceID EIID_IErrorManagerHelper;
interface ILoggingMXBean;
EXTERN const _ELASTOS InterfaceID EIID_ILoggingMXBean;
interface ILogManager;
EXTERN const _ELASTOS InterfaceID EIID_ILogManager;
interface ILogManagerHelper;
EXTERN const _ELASTOS InterfaceID EIID_ILogManagerHelper;
interface ILogRecord;
EXTERN const _ELASTOS InterfaceID EIID_ILogRecord;
interface IStreamHandler;
EXTERN const _ELASTOS InterfaceID EIID_IStreamHandler;
interface IConsoleHandler;
EXTERN const _ELASTOS InterfaceID EIID_IConsoleHandler;
interface IMemoryHandler;
EXTERN const _ELASTOS InterfaceID EIID_IMemoryHandler;
interface ISimpleFormatter;
EXTERN const _ELASTOS InterfaceID EIID_ISimpleFormatter;
interface ISocketHandler;
EXTERN const _ELASTOS InterfaceID EIID_ISocketHandler;
interface IXMLFormatter;
EXTERN const _ELASTOS InterfaceID EIID_IXMLFormatter;
}
}
}

namespace Elastos {
namespace Utility {
namespace Prefs {
interface INodeChangeEvent;
EXTERN const _ELASTOS InterfaceID EIID_INodeChangeEvent;
interface INodeChangeListener;
EXTERN const _ELASTOS InterfaceID EIID_INodeChangeListener;
interface IPreferenceChangeEvent;
EXTERN const _ELASTOS InterfaceID EIID_IPreferenceChangeEvent;
interface IPreferenceChangeListener;
EXTERN const _ELASTOS InterfaceID EIID_IPreferenceChangeListener;
interface IPreferences;
EXTERN const _ELASTOS InterfaceID EIID_IPreferences;
interface IPreferencesHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPreferencesHelper;
interface IPreferencesFactory;
EXTERN const _ELASTOS InterfaceID EIID_IPreferencesFactory;
interface IXMLParserHelper;
EXTERN const _ELASTOS InterfaceID EIID_IXMLParserHelper;
interface IAbstractPreferences;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractPreferences;
interface IFilePreferencesFactoryImpl;
EXTERN const _ELASTOS InterfaceID EIID_IFilePreferencesFactoryImpl;
interface IFilePreferencesImpl;
EXTERN const _ELASTOS InterfaceID EIID_IFilePreferencesImpl;
}
}
}

namespace Elastos {
namespace Utility {
namespace Spi {
interface ILocaleServiceProvider;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleServiceProvider;
interface ICurrencyNameProvider;
EXTERN const _ELASTOS InterfaceID EIID_ICurrencyNameProvider;
interface ILocaleNameProvider;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleNameProvider;
interface ITimeZoneNameProvider;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZoneNameProvider;
}
}
}

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {
interface IAtomicBoolean;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicBoolean;
interface IAtomicInteger32;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger32;
interface IAtomicInteger32Array;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger32Array;
interface IAtomicInteger32FieldUpdater;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger32FieldUpdater;
interface IAtomicInteger64;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger64;
interface IAtomicInteger64Array;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger64Array;
interface IAtomicInteger64FieldUpdater;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicInteger64FieldUpdater;
interface IAtomicMarkableReference;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicMarkableReference;
interface IAtomicReference;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicReference;
interface IAtomicReferenceArray;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicReferenceArray;
interface IAtomicReferenceFieldUpdater;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicReferenceFieldUpdater;
interface IAtomicStampedReference;
EXTERN const _ELASTOS InterfaceID EIID_IAtomicStampedReference;
interface IFences;
EXTERN const _ELASTOS InterfaceID EIID_IFences;
interface ICAtomicBooleanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicBooleanClassObject;
interface ICAtomicInteger32ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicInteger32ClassObject;
interface ICAtomicInteger32ArrayClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicInteger32ArrayClassObject;
interface ICAtomicInteger64ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicInteger64ClassObject;
interface ICAtomicInteger64ArrayClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicInteger64ArrayClassObject;
interface ICAtomicMarkableReferenceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicMarkableReferenceClassObject;
interface ICAtomicReferenceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicReferenceClassObject;
interface ICAtomicReferenceArrayClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicReferenceArrayClassObject;
interface ICAtomicStampedReferenceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAtomicStampedReferenceClassObject;
interface ICFencesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFencesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicBoolean;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicBooleanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger32;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger32ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger32Array;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger32ArrayClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger64;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger64ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger64Array;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicInteger64ArrayClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicMarkableReference;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicMarkableReferenceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicReference;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicReferenceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicReferenceArray;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicReferenceArrayClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicStampedReference;
EXTERN const _ELASTOS ClassID ECLSID_CAtomicStampedReferenceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFences;
EXTERN const _ELASTOS ClassID ECLSID_CFencesClassObject;
}
}
}
}

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {
interface ILockSupport;
EXTERN const _ELASTOS InterfaceID EIID_ILockSupport;
interface IAbstractQueuedInteger64Synchronizer;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractQueuedInteger64Synchronizer;
interface IConditionObject;
EXTERN const _ELASTOS InterfaceID EIID_IConditionObject;
interface IAbstractQueuedSynchronizer;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractQueuedSynchronizer;
interface IAbstractQueuedSynchronizerHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractQueuedSynchronizerHelper;
interface ICondition;
EXTERN const _ELASTOS InterfaceID EIID_ICondition;
interface ILock;
EXTERN const _ELASTOS InterfaceID EIID_ILock;
interface IReadWriteLock;
EXTERN const _ELASTOS InterfaceID EIID_IReadWriteLock;
interface IReentrantLock;
EXTERN const _ELASTOS InterfaceID EIID_IReentrantLock;
interface IReentrantReadWriteLock;
EXTERN const _ELASTOS InterfaceID EIID_IReentrantReadWriteLock;
interface ICLockSupportClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLockSupportClassObject;
interface ICReentrantLockClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICReentrantLockClassObject;
interface ICReentrantReadWriteLockClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICReentrantReadWriteLockClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLockSupport;
EXTERN const _ELASTOS ClassID ECLSID_CLockSupportClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CReentrantLock;
EXTERN const _ELASTOS ClassID ECLSID_CReentrantLockClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CReentrantReadWriteLock;
EXTERN const _ELASTOS ClassID ECLSID_CReentrantReadWriteLockClassObject;
}
}
}
}

namespace Elastos {
namespace Utility {
namespace Concurrent {
interface IBlockingQueue;
EXTERN const _ELASTOS InterfaceID EIID_IBlockingQueue;
interface ICallable;
EXTERN const _ELASTOS InterfaceID EIID_ICallable;
interface ICompletionService;
EXTERN const _ELASTOS InterfaceID EIID_ICompletionService;
interface ICountDownLatch;
EXTERN const _ELASTOS InterfaceID EIID_ICountDownLatch;
interface IExecutor;
EXTERN const _ELASTOS InterfaceID EIID_IExecutor;
interface IExecutors;
EXTERN const _ELASTOS InterfaceID EIID_IExecutors;
interface IExecutorService;
EXTERN const _ELASTOS InterfaceID EIID_IExecutorService;
interface IAbstractExecutorService;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractExecutorService;
interface IFuture;
EXTERN const _ELASTOS InterfaceID EIID_IFuture;
interface IRejectedExecutionHandler;
EXTERN const _ELASTOS InterfaceID EIID_IRejectedExecutionHandler;
interface IRunnableFuture;
EXTERN const _ELASTOS InterfaceID EIID_IRunnableFuture;
interface IThreadFactory;
EXTERN const _ELASTOS InterfaceID EIID_IThreadFactory;
interface IThreadPoolExecutor;
EXTERN const _ELASTOS InterfaceID EIID_IThreadPoolExecutor;
interface ITimeUnit;
EXTERN const _ELASTOS InterfaceID EIID_ITimeUnit;
interface ITimeUnitHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITimeUnitHelper;
interface IArrayBlockingQueue;
EXTERN const _ELASTOS InterfaceID EIID_IArrayBlockingQueue;
interface IBlockingDeque;
EXTERN const _ELASTOS InterfaceID EIID_IBlockingDeque;
interface IPriorityBlockingQueue;
EXTERN const _ELASTOS InterfaceID EIID_IPriorityBlockingQueue;
interface IConcurrentLinkedQueue;
EXTERN const _ELASTOS InterfaceID EIID_IConcurrentLinkedQueue;
interface ICopyOnWriteArrayList;
EXTERN const _ELASTOS InterfaceID EIID_ICopyOnWriteArrayList;
interface ICopyOnWriteArrayListHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICopyOnWriteArrayListHelper;
interface ICyclicBarrier;
EXTERN const _ELASTOS InterfaceID EIID_ICyclicBarrier;
interface IDelayed;
EXTERN const _ELASTOS InterfaceID EIID_IDelayed;
interface IDelayQueue;
EXTERN const _ELASTOS InterfaceID EIID_IDelayQueue;
interface IExchanger;
EXTERN const _ELASTOS InterfaceID EIID_IExchanger;
interface IForkJoinPoolManagedBlocker;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinPoolManagedBlocker;
interface IForkJoinPool;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinPool;
interface IForkJoinPoolForkJoinWorkerThreadFactory;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinPoolForkJoinWorkerThreadFactory;
interface IForkJoinPoolHelper;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinPoolHelper;
interface IForkJoinWorkerThread;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinWorkerThread;
interface IForkJoinTask;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinTask;
interface IForkJoinTaskHelper;
EXTERN const _ELASTOS InterfaceID EIID_IForkJoinTaskHelper;
interface IPhaser;
EXTERN const _ELASTOS InterfaceID EIID_IPhaser;
interface IRunnableScheduledFuture;
EXTERN const _ELASTOS InterfaceID EIID_IRunnableScheduledFuture;
interface ISemaphore;
EXTERN const _ELASTOS InterfaceID EIID_ISemaphore;
interface IWaitQueue;
EXTERN const _ELASTOS InterfaceID EIID_IWaitQueue;
interface ILifoWaitQueue;
EXTERN const _ELASTOS InterfaceID EIID_ILifoWaitQueue;
interface IFifoWaitQueue;
EXTERN const _ELASTOS InterfaceID EIID_IFifoWaitQueue;
interface ISynchronousQueueTransferStack;
EXTERN const _ELASTOS InterfaceID EIID_ISynchronousQueueTransferStack;
interface ISynchronousQueueTransferQueue;
EXTERN const _ELASTOS InterfaceID EIID_ISynchronousQueueTransferQueue;
interface ISynchronousQueue;
EXTERN const _ELASTOS InterfaceID EIID_ISynchronousQueue;
interface IThreadLocalRandom;
EXTERN const _ELASTOS InterfaceID EIID_IThreadLocalRandom;
interface IThreadLocalRandomHelper;
EXTERN const _ELASTOS InterfaceID EIID_IThreadLocalRandomHelper;
interface ITransferQueue;
EXTERN const _ELASTOS InterfaceID EIID_ITransferQueue;
interface IConcurrentMap;
EXTERN const _ELASTOS InterfaceID EIID_IConcurrentMap;
interface ITreeBin;
EXTERN const _ELASTOS InterfaceID EIID_ITreeBin;
interface ITreeNode;
EXTERN const _ELASTOS InterfaceID EIID_ITreeNode;
interface IForwardingNode;
EXTERN const _ELASTOS InterfaceID EIID_IForwardingNode;
interface IReservationNode;
EXTERN const _ELASTOS InterfaceID EIID_IReservationNode;
interface IConcurrentHashMap;
EXTERN const _ELASTOS InterfaceID EIID_IConcurrentHashMap;
interface IConcurrentNavigableMap;
EXTERN const _ELASTOS InterfaceID EIID_IConcurrentNavigableMap;
interface IExecutorCompletionService;
EXTERN const _ELASTOS InterfaceID EIID_IExecutorCompletionService;
interface IScheduledExecutorService;
EXTERN const _ELASTOS InterfaceID EIID_IScheduledExecutorService;
interface IRecursiveAction;
EXTERN const _ELASTOS InterfaceID EIID_IRecursiveAction;
interface IRecursiveTask;
EXTERN const _ELASTOS InterfaceID EIID_IRecursiveTask;
interface IScheduledFutureTask;
EXTERN const _ELASTOS InterfaceID EIID_IScheduledFutureTask;
interface IScheduledThreadPoolExecutor;
EXTERN const _ELASTOS InterfaceID EIID_IScheduledThreadPoolExecutor;
interface ICountedCompleter;
EXTERN const _ELASTOS InterfaceID EIID_ICountedCompleter;
interface ICCountDownLatchClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCountDownLatchClassObject;
interface ICExecutorCompletionServiceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICExecutorCompletionServiceClassObject;
interface ICExecutorsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICExecutorsClassObject;
interface ICFutureTaskClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFutureTaskClassObject;
interface ICLinkedBlockingQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedBlockingQueueClassObject;
interface ICThreadPoolExecutorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThreadPoolExecutorClassObject;
interface ICTimeUnitHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimeUnitHelperClassObject;
interface ICArrayBlockingQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICArrayBlockingQueueClassObject;
interface ICConcurrentLinkedDequeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConcurrentLinkedDequeClassObject;
interface ICConcurrentLinkedQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConcurrentLinkedQueueClassObject;
interface ICConcurrentSkipListMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConcurrentSkipListMapClassObject;
interface ICConcurrentSkipListSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConcurrentSkipListSetClassObject;
interface ICDelayQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDelayQueueClassObject;
interface ICExchangerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICExchangerClassObject;
interface ICThreadLocalRandomClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThreadLocalRandomClassObject;
interface ICThreadLocalRandomHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICThreadLocalRandomHelperClassObject;
interface ICForkJoinPoolClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICForkJoinPoolClassObject;
interface ICForkJoinTaskHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICForkJoinTaskHelperClassObject;
interface ICLinkedBlockingDequeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedBlockingDequeClassObject;
interface ICLinkedTransferQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLinkedTransferQueueClassObject;
interface ICPhaserClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPhaserClassObject;
interface ICCopyOnWriteArrayListClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCopyOnWriteArrayListClassObject;
interface ICCopyOnWriteArrayListHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCopyOnWriteArrayListHelperClassObject;
interface ICCopyOnWriteArraySetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCopyOnWriteArraySetClassObject;
interface ICConcurrentHashMapClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConcurrentHashMapClassObject;
interface ICCyclicBarrierClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCyclicBarrierClassObject;
interface ICPriorityBlockingQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPriorityBlockingQueueClassObject;
interface ICSynchronousQueueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSynchronousQueueClassObject;
interface ICScheduledThreadPoolExecutorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICScheduledThreadPoolExecutorClassObject;
interface ICSemaphoreClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSemaphoreClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCountDownLatch;
EXTERN const _ELASTOS ClassID ECLSID_CCountDownLatchClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CExecutorCompletionService;
EXTERN const _ELASTOS ClassID ECLSID_CExecutorCompletionServiceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CExecutors;
EXTERN const _ELASTOS ClassID ECLSID_CExecutorsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFutureTask;
EXTERN const _ELASTOS ClassID ECLSID_CFutureTaskClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedBlockingQueue;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedBlockingQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThreadPoolExecutor;
EXTERN const _ELASTOS ClassID ECLSID_CThreadPoolExecutorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTimeUnitHelper;
EXTERN const _ELASTOS ClassID ECLSID_CTimeUnitHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CArrayBlockingQueue;
EXTERN const _ELASTOS ClassID ECLSID_CArrayBlockingQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentLinkedDeque;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentLinkedDequeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentLinkedQueue;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentLinkedQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentSkipListMap;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentSkipListMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentSkipListSet;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentSkipListSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDelayQueue;
EXTERN const _ELASTOS ClassID ECLSID_CDelayQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CExchanger;
EXTERN const _ELASTOS ClassID ECLSID_CExchangerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThreadLocalRandom;
EXTERN const _ELASTOS ClassID ECLSID_CThreadLocalRandomClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CThreadLocalRandomHelper;
EXTERN const _ELASTOS ClassID ECLSID_CThreadLocalRandomHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CForkJoinPool;
EXTERN const _ELASTOS ClassID ECLSID_CForkJoinPoolClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CForkJoinTaskHelper;
EXTERN const _ELASTOS ClassID ECLSID_CForkJoinTaskHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedBlockingDeque;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedBlockingDequeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedTransferQueue;
EXTERN const _ELASTOS ClassID ECLSID_CLinkedTransferQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPhaser;
EXTERN const _ELASTOS ClassID ECLSID_CPhaserClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArrayList;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArrayListClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArrayListHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArrayListHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArraySet;
EXTERN const _ELASTOS ClassID ECLSID_CCopyOnWriteArraySetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentHashMap;
EXTERN const _ELASTOS ClassID ECLSID_CConcurrentHashMapClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCyclicBarrier;
EXTERN const _ELASTOS ClassID ECLSID_CCyclicBarrierClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPriorityBlockingQueue;
EXTERN const _ELASTOS ClassID ECLSID_CPriorityBlockingQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSynchronousQueue;
EXTERN const _ELASTOS ClassID ECLSID_CSynchronousQueueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CScheduledThreadPoolExecutor;
EXTERN const _ELASTOS ClassID ECLSID_CScheduledThreadPoolExecutorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSemaphore;
EXTERN const _ELASTOS ClassID ECLSID_CSemaphoreClassObject;
}
}
}

namespace Elastos {
namespace Utility {
namespace Zip {
interface IZipConstants;
EXTERN const _ELASTOS InterfaceID EIID_IZipConstants;
interface IChecksum;
EXTERN const _ELASTOS InterfaceID EIID_IChecksum;
interface IAdler32;
EXTERN const _ELASTOS InterfaceID EIID_IAdler32;
interface ICRC32;
EXTERN const _ELASTOS InterfaceID EIID_ICRC32;
interface IInflater;
EXTERN const _ELASTOS InterfaceID EIID_IInflater;
interface IDeflater;
EXTERN const _ELASTOS InterfaceID EIID_IDeflater;
interface IZipEntry;
EXTERN const _ELASTOS InterfaceID EIID_IZipEntry;
interface IInflaterInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IInflaterInputStream;
interface IInflaterOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IInflaterOutputStream;
interface IDeflaterInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDeflaterInputStream;
interface IDeflaterOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDeflaterOutputStream;
interface IZipInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IZipInputStream;
interface IZipOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IZipOutputStream;
interface ICheckedInputStream;
EXTERN const _ELASTOS InterfaceID EIID_ICheckedInputStream;
interface ICheckedOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_ICheckedOutputStream;
interface IGZIPInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IGZIPInputStream;
interface IGZIPOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IGZIPOutputStream;
interface IZipFileRAFStream;
EXTERN const _ELASTOS InterfaceID EIID_IZipFileRAFStream;
interface IZipFile;
EXTERN const _ELASTOS InterfaceID EIID_IZipFile;
interface ICAdler32ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAdler32ClassObject;
interface ICCRC32ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCRC32ClassObject;
interface ICInflaterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInflaterClassObject;
interface ICDeflaterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDeflaterClassObject;
interface ICInflaterInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInflaterInputStreamClassObject;
interface ICInflaterOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInflaterOutputStreamClassObject;
interface ICDeflaterInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDeflaterInputStreamClassObject;
interface ICDeflaterOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDeflaterOutputStreamClassObject;
interface ICCheckedInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCheckedInputStreamClassObject;
interface ICCheckedOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCheckedOutputStreamClassObject;
interface ICGZIPInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICGZIPInputStreamClassObject;
interface ICGZIPOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICGZIPOutputStreamClassObject;
interface ICZipInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZipInputStreamClassObject;
interface ICZipOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZipOutputStreamClassObject;
interface ICZipEntryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZipEntryClassObject;
interface ICZipFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZipFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAdler32;
EXTERN const _ELASTOS ClassID ECLSID_CAdler32ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCRC32;
EXTERN const _ELASTOS ClassID ECLSID_CCRC32ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInflater;
EXTERN const _ELASTOS ClassID ECLSID_CInflaterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDeflater;
EXTERN const _ELASTOS ClassID ECLSID_CDeflaterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInflaterInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CInflaterInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInflaterOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CInflaterOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDeflaterInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDeflaterInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDeflaterOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDeflaterOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCheckedInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CCheckedInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCheckedOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CCheckedOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CGZIPInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CGZIPInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CGZIPOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CGZIPOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZipInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CZipInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZipOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CZipOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZipEntry;
EXTERN const _ELASTOS ClassID ECLSID_CZipEntryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZipFile;
EXTERN const _ELASTOS ClassID ECLSID_CZipFileClassObject;
}
}
}

namespace Elastos {
namespace Utility {
namespace Jar {
interface IName;
EXTERN const _ELASTOS InterfaceID EIID_IName;
interface IAttributes;
EXTERN const _ELASTOS InterfaceID EIID_IAttributes;
interface IJarEntry;
EXTERN const _ELASTOS InterfaceID EIID_IJarEntry;
interface IManifest;
EXTERN const _ELASTOS InterfaceID EIID_IManifest;
interface IJarFile;
EXTERN const _ELASTOS InterfaceID EIID_IJarFile;
interface IJarInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IJarInputStream;
interface IJarOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IJarOutputStream;
interface IPacker;
EXTERN const _ELASTOS InterfaceID EIID_IPacker;
interface IUnpacker;
EXTERN const _ELASTOS InterfaceID EIID_IUnpacker;
interface IPack200;
EXTERN const _ELASTOS InterfaceID EIID_IPack200;
interface IStrictJarFile;
EXTERN const _ELASTOS InterfaceID EIID_IStrictJarFile;
interface ICNameClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNameClassObject;
interface ICAttributesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributesClassObject;
interface ICJarEntryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarEntryClassObject;
interface ICJarFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarFileClassObject;
interface ICStrictJarFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStrictJarFileClassObject;
interface ICJarInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarInputStreamClassObject;
interface ICJarOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarOutputStreamClassObject;
interface ICManifestClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICManifestClassObject;
interface ICPack200ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPack200ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CName;
EXTERN const _ELASTOS ClassID ECLSID_CNameClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributes;
EXTERN const _ELASTOS ClassID ECLSID_CAttributesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarEntry;
EXTERN const _ELASTOS ClassID ECLSID_CJarEntryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarFile;
EXTERN const _ELASTOS ClassID ECLSID_CJarFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStrictJarFile;
EXTERN const _ELASTOS ClassID ECLSID_CStrictJarFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CJarInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CJarOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CManifest;
EXTERN const _ELASTOS ClassID ECLSID_CManifestClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPack200;
EXTERN const _ELASTOS ClassID ECLSID_CPack200ClassObject;
}
}
}

namespace Elastos {
namespace Sql {
interface ITypes;
EXTERN const _ELASTOS InterfaceID EIID_ITypes;
interface IClob;
EXTERN const _ELASTOS InterfaceID EIID_IClob;
interface IBlob;
EXTERN const _ELASTOS InterfaceID EIID_IBlob;
interface INClob;
EXTERN const _ELASTOS InterfaceID EIID_INClob;
interface IArray;
EXTERN const _ELASTOS InterfaceID EIID_IArray;
interface IStruct;
EXTERN const _ELASTOS InterfaceID EIID_IStruct;
interface ISQLDate;
EXTERN const _ELASTOS InterfaceID EIID_ISQLDate;
interface ITime;
EXTERN const _ELASTOS InterfaceID EIID_ITime;
interface IWrapper;
EXTERN const _ELASTOS InterfaceID EIID_IWrapper;
interface IParameterMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IParameterMetaData;
interface IRowId;
EXTERN const _ELASTOS InterfaceID EIID_IRowId;
interface ITimestamp;
EXTERN const _ELASTOS InterfaceID EIID_ITimestamp;
interface IRef;
EXTERN const _ELASTOS InterfaceID EIID_IRef;
interface ISQLStruct;
EXTERN const _ELASTOS InterfaceID EIID_ISQLStruct;
interface ISQLXML;
EXTERN const _ELASTOS InterfaceID EIID_ISQLXML;
interface ISQLInput;
EXTERN const _ELASTOS InterfaceID EIID_ISQLInput;
interface ISQLOutput;
EXTERN const _ELASTOS InterfaceID EIID_ISQLOutput;
interface ISQLData;
EXTERN const _ELASTOS InterfaceID EIID_ISQLData;
interface IStatement;
EXTERN const _ELASTOS InterfaceID EIID_IStatement;
interface ISQLWarning;
EXTERN const _ELASTOS InterfaceID EIID_ISQLWarning;
interface IResultSetMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IResultSetMetaData;
interface IResultSet;
EXTERN const _ELASTOS InterfaceID EIID_IResultSet;
interface IDatabaseMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IDatabaseMetaData;
interface IPreparedStatement;
EXTERN const _ELASTOS InterfaceID EIID_IPreparedStatement;
interface ICallableStatement;
EXTERN const _ELASTOS InterfaceID EIID_ICallableStatement;
interface ISavePoint;
EXTERN const _ELASTOS InterfaceID EIID_ISavePoint;
interface ISQLPermission;
EXTERN const _ELASTOS InterfaceID EIID_ISQLPermission;
interface IConnection;
EXTERN const _ELASTOS InterfaceID EIID_IConnection;
interface IDriver;
EXTERN const _ELASTOS InterfaceID EIID_IDriver;
interface IDriverManager;
EXTERN const _ELASTOS InterfaceID EIID_IDriverManager;
interface IDataTruncation;
EXTERN const _ELASTOS InterfaceID EIID_IDataTruncation;
interface IDriverPropertyInfo;
EXTERN const _ELASTOS InterfaceID EIID_IDriverPropertyInfo;
interface ICSQLWarningClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSQLWarningClassObject;
interface ICTimeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimeClassObject;
interface ICSqlDateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSqlDateClassObject;
interface ICTimestampClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimestampClassObject;
interface ICDriverPropertyInfoClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDriverPropertyInfoClassObject;
interface ICDataTruncationClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDataTruncationClassObject;
interface ICDriverManagerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDriverManagerClassObject;
interface ICSQLPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSQLPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSQLWarning;
EXTERN const _ELASTOS ClassID ECLSID_CSQLWarningClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTime;
EXTERN const _ELASTOS ClassID ECLSID_CTimeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSqlDate;
EXTERN const _ELASTOS ClassID ECLSID_CSqlDateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTimestamp;
EXTERN const _ELASTOS ClassID ECLSID_CTimestampClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDriverPropertyInfo;
EXTERN const _ELASTOS ClassID ECLSID_CDriverPropertyInfoClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDataTruncation;
EXTERN const _ELASTOS ClassID ECLSID_CDataTruncationClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDriverManager;
EXTERN const _ELASTOS ClassID ECLSID_CDriverManagerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSQLPermission;
EXTERN const _ELASTOS ClassID ECLSID_CSQLPermissionClassObject;
}
}

namespace Elastos {
namespace Sql {
namespace SQLite {
interface IStringEncoderHelper;
EXTERN const _ELASTOS InterfaceID EIID_IStringEncoderHelper;
interface IAuthorizer;
EXTERN const _ELASTOS InterfaceID EIID_IAuthorizer;
interface IBlobR;
EXTERN const _ELASTOS InterfaceID EIID_IBlobR;
interface IBlobW;
EXTERN const _ELASTOS InterfaceID EIID_IBlobW;
interface IBlob;
EXTERN const _ELASTOS InterfaceID EIID_IBlob;
interface ICallback;
EXTERN const _ELASTOS InterfaceID EIID_ICallback;
interface IBusyHandler;
EXTERN const _ELASTOS InterfaceID EIID_IBusyHandler;
interface ITableResult;
EXTERN const _ELASTOS InterfaceID EIID_ITableResult;
interface IFunctionContext;
EXTERN const _ELASTOS InterfaceID EIID_IFunctionContext;
interface IFunction;
EXTERN const _ELASTOS InterfaceID EIID_IFunction;
interface ITrace;
EXTERN const _ELASTOS InterfaceID EIID_ITrace;
interface IProfile;
EXTERN const _ELASTOS InterfaceID EIID_IProfile;
interface IVm;
EXTERN const _ELASTOS InterfaceID EIID_IVm;
interface IStmt;
EXTERN const _ELASTOS InterfaceID EIID_IStmt;
interface IBackup;
EXTERN const _ELASTOS InterfaceID EIID_IBackup;
interface IProgressHandler;
EXTERN const _ELASTOS InterfaceID EIID_IProgressHandler;
interface IDatabase;
EXTERN const _ELASTOS InterfaceID EIID_IDatabase;
interface IDatabaseHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDatabaseHelper;
interface IMockFunction;
EXTERN const _ELASTOS InterfaceID EIID_IMockFunction;
interface IConstants;
EXTERN const _ELASTOS InterfaceID EIID_IConstants;
interface IShell;
EXTERN const _ELASTOS InterfaceID EIID_IShell;
interface IDBDump;
EXTERN const _ELASTOS InterfaceID EIID_IDBDump;
interface ICTableResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTableResultClassObject;
interface ICShellClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICShellClassObject;
interface ICDBDumpClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDBDumpClassObject;
interface ICVmClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICVmClassObject;
interface ICBlobClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBlobClassObject;
interface ICBlobRClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBlobRClassObject;
interface ICBlobWClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBlobWClassObject;
interface ICDatabaseClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatabaseClassObject;
interface ICDatabaseHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatabaseHelperClassObject;
interface ICJDBCDriverClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCDriverClassObject;
interface ICBackupClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBackupClassObject;
interface ICFunctionContextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFunctionContextClassObject;
interface ICStmtClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStmtClassObject;
interface ICStringEncoderHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringEncoderHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTableResult;
EXTERN const _ELASTOS ClassID ECLSID_CTableResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CShell;
EXTERN const _ELASTOS ClassID ECLSID_CShellClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDBDump;
EXTERN const _ELASTOS ClassID ECLSID_CDBDumpClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CVm;
EXTERN const _ELASTOS ClassID ECLSID_CVmClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBlob;
EXTERN const _ELASTOS ClassID ECLSID_CBlobClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBlobR;
EXTERN const _ELASTOS ClassID ECLSID_CBlobRClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBlobW;
EXTERN const _ELASTOS ClassID ECLSID_CBlobWClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatabase;
EXTERN const _ELASTOS ClassID ECLSID_CDatabaseClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatabaseHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDatabaseHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCDriver;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCDriverClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBackup;
EXTERN const _ELASTOS ClassID ECLSID_CBackupClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFunctionContext;
EXTERN const _ELASTOS ClassID ECLSID_CFunctionContextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStmt;
EXTERN const _ELASTOS ClassID ECLSID_CStmtClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringEncoderHelper;
EXTERN const _ELASTOS ClassID ECLSID_CStringEncoderHelperClassObject;
}
}
}

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC {
interface IJDBCConnection;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCConnection;
interface IJDBCStatement;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCStatement;
interface IJDBCPreparedStatement;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCPreparedStatement;
interface IJDBCResultSetMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCResultSetMetaData;
interface ITableResultX;
EXTERN const _ELASTOS InterfaceID EIID_ITableResultX;
interface IJDBCDatabaseMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCDatabaseMetaData;
interface IJDBCResultSet;
EXTERN const _ELASTOS InterfaceID EIID_IJDBCResultSet;
interface ICJDBCConnectionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCConnectionClassObject;
interface ICJDBCPreparedStatementClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCPreparedStatementClassObject;
interface ICJDBCStatementClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCStatementClassObject;
interface ICJDBCResultSetMetaDataClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCResultSetMetaDataClassObject;
interface ICTableResultXClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTableResultXClassObject;
interface ICJDBCDatabaseMetaDataClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCDatabaseMetaDataClassObject;
interface ICJDBCResultSetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJDBCResultSetClassObject;
interface ICDatabaseXClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatabaseXClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCConnection;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCConnectionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCPreparedStatement;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCPreparedStatementClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCStatement;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCStatementClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCResultSetMetaData;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCResultSetMetaDataClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTableResultX;
EXTERN const _ELASTOS ClassID ECLSID_CTableResultXClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCDatabaseMetaData;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCDatabaseMetaDataClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCResultSet;
EXTERN const _ELASTOS ClassID ECLSID_CJDBCResultSetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatabaseX;
EXTERN const _ELASTOS ClassID ECLSID_CDatabaseXClassObject;
}
}
}
}

namespace Libcore {
namespace ICU {
interface ICollationKeyICU;
EXTERN const _ELASTOS InterfaceID EIID_ICollationKeyICU;
interface ILocaleData;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleData;
interface ILocaleDataHelper;
EXTERN const _ELASTOS InterfaceID EIID_ILocaleDataHelper;
interface IICUUtil;
EXTERN const _ELASTOS InterfaceID EIID_IICUUtil;
interface IRuleBasedCollatorICU;
EXTERN const _ELASTOS InterfaceID EIID_IRuleBasedCollatorICU;
interface ITimeZoneNames;
EXTERN const _ELASTOS InterfaceID EIID_ITimeZoneNames;
interface ICollationElementIteratorICU;
EXTERN const _ELASTOS InterfaceID EIID_ICollationElementIteratorICU;
interface ICollationElementIteratorICUHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICollationElementIteratorICUHelper;
interface INativeDecimalFormat;
EXTERN const _ELASTOS InterfaceID EIID_INativeDecimalFormat;
interface INativeBreakIterator;
EXTERN const _ELASTOS InterfaceID EIID_INativeBreakIterator;
interface INativeBreakIteratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativeBreakIteratorHelper;
interface INativeCollationHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativeCollationHelper;
interface INativeIDNHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativeIDNHelper;
interface INativePluralRules;
EXTERN const _ELASTOS InterfaceID EIID_INativePluralRules;
interface INativePluralRulesHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativePluralRulesHelper;
interface INativeNormalizerHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativeNormalizerHelper;
interface IImmutableIndex;
EXTERN const _ELASTOS InterfaceID EIID_IImmutableIndex;
interface IAlphabeticIndex;
EXTERN const _ELASTOS InterfaceID EIID_IAlphabeticIndex;
interface IDateIntervalFormat;
EXTERN const _ELASTOS InterfaceID EIID_IDateIntervalFormat;
interface ITransliterator;
EXTERN const _ELASTOS InterfaceID EIID_ITransliterator;
interface INativeConverterHelper;
EXTERN const _ELASTOS InterfaceID EIID_INativeConverterHelper;
interface ICCollationElementIteratorICUHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollationElementIteratorICUHelperClassObject;
interface ICNativeCollationHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeCollationHelperClassObject;
interface ICCollationKeyICUClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollationKeyICUClassObject;
interface ICLocaleDataClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocaleDataClassObject;
interface ICLocaleDataHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocaleDataHelperClassObject;
interface ICTimeZoneNamesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTimeZoneNamesClassObject;
interface ICNativeIDNHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeIDNHelperClassObject;
interface ICNativeNormalizerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeNormalizerHelperClassObject;
interface ICNativePluralRulesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativePluralRulesClassObject;
interface ICNativePluralRulesHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativePluralRulesHelperClassObject;
interface ICRuleBasedCollatorICUClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRuleBasedCollatorICUClassObject;
interface ICNativeBreakIteratorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeBreakIteratorHelperClassObject;
interface ICAlphabeticIndexClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAlphabeticIndexClassObject;
interface ICDateIntervalFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDateIntervalFormatClassObject;
interface ICTransliteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTransliteratorClassObject;
interface ICICUUtilClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICICUUtilClassObject;
interface ICNativeConverterHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeConverterHelperClassObject;
interface ICNativeDecimalFormatClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNativeDecimalFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIteratorICUHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCollationElementIteratorICUHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeCollationHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativeCollationHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollationKeyICU;
EXTERN const _ELASTOS ClassID ECLSID_CCollationKeyICUClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleData;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleDataClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleDataHelper;
EXTERN const _ELASTOS ClassID ECLSID_CLocaleDataHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTimeZoneNames;
EXTERN const _ELASTOS ClassID ECLSID_CTimeZoneNamesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeIDNHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativeIDNHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeNormalizerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativeNormalizerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativePluralRules;
EXTERN const _ELASTOS ClassID ECLSID_CNativePluralRulesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativePluralRulesHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativePluralRulesHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedCollatorICU;
EXTERN const _ELASTOS ClassID ECLSID_CRuleBasedCollatorICUClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeBreakIteratorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativeBreakIteratorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAlphabeticIndex;
EXTERN const _ELASTOS ClassID ECLSID_CAlphabeticIndexClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDateIntervalFormat;
EXTERN const _ELASTOS ClassID ECLSID_CDateIntervalFormatClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTransliterator;
EXTERN const _ELASTOS ClassID ECLSID_CTransliteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CICUUtil;
EXTERN const _ELASTOS ClassID ECLSID_CICUUtilClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeConverterHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNativeConverterHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNativeDecimalFormat;
EXTERN const _ELASTOS ClassID ECLSID_CNativeDecimalFormatClassObject;
}
}

namespace Libcore {
namespace IO {
interface IAsynchronousCloseMonitor;
EXTERN const _ELASTOS InterfaceID EIID_IAsynchronousCloseMonitor;
interface IOs;
EXTERN const _ELASTOS InterfaceID EIID_IOs;
interface IIoBridge;
EXTERN const _ELASTOS InterfaceID EIID_IIoBridge;
interface ILibcore;
EXTERN const _ELASTOS InterfaceID EIID_ILibcore;
interface ISocketTagger;
EXTERN const _ELASTOS InterfaceID EIID_ISocketTagger;
interface ISocketTaggerHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISocketTaggerHelper;
interface IBase64;
EXTERN const _ELASTOS InterfaceID EIID_IBase64;
interface IBufferIterator;
EXTERN const _ELASTOS InterfaceID EIID_IBufferIterator;
interface IHeapBufferIterator;
EXTERN const _ELASTOS InterfaceID EIID_IHeapBufferIterator;
interface IMemory;
EXTERN const _ELASTOS InterfaceID EIID_IMemory;
interface IMemoryMappedFile;
EXTERN const _ELASTOS InterfaceID EIID_IMemoryMappedFile;
interface IMemoryMappedFileHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMemoryMappedFileHelper;
interface ISizeOf;
EXTERN const _ELASTOS InterfaceID EIID_ISizeOf;
interface INioBufferIterator;
EXTERN const _ELASTOS InterfaceID EIID_INioBufferIterator;
interface IDeleteOnExit;
EXTERN const _ELASTOS InterfaceID EIID_IDeleteOnExit;
interface IDeleteOnExitHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDeleteOnExitHelper;
interface IReporter;
EXTERN const _ELASTOS InterfaceID EIID_IReporter;
interface IDropBox;
EXTERN const _ELASTOS InterfaceID EIID_IDropBox;
interface IIoUtils;
EXTERN const _ELASTOS InterfaceID EIID_IIoUtils;
interface IStreams;
EXTERN const _ELASTOS InterfaceID EIID_IStreams;
interface IStrictLineReader;
EXTERN const _ELASTOS InterfaceID EIID_IStrictLineReader;
interface ICAsynchronousCloseMonitorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAsynchronousCloseMonitorClassObject;
interface ICBase64ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBase64ClassObject;
interface ICLibcoreClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLibcoreClassObject;
interface ICIoBridgeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICIoBridgeClassObject;
interface ICSocketTaggerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocketTaggerHelperClassObject;
interface ICStreamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStreamsClassObject;
interface ICDeleteOnExitHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDeleteOnExitHelperClassObject;
interface ICDropBoxClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDropBoxClassObject;
interface ICMemoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMemoryClassObject;
interface ICIoUtilsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICIoUtilsClassObject;
interface ICMemoryMappedFileClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMemoryMappedFileClassObject;
interface ICStrictLineReaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStrictLineReaderClassObject;
interface ICPosixClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPosixClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAsynchronousCloseMonitor;
EXTERN const _ELASTOS ClassID ECLSID_CAsynchronousCloseMonitorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBase64;
EXTERN const _ELASTOS ClassID ECLSID_CBase64ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLibcore;
EXTERN const _ELASTOS ClassID ECLSID_CLibcoreClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CIoBridge;
EXTERN const _ELASTOS ClassID ECLSID_CIoBridgeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocketTaggerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSocketTaggerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStreams;
EXTERN const _ELASTOS ClassID ECLSID_CStreamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDeleteOnExitHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDeleteOnExitHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDropBox;
EXTERN const _ELASTOS ClassID ECLSID_CDropBoxClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMemory;
EXTERN const _ELASTOS ClassID ECLSID_CMemoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CIoUtils;
EXTERN const _ELASTOS ClassID ECLSID_CIoUtilsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMemoryMappedFile;
EXTERN const _ELASTOS ClassID ECLSID_CMemoryMappedFileClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStrictLineReader;
EXTERN const _ELASTOS ClassID ECLSID_CStrictLineReaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPosix;
EXTERN const _ELASTOS ClassID ECLSID_CPosixClassObject;
}
}

namespace Libcore {
namespace Utility {
interface IBasicLruCache;
EXTERN const _ELASTOS InterfaceID EIID_IBasicLruCache;
interface ICollectionUtils;
EXTERN const _ELASTOS InterfaceID EIID_ICollectionUtils;
interface IZoneInfo;
EXTERN const _ELASTOS InterfaceID EIID_IZoneInfo;
interface IZoneInfoHelper;
EXTERN const _ELASTOS InterfaceID EIID_IZoneInfoHelper;
interface IOffsetInterval;
EXTERN const _ELASTOS InterfaceID EIID_IOffsetInterval;
interface IOffsetIntervalHelper;
EXTERN const _ELASTOS InterfaceID EIID_IOffsetIntervalHelper;
interface IZoneInfoWallTime;
EXTERN const _ELASTOS InterfaceID EIID_IZoneInfoWallTime;
interface ITzData;
EXTERN const _ELASTOS InterfaceID EIID_ITzData;
interface IZoneInfoDB;
EXTERN const _ELASTOS InterfaceID EIID_IZoneInfoDB;
interface ICBasicLruCacheClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicLruCacheClassObject;
interface ICZoneInfoWallTimeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZoneInfoWallTimeClassObject;
interface ICCollectionUtilsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCollectionUtilsClassObject;
interface ICOffsetIntervalHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICOffsetIntervalHelperClassObject;
interface ICZoneInfoHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZoneInfoHelperClassObject;
interface ICZoneInfoDBClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICZoneInfoDBClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLruCache;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLruCacheClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoWallTime;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoWallTimeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCollectionUtils;
EXTERN const _ELASTOS ClassID ECLSID_CCollectionUtilsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_COffsetIntervalHelper;
EXTERN const _ELASTOS ClassID ECLSID_COffsetIntervalHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoHelper;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoDB;
EXTERN const _ELASTOS ClassID ECLSID_CZoneInfoDBClassObject;
}
}

namespace Libcore {
namespace Net {
interface IMimeUtils;
EXTERN const _ELASTOS InterfaceID EIID_IMimeUtils;
interface IUriCodec;
EXTERN const _ELASTOS InterfaceID EIID_IUriCodec;
interface IUriCodecHelper;
EXTERN const _ELASTOS InterfaceID EIID_IUriCodecHelper;
interface ICMimeUtilsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMimeUtilsClassObject;
interface ICUriCodecHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUriCodecHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMimeUtils;
EXTERN const _ELASTOS ClassID ECLSID_CMimeUtilsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUriCodecHelper;
EXTERN const _ELASTOS ClassID ECLSID_CUriCodecHelperClassObject;
}
}

namespace Libcore {
namespace Net {
namespace Http {
interface IHttpDate;
EXTERN const _ELASTOS InterfaceID EIID_IHttpDate;
interface ICHttpDateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpDateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDate;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDateClassObject;
}
}
}

namespace Libcore {
namespace Net {
namespace Url {
interface IFileHandler;
EXTERN const _ELASTOS InterfaceID EIID_IFileHandler;
interface IFileURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IFileURLConnection;
interface IFtpHandler;
EXTERN const _ELASTOS InterfaceID EIID_IFtpHandler;
interface IFtpURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IFtpURLConnection;
interface IFtpURLInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IFtpURLInputStream;
interface IJarHandler;
EXTERN const _ELASTOS InterfaceID EIID_IJarHandler;
interface IJarURLConnectionImpl;
EXTERN const _ELASTOS InterfaceID EIID_IJarURLConnectionImpl;
interface IJarURLConnectionImplHelper;
EXTERN const _ELASTOS InterfaceID EIID_IJarURLConnectionImplHelper;
interface IUrlUtilsHelper;
EXTERN const _ELASTOS InterfaceID EIID_IUrlUtilsHelper;
interface ICFileHandlerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileHandlerClassObject;
interface ICFileURLConnectionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileURLConnectionClassObject;
interface ICFtpHandlerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFtpHandlerClassObject;
interface ICFtpURLInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFtpURLInputStreamClassObject;
interface ICUrlUtilsHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUrlUtilsHelperClassObject;
interface ICJarHandlerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarHandlerClassObject;
interface ICJarURLConnectionImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarURLConnectionImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileHandler;
EXTERN const _ELASTOS ClassID ECLSID_CFileHandlerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileURLConnection;
EXTERN const _ELASTOS ClassID ECLSID_CFileURLConnectionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFtpHandler;
EXTERN const _ELASTOS ClassID ECLSID_CFtpHandlerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFtpURLInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CFtpURLInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUrlUtilsHelper;
EXTERN const _ELASTOS ClassID ECLSID_CUrlUtilsHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarHandler;
EXTERN const _ELASTOS ClassID ECLSID_CJarHandlerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarURLConnectionImpl;
EXTERN const _ELASTOS ClassID ECLSID_CJarURLConnectionImplClassObject;
}
}
}

namespace Libcore {
namespace Net {
namespace Event {
interface INetworkEventListener;
EXTERN const _ELASTOS InterfaceID EIID_INetworkEventListener;
interface INetworkEventDispatcher;
EXTERN const _ELASTOS InterfaceID EIID_INetworkEventDispatcher;
interface INetworkEventDispatcherHelper;
EXTERN const _ELASTOS InterfaceID EIID_INetworkEventDispatcherHelper;
interface ICNetworkEventDispatcherHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNetworkEventDispatcherHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNetworkEventDispatcherHelper;
EXTERN const _ELASTOS ClassID ECLSID_CNetworkEventDispatcherHelperClassObject;
}
}
}

namespace Elastosx {
namespace Crypto {
interface ICipher;
EXTERN const _ELASTOS InterfaceID EIID_ICipher;
interface ICipherHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICipherHelper;
interface ICipherInputStream;
EXTERN const _ELASTOS InterfaceID EIID_ICipherInputStream;
interface ICipherOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_ICipherOutputStream;
interface ICipherSpi;
EXTERN const _ELASTOS InterfaceID EIID_ICipherSpi;
interface IEncryptedPrivateKeyInfo;
EXTERN const _ELASTOS InterfaceID EIID_IEncryptedPrivateKeyInfo;
interface IExemptionMechanism;
EXTERN const _ELASTOS InterfaceID EIID_IExemptionMechanism;
interface IExemptionMechanismHelper;
EXTERN const _ELASTOS InterfaceID EIID_IExemptionMechanismHelper;
interface IExemptionMechanismSpi;
EXTERN const _ELASTOS InterfaceID EIID_IExemptionMechanismSpi;
interface ISecretKey;
EXTERN const _ELASTOS InterfaceID EIID_ISecretKey;
interface IKeyAgreement;
EXTERN const _ELASTOS InterfaceID EIID_IKeyAgreement;
interface IKeyAgreementHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyAgreementHelper;
interface IKeyAgreementSpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyAgreementSpi;
interface IKeyGenerator;
EXTERN const _ELASTOS InterfaceID EIID_IKeyGenerator;
interface IKeyGeneratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyGeneratorHelper;
interface IKeyGeneratorSpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyGeneratorSpi;
interface IMac;
EXTERN const _ELASTOS InterfaceID EIID_IMac;
interface IMacHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMacHelper;
interface IMacSpi;
EXTERN const _ELASTOS InterfaceID EIID_IMacSpi;
interface ISecretKeyFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISecretKeyFactory;
interface ISecretKeyFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISecretKeyFactoryHelper;
interface ISecretKeyFactorySpi;
EXTERN const _ELASTOS InterfaceID EIID_ISecretKeyFactorySpi;
interface ISealedObject;
EXTERN const _ELASTOS InterfaceID EIID_ISealedObject;
interface INullCipher;
EXTERN const _ELASTOS InterfaceID EIID_INullCipher;
interface ICCipherClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCipherClassObject;
interface ICCipherHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCipherHelperClassObject;
interface ICCipherInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCipherInputStreamClassObject;
interface ICCipherOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCipherOutputStreamClassObject;
interface ICEncryptedPrivateKeyInfoClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEncryptedPrivateKeyInfoClassObject;
interface ICExemptionMechanismClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICExemptionMechanismClassObject;
interface ICExemptionMechanismHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICExemptionMechanismHelperClassObject;
interface ICKeyAgreementClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyAgreementClassObject;
interface ICKeyAgreementHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyAgreementHelperClassObject;
interface ICKeyGeneratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyGeneratorClassObject;
interface ICKeyGeneratorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyGeneratorHelperClassObject;
interface ICMacClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMacClassObject;
interface ICMacHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMacHelperClassObject;
interface ICSecretKeyFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSecretKeyFactoryClassObject;
interface ICSecretKeyFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSecretKeyFactoryHelperClassObject;
interface ICSealedObjectClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSealedObjectClassObject;
interface ICNullCipherClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNullCipherClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCipher;
EXTERN const _ELASTOS ClassID ECLSID_CCipherClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCipherHelper;
EXTERN const _ELASTOS ClassID ECLSID_CCipherHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCipherInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CCipherInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCipherOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CCipherOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEncryptedPrivateKeyInfo;
EXTERN const _ELASTOS ClassID ECLSID_CEncryptedPrivateKeyInfoClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CExemptionMechanism;
EXTERN const _ELASTOS ClassID ECLSID_CExemptionMechanismClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CExemptionMechanismHelper;
EXTERN const _ELASTOS ClassID ECLSID_CExemptionMechanismHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyAgreement;
EXTERN const _ELASTOS ClassID ECLSID_CKeyAgreementClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyAgreementHelper;
EXTERN const _ELASTOS ClassID ECLSID_CKeyAgreementHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyGenerator;
EXTERN const _ELASTOS ClassID ECLSID_CKeyGeneratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyGeneratorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CKeyGeneratorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMac;
EXTERN const _ELASTOS ClassID ECLSID_CMacClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMacHelper;
EXTERN const _ELASTOS ClassID ECLSID_CMacHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeyFactory;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeyFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeyFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeyFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSealedObject;
EXTERN const _ELASTOS ClassID ECLSID_CSealedObjectClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNullCipher;
EXTERN const _ELASTOS ClassID ECLSID_CNullCipherClassObject;
}
}

namespace Elastosx {
namespace Crypto {
namespace Interfaces {
interface IDHKey;
EXTERN const _ELASTOS InterfaceID EIID_IDHKey;
interface IDHPrivateKey;
EXTERN const _ELASTOS InterfaceID EIID_IDHPrivateKey;
interface IDHPublicKey;
EXTERN const _ELASTOS InterfaceID EIID_IDHPublicKey;
interface IPBEKey;
EXTERN const _ELASTOS InterfaceID EIID_IPBEKey;
}
}
}

namespace Elastosx {
namespace Crypto {
namespace Spec {
interface IDESKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDESKeySpec;
interface IDESKeySpecHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDESKeySpecHelper;
interface IDESedeKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDESedeKeySpec;
interface IDESedeKeySpecHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDESedeKeySpecHelper;
interface IDHGenParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IDHGenParameterSpec;
interface IDHParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IDHParameterSpec;
interface IDHPrivateKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDHPrivateKeySpec;
interface IDHPublicKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IDHPublicKeySpec;
interface IGCMParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IGCMParameterSpec;
interface IIvParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IIvParameterSpec;
interface IOAEPParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IOAEPParameterSpec;
interface IOAEPParameterSpecHelper;
EXTERN const _ELASTOS InterfaceID EIID_IOAEPParameterSpecHelper;
interface IPBEKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_IPBEKeySpec;
interface IPBEParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IPBEParameterSpec;
interface IPSource;
EXTERN const _ELASTOS InterfaceID EIID_IPSource;
interface IPSpecified;
EXTERN const _ELASTOS InterfaceID EIID_IPSpecified;
interface IPSpecifiedHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPSpecifiedHelper;
interface IRC2ParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IRC2ParameterSpec;
interface IRC5ParameterSpec;
EXTERN const _ELASTOS InterfaceID EIID_IRC5ParameterSpec;
interface ISecretKeySpec;
EXTERN const _ELASTOS InterfaceID EIID_ISecretKeySpec;
interface ICDESKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDESKeySpecClassObject;
interface ICDESKeySpecHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDESKeySpecHelperClassObject;
interface ICDESedeKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDESedeKeySpecClassObject;
interface ICDESedeKeySpecHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDESedeKeySpecHelperClassObject;
interface ICDHGenParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDHGenParameterSpecClassObject;
interface ICDHParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDHParameterSpecClassObject;
interface ICDHPrivateKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDHPrivateKeySpecClassObject;
interface ICDHPublicKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDHPublicKeySpecClassObject;
interface ICGCMParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICGCMParameterSpecClassObject;
interface ICIvParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICIvParameterSpecClassObject;
interface ICOAEPParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICOAEPParameterSpecClassObject;
interface ICOAEPParameterSpecHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICOAEPParameterSpecHelperClassObject;
interface ICPBEKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPBEKeySpecClassObject;
interface ICPBEParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPBEParameterSpecClassObject;
interface ICPSourceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPSourceClassObject;
interface ICPSpecifiedClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPSpecifiedClassObject;
interface ICPSpecifiedHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPSpecifiedHelperClassObject;
interface ICRC2ParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRC2ParameterSpecClassObject;
interface ICRC5ParameterSpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRC5ParameterSpecClassObject;
interface ICSecretKeySpecClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSecretKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDESKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CDESKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDESKeySpecHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDESKeySpecHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDESedeKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CDESedeKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDESedeKeySpecHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDESedeKeySpecHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDHGenParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CDHGenParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDHParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CDHParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDHPrivateKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CDHPrivateKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDHPublicKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CDHPublicKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CGCMParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CGCMParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CIvParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CIvParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_COAEPParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_COAEPParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_COAEPParameterSpecHelper;
EXTERN const _ELASTOS ClassID ECLSID_COAEPParameterSpecHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPBEKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CPBEKeySpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPBEParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CPBEParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPSource;
EXTERN const _ELASTOS ClassID ECLSID_CPSourceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPSpecified;
EXTERN const _ELASTOS ClassID ECLSID_CPSpecifiedClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPSpecifiedHelper;
EXTERN const _ELASTOS ClassID ECLSID_CPSpecifiedHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRC2ParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CRC2ParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRC5ParameterSpec;
EXTERN const _ELASTOS ClassID ECLSID_CRC5ParameterSpecClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeySpec;
EXTERN const _ELASTOS ClassID ECLSID_CSecretKeySpecClassObject;
}
}
}

namespace Elastosx {
namespace Net {
interface ISocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISocketFactory;
interface ISocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISocketFactoryHelper;
interface IServerSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocketFactory;
interface IServerSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IServerSocketFactoryHelper;
interface ICDefaultServerSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultServerSocketFactoryClassObject;
interface ICDefaultSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultSocketFactoryClassObject;
interface ICServerSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICServerSocketFactoryHelperClassObject;
interface ICSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultServerSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultServerSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CServerSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSocketFactoryHelperClassObject;
}
}

namespace Elastosx {
namespace Net {
namespace Ssl {
interface ICertPathTrustManagerParameters;
EXTERN const _ELASTOS InterfaceID EIID_ICertPathTrustManagerParameters;
interface IDefaultHostnameVerifier;
EXTERN const _ELASTOS InterfaceID EIID_IDefaultHostnameVerifier;
interface IDistinguishedNameParser;
EXTERN const _ELASTOS InterfaceID EIID_IDistinguishedNameParser;
interface IHandshakeCompletedEvent;
EXTERN const _ELASTOS InterfaceID EIID_IHandshakeCompletedEvent;
interface IHandshakeCompletedListener;
EXTERN const _ELASTOS InterfaceID EIID_IHandshakeCompletedListener;
interface IHostnameVerifier;
EXTERN const _ELASTOS InterfaceID EIID_IHostnameVerifier;
interface IHttpsURLConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpsURLConnection;
interface IHttpsURLConnectionHelper;
EXTERN const _ELASTOS InterfaceID EIID_IHttpsURLConnectionHelper;
interface IKeyManager;
EXTERN const _ELASTOS InterfaceID EIID_IKeyManager;
interface IKeyManagerFactory;
EXTERN const _ELASTOS InterfaceID EIID_IKeyManagerFactory;
interface IKeyManagerFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IKeyManagerFactoryHelper;
interface IKeyManagerFactorySpi;
EXTERN const _ELASTOS InterfaceID EIID_IKeyManagerFactorySpi;
interface IKeyStoreBuilderParameters;
EXTERN const _ELASTOS InterfaceID EIID_IKeyStoreBuilderParameters;
interface IManagerFactoryParameters;
EXTERN const _ELASTOS InterfaceID EIID_IManagerFactoryParameters;
interface ISSLContext;
EXTERN const _ELASTOS InterfaceID EIID_ISSLContext;
interface ISSLContextHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISSLContextHelper;
interface ISSLContextSpi;
EXTERN const _ELASTOS InterfaceID EIID_ISSLContextSpi;
interface ISSLEngine;
EXTERN const _ELASTOS InterfaceID EIID_ISSLEngine;
interface ISSLEngineResult;
EXTERN const _ELASTOS InterfaceID EIID_ISSLEngineResult;
interface ISSLParameters;
EXTERN const _ELASTOS InterfaceID EIID_ISSLParameters;
interface ISSLServerSocket;
EXTERN const _ELASTOS InterfaceID EIID_ISSLServerSocket;
interface ISSLServerSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISSLServerSocketFactory;
interface ISSLServerSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISSLServerSocketFactoryHelper;
interface ISSLSession;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSession;
interface ISSLSessionBindingEvent;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSessionBindingEvent;
interface ISSLSessionBindingListener;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSessionBindingListener;
interface ISSLSessionContext;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSessionContext;
interface ISSLSocket;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSocket;
interface ISSLSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSocketFactory;
interface ISSLSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSocketFactoryHelper;
interface ITrustManagerFactory;
EXTERN const _ELASTOS InterfaceID EIID_ITrustManagerFactory;
interface ITrustManagerFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITrustManagerFactoryHelper;
interface ITrustManagerFactorySpi;
EXTERN const _ELASTOS InterfaceID EIID_ITrustManagerFactorySpi;
interface ITrustManager;
EXTERN const _ELASTOS InterfaceID EIID_ITrustManager;
interface IX509ExtendedKeyManager;
EXTERN const _ELASTOS InterfaceID EIID_IX509ExtendedKeyManager;
interface IX509KeyManager;
EXTERN const _ELASTOS InterfaceID EIID_IX509KeyManager;
interface IX509TrustManager;
EXTERN const _ELASTOS InterfaceID EIID_IX509TrustManager;
interface ICCertPathTrustManagerParametersClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCertPathTrustManagerParametersClassObject;
interface ICDefaultHostnameVerifierClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultHostnameVerifierClassObject;
interface ICDefaultSSLServerSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultSSLServerSocketFactoryClassObject;
interface ICDefaultSSLSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultSSLSocketFactoryClassObject;
interface ICDistinguishedNameParserClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDistinguishedNameParserClassObject;
interface ICHandshakeCompletedEventClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHandshakeCompletedEventClassObject;
interface ICKeyManagerFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyManagerFactoryClassObject;
interface ICKeyManagerFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKeyManagerFactoryHelperClassObject;
interface ICHttpsURLConnectionHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpsURLConnectionHelperClassObject;
interface ICSSLContextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLContextClassObject;
interface ICSSLContextHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLContextHelperClassObject;
interface ICSSLEngineResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLEngineResultClassObject;
interface ICSSLParametersClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLParametersClassObject;
interface ICSSLPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLPermissionClassObject;
interface ICSSLServerSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLServerSocketFactoryHelperClassObject;
interface ICSSLSessionBindingEventClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLSessionBindingEventClassObject;
interface ICSSLSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLSocketFactoryHelperClassObject;
interface ICTrustManagerFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTrustManagerFactoryClassObject;
interface ICTrustManagerFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTrustManagerFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCertPathTrustManagerParameters;
EXTERN const _ELASTOS ClassID ECLSID_CCertPathTrustManagerParametersClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHostnameVerifier;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHostnameVerifierClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSSLServerSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSSLServerSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSSLSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultSSLSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDistinguishedNameParser;
EXTERN const _ELASTOS ClassID ECLSID_CDistinguishedNameParserClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHandshakeCompletedEvent;
EXTERN const _ELASTOS ClassID ECLSID_CHandshakeCompletedEventClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyManagerFactory;
EXTERN const _ELASTOS ClassID ECLSID_CKeyManagerFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKeyManagerFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CKeyManagerFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpsURLConnectionHelper;
EXTERN const _ELASTOS ClassID ECLSID_CHttpsURLConnectionHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLContext;
EXTERN const _ELASTOS ClassID ECLSID_CSSLContextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLContextHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSSLContextHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLEngineResult;
EXTERN const _ELASTOS ClassID ECLSID_CSSLEngineResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLParameters;
EXTERN const _ELASTOS ClassID ECLSID_CSSLParametersClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLPermission;
EXTERN const _ELASTOS ClassID ECLSID_CSSLPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLServerSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSSLServerSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSessionBindingEvent;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSessionBindingEventClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTrustManagerFactory;
EXTERN const _ELASTOS ClassID ECLSID_CTrustManagerFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CTrustManagerFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CTrustManagerFactoryHelperClassObject;
}
}
}

namespace Elastosx {
namespace Security {
namespace Auth {
namespace Callback {
interface IPasswordCallback;
EXTERN const _ELASTOS InterfaceID EIID_IPasswordCallback;
interface ICallback;
EXTERN const _ELASTOS InterfaceID EIID_ICallback;
interface ICallbackHandler;
EXTERN const _ELASTOS InterfaceID EIID_ICallbackHandler;
interface ICPasswordCallbackClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPasswordCallbackClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPasswordCallback;
EXTERN const _ELASTOS ClassID ECLSID_CPasswordCallbackClassObject;
}
}
}
}

namespace Elastosx {
namespace Security {
namespace Auth {
interface IPrivateCredentialPermission;
EXTERN const _ELASTOS InterfaceID EIID_IPrivateCredentialPermission;
interface ISubject;
EXTERN const _ELASTOS InterfaceID EIID_ISubject;
interface ISubjectHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISubjectHelper;
interface ISubjectDomainCombiner;
EXTERN const _ELASTOS InterfaceID EIID_ISubjectDomainCombiner;
interface IDestroyable;
EXTERN const _ELASTOS InterfaceID EIID_IDestroyable;
interface IAuthPermission;
EXTERN const _ELASTOS InterfaceID EIID_IAuthPermission;
interface ICAuthPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthPermissionClassObject;
interface ICPrivateCredentialPermissionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPrivateCredentialPermissionClassObject;
interface ICSubjectClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSubjectClassObject;
interface ICSubjectHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSubjectHelperClassObject;
interface ICSubjectDomainCombinerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSubjectDomainCombinerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthPermission;
EXTERN const _ELASTOS ClassID ECLSID_CAuthPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPrivateCredentialPermission;
EXTERN const _ELASTOS ClassID ECLSID_CPrivateCredentialPermissionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSubject;
EXTERN const _ELASTOS ClassID ECLSID_CSubjectClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSubjectHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSubjectHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSubjectDomainCombiner;
EXTERN const _ELASTOS ClassID ECLSID_CSubjectDomainCombinerClassObject;
}
}
}

namespace Elastosx {
namespace Security {
namespace Auth {
namespace X500 {
interface IX500Principal;
EXTERN const _ELASTOS InterfaceID EIID_IX500Principal;
interface ICX500PrincipalClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICX500PrincipalClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CX500Principal;
EXTERN const _ELASTOS ClassID ECLSID_CX500PrincipalClassObject;
}
}
}
}

namespace Elastosx {
namespace Security {
namespace Cert {
interface ICertificate;
EXTERN const _ELASTOS InterfaceID EIID_ICertificate;
interface IX509Certificate;
EXTERN const _ELASTOS InterfaceID EIID_IX509Certificate;
interface IX509CertificateHelper;
EXTERN const _ELASTOS InterfaceID EIID_IX509CertificateHelper;
interface ICX509CertificateHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICX509CertificateHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CX509CertificateHelper;
EXTERN const _ELASTOS ClassID ECLSID_CX509CertificateHelperClassObject;
}
}
}

namespace Elastosx {
namespace Sql {
interface ICommonDataSource;
EXTERN const _ELASTOS InterfaceID EIID_ICommonDataSource;
interface IConnectionEvent;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionEvent;
interface IConnectionEventListener;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionEventListener;
interface IDataSource;
EXTERN const _ELASTOS InterfaceID EIID_IDataSource;
interface IStatementEvent;
EXTERN const _ELASTOS InterfaceID EIID_IStatementEvent;
interface IStatementEventListener;
EXTERN const _ELASTOS InterfaceID EIID_IStatementEventListener;
interface IPooledConnection;
EXTERN const _ELASTOS InterfaceID EIID_IPooledConnection;
interface IConnectionPoolDataSource;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionPoolDataSource;
interface IRowSetEvent;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetEvent;
interface IRowSetListener;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetListener;
interface IRowSet;
EXTERN const _ELASTOS InterfaceID EIID_IRowSet;
interface IRowSetMetaData;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetMetaData;
interface IRowSetInternal;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetInternal;
interface IRowSetReader;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetReader;
interface IRowSetWriter;
EXTERN const _ELASTOS InterfaceID EIID_IRowSetWriter;
interface ICConnectionEventClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnectionEventClassObject;
interface ICRowSetEventClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRowSetEventClassObject;
interface ICStatementEventClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStatementEventClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnectionEvent;
EXTERN const _ELASTOS ClassID ECLSID_CConnectionEventClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRowSetEvent;
EXTERN const _ELASTOS ClassID ECLSID_CRowSetEventClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStatementEvent;
EXTERN const _ELASTOS ClassID ECLSID_CStatementEventClassObject;
}
}

namespace Elastosx {
namespace Xml {
interface IXMLConstants;
EXTERN const _ELASTOS InterfaceID EIID_IXMLConstants;
}
}

namespace Elastosx {
namespace Xml {
namespace Parsers {
interface IDocumentBuilder;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentBuilder;
interface IDocumentBuilderFactory;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentBuilderFactory;
interface IDocumentBuilderFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentBuilderFactoryHelper;
interface ISAXParserFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISAXParserFactory;
interface ISAXParserFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISAXParserFactoryHelper;
interface ISAXParser;
EXTERN const _ELASTOS InterfaceID EIID_ISAXParser;
interface ICDocumentBuilderFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDocumentBuilderFactoryHelperClassObject;
interface ICSAXParserFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSAXParserFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDocumentBuilderFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDocumentBuilderFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSAXParserFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSAXParserFactoryHelperClassObject;
}
}
}

namespace Elastosx {
namespace Xml {
namespace Validation {
interface ITypeInfoProvider;
EXTERN const _ELASTOS InterfaceID EIID_ITypeInfoProvider;
interface IValidator;
EXTERN const _ELASTOS InterfaceID EIID_IValidator;
interface IValidatorHandler;
EXTERN const _ELASTOS InterfaceID EIID_IValidatorHandler;
interface ISchema;
EXTERN const _ELASTOS InterfaceID EIID_ISchema;
interface ISchemaFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISchemaFactory;
interface ISchemaFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISchemaFactoryHelper;
interface ISchemaFactoryFinder;
EXTERN const _ELASTOS InterfaceID EIID_ISchemaFactoryFinder;
interface ISchemaFactoryLoader;
EXTERN const _ELASTOS InterfaceID EIID_ISchemaFactoryLoader;
interface ICSchemaFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSchemaFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSchemaFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSchemaFactoryHelperClassObject;
}
}
}

namespace Elastosx {
namespace Xml {
namespace Namespace {
interface IQName;
EXTERN const _ELASTOS InterfaceID EIID_IQName;
interface IQNameHelper;
EXTERN const _ELASTOS InterfaceID EIID_IQNameHelper;
interface INamespaceContext;
EXTERN const _ELASTOS InterfaceID EIID_INamespaceContext;
interface ICQNameClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICQNameClassObject;
interface ICQNameHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICQNameHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CQName;
EXTERN const _ELASTOS ClassID ECLSID_CQNameClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CQNameHelper;
EXTERN const _ELASTOS ClassID ECLSID_CQNameHelperClassObject;
}
}
}

namespace Elastosx {
namespace Xml {
namespace Datatype {
interface IDatatypeConstantsField;
EXTERN const _ELASTOS InterfaceID EIID_IDatatypeConstantsField;
interface IDatatypeConstants;
EXTERN const _ELASTOS InterfaceID EIID_IDatatypeConstants;
interface IDuration;
EXTERN const _ELASTOS InterfaceID EIID_IDuration;
interface IXMLGregorianCalendar;
EXTERN const _ELASTOS InterfaceID EIID_IXMLGregorianCalendar;
interface IDatatypeFactory;
EXTERN const _ELASTOS InterfaceID EIID_IDatatypeFactory;
interface IDatatypeFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IDatatypeFactoryHelper;
interface ICDatatypeConstantsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatatypeConstantsClassObject;
interface ICDatatypeFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDatatypeFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatatypeConstants;
EXTERN const _ELASTOS ClassID ECLSID_CDatatypeConstantsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDatatypeFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CDatatypeFactoryHelperClassObject;
}
}
}

namespace Elastosx {
namespace Xml {
namespace Transform {
interface IOutputKeys;
EXTERN const _ELASTOS InterfaceID EIID_IOutputKeys;
interface IResult;
EXTERN const _ELASTOS InterfaceID EIID_IResult;
interface ISource;
EXTERN const _ELASTOS InterfaceID EIID_ISource;
interface ISourceLocator;
EXTERN const _ELASTOS InterfaceID EIID_ISourceLocator;
interface IURIResolver;
EXTERN const _ELASTOS InterfaceID EIID_IURIResolver;
interface ITransformer;
EXTERN const _ELASTOS InterfaceID EIID_ITransformer;
interface ITemplates;
EXTERN const _ELASTOS InterfaceID EIID_ITemplates;
interface ITransformerFactory;
EXTERN const _ELASTOS InterfaceID EIID_ITransformerFactory;
interface ITransformerFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITransformerFactoryHelper;
interface IErrorListener;
EXTERN const _ELASTOS InterfaceID EIID_IErrorListener;
}
}
}

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Dom {
interface IDOMLocator;
EXTERN const _ELASTOS InterfaceID EIID_IDOMLocator;
interface IDOMResult;
EXTERN const _ELASTOS InterfaceID EIID_IDOMResult;
interface IDOMSource;
EXTERN const _ELASTOS InterfaceID EIID_IDOMSource;
interface ICDOMSourceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDOMSourceClassObject;
interface ICDOMResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDOMResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDOMSource;
EXTERN const _ELASTOS ClassID ECLSID_CDOMSourceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDOMResult;
EXTERN const _ELASTOS ClassID ECLSID_CDOMResultClassObject;
}
}
}
}

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {
interface ISAXResult;
EXTERN const _ELASTOS InterfaceID EIID_ISAXResult;
interface ISAXSource;
EXTERN const _ELASTOS InterfaceID EIID_ISAXSource;
interface ITemplatesHandler;
EXTERN const _ELASTOS InterfaceID EIID_ITemplatesHandler;
interface ITransformerHandler;
EXTERN const _ELASTOS InterfaceID EIID_ITransformerHandler;
interface ISAXTransformerFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISAXTransformerFactory;
interface ICSAXResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSAXResultClassObject;
interface ICSAXSourceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSAXSourceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSAXResult;
EXTERN const _ELASTOS ClassID ECLSID_CSAXResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSAXSource;
EXTERN const _ELASTOS ClassID ECLSID_CSAXSourceClassObject;
}
}
}
}

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {
interface IFilePathToURI;
EXTERN const _ELASTOS InterfaceID EIID_IFilePathToURI;
interface IStreamResult;
EXTERN const _ELASTOS InterfaceID EIID_IStreamResult;
interface IStreamSource;
EXTERN const _ELASTOS InterfaceID EIID_IStreamSource;
interface ICStreamResultClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStreamResultClassObject;
interface ICStreamSourceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStreamSourceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStreamResult;
EXTERN const _ELASTOS ClassID ECLSID_CStreamResultClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStreamSource;
EXTERN const _ELASTOS ClassID ECLSID_CStreamSourceClassObject;
}
}
}
}

namespace Elastosx {
namespace Xml {
namespace XPath {
interface IXPathConstants;
EXTERN const _ELASTOS InterfaceID EIID_IXPathConstants;
interface IXPathExpression;
EXTERN const _ELASTOS InterfaceID EIID_IXPathExpression;
interface IXPathFunction;
EXTERN const _ELASTOS InterfaceID EIID_IXPathFunction;
interface IXPathFunctionResolver;
EXTERN const _ELASTOS InterfaceID EIID_IXPathFunctionResolver;
interface IXPathVariableResolver;
EXTERN const _ELASTOS InterfaceID EIID_IXPathVariableResolver;
interface IXPath;
EXTERN const _ELASTOS InterfaceID EIID_IXPath;
interface IXPathFactory;
EXTERN const _ELASTOS InterfaceID EIID_IXPathFactory;
interface IXPathFactoryFinder;
EXTERN const _ELASTOS InterfaceID EIID_IXPathFactoryFinder;
interface ICXPathConstantsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXPathConstantsClassObject;
interface ICXPathFactoryFinderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXPathFactoryFinderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXPathConstants;
EXTERN const _ELASTOS ClassID ECLSID_CXPathConstantsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXPathFactoryFinder;
EXTERN const _ELASTOS ClassID ECLSID_CXPathFactoryFinderClassObject;
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {
interface IASN1Constants;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Constants;
interface IASN1OctetString;
EXTERN const _ELASTOS InterfaceID EIID_IASN1OctetString;
interface IASN1OctetStringHelper;
EXTERN const _ELASTOS InterfaceID EIID_IASN1OctetStringHelper;
interface IASN1Type;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Type;
interface IASN1StringType;
EXTERN const _ELASTOS InterfaceID EIID_IASN1StringType;
interface IASN1StringTypeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IASN1StringTypeHelper;
interface IASN1Time;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Time;
interface IASN1UTCTime;
EXTERN const _ELASTOS InterfaceID EIID_IASN1UTCTime;
interface IASN1UTCTimeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IASN1UTCTimeHelper;
interface IASN1Choice;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Choice;
interface IASN1Explicit;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Explicit;
interface IASN1TypeCollection;
EXTERN const _ELASTOS InterfaceID EIID_IASN1TypeCollection;
interface IASN1ValueCollection;
EXTERN const _ELASTOS InterfaceID EIID_IASN1ValueCollection;
interface IASN1Sequence;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Sequence;
interface IASN1SequenceOf;
EXTERN const _ELASTOS InterfaceID EIID_IASN1SequenceOf;
interface IASN1Set;
EXTERN const _ELASTOS InterfaceID EIID_IASN1Set;
interface IASN1SetOf;
EXTERN const _ELASTOS InterfaceID EIID_IASN1SetOf;
interface IBitString;
EXTERN const _ELASTOS InterfaceID EIID_IBitString;
interface IBerInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IBerInputStream;
interface IBerInputStreamHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBerInputStreamHelper;
interface IBerOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IBerOutputStream;
interface IDerInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDerInputStream;
interface IDerOutputStream;
EXTERN const _ELASTOS InterfaceID EIID_IDerOutputStream;
interface ICASN1OctetStringClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICASN1OctetStringClassObject;
interface ICASN1OctetStringHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICASN1OctetStringHelperClassObject;
interface ICBerInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBerInputStreamClassObject;
interface ICBerOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBerOutputStreamClassObject;
interface ICDerInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDerInputStreamClassObject;
interface ICDerOutputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDerOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CASN1OctetString;
EXTERN const _ELASTOS ClassID ECLSID_CASN1OctetStringClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CASN1OctetStringHelper;
EXTERN const _ELASTOS ClassID ECLSID_CASN1OctetStringHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBerInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CBerInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBerOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CBerOutputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDerInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDerInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDerOutputStream;
EXTERN const _ELASTOS ClassID ECLSID_CDerOutputStreamClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {
interface ISpiAndProvider;
EXTERN const _ELASTOS InterfaceID EIID_ISpiAndProvider;
interface IEngine;
EXTERN const _ELASTOS InterfaceID EIID_IEngine;
interface IEngineHelper;
EXTERN const _ELASTOS InterfaceID EIID_IEngineHelper;
interface ISecurityAccess;
EXTERN const _ELASTOS InterfaceID EIID_ISecurityAccess;
interface IServices;
EXTERN const _ELASTOS InterfaceID EIID_IServices;
interface ICEngineClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEngineClassObject;
interface ICServicesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICServicesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEngine;
EXTERN const _ELASTOS ClassID ECLSID_CEngineClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CServices;
EXTERN const _ELASTOS ClassID ECLSID_CServicesClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {
interface IContentInfo;
EXTERN const _ELASTOS InterfaceID EIID_IContentInfo;
interface IContentInfoHelper;
EXTERN const _ELASTOS InterfaceID EIID_IContentInfoHelper;
interface ISignedData;
EXTERN const _ELASTOS InterfaceID EIID_ISignedData;
interface ISignedDataHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISignedDataHelper;
interface ISignerInfo;
EXTERN const _ELASTOS InterfaceID EIID_ISignerInfo;
interface ISignerInfoHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISignerInfoHelper;
interface ICContentInfoHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICContentInfoHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CContentInfoHelper;
EXTERN const _ELASTOS ClassID ECLSID_CContentInfoHelperClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {
interface IObjectIdentifier;
EXTERN const _ELASTOS InterfaceID EIID_IObjectIdentifier;
interface IObjectIdentifierHelper;
EXTERN const _ELASTOS InterfaceID EIID_IObjectIdentifierHelper;
interface IJarUtils;
EXTERN const _ELASTOS InterfaceID EIID_IJarUtils;
interface ICJarUtilsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJarUtilsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJarUtils;
EXTERN const _ELASTOS ClassID ECLSID_CJarUtilsClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {
interface IAttributeValue;
EXTERN const _ELASTOS InterfaceID EIID_IAttributeValue;
interface IAttributeTypeAndValue;
EXTERN const _ELASTOS InterfaceID EIID_IAttributeTypeAndValue;
interface IAttributeTypeAndValueHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAttributeTypeAndValueHelper;
interface IName;
EXTERN const _ELASTOS InterfaceID EIID_IName;
interface INameHelper;
EXTERN const _ELASTOS InterfaceID EIID_INameHelper;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
interface IAlgorithmIdentifier;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmIdentifier;
interface IAlgorithmIdentifierHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAlgorithmIdentifierHelper;
interface IBasicConstraints;
EXTERN const _ELASTOS InterfaceID EIID_IBasicConstraints;
interface IBasicConstraintsHelper;
EXTERN const _ELASTOS InterfaceID EIID_IBasicConstraintsHelper;
interface ICertificate;
EXTERN const _ELASTOS InterfaceID EIID_ICertificate;
interface ICertificateHelper;
EXTERN const _ELASTOS InterfaceID EIID_ICertificateHelper;
interface IExtension;
EXTERN const _ELASTOS InterfaceID EIID_IExtension;
interface IExtensionHelper;
EXTERN const _ELASTOS InterfaceID EIID_IExtensionHelper;
interface IExtensions;
EXTERN const _ELASTOS InterfaceID EIID_IExtensions;
interface IExtensionsHelper;
EXTERN const _ELASTOS InterfaceID EIID_IExtensionsHelper;
interface IExtensionValue;
EXTERN const _ELASTOS InterfaceID EIID_IExtensionValue;
interface IKeyUsage;
EXTERN const _ELASTOS InterfaceID EIID_IKeyUsage;
interface ISubjectPublicKeyInfo;
EXTERN const _ELASTOS InterfaceID EIID_ISubjectPublicKeyInfo;
interface ISubjectPublicKeyInfoHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISubjectPublicKeyInfoHelper;
interface ITBSCertificate;
EXTERN const _ELASTOS InterfaceID EIID_ITBSCertificate;
interface ITBSCertificateHelper;
EXTERN const _ELASTOS InterfaceID EIID_ITBSCertificateHelper;
interface IValidity;
EXTERN const _ELASTOS InterfaceID EIID_IValidity;
interface IValidityHelper;
EXTERN const _ELASTOS InterfaceID EIID_IValidityHelper;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
interface IConnectionReuseStrategy;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionReuseStrategy;
interface IFormattedHeader;
EXTERN const _ELASTOS InterfaceID EIID_IFormattedHeader;
interface IHeader;
EXTERN const _ELASTOS InterfaceID EIID_IHeader;
interface IHeaderElement;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderElement;
interface IHeaderElementIterator;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderElementIterator;
interface IHeaderIterator;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderIterator;
interface IHttpClientConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpClientConnection;
interface IHttpConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpConnection;
interface IHttpConnectionMetrics;
EXTERN const _ELASTOS InterfaceID EIID_IHttpConnectionMetrics;
interface IHttpEntity;
EXTERN const _ELASTOS InterfaceID EIID_IHttpEntity;
interface IHttpEntityEnclosingRequest;
EXTERN const _ELASTOS InterfaceID EIID_IHttpEntityEnclosingRequest;
interface IHttpHost;
EXTERN const _ELASTOS InterfaceID EIID_IHttpHost;
interface IHttpInetConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpInetConnection;
interface IHttpMessage;
EXTERN const _ELASTOS InterfaceID EIID_IHttpMessage;
interface IHttpRequest;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequest;
interface IHttpRequestFactory;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestFactory;
interface IHttpRequestInterceptor;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestInterceptor;
interface IHttpResponse;
EXTERN const _ELASTOS InterfaceID EIID_IHttpResponse;
interface IHttpResponseFactory;
EXTERN const _ELASTOS InterfaceID EIID_IHttpResponseFactory;
interface IHttpResponseInterceptor;
EXTERN const _ELASTOS InterfaceID EIID_IHttpResponseInterceptor;
interface IHttpServerConnection;
EXTERN const _ELASTOS InterfaceID EIID_IHttpServerConnection;
interface IHttpStatus;
EXTERN const _ELASTOS InterfaceID EIID_IHttpStatus;
interface IHttpVersion;
EXTERN const _ELASTOS InterfaceID EIID_IHttpVersion;
interface IHttpVersionHelper;
EXTERN const _ELASTOS InterfaceID EIID_IHttpVersionHelper;
interface INameValuePair;
EXTERN const _ELASTOS InterfaceID EIID_INameValuePair;
interface IProtocolVersion;
EXTERN const _ELASTOS InterfaceID EIID_IProtocolVersion;
interface IReasonPhraseCatalog;
EXTERN const _ELASTOS InterfaceID EIID_IReasonPhraseCatalog;
interface IRequestLine;
EXTERN const _ELASTOS InterfaceID EIID_IRequestLine;
interface IStatusLine;
EXTERN const _ELASTOS InterfaceID EIID_IStatusLine;
interface ITokenIterator;
EXTERN const _ELASTOS InterfaceID EIID_ITokenIterator;
interface ICHttpHostClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpHostClassObject;
interface ICProtocolVersionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICProtocolVersionClassObject;
interface ICHttpVersionClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpVersionClassObject;
interface ICHttpVersionHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpVersionHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpHost;
EXTERN const _ELASTOS ClassID ECLSID_CHttpHostClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CProtocolVersion;
EXTERN const _ELASTOS ClassID ECLSID_CProtocolVersionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpVersion;
EXTERN const _ELASTOS ClassID ECLSID_CHttpVersionClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpVersionHelper;
EXTERN const _ELASTOS ClassID ECLSID_CHttpVersionHelperClassObject;
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
interface IAUTH;
EXTERN const _ELASTOS InterfaceID EIID_IAUTH;
interface IAuthScheme;
EXTERN const _ELASTOS InterfaceID EIID_IAuthScheme;
interface IAuthSchemeFactory;
EXTERN const _ELASTOS InterfaceID EIID_IAuthSchemeFactory;
interface IAuthSchemeRegistry;
EXTERN const _ELASTOS InterfaceID EIID_IAuthSchemeRegistry;
interface IAuthScope;
EXTERN const _ELASTOS InterfaceID EIID_IAuthScope;
interface IAuthScopeHelper;
EXTERN const _ELASTOS InterfaceID EIID_IAuthScopeHelper;
interface IAuthState;
EXTERN const _ELASTOS InterfaceID EIID_IAuthState;
interface IBasicUserPrincipal;
EXTERN const _ELASTOS InterfaceID EIID_IBasicUserPrincipal;
interface ICredentials;
EXTERN const _ELASTOS InterfaceID EIID_ICredentials;
interface INTCredentials;
EXTERN const _ELASTOS InterfaceID EIID_INTCredentials;
interface INTUserPrincipal;
EXTERN const _ELASTOS InterfaceID EIID_INTUserPrincipal;
interface IUsernamePasswordCredentials;
EXTERN const _ELASTOS InterfaceID EIID_IUsernamePasswordCredentials;
interface ICAuthSchemeRegistryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthSchemeRegistryClassObject;
interface ICAuthScopeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthScopeClassObject;
interface ICAuthScopeHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthScopeHelperClassObject;
interface ICAuthStateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthStateClassObject;
interface ICBasicUserPrincipalClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicUserPrincipalClassObject;
interface ICNTCredentialsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNTCredentialsClassObject;
interface ICNTUserPrincipalClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNTUserPrincipalClassObject;
interface ICUsernamePasswordCredentialsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUsernamePasswordCredentialsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthSchemeRegistry;
EXTERN const _ELASTOS ClassID ECLSID_CAuthSchemeRegistryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthScope;
EXTERN const _ELASTOS ClassID ECLSID_CAuthScopeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthScopeHelper;
EXTERN const _ELASTOS ClassID ECLSID_CAuthScopeHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthState;
EXTERN const _ELASTOS ClassID ECLSID_CAuthStateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicUserPrincipal;
EXTERN const _ELASTOS ClassID ECLSID_CBasicUserPrincipalClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNTCredentials;
EXTERN const _ELASTOS ClassID ECLSID_CNTCredentialsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNTUserPrincipal;
EXTERN const _ELASTOS ClassID ECLSID_CNTUserPrincipalClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUsernamePasswordCredentials;
EXTERN const _ELASTOS ClassID ECLSID_CUsernamePasswordCredentialsClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {
interface IAuthParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IAuthParamBean;
interface IAuthParams;
EXTERN const _ELASTOS InterfaceID EIID_IAuthParams;
interface IAuthPNames;
EXTERN const _ELASTOS InterfaceID EIID_IAuthPNames;
interface ICAuthParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthParamBeanClassObject;
interface ICAuthParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAuthParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CAuthParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAuthParams;
EXTERN const _ELASTOS ClassID ECLSID_CAuthParamsClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
interface IAuthenticationHandler;
EXTERN const _ELASTOS InterfaceID EIID_IAuthenticationHandler;
interface ICookieStore;
EXTERN const _ELASTOS InterfaceID EIID_ICookieStore;
interface ICredentialsProvider;
EXTERN const _ELASTOS InterfaceID EIID_ICredentialsProvider;
interface IHttpClient;
EXTERN const _ELASTOS InterfaceID EIID_IHttpClient;
interface IHttpRequestRetryHandler;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestRetryHandler;
interface IRedirectHandler;
EXTERN const _ELASTOS InterfaceID EIID_IRedirectHandler;
interface IRequestDirector;
EXTERN const _ELASTOS InterfaceID EIID_IRequestDirector;
interface IResponseHandler;
EXTERN const _ELASTOS InterfaceID EIID_IResponseHandler;
interface IUserTokenHandler;
EXTERN const _ELASTOS InterfaceID EIID_IUserTokenHandler;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {
interface IAbortableHttpRequest;
EXTERN const _ELASTOS InterfaceID EIID_IAbortableHttpRequest;
interface IHttpDelete;
EXTERN const _ELASTOS InterfaceID EIID_IHttpDelete;
interface IHttpGet;
EXTERN const _ELASTOS InterfaceID EIID_IHttpGet;
interface IHttpHead;
EXTERN const _ELASTOS InterfaceID EIID_IHttpHead;
interface IHttpOptions;
EXTERN const _ELASTOS InterfaceID EIID_IHttpOptions;
interface IHttpPost;
EXTERN const _ELASTOS InterfaceID EIID_IHttpPost;
interface IHttpPut;
EXTERN const _ELASTOS InterfaceID EIID_IHttpPut;
interface IHttpTrace;
EXTERN const _ELASTOS InterfaceID EIID_IHttpTrace;
interface IHttpUriRequest;
EXTERN const _ELASTOS InterfaceID EIID_IHttpUriRequest;
interface ICHttpDeleteClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpDeleteClassObject;
interface ICHttpGetClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpGetClassObject;
interface ICHttpHeadClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpHeadClassObject;
interface ICHttpOptionsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpOptionsClassObject;
interface ICHttpPostClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpPostClassObject;
interface ICHttpPutClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpPutClassObject;
interface ICHttpTraceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpTraceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDelete;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDeleteClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpGet;
EXTERN const _ELASTOS ClassID ECLSID_CHttpGetClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpHead;
EXTERN const _ELASTOS ClassID ECLSID_CHttpHeadClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpOptions;
EXTERN const _ELASTOS ClassID ECLSID_CHttpOptionsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpPost;
EXTERN const _ELASTOS ClassID ECLSID_CHttpPostClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpPut;
EXTERN const _ELASTOS ClassID ECLSID_CHttpPutClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpTrace;
EXTERN const _ELASTOS ClassID ECLSID_CHttpTraceClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {
interface IAllClientPNames;
EXTERN const _ELASTOS InterfaceID EIID_IAllClientPNames;
interface IAuthPolicy;
EXTERN const _ELASTOS InterfaceID EIID_IAuthPolicy;
interface IClientParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IClientParamBean;
interface IClientPNames;
EXTERN const _ELASTOS InterfaceID EIID_IClientPNames;
interface ICookiePolicy;
EXTERN const _ELASTOS InterfaceID EIID_ICookiePolicy;
interface IHttpClientParams;
EXTERN const _ELASTOS InterfaceID EIID_IHttpClientParams;
interface ICClientParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICClientParamBeanClassObject;
interface ICHttpClientParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpClientParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CClientParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CClientParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpClientParams;
EXTERN const _ELASTOS ClassID ECLSID_CHttpClientParamsClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {
interface IClientContext;
EXTERN const _ELASTOS InterfaceID EIID_IClientContext;
interface IClientContextConfigurer;
EXTERN const _ELASTOS InterfaceID EIID_IClientContextConfigurer;
interface ICClientContextConfigurerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICClientContextConfigurerClassObject;
interface ICRequestAddCookiesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestAddCookiesClassObject;
interface ICRequestDefaultHeadersClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestDefaultHeadersClassObject;
interface ICRequestProxyAuthenticationClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestProxyAuthenticationClassObject;
interface ICRequestTargetAuthenticationClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestTargetAuthenticationClassObject;
interface ICResponseProcessCookiesClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseProcessCookiesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CClientContextConfigurer;
EXTERN const _ELASTOS ClassID ECLSID_CClientContextConfigurerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestAddCookies;
EXTERN const _ELASTOS ClassID ECLSID_CRequestAddCookiesClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestDefaultHeaders;
EXTERN const _ELASTOS ClassID ECLSID_CRequestDefaultHeadersClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestProxyAuthentication;
EXTERN const _ELASTOS ClassID ECLSID_CRequestProxyAuthenticationClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestTargetAuthentication;
EXTERN const _ELASTOS ClassID ECLSID_CRequestTargetAuthenticationClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseProcessCookies;
EXTERN const _ELASTOS ClassID ECLSID_CResponseProcessCookiesClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
interface IClientConnectionManager;
EXTERN const _ELASTOS InterfaceID EIID_IClientConnectionManager;
interface IClientConnectionManagerFactory;
EXTERN const _ELASTOS InterfaceID EIID_IClientConnectionManagerFactory;
interface IClientConnectionOperator;
EXTERN const _ELASTOS InterfaceID EIID_IClientConnectionOperator;
interface IClientConnectionRequest;
EXTERN const _ELASTOS InterfaceID EIID_IClientConnectionRequest;
interface IConnectionKeepAliveStrategy;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionKeepAliveStrategy;
interface IConnectionReleaseTrigger;
EXTERN const _ELASTOS InterfaceID EIID_IConnectionReleaseTrigger;
interface IEofSensorInputStream;
EXTERN const _ELASTOS InterfaceID EIID_IEofSensorInputStream;
interface IEofSensorWatcher;
EXTERN const _ELASTOS InterfaceID EIID_IEofSensorWatcher;
interface IManagedClientConnection;
EXTERN const _ELASTOS InterfaceID EIID_IManagedClientConnection;
interface IMultihomePlainSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_IMultihomePlainSocketFactory;
interface IMultihomePlainSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IMultihomePlainSocketFactoryHelper;
interface IOperatedClientConnection;
EXTERN const _ELASTOS InterfaceID EIID_IOperatedClientConnection;
interface ICBasicEofSensorWatcherClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicEofSensorWatcherClassObject;
interface ICBasicManagedEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicManagedEntityClassObject;
interface ICEofSensorInputStreamClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEofSensorInputStreamClassObject;
interface ICMultihomePlainSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICMultihomePlainSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicEofSensorWatcher;
EXTERN const _ELASTOS ClassID ECLSID_CBasicEofSensorWatcherClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicManagedEntity;
EXTERN const _ELASTOS ClassID ECLSID_CBasicManagedEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEofSensorInputStream;
EXTERN const _ELASTOS ClassID ECLSID_CEofSensorInputStreamClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CMultihomePlainSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CMultihomePlainSocketFactoryHelperClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {
interface IConnConnectionParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IConnConnectionParamBean;
interface IConnConnectionPNames;
EXTERN const _ELASTOS InterfaceID EIID_IConnConnectionPNames;
interface IConnManagerParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IConnManagerParamBean;
interface IConnManagerParams;
EXTERN const _ELASTOS InterfaceID EIID_IConnManagerParams;
interface IConnManagerPNames;
EXTERN const _ELASTOS InterfaceID EIID_IConnManagerPNames;
interface IConnPerRoute;
EXTERN const _ELASTOS InterfaceID EIID_IConnPerRoute;
interface IConnPerRouteBean;
EXTERN const _ELASTOS InterfaceID EIID_IConnPerRouteBean;
interface IConnRouteParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IConnRouteParamBean;
interface IConnRouteParams;
EXTERN const _ELASTOS InterfaceID EIID_IConnRouteParams;
interface IConnRoutePNames;
EXTERN const _ELASTOS InterfaceID EIID_IConnRoutePNames;
interface ICConnConnectionParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnConnectionParamBeanClassObject;
interface ICConnManagerParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnManagerParamBeanClassObject;
interface ICConnManagerParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnManagerParamsClassObject;
interface ICConnPerRouteBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnPerRouteBeanClassObject;
interface ICConnRouteParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnRouteParamBeanClassObject;
interface ICConnRouteParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICConnRouteParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnConnectionParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CConnConnectionParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnManagerParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CConnManagerParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnManagerParams;
EXTERN const _ELASTOS ClassID ECLSID_CConnManagerParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnPerRouteBean;
EXTERN const _ELASTOS ClassID ECLSID_CConnPerRouteBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnRouteParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CConnRouteParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CConnRouteParams;
EXTERN const _ELASTOS ClassID ECLSID_CConnRouteParamsClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {
interface IBasicRouteDirector;
EXTERN const _ELASTOS InterfaceID EIID_IBasicRouteDirector;
interface IHttpRoute;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRoute;
interface IHttpRouteDirector;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRouteDirector;
interface IHttpRoutePlanner;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRoutePlanner;
interface IRouteInfo;
EXTERN const _ELASTOS InterfaceID EIID_IRouteInfo;
interface IRouteTracker;
EXTERN const _ELASTOS InterfaceID EIID_IRouteTracker;
interface ICBasicRouteDirectorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicRouteDirectorClassObject;
interface ICHttpRouteClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpRouteClassObject;
interface ICRouteTrackerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRouteTrackerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicRouteDirector;
EXTERN const _ELASTOS ClassID ECLSID_CBasicRouteDirectorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRoute;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRouteClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRouteTracker;
EXTERN const _ELASTOS ClassID ECLSID_CRouteTrackerClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {
interface IHostNameResolver;
EXTERN const _ELASTOS InterfaceID EIID_IHostNameResolver;
interface ILayeredSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ILayeredSocketFactory;
interface IPlainSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_IPlainSocketFactory;
interface IPlainSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IPlainSocketFactoryHelper;
interface IScheme;
EXTERN const _ELASTOS InterfaceID EIID_IScheme;
interface ISchemeRegistry;
EXTERN const _ELASTOS InterfaceID EIID_ISchemeRegistry;
interface ISocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISocketFactory;
interface ICPlainSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainSocketFactoryClassObject;
interface ICPlainSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICPlainSocketFactoryHelperClassObject;
interface ICSchemeClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSchemeClassObject;
interface ICSchemeRegistryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSchemeRegistryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CPlainSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CScheme;
EXTERN const _ELASTOS ClassID ECLSID_CSchemeClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSchemeRegistry;
EXTERN const _ELASTOS ClassID ECLSID_CSchemeRegistryClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {
interface IAbstractVerifier;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractVerifier;
interface ISSLSocketFactory;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSocketFactory;
interface ISSLSocketFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_ISSLSocketFactoryHelper;
interface IX509HostnameVerifier;
EXTERN const _ELASTOS InterfaceID EIID_IX509HostnameVerifier;
interface ICAllowAllHostnameVerifierClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAllowAllHostnameVerifierClassObject;
interface ICBrowserCompatHostnameVerifierClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBrowserCompatHostnameVerifierClassObject;
interface ICStrictHostnameVerifierClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStrictHostnameVerifierClassObject;
interface ICSSLSocketFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLSocketFactoryClassObject;
interface ICSSLSocketFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSSLSocketFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAllowAllHostnameVerifier;
EXTERN const _ELASTOS ClassID ECLSID_CAllowAllHostnameVerifierClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBrowserCompatHostnameVerifier;
EXTERN const _ELASTOS ClassID ECLSID_CBrowserCompatHostnameVerifierClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStrictHostnameVerifier;
EXTERN const _ELASTOS ClassID ECLSID_CStrictHostnameVerifierClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactory;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CSSLSocketFactoryHelperClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {
interface IClientCookie;
EXTERN const _ELASTOS InterfaceID EIID_IClientCookie;
interface ICookie;
EXTERN const _ELASTOS InterfaceID EIID_ICookie;
interface ICookieAttributeHandler;
EXTERN const _ELASTOS InterfaceID EIID_ICookieAttributeHandler;
interface ICookieOrigin;
EXTERN const _ELASTOS InterfaceID EIID_ICookieOrigin;
interface ICookieSpec;
EXTERN const _ELASTOS InterfaceID EIID_ICookieSpec;
interface ICookieSpecFactory;
EXTERN const _ELASTOS InterfaceID EIID_ICookieSpecFactory;
interface ICookieSpecRegistry;
EXTERN const _ELASTOS InterfaceID EIID_ICookieSpecRegistry;
interface ISetCookie;
EXTERN const _ELASTOS InterfaceID EIID_ISetCookie;
interface ISetCookie2;
EXTERN const _ELASTOS InterfaceID EIID_ISetCookie2;
interface ISM;
EXTERN const _ELASTOS InterfaceID EIID_ISM;
interface ICCookieIdentityComparatorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieIdentityComparatorClassObject;
interface ICCookieOriginClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieOriginClassObject;
interface ICCookiePathComparatorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookiePathComparatorClassObject;
interface ICCookieSpecRegistryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieSpecRegistryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieIdentityComparator;
EXTERN const _ELASTOS ClassID ECLSID_CCookieIdentityComparatorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieOrigin;
EXTERN const _ELASTOS ClassID ECLSID_CCookieOriginClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookiePathComparator;
EXTERN const _ELASTOS ClassID ECLSID_CCookiePathComparatorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieSpecRegistry;
EXTERN const _ELASTOS ClassID ECLSID_CCookieSpecRegistryClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {
namespace Params {
interface ICookieSpecParamBean;
EXTERN const _ELASTOS InterfaceID EIID_ICookieSpecParamBean;
interface ICookieSpecPNames;
EXTERN const _ELASTOS InterfaceID EIID_ICookieSpecPNames;
interface ICCookieSpecParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCookieSpecParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCookieSpecParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CCookieSpecParamBeanClassObject;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {
interface IAbstractHttpEntity;
EXTERN const _ELASTOS InterfaceID EIID_IAbstractHttpEntity;
interface IBasicHttpEntity;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHttpEntity;
interface IContentLengthStrategy;
EXTERN const _ELASTOS InterfaceID EIID_IContentLengthStrategy;
interface IContentProducer;
EXTERN const _ELASTOS InterfaceID EIID_IContentProducer;
interface ICBasicHttpEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpEntityClassObject;
interface ICBufferedHttpEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedHttpEntityClassObject;
interface ICByteArrayEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteArrayEntityClassObject;
interface ICEntityTemplateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICEntityTemplateClassObject;
interface ICFileEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICFileEntityClassObject;
interface ICInputStreamEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInputStreamEntityClassObject;
interface ICSerializableEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICSerializableEntityClassObject;
interface ICStringEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICStringEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpEntity;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedHttpEntity;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedHttpEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayEntity;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CEntityTemplate;
EXTERN const _ELASTOS ClassID ECLSID_CEntityTemplateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CFileEntity;
EXTERN const _ELASTOS ClassID ECLSID_CFileEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInputStreamEntity;
EXTERN const _ELASTOS ClassID ECLSID_CInputStreamEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CSerializableEntity;
EXTERN const _ELASTOS ClassID ECLSID_CSerializableEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CStringEntity;
EXTERN const _ELASTOS ClassID ECLSID_CStringEntityClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {
interface INTLMEngine;
EXTERN const _ELASTOS InterfaceID EIID_INTLMEngine;
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {
interface IRefQueueHandler;
EXTERN const _ELASTOS InterfaceID EIID_IRefQueueHandler;
interface IPoolEntryRequest;
EXTERN const _ELASTOS InterfaceID EIID_IPoolEntryRequest;
}
}
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace IO {
interface IHttpMessageParser;
EXTERN const _ELASTOS InterfaceID EIID_IHttpMessageParser;
interface IHttpMessageWriter;
EXTERN const _ELASTOS InterfaceID EIID_IHttpMessageWriter;
interface IHttpTransportMetrics;
EXTERN const _ELASTOS InterfaceID EIID_IHttpTransportMetrics;
interface ISessionInputBuffer;
EXTERN const _ELASTOS InterfaceID EIID_ISessionInputBuffer;
interface ISessionOutputBuffer;
EXTERN const _ELASTOS InterfaceID EIID_ISessionOutputBuffer;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Message {
interface IBasicHeaderIterator;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHeaderIterator;
interface IBasicHeaderValueFormatter;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHeaderValueFormatter;
interface IBasicHeaderValueParser;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHeaderValueParser;
interface IBasicHttpResponse;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHttpResponse;
interface IBasicLineFormatter;
EXTERN const _ELASTOS InterfaceID EIID_IBasicLineFormatter;
interface IBasicLineParser;
EXTERN const _ELASTOS InterfaceID EIID_IBasicLineParser;
interface IBasicListHeaderIterator;
EXTERN const _ELASTOS InterfaceID EIID_IBasicListHeaderIterator;
interface IBasicNameValuePair;
EXTERN const _ELASTOS InterfaceID EIID_IBasicNameValuePair;
interface IBasicTokenIterator;
EXTERN const _ELASTOS InterfaceID EIID_IBasicTokenIterator;
interface IHeaderGroup;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderGroup;
interface IHeaderValueFormatter;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderValueFormatter;
interface IHeaderValueParser;
EXTERN const _ELASTOS InterfaceID EIID_IHeaderValueParser;
interface ILineFormatter;
EXTERN const _ELASTOS InterfaceID EIID_ILineFormatter;
interface ILineParser;
EXTERN const _ELASTOS InterfaceID EIID_ILineParser;
interface IParserCursor;
EXTERN const _ELASTOS InterfaceID EIID_IParserCursor;
interface ICBasicLineParserClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicLineParserClassObject;
interface ICBasicTokenIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicTokenIteratorClassObject;
interface ICBasicHeaderValueFormatterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderValueFormatterClassObject;
interface ICBasicHeaderValueParserClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderValueParserClassObject;
interface ICBasicLineFormatterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicLineFormatterClassObject;
interface ICBasicHeaderElementClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderElementClassObject;
interface ICHeaderGroupClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHeaderGroupClassObject;
interface ICBasicHttpResponseClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpResponseClassObject;
interface ICBasicHttpRequestClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpRequestClassObject;
interface ICBasicNameValuePairClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicNameValuePairClassObject;
interface ICBasicListHeaderIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicListHeaderIteratorClassObject;
interface ICBasicHeaderElementIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderElementIteratorClassObject;
interface ICBasicHeaderIteratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderIteratorClassObject;
interface ICBasicHeaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHeaderClassObject;
interface ICBufferedHeaderClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBufferedHeaderClassObject;
interface ICBasicStatusLineClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicStatusLineClassObject;
interface ICParserCursorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICParserCursorClassObject;
interface ICBasicRequestLineClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicRequestLineClassObject;
interface ICBasicHttpEntityEnclosingRequestClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpEntityEnclosingRequestClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLineParser;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLineParserClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicTokenIterator;
EXTERN const _ELASTOS ClassID ECLSID_CBasicTokenIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderValueFormatter;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderValueFormatterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderValueParser;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderValueParserClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLineFormatter;
EXTERN const _ELASTOS ClassID ECLSID_CBasicLineFormatterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderElement;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderElementClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHeaderGroup;
EXTERN const _ELASTOS ClassID ECLSID_CHeaderGroupClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpResponse;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpResponseClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpRequest;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpRequestClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicNameValuePair;
EXTERN const _ELASTOS ClassID ECLSID_CBasicNameValuePairClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicListHeaderIterator;
EXTERN const _ELASTOS ClassID ECLSID_CBasicListHeaderIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderElementIterator;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderElementIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderIterator;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderIteratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeader;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHeaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedHeader;
EXTERN const _ELASTOS ClassID ECLSID_CBufferedHeaderClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicStatusLine;
EXTERN const _ELASTOS ClassID ECLSID_CBasicStatusLineClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CParserCursor;
EXTERN const _ELASTOS ClassID ECLSID_CParserCursorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicRequestLine;
EXTERN const _ELASTOS ClassID ECLSID_CBasicRequestLineClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpEntityEnclosingRequest;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpEntityEnclosingRequestClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Params {
interface IBasicHttpParams;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHttpParams;
interface ICoreConnectionPNames;
EXTERN const _ELASTOS InterfaceID EIID_ICoreConnectionPNames;
interface ICoreProtocolPNames;
EXTERN const _ELASTOS InterfaceID EIID_ICoreProtocolPNames;
interface IDefaultedHttpParams;
EXTERN const _ELASTOS InterfaceID EIID_IDefaultedHttpParams;
interface IHttpConnectionParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IHttpConnectionParamBean;
interface IHttpConnectionParams;
EXTERN const _ELASTOS InterfaceID EIID_IHttpConnectionParams;
interface IHttpParams;
EXTERN const _ELASTOS InterfaceID EIID_IHttpParams;
interface IHttpProtocolParamBean;
EXTERN const _ELASTOS InterfaceID EIID_IHttpProtocolParamBean;
interface IHttpProtocolParams;
EXTERN const _ELASTOS InterfaceID EIID_IHttpProtocolParams;
interface ICHttpProtocolParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpProtocolParamsClassObject;
interface ICHttpConnectionParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpConnectionParamsClassObject;
interface ICBasicHttpParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpParamsClassObject;
interface ICDefaultedHttpParamsClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultedHttpParamsClassObject;
interface ICHttpConnectionParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpConnectionParamBeanClassObject;
interface ICHttpProtocolParamBeanClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpProtocolParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpProtocolParams;
EXTERN const _ELASTOS ClassID ECLSID_CHttpProtocolParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpConnectionParams;
EXTERN const _ELASTOS ClassID ECLSID_CHttpConnectionParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpParams;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultedHttpParams;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultedHttpParamsClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpConnectionParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CHttpConnectionParamBeanClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpProtocolParamBean;
EXTERN const _ELASTOS ClassID ECLSID_CHttpProtocolParamBeanClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {
interface IBasicHttpProcessor;
EXTERN const _ELASTOS InterfaceID EIID_IBasicHttpProcessor;
interface IDefaultedHttpContext;
EXTERN const _ELASTOS InterfaceID EIID_IDefaultedHttpContext;
interface IExecutionContext;
EXTERN const _ELASTOS InterfaceID EIID_IExecutionContext;
interface IHTTP;
EXTERN const _ELASTOS InterfaceID EIID_IHTTP;
interface IHttpContext;
EXTERN const _ELASTOS InterfaceID EIID_IHttpContext;
interface IHttpDateGenerator;
EXTERN const _ELASTOS InterfaceID EIID_IHttpDateGenerator;
interface IHttpDateGeneratorHelper;
EXTERN const _ELASTOS InterfaceID EIID_IHttpDateGeneratorHelper;
interface IHttpExpectationVerifier;
EXTERN const _ELASTOS InterfaceID EIID_IHttpExpectationVerifier;
interface IHttpProcessor;
EXTERN const _ELASTOS InterfaceID EIID_IHttpProcessor;
interface IHttpRequestExecutor;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestExecutor;
interface IHttpRequestHandler;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestHandler;
interface IHttpRequestHandlerRegistry;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestHandlerRegistry;
interface IHttpRequestHandlerResolver;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestHandlerResolver;
interface IHttpRequestInterceptorList;
EXTERN const _ELASTOS InterfaceID EIID_IHttpRequestInterceptorList;
interface IHttpResponseInterceptorList;
EXTERN const _ELASTOS InterfaceID EIID_IHttpResponseInterceptorList;
interface IHttpService;
EXTERN const _ELASTOS InterfaceID EIID_IHttpService;
interface IUriPatternMatcher;
EXTERN const _ELASTOS InterfaceID EIID_IUriPatternMatcher;
interface ICHttpRequestExecutorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpRequestExecutorClassObject;
interface ICBasicHttpProcessorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpProcessorClassObject;
interface ICHttpServiceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpServiceClassObject;
interface ICUriPatternMatcherClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUriPatternMatcherClassObject;
interface ICResponseConnControlClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseConnControlClassObject;
interface ICRequestContentClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestContentClassObject;
interface ICResponseContentClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseContentClassObject;
interface ICRequestTargetHostClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestTargetHostClassObject;
interface ICRequestExpectContinueClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestExpectContinueClassObject;
interface ICBasicHttpContextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICBasicHttpContextClassObject;
interface ICHttpRequestHandlerRegistryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpRequestHandlerRegistryClassObject;
interface ICDefaultedHttpContextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultedHttpContextClassObject;
interface ICHttpDateGeneratorClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpDateGeneratorClassObject;
interface ICHttpDateGeneratorHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHttpDateGeneratorHelperClassObject;
interface ICResponseDateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseDateClassObject;
interface ICRequestDateClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestDateClassObject;
interface ICResponseServerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICResponseServerClassObject;
interface ICRequestUserAgentClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestUserAgentClassObject;
interface ICRequestConnControlClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICRequestConnControlClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRequestExecutor;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRequestExecutorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpProcessor;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpProcessorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpService;
EXTERN const _ELASTOS ClassID ECLSID_CHttpServiceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUriPatternMatcher;
EXTERN const _ELASTOS ClassID ECLSID_CUriPatternMatcherClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseConnControl;
EXTERN const _ELASTOS ClassID ECLSID_CResponseConnControlClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestContent;
EXTERN const _ELASTOS ClassID ECLSID_CRequestContentClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseContent;
EXTERN const _ELASTOS ClassID ECLSID_CResponseContentClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestTargetHost;
EXTERN const _ELASTOS ClassID ECLSID_CRequestTargetHostClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestExpectContinue;
EXTERN const _ELASTOS ClassID ECLSID_CRequestExpectContinueClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpContext;
EXTERN const _ELASTOS ClassID ECLSID_CBasicHttpContextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRequestHandlerRegistry;
EXTERN const _ELASTOS ClassID ECLSID_CHttpRequestHandlerRegistryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultedHttpContext;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultedHttpContextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDateGenerator;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDateGeneratorClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDateGeneratorHelper;
EXTERN const _ELASTOS ClassID ECLSID_CHttpDateGeneratorHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseDate;
EXTERN const _ELASTOS ClassID ECLSID_CResponseDateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestDate;
EXTERN const _ELASTOS ClassID ECLSID_CRequestDateClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CResponseServer;
EXTERN const _ELASTOS ClassID ECLSID_CResponseServerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestUserAgent;
EXTERN const _ELASTOS ClassID ECLSID_CRequestUserAgentClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CRequestConnControl;
EXTERN const _ELASTOS ClassID ECLSID_CRequestConnControlClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {
interface IByteArrayBuffer;
EXTERN const _ELASTOS InterfaceID EIID_IByteArrayBuffer;
interface ICharArrayBuffer;
EXTERN const _ELASTOS InterfaceID EIID_ICharArrayBuffer;
interface IVersionInfo;
EXTERN const _ELASTOS InterfaceID EIID_IVersionInfo;
interface ICCharArrayBufferClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICCharArrayBufferClassObject;
interface ICByteArrayBufferClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICByteArrayBufferClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayBuffer;
EXTERN const _ELASTOS ClassID ECLSID_CCharArrayBufferClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayBuffer;
EXTERN const _ELASTOS ClassID ECLSID_CByteArrayBufferClassObject;
}
}
}
}

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Entity {
interface ICUrlEncodedFormEntityClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICUrlEncodedFormEntityClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CUrlEncodedFormEntity;
EXTERN const _ELASTOS ClassID ECLSID_CUrlEncodedFormEntityClassObject;
}
}
}
}
}

namespace Org {
namespace W3c {
namespace Dom {
interface INodeList;
EXTERN const _ELASTOS InterfaceID EIID_INodeList;
interface INode;
EXTERN const _ELASTOS InterfaceID EIID_INode;
interface IAttr;
EXTERN const _ELASTOS InterfaceID EIID_IAttr;
interface ICharacterData;
EXTERN const _ELASTOS InterfaceID EIID_ICharacterData;
interface IText;
EXTERN const _ELASTOS InterfaceID EIID_IText;
interface ICDATASection;
EXTERN const _ELASTOS InterfaceID EIID_ICDATASection;
interface IComment;
EXTERN const _ELASTOS InterfaceID EIID_IComment;
interface IDOMConfiguration;
EXTERN const _ELASTOS InterfaceID EIID_IDOMConfiguration;
interface IDOMError;
EXTERN const _ELASTOS InterfaceID EIID_IDOMError;
interface IDOMErrorHandler;
EXTERN const _ELASTOS InterfaceID EIID_IDOMErrorHandler;
interface IDOMImplementation;
EXTERN const _ELASTOS InterfaceID EIID_IDOMImplementation;
interface IDOMImplementationList;
EXTERN const _ELASTOS InterfaceID EIID_IDOMImplementationList;
interface IDOMImplementationSource;
EXTERN const _ELASTOS InterfaceID EIID_IDOMImplementationSource;
interface IDOMLocator;
EXTERN const _ELASTOS InterfaceID EIID_IDOMLocator;
interface IDOMStringList;
EXTERN const _ELASTOS InterfaceID EIID_IDOMStringList;
interface IDocument;
EXTERN const _ELASTOS InterfaceID EIID_IDocument;
interface IDocumentFragment;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentFragment;
interface IDocumentType;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentType;
interface IElement;
EXTERN const _ELASTOS InterfaceID EIID_IElement;
interface IEntity;
EXTERN const _ELASTOS InterfaceID EIID_IEntity;
interface IEntityReference;
EXTERN const _ELASTOS InterfaceID EIID_IEntityReference;
interface INameList;
EXTERN const _ELASTOS InterfaceID EIID_INameList;
interface INamedNodeMap;
EXTERN const _ELASTOS InterfaceID EIID_INamedNodeMap;
interface INotation;
EXTERN const _ELASTOS InterfaceID EIID_INotation;
interface IProcessingInstruction;
EXTERN const _ELASTOS InterfaceID EIID_IProcessingInstruction;
interface ITypeInfo;
EXTERN const _ELASTOS InterfaceID EIID_ITypeInfo;
interface IUserDataHandler;
EXTERN const _ELASTOS InterfaceID EIID_IUserDataHandler;
interface ICTextClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICTextClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CText;
EXTERN const _ELASTOS ClassID ECLSID_CTextClassObject;
}
}
}

namespace Org {
namespace W3c {
namespace Dom {
namespace Traversal {
interface INodeFilter;
EXTERN const _ELASTOS InterfaceID EIID_INodeFilter;
interface INodeIterator;
EXTERN const _ELASTOS InterfaceID EIID_INodeIterator;
}
}
}
}

namespace Org {
namespace W3c {
namespace Dom {
namespace Ls {
interface IDOMImplementationLS;
EXTERN const _ELASTOS InterfaceID EIID_IDOMImplementationLS;
interface ILSInput;
EXTERN const _ELASTOS InterfaceID EIID_ILSInput;
interface ILSOutput;
EXTERN const _ELASTOS InterfaceID EIID_ILSOutput;
interface ILSParser;
EXTERN const _ELASTOS InterfaceID EIID_ILSParser;
interface ILSParserFilter;
EXTERN const _ELASTOS InterfaceID EIID_ILSParserFilter;
interface ILSResourceResolver;
EXTERN const _ELASTOS InterfaceID EIID_ILSResourceResolver;
interface ILSSerializer;
EXTERN const _ELASTOS InterfaceID EIID_ILSSerializer;
interface ILSSerializerFilter;
EXTERN const _ELASTOS InterfaceID EIID_ILSSerializerFilter;
}
}
}
}

namespace Org {
namespace Xml {
namespace Sax {
interface IAttributeList;
EXTERN const _ELASTOS InterfaceID EIID_IAttributeList;
interface IAttributes;
EXTERN const _ELASTOS InterfaceID EIID_IAttributes;
interface IContentHandler;
EXTERN const _ELASTOS InterfaceID EIID_IContentHandler;
interface IDocumentHandler;
EXTERN const _ELASTOS InterfaceID EIID_IDocumentHandler;
interface IDTDHandler;
EXTERN const _ELASTOS InterfaceID EIID_IDTDHandler;
interface IEntityResolver;
EXTERN const _ELASTOS InterfaceID EIID_IEntityResolver;
interface IErrorHandler;
EXTERN const _ELASTOS InterfaceID EIID_IErrorHandler;
interface IInputSource;
EXTERN const _ELASTOS InterfaceID EIID_IInputSource;
interface ILocator;
EXTERN const _ELASTOS InterfaceID EIID_ILocator;
interface IParser;
EXTERN const _ELASTOS InterfaceID EIID_IParser;
interface IXMLReader;
EXTERN const _ELASTOS InterfaceID EIID_IXMLReader;
interface IXMLFilter;
EXTERN const _ELASTOS InterfaceID EIID_IXMLFilter;
interface ICInputSourceClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICInputSourceClassObject;
interface ICHandlerBaseClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICHandlerBaseClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CInputSource;
EXTERN const _ELASTOS ClassID ECLSID_CInputSourceClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CHandlerBase;
EXTERN const _ELASTOS ClassID ECLSID_CHandlerBaseClassObject;
}
}
}

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {
interface IAttributeListImpl;
EXTERN const _ELASTOS InterfaceID EIID_IAttributeListImpl;
interface IAttributesImpl;
EXTERN const _ELASTOS InterfaceID EIID_IAttributesImpl;
interface ILocatorImpl;
EXTERN const _ELASTOS InterfaceID EIID_ILocatorImpl;
interface INamespaceSupport;
EXTERN const _ELASTOS InterfaceID EIID_INamespaceSupport;
interface IParserFactory;
EXTERN const _ELASTOS InterfaceID EIID_IParserFactory;
interface IXMLReaderFactory;
EXTERN const _ELASTOS InterfaceID EIID_IXMLReaderFactory;
interface ICAttributeListImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributeListImplClassObject;
interface ICAttributesImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributesImplClassObject;
interface ICLocatorImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocatorImplClassObject;
interface ICNamespaceSupportClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICNamespaceSupportClassObject;
interface ICParserFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICParserFactoryClassObject;
interface ICXMLReaderFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXMLReaderFactoryClassObject;
interface ICDefaultHandlerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultHandlerClassObject;
interface ICXMLReaderAdapterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXMLReaderAdapterClassObject;
interface ICXMLFilterImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXMLFilterImplClassObject;
interface ICParserAdapterClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICParserAdapterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributeListImpl;
EXTERN const _ELASTOS ClassID ECLSID_CAttributeListImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributesImpl;
EXTERN const _ELASTOS ClassID ECLSID_CAttributesImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocatorImpl;
EXTERN const _ELASTOS ClassID ECLSID_CLocatorImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CNamespaceSupport;
EXTERN const _ELASTOS ClassID ECLSID_CNamespaceSupportClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CParserFactory;
EXTERN const _ELASTOS ClassID ECLSID_CParserFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXMLReaderFactory;
EXTERN const _ELASTOS ClassID ECLSID_CXMLReaderFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHandler;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHandlerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXMLReaderAdapter;
EXTERN const _ELASTOS ClassID ECLSID_CXMLReaderAdapterClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXMLFilterImpl;
EXTERN const _ELASTOS ClassID ECLSID_CXMLFilterImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CParserAdapter;
EXTERN const _ELASTOS ClassID ECLSID_CParserAdapterClassObject;
}
}
}
}

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {
interface IAttributes2;
EXTERN const _ELASTOS InterfaceID EIID_IAttributes2;
interface IAttributes2Impl;
EXTERN const _ELASTOS InterfaceID EIID_IAttributes2Impl;
interface IDeclHandler;
EXTERN const _ELASTOS InterfaceID EIID_IDeclHandler;
interface IEntityResolver2;
EXTERN const _ELASTOS InterfaceID EIID_IEntityResolver2;
interface ILexicalHandler;
EXTERN const _ELASTOS InterfaceID EIID_ILexicalHandler;
interface ILocator2;
EXTERN const _ELASTOS InterfaceID EIID_ILocator2;
interface ILocator2Impl;
EXTERN const _ELASTOS InterfaceID EIID_ILocator2Impl;
interface ICLocator2ImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICLocator2ImplClassObject;
interface ICAttributes2ImplClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICAttributes2ImplClassObject;
interface ICDefaultHandler2ClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDefaultHandler2ClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CLocator2Impl;
EXTERN const _ELASTOS ClassID ECLSID_CLocator2ImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CAttributes2Impl;
EXTERN const _ELASTOS ClassID ECLSID_CAttributes2ImplClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHandler2;
EXTERN const _ELASTOS ClassID ECLSID_CDefaultHandler2ClassObject;
}
}
}
}

namespace Org {
namespace Xmlpull {
namespace V1 {
interface IXmlPullParser;
EXTERN const _ELASTOS InterfaceID EIID_IXmlPullParser;
interface IXmlPullParserFactory;
EXTERN const _ELASTOS InterfaceID EIID_IXmlPullParserFactory;
interface IXmlPullParserFactoryHelper;
EXTERN const _ELASTOS InterfaceID EIID_IXmlPullParserFactoryHelper;
interface IXmlSerializer;
EXTERN const _ELASTOS InterfaceID EIID_IXmlSerializer;
interface ICXmlPullParserFactoryClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXmlPullParserFactoryClassObject;
interface ICXmlPullParserFactoryHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICXmlPullParserFactoryHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXmlPullParserFactory;
EXTERN const _ELASTOS ClassID ECLSID_CXmlPullParserFactoryClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CXmlPullParserFactoryHelper;
EXTERN const _ELASTOS ClassID ECLSID_CXmlPullParserFactoryHelperClassObject;
}
}
}

namespace Org {
namespace Xmlpull {
namespace V1 {
namespace Sax2 {
interface ICDriverClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICDriverClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CDriver;
EXTERN const _ELASTOS ClassID ECLSID_CDriverClassObject;
}
}
}
}

namespace Org {
namespace Kxml2 {
namespace IO {
interface IKXmlParser;
EXTERN const _ELASTOS InterfaceID EIID_IKXmlParser;
interface ICKXmlParserClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKXmlParserClassObject;
interface ICKXmlSerializerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICKXmlSerializerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKXmlParser;
EXTERN const _ELASTOS ClassID ECLSID_CKXmlParserClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CKXmlSerializer;
EXTERN const _ELASTOS ClassID ECLSID_CKXmlSerializerClassObject;
}
}
}

namespace Org {
namespace Json {
interface IJSONArray;
EXTERN const _ELASTOS InterfaceID EIID_IJSONArray;
interface IJSONObject;
EXTERN const _ELASTOS InterfaceID EIID_IJSONObject;
interface IJSONObjectHelper;
EXTERN const _ELASTOS InterfaceID EIID_IJSONObjectHelper;
interface IJSONStringer;
EXTERN const _ELASTOS InterfaceID EIID_IJSONStringer;
interface IJSONTokener;
EXTERN const _ELASTOS InterfaceID EIID_IJSONTokener;
interface IJSONTokenerHelper;
EXTERN const _ELASTOS InterfaceID EIID_IJSONTokenerHelper;
interface ICJSONArrayClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONArrayClassObject;
interface ICJSONObjectClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONObjectClassObject;
interface ICJSONObjectHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONObjectHelperClassObject;
interface ICJSONStringerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONStringerClassObject;
interface ICJSONTokenerClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONTokenerClassObject;
interface ICJSONTokenerHelperClassObject;
EXTERN const _ELASTOS InterfaceID EIID_ICJSONTokenerHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONArray;
EXTERN const _ELASTOS ClassID ECLSID_CJSONArrayClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONObjectClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONObjectHelper;
EXTERN const _ELASTOS ClassID ECLSID_CJSONObjectHelperClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONStringer;
EXTERN const _ELASTOS ClassID ECLSID_CJSONStringerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONTokener;
EXTERN const _ELASTOS ClassID ECLSID_CJSONTokenerClassObject;
EXTERN const _ELASTOS ClassID ECLSID_CJSONTokenerHelper;
EXTERN const _ELASTOS ClassID ECLSID_CJSONTokenerHelperClassObject;
}
}




#ifndef E_OUT_OF_MEMORY_ERROR
#define E_OUT_OF_MEMORY_ERROR 0xa1010000
#endif

#ifndef E_ASSERTION_ERROR
#define E_ASSERTION_ERROR 0xa1020000
#endif

#ifndef E_INTERNAL_ERROR
#define E_INTERNAL_ERROR 0xa1030000
#endif

#ifndef E_ILLEGAL_ACCESS_ERROR
#define E_ILLEGAL_ACCESS_ERROR 0xa1040000
#endif

#ifndef E_VERIFY_ERROR
#define E_VERIFY_ERROR 0xa1050000
#endif

#ifndef E_UNKNOWN_ERROR
#define E_UNKNOWN_ERROR 0xa1060000
#endif

#ifndef E_ABSTRACET_METHOD_ERROR
#define E_ABSTRACET_METHOD_ERROR 0xa1070000
#endif

#ifndef E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION
#define E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION 0xa0010000
#endif

#ifndef E_ILLEGAL_ARGUMENT_EXCEPTION
#define E_ILLEGAL_ARGUMENT_EXCEPTION 0xa0020000
#endif

#ifndef E_INDEX_OUT_OF_BOUNDS_EXCEPTION
#define E_INDEX_OUT_OF_BOUNDS_EXCEPTION 0xa0030000
#endif

#ifndef E_NULL_POINTER_EXCEPTION
#define E_NULL_POINTER_EXCEPTION 0xa0040000
#endif

#ifndef E_RUNTIME_EXCEPTION
#define E_RUNTIME_EXCEPTION 0xa0050000
#endif

#ifndef E_ILLEGAL_STATE_EXCEPTION
#define E_ILLEGAL_STATE_EXCEPTION 0xa0060000
#endif

#ifndef E_ILLEGAL_THREAD_STATE_EXCEPTION
#define E_ILLEGAL_THREAD_STATE_EXCEPTION 0xa0070000
#endif

#ifndef E_UNSUPPORTED_OPERATION_EXCEPTION
#define E_UNSUPPORTED_OPERATION_EXCEPTION 0xa0080000
#endif

#ifndef E_SECURITY_EXCEPTION
#define E_SECURITY_EXCEPTION 0xa0090000
#endif

#ifndef E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION
#define E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION 0xa00a0000
#endif

#ifndef E_NO_SUCH_METHOD_EXCEPTION
#define E_NO_SUCH_METHOD_EXCEPTION 0xa00b0000
#endif

#ifndef E_ILLEGAL_MONITOR_STATE_EXCEPTION
#define E_ILLEGAL_MONITOR_STATE_EXCEPTION 0xa00c0000
#endif

#ifndef E_INTERRUPTED_EXCEPTION
#define E_INTERRUPTED_EXCEPTION 0xa00d0000
#endif

#ifndef E_ARITHMETIC_EXCEPTION
#define E_ARITHMETIC_EXCEPTION 0xa00e0000
#endif

#ifndef E_REMOTE_EXCEPTION
#define E_REMOTE_EXCEPTION 0x801c0000
#endif

#ifndef E_NOT_FOUND_EXCEPTION
#define E_NOT_FOUND_EXCEPTION 0xa0100000
#endif

#ifndef E_SUPER_NOT_CALLED_EXCEPTION
#define E_SUPER_NOT_CALLED_EXCEPTION 0xa0110000
#endif

#ifndef E_CLASS_NOT_FOUND_EXCEPTION
#define E_CLASS_NOT_FOUND_EXCEPTION 0xa0120000
#endif

#ifndef E_NEGATIVE_ARRAY_SIZE_EXCEPTION
#define E_NEGATIVE_ARRAY_SIZE_EXCEPTION 0xa0130000
#endif

#ifndef E_NUMBER_FORMAT_EXCEPTION
#define E_NUMBER_FORMAT_EXCEPTION 0xa0140000
#endif

#ifndef E_PATTERN_SYNTAX_EXCEPTION
#define E_PATTERN_SYNTAX_EXCEPTION 0xa0150000
#endif

#ifndef E_CLONE_NOT_SUPPORTED_EXCEPTION
#define E_CLONE_NOT_SUPPORTED_EXCEPTION 0xa0160000
#endif

#ifndef E_NAGATIVE_ARRAY_SIZE_EXCEPTION
#define E_NAGATIVE_ARRAY_SIZE_EXCEPTION 0xa0170000
#endif

#ifndef E_ILLEGAL_ACCESS_EXCEPTION
#define E_ILLEGAL_ACCESS_EXCEPTION 0xa0180000
#endif

#ifndef E_NO_SUCH_FIELD_EXCEPTION
#define E_NO_SUCH_FIELD_EXCEPTION 0xa0190000
#endif

#ifndef E_CLASS_CAST_EXCEPTION
#define E_CLASS_CAST_EXCEPTION 0xa0200000
#endif

#ifndef E_ARRAY_STORE_EXCEPTION
#define E_ARRAY_STORE_EXCEPTION 0xa0210000
#endif

#ifndef E_ACCESS_CONTROL_EXCEPTION
#define E_ACCESS_CONTROL_EXCEPTION 0xac100000
#endif

#ifndef E_DIGEST_EXCEPTION
#define E_DIGEST_EXCEPTION 0xac110000
#endif

#ifndef E_GENERAL_SECURITY_EXCEPTION
#define E_GENERAL_SECURITY_EXCEPTION 0xac120000
#endif

#ifndef E_INVALID_ALGORITHM_PARAMETER_EXCEPTION
#define E_INVALID_ALGORITHM_PARAMETER_EXCEPTION 0xac130000
#endif

#ifndef E_INVALID_KEY_EXCEPTION
#define E_INVALID_KEY_EXCEPTION 0xac140000
#endif

#ifndef E_INVALID_PARAMETER_EXCEPTION
#define E_INVALID_PARAMETER_EXCEPTION 0xac150000
#endif

#ifndef E_KEY_EXCEPTION
#define E_KEY_EXCEPTION 0xac160000
#endif

#ifndef E_KEY_MANAGEMENT_EXCEPTION
#define E_KEY_MANAGEMENT_EXCEPTION 0xac170000
#endif

#ifndef E_KEY_STORE_EXCEPTION
#define E_KEY_STORE_EXCEPTION 0xac180000
#endif

#ifndef E_NO_SUCH_ALGORITHM_EXCEPTION
#define E_NO_SUCH_ALGORITHM_EXCEPTION 0xac190000
#endif

#ifndef E_NO_SUCH_PROVIDER_EXCEPTION
#define E_NO_SUCH_PROVIDER_EXCEPTION 0xac1a0000
#endif

#ifndef E_PRIVILEGED_ACTION_EXCEPTION
#define E_PRIVILEGED_ACTION_EXCEPTION 0xac1b0000
#endif

#ifndef E_SIGNATURE_EXCEPTION
#define E_SIGNATURE_EXCEPTION 0xac1c0000
#endif

#ifndef E_UNRECOVERABLE_ENTRY_EXCEPTION
#define E_UNRECOVERABLE_ENTRY_EXCEPTION 0xac1d0000
#endif

#ifndef E_ACL_NOT_FOUND_EXCEPTION
#define E_ACL_NOT_FOUND_EXCEPTION 0xac010000
#endif

#ifndef E_LAST_OWNER_EXCEPTION
#define E_LAST_OWNER_EXCEPTION 0xac020000
#endif

#ifndef E_NOT_OWNER_EXCEPTION
#define E_NOT_OWNER_EXCEPTION 0xac030000
#endif

#ifndef E_CERTIFICATE_ENCODING_EXCEPTION
#define E_CERTIFICATE_ENCODING_EXCEPTION 0xac040000
#endif

#ifndef E_CERTIFICATE_EXCEPTION
#define E_CERTIFICATE_EXCEPTION 0xac050000
#endif

#ifndef E_CERTIFICATE_EXPIRED_EXCEPTION
#define E_CERTIFICATE_EXPIRED_EXCEPTION 0xac060000
#endif

#ifndef E_CERTIFICATE_NOT_YET_VALID_EXCEPTION
#define E_CERTIFICATE_NOT_YET_VALID_EXCEPTION 0xac070000
#endif

#ifndef E_CERTIFICATE_PARSING_EXCEPTION
#define E_CERTIFICATE_PARSING_EXCEPTION 0xac080000
#endif

#ifndef E_CERT_PATH_BUILDER_EXCEPTION
#define E_CERT_PATH_BUILDER_EXCEPTION 0xac090000
#endif

#ifndef E_CERT_PATH_VALIDATOR_EXCEPTION
#define E_CERT_PATH_VALIDATOR_EXCEPTION 0xac0a0000
#endif

#ifndef E_CERT_STORE_EXCEPTION
#define E_CERT_STORE_EXCEPTION 0xac0b0000
#endif

#ifndef E_CRL_EXCEPTION
#define E_CRL_EXCEPTION 0xac0c0000
#endif

#ifndef E_NOT_SERIALIZABLE_EXCEPTION
#define E_NOT_SERIALIZABLE_EXCEPTION 0xac0d0000
#endif

#ifndef E_INVALID_KEY_SPEC_EXCEPTION
#define E_INVALID_KEY_SPEC_EXCEPTION 0xac0e0000
#endif

#ifndef E_INVALID_PARAMETER_SPEC_EXCEPTION
#define E_INVALID_PARAMETER_SPEC_EXCEPTION 0xac0f0000
#endif

#ifndef E_IO_ERROR
#define E_IO_ERROR 0xa3010000
#endif

#ifndef E_CODER_MALFUNCTION_ERROR
#define E_CODER_MALFUNCTION_ERROR 0xa3020000
#endif

#ifndef E_IO_EXCEPTION
#define E_IO_EXCEPTION 0xa2010000
#endif

#ifndef E_EOF_EXCEPTION
#define E_EOF_EXCEPTION 0xa2020000
#endif

#ifndef E_FILE_NOT_FOUND_EXCEPTION
#define E_FILE_NOT_FOUND_EXCEPTION 0xa2030000
#endif

#ifndef E_SYNC_FAILED_EXCEPTION
#define E_SYNC_FAILED_EXCEPTION 0xa2040000
#endif

#ifndef E_ALREADY_CONNECTED_EXCEPTION
#define E_ALREADY_CONNECTED_EXCEPTION 0xa2050000
#endif

#ifndef E_ASYNCHRONOUS_CLOSE_EXCEPTION
#define E_ASYNCHRONOUS_CLOSE_EXCEPTION 0xa2060000
#endif

#ifndef E_CANCELLED_KEY_EXCEPTION
#define E_CANCELLED_KEY_EXCEPTION 0xa2070000
#endif

#ifndef E_CLOSED_BY_INTERRUPT_EXCEPTION
#define E_CLOSED_BY_INTERRUPT_EXCEPTION 0xa2080000
#endif

#ifndef E_CLOSED_CHANNEL_EXCEPTION
#define E_CLOSED_CHANNEL_EXCEPTION 0xa2090000
#endif

#ifndef E_CLOSED_SELECTOR_EXCEPTION
#define E_CLOSED_SELECTOR_EXCEPTION 0xa20a0000
#endif

#ifndef E_CONNECTION_PENDING_EXCEPTION
#define E_CONNECTION_PENDING_EXCEPTION 0xa20b0000
#endif

#ifndef E_FILE_LOCK_INTERRUPTION_EXCEPTION
#define E_FILE_LOCK_INTERRUPTION_EXCEPTION 0xa20c0000
#endif

#ifndef E_ILLEGAL_BLOCKING_MODE_EXCEPTION
#define E_ILLEGAL_BLOCKING_MODE_EXCEPTION 0xa20d0000
#endif

#ifndef E_ILLEGAL_SELECTOR_EXCEPTION
#define E_ILLEGAL_SELECTOR_EXCEPTION 0xa20e0000
#endif

#ifndef E_NO_CONNECTION_PENDING_EXCEPTION
#define E_NO_CONNECTION_PENDING_EXCEPTION 0xa2100000
#endif

#ifndef E_NON_READABLE_CHANNEL_EXCEPTION
#define E_NON_READABLE_CHANNEL_EXCEPTION 0xa2110000
#endif

#ifndef E_NON_WRITABLE_CHANNEL_EXCEPTION
#define E_NON_WRITABLE_CHANNEL_EXCEPTION 0xa2120000
#endif

#ifndef E_NOT_YET_BOUND_EXCEPTION
#define E_NOT_YET_BOUND_EXCEPTION 0xa2130000
#endif

#ifndef E_NOT_YET_CONNECTED_EXCEPTION
#define E_NOT_YET_CONNECTED_EXCEPTION 0xa2140000
#endif

#ifndef E_OVER_LAPPING_FILE_LOCK_EXCEPTION
#define E_OVER_LAPPING_FILE_LOCK_EXCEPTION 0xa2150000
#endif

#ifndef E_UNRESOLVED_ADDRESS_EXCEPTION
#define E_UNRESOLVED_ADDRESS_EXCEPTION 0xa2160000
#endif

#ifndef E_UNSUPPORTED_ADDRESS_TYPE_EXCEPTION
#define E_UNSUPPORTED_ADDRESS_TYPE_EXCEPTION 0xa2170000
#endif

#ifndef E_BUFFER_UNDER_FLOW_EXCEPTION
#define E_BUFFER_UNDER_FLOW_EXCEPTION 0xa2180000
#endif

#ifndef E_BUFFER_OVER_FLOW_EXCEPTION
#define E_BUFFER_OVER_FLOW_EXCEPTION 0xa2190000
#endif

#ifndef E_INVALID_MARK_EXCEPTION
#define E_INVALID_MARK_EXCEPTION 0xa21a0000
#endif

#ifndef E_READ_ONLY_BUFFER_EXCEPTION
#define E_READ_ONLY_BUFFER_EXCEPTION 0xa21b0000
#endif

#ifndef E_UNSUPPORTED_ENCODING_EXCEPTION
#define E_UNSUPPORTED_ENCODING_EXCEPTION 0xa21c0000
#endif

#ifndef E_INVALID_OBJECT_EXCEPTION
#define E_INVALID_OBJECT_EXCEPTION 0xa21d0000
#endif

#ifndef E_ILLEGAL_CHARSET_NAME_EXCEPTION
#define E_ILLEGAL_CHARSET_NAME_EXCEPTION 0xa21e0000
#endif

#ifndef E_UNSUPPORTED_CHARSET_EXCEPTION
#define E_UNSUPPORTED_CHARSET_EXCEPTION 0xa21f0000
#endif

#ifndef E_CHARACTER_CODING_EXCEPTION
#define E_CHARACTER_CODING_EXCEPTION 0xa2200000
#endif

#ifndef E_UNMAPPABLE_CHARACTER_EXCEPTION
#define E_UNMAPPABLE_CHARACTER_EXCEPTION 0xa2210000
#endif

#ifndef E_MALFORMED_INPUT_EXCEPTION
#define E_MALFORMED_INPUT_EXCEPTION 0xa2220000
#endif

#ifndef E_UTF_DATA_FORMAT_EXCEPTION
#define E_UTF_DATA_FORMAT_EXCEPTION 0xa2230000
#endif

#ifndef E_BUFFER_UNDERFLOW_EXCEPTION
#define E_BUFFER_UNDERFLOW_EXCEPTION 0xa2240000
#endif

#ifndef E_BUFFER_OVERFLOW_EXCEPTION
#define E_BUFFER_OVERFLOW_EXCEPTION 0xa2250000
#endif

#ifndef E_OVERLAPPING_FILE_LOCK_EXCEPTION
#define E_OVERLAPPING_FILE_LOCK_EXCEPTION 0xa2260000
#endif

#ifndef E_INTERRUPTED_IO_EXCEPTION
#define E_INTERRUPTED_IO_EXCEPTION 0xa2270000
#endif

#ifndef E_BIND_EXCEPTION
#define E_BIND_EXCEPTION 0xab010000
#endif

#ifndef E_CONNECT_EXCEPTION
#define E_CONNECT_EXCEPTION 0xab020000
#endif

#ifndef E_HTTP_RETRY_EXCEPTION
#define E_HTTP_RETRY_EXCEPTION 0xab030000
#endif

#ifndef E_MALFORMED_URL_EXCEPTION
#define E_MALFORMED_URL_EXCEPTION 0xab040000
#endif

#ifndef E_NO_ROUTE_TO_HOST_EXCEPTION
#define E_NO_ROUTE_TO_HOST_EXCEPTION 0xab050000
#endif

#ifndef E_PORT_UNREACHABLE_EXCEPTION
#define E_PORT_UNREACHABLE_EXCEPTION 0xab060000
#endif

#ifndef E_PROTOCOL_EXCEPTION
#define E_PROTOCOL_EXCEPTION 0xab070000
#endif

#ifndef E_SOCKET_EXCEPTION
#define E_SOCKET_EXCEPTION 0xab080000
#endif

#ifndef E_SOCKET_TIMEOUT_EXCEPTION
#define E_SOCKET_TIMEOUT_EXCEPTION 0xab090000
#endif

#ifndef E_UNKNOWN_SERVICE_EXCEPTION
#define E_UNKNOWN_SERVICE_EXCEPTION 0xab0b0000
#endif

#ifndef E_URI_SYNTAX_EXCEPTION
#define E_URI_SYNTAX_EXCEPTION 0xab0c0000
#endif

#ifndef E_SOCKET_CLOSE_EXCEPTION
#define E_SOCKET_CLOSE_EXCEPTION 0xab0d0000
#endif

#ifndef E_MALFORMED_REPLY_EXCEPTION
#define E_MALFORMED_REPLY_EXCEPTION 0xab0e0000
#endif

#ifndef E_INVALID_SOCKET_EXCEPTION
#define E_INVALID_SOCKET_EXCEPTION 0xab0f0000
#endif

#ifndef E_UNKNOWN_HOST_EXCEPTION
#define E_UNKNOWN_HOST_EXCEPTION 0xab100000
#endif

#ifndef E_PARSE_EXCEPTION
#define E_PARSE_EXCEPTION 0xad010000
#endif

#ifndef ISimpleDateFormat_PATTERN_CHARS
#define ISimpleDateFormat_PATTERN_CHARS "GyMdkHmsSEDFwWahKzZLc"
#endif

#ifndef E_CONCURRENT_MODIFICATION_EXCEPTION
#define E_CONCURRENT_MODIFICATION_EXCEPTION 0xa7000000
#endif

#ifndef E_FORMATTER_CLOSED_EXCEPTION
#define E_FORMATTER_CLOSED_EXCEPTION 0xa7000001
#endif

#ifndef E_MISSING_FORMAT_ARGUMENT_EXCEPTION
#define E_MISSING_FORMAT_ARGUMENT_EXCEPTION 0xa7000002
#endif

#ifndef E_ILLEGAL_FORMAT_CONVERSION_EXCEPTION
#define E_ILLEGAL_FORMAT_CONVERSION_EXCEPTION 0xa7000003
#endif

#ifndef E_ILLEGAL_FORMAT_FLAGS_EXCEPTION
#define E_ILLEGAL_FORMAT_FLAGS_EXCEPTION 0xa7000004
#endif

#ifndef E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION
#define E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION 0xa7000005
#endif

#ifndef E_FORMAT_FLAGS_CONVERSION_MISMATCH_EXCEPTION
#define E_FORMAT_FLAGS_CONVERSION_MISMATCH_EXCEPTION 0xa7000006
#endif

#ifndef E_MISSING_FORMAT_WIDTH_EXCEPTION
#define E_MISSING_FORMAT_WIDTH_EXCEPTION 0xa7000007
#endif

#ifndef E_ILLEGAL_FORMAT_PRECISION_EXCEPTION
#define E_ILLEGAL_FORMAT_PRECISION_EXCEPTION 0xa7000008
#endif

#ifndef E_ILLEGAL_FORMAT_WIDTH_EXCEPTION
#define E_ILLEGAL_FORMAT_WIDTH_EXCEPTION 0xa7000009
#endif

#ifndef E_EMPTY_STACK_EXCEPTION
#define E_EMPTY_STACK_EXCEPTION 0xa700000a
#endif

#ifndef E_INPUT_MISMATCH_EXCEPTION
#define E_INPUT_MISMATCH_EXCEPTION 0xa700000b
#endif

#ifndef E_MISSING_RESOURCE_EXCEPTION
#define E_MISSING_RESOURCE_EXCEPTION 0xa700000c
#endif

#ifndef E_BROKEN_BARRIER_EXCEPTION
#define E_BROKEN_BARRIER_EXCEPTION 0xa700000d
#endif

#ifndef E_ILLFORMED_LOCALE_EXCEPTION
#define E_ILLFORMED_LOCALE_EXCEPTION 0xa700000e
#endif

#ifndef E_BACKING_STORE_EXCEPTION
#define E_BACKING_STORE_EXCEPTION 0xa700000f
#endif

#ifndef E_CHECKED_ARITHMETIC_EXCEPTION
#define E_CHECKED_ARITHMETIC_EXCEPTION 0xa7000010
#endif

#ifndef E_INVALID_PREFERENCES_FORMAT_EXCEPTION
#define E_INVALID_PREFERENCES_FORMAT_EXCEPTION 0xa7000011
#endif

#ifndef E_JAR_EXCEPTION
#define E_JAR_EXCEPTION 0xa7000012
#endif

#ifndef E_CANCELLATION_EXCEPTION
#define E_CANCELLATION_EXCEPTION 0xa6010000
#endif

#ifndef E_EXECUTION_EXCEPTION
#define E_EXECUTION_EXCEPTION 0xa6020000
#endif

#ifndef E_TIMEOUT_EXCEPTION
#define E_TIMEOUT_EXCEPTION 0xa6030000
#endif

#ifndef E_REJECTED_EXECUTION_EXCEPTION
#define E_REJECTED_EXECUTION_EXCEPTION 0xa6040000
#endif

#ifndef E_ZIP_ERROR
#define E_ZIP_ERROR 0xa5010000
#endif

#ifndef E_DATA_FORMAT_EXCEPTION
#define E_DATA_FORMAT_EXCEPTION 0xa4010000
#endif

#ifndef E_ZIP_EXCEPTION
#define E_ZIP_EXCEPTION 0xa4020000
#endif

#ifndef E_SQL_EXCEPTION
#define E_SQL_EXCEPTION 0xa8010000
#endif

#ifndef E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION
#define E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION 0xa8010001
#endif

#ifndef E_SQL_CLINET_INFO_EXCEPTION
#define E_SQL_CLINET_INFO_EXCEPTION 0xa8010002
#endif

#ifndef E_SQL_DATA_EXCEPTION
#define E_SQL_DATA_EXCEPTION 0xa8010003
#endif

#ifndef E_SQL_INTEGRITY_CONSTRAINT_VIOLATION_EXCEPTION
#define E_SQL_INTEGRITY_CONSTRAINT_VIOLATION_EXCEPTION 0xa8010004
#endif

#ifndef E_SQL_INVALID_AUTHORIZATION_SPEC_EXCEPTION
#define E_SQL_INVALID_AUTHORIZATION_SPEC_EXCEPTION 0xa8010005
#endif

#ifndef E_SQL_NON_TRANSIENT_CONNECTION_EXCEPTION
#define E_SQL_NON_TRANSIENT_CONNECTION_EXCEPTION 0xa8010006
#endif

#ifndef E_SQL_NON_TRANSIENT_EXCEPTION
#define E_SQL_NON_TRANSIENT_EXCEPTION 0xa8010007
#endif

#ifndef E_SQL_RECOVERABLE_EXCEPTION
#define E_SQL_RECOVERABLE_EXCEPTION 0xa8010008
#endif

#ifndef E_SQL_SYNTAX_ERROR_EXCEPTION
#define E_SQL_SYNTAX_ERROR_EXCEPTION 0xa8010009
#endif

#ifndef E_SQL_TIMEOUT_EXCEPTION
#define E_SQL_TIMEOUT_EXCEPTION 0xa801000a
#endif

#ifndef E_SQL_TRANSACTION_ROLLBACK_EXCEPTION
#define E_SQL_TRANSACTION_ROLLBACK_EXCEPTION 0xa801000b
#endif

#ifndef E_SQL_TRANSIENT_CONNECTION_EXCEPTION
#define E_SQL_TRANSIENT_CONNECTION_EXCEPTION 0xa801000c
#endif

#ifndef E_SQL_TRANSIENT_EXCEPTION
#define E_SQL_TRANSIENT_EXCEPTION 0xa801000d
#endif

#ifndef E_BATCH_UPDATE_EXCEPTION
#define E_BATCH_UPDATE_EXCEPTION 0xa801000e
#endif

#ifndef E_SQL_SQLITE_JDBC_EXCEPTION
#define E_SQL_SQLITE_JDBC_EXCEPTION 0xa801000f
#endif

#ifndef E_SQL_SQLITE_JDBC_BATCHUPDATE_EXCEPTION
#define E_SQL_SQLITE_JDBC_BATCHUPDATE_EXCEPTION 0xa8010010
#endif

#ifndef E_SQL_SQLITE_THROWEX_EXCEPTION
#define E_SQL_SQLITE_THROWEX_EXCEPTION 0xa8010011
#endif

#ifndef E_NO_SUCH_ELEMENT_EXCEPTION
#define E_NO_SUCH_ELEMENT_EXCEPTION 0xab000000
#endif

#ifndef E_ERRNO_EXCEPTION
#define E_ERRNO_EXCEPTION 0xaa010000
#endif

#ifndef E_GAI_EXCEPTION
#define E_GAI_EXCEPTION 0xaa020000
#endif

#ifndef E_AEAD_BAD_TAG_EXCEPTION
#define E_AEAD_BAD_TAG_EXCEPTION 0xb0010000
#endif

#ifndef E_BAD_PADDING_EXCEPTION
#define E_BAD_PADDING_EXCEPTION 0xb0020000
#endif

#ifndef E_EXEMPTION_MECHANISM_EXCEPTION
#define E_EXEMPTION_MECHANISM_EXCEPTION 0xb0030000
#endif

#ifndef E_ILLEGAL_BLOCK_SIZE_EXCEPTION
#define E_ILLEGAL_BLOCK_SIZE_EXCEPTION 0xb0040000
#endif

#ifndef E_NO_SUCH_PADDING_EXCEPTION
#define E_NO_SUCH_PADDING_EXCEPTION 0xb0050000
#endif

#ifndef E_SHORT_BUFFER_EXCEPTION
#define E_SHORT_BUFFER_EXCEPTION 0xb0060000
#endif

#ifndef E_DATATYPE_CONFIGURATION_EXCEPTION
#define E_DATATYPE_CONFIGURATION_EXCEPTION 0xb0070000
#endif

#ifndef E_FACTORY_CONFIGURATION_EXCEPTION
#define E_FACTORY_CONFIGURATION_EXCEPTION 0xb0080000
#endif

#ifndef E_INVALID_TRANSFORMATION
#define E_INVALID_TRANSFORMATION 0xb0090000
#endif

#ifndef E_PROVIDER_EXCEPTION
#define E_PROVIDER_EXCEPTION 0xb00a0000
#endif

#ifndef E_XPATH_EXCEPTION
#define E_XPATH_EXCEPTION 0xb0070000
#endif

#ifndef E_XPATH_EXPRESSION_EXCEPTION
#define E_XPATH_EXPRESSION_EXCEPTION 0xb0080000
#endif

#ifndef E_SSL_PEER_UNVERIFIED_EXCEPTION
#define E_SSL_PEER_UNVERIFIED_EXCEPTION 0xb1010000
#endif

#ifndef E_SSL_EXCEPTION
#define E_SSL_EXCEPTION 0xb1020000
#endif

#ifndef E_SSL_PROTOCOL_EXCEPTION
#define E_SSL_PROTOCOL_EXCEPTION 0xb1030000
#endif

#ifndef E_SSL_KEY_EXCEPTION
#define E_SSL_KEY_EXCEPTION 0xb1040000
#endif

#ifndef E_SSL_HANDSHAKE_EXCEPTION
#define E_SSL_HANDSHAKE_EXCEPTION 0xb1050000
#endif

#ifndef E_UNSUPPORTED_CALLBACK_EXCEPTION
#define E_UNSUPPORTED_CALLBACK_EXCEPTION 0xad010000
#endif

#ifndef E_LOGIN_EXCEPTION
#define E_LOGIN_EXCEPTION 0xad020000
#endif

#ifndef E_DESTROY_FAILED_EXCEPTION
#define E_DESTROY_FAILED_EXCEPTION 0xad030000
#endif

#ifndef E_ASN1_EXCEPTION
#define E_ASN1_EXCEPTION 0xae010000
#endif

#ifndef E_CONNECTION_CLOSED_EXCEPTION
#define E_CONNECTION_CLOSED_EXCEPTION 0xb1010000
#endif

#ifndef E_HTTP_EXCEPTION
#define E_HTTP_EXCEPTION 0xb1020000
#endif

#ifndef E_MALFORMED_CHUNK_CODING_EXCEPTION
#define E_MALFORMED_CHUNK_CODING_EXCEPTION 0xb1030000
#endif

#ifndef E_METHOD_NOT_SUPPORTED_EXCEPTION
#define E_METHOD_NOT_SUPPORTED_EXCEPTION 0xb1040000
#endif

#ifndef E_NO_HTTP_RESPONSE_EXCEPTION
#define E_NO_HTTP_RESPONSE_EXCEPTION 0xb1050000
#endif

#ifndef E_UNSUPPORTED_HTTP_VERSION_EXCEPTION
#define E_UNSUPPORTED_HTTP_VERSION_EXCEPTION 0xb1060000
#endif

#ifndef E_AUTHENTICATION_EXCEPTION
#define E_AUTHENTICATION_EXCEPTION 0xb1070000
#endif

#ifndef E_INVALID_CREDENTIALS_EXCEPTION
#define E_INVALID_CREDENTIALS_EXCEPTION 0xb1080000
#endif

#ifndef E_MALFORMED_CHALLENGE_EXCEPTION
#define E_MALFORMED_CHALLENGE_EXCEPTION 0xb1090000
#endif

#ifndef E_CLIENT_PROTOCOL_EXCEPTION
#define E_CLIENT_PROTOCOL_EXCEPTION 0xb10a0000
#endif

#ifndef E_CIRCULAR_REDIRECT_EXCEPTION
#define E_CIRCULAR_REDIRECT_EXCEPTION 0xb10b0000
#endif

#ifndef E_HTTP_RESPONSE_EXCEPTION
#define E_HTTP_RESPONSE_EXCEPTION 0xb10c0000
#endif

#ifndef E_NON_REPEATABLE_REQUEST_EXCEPTION
#define E_NON_REPEATABLE_REQUEST_EXCEPTION 0xb10d0000
#endif

#ifndef E_CONNECTION_POOL_TIMEOUT_EXCEPTION
#define E_CONNECTION_POOL_TIMEOUT_EXCEPTION 0xb10e0000
#endif

#ifndef E_CONNECT_TIMEOUT_EXCEPTION
#define E_CONNECT_TIMEOUT_EXCEPTION 0xb10f0000
#endif

#ifndef E_HTTP_HOST_CONNECT_EXCEPTION
#define E_HTTP_HOST_CONNECT_EXCEPTION 0xb2010000
#endif

#ifndef E_MALFORMED_COOKIE_EXCEPTION
#define E_MALFORMED_COOKIE_EXCEPTION 0xb2020000
#endif

#ifndef E_NTLM_ENGINE_EXCEPTION
#define E_NTLM_ENGINE_EXCEPTION 0xb2030000
#endif

#ifndef E_UNSUPPORTED_DIGEST_ALGORITHM_EXCEPTION
#define E_UNSUPPORTED_DIGEST_ALGORITHM_EXCEPTION 0xb2040000
#endif

#ifndef E_TUNNEL_REFUSED_EXCEPTION
#define E_TUNNEL_REFUSED_EXCEPTION 0xb2050000
#endif

#ifndef E_DATE_PARSE_EXCEPTION
#define E_DATE_PARSE_EXCEPTION 0xb2060000
#endif

#ifndef E_REDIRECT_EXCEPTION
#define E_REDIRECT_EXCEPTION 0xb2070000
#endif

#ifndef E_SAX_EMPTYSTACK_EXCEPTION
#define E_SAX_EMPTYSTACK_EXCEPTION 0xc1010000
#endif

#ifndef E_SAX_NOT_RECOGNIZED_EXCEPTION
#define E_SAX_NOT_RECOGNIZED_EXCEPTION 0xc1010001
#endif

#ifndef E_SAX_EXCEPTION
#define E_SAX_EXCEPTION 0xc1010002
#endif

#ifndef E_SAX_PARSE_EXCEPTION
#define E_SAX_PARSE_EXCEPTION 0xc1010003
#endif

#ifndef E_SAX_NOT_SUPPORTED_EXCEPTION
#define E_SAX_NOT_SUPPORTED_EXCEPTION 0xc1010004
#endif

#ifndef E_XML_PULL_PARSER_EXCEPTION
#define E_XML_PULL_PARSER_EXCEPTION 0xc0010000
#endif

#ifndef E_JSON_EXCEPTION
#define E_JSON_EXCEPTION 0xc2010000
#endif


#ifndef __ENUM_Elastos_Core_ThreadState__
#define __ENUM_Elastos_Core_ThreadState__
namespace Elastos {
namespace Core {
enum {
    ThreadState_NEW = 0,
    ThreadState_RUNNABLE = 1,
    ThreadState_BLOCKED = 2,
    ThreadState_WAITING = 3,
    ThreadState_TIMED_WAITING = 4,
    ThreadState_TERMINATED = 5,
};
typedef _ELASTOS Int32 ThreadState;
}
}

#endif //__ENUM_Elastos_Core_ThreadState__


#ifndef __ENUM_Elastos_Math_RoundingMode__
#define __ENUM_Elastos_Math_RoundingMode__
namespace Elastos {
namespace Math {
enum {
    RoundingMode_UP = 0,
    RoundingMode_DOWN = 1,
    RoundingMode_CEILING = 2,
    RoundingMode_FLOOR = 3,
    RoundingMode_HALF_UP = 4,
    RoundingMode_HALF_DOWN = 5,
    RoundingMode_HALF_EVEN = 6,
    RoundingMode_UNNECESSARY = 7,
};
typedef _ELASTOS Int32 RoundingMode;
}
}

#endif //__ENUM_Elastos_Math_RoundingMode__


#ifndef __ENUM_Elastos_Security_Cert_CRLReason__
#define __ENUM_Elastos_Security_Cert_CRLReason__
namespace Elastos {
namespace Security {
namespace Cert {
enum {
    CRLReason_NULL = 0,
    CRLReason_UNSPECIFIED = 1,
    CRLReason_KEY_COMPROMISE = 2,
    CRLReason_CA_COMPROMISE = 3,
    CRLReason_AFFILIATION_CHANGED = 4,
    CRLReason_SUPERSEDED = 5,
    CRLReason_CESSATION_OF_OPERATION = 6,
    CRLReason_CERTIFICATE_HOLD = 7,
    CRLReason_UNUSED = 8,
    CRLReason_REMOVE_FROM_CRL = 9,
    CRLReason_PRIVILEGE_WITHDRAWN = 10,
    CRLReason_AA_COMPROMISE = 11,
};
typedef _ELASTOS Int32 CRLReason;
}
}
}

#endif //__ENUM_Elastos_Security_Cert_CRLReason__


#ifndef __ENUM_Elastos_Security_KeyRepType__
#define __ENUM_Elastos_Security_KeyRepType__
namespace Elastos {
namespace Security {
enum {
    KeyRepType_SECRET = 0,
    KeyRepType_PUBLIC = 1,
    KeyRepType_PRIVATE = 2,
};
typedef _ELASTOS Int32 KeyRepType;
}
}

#endif //__ENUM_Elastos_Security_KeyRepType__


#ifndef __ENUM_Elastos_IO_ByteOrder__
#define __ENUM_Elastos_IO_ByteOrder__
namespace Elastos {
namespace IO {
enum {
    ByteOrder_BIG_ENDIAN = 0,
    ByteOrder_LITTLE_ENDIAN = 1,
};
typedef _ELASTOS Int32 ByteOrder;
}
}

#endif //__ENUM_Elastos_IO_ByteOrder__


#ifndef __ENUM_Elastos_IO_Channels_FileChannelMapMode__
#define __ENUM_Elastos_IO_Channels_FileChannelMapMode__
namespace Elastos {
namespace IO {
namespace Channels {
enum {
    FileChannelMapMode_NONE = 0,
    FileChannelMapMode_READ_ONLY = 1,
    FileChannelMapMode_READ_WRITE = 2,
    FileChannelMapMode_PRIVATE = 3,
};
typedef _ELASTOS Int32 FileChannelMapMode;
}
}
}

#endif //__ENUM_Elastos_IO_Channels_FileChannelMapMode__


#ifndef __ENUM_AuthenticatorRequestorType__
#define __ENUM_AuthenticatorRequestorType__
enum {
    AuthenticatorRequestorType_PROXY = 0,
    AuthenticatorRequestorType_SERVER = 1,
};
typedef _ELASTOS Int32 AuthenticatorRequestorType;

#endif //__ENUM_AuthenticatorRequestorType__


#ifndef __ENUM_Elastos_Net_CookiePolicyKind__
#define __ENUM_Elastos_Net_CookiePolicyKind__
namespace Elastos {
namespace Net {
enum {
    CookiePolicyKind_ACCEPT_ALL = 0,
    CookiePolicyKind_ACCEPT_NONE = 1,
    CookiePolicyKind_ACCEPT_ORIGINAL_SERVER = 2,
};
typedef _ELASTOS Int32 CookiePolicyKind;
}
}

#endif //__ENUM_Elastos_Net_CookiePolicyKind__


#ifndef __ENUM_Elastos_Net_ResponseSource__
#define __ENUM_Elastos_Net_ResponseSource__
namespace Elastos {
namespace Net {
enum {
    ResponseSource_CACHE = 0,
    ResponseSource_CONDITIONAL_CACHE = 1,
    ResponseSource_NETWORK = 2,
};
typedef _ELASTOS Int32 ResponseSource;
}
}

#endif //__ENUM_Elastos_Net_ResponseSource__


#ifndef __ENUM_Elastos_Net_ProxyType__
#define __ENUM_Elastos_Net_ProxyType__
namespace Elastos {
namespace Net {
enum {
    ProxyType_DIRECT = 0,
    ProxyType_HTTP = 1,
    ProxyType_SOCKS = 2,
};
typedef _ELASTOS Int32 ProxyType;
}
}

#endif //__ENUM_Elastos_Net_ProxyType__


#ifndef __ENUM_Elastos_Text_NormalizerForm__
#define __ENUM_Elastos_Text_NormalizerForm__
namespace Elastos {
namespace Text {
enum {
    NormalizerForm_NFD = 0,
    NormalizerForm_NFC = 1,
    NormalizerForm_NFKD = 2,
    NormalizerForm_NFKC = 3,
};
typedef _ELASTOS Int32 NormalizerForm;
}
}

#endif //__ENUM_Elastos_Text_NormalizerForm__


#ifndef __ENUM_Elastos_Sql_RowIdLifeTime__
#define __ENUM_Elastos_Sql_RowIdLifeTime__
namespace Elastos {
namespace Sql {
enum {
    RowIdLifeTime_UNSUPPORTED = 0,
    RowIdLifeTime_VALID_OTHER = 1,
    RowIdLifeTime_VALID_SESSION = 2,
    RowIdLifeTime_VALID_TRANSACTION = 3,
    RowIdLifeTime_VALID_FOREVER = 4,
};
typedef _ELASTOS Int32 RowIdLifeTime;
}
}

#endif //__ENUM_Elastos_Sql_RowIdLifeTime__


#ifndef __ENUM_Libcore_ICU_DateFormat__
#define __ENUM_Libcore_ICU_DateFormat__
namespace Libcore {
namespace ICU {
enum {
    DateFormat_FULL = 0,
    DateFormat_LONG = 1,
    DateFormat_MEDIUM = 2,
    DateFormat_SHORT = 3,
};
typedef _ELASTOS Int32 DateFormat;
}
}

#endif //__ENUM_Libcore_ICU_DateFormat__


#ifndef __ENUM_Libcore_ICU_Form__
#define __ENUM_Libcore_ICU_Form__
namespace Libcore {
namespace ICU {
enum {
    Form_NFD = 0,
    Form_NFC = 1,
    Form_NFKD = 2,
    Form_NFKC = 3,
};
typedef _ELASTOS Int32 Form;
}
}

#endif //__ENUM_Libcore_ICU_Form__


#ifndef __ENUM_Elastosx_Crypto_NeedToSet__
#define __ENUM_Elastosx_Crypto_NeedToSet__
namespace Elastosx {
namespace Crypto {
enum {
    NeedToSet_NONE = 0,
    NeedToSet_MODE = 1,
    NeedToSet_PADDING = 2,
    NeedToSet_BOTH = 3,
};
typedef _ELASTOS Int32 NeedToSet;
}
}

#endif //__ENUM_Elastosx_Crypto_NeedToSet__


#ifndef __ENUM_Elastosx_Net_Ssl_SSLEngineResult_Status__
#define __ENUM_Elastosx_Net_Ssl_SSLEngineResult_Status__
namespace Elastosx {
namespace Net {
namespace Ssl {
enum {
    BUFFER_OVERFLOW = 0,
    BUFFER_UNDERFLOW = 1,
    CLOSED = 2,
    OK = 3,
};
typedef _ELASTOS Int32 SSLEngineResult_Status;
}
}
}

#endif //__ENUM_Elastosx_Net_Ssl_SSLEngineResult_Status__


#ifndef __ENUM_Elastosx_Net_Ssl_SSLEngineResult_HandshakeStatus__
#define __ENUM_Elastosx_Net_Ssl_SSLEngineResult_HandshakeStatus__
namespace Elastosx {
namespace Net {
namespace Ssl {
enum {
    NOT_HANDSHAKING = 0,
    FINISHED = 1,
    NEED_TASK = 2,
    NEED_WRAP = 3,
    NEED_UNWRAP = 4,
};
typedef _ELASTOS Int32 SSLEngineResult_HandshakeStatus;
}
}
}

#endif //__ENUM_Elastosx_Net_Ssl_SSLEngineResult_HandshakeStatus__


#ifndef __ENUM_Org_Apache_Http_Conn_Routing_TunnelType__
#define __ENUM_Org_Apache_Http_Conn_Routing_TunnelType__
namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {
enum {
    TunnelType_PLAIN = 0,
    TunnelType_TUNNELLED = 1,
};
typedef _ELASTOS Int32 TunnelType;
}
}
}
}
}

#endif //__ENUM_Org_Apache_Http_Conn_Routing_TunnelType__


#ifndef __ENUM_Org_Apache_Http_Conn_Routing_LayerType__
#define __ENUM_Org_Apache_Http_Conn_Routing_LayerType__
namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {
enum {
    LayerType_PLAIN = 0,
    LayerType_LAYERED = 1,
};
typedef _ELASTOS Int32 LayerType;
}
}
}
}
}

#endif //__ENUM_Org_Apache_Http_Conn_Routing_LayerType__


#ifndef __ENUM_Org_Json_JSONStringerScope__
#define __ENUM_Org_Json_JSONStringerScope__
namespace Org {
namespace Json {
enum {
    JSONStringerScope_EMPTY_ARRAY = 0,
    JSONStringerScope_NONEMPTY_ARRAY = 1,
    JSONStringerScope_EMPTY_OBJECT = 2,
    JSONStringerScope_DANGLING_KEY = 3,
    JSONStringerScope_NONEMPTY_OBJECT = 4,
    JSONStringerScope_NULL = 5,
};
typedef _ELASTOS Int32 JSONStringerScope;
}
}

#endif //__ENUM_Org_Json_JSONStringerScope__

typedef _ELASTOS PVoid LocalPtr;

#ifdef __ELASTOS_CORELIBRARY_USER_TYPE_H__
#include "Elastos.CoreLibrary_user_type.h"
#endif

#endif // __CAR_ELASTOS_CORELIBRARY_H__
