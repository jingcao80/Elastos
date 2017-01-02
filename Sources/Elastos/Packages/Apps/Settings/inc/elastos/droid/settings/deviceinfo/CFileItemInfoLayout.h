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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CFILEITEMINFOLAYOUT_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CFILEITEMINFOLAYOUT_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CFileItemInfoLayout.h"
#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Droid::Widget::ICheckable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Handles display of a single row entry on Settings --> Storage --> Misc Files screen
 */
CarClass(CFileItemInfoLayout)
    , public RelativeLayout
    , public ICheckable
{
public:
    TO_STRING_IMPL("CFileItemInfoLayout")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFileItemInfoLayout();

    ~CFileItemInfoLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI Toggle();

    CARAPI SetFileName(
        /* [in] */ const String& fileName);

    CARAPI SetFileSize(
        /* [in] */ const String& filesize);

    //@ViewDebug.ExportedProperty
    //@Override
    CARAPI IsChecked(
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr<ICheckBox>) GetCheckBox();

    /**
     * <p>Changes the checked state of this text view.</p>
     *
     * @param checked TRUE to check the text, FALSE to uncheck it
     */
    //@Override
    CARAPI SetChecked(
        /* [in] */ Boolean checked);

protected:
    /* (non-Javadoc)
     * @see android.view.View#OnFinishInflate()
     */
    //@Override
    CARAPI OnFinishInflate();

private:
    AutoPtr<ITextView> mFileNameView;
    AutoPtr<ITextView> mFileSizeView;
    AutoPtr<ICheckBox> mCheckbox;

    static const Int32 sLengthExternalStorageDirPrefix;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CFILEITEMINFOLAYOUT_H__
