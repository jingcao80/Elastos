
#include "elastos/droid/database/MergeCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {

MergeCursor::MyDataSetObserver::MyDataSetObserver(
    /* [in] */ MergeCursor* host)
    : mHost(host)
{}

ECode MergeCursor::MyDataSetObserver::OnChanged()
{
    mHost->mPos = -1;
    return NOERROR;
}

ECode MergeCursor::MyDataSetObserver::OnInvalidated()
{
    mHost->mPos = -1;
    return NOERROR;
}

CAR_INTERFACE_IMPL(MergeCursor, AbstractCursor, IMergeCursor);

MergeCursor::MergeCursor()
{
    mObserver = new MyDataSetObserver(this);
}

ECode MergeCursor::constructor(
    /* [in] */ ArrayOf<ICursor*>* cursors)
{
    mCursors = cursors;
    mCursor = (*cursors)[0];
    for (Int32 i = 0; i < mCursors->GetLength(); i++) {
        if ((*mCursors)[i] == NULL) continue;
        (*mCursors)[i]->RegisterDataSetObserver(mObserver);
    }
    return NOERROR;
}

ECode MergeCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 mcount = 0;
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            Int32 c;
            (*mCursors)[i]->GetCount(&c);
            mcount += c;
        }
    }
    *count = mcount;
    return NOERROR;
}

ECode MergeCursor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    /* Find the right cursor */
    mCursor = NULL;
    Int32 cursorStartPos = 0;
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length; i++) {
        if ((*mCursors)[i] == NULL) {
            continue;
        }

        Int32 c;
        (*mCursors)[i]->GetCount(&c);
        if (newPosition < (cursorStartPos + c)) {
            mCursor = (*mCursors)[i];
            break;
        }

        cursorStartPos += c;
    }

    /* Move it to the right position */
    if (mCursor != NULL) {
        return mCursor->MoveToPosition(newPosition - cursorStartPos, succeeded);
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode MergeCursor::GetString(
    /* [in] */ Int32 column,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return mCursor->GetString(column, str);
}

ECode MergeCursor::GetInt16(
    /* [in] */ Int32 column,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt16(column, value);
}

ECode MergeCursor::GetInt32(
    /* [in] */ Int32 column,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt32(column, value);
}

ECode MergeCursor::GetInt64(
    /* [in] */ Int32 column,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt64(column, value);
}

ECode MergeCursor::GetFloat(
    /* [in] */ Int32 column,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetFloat(column, value);
}

ECode MergeCursor::GetDouble(
    /* [in] */ Int32 column,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetDouble(column, value);
}

ECode MergeCursor::GetType(
    /* [in] */ Int32 column,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    return mCursor->GetType(column, type);
}

ECode MergeCursor::IsNull(
    /* [in] */ Int32 column,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->IsNull(column, value);
}

ECode MergeCursor::GetBlob(
    /* [in] */ Int32 column,
    /* [out] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    return mCursor->GetBlob(column, blob);
}

ECode MergeCursor::GetColumnNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    if (mCursor != NULL) {
        return mCursor->GetColumnNames(names);
    }
    else {
        *names = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*names)
        return NOERROR;
    }
}

ECode MergeCursor::Deactivate()
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->Deactivate();
        }
    }
    return AbstractCursor::Deactivate();
}

ECode MergeCursor::Close()
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length; i++) {
        if ((*mCursors)[i] == NULL) continue;
        ICloseable::Probe((*mCursors)[i])->Close();
    }
    return AbstractCursor::Close();
}

ECode MergeCursor::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->RegisterContentObserver(observer);
        }
    }
    return NOERROR;
}

ECode MergeCursor::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->UnregisterContentObserver(observer);
        }
    }
    return NOERROR;
}

ECode MergeCursor::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->RegisterDataSetObserver(observer);
        }
    }
    return NOERROR;
}

ECode MergeCursor::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->UnregisterDataSetObserver(observer);
        }
    }
    return NOERROR;
}

ECode MergeCursor::Requery(
    /* [out] */ Boolean* value)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length; i++) {
        if ((*mCursors)[i] == NULL) {
            continue;
        }

        Boolean v;
        if ((*mCursors)[i]->Requery(&v), v == FALSE) {
            *value = FALSE;
            return NOERROR;
        }
    }

    *value = TRUE;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
