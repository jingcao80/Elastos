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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATION_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATION_H__

#include "_Elastos_Droid_Content_CContentProviderOperation.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderOperation)
    , public Object
    , public IContentProviderOperation
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderOperation();

    virtual ~CContentProviderOperation();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContentProviderOperationBuilder* operationBuilder);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContentProviderOperation* cpo,
        /* [in] */ Boolean removeUserIdFromUri);

    /** @hide */
    CARAPI GetWithoutUserIdInUri(
        /* [out] */ IContentProviderOperation** op);

    /**
     * Create a {@link Builder} suitable for building an insert {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the insert.
     * @return a {@link Builder}
     */
    static AutoPtr<IContentProviderOperationBuilder> NewInsert(
        /* [in] */ IUri* uri);

    /**
     * Create a {@link Builder} suitable for building an update {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the update.
     * @return a {@link Builder}
     */
    static AutoPtr<IContentProviderOperationBuilder> NewUpdate(
        /* [in] */ IUri* uri);

    /**
     * Create a {@link Builder} suitable for building a delete {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the delete.
     * @return a {@link Builder}
     */
    static AutoPtr<IContentProviderOperationBuilder> NewDelete(
        /* [in] */ IUri* uri);

    /**
     * Create a {@link Builder} suitable for building a
     * {@link ContentProviderOperation} to assert a set of values as provided
     * through {@link Builder#withValues(ContentValues)}.
     */
    static AutoPtr<IContentProviderOperationBuilder> NewAssertQuery(
        /* [in] */ IUri* uri);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI IsYieldAllowed(
        /* [out] */ Boolean* isAllowed);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI IsDeleteOperation(
        /* [out] */ Boolean* resutl);

    CARAPI IsWriteOperation(
        /* [out] */ Boolean* isWrite);

    CARAPI IsReadOperation(
        /* [out] */ Boolean* isRead);

    /**
     * Applies this operation using the given provider. The backRefs array is used to resolve any
     * back references that were requested using
     * {@link Builder#withValueBackReferences(ContentValues)} and
     * {@link Builder#withSelectionBackReference}.
     * @param provider the {@link ContentProvider} on which this batch is applied
     * @param backRefs a {@link ContentProviderResult} array that will be consulted
     * to resolve any requested back references.
     * @param numBackRefs the number of valid results on the backRefs array.
     * @return a {@link ContentProviderResult} that contains either the {@link Uri} of the inserted
     * row if this was an insert otherwise the number of rows affected.
     * @throws OperationApplicationException thrown if either the insert fails or
     * if the number of rows affected didn't match the expected count
     */
    CARAPI Apply(
        /* [in] */ IContentProvider* provider,
        /* [in] */ ArrayOf<IContentProviderResult *>* backRefs,
        /* [in] */ Int32 numBackRefs,
        /* [out] */ IContentProviderResult** providerResult);

    /**
     * The ContentValues back references are represented as a ContentValues object where the
     * key refers to a column and the value is an index of the back reference whose
     * valued should be associated with the column.
     * <p>
     * This is intended to be a private method but it is exposed for
     * unit testing purposes
     * @param backRefs an array of previous results
     * @param numBackRefs the number of valid previous results in backRefs
     * @return the ContentValues that should be used in this operation application after
     * expansion of back references. This can be called if either mValues or mValuesBackReferences
     * is null
     */
    CARAPI ResolveValueBackReferences(
        /* [in] */ ArrayOf<IContentProviderResult *>* backRefs,
        /* [in] */ Int32 numBackRefs,
        /* [out] */ IContentValues** contentValues);

    /**
     * The Selection Arguments back references are represented as a Map of Integer->Integer where
     * the key is an index into the selection argument array (see {@link Builder#withSelection})
     * and the value is the index of the previous result that should be used for that selection
     * argument array slot.
     * <p>
     * This is intended to be a private method but it is exposed for
     * unit testing purposes
     * @param backRefs an array of previous results
     * @param numBackRefs the number of valid previous results in backRefs
     * @return the ContentValues that should be used in this operation application after
     * expansion of back references. This can be called if either mValues or mValuesBackReferences
     * is null
     */
    CARAPI ResolveSelectionArgsBackReferences(
        /* [in] */ ArrayOf<IContentProviderResult *>* backRefs,
        /* [in] */ Int32 numBackRefs,
        /* [out, callee] */ ArrayOf<String>** stringArray);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /**
     * Return the string representation of the requested back reference.
     * @param backRefs an array of results
     * @param numBackRefs the number of items in the backRefs array that are valid
     * @param backRefIndex which backRef to be used
     * @throws ArrayIndexOutOfBoundsException thrown if the backRefIndex is larger than
     * the numBackRefs
     * @return the string representation of the requested back reference.
     */
    CARAPI BackRefToValue(
        /* [in] */ ArrayOf<IContentProviderResult*>* backRefs,
        /* [in] */ const Int32 numBackRefs,
        /* [in] */ const Int32 backRefIndex,
        /* [out] */ Int64* backRefValue);

private:
    static const String TAG;

    Int32 mType;
    AutoPtr<IUri> mUri;
    String mSelection;
    AutoPtr<ArrayOf<String> > mSelectionArgs;
    AutoPtr<IContentValues> mValues;
    Int32 mExpectedCount;
    AutoPtr<IContentValues> mValuesBackReferences;
    AutoPtr<IHashMap> mSelectionArgsBackReferences;
    Boolean mYieldAllowed;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATION_H__
