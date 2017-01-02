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

#ifndef __ELASTOS_DROID_MTP_MTPSTORAGE_H__
#define __ELASTOS_DROID_MTP_MTPSTORAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Mtp {

class MtpStorage
    : public Object
    , public IMtpStorage
{
public:
    MtpStorage();

    virtual ~MtpStorage();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IStorageVolume* volume,
        /* [in] */ IContext* context);

    CARAPI GetStorageId(
        /* [out] */ Int32* result);

    static CARAPI GetStorageId(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI GetPath(
        /* [out] */ String* result);

    CARAPI GetDescription(
        /* [out] */ String* result);

    CARAPI GetReserveSpace(
        /* [out] */ Int64* result);

    CARAPI IsRemovable(
        /* [out] */ Boolean* result);

    CARAPI GetMaxFileSize(
        /* [out] */ Int64* result);

private:
    Int32 mStorageId;
    String mPath;
    String mDescription;
    Int64 mReserveSpace;
    Boolean mRemovable;
    Int64 mMaxFileSize;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_MTPSTORAGE_H__
