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

#include "FileWriter.h"
#include "CFileOutputStream.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileWriter, OutputStreamWriter, IFileWriter)

ECode FileWriter::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(file, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(file, append, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(fd, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ const String& filename)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(filename, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ const String& filename,
    /* [in] */ Boolean append)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(filename, append, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

} // namespace IO
} // namespace Elastos
