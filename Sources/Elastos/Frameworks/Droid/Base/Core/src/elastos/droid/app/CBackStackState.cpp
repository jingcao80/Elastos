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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CBackStackState.h"
#include "elastos/droid/app/CBackStackRecordTransitionState.h"
#include "elastos/droid/app/CBackStackRecord.h"
#include "elastos/droid/app/FragmentManagerImpl.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/CView.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Transition::CTransitionManager;
using Elastos::Droid::Transition::ITransitionSet;
using Elastos::Droid::Transition::CTransitionSet;
using Elastos::Droid::Transition::EIID_IEpicenterCallback;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::IO::CPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//=====================================================================
// CBackStackState
//=====================================================================

CAR_INTERFACE_IMPL_2(CBackStackState, Object, IBackStackState, IParcelable)

CAR_OBJECT_IMPL(CBackStackState)

CBackStackState::CBackStackState()
    : mTransition(0)
    , mTransitionStyle(0)
    , mIndex(0)
    , mBreadCrumbTitleRes(0)
    , mBreadCrumbShortTitleRes(0)
{}

CBackStackState::~CBackStackState()
{}

ECode CBackStackState::constructor()
{
    return NOERROR;
}

ECode CBackStackState::constructor(
    /* [in] */ IFragmentManagerImpl* fm,
    /* [in] */ IBackStackRecord* bse)
{
    Int32 numRemoved = 0;
    AutoPtr<BackStackRecord> bseObj = (BackStackRecord*)bse;
    AutoPtr<BackStackRecord::Op> op = bseObj->mHead;
    Int32 size;
    while (op != NULL) {
        if (op->mRemoved != NULL) {
            op->mRemoved->GetSize(&size);
            numRemoved += size;
        }
        op = op->mNext;
    }
    mOps = ArrayOf<Int32>::Alloc(bseObj->mNumOp * 7 + numRemoved);

    if (!bseObj->mAddToBackStack) {
//        throw new IllegalStateException("Not on back stack");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    op = bseObj->mHead;
    Int32 pos = 0;
    Boolean hasNext;
    while (op != NULL) {
        Int32 index = -1;
        if (op->mFragment != NULL) op->mFragment->GetIndex(&index);
        (*mOps)[pos++] = op->mCmd;
        (*mOps)[pos++] = index;
        (*mOps)[pos++] = op->mEnterAnim;
        (*mOps)[pos++] = op->mExitAnim;
        (*mOps)[pos++] = op->mPopEnterAnim;
        (*mOps)[pos++] = op->mPopExitAnim;
        if (op->mRemoved != NULL) {
            op->mRemoved->GetSize(&size);
            (*mOps)[pos++] = size;
            AutoPtr<IIterator> it;
            op->mRemoved->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IFragment::Probe(obj)->GetIndex(&index);
                (*mOps)[pos++] = index;
            }
        }
        else {
            (*mOps)[pos++] = 0;
        }

        op = op->mNext;
    }
    mTransition = bseObj->mTransition;
    mTransitionStyle = bseObj->mTransitionStyle;
    mName = bseObj->mName;
    mIndex = bseObj->mIndex;
    mBreadCrumbTitleRes = bseObj->mBreadCrumbTitleRes;
    mBreadCrumbTitleText = bseObj->mBreadCrumbTitleText;
    mBreadCrumbShortTitleRes = bseObj->mBreadCrumbShortTitleRes;
    mBreadCrumbShortTitleText = bseObj->mBreadCrumbShortTitleText;
    mSharedElementSourceNames = bseObj->mSharedElementSourceNames;
    mSharedElementTargetNames = bseObj->mSharedElementTargetNames;
    return NOERROR;
}

ECode CBackStackState::Instantiate(
    /* [in] */ IFragmentManagerImpl* fm,
    /* [out] */ IBackStackRecord** record)
{
    VALIDATE_NOT_NULL(record);

    AutoPtr<FragmentManagerImpl> fmObj = (FragmentManagerImpl*)fm;
    AutoPtr<IBackStackRecord> r;
    CBackStackRecord::New(fm, (IBackStackRecord**)&r);
    BackStackRecord* bse = (BackStackRecord*)r.Get();
    Int32 pos = 0;
    Int32 num = 0;
    while (pos < mOps->GetLength()) {
        AutoPtr<BackStackRecord::Op> op = new BackStackRecord::Op();
        op->mCmd = (*mOps)[pos++];
        if (FragmentManagerImpl::DEBUG) {
            Logger::V(FragmentManagerImpl::TAG,
                "Instantiate %s op #%d base fragment #%d",
                TO_CSTR(bse), num, (*mOps)[pos]);
        }
        Int32 findex = (*mOps)[pos++];
        if (findex >= 0) {
            AutoPtr<IInterface> obj;
            fmObj->mActive->Get(findex, (IInterface**)&obj);
            op->mFragment = IFragment::Probe(obj);
        }
        else {
            op->mFragment = NULL;
        }
        op->mEnterAnim = (*mOps)[pos++];
        op->mExitAnim = (*mOps)[pos++];
        op->mPopEnterAnim = (*mOps)[pos++];
        op->mPopExitAnim = (*mOps)[pos++];
        Int32 N = (*mOps)[pos++];
        if (N > 0) {
            CArrayList::New(N, (IArrayList**)&op->mRemoved);
            for (Int32 i = 0; i < N; i++) {
                if (FragmentManagerImpl::DEBUG) {
                    Logger::V(FragmentManagerImpl::TAG,
                        "Instantiate %s set remove fragment #%d",
                        TO_CSTR(bse), (*mOps)[pos]);
                }
                AutoPtr<IInterface> obj;
                fmObj->mActive->Get((*mOps)[pos++], (IInterface**)&obj);
                op->mRemoved->Add(obj);
            }
        }
        bse->AddOp(op);
        num++;
    }
    bse->mTransition = mTransition;
    bse->mTransitionStyle = mTransitionStyle;
    bse->mName = mName;
    bse->mIndex = mIndex;
    bse->mAddToBackStack = TRUE;
    bse->mBreadCrumbTitleRes = mBreadCrumbTitleRes;
    bse->mBreadCrumbTitleText = mBreadCrumbTitleText;
    bse->mBreadCrumbShortTitleRes = mBreadCrumbShortTitleRes;
    bse->mBreadCrumbShortTitleText = mBreadCrumbShortTitleText;
    bse->mSharedElementSourceNames = mSharedElementSourceNames;
    bse->mSharedElementTargetNames = mSharedElementTargetNames;
    bse->BumpBackStackNesting(1);
    *record = bse;
    REFCOUNT_ADD(*record);
    return NOERROR;
}

ECode CBackStackState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteArrayOf((HANDLE)mOps.Get());
    dest->WriteInt32(mTransition);
    dest->WriteInt32(mTransitionStyle);
    dest->WriteString(mName);
    dest->WriteInt32(mIndex);
    dest->WriteInt32(mBreadCrumbTitleRes);
    dest->WriteInterfacePtr(mBreadCrumbTitleText);
    dest->WriteInt32(mBreadCrumbShortTitleRes);
    dest->WriteInterfacePtr(mBreadCrumbShortTitleText);
    return NOERROR;
}

ECode CBackStackState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mOps = NULL;
    source->ReadArrayOf((HANDLE*)(&mOps));
    source->ReadInt32(&mTransition);
    source->ReadInt32(&mTransitionStyle);
    source->ReadString(&mName);
    source->ReadInt32(&mIndex);
    source->ReadInt32(&mBreadCrumbTitleRes);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mBreadCrumbTitleText = obj != NULL ? ICharSequence::Probe(obj) : NULL;
    source->ReadInt32(&mBreadCrumbShortTitleRes);
    AutoPtr<IInterface> obj_2;
    source->ReadInterfacePtr((HANDLE*)&obj_2);
    mBreadCrumbShortTitleText = obj_2 != NULL ? ICharSequence::Probe(obj_2) : NULL;
    mSharedElementSourceNames = Elastos::Droid::Os::CParcel::CreateStringArrayList(source);
    mSharedElementTargetNames = Elastos::Droid::Os::CParcel::CreateStringArrayList(source);
    return NOERROR;
}

//===================================================================
// BackStackRecord
//===================================================================
CAR_INTERFACE_IMPL_3(BackStackRecord, FragmentTransaction, IBackStackRecord, IRunnable, IFragmentManagerBackStackEntry)

const String BackStackRecord::TAG("BackStackRecord");
const Int32 BackStackRecord::OP_NULL = 0;
const Int32 BackStackRecord::OP_ADD = 1;
const Int32 BackStackRecord::OP_REPLACE = 2;
const Int32 BackStackRecord::OP_REMOVE = 3;
const Int32 BackStackRecord::OP_HIDE = 4;
const Int32 BackStackRecord::OP_SHOW = 5;
const Int32 BackStackRecord::OP_DETACH = 6;
const Int32 BackStackRecord::OP_ATTACH = 7;

BackStackRecord::BackStackRecord()
    : mManager(NULL)
    , mNumOp(0)
    , mEnterAnim(0)
    , mExitAnim(0)
    , mPopEnterAnim(0)
    , mPopExitAnim(0)
    , mTransition(0)
    , mTransitionStyle(0)
    , mAddToBackStack(FALSE)
    , mAllowAddToBackStack(TRUE)
    , mCommitted(FALSE)
    , mIndex(-1)
    , mBreadCrumbTitleRes(0)
    , mBreadCrumbShortTitleRes(0)
{}

BackStackRecord::~BackStackRecord()
{}

ECode BackStackRecord::constructor(
    /* [in] */ IFragmentManagerImpl* manager)
{
    mManager = (FragmentManagerImpl*)manager;
    return NOERROR;
}

ECode BackStackRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("BackStackEntry{");
    sb += StringUtils::ToHexString((Int32)this);
    if (mIndex >= 0) {
        sb += " #";
        sb += mIndex;
    }
    if (mName != NULL) {
        sb += " ";
        sb += mName;
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode BackStackRecord::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    Dump(prefix, writer, TRUE);
    return NOERROR;
}

void BackStackRecord::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ Boolean full)
{
    if (full) {
        writer->Print(prefix); writer->Print(String("mName=")); writer->Print(mName);
                writer->Print(String(" mIndex=")); writer->Print(mIndex);
                writer->Print(String(" mCommitted=")); writer->Print(mCommitted);
        if (mTransition != IFragmentTransaction::TRANSIT_NONE) {
            writer->Print(prefix); writer->Print(String("mTransition=#"));
                    writer->Print(StringUtils::ToHexString(mTransition));
                    writer->Print(String(" mTransitionStyle=#"));
                    writer->Println(StringUtils::ToHexString(mTransitionStyle));
        }
        if (mEnterAnim != 0 || mExitAnim != 0) {
            writer->Print(prefix); writer->Print(String("mEnterAnim=#"));
                    writer->Print(StringUtils::ToHexString(mEnterAnim));
                    writer->Print(String(" mExitAnim=#"));
                    writer->Println(StringUtils::ToHexString(mExitAnim));
        }
        if (mPopEnterAnim != 0 || mPopExitAnim != 0) {
            writer->Print(prefix); writer->Print(String("mPopEnterAnim=#"));
                    writer->Print(StringUtils::ToHexString(mPopEnterAnim));
                    writer->Print(String(" mPopExitAnim=#"));
                    writer->Println(StringUtils::ToHexString(mPopExitAnim));
        }
        if (mBreadCrumbTitleRes != 0 || mBreadCrumbTitleText != NULL) {
            writer->Print(prefix); writer->Print(String("mBreadCrumbTitleRes=#"));
                    writer->Print(StringUtils::ToHexString(mBreadCrumbTitleRes));
                    writer->Print(String(" mBreadCrumbTitleText="));
                    writer->Println(mBreadCrumbTitleText);
        }
        if (mBreadCrumbShortTitleRes != 0 || mBreadCrumbShortTitleText != NULL) {
            writer->Print(prefix); writer->Print(String("mBreadCrumbShortTitleRes=#"));
                    writer->Print(StringUtils::ToHexString(mBreadCrumbShortTitleRes));
                    writer->Print(String(" mBreadCrumbShortTitleText="));
                    writer->Println(mBreadCrumbShortTitleText);
        }
    }

    if (mHead != NULL) {
        writer->Print(prefix); writer->Println(String("Operations:"));
        String innerPrefix = prefix + "    ";
        AutoPtr<Op> op = mHead;
        Int32 num = 0;
        while (op != NULL) {
            String cmdStr;
            switch (op->mCmd) {
                case OP_NULL: cmdStr="NULL"; break;
                case OP_ADD: cmdStr="ADD"; break;
                case OP_REPLACE: cmdStr="REPLACE"; break;
                case OP_REMOVE: cmdStr="REMOVE"; break;
                case OP_HIDE: cmdStr="HIDE"; break;
                case OP_SHOW: cmdStr="SHOW"; break;
                case OP_DETACH: cmdStr="DETACH"; break;
                case OP_ATTACH: cmdStr="ATTACH"; break;
                default: StringBuilder sb("cmd="); sb.Append(op->mCmd); cmdStr = sb.ToString(); break;
            }
            writer->Print(prefix); writer->Print(String("  Op #")); writer->Print(num);
                    writer->Print(String(": ")); writer->Print(cmdStr);
                    writer->Print(String(" ")); writer->Println(op->mFragment);
            if (full) {
                if (op->mEnterAnim != 0 || op->mExitAnim != 0) {
                    writer->Print(innerPrefix); writer->Print(String("mEnterAnim=#"));
                            writer->Print(StringUtils::ToHexString(op->mEnterAnim));
                            writer->Print(String(" mExitAnim=#"));
                            writer->Println(StringUtils::ToHexString(op->mExitAnim));
                }
                if (op->mPopEnterAnim != 0 || op->mPopExitAnim != 0) {
                    writer->Print(innerPrefix); writer->Print(String("mPopEnterAnim=#"));
                            writer->Print(StringUtils::ToHexString(op->mPopEnterAnim));
                            writer->Print(String(" mPopExitAnim=#"));
                            writer->Println(StringUtils::ToHexString(op->mPopExitAnim));
                }
            }
            if (op->mRemoved != NULL) {
                Int32 size;
                op->mRemoved->GetSize(&size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> obj;
                    op->mRemoved->Get(i, (IInterface**)&obj);

                    writer->Print(innerPrefix);
                    if (size == 1) {
                        writer->Print(String("mRemoved: "));
                    }
                    else {
                        if (i == 0) {
                            writer->Println(String("mRemoved:"));
                        }
                        writer->Print(innerPrefix); writer->Print(String("  #"));
                        writer->Print(i); writer->Print(String(": "));
                    }
                    writer->Println(obj);
                }
            }
            op = op->mNext;
            num++;
        }
    }
}

ECode BackStackRecord::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIndex;
    return NOERROR;
}

ECode BackStackRecord::GetBreadCrumbTitleRes(
    /* [out] */ Int32* titleRes)
{
    VALIDATE_NOT_NULL(titleRes);
    *titleRes = mBreadCrumbTitleRes;
    return NOERROR;
}

ECode BackStackRecord::GetBreadCrumbShortTitleRes(
    /* [out] */ Int32* shortTitleRes)
{
    VALIDATE_NOT_NULL(shortTitleRes);
    *shortTitleRes = mBreadCrumbShortTitleRes;
    return NOERROR;
}

ECode BackStackRecord::GetBreadCrumbTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    if (mBreadCrumbTitleRes != 0) {
        return IContext::Probe(mManager->mActivity)->GetText(mBreadCrumbTitleRes, title);
    }
    *title = mBreadCrumbTitleText;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode BackStackRecord::GetBreadCrumbShortTitle(
    /* [out] */ ICharSequence** shortTitle)
{
    VALIDATE_NOT_NULL(shortTitle);
    if (mBreadCrumbShortTitleRes != 0) {
        return IContext::Probe(mManager->mActivity)->GetText(mBreadCrumbShortTitleRes, shortTitle);
    }
    *shortTitle = mBreadCrumbShortTitleText;
    REFCOUNT_ADD(*shortTitle);
    return NOERROR;
}

void BackStackRecord::AddOp(
    /* [in] */ Op* op)
{
    if (mHead == NULL) {
        mHead = mTail = op;
    }
    else {
        op->mPrev = mTail;
        mTail->mNext = op;
        mTail = op;
    }
    op->mEnterAnim = mEnterAnim;
    op->mExitAnim = mExitAnim;
    op->mPopEnterAnim = mPopEnterAnim;
    op->mPopExitAnim = mPopExitAnim;
    mNumOp++;
}


ECode BackStackRecord::Add(
    /* [in] */ IFragment* fragment,
    /* [in] */ const String& tag)
{
    return DoAddOp(0, fragment, tag, OP_ADD);
}

ECode BackStackRecord::Add(
    /* [in] */ Int32 containerViewId,
    /* [in] */ IFragment* fragment)
{
    return DoAddOp(containerViewId, fragment, String(NULL), OP_ADD);
}

ECode BackStackRecord::Add(
    /* [in] */ Int32 containerViewId,
    /* [in] */ IFragment* fragment,
    /* [in] */ const String& tag)
{
    return DoAddOp(containerViewId, fragment, tag, OP_ADD);
}

ECode BackStackRecord::DoAddOp(
    /* [in] */ Int32 containerViewId,
    /* [in] */ IFragment* fragment,
    /* [in] */ const String& tag,
    /* [in] */ Int32 opcmd)
{
    fragment->SetFragmentManager(mManager);

    if (!tag.IsNull()) {
        String memTag;
        fragment->GetTag(&memTag);
        if (!memTag.IsNull() && !tag.Equals(memTag)) {
//            throw new IllegalStateException("Can't change tag of fragment "
//                    + fragment + ": was " + fragment.mTag
//                    + " now " + tag);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        fragment->SetTag(tag);
    }

    if (containerViewId != 0) {
        Int32 fragmentId;
        fragment->GetId(&fragmentId);
        if (fragmentId != 0 && fragmentId != containerViewId) {
//            throw new IllegalStateException("Can't change container ID of fragment "
//                    + fragment + ": was " + fragment.mFragmentId
//                    + " now " + containerViewId);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        fragment->SetFragmentId(containerViewId);
        fragment->SetContainerId(containerViewId);
    }

    AutoPtr<Op> op = new Op();
    op->mCmd = opcmd;
    op->mFragment = fragment;
    AddOp(op);
    return NOERROR;
}

ECode BackStackRecord::Replace(
    /* [in] */ Int32 containerViewId,
    /* [in] */ IFragment* fragment)
{
    return Replace(containerViewId, fragment, String(NULL));
}

ECode BackStackRecord::Replace(
    /* [in] */ Int32 containerViewId,
    /* [in] */ IFragment* fragment,
    /* [in] */ const String& tag)
{
    if (containerViewId == 0) {
//      throw new IllegalArgumentException("Must use non-zero containerViewId");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return DoAddOp(containerViewId, fragment, tag, OP_REPLACE);
}

ECode BackStackRecord::Remove(
    /* [in] */ IFragment* fragment)
{
    AutoPtr<Op> op = new Op();
    op->mCmd = OP_REMOVE;
    op->mFragment = fragment;
    AddOp(op);

    return NOERROR;
}

ECode BackStackRecord::Hide(
    /* [in] */ IFragment* fragment)
{
    AutoPtr<Op> op = new Op();
    op->mCmd = OP_HIDE;
    op->mFragment = fragment;
    AddOp(op);

    return NOERROR;
}

ECode BackStackRecord::Show(
    /* [in] */ IFragment* fragment)
{
    AutoPtr<Op> op = new Op();
    op->mCmd = OP_SHOW;
    op->mFragment = fragment;
    AddOp(op);

    return NOERROR;
}

ECode BackStackRecord::Detach(
    /* [in] */ IFragment* fragment)
{
    AutoPtr<Op> op = new Op();
    op->mCmd = OP_DETACH;
    op->mFragment = fragment;
    AddOp(op);

    return NOERROR;
}

ECode BackStackRecord::Attach(
    /* [in] */ IFragment* fragment)
{
    AutoPtr<Op> op = new Op();
    op->mCmd = OP_ATTACH;
    op->mFragment = fragment;
    AddOp(op);

    return NOERROR;
}

ECode BackStackRecord::SetCustomAnimations(
    /* [in] */ Int32 enter,
    /* [in] */ Int32 exit)
{
    return SetCustomAnimations(enter, exit, 0, 0);
}

ECode BackStackRecord::SetCustomAnimations(
    /* [in] */ Int32 enter,
    /* [in] */ Int32 exit,
    /* [in] */ Int32 popEnter,
    /* [in] */ Int32 popExit)
{
    mEnterAnim = enter;
    mExitAnim = exit;
    mPopEnterAnim = popEnter;
    mPopExitAnim = popExit;
    return NOERROR;
}

ECode BackStackRecord::SetTransition(
    /* [in] */ Int32 transition)
{
    mTransition = transition;
    return NOERROR;
}

ECode BackStackRecord::AddSharedElement(
    /* [in] */ IView* sharedElement,
    /* [in] */ const String& name)
{
    String transitionName;
    sharedElement->GetTransitionName(&transitionName);
    if (transitionName.IsNull()) {
        Logger::E(TAG, "Unique transitionNames are required for all sharedElements");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSharedElementSourceNames == NULL) {
        CArrayList::New((IArrayList**)&mSharedElementSourceNames);
        CArrayList::New((IArrayList**)&mSharedElementTargetNames);
    }
    mSharedElementSourceNames->Add(CoreUtils::Convert(transitionName));
    mSharedElementTargetNames->Add(CoreUtils::Convert(name));
    return NOERROR;
}

ECode BackStackRecord::SetSharedElement(
    /* [in] */ IView* sharedElement,
    /* [in] */ const String& name)
{
    String transitionName;
    sharedElement->GetTransitionName(&transitionName);
    if (transitionName == NULL) {
        Logger::E(TAG, "Unique transitionNames are required for all sharedElements");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSharedElementSourceNames = NULL;
    mSharedElementTargetNames = NULL;
    CArrayList::New(1, (IArrayList**)&mSharedElementSourceNames);
    CArrayList::New(1, (IArrayList**)&mSharedElementTargetNames);

    mSharedElementSourceNames->Add(CoreUtils::Convert(transitionName));
    mSharedElementTargetNames->Add(CoreUtils::Convert(name));
    return NOERROR;
}

ECode BackStackRecord::SetSharedElements(
    /* [in] */ ArrayOf<IPair*>* sharedElements)//* Pair<View, String>... sharedElements*/
{
    if (sharedElements == NULL || sharedElements->GetLength() == 0) {
        mSharedElementSourceNames = NULL;
        mSharedElementTargetNames = NULL;
    }
    else {
        Int32 length = sharedElements->GetLength();
        AutoPtr<IArrayList> sourceNames, targetNames;
        CArrayList::New(length, (IArrayList**)&sourceNames);
        CArrayList::New(length, (IArrayList**)&targetNames);

        for (Int32 i = 0; i < length; i++) {
            IPair* pair = (*sharedElements)[i];
            AutoPtr<IInterface> first, second;
            pair->GetFirst((IInterface**)&first);
            pair->GetSecond((IInterface**)&second);

            String transitionName;
            IView::Probe(first)->GetTransitionName(&transitionName);
            if (transitionName.IsNull()) {
                Logger::E(TAG, "Unique transitionNames are required for all sharedElements");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            sourceNames->Add(CoreUtils::Convert(transitionName));
            targetNames->Add(second);
        }
        mSharedElementSourceNames = sourceNames;
        mSharedElementTargetNames = targetNames;
    }
    return NOERROR;
}

ECode BackStackRecord::SetTransitionStyle(
    /* [in] */ Int32 styleRes)
{
    mTransitionStyle = styleRes;
    return NOERROR;
}

ECode BackStackRecord::AddToBackStack(
    /* [in] */ const String& name)
{
    if (!mAllowAddToBackStack) {
        // throw new IllegalStateException(
        //         "This FragmentTransaction is not allowed to be added to the back stack.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAddToBackStack = TRUE;
    mName = name;
    return NOERROR;
}

ECode BackStackRecord::IsAddToBackStackAllowed(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    *allowed = mAllowAddToBackStack;
    return NOERROR;
}

ECode BackStackRecord::DisallowAddToBackStack()
{
    if (mAddToBackStack) {
        // throw new IllegalStateException(
        //         "This transaction is already being added to the back stack");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAllowAddToBackStack = FALSE;
    return NOERROR;
}

ECode BackStackRecord::SetBreadCrumbTitle(
    /* [in] */ Int32 res)
{
    mBreadCrumbTitleRes = res;
    mBreadCrumbTitleText = NULL;
    return NOERROR;
}

ECode BackStackRecord::SetBreadCrumbTitle(
    /* [in] */ ICharSequence* text)
{
    mBreadCrumbTitleRes = 0;
    mBreadCrumbTitleText = text;
    return NOERROR;
}

ECode BackStackRecord::SetBreadCrumbShortTitle(
    /* [in] */ Int32 res)
{
    mBreadCrumbShortTitleRes = res;
    mBreadCrumbShortTitleText = NULL;
    return NOERROR;
}

ECode BackStackRecord::SetBreadCrumbShortTitle(
    /* [in] */ ICharSequence* text)
{
    mBreadCrumbShortTitleRes = 0;
    mBreadCrumbShortTitleText = text;
    return NOERROR;
}

void BackStackRecord::BumpBackStackNesting(
    /* [in] */ Int32 amt)
{
    if (!mAddToBackStack) {
        return;
    }
    if (FragmentManagerImpl::DEBUG) {
        Logger::V(TAG, "Bump nesting in %s by %d", TO_CSTR(this), amt);
    }
    AutoPtr<Op> op = mHead;
    while (op != NULL) {
        if (op->mFragment != NULL) {
            Int32 newNesting;
            op->mFragment->GetBackStackNesting(&newNesting);
            newNesting += amt;
            op->mFragment->SetBackStackNesting(newNesting);
            if (FragmentManagerImpl::DEBUG) {
                Logger::V(TAG, "Bump nesting of %s to %d", TO_CSTR(op->mFragment), newNesting);
            }
        }

        if (op->mRemoved != NULL) {
            Int32 size;
            op->mRemoved->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                op->mRemoved->Get(i, (IInterface**)&obj);
                IFragment* r = IFragment::Probe(obj);
                Int32 newNesting;
                r->GetBackStackNesting(&newNesting);
                newNesting += amt;
                r->SetBackStackNesting(newNesting);
                if (FragmentManagerImpl::DEBUG) {
                    Logger::V(TAG, "Bump nesting of %s to %d", TO_CSTR(r), newNesting);
                }
            }
        }

        op = op->mNext;
    }
}

ECode BackStackRecord::Commit(
    /* [out] */ Int32* identifier)
{
    VALIDATE_NOT_NULL(identifier);
    *identifier = CommitInternal(FALSE);
    return NOERROR;
}

ECode BackStackRecord::CommitAllowingStateLoss(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CommitInternal(TRUE);
    return NOERROR;
}

Int32 BackStackRecord::CommitInternal(
    /* [in] */ Boolean allowStateLoss)
{
    if (mCommitted) return E_ILLEGAL_STATE_EXCEPTION;//throw new IllegalStateException("commit already called");

    if (FragmentManagerImpl::DEBUG) {
        Logger::V(TAG, "Commit: %s", TO_CSTR(this));
//        LogWriter logw/* = new LogWriter(Logger::VERBOSE, TAG)*/;
//        AutoPtr<IPrintWriter> pw;
//        FastPrintWriter::New(logw, FALSE, 1024, (IPrintWriter**)&pw);
//        Dump(String(NULL), NULL, pw, NULL);
    }

    mCommitted = TRUE;
    if (mAddToBackStack) {
        mManager->AllocBackStackIndex(this, &mIndex);
    }
    else {
        mIndex = -1;
    }
    mManager->EnqueueAction(this, allowStateLoss);
    return mIndex;
}

ECode BackStackRecord::Run()
{
    if (FragmentManagerImpl::DEBUG) Logger::V(TAG, "Run: %s", TO_CSTR(this));

    if (mAddToBackStack) {
        if (mIndex < 0) {
            //throw new IllegalStateException("addToBackStack() called after commit()");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    BumpBackStackNesting(1);

    AutoPtr<ISparseArray> firstOutFragments, lastInFragments;
    CSparseArray::New((ISparseArray**)&firstOutFragments);
    CSparseArray::New((ISparseArray**)&lastInFragments);
    CalculateFragments(firstOutFragments, lastInFragments);
    BeginTransition(firstOutFragments, lastInFragments, FALSE);

    AutoPtr<Op> op = mHead;
    while (op != NULL) {
        switch (op->mCmd) {
            case OP_ADD: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mEnterAnim);
                mManager->AddFragment(f, FALSE);
            }
            break;

            case OP_REPLACE: {
                AutoPtr<IFragment> f = op->mFragment;
                if (mManager->mAdded != NULL) {
                    Int32 N;
                    mManager->mAdded->GetSize(&N);
                    for (Int32 i = 0; i < N; i++) {
                        AutoPtr<IInterface> obj;
                        mManager->mAdded->Get(i, (IInterface**)&obj);
                        IFragment* old = IFragment::Probe(obj);
                        if (FragmentManagerImpl::DEBUG) {
                            Logger::V(TAG, "OP_REPLACE: adding=%s old=%s", TO_CSTR(f), TO_CSTR(old));
                        }
                        Int32 oldId, fId;
                        if (f == NULL || (old->GetContainerId(&oldId), oldId) == (f->GetContainerId(&fId), fId)) {
                            if (old == f.Get()) {
                                op->mFragment = f = NULL;
                            }
                            else {
                                if (op->mRemoved == NULL) {
                                    CArrayList::New((IList**)&op->mRemoved);
                                }
                                op->mRemoved->Add(old);
                                old->SetNextAnim(op->mExitAnim);
                                if (mAddToBackStack) {
                                    Int32 oldNesting;
                                    old->GetBackStackNesting(&oldNesting);
                                    oldNesting += 1;
                                    old->SetBackStackNesting(oldNesting);
                                    if (FragmentManagerImpl::DEBUG) {
                                        Logger::V(TAG, "Bump nesting of %s to %d", TO_CSTR(old), oldNesting);
                                    }
                                }
                                mManager->RemoveFragment(old, mTransition, mTransitionStyle);
                                mManager->mAdded->GetSize(&N);
                            }
                        }
                    }
                }
                if (f != NULL) {
                    f->SetNextAnim(op->mEnterAnim);
                    mManager->AddFragment(f, FALSE);
                }
            }
            break;

            case OP_REMOVE: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mExitAnim);
                mManager->RemoveFragment(f, mTransition, mTransitionStyle);
            }
            break;

            case OP_HIDE: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mExitAnim);
                mManager->HideFragment(f, mTransition, mTransitionStyle);
            }
            break;

            case OP_SHOW: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mEnterAnim);
                mManager->ShowFragment(f, mTransition, mTransitionStyle);
            } break;

            case OP_DETACH: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mExitAnim);
                mManager->DetachFragment(f, mTransition, mTransitionStyle);
            }
            break;

            case OP_ATTACH: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mEnterAnim);
                mManager->AttachFragment(f, mTransition, mTransitionStyle);
            }
            break;

            default: {
                //throw new IllegalArgumentException("Unknown cmd: " + op->cmd);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        op = op->mNext;
    }

    mManager->MoveToState(mManager->mCurState, mTransition, mTransitionStyle, TRUE);

    if (mAddToBackStack) {
        mManager->AddBackStackState(this);
    }

    return NOERROR;
}

void BackStackRecord::SetFirstOut(
    /* [in] */ ISparseArray* fragments,
    /* [in] */ IFragment* fragment)
{
    if (fragment != NULL) {
        Int32 containerId;
        fragment->GetContainerId(&containerId);
        Boolean hidden, added;
        fragment->IsHidden(&hidden);
        fragment->IsAdded(&added);
        AutoPtr<IView> view;
        fragment->GetView((IView**)&view);
        if (containerId != 0 && !hidden && added && view != NULL) {
            AutoPtr<IInterface> obj;
            fragments->Get(containerId, (IInterface**)&obj);
            if (obj == NULL) {
                fragments->Put(containerId, fragment);
            }
        }
    }
}

void BackStackRecord::SetLastIn(
    /* [in] */ ISparseArray* fragments,
    /* [in] */ IFragment* fragment)
{
    if (fragment != NULL) {
        Int32 containerId;
        fragment->GetContainerId(&containerId);
        if (containerId != 0) {
            fragments->Put(containerId, fragment);
        }
    }
}

void BackStackRecord::CalculateFragments(
    /* [in] */ ISparseArray* firstOutFragments,
    /* [in] */ ISparseArray* lastInFragments)
{
    Boolean bval;
    mManager->mContainer->HasView(&bval);
    if (!bval) {
        return; // nothing to see, so no transitions
    }
    AutoPtr<Op> op = mHead;
    while (op != NULL) {
        switch (op->mCmd) {
            case OP_ADD:
                SetLastIn(lastInFragments, op->mFragment);
                break;

            case OP_REPLACE: {
                AutoPtr<IFragment> f = op->mFragment;
                if (mManager->mAdded != NULL) {
                    Int32 N;
                    mManager->mAdded->GetSize(&N);
                    for (Int32 i = 0; i < N; i++) {
                        AutoPtr<IInterface> obj;
                        mManager->mAdded->Get(i, (IInterface**)&obj);
                        IFragment* old = IFragment::Probe(obj);
                        Int32 oldId = 0, fId = 0;
                        if (f == NULL || (old->GetContainerId(&oldId), oldId) == (f->GetContainerId(&fId), fId)) {
                            if (old == f.Get()) {
                                f = NULL;
                            }
                            else {
                                SetFirstOut(firstOutFragments, old);
                            }
                        }
                    }
                }
                SetLastIn(lastInFragments, f);
                break;
            }
            case OP_REMOVE:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_HIDE:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_SHOW:

                SetLastIn(lastInFragments, op->mFragment);
                break;

            case OP_DETACH:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_ATTACH:
                SetLastIn(lastInFragments, op->mFragment);
                break;
        }

        op = op->mNext;
    }
}

ECode BackStackRecord::CalculateBackFragments(
    /* [in] */ ISparseArray* firstOutFragments,
    /* [in] */ ISparseArray* lastInFragments)
{
    Boolean bval;
    mManager->mContainer->HasView(&bval);
    if (!bval) {
        return NOERROR; // nothing to see, so no transitions
    }

    AutoPtr<Op> op = mHead;
    while (op != NULL) {
        switch (op->mCmd) {
            case OP_ADD:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_REPLACE:
                if (op->mRemoved != NULL) {
                    Int32 size;
                    op->mRemoved->GetSize(&size);
                    for (Int32 i = size - 1; i >= 0; i--) {
                        AutoPtr<IInterface> obj;
                        op->mRemoved->Get(i, (IInterface**)&obj);
                        IFragment* r = IFragment::Probe(obj);
                        SetLastIn(lastInFragments, r);
                    }
                }
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_REMOVE:
                SetLastIn(lastInFragments, op->mFragment);
                break;

            case OP_HIDE:
                SetLastIn(lastInFragments, op->mFragment);
                break;

            case OP_SHOW:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;

            case OP_DETACH:
                SetLastIn(lastInFragments, op->mFragment);
                break;

            case OP_ATTACH:
                SetFirstOut(firstOutFragments, op->mFragment);
                break;
        }

        op = op->mNext;
    }
    return NOERROR;
}

AutoPtr<IBackStackRecordTransitionState> BackStackRecord::BeginTransition(
    /* [in] */ ISparseArray* firstOutFragments,
    /* [in] */ ISparseArray* lastInFragments,
    /* [in] */ Boolean isBack)
{
    AutoPtr<IBackStackRecordTransitionState> state;
    CBackStackRecordTransitionState::New((IBackStackRecordTransitionState**)&state);

    // Adding a non-existent target view makes sure that the transitions don't target
    // any views by default. They'll only target the views we tell add. If we don't
    // add any, then no views will be targeted.
    AutoPtr<IView> view;
    CView::New(IContext::Probe(mManager->mActivity), (IView**)&view);
    state->SetNonExistentView(view);

    // Go over all leaving fragments.
    Int32 N;
    firstOutFragments->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 containerId;
        firstOutFragments->KeyAt(i, &containerId);
        ConfigureTransitions(containerId, state, isBack, firstOutFragments,
                lastInFragments);
    }

    // Now go over all entering fragments that didn't have a leaving fragment.
    lastInFragments->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 containerId;
        lastInFragments->KeyAt(i, &containerId);
        AutoPtr<IInterface> obj;
        firstOutFragments->Get(containerId, (IInterface**)&obj);
        if (obj == NULL) {
            ConfigureTransitions(containerId, state, isBack, firstOutFragments,
                    lastInFragments);
        }
    }
    return state;
}

AutoPtr<ITransition> BackStackRecord::CloneTransition(
    /* [in] */ ITransition* transition)
{
    AutoPtr<IInterface> result;
    if (transition != NULL) {
        ICloneable::Probe(transition)->Clone((IInterface**)&result);
    }
    return ITransition::Probe(result);
}

AutoPtr<ITransition> BackStackRecord::GetEnterTransition(
    /* [in] */ IFragment* inFragment,
    /* [in] */ Boolean isBack)
{
    if (inFragment == NULL) {
        return NULL;
    }
    AutoPtr<ITransition> ts;
    if (isBack) {
        inFragment->GetReenterTransition((ITransition**)&ts);
    }
    else {
        inFragment->GetEnterTransition((ITransition**)&ts);
    }
    return CloneTransition(ts);
}

AutoPtr<ITransition> BackStackRecord::GetExitTransition(
    /* [in] */ IFragment* outFragment,
    /* [in] */ Boolean isBack)
{
    if (outFragment == NULL) {
        return NULL;
    }
    AutoPtr<ITransition> ts;
    if (isBack) {
        outFragment->GetReturnTransition((ITransition**)&ts);
    }
    else {
        outFragment->GetExitTransition((ITransition**)&ts);
    }
    return CloneTransition(ts);
}

AutoPtr<ITransition> BackStackRecord::GetSharedElementTransition(
    /* [in] */ IFragment* inFragment,
    /* [in] */ IFragment* outFragment,
    /* [in] */ Boolean isBack)
{
    if (inFragment == NULL || outFragment == NULL) {
        return NULL;
    }
    AutoPtr<ITransition> ts;
    if (isBack) {
        outFragment->GetSharedElementReturnTransition((ITransition**)&ts);
    }
    else {
        inFragment->GetSharedElementReturnTransition((ITransition**)&ts);
    }
    return CloneTransition(ts);
}

AutoPtr<IArrayList> BackStackRecord::CaptureExitingViews(
    /* [in] */ ITransition* exitTransition,
    /* [in] */ IFragment* outFragment,
    /* [in] */ IArrayMap* namedViews)
{
    AutoPtr<IArrayList>  viewList;
    if (exitTransition != NULL) {
        CArrayList::New((IArrayList**)&viewList);
        AutoPtr<IView> root;
        outFragment->GetView((IView**)&root);
        root->CaptureTransitioningViews(IList::Probe(viewList));
        if (namedViews != NULL) {
            viewList->RemoveAll(ICollection::Probe(viewList));
        }

        AddTargets(exitTransition, viewList);
    }
    return viewList;
}

AutoPtr<IArrayMap> BackStackRecord::RemapSharedElements(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IFragment* outFragment,
    /* [in] */ Boolean isBack)
{
    //ArrayMap<String, View>
    AutoPtr<IArrayMap> namedViews;
    CArrayMap::New((IArrayMap**)&namedViews);

    if (mSharedElementSourceNames != NULL) {
        AutoPtr<IView> view;
        outFragment->GetView((IView**)&view);
        view->FindNamedViews(IMap::Probe(namedViews));
        if (isBack) {
            Boolean result;
            namedViews->RetainAll(ICollection::Probe(mSharedElementTargetNames), &result);
        }
        else {
            AutoPtr<IArrayMap> old = namedViews;
            namedViews = RemapNames(mSharedElementSourceNames, mSharedElementTargetNames, old);
        }
    }

    Fragment* of = (Fragment*)outFragment;
    if (isBack) {
        of->mEnterTransitionCallback->OnMapSharedElements(
            IList::Probe(mSharedElementTargetNames), IMap::Probe(namedViews));
        SetBackNameOverrides(state, namedViews, FALSE);
    }
    else {
        of->mExitTransitionCallback->OnMapSharedElements(
            IList::Probe(mSharedElementTargetNames), IMap::Probe(namedViews));
        SetNameOverrides(state, namedViews, FALSE);
    }

    return namedViews;
}

//==========================================================================

CAR_INTERFACE_IMPL(BackStackRecord::AddTargetsOnPreDrawListener, Object, IOnPreDrawListener)

BackStackRecord::AddTargetsOnPreDrawListener::AddTargetsOnPreDrawListener(
    /* [in] */ BackStackRecord* host,
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ ITransition* enterTransition,
    /* [in] */ ITransition* sharedElementTransition,
    /* [in] */ ITransition* overallTransition,
    /* [in] */ IView* container,
    /* [in] */ IFragment* inFragment,
    /* [in] */ IFragment* outFragment,
    /* [in] */ IArrayList* /*<View>*/ hiddenFragmentViews,
    /* [in] */ Boolean isBack,
    /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
    /* [in] */ IArrayList* /*<View>*/ enteringViews)
    : mHost(host)
    , mState(state)
    , mEnterTransition(enterTransition)
    , mSharedElementTransition(sharedElementTransition)
    , mOverallTransition(overallTransition)
    , mContainer(container)
    , mInFragment(inFragment)
    , mOutFragment(outFragment)
    , mHiddenFragmentViews(hiddenFragmentViews)
    , mIsBack(isBack)
    , mSharedElementTargets(sharedElementTargets)
    , mEnteringViews(enteringViews)
{}

ECode BackStackRecord::AddTargetsOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IViewTreeObserver> vto;
    mContainer->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);

    // Don't include any newly-hidden fragments in the transition->
    Int32 id;
    mInFragment->GetContainerId(&id);
    mHost->ExcludeHiddenFragments(mHiddenFragmentViews, id, mOverallTransition);

    AutoPtr<IArrayMap> namedViews;//ArrayMap<String, View> namedViews = NULL;
    if (mSharedElementTransition != NULL) {
        namedViews = mHost->MapSharedElementsIn(mState, mIsBack, mInFragment);
        mHost->RemoveTargets(mSharedElementTransition, mSharedElementTargets);
        mSharedElementTargets->Clear();
        Boolean empty;
        IMap::Probe(namedViews)->IsEmpty(&empty);
        if (empty) {
            AutoPtr<IView> view;
            mState->GetNonExistentView((IView**)&view);
            mSharedElementTargets->Add(TO_IINTERFACE(view));
        }
        else {
            AutoPtr<ICollection> values;
            IMap::Probe(namedViews)->GetValues((ICollection**)&values);
            mSharedElementTargets->AddAll(values);
        }

        mHost->AddTargets(mSharedElementTransition, mSharedElementTargets);

        mHost->SetEpicenterIn(namedViews, mState);

        mHost->CallSharedElementEnd(mState, mInFragment, mOutFragment, mIsBack, namedViews);
    }

    if (mEnterTransition != NULL) {
        AutoPtr<IView> view;
        mInFragment->GetView((IView**)&view);
        if (view != NULL) {
            view->CaptureTransitioningViews(IList::Probe(mEnteringViews));
            if (namedViews != NULL) {
                AutoPtr<ICollection> values;
                IMap::Probe(namedViews)->GetValues((ICollection**)&values);
                mEnteringViews->RemoveAll(values);
            }
            mHost->AddTargets(mEnterTransition, mEnteringViews);
        }
        mHost->SetSharedElementEpicenter(mEnterTransition, mState);
    }

    *result = TRUE;
    return NOERROR;
}
//==========================================================================

AutoPtr<IArrayList>/*<View>*/ BackStackRecord::AddTransitionTargets(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ ITransition* enterTransition,
    /* [in] */ ITransition* sharedElementTransition,
    /* [in] */ ITransition* overallTransition,
    /* [in] */ IView* container,
    /* [in] */ IFragment* inFragment,
    /* [in] */ IFragment* outFragment,
    /* [in] */ IArrayList* /*<View>*/ hiddenFragmentViews,
    /* [in] */ Boolean isBack,
    /* [in] */ IArrayList* /*<View>*/ sharedElementTargets)
{
    if (enterTransition == NULL
        && sharedElementTransition == NULL
        && overallTransition == NULL) {
        return NULL;
    }

    AutoPtr<IArrayList> enteringViews;
    CArrayList::New((IArrayList**)&enteringViews);
    AutoPtr<IViewTreeObserver> vto;
    container->GetViewTreeObserver((IViewTreeObserver**)&vto);
    AutoPtr<IOnPreDrawListener> listener = new AddTargetsOnPreDrawListener(
        this, state, enterTransition, sharedElementTransition, overallTransition, container,
        inFragment, outFragment, hiddenFragmentViews, isBack, sharedElementTargets, enteringViews);
    vto->AddOnPreDrawListener(listener);
    return enteringViews;
}

void BackStackRecord::CallSharedElementEnd(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IFragment* inFragment,
    /* [in] */ IFragment* outFragment,
    /* [in] */ Boolean isBack,
    /* [in] */ IArrayMap* /*<String, View>*/ namedViews)
{
    AutoPtr<ISet> keyset;
    AutoPtr<ICollection> values;
    IMap::Probe(namedViews)->GetKeySet((ISet**)&keyset);
    IMap::Probe(namedViews)->GetValues((ICollection**)&values);

    AutoPtr<IList> names, views;
    CArrayList::New(ICollection::Probe(keyset), (IList**)&names);
    CArrayList::New(values, (IList**)&views);

    Fragment* fragment = NULL;
    if (isBack) {
        fragment = (Fragment*)outFragment;
    }
    else {
        inFragment = (Fragment*)outFragment;
    }
    fragment->mEnterTransitionCallback->OnSharedElementEnd(names, views, NULL);
}

void BackStackRecord::SetEpicenterIn(
    /* [in] */ IArrayMap* /*<String, View>*/ namedViews,
    /* [in] */ IBackStackRecordTransitionState* state)
{
    Boolean bval;
    IMap::Probe(namedViews)->IsEmpty(&bval);
    if (mSharedElementTargetNames != NULL && bval) {
        AutoPtr<IInterface> first, value;
        mSharedElementTargetNames->Get(0, (IInterface**)&first);
        IMap::Probe(namedViews)->Get(first, (IInterface**)&value);
        // now we know the epicenter of the entering transition->
        AutoPtr<IView> epicenter = IView::Probe(value);
        if (epicenter != NULL) {
            state->SetEnteringEpicenterView(epicenter);
        }
    }
}

AutoPtr<IArrayMap> /*<String, View>*/ BackStackRecord::MapSharedElementsIn(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ Boolean isBack,
    /* [in] */ IFragment* inFragment)
{
    // Now map the shared elements in the incoming fragment
    AutoPtr<IArrayMap> namedViews = MapEnteringSharedElements(state, inFragment, isBack);

    Fragment* fragment = (Fragment*)inFragment;
    // remap shared elements and set the name mapping used
    // in the shared element transition->
    if (isBack) {
        fragment->mExitTransitionCallback->OnMapSharedElements(
            IList::Probe(mSharedElementTargetNames), IMap::Probe(namedViews));
        SetBackNameOverrides(state, namedViews, TRUE);
    }
    else {
        fragment->mEnterTransitionCallback->OnMapSharedElements(
            IList::Probe(mSharedElementTargetNames), IMap::Probe(namedViews));
        SetNameOverrides(state, namedViews, TRUE);
    }
    return namedViews;
}

AutoPtr<ITransition> BackStackRecord::MergeTransitions(
    /* [in] */ ITransition* enterTransition,
    /* [in] */ ITransition* exitTransition,
    /* [in] */ ITransition* sharedElementTransition,
    /* [in] */ IFragment* inFragment,
    /* [in] */ Boolean isBack)
{
    Boolean overlap = TRUE;
    if (enterTransition != NULL && exitTransition != NULL) {
        if (isBack)
            inFragment->GetAllowReturnTransitionOverlap(&overlap);
        else
            inFragment->GetAllowEnterTransitionOverlap(&overlap);
    }

    // Wrap the transitions. Explicit targets like in enter and exit will cause the
    // views to be targeted regardless of excluded views. If that happens, then the
    // excluded fragments views (hidden fragments) will still be in the transition->

    AutoPtr<ITransition> transition;
    if (overlap) {
        // Regular transition -- do it all together
        AutoPtr<ITransitionSet> transitionSet;
        CTransitionSet::New((ITransitionSet**)&transitionSet);
        if (enterTransition != NULL) {
            transitionSet->AddTransition(enterTransition);
        }
        if (exitTransition != NULL) {
            transitionSet->AddTransition(exitTransition);
        }
        if (sharedElementTransition != NULL) {
            transitionSet->AddTransition(sharedElementTransition);
        }
        transition = ITransition::Probe(transitionSet);
    }
    else {
        // First do exit, then enter, but allow shared element transition to happen
        // during both.
        AutoPtr<ITransition> staggered;
        if (exitTransition != NULL && enterTransition != NULL) {
            AutoPtr<ITransitionSet> ts;
            CTransitionSet::New((ITransitionSet**)&ts);
            ts->AddTransition(exitTransition);
            ts->AddTransition(enterTransition);
            ts->SetOrdering(ITransitionSet::ORDERING_SEQUENTIAL);
            staggered = ITransition::Probe(ts);
        }
        else if (exitTransition != NULL) {
            staggered = exitTransition;
        }
        else if (enterTransition != NULL) {
            staggered = enterTransition;
        }

        if (sharedElementTransition != NULL) {
            AutoPtr<ITransitionSet> together;
            CTransitionSet::New((ITransitionSet**)&together);
            if (staggered != NULL) {
                together->AddTransition(staggered);
            }
            together->AddTransition(sharedElementTransition);
            transition = ITransition::Probe(together);
        }
        else {
            transition = staggered;
        }
    }
    return transition;
}

void BackStackRecord::ConfigureTransitions(
    /* [in] */ Int32 containerId,
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ Boolean isBack,
    /* [in] */ ISparseArray* /*<Fragment>*/ firstOutFragments,
    /* [in] */ ISparseArray* /*<Fragment>*/ lastInFragments)
{
    AutoPtr<IView> view;
    mManager->mContainer->FindViewById(containerId, (IView**)&view);
    AutoPtr<IViewGroup> sceneRoot = IViewGroup::Probe(view);
    if (sceneRoot != NULL) {
        AutoPtr<IInterface> obj;
        lastInFragments->Get(containerId, (IInterface**)&obj);
        IFragment* inFragment = IFragment::Probe(obj);
        obj = NULL;
        firstOutFragments->Get(containerId, (IInterface**)&obj);
        IFragment* outFragment = IFragment::Probe(obj);

        AutoPtr<ITransition> enterTransition = GetEnterTransition(inFragment, isBack);
        AutoPtr<ITransition> sharedElementTransition = GetSharedElementTransition(inFragment, outFragment,
                isBack);
        AutoPtr<ITransition> exitTransition = GetExitTransition(outFragment, isBack);

        if (enterTransition == NULL && sharedElementTransition == NULL
            && exitTransition == NULL) {
            return; // no transitions!
        }
        if (enterTransition != NULL) {
            AutoPtr<IView> view;
            state->GetNonExistentView((IView**)&view);
            enterTransition->AddTarget(view);
        }

        AutoPtr<IArrayMap> namedViews;//ArrayMap<String, View> = NULL;
        AutoPtr<IArrayList> sharedElementTargets; // = new ArrayList<View>();
        CArrayList::New((IArrayList**)&sharedElementTargets);

        if (sharedElementTransition != NULL) {
            namedViews = RemapSharedElements(state, outFragment, isBack);
            Boolean bval;
            IMap::Probe(namedViews)->IsEmpty(&bval);
            if (bval) {
                AutoPtr<IView> view;
                state->GetNonExistentView((IView**)&view);
                sharedElementTargets->Add(view);
            }
            else {
                AutoPtr<ICollection> views;
                IMap::Probe(namedViews)->GetValues((ICollection**)&views);
                sharedElementTargets->AddAll(views);
            }
            AddTargets(sharedElementTransition, sharedElementTargets);

            // Notify the start of the transition->
            AutoPtr<ISharedElementCallback> callback = isBack ?
                ((Fragment*)outFragment)->mEnterTransitionCallback :
                ((Fragment*)inFragment)->mEnterTransitionCallback;

            AutoPtr<ISet> keyset;
            AutoPtr<ICollection> values;
            IMap::Probe(namedViews)->GetKeySet((ISet**)&keyset);
            IMap::Probe(namedViews)->GetValues((ICollection**)&values);
            AutoPtr<IList> names, views;
            CArrayList::New(ICollection::Probe(keyset), (IList**)&names);
            CArrayList::New(values, (IList**)&views);
            callback->OnSharedElementStart(names, views, NULL);
        }

        Boolean empty;
        AutoPtr<IArrayList> exitingViews = CaptureExitingViews(exitTransition, outFragment, namedViews);
        if (exitingViews == NULL || (exitingViews->IsEmpty(&empty), empty)) {
            exitTransition = NULL;
        }

        // Set the epicenter of the exit transition
        if (mSharedElementTargetNames != NULL && namedViews != NULL) {
            AutoPtr<IInterface> key, value;
            mSharedElementTargetNames->Get(0, (IInterface**)&key);
            IMap::Probe(namedViews)->Get(key, (IInterface**)&value);
            AutoPtr<IView> epicenterView = IView::Probe(value);
            if (epicenterView != NULL) {
                if (exitTransition != NULL) {
                    SetEpicenter(exitTransition, epicenterView);
                }
                if (sharedElementTransition != NULL) {
                    SetEpicenter(sharedElementTransition, epicenterView);
                }
            }
        }

        AutoPtr<ITransition> transition = MergeTransitions(enterTransition, exitTransition,
                sharedElementTransition, inFragment, isBack);

        if (transition != NULL) {
            AutoPtr<IArrayList> hiddenFragments, enteringViews;
            CArrayList::New((IArrayList**)&hiddenFragments);
            enteringViews = AddTransitionTargets(state, enterTransition,
                sharedElementTransition, transition, IView::Probe(sceneRoot), inFragment, outFragment,
                hiddenFragments, isBack, sharedElementTargets);

            CBackStackRecordTransitionState* stateObj = (CBackStackRecordTransitionState*)state;
            transition->SetNameOverrides(stateObj->mNameOverrides);
            // We want to exclude hidden views later, so we need a non-NULL list in the
            // transition now.
            transition->ExcludeTarget(stateObj->mNonExistentView, TRUE);
            // Now exclude all currently hidden fragments.
            ExcludeHiddenFragments(hiddenFragments, containerId, transition);
            CTransitionManager::BeginDelayedTransition(sceneRoot, transition);
            // Remove the view targeting after the transition starts
            RemoveTargetedViewsFromTransitions(sceneRoot, stateObj->mNonExistentView,
                enterTransition, enteringViews, exitTransition, exitingViews,
                sharedElementTransition, sharedElementTargets, transition, hiddenFragments);
        }
    }
}

//=================================================================================
// BackStackRecord::RemoveTargetsOnPreDrawListener
//=================================================================================

CAR_INTERFACE_IMPL(BackStackRecord::RemoveTargetsOnPreDrawListener, Object, IOnPreDrawListener)

BackStackRecord::RemoveTargetsOnPreDrawListener::RemoveTargetsOnPreDrawListener(
    /* [in] */ BackStackRecord* host,
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* nonExistingView,
    /* [in] */ ITransition* enterTransition,
    /* [in] */ IArrayList* /*<View>*/ enteringViews,
    /* [in] */ ITransition* exitTransition,
    /* [in] */ IArrayList* /*<View>*/ exitingViews,
    /* [in] */ ITransition* sharedElementTransition,
    /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
    /* [in] */ ITransition* overallTransition,
    /* [in] */ IArrayList* /*<View>*/ hiddenViews)
    : mHost(host)
    , mSceneRoot(sceneRoot)
    , mNonExistingView(nonExistingView)
    , mEnterTransition(enterTransition)
    , mEnteringViews(enteringViews)
    , mExitTransition(exitTransition)
    , mExitingViews(exitingViews)
    , mSharedElementTransition(sharedElementTransition)
    , mSharedElementTargets(sharedElementTargets)
    , mOverallTransition(overallTransition)
    , mHiddenViews(hiddenViews)
{
}

ECode BackStackRecord::RemoveTargetsOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IViewTreeObserver> vto;
    IView::Probe(mSceneRoot)->GetViewTreeObserver((IViewTreeObserver**)&vto);

    vto->RemoveOnPreDrawListener(this);
    if (mEnterTransition != NULL) {
        mEnterTransition->RemoveTarget(mNonExistingView);
        mHost->RemoveTargets(mEnterTransition, mEnteringViews);
    }
    if (mExitTransition != NULL) {
        mHost->RemoveTargets(mExitTransition, mExitingViews);
    }
    if (mSharedElementTransition != NULL) {
        mHost->RemoveTargets(mSharedElementTransition, mSharedElementTargets);
    }
    Int32 numViews;
    mHiddenViews->GetSize(&numViews);
    for (Int32 i = 0; i < numViews; i++) {
        AutoPtr<IInterface> obj;
        mHiddenViews->Get(i, (IInterface**)&obj);
        mOverallTransition->ExcludeTarget(IView::Probe(obj), FALSE);
    }
    mOverallTransition->ExcludeTarget(mNonExistingView, FALSE);
    *result = TRUE;
    return NOERROR;
}

void BackStackRecord::RemoveTargetedViewsFromTransitions(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* nonExistingView,
    /* [in] */ ITransition* enterTransition,
    /* [in] */ IArrayList* /*<View>*/ enteringViews,
    /* [in] */ ITransition* exitTransition,
    /* [in] */ IArrayList* /*<View>*/ exitingViews,
    /* [in] */ ITransition* sharedElementTransition,
    /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
    /* [in] */ ITransition* overallTransition,
    /* [in] */ IArrayList* /*<View>*/ hiddenViews)
{
    if (overallTransition != NULL) {
        AutoPtr<IViewTreeObserver> vto;
        IView::Probe(sceneRoot)->GetViewTreeObserver((IViewTreeObserver**)&vto);
        AutoPtr<IOnPreDrawListener> listener = new RemoveTargetsOnPreDrawListener(
            this, sceneRoot, nonExistingView, enterTransition, enteringViews,
            exitTransition, exitingViews, sharedElementTransition, sharedElementTargets,
            overallTransition, hiddenViews);

        vto->AddOnPreDrawListener(listener);
    }
}

void BackStackRecord::RemoveTargets(
    /* [in] */ ITransition* transition,
    /* [in] */ IArrayList* /*<View>*/ views)
{
    Int32 numViews;
    views->GetSize(&numViews);
    for (Int32 i = 0; i < numViews; i++) {
        AutoPtr<IInterface> obj;
        views->Get(i, (IInterface**)&obj);
        transition->RemoveTarget(IView::Probe(obj));
    }
}

void BackStackRecord::AddTargets(
    /* [in] */ ITransition* transition,
    /* [in] */ IArrayList* /*<View>*/ views)
{
    Int32 numViews;
    views->GetSize(&numViews);
    for (Int32 i = 0; i < numViews; i++) {
        AutoPtr<IInterface> obj;
        views->Get(i, (IInterface**)&obj);
        transition->AddTarget(IView::Probe(obj));
    }
}

AutoPtr<IArrayMap> /*<String, View>*/ BackStackRecord::RemapNames(
    /* [in] */ IArrayList* /* <String> */ inMap,
    /* [in] */ IArrayList* /* <String> */ toGoInMap,
    /* [in] */ IArrayMap* /* <String, View> */ namedViews)
{
    AutoPtr<IArrayMap> remappedViews;
    CArrayMap::New((IArrayMap**)&remappedViews);

    Boolean empty;
    IMap::Probe(namedViews)->IsEmpty(&empty);
    if (!empty) {
        Int32 numKeys;
        inMap->GetSize(&numKeys);
        for (Int32 i = 0; i < numKeys; i++) {
            AutoPtr<IInterface> key, value;
            inMap->Get(i, (IInterface**)&key);
            IMap::Probe(namedViews)->Get(key, (IInterface**)&value);
            AutoPtr<IView> view = IView::Probe(value);

            if (view != NULL) {
                AutoPtr<IInterface> toKey;
                toGoInMap->Get(i, (IInterface**)&toKey);
                IMap::Probe(remappedViews)->Put(toKey, view);
            }
        }
    }
    return remappedViews;
}

AutoPtr<IArrayMap> /*<String, View>*/ BackStackRecord::MapEnteringSharedElements(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IFragment* inFragment,
    /* [in] */ Boolean isBack)
 {
    AutoPtr<IArrayMap> namedViews; //ArrayMap<String, View>
    CArrayMap::New((IArrayMap**)&namedViews);

    AutoPtr<IView> root;
    inFragment->GetView((IView**)&root);
    if (root != NULL) {
        if (mSharedElementSourceNames != NULL) {
            root->FindNamedViews(IMap::Probe(namedViews));
            if (isBack) {
                AutoPtr<IArrayMap> old = namedViews;
                namedViews = RemapNames(mSharedElementSourceNames,
                        mSharedElementTargetNames, old);
            } else {
                Boolean result;
                namedViews->RetainAll(ICollection::Probe(mSharedElementTargetNames), &result);
            }
        }
    }
    return namedViews;
}

void BackStackRecord::ExcludeHiddenFragments(
    /* [in] */ IArrayList* /* <View> */ hiddenFragmentViews,
    /* [in] */ Int32 containerId,
    /* [in] */ ITransition* transition)
{
    if (mManager->mAdded != NULL) {
        Int32 N;
        mManager->mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mManager->mAdded->Get(i, (IInterface**)&obj);
            IFragment* fragment = IFragment::Probe(obj);
            AutoPtr<IView> view;
            fragment->GetView((IView**)&view);
            AutoPtr<IViewGroup> container;
            fragment->GetContainer((IViewGroup**)&container);
            Int32 id;
            fragment->GetContainerId(&id);
            if (view != NULL && container != NULL && id == containerId) {
                Boolean hidden;
                fragment->IsHidden(&hidden);
                if (hidden) {
                    Boolean contains;
                    hiddenFragmentViews->Contains(view, &contains);
                    if (!contains) {
                        transition->ExcludeTarget(view, TRUE);
                        hiddenFragmentViews->Add(view);
                    }
                }
                else {
                    transition->ExcludeTarget(view, FALSE);
                    hiddenFragmentViews->Remove(view);
                }
            }
        }
    }
}

//=========================================================================
CAR_INTERFACE_IMPL(BackStackRecord::EpicenterCallback, Object, IEpicenterCallback)

BackStackRecord::EpicenterCallback::EpicenterCallback(
    /* [in] */ IRect* epicenter)
    : mEpicenter(epicenter)
{}

ECode BackStackRecord::EpicenterCallback::OnGetEpicenter(
    /* [in] */ ITransition* transition,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEpicenter;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=========================================================================

void BackStackRecord::SetEpicenter(
    /* [in] */ ITransition* transition,
    /* [in] */ IView* view)
{
    AutoPtr<IRect> epicenter;
    CRect::New((IRect**)&epicenter);
    view->GetBoundsOnScreen(epicenter);

    AutoPtr<IEpicenterCallback> cb = new EpicenterCallback(epicenter);
    transition->SetEpicenterCallback(cb);
}

//=========================================================================
CAR_INTERFACE_IMPL(BackStackRecord::SharedElementEpicenterCallback, Object, IEpicenterCallback)

BackStackRecord::SharedElementEpicenterCallback::SharedElementEpicenterCallback(
    /* [in] */ IBackStackRecordTransitionState* state)
    : mState(state)
{
}

ECode BackStackRecord::SharedElementEpicenterCallback::OnGetEpicenter(
    /* [in] */ ITransition* transition,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    if (mEpicenter == NULL && (mState->GetEnteringEpicenterView((IView**)&view), view != NULL)) {
        CRect::New((IRect**)&mEpicenter);
        view->GetBoundsOnScreen(mEpicenter);
    }

    *result = mEpicenter;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=========================================================================

void BackStackRecord::SetSharedElementEpicenter(
    /* [in] */ ITransition* transition,
    /* [in] */ IBackStackRecordTransitionState* state)
{
    AutoPtr<IEpicenterCallback> cb = new SharedElementEpicenterCallback(state);
    transition->SetEpicenterCallback(cb);
}

ECode BackStackRecord::PopFromBackStack(
    /* [in] */ Boolean doStateMove,
    /* [in] */ IBackStackRecordTransitionState* inState,
    /* [in] */ ISparseArray* firstOutFragments,
    /* [in] */ ISparseArray* lastInFragments,
    /* [out] */ IBackStackRecordTransitionState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (FragmentManagerImpl::DEBUG) {
        Logger::V(TAG, "PopFromBackStack: %s", TO_CSTR(this));
//        LogWriter logw/* = new LogWriter(Logger::VERBOSE, TAG)*/;
       //  AutoPtr<IPrintWriter> pw;
       //  FastPrintWriter::New(logw, FALSE, 1024, (IPrintWriter**)&pw);
       //  Dump(String("  "), NULL, pw, NULL);
    }

    AutoPtr<IBackStackRecordTransitionState> state = inState;
    if (state == NULL) {
        Int32 fsize, lsize;
        if ((firstOutFragments->GetSize(&fsize), fsize != 0) ||
            (lastInFragments->GetSize(&lsize), lsize != 0)) {
            state = BeginTransition(firstOutFragments, lastInFragments, TRUE);
        }
    } else if (!doStateMove) {
        SetNameOverrides(state, mSharedElementTargetNames, mSharedElementSourceNames);
    }

    BumpBackStackNesting(-1);

    AutoPtr<Op> op = mTail;
    while (op != NULL) {
        switch (op->mCmd) {
            case OP_ADD: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopExitAnim);
                Int32 newTransition;
                FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                mManager->RemoveFragment(f, newTransition, mTransitionStyle);
            } break;

            case OP_REPLACE: {
                AutoPtr<IFragment> f = op->mFragment;
                if (f != NULL) {
                    f->SetNextAnim(op->mPopExitAnim);
                    Int32 newTransition;
                    FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                    mManager->RemoveFragment(f, newTransition, mTransitionStyle);
                }
                if (op->mRemoved != NULL) {
                    Int32 size;
                    op->mRemoved->GetSize(&size);
                    for (Int32 i = 0; i < size; i++) {
                        AutoPtr<IInterface> obj;
                        op->mRemoved->Get(i, (IInterface**)&obj);
                        IFragment* old = IFragment::Probe(obj);
                        old->SetNextAnim(op->mPopEnterAnim);
                        mManager->AddFragment(old, FALSE);
                    }
                }
            } break;

            case OP_REMOVE: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopEnterAnim);
                mManager->AddFragment(f, FALSE);
            } break;

            case OP_HIDE: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopEnterAnim);
                Int32 newTransition;
                FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                mManager->ShowFragment(f, newTransition, mTransitionStyle);
            } break;

            case OP_SHOW: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopExitAnim);
                Int32 newTransition;
                FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                mManager->HideFragment(f, newTransition, mTransitionStyle);
            } break;

            case OP_DETACH: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopEnterAnim);
                Int32 newTransition;
                FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                mManager->AttachFragment(f, newTransition, mTransitionStyle);
            } break;

            case OP_ATTACH: {
                AutoPtr<IFragment> f = op->mFragment;
                f->SetNextAnim(op->mPopExitAnim);
                Int32 newTransition;
                FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
                mManager->DetachFragment(f, newTransition, mTransitionStyle);
            } break;

            default: {
                //throw new IllegalArgumentException("Unknown cmd: " + op->cmd);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        op = op->mPrev;
    }

    if (doStateMove) {
        Int32 newTransition;
        FragmentManagerImpl::ReverseTransit(mTransition, &newTransition);
        mManager->MoveToState(mManager->mCurState, newTransition, mTransitionStyle, TRUE);
        state = NULL;
    }

    if (mIndex >= 0) {
        mManager->FreeBackStackIndex(mIndex);
        mIndex = -1;
    }

    *result = state;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void BackStackRecord::SetNameOverride(
    /* [in] */ IArrayMap* /*<String, String>*/ overrides,
    /* [in] */ const String& source,
    /* [in] */ const String& target)
{
    if (!source.IsNull() && !target.IsNull() && !source.Equals(target)) {
        IMap* map = IMap::Probe(overrides);
        AutoPtr<ICharSequence> targetObj = CoreUtils::Convert(target);
        Int32 size;
        map->GetSize(&size);
        for (Int32 index = 0; index < size; index++) {
            AutoPtr<IInterface> obj;
            overrides->GetValueAt(index, (IInterface**)&obj);

            if (source.Equals(Object::ToString(obj))) {
                overrides->SetValueAt(index, targetObj);
                return;
            }
        }
        AutoPtr<ICharSequence> sourceObj = CoreUtils::Convert(source);
        map->Put(sourceObj, targetObj);
    }
}

void BackStackRecord::SetNameOverride(
    /* [in] */ IArrayMap* /*<String, String>*/ overrides,
    /* [in] */ ICharSequence* source,
    /* [in] */ ICharSequence* target)
{
    if (source != NULL && target != NULL && !Object::Equals(source, target)) {
        IMap* map = IMap::Probe(overrides);
        IInterface* src = TO_IINTERFACE(source);
        IInterface* tgt = TO_IINTERFACE(target);
        Int32 size;
        map->GetSize(&size);
        for (Int32 index = 0; index < size; index++) {
            AutoPtr<IInterface> obj;
            overrides->GetValueAt(index, (IInterface**)&obj);

            if (Object::Equals(src, obj)) {
                overrides->SetValueAt(index, tgt);
                return;
            }
        }
        map->Put(src, tgt);
    }
}

void BackStackRecord::SetNameOverrides(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IArrayList* /*<String>*/ sourceNames,
    /* [in] */ IArrayList* /*<String>*/ targetNames)
{
    if (sourceNames != NULL) {
        Int32 size;
        sourceNames->GetSize(&size);
        AutoPtr<IArrayMap> nameOverrides;
        state->GetNameOverrides((IArrayMap**)&nameOverrides);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> srcObj, tgtObj;
            sourceNames->Get(i, (IInterface**)&srcObj);
            targetNames->Get(i, (IInterface**)&tgtObj);

            SetNameOverride(nameOverrides,
                ICharSequence::Probe(srcObj), ICharSequence::Probe(tgtObj));
        }
    }
}

void BackStackRecord::SetBackNameOverrides(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IArrayMap* /*<String, View>*/ namedViews,
    /* [in] */ Boolean isEnd)
{
    AutoPtr<IArrayMap> nameOverrides;
    state->GetNameOverrides((IArrayMap**)&nameOverrides);
    Int32 count;
    mSharedElementTargetNames->GetSize(&count);
    IMap* map = IMap::Probe(namedViews);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> srcObj, tgtObj, viewObj;
        mSharedElementSourceNames->Get(i, (IInterface**)&srcObj);
        mSharedElementTargetNames->Get(i, (IInterface**)&tgtObj);

        map->Get(tgtObj, (IInterface**)&viewObj);
        if (viewObj != NULL) {
            String target;
            IView::Probe(viewObj)->GetTransitionName(&target);
            if (isEnd) {
                SetNameOverride(nameOverrides, ICharSequence::Probe(srcObj), CoreUtils::Convert(target));
            } else {
                SetNameOverride(nameOverrides, CoreUtils::Convert(target), ICharSequence::Probe(srcObj));
            }
        }
    }
}

void BackStackRecord::SetNameOverrides(
    /* [in] */ IBackStackRecordTransitionState* state,
    /* [in] */ IArrayMap* namedViews,
    /* [in] */ Boolean isEnd)
{
    IMap* map = IMap::Probe(namedViews);
    AutoPtr<IArrayMap> nameOverrides;
    state->GetNameOverrides((IArrayMap**)&nameOverrides);
    Int32 count;
    map->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> srcObj, viewObj;
        nameOverrides->GetKeyAt(i, (IInterface**)&srcObj);
        nameOverrides->GetValueAt(i, (IInterface**)&viewObj);
        String target;
        IView::Probe(viewObj)->GetTransitionName(&target);

        if (isEnd) {
            SetNameOverride(nameOverrides, ICharSequence::Probe(srcObj), CoreUtils::Convert(target));
        } else {
            SetNameOverride(nameOverrides, CoreUtils::Convert(target), ICharSequence::Probe(srcObj));
        }
    }
}

ECode BackStackRecord::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode BackStackRecord::GetTransition(
    /* [out] */ Int32* trans)
{
    VALIDATE_NOT_NULL(trans);
    *trans = mTransition;
    return NOERROR;
}

ECode BackStackRecord::GetTransitionStyle(
    /* [out] */ Int32* transStyle)
{
    VALIDATE_NOT_NULL(transStyle);
    *transStyle = mTransitionStyle;
    return NOERROR;
}

ECode BackStackRecord::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);
    *empty = mNumOp == 0;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
