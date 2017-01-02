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

#include "elastos/droid/settings/CMediaFormat.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "R.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Os::Storage::IExternalStorageFormatter;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::LayoutInflater;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CMediaFormat::InitViewOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CMediaFormat::InitViewOnClickListener, Object, IViewOnClickListener)

CMediaFormat::InitViewOnClickListener::InitViewOnClickListener(
    /* [in] */ CMediaFormat* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CMediaFormat::InitViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0:{
            if (Utils::IsMonkeyRunning()) {
                return NOERROR;
            }
            AutoPtr<IIntent> intent;
            CIntent::New(IExternalStorageFormatter::FORMAT_ONLY, (IIntent**)&intent);
            assert(0 && "TODO");
            // AutoPtr<IExternalStorageFormatterHelper> helper;
            // CExternalStorageFormatterHelper::AcquireSingleton((IExternalStorageFormatterHelper**)&helper);
            AutoPtr<IComponentName> comp;
            // helper->GetCOMPONENT_NAME((IComponentName**)&comp);
            intent->SetComponent(comp);
            // Transfer the storage volume to the new intent
            AutoPtr<IIntent> tmpIntent;
            mHost->GetIntent((IIntent**)&tmpIntent);
            AutoPtr<IParcelable> parcelable;
            tmpIntent->GetParcelableExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, (IParcelable**)&parcelable);
            IStorageVolume* storageVolume = IStorageVolume::Probe(parcelable);
            intent->PutExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, IParcelable::Probe(storageVolume));
            AutoPtr<IComponentName> compName;
            mHost->StartService(intent, (IComponentName**)&compName);
            mHost->Finish();
            break;
        }
        case 1:
            if (!mHost->RunKeyguardConfirmation(KEYGUARD_REQUEST)) {
                mHost->EstablishFinalConfirmationState();
            }
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CMediaFormat
//===============================================================================

const Int32 CMediaFormat::KEYGUARD_REQUEST;

CAR_OBJECT_IMPL(CMediaFormat)

CMediaFormat::CMediaFormat()
{}

CMediaFormat::~CMediaFormat()
{}

ECode CMediaFormat::constructor()
{
    mFinalClickListener = new InitViewOnClickListener(this, 0);
    mInitiateListener = new InitViewOnClickListener(this, 1);

    return Activity::constructor();
}

Boolean CMediaFormat::RunKeyguardConfirmation(
    /* [in] */ Int32 request)
{
    AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
    helper->constructor(this);
    AutoPtr<ICharSequence> cs1, cs2;
    GetText(R::string::media_format_gesture_prompt, (ICharSequence**)&cs1);
    GetText(R::string::media_format_gesture_explanation, (ICharSequence**)&cs2);
    return helper->LaunchConfirmationActivity(request, cs1, cs2);
}

ECode CMediaFormat::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Activity::OnActivityResult(requestCode, resultCode, data);

    if (requestCode != KEYGUARD_REQUEST) {
        return NOERROR;
    }

    // If the user entered a valid keyguard trace, present the final
    // confirmation prompt; otherwise, go back to the initial state.
    if (resultCode == IActivity::RESULT_OK) {
        EstablishFinalConfirmationState();
    }
    else if (resultCode == IActivity::RESULT_CANCELED) {
        Finish();
    }
    else {
        EstablishInitialState();
    }
    return NOERROR;
}

void CMediaFormat::EstablishFinalConfirmationState()
{
    if (mFinalView == NULL) {
        mInflater->Inflate(R::layout::media_format_final, NULL, (IView**)&mFinalView);
        AutoPtr<IView> finalButton;
        mFinalView->FindViewById(R::id::execute_media_format, (IView**)&finalButton);
        mFinalButton = IButton::Probe(finalButton);
        finalButton->SetOnClickListener(mFinalClickListener);
    }

    SetContentView(mFinalView);
}

void CMediaFormat::EstablishInitialState()
{
    if (mInitialView == NULL) {
        mInflater->Inflate(R::layout::media_format_primary, NULL, (IView**)&mInitialView);
        AutoPtr<IView> initiateButton;
        mInitialView->FindViewById(R::id::initiate_media_format, (IView**)&initiateButton);
        mInitiateButton = IButton::Probe(initiateButton);
        initiateButton->SetOnClickListener(mInitiateListener);
    }

    SetContentView(mInitialView);
}

ECode CMediaFormat::OnCreate(
    /* [in] */ IBundle* savedState)
{
    Activity::OnCreate(savedState);

    mInitialView = NULL;
    mFinalView = NULL;
    LayoutInflater::From(this, (ILayoutInflater**)&mInflater);

    EstablishInitialState();
    return NOERROR;
}

ECode CMediaFormat::OnPause()
{
    Activity::OnPause();

    Boolean res;
    if (IsFinishing(&res), !res) {
        EstablishInitialState();
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
