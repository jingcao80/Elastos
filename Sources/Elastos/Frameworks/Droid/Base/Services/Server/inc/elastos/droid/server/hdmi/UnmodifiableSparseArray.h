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

#ifndef __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__
#define __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Unmodifiable version of {@link SparseArray}.
 */
class UnmodifiableSparseArray
    : public Object
{
public:
    CARAPI constructor(
        /* [in] */ ISparseArray* array);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* valueIfKeyNotFound,
        /* [out] */ IInterface** result);

    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** result);

    CARAPI IndexOfValue(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static const String TAG;

    AutoPtr<ISparseArray> mArray;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__
