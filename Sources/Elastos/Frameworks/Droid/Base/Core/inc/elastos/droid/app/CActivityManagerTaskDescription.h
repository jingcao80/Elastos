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

#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_App_CActivityManagerTaskDescription.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can set and retrieve about the current activity within the recent task list.
 */
CarClass(CActivityManagerTaskDescription)
    , public Object
    , public IActivityManagerTaskDescription
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerTaskDescription();

    CARAPI constructor();

    /**
     * Creates the TaskDescription to the specified values.
     *
     * @param label A label and description of the current state of this task.
     * @param icon An icon that represents the current state of this task.
     * @param colorPrimary A color to override the theme's primary color.  This color must be opaque.
     */
    CARAPI constructor(
        /* [in] */ const String& label,
        /* [in] */ IBitmap* icon,
        /* [in] */ Int32 colorPrimary);

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& label,
        /* [in] */ Int32 colorPrimary,
        /* [in] */ const String& iconFilename);

    /**
     * Creates the TaskDescription to the specified values.
     *
     * @param label A label and description of the current state of this activity.
     * @param icon An icon that represents the current state of this activity.
     */
    CARAPI constructor(
        /* [in] */ const String& label,
        /* [in] */ IBitmap* icon);

    /**
     * Creates the TaskDescription to the specified values.
     *
     * @param label A label and description of the current state of this activity.
     */
    CARAPI constructor(
        /* [in] */ const String& label);

    /**
     * Creates an empty TaskDescription.
     */
    CARAPI constructor(
        /* [in] */ IActivityManagerTaskDescription* td);

    /**
     * Sets the label for this task description.
     * @hide
     */
    CARAPI SetLabel(
        /* [in] */ const String& label);

    /**
     * Sets the primary color for this task description.
     * @hide
     */
    CARAPI SetPrimaryColor(
        /* [in] */ Int32 primaryColor);

    /**
     * Sets the icon for this task description.
     * @hide
     */
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon);

    /**
     * Moves the icon bitmap reference from an actual Bitmap to a file containing the
     * bitmap.
     * @hide
     */
    CARAPI SetIconFilename(
        /* [in] */ const String& iconFilename);

    /**
     * @return The label and description of the current state of this task.
     */
    CARAPI GetLabel(
        /* [out] */ String* label);

    /**
     * @return The icon that represents the current state of this task.
     */
    CARAPI GetIcon(
        /* [out] */ IBitmap** bm);

    /** @hide */
    CARAPI GetIconFilename(
        /* [out] */ String* name);

    /** @hide */
    CARAPI GetInMemoryIcon(
        /* [out] */ IBitmap** bm);

    /**
     * @return The color override on the theme's primary color.
     */
    CARAPI GetPrimaryColor(
        /* [out] */ Int32* color);

    /** @hide */
    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* xs);

    /** @hide */
    CARAPI RestoreFromXml(
        /* [in] */ const String& attrName,
        /* [in] */ const String& attrValue);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI LoadTaskDescriptionIcon(
        /* [in] */ const String& iconFilename,
        /* [out] */ IBitmap** bm);

public:
    /** @hide */
    static const String ATTR_TASKDESCRIPTION_PREFIX;

private:

    static const String ATTR_TASKDESCRIPTIONLABEL;
    static const String ATTR_TASKDESCRIPTIONCOLOR;
    static const String ATTR_TASKDESCRIPTIONICONFILENAME;

    String mLabel;
    AutoPtr<IBitmap> mIcon;
    String mIconFilename;
    Int32 mColorPrimary;

};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_H__
