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

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERINFO_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERINFO_H__

#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_View_TextService_CSpellCheckerInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;

using Elastos::Core::CString;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

/**
 * This class is used to specify meta information of a spell checker.
 */
CarClass(CSpellCheckerInfo)
    , public Object
    , public ISpellCheckerInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSpellCheckerInfo();

    CARAPI constructor();

    /**
     * Constructor.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service);

    /**
     * Return a unique ID for this spell checker.  The ID is generated from
     * the package and class name implementing the method.
     */
    CARAPI GetId(
        /* [out] */ String* id);

    /**
     * Return the component of the service that implements.
     */
    CARAPI GetComponent(
        /* [out] */ IComponentName** name);

    /**
     * Return the .apk package that implements this.
     */
    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    /**
     * Used to package this object into a {@link Parcel}.
     *
     * @param dest The {@link Parcel} to be written.
     * @param flags The flags used for parceling.
     */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Load the user-displayed label for this spell checker.
     *
     * @param pm Supply a PackageManager used to load the spell checker's resources.
     */
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** lable);

    /**
     * Load the user-displayed icon for this spell checker.
     *
     * @param pm Supply a PackageManager used to load the spell checker's resources.
     */
    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** drawable);

    /**
     * Return the raw information about the Service implementing this
     * spell checker.  Do not modify the returned object.
     */
    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** info);

    /**
     * Return the class name of an activity that provides a settings UI.
     * You can launch this activity be starting it with
     * an {@link android.content.Intent} whose action is MAIN and with an
     * explicit {@link android.content.ComponentName}
     * composed of {@link #getPackageName} and the class name returned here.
     *
     * <p>A null will be returned if there is no settings activity.
     */
    CARAPI GetSettingsActivity(
        /* [out] */ String* name);

    /**
     * Return the count of the subtypes.
     */
    CARAPI GetSubtypeCount(
        /* [out] */ Int32* count);

    /**
     * Return the subtype at the specified index.
     *
     * @param index the index of the subtype to return.
     */
    CARAPI GetSubtypeAt(
        /* [in] */ Int32 index,
        /* [out] */ ISpellCheckerSubtype** subtype);

private:
    static const CString TAG;
    AutoPtr<IResolveInfo> mService;
    String mId;
    Int32 mLabel;

    /**
     * The spell checker setting activity's name, used by the system settings to
     * launch the setting activity.
     */
    String mSettingsActivityName;

    /**
     * The array of subtypes.
     */
    AutoPtr<IArrayList> mSubtypes;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERINFO_H__
