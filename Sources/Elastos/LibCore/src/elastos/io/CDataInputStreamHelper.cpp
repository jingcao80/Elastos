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

#include "CDataInputStreamHelper.h"
#include "DataInputStream.h"

namespace Elastos {
namespace IO {

CAR_SINGLETON_IMPL(CDataInputStreamHelper)

CAR_INTERFACE_IMPL(CDataInputStreamHelper, Singleton, IDataInputStreamHelper)

CDataInputStreamHelper::ReadUTF(
    /* [in] */ IDataInput* dataInput,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 value = 0;
    dataInput->ReadUnsignedInt16(&value);
    *str = DataInputStream::DecodeUTF(value, dataInput);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
