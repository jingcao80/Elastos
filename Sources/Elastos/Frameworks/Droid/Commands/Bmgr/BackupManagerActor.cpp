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

#include "BackupManagerActor.h"
#include "elastos/droid/os/ServiceManager.h"

using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Bmgr {

BackupManagerActor::BackupManagerActor()
{

}

BackupManagerActor::~BackupManagerActor()
{

}

void BackupManagerActor::Init()
{
    AutoPtr<IInterface> backupManager = ServiceManager::GetService(String("backup"));
    mBackupManager = IIBackupManager::Probe(backupManager);
    if (mBackupManager == NULL) {
        printf("BackupManagerActor::Init mBackupManager is NULL\n");
        assert(0);
    }
}

void BackupManagerActor::DoEnabled()
{
    Boolean isEnabled;
    mBackupManager->IsBackupEnabled(&isEnabled);
    printf("BackupManager currently : %d\n", isEnabled);
}

void BackupManagerActor::DoEnable(Boolean isEnable)
{
    mBackupManager->SetBackupEnabled(isEnable);
}

void BackupManagerActor::DoBackup(const String &pkg)
{
    printf("BackupManagerActor::DoBackup-->pkgName(%s)\n",pkg.string() );
    mBackupManager->DataChanged(pkg);
}

void BackupManagerActor::DoTransport(const String &transportName)
{
    String oldport("temp");
    mBackupManager->SelectBackupTransport(transportName, &oldport);
    printf("BackupManagerActor::DoTransport-->(%s) formerly %s\n",transportName.string(), oldport.string() );
}

void BackupManagerActor::DoListTransport()
{
    mBackupManager->ListAllTransports((ArrayOf<String>**)&mTransports);
    printf("listTransports:\n");
    for(Int32 i = 0; i < mTransports->GetLength(); ++i) {
        printf("\t %s\n", (*mTransports)[i].string());
    }
}

void BackupManagerActor::DoRun()
{
    mBackupManager->BackupNow();
    printf("BackupManagerActor::Run ok\n");
}

void BackupManagerActor::DoRestoreAll()
{
}

void BackupManagerActor::DoRestorePackage(String &pkgName)
{
    printf("BackupManagerActor::DoRestorePackage(%s)\n", pkgName.string());
    //AutoPtr<RestoreObserver> observer = new RestoreObserver();
    AutoPtr<IIRestoreObserver> observer;
    CRestoreObserver::New((IIRestoreObserver**)&observer);
    ECode ec = mBackupManager->BeginRestoreSession(pkgName, String(NULL), (IIRestoreSession**)&mRestoreSession);
    if (FAILED(ec)) {
        printf("BackupManagerActor::DoRestorePackage error\n");
    }
    if(mRestoreSession == NULL) {
        printf("BackupManagerActor::DoRestorePackage:: mRestoreSession is NULL\n");
    }
    Int32 value = 0;
     AutoPtr<IObject> obj = IObject::Probe(mRestoreSession);
    //printf("Hominlinx-->BackupManagerActor::mRestoreSession[%p %p %p] %p %p\n",mRestoreSession.Get(), obj.Get(), (IInterface*)(IIRestoreSession*)mRestoreSession.Get(), observer.Get(), (IInterface*)(IIRestoreObserver*)observer.Get());
    ec = mRestoreSession->RestorePackage(pkgName, observer, &value);
    //mRestoreSession->EndRestoreSession();
    AutoPtr<IWait> wait = IWait::Probe(observer);
    if (value == 0) {
        wait->WaitForCompletion();
    }
    else {
        printf("BackupManageDemo::DoRestorePackage:: unable to retore package:%s", pkgName.string());
    }

    mRestoreSession->EndRestoreSession();
}

}
}
}
}
