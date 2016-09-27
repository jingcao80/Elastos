
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
