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

#include "FileReader.h"
#include "CFileInputStream.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileReader, InputStreamReader, IFileReader)

ECode FileReader::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IInputStream> is;
    FAIL_RETURN(CFileInputStream::New(file, (IInputStream**)&is))
    return InputStreamReader::constructor(is);
}

ECode FileReader::constructor(
    /* [in] */ IFileDescriptor* file)
{
    AutoPtr<IInputStream> is;
    CFileInputStream::New(file, (IInputStream**)&is);
    return InputStreamReader::constructor(is);
}

ECode FileReader::constructor(
    /* [in] */ const String& file)
{
    AutoPtr<IInputStream> is;
    FAIL_RETURN(CFileInputStream::New(file, (IInputStream**)&is))
    return InputStreamReader::constructor(is);
}


} // namespace IO
} // namespace Elastos