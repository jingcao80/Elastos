
#ifndef __ELASTOS_DROID_WIDGET_SUGGESTIONSADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SUGGESTIONSADAPTER_H__

#include "elastos/droid/widget/ResourceCursorAdapter.h"


using Elastos::Droid::Net::IUri;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::ISearchableInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
/**
 * Provides the contents for the suggestion drop-down list.in {@link SearchDialog}.
 *
 * @hide
 */
namespace Elastos {
namespace Droid {
namespace Widget {

class _SuggestionsAdapter
    : public ResourceCursorAdapter
    , public ElRefBase
{
private:

    class FilterDelayer
        : Object
        , public IFilterDelayer
    {
    public:
        CAR_INTERFACE_DECL()

        FilterDelayer();
        /**
         * @param constraint The constraint passed to {@link Filter#filter(CharSequence)}
         * @return The delay that should be used for
         *         {@link Handler#sendMessageDelayed(android.os.Message, Int64)}
         */
        CARAPI GetPostingDelay(
            /* [in] */ ICharSequence* constraint,
            /* [out] */ Int64* delay);
    private:
        Int32 mPreviousLength;
    };

    class ChildViewCache
        : public Object
    {
    public:
        CAR_INTERFACE_DECL()

        ChildViewCache(
            /* [in] */ IView* v);
    public:
        AutoPtr<ITextView> mText1;
        AutoPtr<ITextView> mText2;
        AutoPtr<IImageView> mIcon1;
        AutoPtr<IImageView> mIcon2;
        AutoPtr<IImageView> mIconRefine;
    };
public:

    _SuggestionsAdapter(
        /* [in] */ IContext* ctx,
        /* [in] */ ISearchView* searchView,
        /* [in] */ ISearchableInfo* searchable,
        /* [in] */ Int32 reservedWords);

    CARAPI SetQueryRefinement(
        /* [in] */ Int32 refineWhat);

    CARAPI GetQueryRefinement(
        /* [out] */ Int32* value);

    virtual CARAPI_(Boolean) HasStableIds();

    virtual CARAPI_(AutoPtr<ICursor>) RunQueryOnBackgroundThread(
        /* [in] */ ICharSequence* constraint);

    CARAPI Close();

    virtual CARAPI NotifyDataSetChanged();

    virtual CARAPI NotifyDataSetInvalidated();

    virtual CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);


    virtual CARAPI_(AutoPtr<IView>) NewView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent);

    virtual CARAPI BindView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI_(AutoPtr<ICharSequence>) ConvertToString(
        /* [in] */ ICursor* cursor);

    virtual AutoPtr<IView> GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent);


    static CARAPI_(String) GetColumnString(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& columnName);
protected:

    CARAPI_(void) UpdateSpinnerState(
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<ICharSequence>) FormatUrl(
        /* [in] */ ICharSequence* url);

    CARAPI_(void) SetViewText(
        /* [in] */ ITextView* v,
        /* [in] */ ICharSequence* text);

    CARAPI_(AutoPtr<IDrawable>) GetIcon1(
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<IDrawable>) GetIcon2(
        /* [in] */ ICursor* cursor);


    CARAPI_(void) SetViewDrawable(
        /* [in] */ IImageView* v,
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 nullVisibility);

    CARAPI_(AutoPtr<IDrawable>) GetDrawableFromResourceValue(
        /* [in] */ const String& id);

    CARAPI_(AutoPtr<IDrawable>) GetDrawable(
        /* [in] */ IUri* uri);

    CARAPI_(AutoPtr<IDrawable>) CheckIconCache(
        /* [in] */ const String& resourceUri);

    CARAPI_(void) StoreInIconCache(
        /* [in] */ const String& resourceUri,
        /* [in] */ IDrawable* drawable);

    CARAPI_(AutoPtr<IDrawable>) GetDefaultIcon1(
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<IDrawable>) GetActivityIconWithCache(
        /* [in] */ IComponentName* component);

    CARAPI_(AutoPtr<IDrawable>) GetActivityIcon(
        /* [in] */ IComponentName* component);

    static CARAPI_(String) GetStringOrNull(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 col);

    const static Boolean DBG; // = FALSE;
    const static String TAG; // = "SuggestionsAdapter";
    const static Int32 QUERY_LIMIT; // = 50;
    const static Int32 INVALID_INDEX; // = -1;
    const static Int64 DELETE_KEY_POST_DELAY; // = 500L;

    AutoPtr<ISearchManager> mSearchManager;
    AutoPtr<ISearchView> mSearchView;
    AutoPtr<ISearchableInfo> mSearchable;
    AutoPtr<IContext> mProviderContext;
    // WeakHashMap<String, Drawable.ConstantState> mOutsideDrawablesCache;
    Boolean mClosed;
    Int32 mQueryRefinement;

    // URL color
    AutoPtr<IColorStateList> mUrlColor;


    // Cached column indexes, updated when the cursor changes.
    Int32 mText1Col; // = INVALID_INDEX;
    Int32 mText2Col; // = INVALID_INDEX;
    Int32 mText2UrlCol; // = INVALID_INDEX;
    Int32 mIconName1Col; // = INVALID_INDEX;
    Int32 mIconName2Col; // = INVALID_INDEX;
    Int32 mFlagsCol; // = INVALID_INDEX;

    // private final Runnable mStartSpinnerRunnable;
    // private final Runnable mStopSpinnerRunnable;

    /**
     * The amount of time we delay in the filter when the user presses the delete key.
     * @see Filter#setDelayer(android.widget.Filter.Delayer).
     */
};

class SuggestionsAdapter
    : public ISuggestionsAdapter
    , public IViewOnClickListener
    , public IFilterable
    , public ICursorFilterClient
    , public ISpinnerAdapter
    , public _SuggestionsAdapter
{
public:
    IADAPTER_METHODS_DECL()
    IBASEADAPTER_METHODS_DECL()
    ILISTADAPTER_METHODS_DECL()
    ISPINNERADAPTER_METHODS_DECL()
    ICURSORADAPTER_METHODS_DECL()
    IRESOURCECURSORADAPTER_METHODS_DECL()
    CAR_INTERFACE_DECL()

    SuggestionsAdapter(
        /* [in] */ IContext* ctx,
        /* [in] */ ISearchView* searchView,
        /* [in] */ ISearchableInfo* searchable,
        /* [in] */ Int32 reservedWords);

    /**
     * Enables query refinement for all suggestions. This means that an additional icon
     * will be shown for each entry. When clicked, the suggested text on that line will be
     * copied to the query text field.
     * <p>
     *
     * @param refine which queries to refine. Possible values are {@link #REFINE_NONE},
     * {@link #REFINE_BY_ENTRY}, and {@link #REFINE_ALL}.
     */
    CARAPI SetQueryRefinement(
        /* [in] */ Int32 refineWhat);

    /**
     * Returns the current query refinement preference.
     * @return value of query refinement preference
     */
    CARAPI GetQueryRefinement(
        /* [out] */ Int32* value);

    CARAPI Close();

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    CARAPI OnClick(
        /* [in] */ IView* v);

    /**
     * <p>Returns a filter that can be used to constrain data with a filtering
     * pattern.</p>
     *
     * <p>This method is usually implemented by {@link android.widget.Adapter}
     * classes.</p>
     *
     * @return a filter used to constrain data
     */
    CARAPI GetFilter(
        /* [out] */ IFilter** filter);

    CARAPI ConvertToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** charSequence);

    CARAPI RunQueryOnBackgroundThread(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ ICursor** cursor);

    CARAPI GetCursor(
        /* [out] */ ICursor** cursor);

    CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);
};

}// Widget
}// Droid
}// Elastos

#endif
