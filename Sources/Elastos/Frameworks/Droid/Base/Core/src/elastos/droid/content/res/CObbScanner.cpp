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
#include "elastos/droid/content/res/CObbScanner.h"
#include "elastos/droid/content/res/CObbInfo.h"
#include <androidfw/ObbFile.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Slogger;

static const String TAG("CObbScanner");

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CObbScanner, Singleton, IObbScanner)
CAR_SINGLETON_IMPL(CObbScanner)

ECode CObbScanner::GetObbInfo(
    /* [in] */ const String& filePath,
    /* [out] */ IObbInfo** obbInfo)
{
    VALIDATE_NOT_NULL(obbInfo)
    *obbInfo = NULL;

    if (filePath.IsNull()) {
        Slogger::E(TAG, "file path cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IFile> obbFile;
    CFile::New(filePath, (IFile**)&obbFile);
    Boolean isExist = FALSE;
    obbFile->Exists(&isExist);
    if (!isExist) {
        Slogger::E(TAG, "OBB file does not exist: %s", filePath.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    /*
     * XXX This will fail to find the real canonical path if bind mounts are
     * used, but we don't use any bind mounts right now.
     */
    String canonicalFilePath;
    obbFile->GetCanonicalPath(&canonicalFilePath);
    CObbInfo::New(obbInfo);
    (*obbInfo)->SetFilename(canonicalFilePath);
    return NativeGetObbInfo(canonicalFilePath, *obbInfo);
}

ECode CObbScanner::NativeGetObbInfo(
    /* [in] */ const String& filePath,
    /* [in, out] */ IObbInfo* obbInfo)
{
    assert(obbInfo != NULL);

    android::sp<android::ObbFile> obb = new android::ObbFile();
    if (!obb->readFrom(filePath.string())) {
        Slogger::E(TAG, "Could not read OBB file");
        return E_IO_EXCEPTION;
    }

    String packageNameStr(obb->getPackageName().string());

    if (packageNameStr.IsNull()) {
        Slogger::E(TAG, "Could not read OBB file");
        return E_IO_EXCEPTION;
    }

    obbInfo->SetPackageName(packageNameStr);
    obbInfo->SetVersion(obb->getVersion());
    obbInfo->SetFlags(obb->getFlags());

    size_t saltLen;
    const unsigned char* salt = obb->getSalt(&saltLen);
    if (saltLen > 0) {
        AutoPtr<ArrayOf<Byte> > saltArray = ArrayOf<Byte>::Alloc(saltLen);
        saltArray->Copy((const Byte*)salt, saltLen);
        obbInfo->SetSalt(saltArray);
    }

    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

