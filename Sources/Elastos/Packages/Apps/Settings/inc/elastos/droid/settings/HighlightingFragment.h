

package com.android.settings;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

public class HighlightingFragment extends Fragment {

    private static const String TAG = "HighlightSettingsFragment";

    private static const Int32 DELAY_HIGHLIGHT_DURATION_MILLIS = 400;
    private static const String SAVE_HIGHLIGHTED_KEY = "android:view_highlighted";

    private String mViewKey;
    private Boolean mViewHighlighted = FALSE;
    private Drawable mHighlightDrawable;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        if (icicle != NULL) {
            mViewHighlighted = icicle->GetBoolean(SAVE_HIGHLIGHTED_KEY);
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        outState->PutBoolean(SAVE_HIGHLIGHTED_KEY, mViewHighlighted);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Bundle args = GetArguments();
        if (args != NULL) {
            mViewKey = args->GetString(SettingsActivity.EXTRA_FRAGMENT_ARG_KEY);
            HighlightViewIfNeeded();
        }
    }

    CARAPI HighlightViewIfNeeded() {
        if (!mViewHighlighted &&!TextUtils->IsEmpty(mViewKey)) {
            HighlightView(mViewKey);
        }
    }

    private Drawable GetHighlightDrawable() {
        if (mHighlightDrawable == NULL) {
            mHighlightDrawable = GetActivity()->GetDrawable(R.drawable.preference_highlight);
        }
        return mHighlightDrawable;
    }

    private void HighlightView(String key) {
        final Drawable highlight = GetHighlightDrawable();

        // Try locating the View thru its Tag / Key
        final View view = FindViewForKey(GetView(), key);
        if (view != NULL ) {
            view->SetBackground(highlight);

            GetView()->PostDelayed(new Runnable() {
                //@Override
                CARAPI Run() {
                    final Int32 centerX = view->GetWidth() / 2;
                    final Int32 centerY = view->GetHeight() / 2;
                    highlight->SetHotspot(centerX, centerY);
                    view->SetPressed(TRUE);
                    view->SetPressed(FALSE);
                }
            }, DELAY_HIGHLIGHT_DURATION_MILLIS);

            mViewHighlighted = TRUE;
        }
    }

    private View FindViewForKey(View root, String key) {
        if (CheckTag(root, key)) {
            return root;
        }
        if (root instanceof ViewGroup) {
            final ViewGroup group = (ViewGroup) root;
            final Int32 count = group->GetChildCount();
            for (Int32 n = 0; n < count; n++) {
                final View child = group->GetChildAt(n);
                final View view = FindViewForKey(child, key);
                if (view != NULL) {
                    return view;
                }
            }
        }
        return NULL;
    }

    private Boolean CheckTag(View view, String key) {
        final Object tag = view->GetTag(R.id.preference_highlight_key);
        if (tag == NULL || !(tag instanceof String)) {
            return FALSE;
        }
        final String viewKey = (String) tag;
        return (!TextUtils->IsEmpty(viewKey) && viewKey->Equals(key));
    }
}
