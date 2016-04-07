#include "elastos/droid/widget/SuggestionsAdapter.h"
#include "elastos/droid/widget/SearchView.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/style/CTextAppearanceSpan.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/R.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Core::StringUtils;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::R;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Style::ITextAppearanceSpan;
using Elastos::Droid::Text::Style::CTextAppearanceSpan;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentResolverOpenResourceIdResult;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;

namespace Elastos {
namespace Droid {
namespace Widget {

const Boolean _SuggestionsAdapter::DBG = FALSE;
String const  _SuggestionsAdapter::TAG("SuggestionsAdapter");
const Int32 _SuggestionsAdapter::QUERY_LIMIT = 50;
const Int32 _SuggestionsAdapter::INVALID_INDEX = -1;
const Int64 _SuggestionsAdapter::DELETE_KEY_POST_DELAY = 500L;

CAR_INTERFACE_IMPL(_SuggestionsAdapter::FilterDelayer, IFilterDelayer)
CAR_INTERFACE_IMPL(_SuggestionsAdapter::ChildViewCache, IInterface)

IADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)
IBASEADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)
ILISTADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)
ISPINNERADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)
ICURSORADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)
IRESOURCECURSORADAPTER_METHODS_IMPL(SuggestionsAdapter, _SuggestionsAdapter)

_SuggestionsAdapter::FilterDelayer::FilterDelayer()
    : mPreviousLength(0)
{}

ECode _SuggestionsAdapter::FilterDelayer::GetPostingDelay(
    /* [in] */ ICharSequence* constraint,
    /* [out] */ Int64* delay)
{
    if (constraint == NULL) return 0;

    Int32 length;
    constraint->GetLength(&length);
    *delay = length < mPreviousLength ? DELETE_KEY_POST_DELAY : 0;
    mPreviousLength = length;
    return NOERROR;
}

_SuggestionsAdapter::ChildViewCache::ChildViewCache(
    /* [in] */ IView* v)
{
    AutoPtr<IView> view;
    v->FindViewById(R::id::text1, (IView**)&view);
    mText1 = ITextView::Probe(view);
    view = NULL;
    v->FindViewById(R::id::text2, (IView**)&view);
    mText2 = ITextView::Probe(view);
    view = NULL;
    v->FindViewById(R::id::icon1, (IView**)&view);
    mIcon1 = IImageView::Probe(obj);
    view = NULL;
    v->FindViewById(R::id::icon2, (IView**)&view);
    mIcon2 = IImageView::Probe(obj);
    view = NULL;
    v->FindViewById(R::id::edit_query, (IView**)&view);
    mIconRefine = IImageView::Probe(obj);
}

_SuggestionsAdapter::_SuggestionsAdapter(
    /* [in] */ IContext* ctx,
    /* [in] */ ISearchView* searchView,
    /* [in] */ ISearchableInfo* searchable,
    /* [in] */ Int32 reservedWords)
    : ResourceCursorAdapter(ctx, R::layout::search_dropdown_item_icons_2line, NULL, TRUE)
{
    AutoPtr<IInterface> smTemp;
    ctx->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&smTemp);
    mSearchManager = ISearchManager::Probe(smTemp);
    mSearchView = searchView;
    mSearchable = searchable;
    // set up provider resources (gives us icons, etc.)
    AutoPtr<IContext> activityContext;
    mSearchable->GetActivityContext(ctx, (IContext**)&activityContext);
    mSearchable->GetProviderContext(ctx, activityContext, (IContext**)&mProviderContext);

    // mOutsideDrawablesCache = outsideDrawablesCache;
    // mStartSpinnerRunnable = new Runnable() {
    // public void run() {
    // // mSearchView.setWorking(true); // TODO:
    // }
    // };
    //
    // mStopSpinnerRunnable = new Runnable() {
    // public void run() {
    // // mSearchView.setWorking(false); // TODO:
    // }
    // };

    // delay 500ms when deleting

    AutoPtr<IFilterDelayer> delayer = new FilterDelayer();
    GetFilter()->SetDelayer(delayer);
}

ECode _SuggestionsAdapter::SetQueryRefinement(
    /* [in] */ Int32 refineWhat)
{
    mQueryRefinement = refineWhat;
    return NOERROR;
}

ECode _SuggestionsAdapter::GetQueryRefinement(
    /* [in] */ Int32* value)
{
    *value = mQueryRefinement;
    return NOERROR;
}

Boolean _SuggestionsAdapter::HasStableIds()
{
    return FALSE;
}

AutoPtr<ICursor> _SuggestionsAdapter::RunQueryOnBackgroundThread(
    /* [in] */ ICharSequence* constraint)
{
    // if (DBG) Log.d(LOG_TAG, "runQueryOnBackgroundThread(" + constraint + ")");
    String query("");
    if(constraint == NULL)
    {
        constraint->ToString(&query);
    }
    /**
     * for in app search we show the progress spinner until the cursor is returned with
     * the results.
     */
     AutoPtr<ICursor> cursor;
     Int32 visible, windowVisible;
     mSearchView->GetVisibility(&visible);
     mSearchView->GetWindowVisibility(&windowVisible);
     if(visible != IView::VISIBLE || windowVisible != IView::VISIBLE)
     {
        return NULL;
     }
     //mSearchView.getWindow().getDecorView().post(mStartSpinnerRunnable); // TODO:
    // try {
    mSearchManager->GetSuggestions(mSearchable, query, QUERY_LIMIT, (ICursor**)&cursor);
    // trigger fill window so the spinner stays up until the results are copied over and
    // closer to being ready
    if (cursor != NULL) {
        // cursor.getCount();
        return cursor;
    }
    // } catch (RuntimeException e) {
    //     Log.w(LOG_TAG, "Search suggestions query threw an exception.", e);
    // }
    return NULL;
}

ECode _SuggestionsAdapter::Close()
{
    // if (DBG) Log.d(LOG_TAG, "close()");
    ChangeCursor(NULL);
    mClosed = TRUE;
    return NOERROR;
}

ECode _SuggestionsAdapter::NotifyDataSetChanged()
{
    // if (DBG) Log.d(LOG_TAG, "notifyDataSetChanged");
    FAIL_RETURN(ResourceCursorAdapter::NotifyDataSetChanged());

    // mSearchView.onDataSetChanged(); // TODO:

    UpdateSpinnerState(GetCursor());
    return NOERROR;
}

ECode _SuggestionsAdapter::NotifyDataSetInvalidated()
{
    // if (DBG) Log.d(LOG_TAG, "notifyDataSetInvalidated");
    FAIL_RETURN(ResourceCursorAdapter::NotifyDataSetInvalidated());

    UpdateSpinnerState(GetCursor());
    return NOERROR;
}

ECode _SuggestionsAdapter::ChangeCursor(
    /* [in] */ ICursor* cursor)
{
    // if (DBG) Log.d(LOG_TAG, "changeCursor(" + c + ")");

    if (mClosed) {
        // Log.w(LOG_TAG, "Tried to change cursor after adapter was closed.");
        if (cursor != NULL)
            return cursor->Close();
    }

    // try {
    FAIL_RETURN(ResourceCursorAdapter::ChangeCursor(cursor));

    if (cursor != NULL) {
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_TEXT_1, &mText1Col);
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_TEXT_2, &mText2Col);
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_TEXT_2_URL, &mText2UrlCol);
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_ICON_1, &mIconName1Col);
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_ICON_2, &mIconName2Col);
         cursor->GetColumnIndex(ISearchManager::SUGGEST_COLUMN_FLAGS, &mFlagsCol);
    }
    // } catch (Exception e) {
    //     Log.e(LOG_TAG, "error changing cursor and caching columns", e);
    // }
    return NOERROR;
}


AutoPtr<IView> _SuggestionsAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> v = ResourceCursorAdapter::NewView(context, cursor, parent);
    AutoPtr<ChildViewCache> cache = new ChildViewCache(v);
    v->SetTag(cache);
    return v;
}

ECode _SuggestionsAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IInterface> obj;
    view->GetTag((IInterface**)&obj);
    AutoPtr<ChildViewCache> views = (ChildViewCache*) IObject::Probe(obj);

    Int32 flags = 0;
    if (mFlagsCol != INVALID_INDEX) {
        flags = cursor->GetInt32(mFlagsCol, &flags);
    }
    if (views->mText1 != NULL) {
        String sTemp = GetStringOrNull(cursor, mText1Col);
        AutoPtr<ICharSequence> text1 ;
        CStringWrapper::New(sTemp, (ICharSequence**)&text1);
        SetViewText(views->mText1, text1);
    }
    if (views->mText2 != NULL) {
        // First check TEXT_2_URL
        String sTemp;
        sTemp = GetStringOrNull(cursor, mText2UrlCol);
        AutoPtr<ICharSequence> text2 ;
        CStringWrapper::New(sTemp, (ICharSequence**)&text2);
        if (text2 != NULL) {
            text2 = FormatUrl(text2);
        } else {
            sTemp = GetStringOrNull(cursor, mText2Col);
            CStringWrapper::New(sTemp, (ICharSequence**)&text2);
        }

        // If no second line of text is indicated, allow the first line of text
        // to be up to two lines if it wants to be.
        if (TextUtils::IsEmpty(text2)) {
            if (views->mText1 != NULL) {
                views->mText1->SetSingleLine(FALSE);
                views->mText1->SetMaxLines(2);
            }
        } else {
            if (views->mText1 != NULL) {
                views->mText1->SetSingleLine(TRUE);
                views->mText1->SetMaxLines(1);
            }
        }
        SetViewText(views->mText2, text2);
    }

    if (views->mIcon1 != NULL) {
        SetViewDrawable(views->mIcon1, GetIcon1(cursor), IView::INVISIBLE);
    }
    if (views->mIcon2 != NULL) {
        SetViewDrawable(views->mIcon2, GetIcon2(cursor), IView::GONE);
    }
    if (mQueryRefinement == ISuggestionsAdapter::REFINE_ALL
            || (mQueryRefinement == ISuggestionsAdapter::REFINE_BY_ENTRY
                    && (flags & ISearchManager::FLAG_QUERY_REFINEMENT) != 0)) {
        views->mIconRefine->SetVisibility(IView::VISIBLE);
        AutoPtr<ICharSequence> cs;
        views->mText1->GetText((ICharSequence**)&cs);
        views->mIconRefine->SetTag(cs);
        views->mIconRefine->SetOnClickListener(this);
    }
    else {
        views->mIconRefine->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode _SuggestionsAdapter::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    if(ICharSequence::Probe(tag))
    {
        SearchView* sv = (SearchView*)mSearchView.Get();
        sv->OnQueryRefine(ICharSequence::Probe(tag));
    }
    return NOERROR;
}

AutoPtr<ICharSequence> _SuggestionsAdapter::ConvertToString(
    /* [in] */ ICursor* cursor)
{
    if (cursor == NULL) {
        return NULL;
    }

    String query = GetColumnString(cursor, ISearchManager::SUGGEST_COLUMN_QUERY);
    if (query != NULL) {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(query, (ICharSequence**)&cs);
        return cs;
    }

    Boolean b;
    if (mSearchable->ShouldRewriteQueryFromData(&b), b) {
        String data = GetColumnString(cursor, ISearchManager::SUGGEST_COLUMN_INTENT_DATA);
        if (data != NULL) {
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(data, (ICharSequence**)&cs);
            return cs;
        }
    }

    if (mSearchable->ShouldRewriteQueryFromText(&b), b) {
        String text1 = GetColumnString(cursor, ISearchManager::SUGGEST_COLUMN_TEXT_1);
        if (text1 != NULL) {
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(text1, (ICharSequence**)&cs);
            return cs;
        }
    }

    return NULL;
}

AutoPtr<IView> _SuggestionsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> v = ResourceCursorAdapter::GetView(position, convertView, parent);
    if (v == NULL) {
        AutoPtr<IInterface> obj;
        mWeakContext->Resolve(EIID_IContext, (IInterface**)&obj);
        IContext* context = IContext::Probe(obj);
        if (context == NULL) {
            return NULL;
        }

        // Log.w(LOG_TAG, "Search suggestions cursor threw exception.", e);
        // Put exception string in item title
        v = NewView(context, mCursor, parent);
        if (v != NULL) {
            AutoPtr<IInterface> obj;
            v->GetTag((IInterface**)&obj);
            AutoPtr<ChildViewCache> views = (ChildViewCache*)obj.Get();

            AutoPtr<ITextView> tv = views->mText1;
            String e("RuntimeException");
            AutoPtr<ICharSequence> cs;
            CStringWrapper::New(e, (ICharSequence**)&cs);
            tv->SetText(cs);
        }
    }
    return v;
}


String _SuggestionsAdapter::GetColumnString(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& columnName)
{
    Int32 col;
    cursor->GetColumnIndex(columnName, &col);
    return GetStringOrNull(cursor, col);
}

void _SuggestionsAdapter::UpdateSpinnerState(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IBundle> extras;
    if(cursor)
        cursor->GetExtras((IBundle**)&extras);
    // if (DBG) {
    //     Log.d(LOG_TAG, "updateSpinnerState - extra = "
    //         + (extras != null
    //                 ? extras.getBoolean(ISearchManager::CURSOR_EXTRA_KEY_IN_PROGRESS)
    //                 : null));
    // }
    // Check if the Cursor indicates that the query is not complete and show the spinner
    Boolean b;
    if (extras != NULL)
        if(extras->GetBoolean(ISearchManager::CURSOR_EXTRA_KEY_IN_PROGRESS, &b), b) {
        // mSearchView.getWindow().getDecorView().post(mStartSpinnerRunnable); // TODO:
        return;
    }
    // If cursor is null or is done, stop the spinner
    // mSearchView.getWindow().getDecorView().post(mStopSpinnerRunnable); // TODO:
}

AutoPtr<ICharSequence> _SuggestionsAdapter::FormatUrl(
    /* [in] */ ICharSequence* url)
{
    if (mUrlColor == NULL) {
        AutoPtr<IInterface> obj;
        mWeakContext->Resolve(EIID_IContext, (IInterface**)&obj);
        AutoPtr<IContext> context = IContext::Probe(obj);

        // Lazily get the URL color from the current theme.
        AutoPtr<ITypedValue> colorValue;
        CTypedValue::New((ITypedValue**)&colorValue);
        AutoPtr<IResourcesTheme> theme;
        context->GetTheme((IResourcesTheme**)&theme);
        Boolean rst;
        theme->ResolveAttribute(R::attr::textColorSearchUrl, colorValue, TRUE, &rst);
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        Int32 resourceId;
        colorValue->GetResourceId(&resourceId);
        r->GetColorStateList(resourceId, (IColorStateList**)&mUrlColor);
    }

    AutoPtr<ISpannableString> text;
    CSpannableString::New(url, (ISpannableString**)&text);
    AutoPtr<ITextAppearanceSpan> span;
    CTextAppearanceSpan::New(String(NULL), 0, 0, mUrlColor, NULL, (ITextAppearanceSpan**)&span);
    Int32 length;
    url->GetLength(&length);
    text->SetSpan(span, 0, length, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    return text;
}

void _SuggestionsAdapter::SetViewText(
    /* [in] */ ITextView* v,
    /* [in] */ ICharSequence* text)
{
    // Set the text even if it's null, since we need to clear any previous text.
    v->SetText(text);

    if (TextUtils::IsEmpty(text)) {
        v->SetVisibility(IView::GONE);
    } else {
        v->SetVisibility(IView::VISIBLE);
    }
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetIcon1(
    /* [in] */ ICursor* cursor)
{
    if (mIconName1Col == INVALID_INDEX) {
        return NULL;
    }
    String value;
    cursor->GetString(mIconName1Col, &value);
    AutoPtr<IDrawable> drawable = GetDrawableFromResourceValue(value);
    if (drawable != NULL) {
        return drawable;
    }
    return GetDefaultIcon1(cursor);
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetIcon2(
    /* [in] */ ICursor* cursor)
{
    if (mIconName2Col == INVALID_INDEX) {
        return NULL;
    }
    String value;
    cursor->GetString(mIconName2Col, &value);
    return GetDrawableFromResourceValue(value);

}


void _SuggestionsAdapter::SetViewDrawable(
    /* [in] */ IImageView* v,
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 nullVisibility)
{
    // Set the icon even if the drawable is null, since we need to clear any
    // previous icon.
    v->SetImageDrawable(drawable);

    if (drawable == NULL) {
        v->SetVisibility(nullVisibility);
    } else {
        v->SetVisibility(IView::VISIBLE);

        // This is a hack to get any animated drawables (like a 'working' spinner)
        // to animate. You have to setVisible true on an AnimationDrawable to get
        // it to start animating, but it must first have been false or else the
        // call to setVisible will be ineffective. We need to clear up the story
        // about animated drawables in the future, see http://b/1878430.
        Boolean rst;
        drawable->SetVisible(FALSE, FALSE, &rst);
        drawable->SetVisible(TRUE, FALSE, &rst);
    }
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetDrawableFromResourceValue(
    /* [in] */ const String& drawableId)
{
    if (drawableId.IsNullOrEmpty() || drawableId.Equals("0")) {
        return NULL;
    }
    // try {
    // First, see if it's just an integer
    Int32 resourceId = StringUtils::ParseInt32(drawableId);
    if(resourceId == 0)
    {
        // It's not an integer, use it as a URI
        AutoPtr<IDrawable> drawable = CheckIconCache(drawableId);
        if (drawable != NULL) {
            return drawable;
        }
        AutoPtr<IUri> uri;
        Uri::Parse(drawableId, (IUri**)&uri);
        drawable = GetDrawable(uri);
        StoreInIconCache(drawableId, drawable);
        return drawable;
    }
    // It's an int, look for it in the cache
    String drawableUri(""), pkgName;
    mProviderContext->GetPackageName(&pkgName);
    drawableUri += IContentResolver::SCHEME_ANDROID_RESOURCE;
    drawableUri += "://";
    drawableUri += pkgName;
    drawableUri += "/";
    drawableUri += drawableId;

    // Must use URI as cache key, since ints are app-specific
    AutoPtr<IDrawable> drawable = CheckIconCache(drawableUri);
    if (drawable != NULL) {
        return drawable;
    }
    // Not cached, find it by resource ID
    AutoPtr<IResources> res;
    mProviderContext->GetResources((IResources**)&res);
    res->GetDrawable(resourceId, (IDrawable**)&drawable);
    // Stick it in the cache, using the URI as key
    StoreInIconCache(drawableUri, drawable);
    return drawable;
    // } catch (NumberFormatException nfe) {
    //     // It's not an integer, use it as a URI
    //     Drawable drawable = checkIconCache(drawableId);
    //     if (drawable != NULL) {
    //         return drawable;
    //     }
    //     Uri uri = Uri.parse(drawableId);
    //     drawable = getDrawable(uri);
    //     storeInIconCache(drawableId, drawable);
    //     return drawable;
    // } catch (Resources.NotFoundException nfe) {
    //     // It was an integer, but it couldn't be found, bail out
    //     Log.w(LOG_TAG, "Icon resource not found: " + drawableId);
    //     return NULL;
    // }
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetDrawable(
    /* [in] */ IUri* uri)
{
    // try {

    String scheme;
    uri->GetScheme(&scheme);
    if (IContentResolver::SCHEME_ANDROID_RESOURCE.Equals(scheme)) {
        // Load drawables through Resources, to get the source density information
        AutoPtr<IContentResolver>cr;
        mProviderContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IContentResolverOpenResourceIdResult> r;
        cr->GetResourceId(uri, (IContentResolverOpenResourceIdResult**)&r);
        // try {
        Int32 rId;
        AutoPtr<IResources> rsc;
        r->GetResources((IResources**)&rsc);
        r->GetResourceId(&rId);
        AutoPtr<IDrawable> drawable;
        rsc->GetDrawable(rId, (IDrawable**)&drawable);
        return drawable;
        // } catch (Resources.NotFoundException ex) {
        //     throw new FileNotFoundException("Resource does not exist: " + uri);
        // }
    } else {
        // Let the ContentResolver handle content and file URIs.
        AutoPtr<IInputStream> stream;
        AutoPtr<IContentResolver>cr;
        mProviderContext->GetContentResolver((IContentResolver**)&cr);
        if(FAILED(cr->OpenInputStream(uri, (IInputStream**)&stream)))
        {
            return NULL;
        }
        if (stream == NULL) {
            // throw new FileNotFoundException("Failed to open " + uri);
            return NULL;
        }
        // try {
        AutoPtr<IDrawable> drawable;
        Drawable::Drawable::CreateFromStream(stream, String(NULL), (IDrawable**)&drawable);
        stream->Close();
        return drawable;
        // } finally {
        //     try {
        //         stream.close();
        //     } catch (IOException ex) {
        //         Log.e(LOG_TAG, "Error closing icon stream for " + uri, ex);
        //     }
        // }
    }
    // } catch (FileNotFoundException fnfe) {
    //     Log.w(LOG_TAG, "Icon not found: " + uri + ", " + fnfe.getMessage());
    //     return null;
    // }
}

AutoPtr<IDrawable> _SuggestionsAdapter::CheckIconCache(
    /* [in] */ const String& resourceUri)
{
    AutoPtr<IDrawableConstantState> cached;
    // mOutsideDrawablesCache.get(resourceUri);
    if (cached == NULL) {
        return NULL;
    }
    // if (DBG) Log.d(LOG_TAG, "Found icon in cache: " + resourceUri);
    AutoPtr<IDrawable> d;
    cached->NewDrawable((IDrawable**)&d);
    return d;
}

void _SuggestionsAdapter::StoreInIconCache(
    /* [in] */ const String& resourceUri,
    /* [in] */ IDrawable* drawable)
{
    if (drawable != NULL) {
        // mOutsideDrawablesCache.put(resourceUri, drawable.getConstantState());
    }
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetDefaultIcon1(
    /* [in] */ ICursor* cursor)
{
    // Check the component that gave us the suggestion
    AutoPtr<IComponentName> cName;
    mSearchable->GetSearchActivity((IComponentName**)&cName);
    AutoPtr<IDrawable> drawable = GetActivityIconWithCache(cName);
    if (drawable != NULL) {
        return drawable;
    }

    AutoPtr<IInterface> obj;
    mWeakContext->Resolve(EIID_IContext, (IInterface**)&obj);
    AutoPtr<IContext> context = IContext::Probe(obj);

    // Fall back to a default icon
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->GetDefaultActivityIcon((IDrawable**)&drawable);
    return drawable;
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetActivityIconWithCache(
    /* [in] */ IComponentName* component)
{
    // First check the icon cache
    String componentIconKey;
    component->FlattenToShortString(&componentIconKey);
    // Using containsKey() since we also store null values.
    // if (mOutsideDrawablesCache.containsKey(componentIconKey)) {
    //     Drawable.ConstantState cached = mOutsideDrawablesCache.get(componentIconKey);
    //     return cached == null ? null : cached.newDrawable(mProviderContext.getResources());
    // }
    // Then try the activity or application icon
    AutoPtr<IDrawable> drawable = GetActivityIcon(component);
    // Stick it in the cache so we don't do this lookup again.
    AutoPtr<IDrawableConstantState> toCache;
    if (drawable) {
        drawable->GetConstantState((IDrawableConstantState**)&toCache);
    }
    // mOutsideDrawablesCache.put(componentIconKey, toCache);
    return drawable;
}

AutoPtr<IDrawable> _SuggestionsAdapter::GetActivityIcon(
    /* [in] */ IComponentName* component)
{
    AutoPtr<IInterface> obj;
    mWeakContext->Resolve(EIID_IContext, (IInterface**)&obj);
    AutoPtr<IContext> context = IContext::Probe(obj);

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IActivityInfo> activityInfo;
    // try {
    ECode ec = pm->GetActivityInfo(component, IPackageManager::GET_META_DATA, (IActivityInfo**)&activityInfo);
    if (FAILED(ec)) {
        return NULL;
    }
    // } catch (NameNotFoundException ex) {
    //     Log.w(LOG_TAG, ex.toString());
    //     return null;
    // }
    Int32 iconId;
    activityInfo->GetIconResource(&iconId);
    if (iconId == 0) return NULL;
    String pkg;
    component->GetPackageName(&pkg);
    AutoPtr<IDrawable> drawable;
    AutoPtr<IApplicationInfo> appInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    pm->GetDrawable(pkg, iconId, appInfo, (IDrawable**)&drawable);
    // if (drawable == NULL) {
    //     // Log.w(LOG_TAG, "Invalid icon resource " + iconId + " for "
    //     //         + component.flattenToShortString());
    //     return NULL;
    // }
    return drawable;
}

String _SuggestionsAdapter::GetStringOrNull(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 col)
{
    if (col == INVALID_INDEX) {
        return String(NULL);
    }
    // try {
        String s(NULL);
        cursor->GetString(col, &s);
        return s;
    // } catch (Exception e) {
    //     Log.e(LOG_TAG,
    //             "unexpected error retrieving valid column from cursor, "
    //                     + "did the remote process die?", e);
    //     return null;
    // }
}

SuggestionsAdapter::SuggestionsAdapter(
        /* [in] */ IContext* ctx,
        /* [in] */ ISearchView* searchView,
        /* [in] */ ISearchableInfo* searchable,
        /* [in] */ Int32 reservedWords)
    : _SuggestionsAdapter(ctx, searchView, searchable, reservedWords)
{}


PInterface SuggestionsAdapter::Probe(
        /* [in] */ REIID riid)
{
    if (riid == EIID_ISpinnerAdapter) {
        return (ISpinnerAdapter*)this;
    }
    else if (riid == EIID_ICursorFilterClient) {
        return (ICursorFilterClient*)this;
    }
    else if (riid == EIID_IFilterable) {
        return (IFilterable*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_ISuggestionsAdapter) {
        return (ISuggestionsAdapter*)this;
    }
    else if (riid == EIID_IResourceCursorAdapter) {
        return (IResourceCursorAdapter*)this;
    }
    else if (riid == EIID_ICursorAdapter) {
        return (ICursorAdapter*)this;
    }
    else if (riid == EIID_IBaseAdapter) {
        return (IBaseAdapter*)this;
    }
    else if (riid == EIID_IListAdapter) {
        return (IListAdapter*)this;
    }
    else if (riid == EIID_IAdapter) {
        return (IAdapter*)this;
    }
    else if (riid == EIID_IInterface) {
        return (ISuggestionsAdapter*)this;
    }
    return NULL;
}

UInt32 SuggestionsAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 SuggestionsAdapter::Release()
{
    return ElRefBase::Release();
}

ECode SuggestionsAdapter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(ISuggestionsAdapter *)this) {
        *iid = EIID_ISuggestionsAdapter ;
    }
    else if (object == (IInterface*)(IViewOnClickListener *)this) {
        *iid = EIID_IViewOnClickListener ;
    }
    else if (object == (IInterface*)(IFilterable *)this) {
        *iid = EIID_IFilterable ;
    }
        else if (object == (IInterface*)(ICursorFilterClient *)this) {
        *iid = EIID_ICursorFilterClient ;
    }
    else if (object == (IInterface*)(ISpinnerAdapter *)this) {
        *iid = EIID_ISpinnerAdapter ;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

    /**
     * Enables query refinement for all suggestions. This means that an additional icon
     * will be shown for each entry. When clicked, the suggested text on that line will be
     * copied to the query text field.
     * <p>
     *
     * @param refine which queries to refine. Possible values are {@link #REFINE_NONE},
     * {@link #REFINE_BY_ENTRY}, and {@link #REFINE_ALL}.
     */
ECode SuggestionsAdapter::SetQueryRefinement(
        /* [in] */ Int32 refineWhat)
{
    return _SuggestionsAdapter::SetQueryRefinement(refineWhat);
}

    /**
     * Returns the current query refinement preference.
     * @return value of query refinement preference
     */
ECode SuggestionsAdapter::GetQueryRefinement(
        /* [out] */ Int32* value)
{
    return _SuggestionsAdapter::GetQueryRefinement(value);
}

ECode SuggestionsAdapter::Close()
{
    return _SuggestionsAdapter::Close();
}

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
ECode SuggestionsAdapter::OnClick(
        /* [in] */ IView* v)
{
    return _SuggestionsAdapter::OnClick(v);
}

    /**
     * <p>Returns a filter that can be used to constrain data with a filtering
     * pattern.</p>
     *
     * <p>This method is usually implemented by {@link android.widget.Adapter}
     * classes.</p>
     *
     * @return a filter used to constrain data
     */
ECode SuggestionsAdapter::GetFilter(
        /* [out] */ IFilter** filter)
{
    AutoPtr<IFilter> f = _SuggestionsAdapter::GetFilter();
    (*filter) = f;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode SuggestionsAdapter::ConvertToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** charSequence)
{
    AutoPtr<ICharSequence> cs = _SuggestionsAdapter::ConvertToString(cursor);
    (*charSequence) = cs;
    REFCOUNT_ADD(*charSequence);
    return NOERROR;
}

ECode SuggestionsAdapter::RunQueryOnBackgroundThread(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ ICursor** cursor)
{
    AutoPtr<ICursor> c = _SuggestionsAdapter::RunQueryOnBackgroundThread(constraint);
    (*cursor) = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode SuggestionsAdapter::GetCursor(
        /* [out] */ ICursor** cursor)
{
    AutoPtr<ICursor> c = _SuggestionsAdapter::GetCursor();
    (*cursor) = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode SuggestionsAdapter::ChangeCursor(
        /* [in] */ ICursor* cursor)
{
    return _SuggestionsAdapter::ChangeCursor(cursor);
}

}// Widget
}// Droid
}// Elastos


