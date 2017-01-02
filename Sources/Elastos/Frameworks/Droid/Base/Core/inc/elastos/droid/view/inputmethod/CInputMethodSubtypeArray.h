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

#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEARRAY_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEARRAY_H__

#include "_Elastos_Droid_View_InputMethod_CInputMethodSubtypeArray.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

/**
 * An array-like container that stores multiple instances of {@link InputMethodSubtype}.
 *
 * <p>This container is designed to reduce the risk of {@link TransactionTooLargeException}
 * when one or more instancess of {@link InputMethodInfo} are transferred through IPC.
 * Basically this class does following three tasks.</p>
 * <ul>
 * <li>Applying compression for the marshalled data</li>
 * <li>Lazily unmarshalling objects</li>
 * <li>Caching the marshalled data when appropriate</li>
 * </ul>
 *
 * @hide
 */
CarClass(CInputMethodSubtypeArray)
    , public Object
    , public IInputMethodSubtypeArray
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Create a new instance of {@link InputMethodSubtypeArray} from an existing list of
     * {@link InputMethodSubtype}.
     *
     * @param subtypes A list of {@link InputMethodSubtype} from which
     * {@link InputMethodSubtypeArray} will be created.
     */
    CARAPI constructor(
        /* [in] */ IList* subtypes);

    /**
     * Unmarshall an instance of {@link InputMethodSubtypeArray} from a given {@link Parcel}
     * object.
     *
     * @param source A {@link Parcel} object from which {@link InputMethodSubtypeArray} will be
     * unmarshalled.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Marshall the instance into a given {@link Parcel} object.
     *
     * <p>This methods may take a bit additional time to compress data lazily when called
     * first time.</p>
     *
     * @param source A {@link Parcel} object to which {@link InputMethodSubtypeArray} will be
     * marshalled.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Return {@link InputMethodSubtype} specified with the given index.
     *
     * <p>This methods may take a bit additional time to decompress data lazily when called
     * first time.</p>
     *
     * @param index The index of {@link InputMethodSubtype}.
     */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ IInputMethodSubtype** result);

    /**
     * Return the number of {@link InputMethodSubtype} objects.
     */
    CARAPI GetCount(
        /* [out] */ Int32* result);

private:
    static AutoPtr<ArrayOf<Byte> > Marshall(
        /* [in] */ ArrayOf<IInputMethodSubtype*>* array);

    static AutoPtr<ArrayOf<IInputMethodSubtype*> > Unmarshall(
        /* [in] */ ArrayOf<Byte>* data);

    static AutoPtr<ArrayOf<Byte> > Compress(
        /* [in] */ ArrayOf<Byte>* data);

    static AutoPtr<ArrayOf<Byte> > Decompress(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 expectedSize);

private:
    static const String TAG;

    Object mLockObject;
    Int32 mCount;

    /* volatile */ AutoPtr<ArrayOf<IInputMethodSubtype*> > mInstance;
    /* volatile */ AutoPtr<ArrayOf<Byte> > mCompressedData;
    /* volatile */ Int32 mDecompressedSize;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEARRAY_H__
