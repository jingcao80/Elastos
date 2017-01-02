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

#include "CFileHelper.h"
#include "CFile.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CFileHelper, Singleton, IFileHelper)

CAR_SINGLETON_IMPL(CFileHelper)

ECode CFileHelper::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [out] */ IFile** file)
{
    return CFile::CreateTempFile(prefix, suffix, file);
}

ECode CFileHelper::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [in] */ IFile* directory,
    /* [out] */ IFile** file)
{
    return CFile::CreateTempFile(prefix, suffix, directory, file);
}

ECode CFileHelper::GetSeparatorChar(
    /* [out] */ Char32* separatorChar)
{
    VALIDATE_NOT_NULL(separatorChar);
    *separatorChar = CFile::sSeparatorChar;
    return NOERROR;
}

ECode CFileHelper::GetSeparator(
    /* [out] */ String* separator)
{
    VALIDATE_NOT_NULL(separator);
    *separator = CFile::sSeparator;
    return NOERROR;
}

ECode CFileHelper::GetPathSeparatorChar(
    /* [out] */ Char32* pathSeparatorChar)
{
    VALIDATE_NOT_NULL(pathSeparatorChar);
    *pathSeparatorChar = CFile::sPathSeparatorChar;
    return NOERROR;
}

ECode CFileHelper::GetPathSeparator(
    /* [out] */ String* pathSeparator)
{
    VALIDATE_NOT_NULL(pathSeparator);
    *pathSeparator = CFile::sPathSeparator;
    return NOERROR;
}

ECode CFileHelper::ListRoots(
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    return CFile::ListRoots(files);
}

} // namespace IO
} // namespace Elastos
