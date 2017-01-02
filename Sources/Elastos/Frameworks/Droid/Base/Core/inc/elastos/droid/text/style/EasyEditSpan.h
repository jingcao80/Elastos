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

#ifndef __ELASTOS_DROID_TEXT_STYLE_EasyEditSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_EasyEditSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Provides an easy way to edit a portion of text.
 * <p>
 * The {@link TextView} uses this span to allow the user to delete a chuck of text in one click.
 * <p>
 * {@link TextView} removes the span when the user deletes the whole text or modifies it.
 * <p>
 * This span can be also used to receive notification when the user deletes or modifies the text;
 */
class EasyEditSpan
    : public Object
    , public IEasyEditSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    EasyEditSpan();

    virtual ~EasyEditSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPendingIntent* intent);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    /**
     * @return True if the {@link TextView} should offer the ability to delete the text.
     *
     * @hide
     */
    CARAPI IsDeleteEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Enables or disables the deletion of the text.
     *
     * @hide
     */
    CARAPI SetDeleteEnabled(
        /* [in] */ Boolean value);

    /**
     * @return the pending intent to send when the wrapped text is deleted or modified.
     *
     * @hide
     */
    CARAPI GetPendingIntent(
        /* [out] */ IPendingIntent** intent);

private:
    AutoPtr<IPendingIntent> mPendingIntent;

    Boolean mDeleteEnabled;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_EasyEditSpan_H__
