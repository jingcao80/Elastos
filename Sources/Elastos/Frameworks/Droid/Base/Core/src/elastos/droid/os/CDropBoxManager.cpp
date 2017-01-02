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

#include "elastos/droid/os/CDropBoxManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

const String CDropBoxManager::TAG("DropBoxManager");

CAR_INTERFACE_IMPL(CDropBoxManager, Object, IDropBoxManager)

CAR_OBJECT_IMPL(CDropBoxManager)

ECode CDropBoxManager::constructor(
    /* [in] */ IDropBoxManagerService* service)
{
    mService = service;
    return NOERROR;
}

ECode CDropBoxManager::constructor()
{
    mService = NULL;
    return NOERROR;
}

ECode CDropBoxManager::AddText(
    /* [in] */ const String& tag,
    /* [in] */ const String& data)
{
    //try {
    AutoPtr<IDropBoxManagerEntry> entry;
    CDropBoxManagerEntry::New(tag, 0, data, (IDropBoxManagerEntry**)&entry);
    mService->Add(entry);
    //} catch (RemoteException e) {}
    return NOERROR;
}

ECode CDropBoxManager::AddData(
    /* [in] */ const String& tag,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 flags)
{
    if (data == NULL) {
        Slogger::E(TAG, "data == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        // mService->Add(new Entry(tag, 0, data, flags));
    AutoPtr<IDropBoxManagerEntry> entry;
    CDropBoxManagerEntry::New(tag, 0, data, flags, (IDropBoxManagerEntry**)&entry);
    mService->Add(entry);
     // } catch (RemoteException e) {}
    return NOERROR;
}

ECode CDropBoxManager::AddFile(
    /* [in] */ const String& tag,
    /* [in] */ IFile* file,
    /* [in] */ Int32 flags)
{
    if (file == NULL)  {
        Slogger::E(TAG, "file == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // Entry entry = new Entry(tag, 0, file, flags);
    AutoPtr<IDropBoxManagerEntry> entry;
    CDropBoxManagerEntry::New(tag, 0, file, flags, (IDropBoxManagerEntry**)&entry);
    // try {
        mService->Add(entry);
    // } catch (RemoteException e) {
        // ignore
    // } finally {
        if (entry != NULL) {
            ICloseable::Probe(entry)->Close();
        }
    // }
    return NOERROR;
}

ECode CDropBoxManager::IsTagEnabled(
    /* [in] */ const String& tag,
    /* [out] */ Boolean* isTagEnabled)
{
    // try {
    VALIDATE_NOT_NULL(isTagEnabled);
    return mService->IsTagEnabled(tag, isTagEnabled);
     // } catch (RemoteException e) { return false; }
}

ECode CDropBoxManager::GetNextEntry(
    /* [in] */ const String& tag,
    /* [in] */ Int64 msec,
    /* [out] */ IDropBoxManagerEntry** nextEntry)
{
    // try {
    return mService->GetNextEntry(tag, msec, nextEntry);
    // } catch (RemoteException e) { return null; }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
