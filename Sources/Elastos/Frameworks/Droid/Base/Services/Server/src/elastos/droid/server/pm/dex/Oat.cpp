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

#include "elastos/droid/server/pm/dex/Oat.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

static const int kPageSize = 4096;

const uint8_t OatHeader::sOatMagic[] = { 'o', 'a', 't', '\n' };
const uint8_t OatHeader::sOatVersion[] = { '0', '3', '9', '\0' };

Boolean OatHeader::IsValid() const
{
    if (memcmp(magic_, sOatMagic, sizeof(sOatMagic)) != 0) {
        return FALSE;
    }
    if (memcmp(version_, sOatVersion, sizeof(sOatVersion)) != 0) {
      return FALSE;
    }
    if ((executable_offset_ & (kPageSize - 1)) != 0) {
      return FALSE;
    }
    if ((image_patch_delta_ & (kPageSize - 1)) != 0) {
      return FALSE;
    }
    return TRUE;
}

uint32_t OatHeader::GetDexFileCount() const
{
    assert(IsValid());
    return dex_file_count_;
}

uint32_t OatHeader::GetKeyValueStoreSize() const
{
    assert(IsValid());
    return key_value_store_size_;
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
