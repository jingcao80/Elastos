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

#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODINFO_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_InputMethod_CInputMethodInfo.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodSubtypeArray;

using Elastos::Utility::IMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CInputMethodInfo)
    , public Object
    , public IInputMethodInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputMethodInfo();

    CARAPI constructor();

    /**
     * Constructor.
     *
     * @param context The Context in which we are parsing the input method.
     * @param service The ResolveInfo returned from the package manager about
     * this input method's component.
     */
    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IResolveInfo* service);

    /**
     * Constructor.
     *
     * @param context The Context in which we are parsing the input method.
     * @param service The ResolveInfo returned from the package manager about
     * this input method's component.
     * @param additionalSubtypes additional subtypes being added to this InputMethodInfo
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service,
        /* [in] */ IMap* additionalSubtypesMap);

    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ ICharSequence* label,
        /* [in] */ const String& settingsActivity);

    /**
     * Temporary API for creating a built-in input method for test.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IResolveInfo* ri,
        /* [in] */ Boolean isAuxIme,
        /* [in] */ const String& settingsActivity,
        /* [in] */ IList* subtypes,
        /* [in] */ Int32 isDefaultResId,
        /* [in] */ Boolean forceDefault);

    /**
     * Temporary API for creating a built-in input method for test.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IResolveInfo* ri,
        /* [in] */ Boolean isAuxIme,
        /* [in] */ const String& settingsActivity,
        /* [in] */ IList* subtypes,
        /* [in] */ Int32 isDefaultResId,
        /* [in] */ Boolean forceDefault,
        /* [in] */ Boolean supportsSwitchingToNextInputMethod);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI GetServiceName(
        /* [out] */ String* name);

    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** info);

    CARAPI GetComponent(
        /* [out] */ IComponentName** name);

    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** str);

    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** drawable);

    CARAPI GetSettingsActivity(
        /* [out] */ String* settings);

    CARAPI GetSubtypeCount(
        /* [out] */ Int32* count);

    CARAPI GetSubtypeAt(
        /* [in] */ Int32 index,
        /* [out] */ IInputMethodSubtype** subtype);

    CARAPI GetIsDefaultResourceId(
        /* [out] */ Int32* id);

    /**
     * Return whether or not this ime is a default ime or not.
     * @hide
     */
    CARAPI IsDefault(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsAuxiliaryIme(
        /* [in] */ Boolean* auxIme);

    /**
     * @return true if this input method supports ways to switch to a next input method.
     * @hide
     */
    CARAPI SupportsSwitchingToNextInputMethod(
        /* [out] */ Boolean* supports);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInputMethodInfo* o,
        /* [out] */ Boolean* e);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* e);

private:
    static AutoPtr<IResolveInfo> BuildDummyResolveInfo(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ ICharSequence* label);

private:
    static const String TAG;

    /**
     * The Service that implements this input method component.
     */
    AutoPtr<CResolveInfo> mService;

    /**
     * The unique string Id to identify the input method.  This is generated
     * from the input method component.
     */
    String mId;

    /**
     * The input method setting activity's name, used by the system settings to
     * launch the setting activity of this input method.
     */
    String mSettingsActivityName;

    /**
     * The resource in the input method's .apk that holds a boolean indicating
     * whether it should be considered the default input method for this
     * system.  This is a resource ID instead of the final value so that it
     * can change based on the configuration (in particular locale).
     */
    Int32 mIsDefaultResId;

    /**
     * An array-like container of the subtypes.
     */
    AutoPtr<IInputMethodSubtypeArray> mSubtypes;

    Boolean mIsAuxIme;

    /**
     * Caveat: mForceDefault must be false for production. This flag is only for test.
     */
    Boolean mForceDefault;

    /**
     * The flag whether this IME supports ways to switch to a next input method (e.g. globe key.)
     */
    Boolean mSupportsSwitchingToNextInputMethod;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODINFO_H__
