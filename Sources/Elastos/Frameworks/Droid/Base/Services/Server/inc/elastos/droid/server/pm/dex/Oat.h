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

#ifndef __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__

#include <elastos/coredef.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

enum InstructionSet {
    kNone,
    kArm,
    kArm64,
    kThumb2,
    kX86,
    kX86_64,
    kMips,
    kMips64
};

enum InstructionFeatures {
    kHwDiv  = 0x1,              // Supports hardware divide.
    kHwLpae = 0x2,              // Supports Large Physical Address Extension.
};

// This is a bitmask of supported features per architecture.
class __attribute__ ((__aligned__(4), __packed__)) InstructionSetFeatures
{
public:
    InstructionSetFeatures() : mMask(0) {}
    explicit InstructionSetFeatures(uint32_t mask) : mMask(mask) {}

    static CARAPI_(InstructionSetFeatures) GuessInstructionSetFeatures();

    CARAPI_(Boolean) HasDivideInstruction() const
    {
        return (mMask & kHwDiv) != 0;
    }

    CARAPI_(void) SetHasDivideInstruction(Boolean v)
    {
        mMask = (mMask & ~kHwDiv) | (v ? kHwDiv : 0);
    }

    CARAPI_(Boolean) HasLpae() const
    {
        return (mMask & kHwLpae) != 0;
    }

    CARAPI_(void) SetHasLpae(Boolean v)
    {
        mMask = (mMask & ~kHwLpae) | (v ? kHwLpae : 0);
    }

    CARAPI_(String) GetFeatureString() const;

    // Other features in here.

    CARAPI_(Boolean) operator==(const InstructionSetFeatures &peer) const
    {
        return mMask == peer.mMask;
    }

    CARAPI_(Boolean) operator!=(const InstructionSetFeatures &peer) const
    {
        return mMask != peer.mMask;
    }

    CARAPI_(Boolean) operator<=(const InstructionSetFeatures &peer) const
    {
        return mMask != peer.mMask;
    }

private:
    uint32_t mMask;
};

class __attribute__ ((__aligned__(4), __packed__)) OatHeader
{
public:
    static const uint8_t sOatMagic[4];
    static const uint8_t sOatVersion[4];

    CARAPI_(Boolean) IsValid() const;

    CARAPI_(uint32_t) GetDexFileCount() const;

    CARAPI_(uint32_t) GetKeyValueStoreSize() const;

private:
    uint8_t magic_[4];
    uint8_t version_[4];
    uint32_t adler32_checksum_;

    InstructionSet instruction_set_;
    InstructionSetFeatures instruction_set_features_;
    uint32_t dex_file_count_;
    uint32_t executable_offset_;
    uint32_t interpreter_to_interpreter_bridge_offset_;
    uint32_t interpreter_to_compiled_code_bridge_offset_;
    uint32_t jni_dlsym_lookup_offset_;
    uint32_t portable_imt_conflict_trampoline_offset_;
    uint32_t portable_resolution_trampoline_offset_;
    uint32_t portable_to_interpreter_bridge_offset_;
    uint32_t quick_generic_jni_trampoline_offset_;
    uint32_t quick_imt_conflict_trampoline_offset_;
    uint32_t quick_resolution_trampoline_offset_;
    uint32_t quick_to_interpreter_bridge_offset_;

    // The amount that the image this oat is associated with has been patched.
    int32_t image_patch_delta_;

    uint32_t image_file_location_oat_checksum_;
    uint32_t image_file_location_oat_data_begin_;

    uint32_t key_value_store_size_;
    uint8_t key_value_store_[0];  // note variable width data at end
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__
