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

#ifndef __ELASTOS_DROID_SETTINGS_ENCRYPTIONINTERSTITIAL_H__
#define __ELASTOS_DROID_SETTINGS_ENCRYPTIONINTERSTITIAL_H__

#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {

class EncryptionInterstitial
    : public SettingsActivity
{
public:
    class EncryptionInterstitialFragment
        : public SettingsPreferenceFragment
        , public IDialogInterfaceOnClickListener
    {
    public:
        class InnerListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            TO_STRING_IMPL("EncryptionInterstitial::EncryptionInterstitialFragment::InnerListener")

            CAR_INTERFACE_DECL()

            InnerListener(
                /* [in] */ EncryptionInterstitialFragment* host);

            //@Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            EncryptionInterstitialFragment* mHost;
        };

    public:
        TO_STRING_IMPL("EncryptionInterstitial::EncryptionInterstitialFragment")

        CAR_INTERFACE_DECL()

        EncryptionInterstitialFragment();

        ~EncryptionInterstitialFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** result);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ Int32 dialogId,
            /* [out] */ IDialog** dialog);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CARAPI_(void) SetRequirePasswordState(
            /* [in] */ Boolean required);

    private:
        static const Int32 ACCESSIBILITY_WARNING_DIALOG;
        AutoPtr<IRadioButton> mRequirePasswordToDecryptButton;
        AutoPtr<IRadioButton> mDontRequirePasswordToDecryptButton;
        AutoPtr<ITextView> mEncryptionMessage;
        Boolean mPasswordRequired;
    };

public:
    EncryptionInterstitial();

    ~EncryptionInterstitial();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    static CARAPI_(AutoPtr<IIntent>) CreateStartIntent(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 quality,
        /* [in] */ Boolean requirePasswordDefault);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    static const String EXTRA_REQUIRE_PASSWORD;

private:
    static const String EXTRA_PASSWORD_QUALITY;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_ENCRYPTIONINTERSTITIAL_H__