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

#include "CFilterInputStream.h"
#include "CBufferedInputStream.h"
#include "CFilterOutputStream.h"
#include "CBufferedOutputStream.h"
#include "CBufferedReader.h"
#include "CBufferedWriter.h"
#include "CStringWriter.h"
#include "CByteArrayInputStream.h"
#include "CByteArrayOutputStream.h"
#include "CCharArrayReader.h"
#include "CCharArrayWriter.h"
#include "CDataInputStream.h"
#include "CPushbackInputStream.h"
#include "CDataOutputStream.h"
#include "CLineNumberInputStream.h"
#include "CLineNumberReader.h"
#include "COutputStreamWriter.h"
#include "CPushbackReader.h"
#include "CSequenceInputStream.h"
#include "CStringBufferInputStream.h"
#include "CStringReader.h"
#include "CFile.h"
#include "CFileInputStream.h"
#include "CFileOutputStream.h"
#include "CInputStreamReader.h"
#include "CFileReader.h"
#include "CFileWriter.h"
#include "CPrintWriter.h"
#include "CPipedInputStream.h"
#include "CPipedOutputStream.h"
#include "CPipedReader.h"
#include "CPipedWriter.h"
#include "CStreamTokenizer.h"
#include "CPrintStream.h"

namespace Elastos {
namespace IO {

CAR_OBJECT_IMPL(CFilterInputStream)

CAR_OBJECT_IMPL(CBufferedInputStream)

CAR_OBJECT_IMPL(CFilterOutputStream)

CAR_OBJECT_IMPL(CBufferedOutputStream)

CAR_OBJECT_IMPL(CBufferedReader)

CAR_OBJECT_IMPL(CBufferedWriter)

CAR_OBJECT_IMPL(CStringWriter)

CAR_OBJECT_IMPL(CByteArrayInputStream)

CAR_OBJECT_IMPL(CByteArrayOutputStream)

CAR_OBJECT_IMPL(CCharArrayReader)

CAR_OBJECT_IMPL(CCharArrayWriter)

CAR_OBJECT_IMPL(CDataInputStream)

CAR_OBJECT_IMPL(CPushbackInputStream)

CAR_OBJECT_IMPL(CDataOutputStream)

CAR_OBJECT_IMPL(CLineNumberInputStream)

CAR_OBJECT_IMPL(CLineNumberReader)

CAR_OBJECT_IMPL(COutputStreamWriter)

CAR_OBJECT_IMPL(CPushbackReader)

CAR_OBJECT_IMPL(CInputStreamReader)

CAR_OBJECT_IMPL(CSequenceInputStream)

CAR_OBJECT_IMPL(CStringBufferInputStream)

CAR_OBJECT_IMPL(CStringReader)

CAR_OBJECT_IMPL(CFileInputStream)

CAR_OBJECT_IMPL(CFileOutputStream)

CAR_OBJECT_IMPL(CFile)

CAR_OBJECT_IMPL(CFileReader)

CAR_OBJECT_IMPL(CFileWriter)

CAR_OBJECT_IMPL(CPrintWriter)

CAR_OBJECT_IMPL(CPipedInputStream)

CAR_OBJECT_IMPL(CPipedOutputStream)

CAR_OBJECT_IMPL(CPipedReader)

CAR_OBJECT_IMPL(CPipedWriter)

CAR_OBJECT_IMPL(CStreamTokenizer)

CAR_OBJECT_IMPL(CPrintStream)

} // namespace IO
} // namespace Elastos
