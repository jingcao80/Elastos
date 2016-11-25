
#undef LOG_TAG
#define LOG_TAG "NativeCursorWindow"

#include "elastos/droid/database/NativeCursorWindow.h"
#include <binder/Parcel.h>
#include <utils/Log.h>
#include <utils/String8.h>

#include <cutils/ashmem.h>
#include <sys/mman.h>

#include <assert.h>
#include <stdlib.h>

namespace Elastos {
namespace Droid {
namespace Database {

NativeCursorWindow::NativeCursorWindow(const String& name, int ashmemFd,
        void* data, size_t size, bool readOnly) :
        mName(name), mAshmemFd(ashmemFd), mData(data), mSize(size), mReadOnly(readOnly) {
    mHeader = static_cast<Header*>(mData);
}

NativeCursorWindow::~NativeCursorWindow() {
    ::munmap(mData, mSize);
    ::close(mAshmemFd);
}

android::status_t NativeCursorWindow::create(const String& name, size_t size, NativeCursorWindow** outCursorWindow) {
    String ashmemName("NativeCursorWindow: ");
    ashmemName.Append(name);

    android::status_t result;
    int ashmemFd = ashmem_create_region(ashmemName.string(), size);
    if (ashmemFd < 0) {
        result = -errno;
    } else {
        result = ashmem_set_prot_region(ashmemFd, PROT_READ | PROT_WRITE);
        if (result >= 0) {
            void* data = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, ashmemFd, 0);
            if (data == MAP_FAILED) {
                result = -errno;
            } else {
                result = ashmem_set_prot_region(ashmemFd, PROT_READ);
                if (result >= 0) {
                    NativeCursorWindow* window = new NativeCursorWindow(name, ashmemFd,
                            data, size, false /*readOnly*/);
                    result = window->clear();
                    if (!result) {
                        LOG_WINDOW("Created new NativeCursorWindow: freeOffset=%d, "
                                "numRows=%d, numColumns=%d, mSize=%d, mData=%p",
                                window->mHeader->freeOffset,
                                window->mHeader->numRows,
                                window->mHeader->numColumns,
                                window->mSize, window->mData);
                        *outCursorWindow = window;
                        return android::OK;
                    }
                    delete window;
                }
            }
            ::munmap(data, size);
        }
        ::close(ashmemFd);
    }
    *outCursorWindow = NULL;
    return result;
}

android::status_t NativeCursorWindow::createFromParcel(IParcel* parcel, NativeCursorWindow** outCursorWindow) {
    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    android::String8 str8(p->readString8());
    String name(str8.string());

    android::status_t result;
    int ashmemFd = p->readFileDescriptor();
    if (ashmemFd == int(android::BAD_TYPE)) {
        result = android::BAD_TYPE;
    } else {
        ssize_t size = ashmem_get_size_region(ashmemFd);
        if (size < 0) {
            result = android::UNKNOWN_ERROR;
        } else {
            int dupAshmemFd = ::dup(ashmemFd);
            if (dupAshmemFd < 0) {
                result = -errno;
            } else {
                void* data = ::mmap(NULL, size, PROT_READ, MAP_SHARED, dupAshmemFd, 0);
                if (data == MAP_FAILED) {
                    result = -errno;
                } else {
                    NativeCursorWindow* window = new NativeCursorWindow(name, dupAshmemFd,
                            data, size, true /*readOnly*/);
                    LOG_WINDOW("Created NativeCursorWindow from parcel: freeOffset=%d, "
                            "numRows=%d, numColumns=%d, mSize=%d, mData=%p",
                            window->mHeader->freeOffset,
                            window->mHeader->numRows,
                            window->mHeader->numColumns,
                            window->mSize, window->mData);
                    *outCursorWindow = window;
                    return android::OK;
                }
                ::close(dupAshmemFd);
            }
        }
    }
    *outCursorWindow = NULL;
    return result;
}

android::status_t NativeCursorWindow::writeToParcel(IParcel* parcel) {
    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    android::String8 name(mName.string());
    android::status_t status = p->writeString8(name);
    if (!status) {
        status = p->writeDupFileDescriptor(mAshmemFd);
    }
    return status;
}

android::status_t NativeCursorWindow::clear() {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    mHeader->freeOffset = sizeof(Header) + sizeof(RowSlotChunk);
    mHeader->firstChunkOffset = sizeof(Header);
    mHeader->numRows = 0;
    mHeader->numColumns = 0;

    RowSlotChunk* firstChunk = static_cast<RowSlotChunk*>(offsetToPtr(mHeader->firstChunkOffset));
    firstChunk->nextChunkOffset = 0;
    return android::OK;
}

android::status_t NativeCursorWindow::setNumColumns(uint32_t numColumns) {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    uint32_t cur = mHeader->numColumns;
    if ((cur > 0 || mHeader->numRows > 0) && cur != numColumns) {
        ALOGE("Trying to go from %d columns to %d", cur, numColumns);
        return android::INVALID_OPERATION;
    }
    mHeader->numColumns = numColumns;
    return android::OK;
}

android::status_t NativeCursorWindow::allocRow() {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    // Fill in the row slot
    RowSlot* rowSlot = allocRowSlot();
    if (rowSlot == NULL) {
        return android::NO_MEMORY;
    }

    // Allocate the slots for the field directory
    size_t fieldDirSize = mHeader->numColumns * sizeof(FieldSlot);
    uint32_t fieldDirOffset = alloc(fieldDirSize, true /*aligned*/);
    if (!fieldDirOffset) {
        mHeader->numRows--;
        LOG_WINDOW("The row failed, so back out the new row accounting "
                "from allocRowSlot %d", mHeader->numRows);
        return android::NO_MEMORY;
    }
    FieldSlot* fieldDir = static_cast<FieldSlot*>(offsetToPtr(fieldDirOffset));
    memset(fieldDir, 0, fieldDirSize);

    // LOG_WINDOW("Allocated row %u, rowSlot is at offset %u, fieldDir is %d bytes at offset %u\n",
    //         mHeader->numRows - 1, offsetFromPtr(rowSlot), fieldDirSize, fieldDirOffset);
    rowSlot->offset = fieldDirOffset;
    return android::OK;
}

android::status_t NativeCursorWindow::freeLastRow() {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    if (mHeader->numRows > 0) {
        mHeader->numRows--;
    }
    return android::OK;
}

uint32_t NativeCursorWindow::alloc(size_t size, bool aligned) {
    uint32_t padding;
    if (aligned) {
        // 4 byte alignment
        padding = (~mHeader->freeOffset + 1) & 3;
    } else {
        padding = 0;
    }

    uint32_t offset = mHeader->freeOffset + padding;
    uint32_t nextFreeOffset = offset + size;
    if (nextFreeOffset > mSize) {
        ALOGW("Window is full: requested allocation %d bytes, "
                "free space %d bytes, window size %d bytes",
                size, freeSpace(), mSize);
        return 0;
    }

    mHeader->freeOffset = nextFreeOffset;
    return offset;
}

NativeCursorWindow::RowSlot* NativeCursorWindow::getRowSlot(uint32_t row) {
    uint32_t chunkPos = row;
    RowSlotChunk* chunk = static_cast<RowSlotChunk*>(
            offsetToPtr(mHeader->firstChunkOffset));
    while (chunkPos >= ROW_SLOT_CHUNK_NUM_ROWS) {
        chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
        chunkPos -= ROW_SLOT_CHUNK_NUM_ROWS;
    }
    return &chunk->slots[chunkPos];
}

NativeCursorWindow::RowSlot* NativeCursorWindow::allocRowSlot() {
    uint32_t chunkPos = mHeader->numRows;
    RowSlotChunk* chunk = static_cast<RowSlotChunk*>(
            offsetToPtr(mHeader->firstChunkOffset));
    while (chunkPos > ROW_SLOT_CHUNK_NUM_ROWS) {
        chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
        chunkPos -= ROW_SLOT_CHUNK_NUM_ROWS;
    }
    if (chunkPos == ROW_SLOT_CHUNK_NUM_ROWS) {
        if (!chunk->nextChunkOffset) {
            chunk->nextChunkOffset = alloc(sizeof(RowSlotChunk), true /*aligned*/);
            if (!chunk->nextChunkOffset) {
                return NULL;
            }
        }
        chunk = static_cast<RowSlotChunk*>(offsetToPtr(chunk->nextChunkOffset));
        chunk->nextChunkOffset = 0;
        chunkPos = 0;
    }
    mHeader->numRows += 1;
    return &chunk->slots[chunkPos];
}

NativeCursorWindow::FieldSlot* NativeCursorWindow::getFieldSlot(uint32_t row, uint32_t column) {
    if (row >= mHeader->numRows || column >= mHeader->numColumns) {
        ALOGE("Failed to read row %d, column %d from a NativeCursorWindow which "
                "has %d rows, %d columns.",
                row, column, mHeader->numRows, mHeader->numColumns);
        return NULL;
    }
    RowSlot* rowSlot = getRowSlot(row);
    if (!rowSlot) {
        ALOGE("Failed to find rowSlot for row %d.", row);
        return NULL;
    }
    FieldSlot* fieldDir = static_cast<FieldSlot*>(offsetToPtr(rowSlot->offset));
    return &fieldDir[column];
}

android::status_t NativeCursorWindow::putBlob(uint32_t row, uint32_t column, const void* value, size_t size) {
    return putBlobOrString(row, column, value, size, FIELD_TYPE_BLOB);
}

android::status_t NativeCursorWindow::putString(uint32_t row, uint32_t column, const char* value,
        size_t sizeIncludingNull) {
    return putBlobOrString(row, column, value, sizeIncludingNull, FIELD_TYPE_STRING);
}

android::status_t NativeCursorWindow::putBlobOrString(uint32_t row, uint32_t column,
        const void* value, size_t size, int32_t type) {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    FieldSlot* fieldSlot = getFieldSlot(row, column);
    if (!fieldSlot) {
        return android::BAD_VALUE;
    }

    uint32_t offset = alloc(size);
    if (!offset) {
        return android::NO_MEMORY;
    }

    memcpy(offsetToPtr(offset), value, size);

    fieldSlot->type = type;
    fieldSlot->data.buffer.offset = offset;
    fieldSlot->data.buffer.size = size;
    return android::OK;
}

android::status_t NativeCursorWindow::putLong(uint32_t row, uint32_t column, int64_t value) {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    FieldSlot* fieldSlot = getFieldSlot(row, column);
    if (!fieldSlot) {
        return android::BAD_VALUE;
    }

    fieldSlot->type = FIELD_TYPE_INTEGER;
    fieldSlot->data.l = value;
    return android::OK;
}

android::status_t NativeCursorWindow::putDouble(uint32_t row, uint32_t column, double value) {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    FieldSlot* fieldSlot = getFieldSlot(row, column);
    if (!fieldSlot) {
        return android::BAD_VALUE;
    }

    fieldSlot->type = FIELD_TYPE_FLOAT;
    fieldSlot->data.d = value;
    return android::OK;
}

android::status_t NativeCursorWindow::putNull(uint32_t row, uint32_t column) {
    if (mReadOnly) {
        return android::INVALID_OPERATION;
    }

    FieldSlot* fieldSlot = getFieldSlot(row, column);
    if (!fieldSlot) {
        return android::BAD_VALUE;
    }

    fieldSlot->type = FIELD_TYPE_NULL;
    fieldSlot->data.buffer.offset = 0;
    fieldSlot->data.buffer.size = 0;
    return android::OK;
}

} //Database
} //Droid
} //Elastos
