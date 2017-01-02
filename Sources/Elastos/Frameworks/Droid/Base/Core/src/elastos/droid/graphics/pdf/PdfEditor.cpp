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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/pdf/PdfEditor.h"
#include <elastos/droid/system/OsConstants.h>
#include <elastos/core/Mutex.h>
#include <elastos/utility/logging/Logger.h>
#include <fpdfview.h>
#include <fpdfedit.h>
#include <fpdfsave.h>
#include <utils/Log.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::Mutex;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {

CAR_INTERFACE_IMPL(PdfEditor, Object, IPdfEditor);
PdfEditor::PdfEditor()
    : mNativeDocument(0)
    , mPageCount(0)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
}

ECode PdfEditor::constructor(
    /* [in] */ /*@NonNull*/ IParcelFileDescriptor* input) /*throws IOException*/
{
    if (input == NULL) {
        // throw new NullPointerException("input cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 size = 0;
    // try {
    AutoPtr<IFileDescriptor> fd;
    input->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<ILibcore> lc;
    CLibcore::AcquireSingleton((ILibcore**)&lc);
    AutoPtr<IOs> os;
    lc->GetOs((IOs**)&os);
    if (FAILED((os->Lseek(fd, 0, OsConstants::_SEEK_SET, &size)))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IStructStat> ss;
    if (FAILED((os->Fstat(fd, (IStructStat**)&ss)))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ss->GetSize(&size);
    // } catch (ErrnoException ee) {
    //     throw new IllegalArgumentException("file descriptor not seekable");
    // }

    mInput = input;
    Int32 iFd = 0;
    FAIL_RETURN(NativeOpen(mInput->GetFd(&iFd), size, &mNativeDocument));
    mPageCount = NativeGetPageCount(mNativeDocument);
    return mCloseGuard->Open(String("PdfEditor::Close"));
}

PdfEditor::~PdfEditor()
{
    mCloseGuard->WarnIfOpen();
    if (mInput != NULL) {
        DoClose();
    }
}

ECode PdfEditor::GetPageCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    FAIL_RETURN(ThrowIfClosed());
    *count = mPageCount;
    return NOERROR;
}

ECode PdfEditor::RemovePage(
    /* [in] */ Int32 pageIndex)
{
    FAIL_RETURN(ThrowIfClosed());
    FAIL_RETURN(ThrowIfPageNotInDocument(pageIndex));
    mPageCount = NativeRemovePage(mNativeDocument, pageIndex);
    return NOERROR;
}

ECode PdfEditor::Write(
    /* [in] */ IParcelFileDescriptor* output) /*throws IOException*/
{
    Int32 fd = 0;
    ECode ec = ThrowIfClosed();
    FAIL_GOTO(ec, error);
    ec = NativeWrite(mNativeDocument, (output->GetFd(&fd), fd));
    FAIL_GOTO(ec, error);

error:
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    return iu->CloseQuietly(ICloseable::Probe(output));
}

ECode PdfEditor::Close()
{
    FAIL_RETURN(ThrowIfClosed());
    DoClose();
    return NOERROR;
}

void PdfEditor::DoClose()
{
    NativeClose(mNativeDocument);
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(ICloseable::Probe(mInput));
    mInput = NULL;
    mCloseGuard->Close();
}

ECode PdfEditor::ThrowIfClosed()
{
    if (mInput == NULL) {
        // throw new IllegalStateException("Already closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode PdfEditor::ThrowIfPageNotInDocument(
    /* [in] */ Int32 pageIndex)
{
    if (pageIndex < 0 || pageIndex >= mPageCount) {
        // throw new IllegalArgumentException("Invalid page index");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

////////////////////////////////////// Jni ////////////////////
static Mutex sLock;
static int sUnmatchedInitRequestCount = 0;
static void InitializeLibraryIfNeeded()
{
    Mutex::AutoLock _l(sLock);
    if (sUnmatchedInitRequestCount == 0) {
        FPDF_InitLibrary(NULL);
    }
    sUnmatchedInitRequestCount++;
}

static void DestroyLibraryIfNeeded()
{
    Mutex::AutoLock _l(sLock);
    sUnmatchedInitRequestCount--;
    if (sUnmatchedInitRequestCount == 0) {
       FPDF_DestroyLibrary();
    }
}

static int getBlock(
    /* [in] */ void* param,
    /* [in] */ unsigned long position,
    /* [in] */ unsigned char* outBuffer,
    /* [in] */ unsigned long size)
{
    const int fd = reinterpret_cast<intptr_t>(param);
    const int readCount = pread(fd, outBuffer, size, position);
    if (readCount < 0) {
        ALOGE("Cannot read from file descriptor. Error:%d", errno);
        return 0;
    }
    return 1;
}

ECode PdfEditor::NativeOpen(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 size,
    /* [out] */ Int64* resVal)
{
    VALIDATE_NOT_NULL(resVal);
    InitializeLibraryIfNeeded();

    FPDF_FILEACCESS loader;
    loader.m_FileLen = size;
    loader.m_Param = reinterpret_cast<void*>(intptr_t(fd));
    loader.m_GetBlock = &getBlock;

    FPDF_DOCUMENT document = FPDF_LoadCustomDocument(&loader, NULL);

    if (!document) {
        // const long error = FPDF_GetLastError();
        // jniThrowException(env, "java/io/IOException",
        //         "cannot create document. Error:" + error);
        DestroyLibraryIfNeeded();
        *resVal = -1;
        return E_IO_EXCEPTION;
    }

    *resVal = reinterpret_cast<Int64>(document);
    return NOERROR;
}

void PdfEditor::NativeClose(
    /* [in] */ Int64 documentPtr)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    FPDF_CloseDocument(document);
    DestroyLibraryIfNeeded();
}

Int32 PdfEditor::NativeGetPageCount(
    /* [in] */ Int64 documentPtr)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    return FPDF_GetPageCount(document);
}

Int32 PdfEditor::NativeRemovePage(
    /* [in] */ Int64 documentPtr,
    /* [in] */ Int32 pageIndex)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    FPDFPage_Delete(document, pageIndex);
    return FPDF_GetPageCount(document);
}

struct PdfToFdWriter : FPDF_FILEWRITE {
    int dstFd;
};

static bool writeAllBytes(
    /* [in] */ const int fd,
    /* [in] */ const void* buffer,
    /* [in] */ const size_t byteCount)
{
    char* writeBuffer = static_cast<char*>(const_cast<void*>(buffer));
    size_t remainingBytes = byteCount;
    while (remainingBytes > 0) {
        ssize_t writtenByteCount = write(fd, writeBuffer, remainingBytes);
        if (writtenByteCount == -1) {
            if (errno == EINTR) {
                continue;
            }
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                    "Error writing to buffer: %d", errno);
            return false;
        }
        remainingBytes -= writtenByteCount;
        writeBuffer += writtenByteCount;
    }
    return true;
}

static int writeBlock(
    /* [in] */ FPDF_FILEWRITE* owner,
    /* [in] */ const void* buffer,
    /* [in] */ unsigned long size)
{
    const PdfToFdWriter* writer = reinterpret_cast<PdfToFdWriter*>(owner);
    const bool success = writeAllBytes(writer->dstFd, buffer, size);
    if (success < 0) {
        Logger::E(String("PdfEditor"), String("Cannot write to file descriptor. Error:%d"), errno);
        return 0;
    }
    return 1;
}

ECode PdfEditor::NativeWrite(
    /* [in] */ Int64 documentPtr,
    /* [in] */ Int32 fd)
{
    FPDF_DOCUMENT document = reinterpret_cast<FPDF_DOCUMENT>(documentPtr);
    PdfToFdWriter writer;
    writer.dstFd = fd;
    writer.WriteBlock = &writeBlock;
    const bool success = FPDF_SaveAsCopy(document, &writer, FPDF_NO_INCREMENTAL);
    if (!success) {
        // jniThrowException(env, "java/io/IOException",
        //         "cannot write to fd. Error:" + errno);
        DestroyLibraryIfNeeded();
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
