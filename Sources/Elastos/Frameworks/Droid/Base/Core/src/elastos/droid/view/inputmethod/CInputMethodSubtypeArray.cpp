
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/view/inputmethod/CInputMethodSubtypeArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Zip::IGZIPOutputStream;
using Elastos::Utility::Zip::CGZIPOutputStream;
using Elastos::Utility::Zip::IGZIPInputStream;
using Elastos::Utility::Zip::CGZIPInputStream;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CInputMethodSubtypeArray::
//========================================================================================
const String CInputMethodSubtypeArray::TAG("InputMethodSubtypeArray");

CAR_INTERFACE_IMPL_2(CInputMethodSubtypeArray, Object, IInputMethodSubtypeArray, IParcelable)

CAR_OBJECT_IMPL(CInputMethodSubtypeArray)

ECode CInputMethodSubtypeArray::constructor()
{
    return NOERROR;
}

ECode CInputMethodSubtypeArray::constructor(
    /* [in] */ IList* subtypes)
{
    if (subtypes == NULL) {
        mCount = 0;
        return NOERROR;
    }
    subtypes->GetSize(&mCount);
    AutoPtr<ArrayOf<IInputMethodSubtype*> > p = ArrayOf<IInputMethodSubtype*>::Alloc(mCount);
    subtypes->ToArray((ArrayOf<IInterface*>*)p.Get(), (ArrayOf<IInterface*>**)&mInstance);
    return NOERROR;
}

ECode CInputMethodSubtypeArray::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCount);
    if (mCount > 0) {
        source->ReadInt32(&mDecompressedSize);
        source->ReadArrayOf((Handle32*)&mCompressedData);
    }
    return NOERROR;
}

ECode CInputMethodSubtypeArray::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mCount == 0) {
        dest->WriteInt32(mCount);
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > compressedData = mCompressedData;
    Int32 decompressedSize = mDecompressedSize;
    if (compressedData == NULL && decompressedSize == 0) {
        synchronized(mLockObject) {
            compressedData = mCompressedData;
            decompressedSize = mDecompressedSize;
            if (compressedData == NULL && decompressedSize == 0) {
                AutoPtr<ArrayOf<Byte> > decompressedData = Marshall(mInstance);
                compressedData = Compress(decompressedData);
                if (compressedData == NULL) {
                    decompressedSize = -1;
                    Slogger::I(TAG, "Failed to compress data.");
                }
                else {
                    decompressedSize = decompressedData->GetLength();
                }
                mDecompressedSize = decompressedSize;
                mCompressedData = compressedData;
            }
        }
    }

    if (compressedData != NULL && decompressedSize > 0) {
        dest->WriteInt32(mCount);
        dest->WriteInt32(decompressedSize);
        dest->WriteArrayOf((Handle32)compressedData.Get());
    }
    else {
        Slogger::I(TAG, "Unexpected state. Behaving as an empty array.");
        dest->WriteInt32(0);
    }
    return NOERROR;
}

ECode CInputMethodSubtypeArray::Get(
    /* [in] */ Int32 index,
    /* [out] */ IInputMethodSubtype** result)
{
    VALIDATE_NOT_NULL(result)

    if (index < 0 || mCount <= index) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<ArrayOf<IInputMethodSubtype*> > instance = mInstance;
    if (instance == NULL) {
        synchronized(mLockObject) {
            instance = mInstance;
            if (instance == NULL) {
                AutoPtr<ArrayOf<Byte> > decompressedData =
                      Decompress(mCompressedData, mDecompressedSize);
                // Clear the compressed data until {@link #getMarshalled()} is called.
                mCompressedData = NULL;
                mDecompressedSize = 0;
                if (decompressedData != NULL) {
                    instance = Unmarshall(decompressedData);
                }
                else {
                    Slogger::E(TAG, "Failed to decompress data. Returns null as fallback.");
                    instance = ArrayOf<IInputMethodSubtype*>::Alloc(mCount);
                }
                mInstance = instance;
            }
        }
    }
    *result = (*instance)[index];
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CInputMethodSubtypeArray::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCount;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CInputMethodSubtypeArray::Marshall(
    /* [in] */ ArrayOf<IInputMethodSubtype*>* array)
{
    AutoPtr<IParcel> parcel;

    CParcel::New((IParcel**)&parcel);
    parcel->WriteArrayOf((Handle32)array);
    AutoPtr<ArrayOf<Byte> > arr;
    parcel->Marshall((ArrayOf<Byte>**)&arr);

    return arr;
}

AutoPtr<ArrayOf<IInputMethodSubtype*> > CInputMethodSubtypeArray::Unmarshall(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IParcel> parcel;

    CParcel::New((IParcel**)&parcel);
    parcel->Unmarshall(data, 0, data->GetLength());
    parcel->SetDataPosition(0);
    AutoPtr<ArrayOf<IInputMethodSubtype*> > arr;
    parcel->ReadArrayOf((Handle32*)&arr);

    return arr;
}

AutoPtr<ArrayOf<Byte> > CInputMethodSubtypeArray::Compress(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IByteArrayOutputStream> resultStream;
    AutoPtr<IGZIPOutputStream> zipper;
    // try {
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&resultStream);
    CGZIPOutputStream::New(IOutputStream::Probe(resultStream), (IGZIPOutputStream**)&zipper);
    IOutputStream::Probe(zipper)->Write(data);
    // } catch(IOException e) {
    //     return NULL;
    // } finally {
    // try {
    if (zipper != NULL) {
        IOutputStream::Probe(zipper)->Close();
    }
    // } catch (IOException e) {
    //     zipper = NULL;
    //     // Slog.e(TAG, "Failed to close the stream.", e);
    //     // swallowed, not propagated back to the caller
    // }
    // try {
    if (resultStream != NULL) {
        IOutputStream::Probe(resultStream)->Close();
    }
    // } catch (IOException e) {
    //     resultStream = NULL;
    //     // Slog.e(TAG, "Failed to close the stream.", e);
    //     // swallowed, not propagated back to the caller
    // }
    // }
    AutoPtr<ArrayOf<Byte> > arr;
    if (resultStream != NULL) {
        resultStream->ToByteArray((ArrayOf<Byte>**)&arr);
    }
    return arr;
}

AutoPtr<ArrayOf<Byte> > CInputMethodSubtypeArray::Decompress(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 expectedSize)
{
    AutoPtr<IByteArrayInputStream> inputStream;
    AutoPtr<IGZIPInputStream> unzipper;

    CByteArrayInputStream::New(data, (IByteArrayInputStream**)&inputStream);
    CGZIPInputStream::New(IInputStream::Probe(inputStream), (IGZIPInputStream**)&unzipper);
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(expectedSize);
    Int32 totalReadBytes = 0;
    while (totalReadBytes < result->GetLength()) {
        Int32 restBytes = result->GetLength() - totalReadBytes;
        Int32 readBytes = 0;
        IInputStream::Probe(unzipper)->Read(result, totalReadBytes, restBytes, &readBytes);
        if (readBytes < 0) {
            break;
        }
        totalReadBytes += readBytes;
    }
    if (expectedSize != totalReadBytes) {
        if (unzipper != NULL) {
            IInputStream::Probe(unzipper)->Close();
        }
        if (inputStream != NULL) {
            IInputStream::Probe(inputStream)->Close();
        }
        return NULL;
    }
    if (unzipper != NULL) {
        IInputStream::Probe(unzipper)->Close();
    }
    if (inputStream != NULL) {
        IInputStream::Probe(inputStream)->Close();
    }
    return result;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
