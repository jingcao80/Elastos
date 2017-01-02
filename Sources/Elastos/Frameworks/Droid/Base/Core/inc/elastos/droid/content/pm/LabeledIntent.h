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

#ifndef __ELASTOS_DROID_CONTENT_PM_LABELEDINTENT_H__
#define __ELASTOS_DROID_CONTENT_PM_LABELEDINTENT_H__

#include "elastos/droid/content/Intent.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
* A special subclass of Intent that can have a custom label/icon
* associated with it.  Primarily for use with {@link Intent#ACTION_CHOOSER}.
 */
class LabeledIntent
    : public Intent
    , public ILabeledIntent
{
public:
    CAR_INTERFACE_DECL()

    LabeledIntent();

    virtual ~LabeledIntent();

    CARAPI constructor();

    /**
     * Create a labeled intent from the given intent, supplying the label
     * and icon resources for it.
     *
     * @param origIntent The original Intent to copy.
     * @param sourcePackage The package in which the label and icon live.
     * @param labelRes Resource containing the label, or 0 if none.
     * @param icon Resource containing the icon, or 0 if none.
     */
    CARAPI constructor(
        /* [in] */ IIntent* origIntent,
        /* [in] */ const String& sourcePackage,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon);

    /**
     * Create a labeled intent from the given intent, supplying a textual
     * label and icon resource for it.
     *
     * @param origIntent The original Intent to copy.
     * @param sourcePackage The package in which the label and icon live.
     * @param nonLocalizedLabel Concrete text to use for the label.
     * @param icon Resource containing the icon, or 0 if none.
     */
    CARAPI constructor(
        /* [in] */ IIntent* origIntent,
        /* [in] */ const String& sourcePackage,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 icon);

    /**
     * Create a labeled intent with no intent data but supplying the label
     * and icon resources for it.
     *
     * @param sourcePackage The package in which the label and icon live.
     * @param labelRes Resource containing the label, or 0 if none.
     * @param icon Resource containing the icon, or 0 if none.
     */
    CARAPI constructor(
        /* [in] */ const String& sourcePackage,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon);

    /**
     * Create a labeled intent with no intent data but supplying a textual
     * label and icon resource for it.
     *
     * @param sourcePackage The package in which the label and icon live.
     * @param nonLocalizedLabel Concrete text to use for the label.
     * @param icon Resource containing the icon, or 0 if none.
     */
    CARAPI constructor(
        /* [in] */ const String& sourcePackage,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 icon);

    /**
     * Return the name of the package holding label and icon resources.
     */
    CARAPI GetSourcePackage(
        /* [out] */ String* sourcePackage);

    /**
     * Return any resource identifier that has been given for the label text.
     */
    CARAPI GetLabelResource(
        /* [out] */ Int32* labelRes);

    /**
     * Return any concrete text that has been given for the label text.
     */
    CARAPI GetNonLocalizedLabel(
        /* [out] */ ICharSequence** label);

    /**
     * Return any resource identifier that has been given for the label icon.
     */
    CARAPI GetIconResource(
        /* [out] */ Int32* iconRes);

    /**
     * Retrieve the label associated with this object.  If the object does
     * not have a label, null will be returned, in which case you will probably
     * want to load the label from the underlying resolved info for the Intent.
     */
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Retrieve the icon associated with this object.  If the object does
     * not have a icon, null will be returned, in which case you will probably
     * want to load the icon from the underlying resolved info for the Intent.
     */
    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mSourcePackage;

    Int32 mLabelRes;

    AutoPtr<ICharSequence> mNonLocalizedLabel;

    Int32 mIcon;

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_LABELEDINTENT_H__
