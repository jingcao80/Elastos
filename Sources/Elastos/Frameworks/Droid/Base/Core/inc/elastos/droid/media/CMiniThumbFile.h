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

#ifndef __ELASTOS_DROID_MEDIA_CMINITHUMBFILE_H__
#define __ELASTOS_DROID_MEDIA_CMINITHUMBFILE_H__

#include "_Elastos_Droid_Media_CMiniThumbFile.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IRandomAccessFile;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMiniThumbFile)
    , public Object
    , public IMiniThumbFile
{
public:
    CMiniThumbFile();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IUri* uri);

    CARAPI Deactivate();

    // Get the magic number for the specified id in the mini-thumb file.
    // Returns 0 if the magic is not available.
    CARAPI GetMagic(
        /* [in] */ Int64 id,
        /* [out] */ Int64* result);

    CARAPI SaveMiniThumbToFile(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int64 id,
        /* [in] */ Int64 magic);

    /**
     * Gallery app can use this method to retrieve mini-thumbnail. Full size
     * images share the same IDs with their corresponding thumbnails.
     *
     * @param id the ID of the image (same of full size image).
     * @param data the buffer to store mini-thumbnail.
     */
    CARAPI GetMiniThumbFromFile(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * We store different types of thumbnails in different files. To remain backward compatibility,
     * we should hashcode of content://media/external/images/media remains the same.
     */
    /*synchronized*/
    static CARAPI Reset();

    /*synchronized*/
    static CARAPI Instance(
        /* [in] */ IUri* uri,
        /* [out] */ IMiniThumbFile** result);

private:
    CARAPI_(String) RandomAccessFilePath(
        /* [in] */ Int32 version);

    CARAPI_(void) RemoveOldFile();

    CARAPI_(AutoPtr<IRandomAccessFile>) MiniThumbDataFile();

private:
    static const String TAG; // = "MiniThumbFile";
    static const Int32 MINI_THUMB_DATA_FILE_VERSION;// = 3;
    static const Int32 HEADER_SIZE;// = 1 + 8 + 4;

    AutoPtr<IUri> mUri;
    AutoPtr<IRandomAccessFile> mMiniThumbFile;
    AutoPtr<IFileChannel> mChannel;
    AutoPtr<IByteBuffer> mBuffer;

    static HashMap< String, AutoPtr<IMiniThumbFile> > sThumbFiles; // = new Hashtable<String, MiniThumbFile>();

    Object mThislock;
    static Object mStaticThislock;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMINITHUMBFILE_H__
