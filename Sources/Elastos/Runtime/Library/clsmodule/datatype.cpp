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
#include <assert.h>

#include "clsbase.h"

int GetOriginalType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* srcDescriptor,
    /* [in] */ TypeDescriptor* destDescriptor)
{
    destDescriptor->mPointer = srcDescriptor->mPointer;
    destDescriptor->mUnsigned = srcDescriptor->mUnsigned;

    while (srcDescriptor->mType == Type_alias) {
        srcDescriptor = &module->mAliasDirs[srcDescriptor->mIndex]->mType;
        destDescriptor->mPointer += srcDescriptor->mPointer;
        destDescriptor->mUnsigned |= srcDescriptor->mUnsigned;
    }

    destDescriptor->mType = srcDescriptor->mType;
    destDescriptor->mIndex = srcDescriptor->mIndex;
    destDescriptor->mNestedType = srcDescriptor->mNestedType;

    _ReturnOK(CLS_NoError);
}

int GetArrayBaseType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* srcDescriptor,
    /* [in] */ TypeDescriptor* destDescriptor)
{
    TypeDescriptor* type = (TypeDescriptor *)srcDescriptor;

    while (Type_Array == type->mType) {
        type = &module->mArrayDirs[type->mIndex]->mType;
    }

    memcpy(destDescriptor, type, sizeof(TypeDescriptor));

    _ReturnOK(CLS_NoError);
}

BOOL IsEqualType(
    /* [in] */ const CLSModule* module,
    /* [in] */ const TypeDescriptor* descriptor1,
    /* [in] */ const TypeDescriptor* descriptor2)
{
    TypeDescriptor src, dest;

    GetOriginalType(module, descriptor1, &src);
    GetOriginalType(module, descriptor2, &dest);

    _Return (!memcmp(&src, &dest, sizeof(src)));
}
