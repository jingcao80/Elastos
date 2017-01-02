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

#ifndef __ELASTOS_DROID_GRAPHICS_UTILS_H__
#define __ELASTOS_DROID_GRAPHICS_UTILS_H__

#include <skia/core/SkStream.h>
#include <androidfw/Asset.h>
#include <unistd.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class AssetStreamAdaptor : public SkStreamRewindable {
public:
    // Enum passed to constructor. If set to kYes_OwnAsset,
    // the passed in Asset will be deleted upon destruction.
    enum OwnAsset {
        kYes_OwnAsset,
        kNo_OwnAsset,
    };

    // Enum passed to constructor. If set to kYes_HasMemoryBase,
    // getMemoryBase will return the Asset's buffer.
    enum HasMemoryBase {
        kYes_HasMemoryBase,
        kNo_HasMemoryBase,
    };

    AssetStreamAdaptor(android::Asset*, OwnAsset, HasMemoryBase);
    ~AssetStreamAdaptor();

    virtual bool rewind();
    virtual size_t read(void* buffer, size_t size);
    virtual bool hasLength() const { return true; }
    virtual size_t getLength() const;
    virtual bool isAtEnd() const;

    virtual const void* getMemoryBase() { return fMemoryBase; }

    virtual SkStreamRewindable* duplicate() const;
private:
    android::Asset*              fAsset;
    const void* const   fMemoryBase;
    const OwnAsset      fOwnAsset;
};

/**
 *  Make a deep copy of the asset, and return it as a stream, or NULL if there
 *  was an error.
 *  FIXME: If we could "ref/reopen" the asset, we may not need to copy it here.
 */

SkMemoryStream* CopyAssetToStream(android::Asset*);

/** Restore the file descriptor's offset in our destructor
 */
class AutoFDSeek {
public:
    AutoFDSeek(int fd) : mFD(fd)
    {
        mCurr = ::lseek(fd, 0, SEEK_CUR);
    }

    ~AutoFDSeek()
    {
        if (mCurr >= 0) {
            ::lseek(mFD, mCurr, SEEK_SET);
        }
    }
private:
    int     mFD;
    off64_t   mCurr;
};

}; // namespace Graphics
}; // namespace Droid
}; // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_UTILS_H__
