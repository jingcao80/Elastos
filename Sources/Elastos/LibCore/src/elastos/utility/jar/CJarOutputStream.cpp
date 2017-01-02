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

#include "CJarOutputStream.h"
#include "CZipEntry.h"

using Elastos::IO::EIID_IOutputStream;
using Elastos::Utility::Zip::CZipEntry;

namespace Elastos {
namespace Utility {
namespace Jar {

CAR_INTERFACE_IMPL(CJarOutputStream, ZipOutputStream, IJarOutputStream)

CAR_OBJECT_IMPL(CJarOutputStream)

ECode CJarOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ IManifest* manifest)
{
    FAIL_RETURN(ZipOutputStream::constructor(os))
    if (manifest == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mManifest = manifest;
    AutoPtr<IZipEntry> ze;
    CZipEntry::New(IJarFile::MANIFEST_NAME, (IZipEntry**)&ze);
    FAIL_RETURN(PutNextEntry(ze))
    FAIL_RETURN(mManifest->Write(this))
    CloseEntry();
    return NOERROR;
}

ECode CJarOutputStream::constructor(
    /* [in] */ IOutputStream* os)
{
    return ZipOutputStream::constructor(os);
}


} // namespace Jar
} // namespace Utility
} // namespace Elastos

