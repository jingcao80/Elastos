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

#ifndef __ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODSUBTYPESWITCHINGCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODSUBTYPESWITCHINGCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparable;
using Elastos::Core::IComparator;
using Elastos::Utility::ITreeMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

class InputMethodSubtypeSwitchingController
    : public Object
    , public IInputMethodSubtypeSwitchingController
{
public:
    class ImeSubtypeListItem
        : public Object
        , public IImeSubtypeListItem
        , public IComparable
    {
    public:
        ImeSubtypeListItem(
            /* [in] */ ICharSequence* imeName,
            /* [in] */ ICharSequence* subtypeName,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ Int32 subtypeId,
            /* [in] */ const String& subtypeLocale,
            /* [in] */ const String& systemLocale);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI CompareTo(
            /* [in] */ IInterface* _other,
            /* [out] */ Int32* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetImeName(
            /* [out] */ ICharSequence** imeName);

        CARAPI SetImeName(
            /* [in] */ ICharSequence* imeName);

        CARAPI GetSubtypeName(
            /* [out] */ ICharSequence** subtypeName);

        CARAPI SetSubtypeName(
            /* [in] */ ICharSequence* subtypeName);

        CARAPI GetImi(
            /* [out] */ IInputMethodInfo** imi);

        CARAPI SetImi(
            /* [in] */ IInputMethodInfo* imi);

        CARAPI GetSubtypeId(
            /* [out] */ Int32* subtypeId);

        CARAPI SetSubtypeId(
            /* [in] */ Int32 subtypeId);

        CARAPI GetIsSystemLocale(
            /* [out] */ Boolean* isSystemLocale);

        CARAPI SetIsSystemLocale(
            /* [in] */ Boolean isSystemLocale);

        CARAPI GetIsSystemLanguage(
            /* [out] */ Boolean* isSystemLanguage);

        CARAPI SetIsSystemLanguage(
            /* [in] */ Boolean isSystemLanguage);

    private:
        /**
         * Returns the language component of a given locale string.
         * TODO: Use {@link Locale#getLanguage()} instead.
         */
        static CARAPI_(String) ParseLanguageFromLocaleString(
            /* [in] */ const String& locale);

    public:
        AutoPtr<ICharSequence> mImeName;
        AutoPtr<ICharSequence> mSubtypeName;
        AutoPtr<IInputMethodInfo> mImi;
        Int32 mSubtypeId;
        Boolean mIsSystemLocale;
        Boolean mIsSystemLanguage;
    };

private:
    class InputMethodAndSubtypeList : public Object
    {
    private:
        class Comparator
            : public Object
            , public IComparator
        {
        public:
            Comparator(
                /* [in] */ IPackageManager* pm);

            CAR_INTERFACE_DECL()

            // @Override
            CARAPI Compare(
                /* [in] */ IInterface* _imi1,
                /* [in] */ IInterface* _imi2,
                /* [out] */ Int32* result);

        private:
            AutoPtr<IPackageManager> mPm;
        };

    public:
        InputMethodAndSubtypeList(
            /* [in] */ IContext* context,
            /* [in] */ IInputMethodSettings* settings);

        CARAPI_(AutoPtr<IList>) GetSortedInputMethodAndSubtypeList();

        CARAPI_(AutoPtr<IList>) GetSortedInputMethodAndSubtypeList(
            /* [in] */ Boolean showSubtypes,
            /* [in] */ Boolean inputShown,
            /* [in] */ Boolean isScreenLocked);

    private:
        AutoPtr<IContext> mContext;
        // Used to load label
        AutoPtr<IPackageManager> mPm;
        String mSystemLocaleStr;
        AutoPtr<IInputMethodSettings> mSettings;
        AutoPtr<ITreeMap> mSortedImmis;
    };

    class ControllerImpl;

    class StaticRotationList : public Object
    {
        friend class InputMethodSubtypeSwitchingController::ControllerImpl;
    public:
        StaticRotationList(
            /* [in] */ IList* imeSubtypeList);

        CARAPI_(AutoPtr<ImeSubtypeListItem>) GetNextInputMethodLocked(
            /* [in] */ Boolean onlyCurrentIme,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        /**
         * Returns the index of the specified input method and subtype in the given list.
         * @param imi The {@link InputMethodInfo} to be searched.
         * @param subtype The {@link InputMethodSubtype} to be searched. NULL if the input method
         * does not have a subtype.
         * @return The index in the given list. -1 if not found.
         */
        CARAPI_(Int32) GetIndex(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        AutoPtr<IList> mImeSubtypeList;
    };

    class DynamicRotationList : public Object
    {
        friend class InputMethodSubtypeSwitchingController::ControllerImpl;
    public:
        DynamicRotationList(
            /* [in] */ IList* imeSubtypeListItems);


        CARAPI_(void) OnUserAction(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

        CARAPI_(AutoPtr<ImeSubtypeListItem>) GetNextInputMethodLocked(
            /* [in] */ Boolean onlyCurrentIme,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        /**
         * Returns the index of the specified object in
         * {@link #mUsageHistoryOfSubtypeListItemIndex}.
         * <p>We call the index of {@link #mUsageHistoryOfSubtypeListItemIndex} as "Usage Rank"
         * so as not to be confused with the index in {@link #mImeSubtypeList}.
         * @return -1 when the specified item doesn't belong to {@link #mImeSubtypeList} actually.
         */
        CARAPI_(Int32) GetUsageRank(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        static const String TAG;
        AutoPtr<IList> mImeSubtypeList;
        AutoPtr<ArrayOf<Int32> > mUsageHistoryOfSubtypeListItemIndex;
    };

    // @VisibleForTesting
    class ControllerImpl : public Object
    {
    public:
        static CARAPI_(AutoPtr<ControllerImpl>) CreateFrom(
            /* [in] */ ControllerImpl* currentInstance,
            /* [in] */ IList* sortedEnabledItems);

        CARAPI_(AutoPtr<ImeSubtypeListItem>) GetNextInputMethod(
            /* [in] */ Boolean onlyCurrentIme,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

        CARAPI_(void) OnUserActionLocked(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        ControllerImpl(
            /* [in] */ DynamicRotationList* switchingAwareRotationList,
            /* [in] */ StaticRotationList* switchingUnawareRotationList);

        static CARAPI_(AutoPtr<IList>) FilterImeSubtypeList(
            /* [in] */ IList* items,
            /* [in] */ Boolean supportsSwitchingToNextInputMethod);

    private:
        AutoPtr<DynamicRotationList> mSwitchingAwareRotationList;
        AutoPtr<StaticRotationList> mSwitchingUnawareRotationList;
    };


public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IInputMethodSubtypeSwitchingController>) CreateInstanceLocked(
        /* [in] */ IInputMethodSettings* settings,
        /* [in] */ IContext* context);

    CARAPI OnUserActionLocked(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI ResetCircularListLocked(
        /* [in] */ IContext* context);

    CARAPI GetNextInputMethodLocked(
        /* [in] */ Boolean onlyCurrentIme,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ IImeSubtypeListItem** listItem);

    CARAPI GetSortedInputMethodAndSubtypeListLocked(
        /* [in] */ Boolean showSubtypes,
        /* [in] */ Boolean inputShown,
        /* [in] */ Boolean isScreenLocked,
        /* [out] */ IList** list);

private:
    static CARAPI_(Int32) CalculateSubtypeId(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    InputMethodSubtypeSwitchingController(
        /* [in] */ IInputMethodSettings* settings,
        /* [in] */ IContext* context);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 NOT_A_SUBTYPE_ID;

    AutoPtr<IInputMethodSettings> mSettings;
    AutoPtr<InputMethodAndSubtypeList> mSubtypeList;
    AutoPtr<ControllerImpl> mController;
};

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODSUBTYPESWITCHINGCONTROLLER_H__
