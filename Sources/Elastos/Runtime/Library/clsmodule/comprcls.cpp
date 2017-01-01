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

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <malloc.h>

#include <clsbase.h>

int CompressCLS(
    /* [in] */ void* dest)
{
    CLSModule* destModule = (CLSModule *)dest;
    CLSModule* srcModule = (CLSModule *)alloca(destModule->mSize);
    if (!srcModule) _ReturnError(CLSError_OutOfMemory);

    memcpy(srcModule, destModule, destModule->mSize);
    int dataSize = srcModule->mSize - sizeof(CLSModule);

    if (compress(
        (Bytef *)destModule + sizeof(CLSModule),
        (uLongf *)&dataSize,
        (Bytef *)srcModule + sizeof(CLSModule),
        (uLongf)dataSize) != Z_OK) {
        _ReturnError(CLSError_Compress);
    }

    _ReturnOK(dataSize + sizeof(CLSModule));
}

int UncompressCLS(
    /* [in] */ const void* src,
    /* [in] */ int size,
    /* [in] */ CLSModule* destModule)
{
    CLSModule* srcModule = (CLSModule *)src;
    int dataSize = srcModule->mSize - sizeof(CLSModule);

    if (uncompress(
        (Bytef *)destModule + sizeof(CLSModule),
        (uLongf *)&dataSize,
        (Bytef *)srcModule + sizeof(CLSModule),
        (uLongf)size - sizeof(CLSModule)) != Z_OK) {
        _ReturnError(CLSError_Uncompress);
    }

    memcpy(destModule, srcModule, sizeof(CLSModule));
    _ReturnOK(dataSize + sizeof(CLSModule));
}
