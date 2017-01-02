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

#include "CFileDescriptorHelper.h"
#include "CFileDescriptor.h"

namespace Elastos {
namespace IO {

CAR_SINGLETON_IMPL(CFileDescriptorHelper)

CAR_INTERFACE_IMPL(CFileDescriptorHelper, Singleton, IFileDescriptorHelper)

ECode CFileDescriptorHelper::GetInFile(
    /* [out] */ IFileDescriptor** inFile)
{
    VALIDATE_NOT_NULL(inFile);

    *inFile = CFileDescriptor::IN;
    REFCOUNT_ADD(*inFile);
    return NOERROR;
}

ECode CFileDescriptorHelper::GetOutFile(
    /* [out] */ IFileDescriptor** outFile)
{
    VALIDATE_NOT_NULL(outFile);

    *outFile = CFileDescriptor::OUT;
    REFCOUNT_ADD(*outFile);
    return NOERROR;
}

ECode CFileDescriptorHelper::GetErrFile(
    /* [out] */ IFileDescriptor** errFile)
{
    VALIDATE_NOT_NULL(errFile);

    *errFile = CFileDescriptor::ERR;
    REFCOUNT_ADD(*errFile);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
