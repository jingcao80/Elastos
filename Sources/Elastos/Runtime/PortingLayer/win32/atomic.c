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

#include "sys/atomics.h"
#include <windows.h>

int __atomic_inc(int *ptr)
{
    return InterlockedIncrement((long *)ptr);
}

int __atomic_dec(int *ptr)
{
    return InterlockedDecrement((long *)ptr);
}

int __atomic_swap(int _new, int *ptr)
{
    return InterlockedExchangeAdd((long *)ptr, _new);
}

int __atomic_cmpxchg(int old, int _new, int *ptr)
{
    return InterlockedCompareExchange((long *)ptr, _new, old);
}
