
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
    AutoPtr<ISpanned> sp;
    if((sp = ISpanned::Probe(mSource), sp)!=NULL)
    {
        Int32 st;
        sp->GetSpanStart(CTextKeyListener::ACTIVE, &st);
        Int32 en;
        sp->GetSpanEnd(CTextKeyListener::ACTIVE, &en);

        if (i >= st && i < en) {
            mSource->GetCharAt(i, c);
            return NOERROR;
        }

        AutoPtr< ArrayOf< IPasswordTransformationMethodVisible* > > visible;
        Int32 spLen;
        ISpanned::Probe(sp)->GetSpans(0, (ICharSequence::Probe(sp)->GetLength(&spLen), spLen), EIID_IPasswordTransformationMethodVisible, (ArrayOf< IInterface* >**)&visible);

        for (Int32 a = 0; a < visible->GetLength(); a++) {
            Int32 start;
            if ((sp->GetSpanStart( (((Visible*)(*visible)[a]))->mTransformer, &start), start) >= 0) {
                sp->GetSpanStart((*visible)[a], &st);
                sp->GetSpanEnd((*visible)[a], &en);

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
    //Java:    char[] buf = new char[end - start]; getChars(start, end, buf, 0); return new String(buf);
    AutoPtr< ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(end - start);
    GetChars(start, end, buf, 0);
    StringBuilder sb;
    sb.Append(*buf);
    AutoPtr<ICharSequence> cs;
    CString::New(sb.ToString(), (ICharSequence**)&cs);
    *ret = cs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CPasswordTransformationMethod::PasswordCharSequence::ToString(
    /* [out] */ String* str)
{
    //Java:return subSequence(0, length()).toString();
    Int32 len;
    AutoPtr<ICharSequence> cs;
    SubSequence(0, (GetLength(&len), len), (ICharSequence**)&cs);
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

    AutoPtr<ISpanned> sp;
    if((sp = ISpanned::Probe(mSource), sp)!=NULL){
        sp->GetSpanStart(CTextKeyListener::ACTIVE, &st);
        sp->GetSpanEnd(CTextKeyListener::ACTIVE, &en);

        AutoPtr< ArrayOf< IPasswordTransformationMethodVisible* > > visible;
        Int32 spLen;
        AutoPtr<ArrayOf<IInterface*> > arrayTemp;

        ISpanned::Probe(sp)->GetSpans(0, (ICharSequence::Probe(sp)->GetLength(&spLen), spLen), EIID_IPasswordTransformationMethodVisible, (ArrayOf<IInterface*>**)&arrayTemp);
        visible = ArrayOf< IPasswordTransformationMethodVisible* >::Alloc(arrayTemp->GetLength());
        for (Int32 len = 0; len < arrayTemp->GetLength(); len++) {
            visible->Set(len, IPasswordTransformationMethodVisible::Probe((*arrayTemp)[len]));
        }

        nvisible = visible->GetLength();
        starts = ArrayOf<Int32>::Alloc(nvisible);
        ends = ArrayOf<Int32>::Alloc(nvisible);

        for (Int32 i = 0; i < nvisible; i++) {
            Int32 startP;
            sp->GetSpanStart(((Visible*)((*visible)[i]))->mTransformer, &startP);
            if (startP >= 0) {
                sp->GetSpanStart((IInterface*)(((*visible)[i])->Probe(EIID_IInterface)), &((*starts)[i]));
                sp->GetSpanEnd((IInterface*)(((*visible)[i]->Probe(EIID_IInterface))), &((*ends)[i]));
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

CAR_INTERFACE_IMPL_4(CPasswordTransformationMethod::Visible, Handler, IUpdateLayout, IUpdateAppearance, IRunnable, IPasswordTransformationMethodVisible)

ECode CPasswordTransformationMethod::Visible::constructor(
    /* [in] */ ISpannable* sp,
    /* [in] */ IPasswordTransformationMethod* ptm)
{
    Handler::constructor();

    mText = sp;
    mTransformer = ptm;
    Boolean result;
    return PostAtTime(IRunnable::Probe(this), SystemClock::GetUptimeMillis() + 1500, &result);
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
CAR_INTERFACE_IMPL_4(CPasswordTransformationMethod, Object, IPasswordTransformationMethod, ITransformationMethod, ITextWatcher, INoCopySpan)

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
        //Java:    ViewReference[] vr = sp.getSpans(0, sp.length(), ViewReference.class);
        AutoPtr< ArrayOf< IView* > > vr;
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        ISpanned::Probe(sp)->GetSpans(0, spLen, EIID_IView, (ArrayOf< IInterface* >**)&vr);

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
        CPasswordTransformationMethod::NewByFriend((CPasswordTransformationMethod**)&sInstance);
    }

    *ret = (IPasswordTransformationMethod*)sInstance.Get();
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
        //Java:    ViewReference[] vr = sp.getSpans(0, s.length(), ViewReference.class);
        AutoPtr< ArrayOf< IView* > > vr;
        Int32 spLen;
        ICharSequence::Probe(sp)->GetLength(&spLen);
        ISpanned::Probe(sp)->GetSpans(0, spLen, EIID_IView, (ArrayOf< IInterface* >**)&vr);
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
        AutoPtr<IView> v = NULL;
        for (Int32 i = 0; v == NULL && i < vr->GetLength(); i++) {
            v = ((*vr)[i]);
        }

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
                    sp->SetSpan(IPasswordTransformationMethod::Probe(visible), start, start + count, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
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
    //Java:    Visible[] old = sp.getSpans(0, sp.length(), Visible.class);
    AutoPtr< ArrayOf< IPasswordTransformationMethodVisible* > > old;
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

