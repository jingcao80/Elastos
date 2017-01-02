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

#ifndef __ELASTOS_DROID_OS_CWORKSOURCE_H__
#define __ELASTOS_DROID_OS_CWORKSOURCE_H__

#include "_Elastos_Droid_Os_CWorkSource.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Describes the source of some work that may be done by someone else.
 * Currently the public representation of what a work source is is not
 * defined; this is an opaque container.
 */
CarClass(CWorkSource)
    , public Object
    , public IWorkSource
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWorkSource();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWorkSource* orig);

    CARAPI constructor(
        /* [in] */ Int32 uid);

    CARAPI constructor(
        /* [in] */ Int32 uid,
        /* [in] */ const String& name);

    CARAPI GetSize(
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI GetName(
        /* [in] */ Int32 index,
        /* [out] */ String* name);

    CARAPI ClearNames();

    /**
     * Clear names from this WorkSource.  Uids are left intact.
     *
     * <p>Useful when combining with another WorkSource that doesn't have names.
     * @hide
     */

    /**
     * Clear this WorkSource to be empty.
     */
    CARAPI Clear();

    CARAPI Equals(
        /* [in] */ IWorkSource* o,
        /* [out] */ Boolean* equal);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    /**
     * Compare this WorkSource with another.
     * @param other The WorkSource to compare against.
     * @return If there is a difference, true is returned.
     */
    CARAPI Diff(
        /* [in] */ IWorkSource* other,
        /* [out] */ Boolean* difference);

    /**
     * Replace the current contents of this work source with the given
     * work source.  If <var>other</var> is null, the current work source
     * will be made empty.
     */
    CARAPI Set(
        /* [in] */ IWorkSource* other);

    /** @hide */
    CARAPI Set(
        /* [in] */ Int32 uid);

    CARAPI Set(
        /* [in] */ Int32 index,
        /* [in] */ const String& name);

    /** @hide */
    CARAPI SetReturningDiffs(
        /* [in] */ IWorkSource* other,
        /* [out, callee] */ ArrayOf<IWorkSource*>** result);

    /**
     * Merge the contents of <var>other</var> WorkSource in to this one.
     *
     * @param other The other WorkSource whose contents are to be merged.
     * @return Returns true if any new sources were added.
     */
    CARAPI Add(
        /* [in] */ IWorkSource* other,
        /* [out] */ Boolean* added);

    /** @hide */
    CARAPI AddReturningNewbs(
        /* [in] */ IWorkSource* other,
        /* [out] */ IWorkSource** result);

    /** @hide */
    CARAPI Add(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* added);

    /** @hide */
    CARAPI Add(
        /* [in] */ Int32 uid,
        /* [in] */ const String& name,
        /* [out] */ Boolean* added);

    /** @hide */
    CARAPI AddReturningNewbs(
        /* [in] */ Int32 uid,
        /* [out] */ IWorkSource** result);

    CARAPI Remove(
        /* [in] */ IWorkSource* other,
        /* [out] */ Boolean* successed);

    /** @hide */
    CARAPI StripNames(
        /* [out] */ IWorkSource** other);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI UpdateLocked(
        /* [in] */ IWorkSource* other,
        /* [in] */ Boolean set,
        /* [in] */ Boolean returnNewbs,
        /* [out] */ Boolean* result);

    CARAPI RemoveUids(
        /* [in] */ IWorkSource* other,
        /* [out] */ Boolean* result);

    CARAPI RemoveUidsAndNames(
        /* [in] */ IWorkSource* other,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IWorkSource>) AddWork(
        /* [in] */ IWorkSource* cur,
        /* [in] */ Int32 newUid);

    CARAPI UpdateUidsLocked(
        /* [in] */ IWorkSource* other,
        /* [in] */ Boolean set,
        /* [in] */ Boolean returnNewbs,
        /* [out] */ Boolean* result);

    CARAPI_(Int32) Compare(
        /* [in] */ IWorkSource* other,
        /* [in] */ Int32 i1,
        /* [in] */ Int32 i2);

    CARAPI_(AutoPtr<IWorkSource>) AddWork(
        /* [in] */ IWorkSource* cur,
        /* [in] */ Int32 newUid,
        /* [in] */ const String& newName);

    CARAPI UpdateUidsAndNamesLocked(
        /* [in] */ IWorkSource* other,
        /* [in] */ Boolean set,
        /* [in] */ Boolean returnNewbs,
        /* [out] */ Boolean* result);

    CARAPI Insert(
        /* [in] */ Int32 index,
        /* [in] */ Int32 uid);

    CARAPI Insert(
        /* [in] */ Int32 index,
        /* [in] */ Int32 uid,
        /* [in] */ const String& name);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static const String TAG;// = "WorkSource";
    static const Boolean DEBUG;// = false;

    Int32 mNum;
    AutoPtr< ArrayOf<Int32> > mUids;
    AutoPtr< ArrayOf<String> > mNames;

    /**
     * Internal statics to avoid object allocations in some operations.
     * The WorkSource object itself is not thread safe, but we need to
     * hold sTmpWorkSource lock while working with these statics.
     */
    static const AutoPtr<IWorkSource> sTmpWorkSource;

    /**
     * For returning newbie work from a modification operation.
     */
    static AutoPtr<IWorkSource> sNewbWork;
    /**
     * For returning gone work form a modification operation.
     */
    static AutoPtr<IWorkSource> sGoneWork;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CWORKSOURCE_H__
