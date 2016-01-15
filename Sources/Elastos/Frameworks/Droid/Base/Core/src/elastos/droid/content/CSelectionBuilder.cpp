
#include "elastos/droid/content/CSelectionBuilder.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CSelectionBuilder::CSelectionBuilder()
    : mSelection(NULL)
{
    mSelection = new StringBuilder();
}

CSelectionBuilder::~CSelectionBuilder()
{
    mSelectionArgs.Clear();
}

ECode CSelectionBuilder::Reset()
{
    if (NULL != mSelection) mSelection->Reset();
     mSelectionArgs.Clear();
    return NOERROR;
}

ECode CSelectionBuilder::Append(
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<IInterface*>* selectionArgs)
{
    if (selection.IsNullOrEmpty()) {
        if (NULL != selectionArgs && selectionArgs->GetLength() > 0) {
            //throw new IllegalArgumentException("Valid selection required when including arguments");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // Shortcut when clause is empty
        return NOERROR;
    }

    if (mSelection->GetLength() > 0) {
        *mSelection += " AND ";
    }

    *mSelection += "(";
    *mSelection += selection;
    *mSelection += ")";
    if (NULL != selectionArgs) {
        // for (Object arg : selectionArgs) {
        //     // TODO: switch to storing direct Object instances once
        //     // http://b/2464440 is fixed
        //     mSelectionArgs.add(String.valueOf(arg));
        // }
        const Int32 N = selectionArgs->GetLength();
        AutoPtr<IInterface> arg;
        for (Int32 i = 0; i < N; i++) {
            // TODO: switch to storing direct Object instances once
            // http://b/2464440 is fixed
            arg = (*selectionArgs)[i];
            if (ICharSequence::Probe(arg) != NULL) {
                String tmp;
                FAIL_RETURN(((ICharSequence*) (IInterface*) arg)->ToString(&tmp))
                mSelectionArgs.PushBack(tmp);
            }
            else if (IInteger64::Probe(arg) != NULL) {
                Int64 tmp = 0;
                FAIL_RETURN(((IInteger64*) (IInterface*) arg)->GetValue(&tmp))
                mSelectionArgs.PushBack(StringUtils::Int64ToString(tmp));
            }
            else if (IInteger32::Probe(arg) != NULL) {
                Int32 tmp = 0;
                FAIL_RETURN(((IInteger32*) (IInterface*) arg)->GetValue(&tmp))
                mSelectionArgs.PushBack(StringUtils::Int32ToString(tmp));
            }
            else if (IBoolean::Probe(arg) != NULL) {
                Boolean tmp = FALSE;
                FAIL_RETURN(((IBoolean*) (IInterface*) arg)->GetValue(&tmp))
                mSelectionArgs.PushBack(StringUtils::BooleanToString(tmp));
            }
            else if (IFloat::Probe(arg) != NULL) {
                Float tmp = 0.0f;
                FAIL_RETURN(((IFloat*) (IInterface*) arg)->GetValue(&tmp))
                mSelectionArgs.PushBack(StringUtils::DoubleToString((Double)tmp));
            }
            else if (IDouble::Probe(arg) != NULL) {
                Double tmp = 0.0;
                FAIL_RETURN(((IDouble*) (IInterface*) arg)->GetValue(&tmp))
                mSelectionArgs.PushBack(StringUtils::DoubleToString(tmp));
            }
            else {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode CSelectionBuilder::GetSelection(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return mSelection->ToString(str);
}

ECode CSelectionBuilder::GetSelectionArgs(
    /* [out, callee] */ ArrayOf<String>** selectionArgs)
{
    VALIDATE_NOT_NULL(selectionArgs)
    *selectionArgs = NULL;
    if (!mSelectionArgs.IsEmpty()) {
        AutoPtr<ArrayOf<String> > tmpArray = ArrayOf<String>::Alloc(mSelectionArgs.GetSize());
        List<String>::Iterator it = mSelectionArgs.Begin();
        for (Int32 index = 0; it != mSelectionArgs.End(); it++, index++) {
            (*tmpArray)[index] = *it;
        }
        *selectionArgs = tmpArray;
        REFCOUNT_ADD(*selectionArgs);
    }
    return NOERROR;
}

ECode CSelectionBuilder::Query(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return Query(db, table, columns, String(NULL), String(NULL), orderBy, String(NULL), cursor);
}

ECode CSelectionBuilder::Query(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(db)
    VALIDATE_NOT_NULL(cursor)
    String selection;
    FAIL_RETURN(GetSelection(&selection))
    AutoPtr<ArrayOf<String> > selectionArgs;
    FAIL_RETURN(GetSelectionArgs((ArrayOf<String>**)&selectionArgs))
    return db->Query(table, columns, selection, selectionArgs, groupBy, having,
            orderBy, limit, cursor);
}

ECode CSelectionBuilder::Update(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ IContentValues* values,
    /* [out] */ Int32* affectedRow)
{
    VALIDATE_NOT_NULL(db)
    VALIDATE_NOT_NULL(affectedRow)
    String selection;
    FAIL_RETURN(GetSelection(&selection))
    AutoPtr<ArrayOf<String> > selectionArgs;
    FAIL_RETURN(GetSelectionArgs((ArrayOf<String>**)&selectionArgs))
    return db->Update(table, values, selection, selectionArgs, affectedRow);
}

ECode CSelectionBuilder::Delete(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [out] */ Int32* affectedRow)
{
    VALIDATE_NOT_NULL(db)
    VALIDATE_NOT_NULL(affectedRow)
    String selection;
    FAIL_RETURN(GetSelection(&selection))
    AutoPtr<ArrayOf<String> > selectionArgs;
    FAIL_RETURN(GetSelectionArgs((ArrayOf<String>**)&selectionArgs))
    return db->Delete(table, selection, selectionArgs, affectedRow);
}

}
}
}
}

