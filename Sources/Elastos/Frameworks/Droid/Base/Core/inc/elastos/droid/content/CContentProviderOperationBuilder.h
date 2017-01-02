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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONBUILDER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONBUILDER_H__

#include "_Elastos_Droid_Content_CContentProviderOperationBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderOperationBuilder)
    , public Object
    , public IContentProviderOperationBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderOperationBuilder();

    virtual ~CContentProviderOperationBuilder();

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ IUri* uri);

    /** Create a ContentProviderOperation from this {@link Builder}. */
    CARAPI Build(
        /* [out] */ IContentProviderOperation** providerOperation);

    /**
     * Add a {@link ContentValues} of back references. The key is the name of the column
     * and the value is an integer that is the index of the previous result whose
     * value should be used for the column. The value is added as a {@link String}.
     * A column value from the back references takes precedence over a value specified in
     * {@link #withValues}.
     * This can only be used with builders of type insert, update, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithValueBackReferences(
        /* [in] */ IContentValues* backReferences);

    /**
     * Add a ContentValues back reference.
     * A column value from the back references takes precedence over a value specified in
     * {@link #withValues}.
     * This can only be used with builders of type insert, update, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithValueBackReference(
        /* [in] */ const String& key,
        /* [in] */ Int32 previousResult);

    /**
     * Add a back references as a selection arg. Any value at that index of the selection arg
     * that was specified by {@link #withSelection} will be overwritten.
     * This can only be used with builders of type update, delete, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithSelectionBackReference(
        /* [in] */ Int32 selectionArgIndex,
        /* [in] */ Int32 previousResult);

    /**
     * The ContentValues to use. This may be null. These values may be overwritten by
     * the corresponding value specified by {@link #withValueBackReference} or by
     * future calls to {@link #withValues} or {@link #withValue}.
     * This can only be used with builders of type insert, update, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithValues(
        /* [in] */ IContentValues* values);

    /**
     * A value to insert or update. This value may be overwritten by
     * the corresponding value specified by {@link #withValueBackReference}.
     * This can only be used with builders of type insert, update, or assert.
     * @param key the name of this value
     * @param value the value itself. the type must be acceptable for insertion by
     * {@link ContentValues#put}
     * @return this builder, to allow for chaining.
     */
    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ IInterface* value);

    CARAPI WithByte(
        /* [in] */ const String& key,
        /* [in] */ Byte value);

    CARAPI WithBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ Int16 value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ Int64 value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ Float value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ Double value);

    CARAPI WithValue(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value);

    /**
     * The selection and arguments to use. An occurrence of '?' in the selection will be
     * replaced with the corresponding occurence of the selection argument. Any of the
     * selection arguments may be overwritten by a selection argument back reference as
     * specified by {@link #withSelectionBackReference}.
     * This can only be used with builders of type update, delete, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithSelection(
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * If set then if the number of rows affected by this operation do not match
     * this count {@link OperationApplicationException} will be throw.
     * This can only be used with builders of type update, delete, or assert.
     * @return this builder, to allow for chaining.
     */
    CARAPI WithExpectedCount(
        /* [in] */ Int32 count);

    CARAPI WithYieldAllowed(
        /* [in] */ Boolean yieldAllowed);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetSelection(
        /* [out] */ String* selection);

    CARAPI GetSelectionArgs(
        /* [out, callee] */ ArrayOf<String>** selectionArgs);

    CARAPI GetValues(
        /* [out] */ IContentValues** values);

    CARAPI GetExpectedCount(
        /* [out] */ Int32* expectedCount);

    CARAPI GetValuesBackReferences(
        /* [out] */ IContentValues** valuesBackReferences);

    CARAPI GetSelectionArgsBackReferences(
        /* [out] */ IHashMap** selectionArgsBackRef);

    CARAPI GetYieldAllowed(
        /* [out] */ Boolean* yieldAllowed);

private:
    CARAPI CreateContentValueIfNull();

private:
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

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONBUILDER_H__
