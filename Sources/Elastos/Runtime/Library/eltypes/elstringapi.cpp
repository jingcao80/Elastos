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

#include <sys/types.h>
#include <eltypes.h>
#include <stdlib.h>
#include <ctype.h>

#include <ucase.h>

#define _atoi64 atol

_ELASTOS_NAMESPACE_USING

extern "C"
{
    ECO_PUBLIC SharedBuffer* gElEmptyStringBuf = NULL;
    ECO_PUBLIC char* gElEmptyString = NULL;
}

static void InitString()
{
    SharedBuffer* buf = SharedBuffer::Alloc(1);
    char* str = (char*)buf->GetData();
    *str = 0;
    gElEmptyStringBuf = buf;
    gElEmptyString = str;
}

static void UninitString()
{
    SharedBuffer::GetBufferFromData(gElEmptyString)->Release();
    gElEmptyStringBuf = NULL;
    gElEmptyString = NULL;
}

class LibUtilsFirstStatics
{
public:
    LibUtilsFirstStatics()
    {
        InitString();
    }

    ~LibUtilsFirstStatics()
    {
        UninitString();
    }
};

static LibUtilsFirstStatics gFirstStatics;

extern "C" {

ECO_PUBLIC Char32 __cdecl _String_ToLowerCase(
    /* [in] */ Char32 ch)
{
    return u_tolower(ch);
}

ECO_PUBLIC Char32 __cdecl _String_ToUpperCase(
    /* [in] */ Char32 ch)
{
    return u_toupper(ch);
}

}
