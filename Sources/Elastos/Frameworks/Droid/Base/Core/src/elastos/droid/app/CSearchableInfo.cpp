
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/CSearchableInfo.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/app/CActionKeyInfo.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

const Boolean CSearchableInfo::DBG;
const String CSearchableInfo::TAG("SearchableInfo");
const String CSearchableInfo::MD_LABEL_SEARCHABLE("android.app.searchable");
const String CSearchableInfo::MD_XML_ELEMENT_SEARCHABLE("searchable");
const String CSearchableInfo::MD_XML_ELEMENT_SEARCHABLE_ACTION_KEY("actionkey");
const Int32 CSearchableInfo::SEARCH_MODE_BADGE_LABEL;
const Int32 CSearchableInfo::SEARCH_MODE_BADGE_ICON;
const Int32 CSearchableInfo::SEARCH_MODE_QUERY_REWRITE_FROM_DATA;
const Int32 CSearchableInfo::SEARCH_MODE_QUERY_REWRITE_FROM_TEXT;
const Int32 CSearchableInfo::VOICE_SEARCH_SHOW_BUTTON;
const Int32 CSearchableInfo::VOICE_SEARCH_LAUNCH_WEB_SEARCH;
const Int32 CSearchableInfo::VOICE_SEARCH_LAUNCH_RECOGNIZER;

CAR_INTERFACE_IMPL_2(CSearchableInfo, Object, ISearchableInfo, IParcelable)

CAR_OBJECT_IMPL(CSearchableInfo)

CSearchableInfo::CSearchableInfo()
    : mLabelId(0)
    , mHintId(0)
    , mSearchMode(0)
    , mIconId(0)
    , mSearchButtonText(0)
    , mSearchInputType(0)
    , mSearchImeOptions(0)
    , mIncludeInGlobalSearch(0)
    , mQueryAfterZeroResults(0)
    , mAutoUrlDetect(0)
    , mSettingsDescriptionId(0)
    , mSuggestThreshold(0)
    , mVoiceSearchMode(0)
    , mVoiceLanguageModeId(0)
    , mVoicePromptTextId(0)
    , mVoiceLanguageId(0)
    , mVoiceMaxResults(0)
{}

ECode CSearchableInfo::constructor()
{
    return NOERROR;
}

ECode CSearchableInfo::constructor(
    /* [in] */ IContext* activityContext,
    /* [in] */ IAttributeSet* attr,
    /* [in] */ IComponentName* cName)
{
    mSearchActivity = cName;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::Searchable),
        ArraySize(R::styleable::Searchable));
    AutoPtr<ITypedArray> a;
    activityContext->ObtainStyledAttributes(attr, attrIds, (ITypedArray**)&a);
    a->GetResourceId(R::styleable::Searchable_label, 0, &mLabelId);
    a->GetResourceId(R::styleable::Searchable_hint, 0, &mHintId);
    a->GetResourceId(R::styleable::Searchable_icon, 0, &mIconId);
    a->GetResourceId(R::styleable::Searchable_searchButtonText, 0, &mSearchButtonText);
    a->GetInt32(R::styleable::Searchable_inputType,
        IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_NORMAL, &mSearchInputType);
    a->GetInt32(R::styleable::Searchable_imeOptions, IEditorInfo::IME_ACTION_GO, &mSearchImeOptions);
    a->GetBoolean(R::styleable::Searchable_includeInGlobalSearch, FALSE, &mIncludeInGlobalSearch);
    a->GetBoolean(R::styleable::Searchable_queryAfterZeroResults, FALSE, &mQueryAfterZeroResults);
    a->GetBoolean(R::styleable::Searchable_autoUrlDetect, FALSE, &mAutoUrlDetect);
    a->GetResourceId(R::styleable::Searchable_searchSettingsDescription, 0, &mSettingsDescriptionId);
    a->GetString(R::styleable::Searchable_searchSuggestAuthority, &mSuggestAuthority);
    a->GetString(R::styleable::Searchable_searchSuggestPath, &mSuggestPath);
    a->GetString(R::styleable::Searchable_searchSuggestSelection, &mSuggestSelection);
    a->GetString(R::styleable::Searchable_searchSuggestIntentAction, &mSuggestIntentAction);
    a->GetString(R::styleable::Searchable_searchSuggestIntentData, &mSuggestIntentData);
    a->GetInt32(R::styleable::Searchable_searchSuggestThreshold, 0, &mSuggestThreshold);
    a->GetInt32(R::styleable::Searchable_voiceSearchMode, 0, &mVoiceSearchMode);
    a->GetResourceId(R::styleable::Searchable_voiceLanguageModel, 0, &mVoiceLanguageModeId);
    a->GetResourceId(R::styleable::Searchable_voicePromptText, 0, &mVoicePromptTextId);
    a->GetResourceId(R::styleable::Searchable_voiceLanguage, 0, &mVoiceLanguageId);
    a->GetInt32(R::styleable::Searchable_voiceMaxResults, 0, &mVoiceMaxResults);
    a->Recycle();

    // get package info for suggestions provider (if any)
    String suggestProviderPackage(NULL);
    if (mSuggestAuthority != NULL) {
        AutoPtr<IPackageManager> pm;
        activityContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IProviderInfo> pi;
        pm->ResolveContentProvider(mSuggestAuthority, 0, (IProviderInfo**)&pi);
        if (pi != NULL) {
            IPackageItemInfo::Probe(pi)->GetPackageName(&suggestProviderPackage);
        }
    }
    mSuggestProviderPackage = suggestProviderPackage;

    // for now, implement some form of rules - minimal data
    if (mLabelId == 0) {
        // throw new IllegalArgumentException("Search label must be a resource reference.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestAuthority(
    /* [out] */ String* auth)
{
    *auth = mSuggestAuthority;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestPackage(
    /* [out] */ String* pkg)
{
    *pkg = mSuggestProviderPackage;
    return NOERROR;
}

ECode CSearchableInfo::GetSearchActivity(
    /* [out] */ IComponentName** nameRst)
{
    *nameRst = mSearchActivity;
    REFCOUNT_ADD(*nameRst);
    return NOERROR;
}

ECode CSearchableInfo::UseBadgeLabel(
    /* [out] */ Boolean* isUsing)
{
    *isUsing = (0 != (mSearchMode & SEARCH_MODE_BADGE_LABEL));
    return NOERROR;
}

ECode CSearchableInfo::UseBadgeIcon(
    /* [out] */ Boolean* isUsing)
{
    *isUsing = ((0 != (mSearchMode & SEARCH_MODE_BADGE_ICON)) && (mIconId != 0));
    return NOERROR;
}

ECode CSearchableInfo::ShouldRewriteQueryFromData(
    /* [out] */ Boolean* isShould)
{
    *isShould = (0 != (mSearchMode & SEARCH_MODE_QUERY_REWRITE_FROM_DATA));
    return NOERROR;
}

ECode CSearchableInfo::ShouldRewriteQueryFromText(
    /* [out] */ Boolean* isShould)
{
    *isShould = (0 != (mSearchMode & SEARCH_MODE_QUERY_REWRITE_FROM_TEXT));
    return NOERROR;
}

ECode CSearchableInfo::GetSettingsDescriptionId(
    /* [out] */ Boolean* id)
{
    *id = mSettingsDescriptionId;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestPath(
    /* [out] */ String* path)
{
    *path = mSuggestPath;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestSelection(
    /* [out] */ String* selection)
{
    *selection = mSuggestSelection;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestIntentAction(
    /* [out] */ String* action)
{
    *action = mSuggestIntentAction;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestIntentData(
    /* [out] */ String* data)
{
    *data = mSuggestIntentData;
    return NOERROR;
}

ECode CSearchableInfo::GetSuggestThreshold(
    /* [out] */ Int32* threshold)
{
    *threshold = mSuggestThreshold;
    return NOERROR;
}

ECode CSearchableInfo::GetActivityContext(
    /* [in] */ IContext* inCtx,
    /* [out] */ IContext** outCtx)
{
    return CreateActivityContext(inCtx, mSearchActivity, outCtx);
}

ECode CSearchableInfo::GetProviderContext(
    /* [in] */ IContext* ctx,
    /* [in] */ IContext* activityCtx,
    /* [out] */ IContext** rstCtx)
{

    String pkgName;
    mSearchActivity->GetPackageName(&pkgName);
    if (pkgName.Equals(mSuggestProviderPackage))
    {
        *rstCtx = activityCtx;
        REFCOUNT_ADD(*rstCtx);
        return NOERROR;
    }
    if (mSuggestProviderPackage != NULL)
    {
        return ctx->CreatePackageContext(mSuggestProviderPackage, 0, rstCtx);
    }

    *rstCtx = NULL;
    return NOERROR;
}

ECode CSearchableInfo::FindActionKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ IActionKeyInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if (mActionKeys.IsEmpty()) {
        return NOERROR;
    }

    HashMap<Int32 , AutoPtr<IActionKeyInfo> >::Iterator it = mActionKeys.Find(keyCode);
    if (it != mActionKeys.End()) {
        *info = it->mSecond;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    return NOERROR;
}

ECode CSearchableInfo::GetLabelId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mLabelId;
    return NOERROR;
}

ECode CSearchableInfo::GetHintId(
    /* [out] */ Int32* id)
{
    *id = mHintId;
    return NOERROR;
}

ECode CSearchableInfo::GetIconId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIconId;
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceSearchEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = (0 != (mVoiceSearchMode & VOICE_SEARCH_SHOW_BUTTON));
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceSearchLaunchWebSearch(
    /* [out] */ Boolean* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = (0 != (mVoiceSearchMode & VOICE_SEARCH_LAUNCH_WEB_SEARCH));
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceSearchLaunchRecognizer(
    /* [out] */ Boolean* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = (0 != (mVoiceSearchMode & VOICE_SEARCH_LAUNCH_RECOGNIZER));
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceLanguageModeId(
    /* [out] */ Int32* modeId)
{
    VALIDATE_NOT_NULL(modeId);
    *modeId = mVoiceLanguageModeId;
    return NOERROR;
}

ECode CSearchableInfo::GetVoicePromptTextId(
    /* [out] */ Int32* textId)
{
    VALIDATE_NOT_NULL(textId);
    *textId = mVoicePromptTextId;
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceLanguageId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mVoiceLanguageId;
    return NOERROR;
}

ECode CSearchableInfo::GetVoiceMaxResults(
    /* [out] */ Int32* results)
{
    VALIDATE_NOT_NULL(results);
    *results = mVoiceMaxResults;
    return NOERROR;
}

ECode CSearchableInfo::GetSearchButtonText(
    /* [out] */ Int32* text)
{
    VALIDATE_NOT_NULL(text);
    *text = mSearchButtonText;
    return NOERROR;
}

ECode CSearchableInfo::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mSearchInputType;
    return NOERROR;
}

ECode CSearchableInfo::GetImeOptions(
    /* [out] */ Int32* options)
{
    VALIDATE_NOT_NULL(options);
    *options = mSearchImeOptions;
    return NOERROR;
}

ECode CSearchableInfo::ShouldIncludeInGlobalSearch(
    /* [out] */ Boolean* isShould)
{
    VALIDATE_NOT_NULL(isShould);
    *isShould = mIncludeInGlobalSearch;
    return NOERROR;
}

ECode CSearchableInfo::QueryAfterZeroResults(
    /* [out] */ Boolean* results)
{
    VALIDATE_NOT_NULL(results);
    *results = mQueryAfterZeroResults;
    return NOERROR;
}

ECode CSearchableInfo::AutoUrlDetect(
    /* [out] */ Boolean* autoDetect)
{
    VALIDATE_NOT_NULL(autoDetect);
    *autoDetect = mAutoUrlDetect;
    return NOERROR;
}

ECode CSearchableInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mLabelId);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->ReadFromParcel(source, (IComponentName**)&mSearchActivity);
    source->ReadInt32(&mHintId);
    source->ReadInt32(&mSearchMode);
    source->ReadInt32(&mIconId);
    source->ReadInt32(&mSearchButtonText);
    source->ReadInt32(&mSearchInputType);
    source->ReadInt32(&mSearchImeOptions);
    Int32 temp;
    source->ReadInt32(&temp);
    mIncludeInGlobalSearch = temp != 0;
    source->ReadInt32(&temp);
    mQueryAfterZeroResults = temp != 0;
    source->ReadInt32(&temp);
    mAutoUrlDetect = temp != 0;
    source->ReadInt32(&mSettingsDescriptionId);
    source->ReadString(&mSuggestAuthority);
    source->ReadString(&mSuggestPath);
    source->ReadString(&mSuggestSelection);
    source->ReadString(&mSuggestIntentAction);
    source->ReadString(&mSuggestIntentData);
    source->ReadInt32(&mSuggestThreshold);

    Int32 count;
    source->ReadInt32(&count);
    for(; count > 0; count--) {
        AutoPtr<IActionKeyInfo> info;
        CActionKeyInfo::New((IActionKeyInfo**)&info);
        IParcelable* pTemp = IParcelable::Probe(info);
        pTemp->ReadFromParcel(source);
        AddActionKey(info);
    }

    source->ReadString(&mSuggestProviderPackage);
    source->ReadInt32(&mVoiceSearchMode);
    source->ReadInt32(&mVoiceLanguageModeId);
    source->ReadInt32(&mVoicePromptTextId);
    source->ReadInt32(&mVoiceLanguageId);
    source->ReadInt32(&mVoiceMaxResults);
    return NOERROR;
}

ECode CSearchableInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mLabelId);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->WriteToParcel(mSearchActivity, dest);
    dest->WriteInt32(mHintId);
    dest->WriteInt32(mSearchMode);
    dest->WriteInt32(mIconId);
    dest->WriteInt32(mSearchButtonText);
    dest->WriteInt32(mSearchInputType);
    dest->WriteInt32(mSearchImeOptions);
    dest->WriteInt32(mIncludeInGlobalSearch ? 1 : 0);
    dest->WriteInt32(mQueryAfterZeroResults ? 1 : 0);
    dest->WriteInt32(mAutoUrlDetect ? 1 : 0);
    dest->WriteInt32(mSettingsDescriptionId);
    dest->WriteString(mSuggestAuthority);
    dest->WriteString(mSuggestPath);
    dest->WriteString(mSuggestSelection);
    dest->WriteString(mSuggestIntentAction);
    dest->WriteString(mSuggestIntentData);
    dest->WriteInt32(mSuggestThreshold);

    if (mActionKeys.IsEmpty()) {
        dest->WriteInt32(0);
    }
    else {
        Int32 count = mActionKeys.GetSize();
        dest->WriteInt32(count);
        HashMap<Int32 , AutoPtr<IActionKeyInfo> >::Iterator it = mActionKeys.Begin();
        for (; it != mActionKeys.End(); it++) {
            IParcelable* pTemp = IParcelable::Probe(it->mSecond);
            pTemp->WriteToParcel(dest);
        }
    }

    dest->WriteString(mSuggestProviderPackage);
    dest->WriteInt32(mVoiceSearchMode);
    dest->WriteInt32(mVoiceLanguageModeId);
    dest->WriteInt32(mVoicePromptTextId);
    dest->WriteInt32(mVoiceLanguageId);
    dest->WriteInt32(mVoiceMaxResults);
    return NOERROR;
}

ECode CSearchableInfo::GetActivityMetaData(
    /* [in] */ IContext* ctx,
    /* [in] */ IActivityInfo* activityInfo,
    /* [in] */ Int32 userId,
    /* [out] */ ISearchableInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoPtr<IContext> userContext;
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(userId, (IUserHandle**)&userHandle);
    FAIL_RETURN(ctx->CreatePackageContextAsUser(
            String("system"), 0, userHandle, (IContext**)&userContext));
    // for each component, try to find metadata
    AutoPtr<IXmlResourceParser> xml;
    AutoPtr<IPackageManager> pm;
    userContext->GetPackageManager((IPackageManager**)&pm);
    IPackageItemInfo* pi = IPackageItemInfo::Probe(activityInfo);
    pi->LoadXmlMetaData(pm, MD_LABEL_SEARCHABLE, (IXmlResourceParser**)&xml);
    if (xml == NULL) {
        return NOERROR;
    }

    AutoPtr<IComponentName> cName;
    String pckName, name;
    pi->GetPackageName(&pckName);
    pi->GetName(&name);
    CComponentName::New(pckName, name, (IComponentName**)&cName);
    GetActivityMetaData(userContext, IXmlPullParser::Probe(xml), cName, info);
    xml->Close();

    // if (DBG) {
    //     if (searchable != null) {
    //         Log.d(TAG, "Checked " + activityInfo.name
    //                 + ",label=" + searchable.getLabelId()
    //                 + ",icon=" + searchable.getIconId()
    //                 + ",suggestAuthority=" + searchable.getSuggestAuthority()
    //                 + ",target=" + searchable.getSearchActivity().getClassName()
    //                 + ",global=" + searchable.shouldIncludeInGlobalSearch()
    //                 + ",settingsDescription=" + searchable.getSettingsDescriptionId()
    //                 + ",threshold=" + searchable.getSuggestThreshold());
    //     } else {
    //         Log.d(TAG, "Checked " + activityInfo.name + ", no searchable meta-data");
    //     }
    // }
    return NOERROR;
}

ECode CSearchableInfo::CreateActivityContext(
    /* [in] */ IContext* ctx,
    /* [in] */ IComponentName* activity,
    /* [out] */ IContext** outCtx)
{
    outCtx = NULL;
    String pckName;
    activity->GetPackageName(&pckName);
    return ctx->CreatePackageContext(pckName, 0, outCtx);
}

ECode CSearchableInfo::GetActivityMetaData(
    /* [in] */ IContext* ctx,
    /* [in] */ IXmlPullParser* xml,
    /* [in] */ IComponentName* cName,
    /* [out] */ ISearchableInfo** info)
{
    *info = NULL;
    AutoPtr<IContext> activityCtx;
    AutoPtr<ISearchableInfo> result;
    CreateActivityContext(ctx, cName, (IContext**)&activityCtx);
    if (activityCtx == NULL)
        return NOERROR;

    // in order to use the attributes mechanism, we have to walk the parser
    // forward through the file until it's reading the tag of interest.
    Int32 tagType;
    FAIL_RETURN(xml->Next(&tagType));
    String xmlName;

    while (tagType != IXmlPullParser::END_DOCUMENT) {
        if (tagType == IXmlPullParser::START_TAG) {
            xml->GetName(&xmlName);
            if (xmlName.Equals(MD_XML_ELEMENT_SEARCHABLE)) {
                AutoPtr<IAttributeSet> attr = Xml::AsAttributeSet(xml);
                if (attr != NULL) {
                    result = NULL;
                    ASSERT_SUCCEEDED(CSearchableInfo::New(activityCtx, attr, cName, (ISearchableInfo**)&result));
                }
            }
            else if (xmlName.Equals(MD_XML_ELEMENT_SEARCHABLE_ACTION_KEY)) {
                if (result == NULL) {
                    // Can't process an embedded element if we haven't seen the enclosing
                    return NOERROR;
                }
                AutoPtr<IAttributeSet> attr = Xml::AsAttributeSet(xml);
                if (attr != NULL) {
                    // try {
                    AutoPtr<IActionKeyInfo> keyInfo;
                    ASSERT_SUCCEEDED(CActionKeyInfo::New(activityCtx, attr, (IActionKeyInfo**)&keyInfo));
                    ((CSearchableInfo*)(result.Get()))->AddActionKey(keyInfo);
                    // } catch (IllegalArgumentException ex) {
                    //     Log.w(TAG, "Invalid action key for " +
                    //             cName.flattenToShortString() + ": " + ex.getMessage());
                    //     return null;
                    // }
                }
            }
        }
        xml->Next(&tagType);
    }

    *info = result;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

void CSearchableInfo::AddActionKey(
    /* [in] */ IActionKeyInfo* keyInfo)
{
    Int32 keyCode;
    keyInfo->GetKeyCode(&keyCode);
    mActionKeys[keyCode] = keyInfo;
}

}// namespace App
}// namespace Droid
}// namespace Elastos
