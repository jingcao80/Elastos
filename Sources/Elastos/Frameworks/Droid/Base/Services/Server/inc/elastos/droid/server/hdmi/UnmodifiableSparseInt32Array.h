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

#ifndef __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__
#define __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Unmodifiable version of {@link SparseIntArray}.
 */
class UnmodifiableSparseInt32Array
    : public Object
{
public:
    CARAPI constructor(
        /* [in] */ ISparseInt32Array* array);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ Int32 valueIfKeyNotFound,
        /* [out] */ Int32* result);

    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI IndexOfValue(
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static const String TAG;

    AutoPtr<ISparseInt32Array> mArray;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__
