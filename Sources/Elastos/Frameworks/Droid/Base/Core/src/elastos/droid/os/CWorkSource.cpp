
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CWorkSource.h"
#include <elastos/utility/Arrays.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<IWorkSource> InitsTmpWorkSource()
{
    AutoPtr<IWorkSource> ws;
    CWorkSource::New(0, (IWorkSource**)&ws);
    return ws;
}

const AutoPtr<IWorkSource> CWorkSource::sTmpWorkSource = InitsTmpWorkSource();
AutoPtr<IWorkSource> CWorkSource::sNewbWork;
AutoPtr<IWorkSource> CWorkSource::sGoneWork;

const String CWorkSource::TAG("WorkSource");
const Boolean CWorkSource::DEBUG = FALSE;

CAR_INTERFACE_IMPL_2(CWorkSource, Object, IWorkSource, IParcelable)

CAR_OBJECT_IMPL(CWorkSource)

CWorkSource::CWorkSource()
    : mNum(0)
{}

ECode CWorkSource::constructor()
{
    return NOERROR;
}

ECode CWorkSource::constructor(
    /* [in] */ IWorkSource* orig)
{
    if (orig == NULL) {
        return NOERROR;
    }
    CWorkSource* ws = (CWorkSource*)orig;
    mNum = ws->mNum;
    if (ws->mUids != NULL) {
        mUids = ws->mUids->Clone();
    }

    if (ws->mNames != NULL) {
        mNames = ws->mNames->Clone();
    }
    return NOERROR;
}

ECode CWorkSource::constructor(
    /* [in] */ Int32 uid)
{
    mNum = 1;
    mUids = ArrayOf<Int32>::Alloc(2);
    (*mUids)[0] = uid;
    (*mUids)[1] = 0;
    return NOERROR;
}

ECode CWorkSource::constructor(
    /* [in] */ Int32 uid,
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mNum = 1;
    mUids = ArrayOf<Int32>::Alloc(2);
    (*mUids)[0] = uid;
    (*mUids)[1] = 0;

    mNames = ArrayOf<String>::Alloc(2);
    (*mNames)[0] = name;
    (*mNames)[1] = 0;
    return NOERROR;
}

ECode CWorkSource::GetSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNum;
    return NOERROR;
}

ECode CWorkSource::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (*mUids)[index];
    return NOERROR;
}

ECode CWorkSource::GetName(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    if (mNames != NULL) {
        *name = (*mNames)[index];
    }
    else {
        *name = String(NULL);
    }
    return NOERROR;
}

ECode CWorkSource::ClearNames()
{
    if (mNames != NULL) {
        mNames = NULL;
        // Clear out any duplicate uids now that we don't have names to disambiguate them.
        Int32 destIndex = 1;
        Int32 newNum = mNum;
        for (Int32 sourceIndex = 1; sourceIndex < mNum; sourceIndex++) {
            if ((*mUids)[sourceIndex] == (*mUids)[sourceIndex - 1]) {
                newNum--;
            }
            else {
                (*mUids)[destIndex] = (*mUids)[sourceIndex];
                destIndex++;
            }
        }
        mNum = newNum;
    }
    return NOERROR;
}

ECode CWorkSource::Clear()
{
    mNum = 0;
    return NOERROR;
}

ECode CWorkSource::Equals(
    /* [in] */ IWorkSource* o,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    Boolean difference;
    FAIL_RETURN(Diff(o, &difference));
    *equal = !difference;
    return NOERROR;
}

ECode CWorkSource::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    return Equals(IWorkSource::Probe(o), equal);
}

ECode CWorkSource::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    Int32 result = 0;
    for (Int32 i = 0; i < mNum; i++) {
        result = ((result << 4) | (((Int64)result) >> 28)) ^ (*mUids)[i];
    }
    if (mNames != NULL) {
        for (Int32 i = 0; i < mNum; i++) {
            result = ((result << 4) | (((Int64)result) >> 28)) ^ (*mNames)[i].GetHashCode();
        }
    }
    *hash = result;
    return NOERROR;
}

ECode CWorkSource::Diff(
    /* [in] */ IWorkSource* other,
    /* [out] */ Boolean* difference)
{
    VALIDATE_NOT_NULL(difference);
    CWorkSource* otherObj = (CWorkSource*)other;
    Int32 N = mNum;
    if (N != otherObj->mNum) {
        *difference = TRUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int32> > uids1 = mUids;
    AutoPtr< ArrayOf<Int32> > uids2 = otherObj->mUids;
    AutoPtr< ArrayOf<String> > names1 = mNames;
    AutoPtr< ArrayOf<String> > names2 = otherObj->mNames;
    for (Int32 i = 0; i < N; i++) {
        if ((*uids1)[i] != (*uids2)[i]) {
            *difference = TRUE;
            return NOERROR;
        }
        if (names1 != NULL && names2 != NULL && !(*names1)[i].Equals((*names2)[i])) {
            *difference = TRUE;
            return NOERROR;
        }
    }
    *difference = FALSE;
    return NOERROR;
}

ECode CWorkSource::Set(
    /* [in] */ IWorkSource* otherObj)
{
    if (otherObj == NULL) {
        mNum = 0;
        return NOERROR;
    }
    CWorkSource* other = (CWorkSource*)otherObj;
    mNum = other->mNum;
    if (other->mUids != NULL) {
        if (mUids != NULL && mUids->GetLength() >= mNum) {
            mUids->Copy(other->mUids, mNum);
        }
        else {
            mUids = other->mUids->Clone();
        }
        if (other->mNames != NULL) {
            if (mNames != NULL && mNames->GetLength() >= mNum) {
                mNames->Copy(other->mNames, 0, mNum);
            }
            else {
                mNames = other->mNames->Clone();
            }
        }
        else {
            mNames = NULL;
        }
    }
    else {
        mUids = NULL;
        mNames = NULL;
    }
    return NOERROR;
}

ECode CWorkSource::Set(
    /* [in] */ Int32 uid)
{
    mNum = 1;
    if (mUids == NULL) mUids = ArrayOf<Int32>::Alloc(2);
    (*mUids)[0] = uid;
    mNames = NULL;
    return NOERROR;
}

ECode CWorkSource::Set(
    /* [in] */ Int32 uid,
    /* [in] */ const String& name)
{
    if (name == NULL) {
        // throw new NullPointerException("Name can't be NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    mNum = 1;
    if (mUids == NULL) {
        mUids = ArrayOf<Int32>::Alloc(2);
        mNames = ArrayOf<String>::Alloc(2);
    }
    (*mUids)[0] = uid;
    (*mNames)[0] = name;
    return NOERROR;
}

ECode CWorkSource::SetReturningDiffs(
    /* [in] */ IWorkSource* other,
    /* [out, callee] */ ArrayOf<IWorkSource*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    {
        AutoLock lock(sTmpWorkSource.Get());

        sNewbWork = NULL;
        sGoneWork = NULL;
        Boolean bval;
        UpdateLocked(other, TRUE, TRUE, &bval);
        if (sNewbWork != NULL || sGoneWork != NULL) {
            AutoPtr< ArrayOf<IWorkSource*> > diffs = ArrayOf<IWorkSource*>::Alloc(2);
            diffs->Set(0, sNewbWork);
            diffs->Set(1, sGoneWork);
            *result = diffs;
            REFCOUNT_ADD(*result);
        }
    }

    return NOERROR;
}

ECode CWorkSource::Add(
    /* [in] */ IWorkSource* other,
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);

    AutoLock lock(sTmpWorkSource.Get());
    return UpdateLocked(other, FALSE, FALSE, added);
}

ECode CWorkSource::AddReturningNewbs(
    /* [in] */ IWorkSource* other,
    /* [out] */ IWorkSource** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(sTmpWorkSource.Get());
    sNewbWork = NULL;
    Boolean bval;
    UpdateLocked(other, FALSE, TRUE, &bval);
    *result = sNewbWork;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWorkSource::Add(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);
    *added = FALSE;

    if (mNum <= 0) {
        mNames = NULL;
        Insert(0, uid);
        *added = TRUE;
        return NOERROR;
    }

    if (mNames != NULL) {
        // throw new IllegalArgumentException("Adding without name to named " + this);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 i = 0;
    Arrays::BinarySearch(mUids, 0, mNum, uid, &i);
    // if (DEBUG) Log.d(TAG, "Adding uid " + uid + " to " + this + ": binsearch res = " + i);
    if (i >= 0) {
        return NOERROR;
    }

    Insert(-i - 1, uid);
    *added = TRUE;
    return NOERROR;
}

ECode CWorkSource::Add(
    /* [in] */ Int32 uid,
    /* [in] */ const String& name,
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);
    *added = FALSE;

    if (mNum <= 0) {
        Insert(0, uid, name);
        *added = TRUE;
        return NOERROR;
    }

    if (mNames == NULL) {
        // throw new IllegalArgumentException("Adding name to unnamed " + this);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 i;
    for (i = 0; i < mNum; i++) {
        if ((*mUids)[i] > uid) {
            break;
        }
        if ((*mUids)[i] == uid) {
            Int32 diff = (*mNames)[i].Compare(name);
            if (diff > 0) {
                break;
            }
            if (diff == 0) {
                return NOERROR;
            }
        }
    }

    Insert(i, uid, name);
    *added = TRUE;
    return NOERROR;
}

ECode CWorkSource::AddReturningNewbs(
    /* [in] */ Int32 uid,
    /* [out] */ IWorkSource** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(sTmpWorkSource.Get());
        sNewbWork = NULL;
        ((CWorkSource*)sTmpWorkSource.Get())->mUids->Set(0, uid);
        Boolean bval;
        UpdateLocked(sTmpWorkSource, FALSE, TRUE, &bval);
        *result = sNewbWork;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

ECode CWorkSource::Remove(
    /* [in] */ IWorkSource* otherObj,
    /* [out] */ Boolean* successed)
{
    VALIDATE_NOT_NULL(successed);
    *successed = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    if (mNum <= 0 || other->mNum <= 0) {
        return NOERROR;
    }
    if (mNames == NULL && other->mNames == NULL) {
        return RemoveUids(otherObj, successed);
    }

    if (mNames == NULL) {
        // throw new IllegalArgumentException("Other " + other + " has names, but target "
        //         + this + " does not");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (other->mNames == NULL) {
        // throw new IllegalArgumentException("Target " + this + " has names, but other "
        //         + other + " does not");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return RemoveUidsAndNames(otherObj, successed);
}

ECode CWorkSource::StripNames(
    /* [out] */ IWorkSource** workSource)
{
    VALIDATE_NOT_NULL(workSource)
    if (mNum <= 0) {
        return CWorkSource::New(workSource);
    }

    AutoPtr<IWorkSource> result;
    CWorkSource::New((IWorkSource**)&result);
    Boolean bval;
    Int32 lastUid = -1;
    for (Int32 i = 0; i < mNum; i++) {
        Int32 uid = (*mUids)[i];
        if (i == 0 || lastUid != uid) {
            result->Add(uid, &bval);
        }
    }

    *workSource = result;
    REFCOUNT_ADD(*workSource)
    return NOERROR;
}

ECode CWorkSource::RemoveUids(
    /* [in] */ IWorkSource* otherObj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    Int32 N1 = mNum;
    ArrayOf<Int32>* uids1 = mUids;
    Int32 N2 = other->mNum;
    ArrayOf<Int32>* uids2 = other->mUids;
    Boolean changed = FALSE;
    Int32 i1 = 0, i2 = 0;
    // if (DEBUG) Log.d(TAG, "Remove " + other + " from " + this);
    while (i1 < N1 && i2 < N2) {
        // if (DEBUG) Log.d(TAG, "Step: target @ " + i1 + " of " + N1 + ", other @ " + i2
        //         + " of " + N2);
        if ((*uids2)[i2] == (*uids1)[i1]) {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1
            //         + ": remove " + uids1[i1]);
            N1--;
            changed = TRUE;
            if (i1 < N1) {
                uids1->Copy(i1, uids1, i1 + 1, N1 - i1);
            }
            i2++;
        }
        else if ((*uids2)[i2] > (*uids1)[i1]) {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip i1");
            i1++;
        }
        else {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip i2");
            i2++;
        }
    }

    mNum = N1;

    *result = changed;
    return NOERROR;
}

ECode CWorkSource::RemoveUidsAndNames(
    /* [in] */ IWorkSource* otherObj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    Int32 N1 = mNum;
    ArrayOf<Int32>* uids1 = mUids;
    ArrayOf<String>* names1 = mNames;
    Int32 N2 = other->mNum;
    ArrayOf<Int32>* uids2 = other->mUids;
    ArrayOf<String>* names2 = other->mNames;
    Boolean changed = FALSE;
    Int32 i1 = 0, i2 = 0;
    // if (DEBUG) Log.d(TAG, "Remove " + other + " from " + this);
    while (i1 < N1 && i2 < N2) {
        // if (DEBUG) Log.d(TAG, "Step: target @ " + i1 + " of " + N1 + ", other @ " + i2
        //         + " of " + N2 + ": " + uids1[i1] + " " + names1[i1]);
        if ((*uids2)[i2] == (*uids1)[i1] && (*names2)[i2].Equals((*names1)[i1])) {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1
            //         + ": remove " + uids1[i1] + " " + names1[i1]);
            N1--;
            changed = TRUE;
            if (i1 < N1) {
                uids1->Copy(i1, uids1, i1 + 1, N1-i1);
                names1->Copy(i1, names1, i1 + 1, N1-i1);
            }
            i2++;
        }
        else if ((*uids2)[i2] > (*uids1)[i1]
                || ((*uids2)[i2] == (*uids1)[i1]
                    && (*names2)[i2].Compare((*names1)[i1]) > 0)) {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip i1");
            i1++;
        }
        else {
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip i2");
            i2++;
        }
    }

    mNum = N1;

    *result = changed;
    return NOERROR;
}

ECode CWorkSource::UpdateLocked(
    /* [in] */ IWorkSource* otherObj,
    /* [in] */ Boolean set,
    /* [in] */ Boolean returnNewbs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    if (mNames == NULL && other->mNames == NULL) {
        return UpdateUidsLocked(otherObj, set, returnNewbs, result);
    }

    if (mNum > 0 && mNames == NULL) {
        // throw new IllegalArgumentException("Other " + other + " has names, but target "
        //         + this + " does not");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (other->mNum > 0 && other->mNames == NULL) {
        // throw new IllegalArgumentException("Target " + this + " has names, but other "
        //         + other + " does not");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return UpdateUidsAndNamesLocked(otherObj, set, returnNewbs, result);
}

AutoPtr<IWorkSource> CWorkSource::AddWork(
    /* [in] */ IWorkSource* cur,
    /* [in] */ Int32 newUid)
{
    AutoPtr<IWorkSource> workSource;
    if (cur == NULL) {
        CWorkSource::New(newUid, (IWorkSource**)&workSource);
        return workSource;
    }

    CWorkSource* other = (CWorkSource*)cur;
    other->Insert(other->mNum, newUid);
    return cur;
}

ECode CWorkSource::UpdateUidsLocked(
    /* [in] */ IWorkSource* otherObj,
    /* [in] */ Boolean set,
    /* [in] */ Boolean returnNewbs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    Int32 N1 = mNum;
    ArrayOf<Int32>* uids1 = mUids;
    Int32 N2 = other->mNum;
    ArrayOf<Int32>* uids2 = other->mUids;
    Boolean changed = FALSE;
    Int32 i1 = 0, i2 = 0;
    // if (DEBUG) Log.d(TAG, "Update " + this + " with " + other + " set=" + set
    //         + " returnNewbs=" + returnNewbs);
    while (i1 < N1 || i2 < N2) {
        // if (DEBUG) Log.d(TAG, "Step: target @ " + i1 + " of " + N1 + ", other @ " + i2
        //         + " of " + N2);
        if (i1 >= N1 || (i2 < N2 && (*uids2)[i2] < (*uids1)[i1])) {
            // Need to Insert a new uid.
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1
            //         + ": Insert " + uids2[i2]);
            changed = TRUE;
            if (uids1 == NULL) {
                uids1 = ArrayOf<Int32>::Alloc(4);
                (*uids1)[0] = (*uids2)[i2];
            }
            else if (N1 >= uids1->GetLength()) {
                AutoPtr<ArrayOf<Int32> > newuids = ArrayOf<Int32>::Alloc((uids1->GetLength()*3)/2);
                if (i1 > 0) newuids->Copy(uids1, 0, i1);
                if (i1 < N1) newuids->Copy(i1 + 1, uids1, i1, N1 - i1);
                uids1 = newuids;
                (*uids1)[i1] = (*uids2)[i2];
            }
            else {
                if (i1 < N1) uids1->Copy(i1 + 1, uids1, i1, N1 - i1);
                (*uids1)[i1] = (*uids2)[i2];
            }
            if (returnNewbs) {
                sNewbWork = AddWork(sNewbWork, (*uids2)[i2]);
            }
            N1++;
            i1++;
            i2++;
        }
        else {
            if (!set) {
                // Skip uids that already exist or are not in 'other'.
                // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip");
                if (i2 < N2 && (*uids2)[i2] == (*uids1)[i1]) {
                    i2++;
                }
                i1++;
            } else {
                // Remove any uids that don't exist in 'other'.
                Int32 start = i1;
                while (i1 < N1 && (i2 >= N2 || (*uids2)[i2] > (*uids1)[i1])) {
                    // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + ": remove " + (*uids1)[i1]);
                    sGoneWork = AddWork(sGoneWork, (*uids1)[i1]);
                    i1++;
                }
                if (start < i1) {
                    uids1->Copy(start, uids1, i1, N1 - i1);
                    N1 -= i1-start;
                    i1 = start;
                }
                // If there is a matching uid, skip it.
                if (i1 < N1 && i2 < N2 && (*uids2)[i2] == (*uids1)[i1]) {
                    // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + N1 + ": skip");
                    i1++;
                    i2++;
                }
            }
        }
    }

    mNum = N1;
    mUids = uids1;

    *result = changed;
    return NOERROR;
}

Int32 CWorkSource::Compare(
    /* [in] */ IWorkSource* otherObj,
    /* [in] */ Int32 i1,
    /* [in] */ Int32 i2)
{
    CWorkSource* other = (CWorkSource*)otherObj;
    Int32 diff = (*mUids)[i1] - (*other->mUids)[i2];
    if (diff != 0) {
        return diff;
    }
    return (*mNames)[i1].Compare((*other->mNames)[i2]);
}

AutoPtr<IWorkSource> CWorkSource::AddWork(
    /* [in] */ IWorkSource* cur,
    /* [in] */ Int32 newUid,
    /* [in] */ const String& newName)
{
    if (cur == NULL) {
        AutoPtr<CWorkSource> workSource;
        CWorkSource::NewByFriend((CWorkSource**)&workSource);
        workSource->constructor(newUid, newName);
        return workSource;
    }

    CWorkSource* other = (CWorkSource*)cur;
    other->Insert(other->mNum, newUid, newName);
    return cur;
}

ECode CWorkSource::UpdateUidsAndNamesLocked(
    /* [in] */ IWorkSource* otherObj,
    /* [in] */ Boolean set,
    /* [in] */ Boolean returnNewbs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CWorkSource* other = (CWorkSource*)otherObj;
    Int32 N2 = other->mNum;
    ArrayOf<Int32>* uids2 = other->mUids;
    ArrayOf<String>* names2 = other->mNames;
    Boolean changed = FALSE;
    Int32 i1 = 0, i2 = 0;
    // if (DEBUG) Log.d(TAG, "Update " + this + " with " + other + " set=" + set
    //         + " returnNewbs=" + returnNewbs);
    while (i1 < mNum || i2 < N2) {
        // if (DEBUG) Log.d(TAG, "Step: target @ " + i1 + " of " + mNum + ", other @ " + i2
        //         + " of " + N2);
        Int32 diff = -1;
        if (i1 >= mNum || (i2 < N2 && (diff = Compare(other, i1, i2)) > 0)) {
            // Need to Insert a new uid.
            changed = TRUE;
            // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + mNum
            //         + ": Insert " + uids2[i2] + " " + names2[i2]);
            Insert(i1, (*uids2)[i2], (*names2)[i2]);
            if (returnNewbs) {
                sNewbWork = AddWork(sNewbWork, (*uids2)[i2], (*names2)[i2]);
            }
            i1++;
            i2++;
        } else {
            if (!set) {
                // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + mNum + ": skip");
                if (i2 < N2 && diff == 0) {
                    i2++;
                }
                i1++;
            }
            else {
                // Remove any uids that don't exist in 'other'.
                Int32 start = i1;
                while (diff < 0) {
                    // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + ": remove " + mUids[i1]
                    //         + " " + mNames[i1]);
                    sGoneWork = AddWork(sGoneWork, (*mUids)[i1], (*mNames)[i1]);
                    i1++;
                    if (i1 >= mNum) {
                        break;
                    }
                    diff = i2 < N2 ? Compare(other, i1, i2) : -1;
                }
                if (start < i1) {
                    mUids->Copy(start, mUids, i1, mNum - i1);
                    mNames->Copy(start, mNames, i1, mNum - i1);
                    mNum -= i1-start;
                    i1 = start;
                }
                // If there is a matching uid, skip it.
                if (i1 < mNum && diff == 0) {
                    // if (DEBUG) Log.d(TAG, "i1=" + i1 + " i2=" + i2 + " N1=" + mNum + ": skip");
                    i1++;
                    i2++;
                }
            }
        }
    }

    *result = changed;
    return NOERROR;
}

ECode CWorkSource::Insert(
    /* [in] */ Int32 index,
    /* [in] */ Int32 uid)
{
    // if (DEBUG) Log.d(TAG, "Insert in " + this + " @ " + index + " uid " + uid);
    if (mUids == NULL) {
        mUids = ArrayOf<Int32>::Alloc(4);
        (*mUids)[0] = uid;
        mNum = 1;
    }
    else if (mNum >= mUids->GetLength()) {
        ArrayOf<Int32>* newuids = ArrayOf<Int32>::Alloc((mNum*3)/2);
        if (index > 0) {
            newuids->Copy(mUids, 0, index);
        }
        if (index < mNum) {
            newuids->Copy(index + 1, mUids, index, mNum - index);
        }
        mUids = newuids;
        (*mUids)[index] = uid;
        mNum++;
    }
    else {
        if (index < mNum) {
            mUids->Copy(index + 1, mUids, index, mNum - index);
        }
        (*mUids)[index] = uid;
        mNum++;
    }
    return NOERROR;
}

ECode CWorkSource::Insert(
    /* [in] */ Int32 index,
    /* [in] */ Int32 uid,
    /* [in] */ const String& name)
{
    if (mUids == NULL) {
        mUids = ArrayOf<Int32>::Alloc(4);
        (*mUids)[0] = uid;
        mNames = ArrayOf<String>::Alloc(4);
        (*mNames)[0] = name;
        mNum = 1;
    }
    else if (mNum >= mUids->GetLength()) {
        AutoPtr<ArrayOf<Int32> > newuids = ArrayOf<Int32>::Alloc((mNum*3)/2);
        AutoPtr<ArrayOf<String> > newnames = ArrayOf<String>::Alloc((mNum*3)/2);
        if (index > 0) {
            newuids->Copy(mUids, 0, index);
            newnames->Copy(mNames, 0, index);
        }
        if (index < mNum) {
            newuids->Copy(index + 1, mUids, index, mNum - index);
            newnames->Copy(index + 1, mNames, index, mNum - index);
        }
        mUids = newuids;
        mNames = newnames;
        (*mUids)[index] = uid;
        (*mNames)[index] = name;
        mNum++;
    }
    else {
        if (index < mNum) {
            mUids->Copy(index + 1, mUids, index, mNum - index);
            mNames->Copy(index + 1, mNames, index, mNum - index);
        }
        (*mUids)[index] = uid;
        (*mNames)[index] = name;
        mNum++;
    }
    return NOERROR;
}

ECode CWorkSource::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder result;
    result.Append("WorkSource{");
    for (Int32 i = 0; i < mNum; i++) {
        if (i != 0) {
            result.Append(", ");
        }
        result.Append((*mUids)[i]);
        if (mNames != NULL) {
            result.Append(" ");
            result.Append((*mNames)[i]);
        }
    }
    result.Append("}");
    *str = result.ToString();
    return NOERROR;
}

ECode CWorkSource::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNum);
    source->ReadArrayOf((Handle32*)&mUids);
    source->ReadArrayOfString((ArrayOf<String>**)&mNames);
    return NOERROR;
}

ECode CWorkSource::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNum);
    dest->WriteArrayOf((Handle32)mUids.Get());
    dest->WriteArrayOfString(mNames);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
