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

#include "elastos/droid/app/CFragmentManagerState.h"
#include "elastos/droid/app/CFragmentState.h"
#include "elastos/droid/app/CBackStackState.h"
#include "elastos/droid/app/CFragmentState.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CFragmentManagerState, Object, IFragmentManagerState, IParcelable)

CAR_OBJECT_IMPL(CFragmentManagerState)

CFragmentManagerState::CFragmentManagerState()
{}

ECode CFragmentManagerState::constructor()
{
    return NOERROR;
}

ECode CFragmentManagerState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 value;
    AutoPtr<IParcelable> parcelable;

    mActive = NULL;
    source->ReadInt32(&value);
    if (value >= 0) {
        Int32 size = value;
        mActive = ArrayOf<IFragmentState*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IFragmentState> nc;
            source->ReadInt32(&value);
            if (value != 0) {
                CFragmentState::New((IFragmentState**)&nc);
                parcelable = IParcelable::Probe(nc);
                parcelable->ReadFromParcel(source);
            }
            mActive->Set(i, nc);
        }
    }

    mAdded = NULL;
    source->ReadArrayOf((Handle32*)(&mAdded));

    mBackStack = NULL;
    source->ReadInt32(&value);
    if (value >= 0) {
        Int32 size = value;
        mBackStack = ArrayOf<IBackStackState*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IBackStackState> nc;
            source->ReadInt32(&value);
            if (value != 0) {
                CBackStackState::New((IBackStackState**)&nc);
                parcelable = IParcelable::Probe(nc);
                parcelable->ReadFromParcel(source);
            }
            mBackStack->Set(i, nc);
        }
    }
    return NOERROR;
}

ECode CFragmentManagerState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoPtr<IParcelable> parcelable;

    if (mActive == NULL) {
        dest->WriteInt32(-1);
    }
    else {
        Int32 size = mActive->GetLength();
        dest->WriteInt32(size);
        AutoPtr<IFragmentState> nc;
        for (Int32 i = 0; i < size; ++i) {
            nc = (*mActive)[i];
            if (nc != NULL) {
                dest->WriteInt32(1);
                parcelable = IParcelable::Probe(nc);
                parcelable->WriteToParcel(dest);
            } else {
                dest->WriteInt32(0);
            }
        }
    }

    dest->WriteArrayOf((Handle32)mAdded.Get());

    if (mBackStack == NULL) {
        dest->WriteInt32(-1);
    }
    else {
        Int32 size = mBackStack->GetLength();
        dest->WriteInt32(size);
        AutoPtr<IBackStackState> nc;
        for (Int32 i = 0; i < size; ++i) {
            nc = (*mBackStack)[i];
            if (nc != NULL) {
                dest->WriteInt32(1);
                parcelable = IParcelable::Probe(nc);
                parcelable->WriteToParcel(dest);
            } else {
                dest->WriteInt32(0);
            }
        }
    }
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos