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

#include "StringBuilder.h"
#include <stdlib.h>
#include <string.h>

StringBuilder::StringBuilder(
    /* [in] */ int cap)
    : mSize(0)
    , mCapacity(cap)
{
    mBuf = (char*)malloc(mCapacity);
}

StringBuilder::~StringBuilder()
{
    free(mBuf);
    mCapacity = mSize = 0;
}

void StringBuilder::Append(
    /* [in] */ const char* str)
{
    int size = strlen(str);
    if (mSize + size + 1 > mCapacity) {
        int newSize = ((mSize + size) * 3) / 2;
        char* data = (char*)realloc(mBuf, newSize);
        mBuf = data;
        mCapacity = newSize;
    }
    strcpy(mBuf + mSize, str);
    mSize += size;
}

char* StringBuilder::ToString()
{
    return mBuf;
}

int StringBuilder::GetLength()
{
    return mSize;
}

void StringBuilder::Reset()
{
    mSize = 0;
}
