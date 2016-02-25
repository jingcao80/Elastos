
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CContentProviderOperationBuilder.h"
#include "elastos/droid/content/CContentProviderOperation.h"
#include "elastos/droid/content/CContentValues.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IByte;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::EIID_IByte;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentProviderOperationBuilder, Object, IContentProviderOperationBuilder)

CAR_OBJECT_IMPL(CContentProviderOperationBuilder)

CContentProviderOperationBuilder::CContentProviderOperationBuilder()
    : mType(0)
    , mExpectedCount(0)
    , mYieldAllowed(FALSE)
{}

CContentProviderOperationBuilder::~CContentProviderOperationBuilder()
{
}

ECode CContentProviderOperationBuilder::constructor(
    /* [in] */ Int32 type,
    /* [in] */ IUri* uri)
{
    VALIDATE_NOT_NULL(uri)
    mType = type;
    mUri = uri;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::Build(
    /* [out] */ IContentProviderOperation** providerOperation)
{
    VALIDATE_NOT_NULL(providerOperation)

    Int32 size = 0;
    if (mType == IContentProviderOperation::TYPE_UPDATE) {
        if ((NULL == mValues || (mValues->GetSize(&size), size) == 0)
                && (NULL == mValuesBackReferences || (mValuesBackReferences->GetSize(&size), size) == 0)) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (mType == IContentProviderOperation::TYPE_ASSERT) {
        if ((NULL == mValues || (mValues->GetSize(&size), size) == 0)
                && (NULL == mValuesBackReferences || (mValuesBackReferences->GetSize(&size), size) == 0)
                && (mExpectedCount == 0)) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    FAIL_RETURN(CContentProviderOperation::New(THIS_PROBE(IContentProviderOperationBuilder), providerOperation))
    return NOERROR;
}


ECode CContentProviderOperationBuilder::WithValueBackReferences(
    /* [in] */ IContentValues* backReferences)
{
    VALIDATE_NOT_NULL(backReferences)

    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have value back-references");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mValuesBackReferences = backReferences;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithValueBackReference(
    /* [in] */ const String& key,
    /* [in] */ Int32 previousResult)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have value back-references");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (NULL == mValuesBackReferences) {
        FAIL_RETURN(CContentValues::New((IContentValues**)&mValuesBackReferences));
    }

    return mValuesBackReferences->Put(key, previousResult);
}

ECode CContentProviderOperationBuilder::WithSelectionBackReference(
    /* [in] */ Int32 selectionArgIndex,
    /* [in] */ Int32 previousResult)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_DELETE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, deletes, and asserts can have selection back-references");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (NULL == mSelectionArgsBackReferences) {
        CHashMap::New((IHashMap**)&mSelectionArgsBackReferences);
    }

    AutoPtr<IInteger32> selection = CoreUtils::Convert(selectionArgIndex);
    AutoPtr<IInteger32> previous = CoreUtils::Convert(previousResult);
    mSelectionArgsBackReferences->Put(TO_IINTERFACE(selection), TO_IINTERFACE(previous));
    return NOERROR;
}

ECode CContentProviderOperationBuilder::CreateContentValueIfNull()
{
    if (NULL == mValues) {
        FAIL_RETURN(CContentValues::New((IContentValues**)&mValues));
    }
    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithValues(
    /* [in] */ IContentValues* values)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    FAIL_RETURN(mValues->PutAll(values))
    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ IInterface* value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    if (NULL == value) {
        FAIL_RETURN(mValues->PutNull(key))
    }
    else if (ICharSequence::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, ICharSequence::Probe(value)))
    }
    else if (IByte::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IByte::Probe(value)))
    }
    else if (IInteger16::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IInteger16::Probe(value)))
    }
    else if (IInteger32::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IInteger32::Probe(value)))
    }
    else if (IInteger64::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IInteger64::Probe(value)))
    }
    else if (IFloat::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IFloat::Probe(value)))
    }
    else if (IDouble::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IDouble::Probe(value)))
    }
    else if (IBoolean::Probe(value) != NULL) {
        FAIL_RETURN(mValues->Put(key, IBoolean::Probe(value)))
    }
    else if (IArrayOf::Probe(value) != NULL) {
        InterfaceID elemType;
        IArrayOf::Probe(value)->GetTypeId(&elemType);
        if (elemType == EIID_IByte) {
            FAIL_RETURN(mValues->Put(key, IArrayOf::Probe(value)))
        }
        else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithByte(
    /* [in] */ const String& key,
    /* [in] */ Byte value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->PutByte(key, value);
}

ECode CContentProviderOperationBuilder::WithBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ Int16 value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ Float value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ Double value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithValue(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_UPDATE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CreateContentValueIfNull())

    return mValues->Put(key, value);
}

ECode CContentProviderOperationBuilder::WithSelection(
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_DELETE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, deletes, and asserts can have selections");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSelection = selection;
    if (NULL == selectionArgs) {
        mSelectionArgs = NULL;
    }
    else {
        mSelectionArgs = ArrayOf<String>::Alloc(selectionArgs->GetLength());
        mSelectionArgs->Copy(selectionArgs);
    }

    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithExpectedCount(
    /* [in] */ Int32 count)
{
    if (mType != IContentProviderOperation::TYPE_INSERT
        && mType != IContentProviderOperation::TYPE_DELETE
        && mType != IContentProviderOperation::TYPE_ASSERT) {
        Logger::E("CContentProviderOperationBuilder", "only inserts, updates, and asserts can have expected counts");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mExpectedCount = count;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::WithYieldAllowed(
    /* [in] */ Boolean yieldAllowed)
{
    mYieldAllowed = yieldAllowed;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetSelection(
    /* [out] */ String* selection)
{
    VALIDATE_NOT_NULL(selection)
    *selection = mSelection;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetSelectionArgs(
    /* [out, callee] */ ArrayOf<String>** selectionArgs)
{
    VALIDATE_NOT_NULL(selectionArgs)
    *selectionArgs = mSelectionArgs;
    REFCOUNT_ADD(*selectionArgs)
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetValues(
    /* [out] */ IContentValues** values)
{
    VALIDATE_NOT_NULL(values)
    *values = mValues;
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetExpectedCount(
    /* [out] */ Int32* expectedCount)
{
    VALIDATE_NOT_NULL(expectedCount)
    *expectedCount = mExpectedCount;
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetValuesBackReferences(
    /* [out] */ IContentValues** valuesBackReferences)
{
    VALIDATE_NOT_NULL(valuesBackReferences)
    *valuesBackReferences = mValuesBackReferences;
    REFCOUNT_ADD(*valuesBackReferences)
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetSelectionArgsBackReferences(
    /* [out] */ IHashMap** selectionArgsBackRef)
{
    VALIDATE_NOT_NULL(selectionArgsBackRef)
    *selectionArgsBackRef = mSelectionArgsBackReferences;
    REFCOUNT_ADD(*selectionArgsBackRef)
    return NOERROR;
}

ECode CContentProviderOperationBuilder::GetYieldAllowed(
    /* [out] */ Boolean* yieldAllowed)
{
    VALIDATE_NOT_NULL(yieldAllowed)
    *yieldAllowed = mYieldAllowed;
    return NOERROR;
}

}
}
}

