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

#include "elastos/droid/server/net/DelayedDiskWrite.h"
#include <elastos/core/AutoLock.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

//=====================================================================
// DelayedDiskWrite::InnerSub_Runnable
//=====================================================================
DelayedDiskWrite::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ DelayedDiskWrite* host,
    /* [in] */ const String& filePath,
    /* [in] */ IDelayedDiskWriteWriter* w)
    : mHost(host)
    , mFilePath(filePath)
    , mWriter(w)
{}

ECode DelayedDiskWrite::InnerSub_Runnable::Run()
{
    return mHost->DoWrite(mFilePath, mWriter);
}

//=====================================================================
// DelayedDiskWrite
//=====================================================================
DelayedDiskWrite::DelayedDiskWrite()
    : mWriteSequence(0)
    , TAG("DelayedDiskWrite")
{}

ECode DelayedDiskWrite::Write(
    /* [in] */ const String& filePath,
    /* [in] */ IDelayedDiskWriteWriter* w)
{
    if (TextUtils::IsEmpty(filePath)) {
        Logger::E(TAG, "empty file path");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    /* Do a delayed write to disk on a separate handler thread */
    {
        AutoLock syncLock(this);
        if (++mWriteSequence == 1) {
            CHandlerThread::New(String("DelayedDiskWriteThread"), (IHandlerThread**)&mDiskWriteHandlerThread);
            IThread::Probe(mDiskWriteHandlerThread)->Start();
            AutoPtr<ILooper> looper;
            mDiskWriteHandlerThread->GetLooper((ILooper**)&looper);
            CHandler::New(looper, (IHandler**)&mDiskWriteHandler);
        }
    }

    AutoPtr<IRunnable> runnable = new InnerSub_Runnable(this, filePath, w);
    Boolean b;
    mDiskWriteHandler->Post(runnable, &b);
    return NOERROR;
}

ECode DelayedDiskWrite::DoWrite(
    /* [in] */ const String& filePath,
    /* [in] */ IDelayedDiskWriteWriter* w)
{
    AutoPtr<IDataOutputStream> out;
    ECode ec;
    do {
        AutoPtr<IFileOutputStream> fos;
        if (FAILED(ec = CFileOutputStream::New(filePath, (IFileOutputStream**)&fos))) break;
        AutoPtr<IBufferedOutputStream> bos;
        if (FAILED(ec = CBufferedOutputStream::New(IOutputStream::Probe(fos), (IBufferedOutputStream**)&bos))) break;
        if (FAILED(ec = CDataOutputStream::New(IOutputStream::Probe(bos), (IDataOutputStream**)&out))) break;
        if (FAILED(ec = w->OnWriteCalled(out))) break;
    } while(FALSE);
    if (FAILED(ec)) {
        if ((ECode)E_IO_EXCEPTION == ec)
            Logger::E(TAG, "Error writing data file %s", filePath.string());
        else
            return ec;
    }
    // finally {
    if (out != NULL) {
        // try {
        IOutputStream::Probe(out)->Close();
        // } catch (Exception e) {}
    }
    // }
    // Quit if no more writes sent
    {
        AutoLock syncLock(this);
        if (--mWriteSequence == 0) {
            AutoPtr<ILooper> looper;
            mDiskWriteHandler->GetLooper((ILooper**)&looper);
            looper->Quit();
            mDiskWriteHandler = NULL;
            mDiskWriteHandlerThread = NULL;
        }
    }
    return NOERROR;
}

ECode DelayedDiskWrite::Loge(
    /* [in] */ const String& s)
{
    Logger::E(TAG, s);
    return NOERROR;
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos
