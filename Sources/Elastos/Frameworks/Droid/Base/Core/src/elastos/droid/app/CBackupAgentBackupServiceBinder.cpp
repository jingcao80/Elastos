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

#include "CBackupAgentBackupServiceBinder.h"
#include "elastos/droid/app/backup/CBackupDataInput.h"
#include "elastos/droid/app/backup/CBackupDataOutput.h"
#include "elastos/droid/app/backup/CFullBackupDataOutput.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFileOutputStream;
using Elastos::Droid::App::Backup::CBackupDataInput;
using Elastos::Droid::App::Backup::CBackupDataOutput;
using Elastos::Droid::App::Backup::CFullBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IFullBackupDataOutput;
using Elastos::Droid::Os::Binder;

namespace Elastos {
namespace Droid {
namespace App {

const String CBackupAgentBackupServiceBinder::TAG("BackupServiceBinder");
const Boolean CBackupAgentBackupServiceBinder::DEBUG = FALSE;

ECode CBackupAgentBackupServiceBinder::constructor(
    /* [in] */ Handle32 host)
{
    mHost = (BackupAgent*)host;
    return NOERROR;
}

ECode CBackupAgentBackupServiceBinder::DoBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // Ensure that we're running with the app's normal permission level
    Int64 oldId = Binder::ClearCallingIdentity();

    if (DEBUG) Logger::D(TAG, "DoBackup() invoked");

    AutoPtr<IFileDescriptor> fd;
    data->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IBackupDataOutput> output;
    CBackupDataOutput::New(fd, (IBackupDataOutput**)&output);

    // try {
    ECode ec = mHost->OnBackup(oldState, output, newState);
    if (FAILED(ec)) {
        Logger::W(TAG, "onBackup threw:%x", ec);
    }
    // } catch (IOException ex) {
    //     Log.d(TAG, "onBackup (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw new RuntimeException(ex);
    // } catch (RuntimeException ex) {
    //     Log.d(TAG, "onBackup (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw ex;
    // } finally {
    Binder::RestoreCallingIdentity(oldId);
        // try {
    callbackBinder->OpComplete(token);
        // } catch (RemoteException e) {
        //     // we'll time out anyway, so we're safe
        // }
    // }
    return ec;
}

ECode CBackupAgentBackupServiceBinder::DoRestore(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // Ensure that we're running with the app's normal permission level
    Int64 oldId = Binder::ClearCallingIdentity();

    if (DEBUG) Logger::D(TAG, "DoRestore() invoked");

    AutoPtr<IFileDescriptor> fd;
    data->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IBackupDataInput> input;
    CBackupDataInput::New(fd, (IBackupDataInput**)&input);
    // try {
    ECode ec = mHost->OnRestore(input, appVersionCode, newState);
    if (FAILED(ec)) {
        Logger::W(TAG, "onRestore threw:%x", ec);
    }
    // } catch (IOException ex) {
    //     Log.d(TAG, "onRestore (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw new RuntimeException(ex);
    // } catch (RuntimeException ex) {
    //     Log.d(TAG, "onRestore (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw ex;
    // } finally {
    Binder::RestoreCallingIdentity(oldId);
        // try {
    callbackBinder->OpComplete(token);
        // } catch (RemoteException e) {
            // we'll time out anyway, so we're safe
        // }
    // }
    return ec;
}

ECode CBackupAgentBackupServiceBinder::DoFullBackup(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // Ensure that we're running with the app's normal permission level
    Int64 oldId = Binder::ClearCallingIdentity();

    if (DEBUG) Logger::D(TAG, "DoFullBackup() invoked");

    // try {
    AutoPtr<IFullBackupDataOutput> fulloutput;
    CFullBackupDataOutput::New(data, (IFullBackupDataOutput**)&fulloutput);
    ECode ec = mHost->OnFullBackup(fulloutput);
    if (FAILED(ec)) {
        Logger::W(TAG, "OnFullBackup threw:%x", ec);
    }
    // } catch (IOException ex) {
    //     Log.d(TAG, "onBackup (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw new RuntimeException(ex);
    // } catch (RuntimeException ex) {
    //     Log.d(TAG, "onBackup (" + BackupAgent.this.getClass().getName() + ") threw", ex);
    //     throw ex;
    // } finally {
    // Send the EOD marker indicating that there is no more data
    // forthcoming from this agent.
        // try {
    AutoPtr<IFileDescriptor> fd;
    data->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileOutputStream> out;
    CFileOutputStream::New(fd, (IFileOutputStream**)&out);
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(4);
    out->WriteBytes(*buf);
        // } catch (IOException e) {
        //     Log.e(TAG, "Unable to finalize backup stream!");
        // }

    Binder::RestoreCallingIdentity(oldId);
        // try {
    callbackBinder->OpComplete(token);
        // } catch (RemoteException e) {
            // we'll time out anyway, so we're safe
        // }
    // }
    return ec;
}

ECode CBackupAgentBackupServiceBinder::DoRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ const String& domainName,
    /* [in] */ const String& path,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    Int64 oldId = Binder::ClearCallingIdentity();

    // try {
    ECode ec = mHost->OnRestoreFile(data, size, type, domainName, path, mode, mtime);
    // } catch (IOException e) {
    //     throw new RuntimeException(e);
    // } finally {
    Binder::RestoreCallingIdentity(oldId);
        // try {
    callbackBinder->OpComplete(token);
        // } catch (RemoteException e) {
            // we'll time out anyway, so we're safe
        // }
    // }
    return ec;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
