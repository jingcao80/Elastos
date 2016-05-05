
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/text/method/TextKeyListener.h"
#include "elastos/droid/text/method/CTextKeyListener.h"
#include "elastos/droid/text/method/CQwertyKeyListener.h"
#include "elastos/droid/text/method/CMultiTapKeyListener.h"
#include "elastos/droid/text/SpannableStringInternal.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::EIID_IContentResolver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::Method::CQwertyKeyListener;
using Elastos::Droid::View::IKeyCharacterMap;


namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<TextKeyListener::NullKeyListener> TextKeyListener::NullKeyListener::sInstance;

CAR_INTERFACE_IMPL(TextKeyListener::NullKeyListener, Object, IKeyListener)

ECode TextKeyListener::NullKeyListener::GetInputType(
    /* [out] */ Int32* inputType)
{
    VALIDATE_NOT_NULL(inputType);
    *inputType = IInputType::TYPE_NULL;
    return NOERROR;
}

ECode TextKeyListener::NullKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode TextKeyListener::NullKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode TextKeyListener::NullKeyListener::OnKeyOther(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode TextKeyListener::NullKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return NOERROR;
}

AutoPtr<TextKeyListener::NullKeyListener> TextKeyListener::NullKeyListener::GetInstance()
{
    if (sInstance != NULL)
        return sInstance;

    sInstance = new NullKeyListener();
    return sInstance;
}

TextKeyListener::SettingsObserver::SettingsObserver(
    /* [in] */ TextKeyListener* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(TextKeyListener::SettingsObserver, Object, IContentObserver)

//@Override
ECode TextKeyListener::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (mHost->mResolver != NULL) {
        AutoPtr<IInterface> obj;
        mHost->mResolver->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj == NULL) {
            mHost->mPrefsInited = FALSE;
        }
        else {
            AutoPtr<IContentResolver> resolver = IContentResolver::Probe(obj);
            mHost->UpdatePrefs(resolver);
        }
    }
    else {
        mHost->mPrefsInited = FALSE;
    }
    return NOERROR;
}

//override
using Elastos::Droid::Database::IIContentObserver;
ECode TextKeyListener::SettingsObserver::GetContentObserver(
    /* [out] */ IIContentObserver** observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::ReleaseContentObserver(
    /* [out] */ IIContentObserver** observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::DispatchChange(
    /* [in] */ Boolean selfChange)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return E_NOT_IMPLEMENTED;
}

ECode TextKeyListener::SettingsObserver::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    return E_NOT_IMPLEMENTED;
}

const AutoPtr<IInterface> TextKeyListener::ACTIVE /*= MetaKeyKeyListener::NewNoCopySpan()*/;
const AutoPtr<IInterface> TextKeyListener::CAPPED /*= MetaKeyKeyListener::NewNoCopySpan()*/;
const AutoPtr<IInterface> TextKeyListener::INHIBIT_REPLACEMENT/* = MetaKeyKeyListener::NewNoCopySpan()*/;
const AutoPtr<IInterface> TextKeyListener::LAST_TYPED/* = MetaKeyKeyListener::NewNoCopySpan()*/;

const Int32 TextKeyListener::AUTO_CAP = 1;
const Int32 TextKeyListener::AUTO_TEXT = 2;
const Int32 TextKeyListener::AUTO_PERIOD = 4;
const Int32 TextKeyListener::SHOW_PASSWORD = 8;

const Int32 TextKeyListener::CAPITALIZELENGTH = 4;
AutoPtr<ArrayOf<ITextKeyListener*> > TextKeyListener::sInstance = ArrayOf<ITextKeyListener*>::Alloc(CAPITALIZELENGTH * 2);

CAR_INTERFACE_IMPL_6(TextKeyListener, Object, ITextKeyListener, IBaseKeyListener, IMetaKeyKeyListener, IKeyListener, ISpanWatcher, INoCopySpan)

TextKeyListener::TextKeyListener()
{}

TextKeyListener::~TextKeyListener()
{}

ECode TextKeyListener::constructor(
    /* [in] */ Capitalize cap,
    /* [in] */ Boolean autotext)
{
    mAutoCap = cap;
    mAutoText = autotext;
    return NOERROR;
}

/**
 * Returns a new or existing instance with no automatic capitalization
 * or correction.
 */
ECode TextKeyListener::GetInstance(
        /* [out] */ ITextKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    return GetInstance(FALSE, Capitalize_NONE, ret);
}

/**
 * Returns a new or existing instance with the specified capitalization
 * and correction properties.
 *
 * @param cap when, if ever, to automatically capitalize.
 * @param autotext whether to automatically do spelling corrections.
 */
ECode TextKeyListener::GetInstance(
        /* [in] */ Boolean autotext,
        /* [in] */ Capitalize cap,
        /* [out] */ ITextKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 off = cap * 2 + (autotext ? 1 : 0);

    if ((*sInstance)[off] == NULL) {
        AutoPtr<ITextKeyListener> listener;
        CTextKeyListener::New(cap, autotext, (ITextKeyListener**)&listener);
        sInstance->Set(off, listener);
    }

    *ret = (*sInstance)[off];
    REFCOUNT_ADD(*ret)
    return NOERROR;
}



/**
 * Returns whether it makes sense to automatically capitalize at the
 * specified position in the specified text, with the specified rules.
 *
 * @param cap the capitalization rules to consider.
 * @param cs the text in which an insertion is being made.
 * @param off the offset into that text where the insertion is being made.
 *
 * @return whether the character being inserted should be capitalized.
 */
ECode TextKeyListener::ShouldCap(
    /* [in] */ Capitalize cap,
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 off,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    if (cap == Capitalize_NONE) {
        *ret = FALSE;
        return NOERROR;
    }
    if (cap == Capitalize_CHARACTERS) {
        *ret = TRUE;
        return NOERROR;
    }

    *ret = TextUtils::GetCapsMode(cs, off, cap == Capitalize_WORDS
        ? ITextUtils::CAP_MODE_WORDS : ITextUtils::CAP_MODE_SENTENCES)
            != 0;
    return NOERROR;
}

ECode TextKeyListener::GetInputType(
        /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    return BaseKeyListener::MakeTextContentType(mAutoCap, mAutoText, ret);
}

//@Override
ECode TextKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IKeyListener> im = GetKeyListener(event);

    return im->OnKeyDown(view, content, keyCode, event, ret);
}

//@Override
ECode TextKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IKeyListener> im = GetKeyListener(event);

    return im->OnKeyUp(view, content, keyCode, event, ret);
}

//@Override
ECode TextKeyListener::OnKeyOther(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IKeyListener> im = GetKeyListener(event);

    return im->OnKeyOther(view, content, event, ret);
}

/**
 * Clear all the input state (autotext, autocap, multitap, undo)
 * from the specified Editable, going beyond Editable.clear(), which
 * just clears the text but not the input state.
 *
 * @param e the buffer whose text and state are to be cleared.
 */
ECode TextKeyListener::Clear(
    /* [in] */ IEditable* e)
{
    e->Clear();
    ISpannable::Probe(e)->RemoveSpan(ACTIVE);
    ISpannable::Probe(e)->RemoveSpan(CAPPED);
    ISpannable::Probe(e)->RemoveSpan(INHIBIT_REPLACEMENT);
    ISpannable::Probe(e)->RemoveSpan(LAST_TYPED);

    Int32 len;
    ICharSequence::Probe(e)->GetLength(&len);
    AutoPtr< ArrayOf<IInterface*> > repl = NULL;
    ISpanned::Probe(e)->GetSpans(0, len, /*EIID_Replaced*/EIID_INoCopySpan, (ArrayOf<IInterface*>**)&repl);
    Int32 count = repl->GetLength();
    for (Int32 i = 0; i < count; i++) {
        ISpannable::Probe(e)->RemoveSpan((*repl)[i]);
    }
    return NOERROR;
}

ECode TextKeyListener::OnSpanAdded(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return NOERROR;
}

ECode TextKeyListener::OnSpanRemoved(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return NOERROR;
}

ECode TextKeyListener::OnSpanChanged(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 st,
    /* [in] */ Int32 en)
{
    if (what == Selection::SELECTION_END) {
        s->RemoveSpan(ACTIVE);
    }
    return NOERROR;
}

AutoPtr<IKeyListener> TextKeyListener::GetKeyListener(
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IKeyCharacterMap> kmap;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&kmap);
    Int32 kind;
    kmap->GetKeyboardType(&kind);

    if (kind == IKeyCharacterMap::ALPHA) {
        AutoPtr<IQwertyKeyListener> qkl;
        QwertyKeyListener::GetInstance(mAutoText, mAutoCap, (IQwertyKeyListener**)&qkl);
        return IKeyListener::Probe(qkl);
    }
    else if (kind == IKeyCharacterMap::NUMERIC) {
        AutoPtr<IMultiTapKeyListener> mtkl;
        MultiTapKeyListener::GetInstance(mAutoText, mAutoCap, (IMultiTapKeyListener**)&mtkl);
        return IKeyListener::Probe(mtkl);
    }
    else if (kind == IKeyCharacterMap::FULL
        || kind == IKeyCharacterMap::SPECIAL_FUNCTION) {
        // We consider special function keyboards full keyboards as a workaround for
        // devices that do not have built-in keyboards.  Applications may try to inject
        // key events using the built-in keyboard device id which may be configured as
        // a special function keyboard using a default key map.  Ideally, as of Honeycomb,
        // these applications should be modified to use KeyCharacterMap.VIRTUAL_KEYBOARD.

        AutoPtr<IQwertyKeyListener> listener;
        QwertyKeyListener::GetInstanceForFullKeyboard((IQwertyKeyListener**)&listener);
        return IKeyListener::Probe(listener);
    }

    AutoPtr<NullKeyListener> nkl = NullKeyListener::GetInstance();
    return IKeyListener::Probe(nkl);
}

ECode TextKeyListener::ReleaseListener()
{
    if (mResolver != NULL) {
        AutoPtr<IInterface> obj;
        mResolver->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj != NULL) {
            AutoPtr<IContentResolver> resolver = IContentResolver::Probe(obj);
            resolver->UnregisterContentObserver((IContentObserver*)mObserver);
        }

        mObserver = NULL;
        mResolver = NULL;
        mPrefsInited = FALSE;
    }

    return NOERROR;
}

void TextKeyListener::InitPrefs(
    /* [in] */ IContext* context)
{
    mResolver = NULL;
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    if (contentResolver != NULL) {
        IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(contentResolver);
        assert(wrs != NULL);
        wrs->GetWeakReference((IWeakReference**)&mResolver);
    }

    if (mObserver == NULL) {
        mObserver = new SettingsObserver(this);
        contentResolver->RegisterContentObserver(Settings::System::CONTENT_URI.Get(), TRUE, (IContentObserver*)mObserver);
    }

    UpdatePrefs(contentResolver);
    mPrefsInited = TRUE;
}

void TextKeyListener::UpdatePrefs(
    /* [in] */ IContentResolver* resolver)
{
    Boolean cap;
    Int32 value;
    Settings::System::GetInt32(resolver, ISettingsSystem::TEXT_AUTO_CAPS, 1, &value);
    cap = value > 0;
    Boolean text;
    Settings::System::GetInt32(resolver, ISettingsSystem::TEXT_AUTO_REPLACE, 1, &value);
    text = value > 0;
    Boolean period;
    Settings::System::GetInt32(resolver, ISettingsSystem::TEXT_AUTO_PUNCTUATE, 1, &value);
    period = value > 0;
    Boolean pw;
    Settings::System::GetInt32(resolver, ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &value);
    pw = value > 0;

    mPrefs = (cap ? AUTO_CAP : 0) |
             (text ? AUTO_TEXT : 0) |
             (period ? AUTO_PERIOD : 0) |
             (pw ? SHOW_PASSWORD : 0);
}

ECode TextKeyListener::GetPrefs(
    /* [in] */ IContext* context,
    /* [out] */ Int32* ret)
{
    synchronized(mLock) {
        if (!mPrefsInited || mResolver == NULL) {
            InitPrefs(context);
        }
    }

    *ret = mPrefs;
    return NOERROR;
}

ECode TextKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return E_NOT_IMPLEMENTED;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
