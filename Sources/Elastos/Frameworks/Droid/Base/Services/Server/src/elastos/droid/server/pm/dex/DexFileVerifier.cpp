
#include "elastos/droid/server/pm/dex/DexFileVerifier.h"
#include <zlib.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

static uint32_t MapTypeToBitMask(uint32_t map_type)
{
    switch (map_type) {
        case DexFile::kDexTypeHeaderItem:               return 1 << 0;
        case DexFile::kDexTypeStringIdItem:             return 1 << 1;
        case DexFile::kDexTypeTypeIdItem:               return 1 << 2;
        case DexFile::kDexTypeProtoIdItem:              return 1 << 3;
        case DexFile::kDexTypeFieldIdItem:              return 1 << 4;
        case DexFile::kDexTypeMethodIdItem:             return 1 << 5;
        case DexFile::kDexTypeClassDefItem:             return 1 << 6;
        case DexFile::kDexTypeMapList:                  return 1 << 7;
        case DexFile::kDexTypeTypeList:                 return 1 << 8;
        case DexFile::kDexTypeAnnotationSetRefList:     return 1 << 9;
        case DexFile::kDexTypeAnnotationSetItem:        return 1 << 10;
        case DexFile::kDexTypeClassDataItem:            return 1 << 11;
        case DexFile::kDexTypeCodeItem:                 return 1 << 12;
        case DexFile::kDexTypeStringDataItem:           return 1 << 13;
        case DexFile::kDexTypeDebugInfoItem:            return 1 << 14;
        case DexFile::kDexTypeAnnotationItem:           return 1 << 15;
        case DexFile::kDexTypeEncodedArrayItem:         return 1 << 16;
        case DexFile::kDexTypeAnnotationsDirectoryItem: return 1 << 17;
    }
    return 0;
}

static bool IsDataSectionType(uint32_t map_type)
{
    switch (map_type) {
        case DexFile::kDexTypeHeaderItem:
        case DexFile::kDexTypeStringIdItem:
        case DexFile::kDexTypeTypeIdItem:
        case DexFile::kDexTypeProtoIdItem:
        case DexFile::kDexTypeFieldIdItem:
        case DexFile::kDexTypeMethodIdItem:
        case DexFile::kDexTypeClassDefItem:
            return FALSE;
    }
    return TRUE;
}

Boolean DexFileVerifier::Verify(
    /* [in] */ const DexFile* dex_file,
    /* [in] */ const Byte* begin,
    /* [in] */ size_t size,
    /* [in] */ const char* location,
    /* [out] */ String* error_msg)
{
    AutoPtr<DexFileVerifier> verifier = new DexFileVerifier(dex_file, begin, size, location);
    if (!verifier->Verify()) {
        *error_msg = verifier->FailureReason();
        return FALSE;
    }
    return TRUE;
}

Boolean DexFileVerifier::CheckListSize(
    /* [in] */ const void* start,
    /* [in] */ size_t count,
    /* [in] */ size_t elem_size,
    /* [in] */ const char* label)
{
    // Check that size is not 0.
    assert(elem_size != 0U);

    const byte* range_start = reinterpret_cast<const byte*>(start);
    const byte* file_start = reinterpret_cast<const byte*>(mBegin);

    // Check for overflow.
    uintptr_t max = 0 - 1;
    size_t available_bytes_till_end_of_mem = max - reinterpret_cast<uintptr_t>(start);
    size_t max_count = available_bytes_till_end_of_mem / elem_size;
    if (max_count < count) {
        ErrorStringPrintf("Overflow in range for %s: %zx for %zu@%zu", label,
                        static_cast<size_t>(range_start - file_start),
                        count, elem_size);
        return FALSE;
    }

    const byte* range_end = range_start + count * elem_size;
    const byte* file_end = file_start + mSize;
    if ((range_start < file_start) || (range_end > file_end)) {
        // Note: these two tests are enough as we make sure above that there's no overflow.
        ErrorStringPrintf("Bad range for %s: %zx to %zx", label,
                        static_cast<size_t>(range_start - file_start),
                        static_cast<size_t>(range_end - file_start));
        return FALSE;
    }
    return TRUE;
}

Boolean DexFileVerifier::CheckValidOffsetAndSize(
    /* [in] */ uint32_t offset,
    /* [in] */ uint32_t size,
    /* [in] */ const char* label)
{
    if (size == 0) {
        if (offset != 0) {
            ErrorStringPrintf("Offset(%d) should be zero when size is zero for %s.", offset, label);
            return FALSE;
        }
    }
    if (mSize <= offset) {
        ErrorStringPrintf("Offset(%d) should be within file size(%zu) for %s.", offset, mSize, label);
        return FALSE;
    }
    return TRUE;
}

Boolean DexFileVerifier::CheckHeader()
{
    // Check file size from the header.
    uint32_t expected_size = mHeader->mFileSize;
    if (mSize != expected_size) {
        ErrorStringPrintf("Bad file size (%zd, expected %ud)", mSize, expected_size);
        return FALSE;
    }

    // Compute and verify the checksum in the header.
    uint32_t adler_checksum = adler32(0L, Z_NULL, 0);
    const uint32_t non_sum = sizeof(mHeader->mMagic) + sizeof(mHeader->mChecksum);
    const byte* non_sum_ptr = reinterpret_cast<const byte*>(mHeader) + non_sum;
    adler_checksum = adler32(adler_checksum, non_sum_ptr, expected_size - non_sum);
    if (adler_checksum != mHeader->mChecksum) {
        ErrorStringPrintf("Bad checksum (%08x, expected %08x)", adler_checksum, mHeader->mChecksum);
        return FALSE;
    }

    // Check the contents of the header.
    if (mHeader->mEndianTag != DexFile::sDexEndianConstant) {
        ErrorStringPrintf("Unexpected endian_tag: %x", mHeader->mEndianTag);
        return FALSE;
    }

    if (mHeader->mHeaderSize != sizeof(DexFile::Header)) {
        ErrorStringPrintf("Bad header size: %ud", mHeader->mHeaderSize);
        return FALSE;
    }

    // Check that all offsets are inside the file.
    Boolean result =
        CheckValidOffsetAndSize(mHeader->mLinkOff, mHeader->mLinkSize, "link") &&
        CheckValidOffsetAndSize(mHeader->mMapOff, mHeader->mMapOff, "map") &&
        CheckValidOffsetAndSize(mHeader->mStringIdsOff, mHeader->mStringIdsSize, "string-ids") &&
        CheckValidOffsetAndSize(mHeader->mTypeIdsOff, mHeader->mTypeIdsSize, "type-ids") &&
        CheckValidOffsetAndSize(mHeader->mProtoIdsOff, mHeader->mProtoIdsSize, "proto-ids") &&
        CheckValidOffsetAndSize(mHeader->mFieldIdsOff, mHeader->mFieldIdsSize, "field-ids") &&
        CheckValidOffsetAndSize(mHeader->mMethodIdsOff, mHeader->mMethodIdsSize, "method-ids") &&
        CheckValidOffsetAndSize(mHeader->mClassDefsOff, mHeader->mClassDefsSize, "class-defs") &&
        CheckValidOffsetAndSize(mHeader->mDataOff, mHeader->mDataSize, "data");

   return result;
}

Boolean DexFileVerifier::CheckMap()
{
    const DexFile::MapList* map = reinterpret_cast<const DexFile::MapList*>(mBegin +
                                                                        mHeader->mMapOff);
    // Check that map list content is available.
    if (!CheckListSize(map, 1, sizeof(DexFile::MapList), "maplist content")) {
        return FALSE;
    }

    const DexFile::MapItem* item = map->mList;

    uint32_t count = map->mSize;
    uint32_t last_offset = 0;
    uint32_t data_item_count = 0;
    uint32_t data_items_left = mHeader->mDataSize;
    uint32_t used_bits = 0;

    // Sanity check the size of the map list.
    if (!CheckListSize(item, count, sizeof(DexFile::MapItem), "map size")) {
        return FALSE;
    }

    // Check the items listed in the map.
    for (uint32_t i = 0; i < count; i++) {
        if (last_offset >= item->mOffset && i != 0) {
            ErrorStringPrintf("Out of order map item: %x then %x", last_offset, item->mOffset);
            return FALSE;
        }
        if (item->mOffset >= mHeader->mFileSize) {
            ErrorStringPrintf("Map item after end of file: %x, size %x",
                            item->mOffset, mHeader->mFileSize);
            return FALSE;
        }

        if (IsDataSectionType(item->mType)) {
            uint32_t icount = item->mSize;
            if (icount > data_items_left) {
                ErrorStringPrintf("Too many items in data section: %ud", data_item_count + icount);
                return FALSE;
            }
            data_items_left -= icount;
            data_item_count += icount;
        }

        uint32_t bit = MapTypeToBitMask(item->mType);

        if (bit == 0) {
            ErrorStringPrintf("Unknown map section type %x", item->mType);
            return FALSE;
        }

        if ((used_bits & bit) != 0) {
            ErrorStringPrintf("Duplicate map section of type %x", item->mType);
            return FALSE;
        }

        used_bits |= bit;
        last_offset = item->mOffset;
        item++;
    }

    // Check for missing sections in the map.
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeHeaderItem)) == 0) {
        ErrorStringPrintf("Map is missing header entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeMapList)) == 0) {
        ErrorStringPrintf("Map is missing map_list entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeStringIdItem)) == 0 &&
                 ((mHeader->mStringIdsOff != 0) || (mHeader->mStringIdsSize != 0))) {
        ErrorStringPrintf("Map is missing string_ids entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeTypeIdItem)) == 0 &&
                 ((mHeader->mTypeIdsOff != 0) || (mHeader->mTypeIdsSize != 0))) {
        ErrorStringPrintf("Map is missing type_ids entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeProtoIdItem)) == 0 &&
                 ((mHeader->mProtoIdsOff != 0) || (mHeader->mProtoIdsSize != 0))) {
        ErrorStringPrintf("Map is missing proto_ids entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeFieldIdItem)) == 0 &&
                 ((mHeader->mFieldIdsOff != 0) || (mHeader->mFieldIdsSize != 0))) {
        ErrorStringPrintf("Map is missing field_ids entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeMethodIdItem)) == 0 &&
                 ((mHeader->mMethodIdsOff != 0) || (mHeader->mMethodIdsSize != 0))) {
        ErrorStringPrintf("Map is missing method_ids entry");
        return FALSE;
    }
    if ((used_bits & MapTypeToBitMask(DexFile::kDexTypeClassDefItem)) == 0 &&
                 ((mHeader->mClassDefsOff != 0) || (mHeader->mClassDefsSize != 0))) {
        ErrorStringPrintf("Map is missing class_defs entry");
        return FALSE;
    }
    return TRUE;
}

Boolean DexFileVerifier::CheckPadding(
    /* [in] */ size_t offset,
    /* [in] */ uint32_t aligned_offset)
{
    if (offset < aligned_offset) {
        if (!CheckListSize(mBegin + offset, aligned_offset - offset, sizeof(byte), "section")) {
            return FALSE;
        }
        while (offset < aligned_offset) {
            if (*mPtr != '\0') {
                ErrorStringPrintf("Non-zero padding %x before section start at %zx", *mPtr, offset);
                return FALSE;
            }
            mPtr++;
            offset++;
        }
    }
    return TRUE;
}

Boolean DexFileVerifier::CheckIntraSectionIterate(
    /* [in] */ size_t offset,
    /* [in] */ uint32_t section_count,
    /* [in] */ uint16_t type)
{
    // Get the right alignment mask for the type of section.
    size_t alignment_mask;
    switch (type) {
        case DexFile::kDexTypeClassDataItem:
        case DexFile::kDexTypeStringDataItem:
        case DexFile::kDexTypeDebugInfoItem:
        case DexFile::kDexTypeAnnotationItem:
        case DexFile::kDexTypeEncodedArrayItem:
            alignment_mask = sizeof(uint8_t) - 1;
            break;
        default:
            alignment_mask = sizeof(uint32_t) - 1;
            break;
    }

    // // Iterate through the items in the section.
    // for (uint32_t i = 0; i < section_count; i++) {
    //     size_t aligned_offset = (offset + alignment_mask) & ~alignment_mask;

    //     // Check the padding between items.
    //     if (!CheckPadding(offset, aligned_offset)) {
    //         return FALSE;
    //     }

    //     // Check depending on the section type.
    //     switch (type) {
    //         case DexFile::kDexTypeStringIdItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::StringId), "string_ids")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::StringId);
    //             break;
    //         }
    //         case DexFile::kDexTypeTypeIdItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::TypeId), "type_ids")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::TypeId);
    //             break;
    //         }
    //         case DexFile::kDexTypeProtoIdItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::ProtoId), "proto_ids")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::ProtoId);
    //             break;
    //         }
    //         case DexFile::kDexTypeFieldIdItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::FieldId), "field_ids")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::FieldId);
    //             break;
    //         }
    //         case DexFile::kDexTypeMethodIdItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::MethodId), "method_ids")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::MethodId);
    //             break;
    //         }
    //         case DexFile::kDexTypeClassDefItem: {
    //             if (!CheckListSize(mPtr, 1, sizeof(DexFile::ClassDef), "class_defs")) {
    //                 return FALSE;
    //             }
    //             mPtr += sizeof(DexFile::ClassDef);
    //             break;
    //         }
    //         case DexFile::kDexTypeTypeList: {
    //             if (!CheckList(sizeof(DexFile::TypeItem), "type_list", &mPtr)) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeAnnotationSetRefList: {
    //             if (!CheckList(sizeof(DexFile::AnnotationSetRefItem), "annotation_set_ref_list", &mPtr)) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeAnnotationSetItem: {
    //             if (!CheckList(sizeof(uint32_t), "annotation_set_item", &mPtr)) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeClassDataItem: {
    //             if (!CheckIntraClassDataItem()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeCodeItem: {
    //             if (!CheckIntraCodeItem()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeStringDataItem: {
    //             if (!CheckIntraStringDataItem()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeDebugInfoItem: {
    //             if (!CheckIntraDebugInfoItem()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeAnnotationItem: {
    //             if (!CheckIntraAnnotationItem()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeEncodedArrayItem: {
    //             if (!CheckEncodedArray()) {
    //                 return FALSE;
    //             }
    //             break;
    //         }
    //         case DexFile::kDexTypeAnnotationsDirectoryItem: {
    //             if (!CheckIntraAnnotationsDirectoryItem()) {
    //             return FALSE;
    //             }
    //             break;
    //         }
    //         default:
    //         ErrorStringPrintf("Unknown map item type %x", type);
    //         return FALSE;
    //     }

    //     if (IsDataSectionType(type)) {
    //         offset_to_type_map_.Put(aligned_offset, type);
    //     }

    //     aligned_offset = mPtr - begin_;
    //     if (aligned_offset > size_) {
    //         ErrorStringPrintf("Item %d at ends out of bounds", i);
    //         return FALSE;
    //     }

    //     offset = aligned_offset;
    // }

    return TRUE;
}

Boolean DexFileVerifier::CheckIntraIdSection(
    /* [in] */ size_t offset,
    /* [in] */ uint32_t count,
    /* [in] */ uint16_t type)
{
    uint32_t expected_offset;
    uint32_t expected_size;

    // Get the expected offset and size from the header.
    switch (type) {
        case DexFile::kDexTypeStringIdItem:
            expected_offset = mHeader->mStringIdsOff;
            expected_size = mHeader->mStringIdsSize;
            break;
        case DexFile::kDexTypeTypeIdItem:
            expected_offset = mHeader->mTypeIdsOff;
            expected_size = mHeader->mTypeIdsSize;
            break;
        case DexFile::kDexTypeProtoIdItem:
            expected_offset = mHeader->mProtoIdsOff;
            expected_size = mHeader->mProtoIdsSize;
            break;
        case DexFile::kDexTypeFieldIdItem:
            expected_offset = mHeader->mFieldIdsOff;
            expected_size = mHeader->mFieldIdsSize;
            break;
        case DexFile::kDexTypeMethodIdItem:
            expected_offset = mHeader->mMethodIdsOff;
            expected_size = mHeader->mMethodIdsSize;
            break;
        case DexFile::kDexTypeClassDefItem:
            expected_offset = mHeader->mClassDefsOff;
            expected_size = mHeader->mClassDefsSize;
            break;
        default:
            ErrorStringPrintf("Bad type for id section: %x", type);
            return FALSE;
    }

    // Check that the offset and size are what were expected from the header.
    if (offset != expected_offset) {
        ErrorStringPrintf("Bad offset for section: got %zx, expected %x", offset, expected_offset);
        return FALSE;
    }
    if (count != expected_size) {
        ErrorStringPrintf("Bad size for section: got %x, expected %x", count, expected_size);
        return FALSE;
    }

    return CheckIntraSectionIterate(offset, count, type);
}

Boolean DexFileVerifier::CheckIntraDataSection(
    /* [in] */ size_t offset,
    /* [in] */ uint32_t count,
    /* [in] */ uint16_t type)
{
    size_t data_start = mHeader->mDataOff;
    size_t data_end = data_start + mHeader->mDataSize;

    // Sanity check the offset of the section.
    if ((offset < data_start) || (offset > data_end)) {
        ErrorStringPrintf("Bad offset for data subsection: %zx", offset);
        return false;
    }

    if (!CheckIntraSectionIterate(offset, count, type)) {
        return FALSE;
    }

    size_t next_offset = mPtr - mBegin;
    if (next_offset > data_end) {
        ErrorStringPrintf("Out-of-bounds end of data subsection: %zx", next_offset);
        return FALSE;
    }

    return TRUE;
}

Boolean DexFileVerifier::CheckIntraSection()
{
    const DexFile::MapList* map = reinterpret_cast<const DexFile::MapList*>(mBegin + mHeader->mMapOff);
    const DexFile::MapItem* item = map->mList;

    uint32_t count = map->mSize;
    size_t offset = 0;
    mPtr = mBegin;

    // Check the items listed in the map.
    while (count--) {
        uint32_t section_offset = item->mOffset;
        uint32_t section_count = item->mSize;
        uint16_t type = item->mType;

        // Check for padding and overlap between items.
        if (!CheckPadding(offset, section_offset)) {
            return FALSE;
        }
        else if (offset > section_offset) {
            ErrorStringPrintf("Section overlap or out-of-order map: %zx, %x", offset, section_offset);
            return FALSE;
        }

        // Check each item based on its type.
        switch (type) {
            case DexFile::kDexTypeHeaderItem:
                if (section_count != 1) {
                    ErrorStringPrintf("Multiple header items");
                    return FALSE;
                }
                if (section_offset != 0) {
                    ErrorStringPrintf("Header at %x, not at start of file", section_offset);
                    return FALSE;
                }
                mPtr = mBegin + mHeader->mHeaderSize;
                offset = mHeader->mHeaderSize;
                break;
            case DexFile::kDexTypeStringIdItem:
            case DexFile::kDexTypeTypeIdItem:
            case DexFile::kDexTypeProtoIdItem:
            case DexFile::kDexTypeFieldIdItem:
            case DexFile::kDexTypeMethodIdItem:
            case DexFile::kDexTypeClassDefItem:
                if (!CheckIntraIdSection(section_offset, section_count, type)) {
                    return FALSE;
                }
                offset = mPtr - mBegin;
                break;
            case DexFile::kDexTypeMapList:
                if (section_count != 1) {
                    ErrorStringPrintf("Multiple map list items");
                    return FALSE;
                }
                if (section_offset != mHeader->mMapOff) {
                    ErrorStringPrintf("Map not at header-defined offset: %x, expected %x",
                                    section_offset, mHeader->mMapOff);
                    return FALSE;
                }
                mPtr += sizeof(uint32_t) + (map->mSize * sizeof(DexFile::MapItem));
                offset = section_offset + sizeof(uint32_t) + (map->mSize * sizeof(DexFile::MapItem));
                break;
            case DexFile::kDexTypeTypeList:
            case DexFile::kDexTypeAnnotationSetRefList:
            case DexFile::kDexTypeAnnotationSetItem:
            case DexFile::kDexTypeClassDataItem:
            case DexFile::kDexTypeCodeItem:
            case DexFile::kDexTypeStringDataItem:
            case DexFile::kDexTypeDebugInfoItem:
            case DexFile::kDexTypeAnnotationItem:
            case DexFile::kDexTypeEncodedArrayItem:
            case DexFile::kDexTypeAnnotationsDirectoryItem:
                if (!CheckIntraDataSection(section_offset, section_count, type)) {
                    return FALSE;
                }
                offset = mPtr - mBegin;
                break;
            default:
                ErrorStringPrintf("Unknown map item type %x", type);
                return FALSE;
        }

        item++;
    }

    return TRUE;
}

Boolean DexFileVerifier::CheckInterSection()
{
    const DexFile::MapList* map = reinterpret_cast<const DexFile::MapList*>(mBegin + mHeader->mMapOff);
    const DexFile::MapItem* item = map->mList;
    uint32_t count = map->mSize;

    // Cross check the items listed in the map.
    while (count--) {
        uint32_t section_offset = item->mOffset;
        uint32_t section_count = item->mSize;
        uint16_t type = item->mType;

        switch (type) {
            case DexFile::kDexTypeHeaderItem:
            case DexFile::kDexTypeMapList:
            case DexFile::kDexTypeTypeList:
            case DexFile::kDexTypeCodeItem:
            case DexFile::kDexTypeStringDataItem:
            case DexFile::kDexTypeDebugInfoItem:
            case DexFile::kDexTypeAnnotationItem:
            case DexFile::kDexTypeEncodedArrayItem:
                break;
            case DexFile::kDexTypeStringIdItem:
            case DexFile::kDexTypeTypeIdItem:
            case DexFile::kDexTypeProtoIdItem:
            case DexFile::kDexTypeFieldIdItem:
            case DexFile::kDexTypeMethodIdItem:
            case DexFile::kDexTypeClassDefItem:
            case DexFile::kDexTypeAnnotationSetRefList:
            case DexFile::kDexTypeAnnotationSetItem:
            case DexFile::kDexTypeClassDataItem:
            case DexFile::kDexTypeAnnotationsDirectoryItem: {
                // TODO:
                // if (!CheckInterSectionIterate(section_offset, section_count, type)) {
                //     return FALSE;
                // }
                break;
            }
            default:
                ErrorStringPrintf("Unknown map item type %x", type);
                return FALSE;
        }

        item++;
    }

    return TRUE;
}

Boolean DexFileVerifier::Verify()
{
    // Check the header.
    if (!CheckHeader()) {
        return FALSE;
    }

    // Check the map section.
    if (!CheckMap()) {
        return FALSE;
    }

    // Check structure within remaining sections.
    if (!CheckIntraSection()) {
        return FALSE;
    }

    // Check references from one section to another.
    if (!CheckInterSection()) {
        return FALSE;
    }

    return TRUE;
}

void DexFileVerifier::ErrorStringPrintf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    mFailureReason = "";
    mFailureReason.AppendFormat("Failure to verify dex file '%s': ", mLocation);
    mFailureReason.AppendFormat(fmt, ap);
    va_end(ap);
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
