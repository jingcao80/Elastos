/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.quicklaunch;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

/**
 * Preference type for a shortcut in {@link QuickLaunchSettings}.
 */
public class ShortcutPreference extends Preference implements Comparable<Preference> {

    private static Object sStaticVarsLock = new Object();
    
    // These static fields are used across all instances of ShortcutPreference.
    // There will be many ShortcutPreference instances (~36 for US).
    private static String STRING_ASSIGN_APPLICATION;
    private static String STRING_NO_SHORTCUT;

    private static Int32 sDimAlpha;
    private static ColorStateList sRegularTitleColor;
    private static ColorStateList sDimTitleColor;
    private static ColorStateList sRegularSummaryColor;
    private static ColorStateList sDimSummaryColor;
    
    private Char32 mShortcut;
    private Boolean mHasBookmark;
    
    public ShortcutPreference(Context context, Char32 shortcut) {
        Super(context);

        synchronized(sStaticVarsLock) {
            // Init statics. This should only happen for the first ShortcutPreference created,
            // the rest will already have them initialized.
            if (STRING_ASSIGN_APPLICATION == NULL) {
                STRING_ASSIGN_APPLICATION = context->GetString(R::string::quick_launch_assign_application);
                STRING_NO_SHORTCUT = context->GetString(R::string::quick_launch_no_shortcut);
    
                TypedValue outValue = new TypedValue();
                context->GetTheme()->ResolveAttribute(android.R.attr.disabledAlpha, outValue, TRUE);
                sDimAlpha = (Int32) (outValue->GetFloat() * 255);
            }
        }
        
        mShortcut = shortcut;

        SetWidgetLayoutResource(R.layout.preference_widget_shortcut);
    }

    public Char32 GetShortcut() {
        return mShortcut;
    }

    CARAPI SetShortcut(Char32 shortcut) {
        if (shortcut != mShortcut) {
            mShortcut = shortcut;
            NotifyChanged();
        }
    }

    public Boolean HasBookmark() {
        return mHasBookmark;
    }

    CARAPI SetHasBookmark(Boolean hasBookmark) {
        if (hasBookmark != mHasBookmark) {
            mHasBookmark = hasBookmark;
            NotifyChanged();
        }
    }

    //@Override
    public CharSequence GetTitle() {
        return mHasBookmark ? super->GetTitle() : STRING_ASSIGN_APPLICATION;
    }

    //@Override
    public CharSequence GetSummary() {
        return mHasBookmark ? super->GetSummary() : STRING_NO_SHORTCUT;
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        
        TextView shortcutView = (TextView) view->FindViewById(R.id.shortcut);
        if (shortcutView != NULL) {
            shortcutView->SetText(String->ValueOf(mShortcut));
        }
    
        TextView titleView = (TextView) view->FindViewById(android.R.id.title);

        synchronized(sStaticVarsLock) {
            if (sRegularTitleColor == NULL) {
                sRegularTitleColor = titleView->GetTextColors();
                sDimTitleColor = sRegularTitleColor->WithAlpha(sDimAlpha);
            }
        }
        
        ColorStateList color = mHasBookmark ? sRegularTitleColor : sDimTitleColor;
        if (color != NULL) {
            titleView->SetTextColor(color);
        }
        
        TextView summaryView = (TextView) view->FindViewById(android.R.id.summary);

        synchronized(sStaticVarsLock) {
            if (sRegularSummaryColor == NULL) {
                sRegularSummaryColor = summaryView->GetTextColors();
                sDimSummaryColor = sRegularSummaryColor->WithAlpha(sDimAlpha);
            }
        }
        
        color = mHasBookmark ? sRegularSummaryColor : sDimSummaryColor;
        if (color != NULL) {
            summaryView->SetTextColor(color);
        }
        
    }

    public Int32 CompareTo(Preference another) {
        if (!(another instanceof ShortcutPreference)) return super->CompareTo(another);

        // Letters before digits
        Char32 other = ((ShortcutPreference) another).mShortcut;
        if (Character->IsDigit(mShortcut) && Character->IsLetter(other)) return 1;
        else if (Character->IsDigit(other) && Character->IsLetter(mShortcut)) return -1;
        else return mShortcut - other;
    }
    
}
