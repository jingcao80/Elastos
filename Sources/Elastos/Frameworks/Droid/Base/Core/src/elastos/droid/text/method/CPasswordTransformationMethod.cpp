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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/method/CPasswordTransformationMethod.h"
#include "elastos/droid/text/method/CTextKeyListener.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Text::Method::CTextKeyListener;
using Elastos::Droid::Text::Style::EIID_IUpdateLayout;
using Elastos::Droid::Text::Style::EIID_IUpdateAppearance;
using Elastos::Droid::Text::Style::IUpdateAppearance;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IPasswordTransformationMethod> CPasswordTransformationMethod::sInstance;
Char32 CPasswordTransformationMethod::DOT = 0x2022;

/*****************************CPasswordTransformationMethod::PasswordCharSequence*****************************/
CAR_INTERFACE_IMPL_2(CPasswordTransformationMethod::PasswordCharSequence, Object, IGetChars, ICharSequence)

CPasswordTransformationMethod::PasswordCharSequence::PasswordCharSequence()
{}

ECode CPasswordTransformationMethod::PasswordCharSequence::constructor(
    /* [in] */ ICharSequence* source)
{
    mSource = source;
    return NOERROR;
}

ECode CPasswordTransformationMethod::PasswordCharSequence::GetLength(
    /* [out] */ Int32* number)
{
    return mSource->GetLength(number);
}

ECode CPasswordTransformationMethod::PasswordCharSequence::GetCharAt(
    /* [in] */ Int32 i,
    /* [out] */ Char32* c)
{
    AutoPtr<ISpanned> sp = ISpanned::Probe(mSource);
    if (sp != NULL) {
        Int32 st, en;
        sp->GetSpanStart(CTextKeyListener::ACTIVE, &st);
        sp->GetSpanEnd(CTextKeyListener::ACTIVE, &en);

        if (i >= st && i < en) {
            mSource->GetCharAt(i, c);
            return NOERROR;
        }

        AutoPtr< ArrayOf< IInterface* > > visibles; // IPasswordTransformationMethodVisible
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        sp->GetSpans(0, spLen, EIID_IPasswordTransformationMethodVisible, (ArrayOf< IInterface* >**)&visibles);

        for (Int32 a = 0; a < visibles->GetLength(); a++) {
            IPasswordTransformationMethodVisible* ptmv = IPasswordTransformationMethodVisible::Probe((*visibles)[i]);
            Visible* visible = (Visible*)ptmv;
            Int32 start;
            if ((sp->GetSpanStart(visible->mTransformer, &start), start) >= 0) {
                sp->GetSpanStart((*visibles)[a], &st);
                sp->GetSpanEnd((*visibles)[a], &en);

                if (i >= st && i < en) {
                    mSource->GetCharAt(i, c);
                    return NOERROR;
                }
            }
        }
    }
    *c=DOT;
    return NOERROR;
}

ECode CPasswordTransformationMethod::PasswordCharSequence::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr< ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(end - start);
    GetChars(start, end, buf, 0);
    StringBuilder sb;
    sb.Append(*buf);
    AutoPtr<ICharSequence> cs = sb.ToCharSequence();
    *ret = cs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CPasswordTransformationMethod::PasswordCharSequence::ToString(
    /* [out] */ String* str)
{
    Int32 len;
    GetLength(&len);
    AutoPtr<ICharSequence> cs;
    SubSequence(0, len, (ICharSequence**)&cs);
    cs->ToString(str);
    return NOERROR;
}

ECode CPasswordTransformationMethod::PasswordCharSequence::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 off)
{
    TextUtils::GetChars(mSource, start, end, dest, off);

    Int32 st = -1, en = -1;
    Int32 nvisible = 0;
    AutoPtr< ArrayOf<Int32> > starts = NULL, ends = NULL;

    AutoPtr<ISpanned> sp = ISpanned::Probe(mSource);
    if (sp != NULL) {
        sp->GetSpanStart(CTextKeyListener::ACTIVE, &st);
        sp->GetSpanEnd(CTextKeyListener::ACTIVE, &en);

        AutoPtr< ArrayOf< IInterface* > > visibles; //IPasswordTransformationMethodVisible
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        sp->GetSpans(0, spLen, EIID_IPasswordTransformationMethodVisible, (ArrayOf<IInterface*>**)&visibles);

        nvisible = visibles->GetLength();
        starts = ArrayOf<Int32>::Alloc(nvisible);
        ends = ArrayOf<Int32>::Alloc(nvisible);

        for (Int32 i = 0; i < nvisible; i++) {
            IPasswordTransformationMethodVisible* ptmv = IPasswordTransformationMethodVisible::Probe((*visibles)[i]);
            Visible* visible = (Visible*)ptmv;
            Int32 startP;
            sp->GetSpanStart(visible->mTransformer, &startP);
            if (startP >= 0) {
                sp->GetSpanStart((*visibles)[i], &((*starts)[i]));
                sp->GetSpanEnd((*visibles)[i], &((*ends)[i]));
             }
        }
    }

    for (Int32 i = start; i < end; i++) {
        if (! (i >= st && i < en)) {
            Boolean visible = FALSE;

            for (Int32 a = 0; a < nvisible; a++) {
                if (i >= (*starts)[a] && i < (*ends)[a]) {
                    visible = TRUE;
                    break;
                }
            }

            if (!visible) {
                (*dest)[i - start + off] = DOT;
            }
        }
    }
    return NOERROR;
}

/*****************************CPasswordTransformationMethod::Visible*****************************/
CPasswordTransformationMethod::Visible::Visible()
{}

CAR_INTERFACE_IMPL_4(CPasswordTransformationMethod::Visible, Handler, IUpdateLayout, \
    IUpdateAppearance, IRunnable, IPasswordTransformationMethodVisible)

ECode CPasswordTransformationMethod::Visible::constructor(
    /* [in] */ ISpannable* sp,
    /* [in] */ IPasswordTransformationMethod* ptm)
{
    Handler::constructor();

    mText = sp;
    mTransformer = ptm;
    Boolean result;
    return PostAtTime(this, SystemClock::GetUptimeMillis() + 1500, &result);
}

ECode CPasswordTransformationMethod::Visible::Run()
{
    mText->RemoveSpan(TO_IINTERFACE(this));
    return NOERROR;
}

CPasswordTransformationMethod::ViewReference::ViewReference()
{}

CAR_INTERFACE_IMPL_2(CPasswordTransformationMethod::ViewReference, Object, IWeakReference, INoCopySpan)

ECode CPasswordTransformationMethod::ViewReference::constructor(
    /* [in] */ IView* v)
{
    return NOERROR;
}

/*****************************CPasswordTransformationMethod*****************************/
CAR_INTERFACE_IMPL_4(CPasswordTransformationMethod, Object, IPasswordTransformationMethod, \
    ITransformationMethod, ITextWatcher, INoCopySpan)

CAR_OBJECT_IMPL(CPasswordTransformationMethod)

CPasswordTransformationMethod::CPasswordTransformationMethod()
{}

CPasswordTransformationMethod::~CPasswordTransformationMethod()
{}

ECode CPasswordTransformationMethod::constructor()
{
    return NOERROR;
}

ECode CPasswordTransformationMethod::GetTransformation(
    /* [in] */ ICharSequence* source,
    /* [in] */ IView* view,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);

    AutoPtr<ISpannable> sp = ISpannable::Probe(source);
    if (sp != NULL){
        /*
         * Remove any references to other views that may still be
         * attached.  This will happen when you flip the screen
         * while a password field is showing; there will still
         * be references to the old EditText in the text.
         */
        AutoPtr< ArrayOf< IInterface* > > vr; // ViewReference
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        ISpanned::Probe(sp)->GetSpans(0, spLen, EIID_INoCopySpan, (ArrayOf< IInterface* >**)&vr);

        for (Int32 i = 0; i < vr->GetLength(); i++) {
            sp->RemoveSpan((*vr)[i]);
        }

        RemoveVisibleSpans(sp);

        sp->SetSpan(view, 0, 0, ISpanned::SPAN_POINT_POINT);
    }

    AutoPtr<PasswordCharSequence> cs = new PasswordCharSequence();
    cs->constructor(source);
    *ret = cs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CPasswordTransformationMethod::GetInstance(
    /* [out] */ IPasswordTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret);
    if (sInstance == NULL) {
        CPasswordTransformationMethod::New((IPasswordTransformationMethod**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CPasswordTransformationMethod::BeforeTextChanged(
    /* [in] */ ICharSequence * s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    // This callback isn't used.
    return NOERROR;
}

ECode CPasswordTransformationMethod::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    AutoPtr<ISpannable> sp = ISpannable::Probe(s);
    if (sp != NULL) {
        AutoPtr< ArrayOf< IInterface* > > vr; //ViewReference
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        ISpanned::Probe(sp)->GetSpans(0, spLen, EIID_INoCopySpan, (ArrayOf< IInterface* >**)&vr);
        if (vr->GetLength() == 0) {
            return NOERROR;
        }

        /*
         * There should generally only be one ViewReference in the text,
         * but make sure to look through all of them if necessary in case
         * something strange is going on.  (We might still end up with
         * multiple ViewReferences if someone moves text from one password
         * field to another.)
         */
        AutoPtr<INoCopySpan> ncs;
        for (Int32 i = 0; vr == NULL && i < vr->GetLength(); i++) {
            ncs = INoCopySpan::Probe((*vr)[i]);
        }

        if (ncs == NULL) {
            return NOERROR;
        }
        ViewReference* vrObj = (ViewReference*)ncs.Get();

        AutoPtr<IView> v;
        vrObj->Resolve(EIID_IView, (IInterface**)&v);
        if (v == NULL) {
            return NOERROR;
        }

        AutoPtr<ITextKeyListener> tkl;
        TextKeyListener::GetInstance((ITextKeyListener**)&tkl);
        AutoPtr<IContext> context;
        v->GetContext((IContext**)&context);
        Int32 pref;
        ((TextKeyListener*)tkl.Get())->GetPrefs(context, &pref);
        if ((pref & TextKeyListener::SHOW_PASSWORD) != 0) {
            if (count > 0) {
                RemoveVisibleSpans(sp);

                if (count == 1) {
                    AutoPtr<IPasswordTransformationMethod> tpm = this;
                    AutoPtr<Visible> visible = new Visible();
                    visible->constructor(sp, tpm);
                    sp->SetSpan(IPasswordTransformationMethod::Probe(visible),
                        start, start + count, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            }
        }
    }
    return NOERROR;
}

ECode CPasswordTransformationMethod::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    // This callback isn't used.
    return NOERROR;
}

ECode CPasswordTransformationMethod::OnFocusChanged(
    /* [in] */ IView* view,
    /* [in] */ ICharSequence* sourceText,
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    if (!focused) {
        AutoPtr<ISpannable> sp = ISpannable::Probe(sourceText);
        if(sp!=NULL){
            RemoveVisibleSpans(sp);
        }
    }
    return NOERROR;
}

void CPasswordTransformationMethod::RemoveVisibleSpans(
    /* [in] */ ISpannable* sp)
{
    AutoPtr< ArrayOf< IInterface* > > old;
    Int32 spLen;
    ICharSequence::Probe(sp)->GetLength(&spLen);
    ISpanned::Probe(sp)->GetSpans(0, spLen, EIID_IPasswordTransformationMethodVisible, (ArrayOf< IInterface* >**)&old);
    for (Int32 i = 0; i < old->GetLength(); i++) {
        sp->RemoveSpan((*old)[i]);
    }
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

