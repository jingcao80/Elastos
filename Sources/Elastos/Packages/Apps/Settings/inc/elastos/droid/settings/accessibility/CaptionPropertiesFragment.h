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

/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.accessibility;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnLayoutChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ViewGroup::ILayoutParams;
using Elastos::Droid::View::Accessibility::ICaptioningManager;
using Elastos::Droid::View::Accessibility::CaptioningManager::ICaptionStyle;

using Elastos::Droid::Internal::Widget::ISubtitleView;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Accessibility::ListDialogPreference::IOnValueChangedListener;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::IToggleSwitch;
using Elastos::Droid::Settings::Widget::ToggleSwitch::IOnBeforeCheckedChangeListener;

using Elastos::Utility::ILocale;

/**
 * Settings fragment containing captioning properties.
 */
public class CaptionPropertiesFragment extends SettingsPreferenceFragment
        implements OnPreferenceChangeListener, OnValueChangedListener {
    private static const String PREF_BACKGROUND_COLOR = "captioning_background_color";
    private static const String PREF_BACKGROUND_OPACITY = "captioning_background_opacity";
    private static const String PREF_FOREGROUND_COLOR = "captioning_foreground_color";
    private static const String PREF_FOREGROUND_OPACITY = "captioning_foreground_opacity";
    private static const String PREF_WINDOW_COLOR = "captioning_window_color";
    private static const String PREF_WINDOW_OPACITY = "captioning_window_opacity";
    private static const String PREF_EDGE_COLOR = "captioning_edge_color";
    private static const String PREF_EDGE_TYPE = "captioning_edge_type";
    private static const String PREF_FONT_SIZE = "captioning_font_size";
    private static const String PREF_TYPEFACE = "captioning_typeface";
    private static const String PREF_LOCALE = "captioning_locale";
    private static const String PREF_PRESET = "captioning_preset";
    private static const String PREF_CUSTOM = "custom";

    /** WebVtt specifies line height as 5.3% of the viewport height. */
    private static const Float LINE_HEIGHT_RATIO = 0.0533f;

    private CaptioningManager mCaptioningManager;
    private SubtitleView mPreviewText;
    private View mPreviewWindow;
    private View mPreviewViewport;
    private SwitchBar mSwitchBar;
    private ToggleSwitch mToggleSwitch;

    // Standard options.
    private LocalePreference mLocale;
    private ListPreference mFontSize;
    private PresetPreference mPreset;

    // Custom options.
    private ListPreference mTypeface;
    private ColorPreference mForegroundColor;
    private ColorPreference mForegroundOpacity;
    private EdgeTypePreference mEdgeType;
    private ColorPreference mEdgeColor;
    private ColorPreference mBackgroundColor;
    private ColorPreference mBackgroundOpacity;
    private ColorPreference mWindowColor;
    private ColorPreference mWindowOpacity;
    private PreferenceCategory mCustom;

    private Boolean mShowingCustom;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mCaptioningManager = (CaptioningManager) GetSystemService(Context.CAPTIONING_SERVICE);

        AddPreferencesFromResource(R.xml.captioning_settings);
        InitializeAllPreferences();
        UpdateAllPreferences();
        RefreshShowingCustom();
        InstallUpdateListeners();
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View rootView = inflater->Inflate(R.layout.captioning_preview, container, FALSE);

        // We have to do this now because PreferenceFrameLayout looks at it
        // only when the view is added.
        if (container instanceof PreferenceFrameLayout) {
            ((PreferenceFrameLayout.LayoutParams) rootView->GetLayoutParams()).removeBorders = TRUE;
        }

        final View content = super->OnCreateView(inflater, container, savedInstanceState);
        ((ViewGroup) rootView->FindViewById(R.id.properties_fragment)).AddView(
                content, LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);

        return rootView;
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);

        final Boolean enabled = mCaptioningManager->IsEnabled();
        mPreviewText = (SubtitleView) view->FindViewById(R.id.preview_text);
        mPreviewText->SetVisibility(enabled ? View.VISIBLE : View.INVISIBLE);

        mPreviewWindow = view->FindViewById(R.id.preview_window);
        mPreviewViewport = view->FindViewById(R.id.preview_viewport);
        mPreviewViewport->AddOnLayoutChangeListener(new OnLayoutChangeListener() {
            //@Override
            CARAPI OnLayoutChange(View v, Int32 left, Int32 top, Int32 right, Int32 bottom,
                    Int32 oldLeft, Int32 oldTop, Int32 oldRight, Int32 oldBottom) {
                RefreshPreviewText();
            }
        });
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Boolean enabled = mCaptioningManager->IsEnabled();
        SettingsActivity activity = (SettingsActivity) GetActivity();
        mSwitchBar = activity->GetSwitchBar();
        mSwitchBar->SetCheckedInternal(enabled);
        mToggleSwitch = mSwitchBar->GetSwitch();

        GetPreferenceScreen()->SetEnabled(enabled);

        RefreshPreviewText();

        InstallSwitchBarToggleSwitch();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        RemoveSwitchBarToggleSwitch();
    }

    private void RefreshPreviewText() {
        final Context context = GetActivity();
        if (context == NULL) {
            // We've been destroyed, abort!
            return;
        }

        final SubtitleView preview = mPreviewText;
        if (preview != NULL) {
            final Int32 styleId = mCaptioningManager->GetRawUserStyle();
            ApplyCaptionProperties(mCaptioningManager, preview, mPreviewViewport, styleId);

            final Locale locale = mCaptioningManager->GetLocale();
            if (locale != NULL) {
                final CharSequence localizedText = AccessibilityUtils->GetTextForLocale(
                        context, locale, R::string::captioning_preview_text);
                preview->SetText(localizedText);
            } else {
                preview->SetText(R::string::captioning_preview_text);
            }

            final CaptionStyle style = mCaptioningManager->GetUserStyle();
            if (style->HasWindowColor()) {
                mPreviewWindow->SetBackgroundColor(style.windowColor);
            } else {
                final CaptionStyle defStyle = CaptionStyle.DEFAULT;
                mPreviewWindow->SetBackgroundColor(defStyle.windowColor);
            }
        }
    }

    public static void ApplyCaptionProperties(CaptioningManager manager, SubtitleView previewText,
            View previewWindow, Int32 styleId) {
        previewText->SetStyle(styleId);

        final Context context = previewText->GetContext();
        final ContentResolver cr = context->GetContentResolver();
        final Float fontScale = manager->GetFontScale();
        if (previewWindow != NULL) {
            // Assume the viewport is clipped with a 16:9 aspect ratio.
            final Float virtualHeight = Math->Max(9 * previewWindow->GetWidth(),
                    16 * previewWindow->GetHeight()) / 16.0f;
            previewText->SetTextSize(virtualHeight * LINE_HEIGHT_RATIO * fontScale);
        } else {
            final Float textSize = context->GetResources()->GetDimension(
                    R.dimen.caption_preview_text_size);
            previewText->SetTextSize(textSize * fontScale);
        }

        final Locale locale = manager->GetLocale();
        if (locale != NULL) {
            final CharSequence localizedText = AccessibilityUtils->GetTextForLocale(
                    context, locale, R::string::captioning_preview_characters);
            previewText->SetText(localizedText);
        } else {
            previewText->SetText(R::string::captioning_preview_characters);
        }
    }

    protected void OnInstallSwitchBarToggleSwitch() {
        mToggleSwitch->SetOnBeforeCheckedChangeListener(new OnBeforeCheckedChangeListener() {
            //@Override
            public Boolean OnBeforeCheckedChanged(ToggleSwitch toggleSwitch, Boolean checked) {
                mSwitchBar->SetCheckedInternal(checked);
                Settings.Secure->PutInt(GetActivity()->GetContentResolver(),
                        Settings.Secure.ACCESSIBILITY_CAPTIONING_ENABLED, checked ? 1 : 0);
                GetPreferenceScreen()->SetEnabled(checked);
                if (mPreviewText != NULL) {
                    mPreviewText->SetVisibility(checked ? View.VISIBLE : View.INVISIBLE);
                }
                return FALSE;
            }
        });
    }

    private void InstallSwitchBarToggleSwitch() {
        OnInstallSwitchBarToggleSwitch();
        mSwitchBar->Show();
    }

    private void RemoveSwitchBarToggleSwitch() {
        mSwitchBar->Hide();
        mToggleSwitch->SetOnBeforeCheckedChangeListener(NULL);
    }

    private void InitializeAllPreferences() {
        mLocale = (LocalePreference) FindPreference(PREF_LOCALE);
        mFontSize = (ListPreference) FindPreference(PREF_FONT_SIZE);

        final Resources res = GetResources();
        final Int32[] presetValues = res->GetIntArray(R.array.captioning_preset_selector_values);
        final String[] presetTitles = res->GetStringArray(R.array.captioning_preset_selector_titles);
        mPreset = (PresetPreference) FindPreference(PREF_PRESET);
        mPreset->SetValues(presetValues);
        mPreset->SetTitles(presetTitles);

        mCustom = (PreferenceCategory) FindPreference(PREF_CUSTOM);
        mShowingCustom = TRUE;

        final Int32[] colorValues = res->GetIntArray(R.array.captioning_color_selector_values);
        final String[] colorTitles = res->GetStringArray(R.array.captioning_color_selector_titles);
        mForegroundColor = (ColorPreference) mCustom->FindPreference(PREF_FOREGROUND_COLOR);
        mForegroundColor->SetTitles(colorTitles);
        mForegroundColor->SetValues(colorValues);

        final Int32[] opacityValues = res->GetIntArray(R.array.captioning_opacity_selector_values);
        final String[] opacityTitles = res->GetStringArray(
                R.array.captioning_opacity_selector_titles);
        mForegroundOpacity = (ColorPreference) mCustom->FindPreference(PREF_FOREGROUND_OPACITY);
        mForegroundOpacity->SetTitles(opacityTitles);
        mForegroundOpacity->SetValues(opacityValues);

        mEdgeColor = (ColorPreference) mCustom->FindPreference(PREF_EDGE_COLOR);
        mEdgeColor->SetTitles(colorTitles);
        mEdgeColor->SetValues(colorValues);

        // Add "none" as an additional option for backgrounds.
        final Int32[] bgColorValues = new Int32[colorValues.length + 1];
        final String[] bgColorTitles = new String[colorTitles.length + 1];
        System->Arraycopy(colorValues, 0, bgColorValues, 1, colorValues.length);
        System->Arraycopy(colorTitles, 0, bgColorTitles, 1, colorTitles.length);
        bgColorValues[0] = Color.TRANSPARENT;
        bgColorTitles[0] = GetString(R::string::color_none);
        mBackgroundColor = (ColorPreference) mCustom->FindPreference(PREF_BACKGROUND_COLOR);
        mBackgroundColor->SetTitles(bgColorTitles);
        mBackgroundColor->SetValues(bgColorValues);

        mBackgroundOpacity = (ColorPreference) mCustom->FindPreference(PREF_BACKGROUND_OPACITY);
        mBackgroundOpacity->SetTitles(opacityTitles);
        mBackgroundOpacity->SetValues(opacityValues);

        mWindowColor = (ColorPreference) mCustom->FindPreference(PREF_WINDOW_COLOR);
        mWindowColor->SetTitles(bgColorTitles);
        mWindowColor->SetValues(bgColorValues);

        mWindowOpacity = (ColorPreference) mCustom->FindPreference(PREF_WINDOW_OPACITY);
        mWindowOpacity->SetTitles(opacityTitles);
        mWindowOpacity->SetValues(opacityValues);

        mEdgeType = (EdgeTypePreference) mCustom->FindPreference(PREF_EDGE_TYPE);
        mTypeface = (ListPreference) mCustom->FindPreference(PREF_TYPEFACE);
    }

    private void InstallUpdateListeners() {
        mPreset->SetOnValueChangedListener(this);
        mForegroundColor->SetOnValueChangedListener(this);
        mForegroundOpacity->SetOnValueChangedListener(this);
        mEdgeColor->SetOnValueChangedListener(this);
        mBackgroundColor->SetOnValueChangedListener(this);
        mBackgroundOpacity->SetOnValueChangedListener(this);
        mWindowColor->SetOnValueChangedListener(this);
        mWindowOpacity->SetOnValueChangedListener(this);
        mEdgeType->SetOnValueChangedListener(this);

        mTypeface->SetOnPreferenceChangeListener(this);
        mFontSize->SetOnPreferenceChangeListener(this);
        mLocale->SetOnPreferenceChangeListener(this);
    }

    private void UpdateAllPreferences() {
        final Int32 preset = mCaptioningManager->GetRawUserStyle();
        mPreset->SetValue(preset);

        final Float fontSize = mCaptioningManager->GetFontScale();
        mFontSize->SetValue(Float->ToString(fontSize));

        final ContentResolver cr = GetContentResolver();
        final CaptionStyle attrs = CaptionStyle->GetCustomStyle(cr);
        mEdgeType->SetValue(attrs.edgeType);
        mEdgeColor->SetValue(attrs.edgeColor);

        ParseColorOpacity(mForegroundColor, mForegroundOpacity, attrs.foregroundColor);
        ParseColorOpacity(mBackgroundColor, mBackgroundOpacity, attrs.backgroundColor);
        ParseColorOpacity(mWindowColor, mWindowOpacity, attrs.windowColor);

        final String rawTypeface = attrs.mRawTypeface;
        mTypeface->SetValue(rawTypeface == NULL ? "" : rawTypeface);

        final String rawLocale = mCaptioningManager->GetRawLocale();
        mLocale->SetValue(rawLocale == NULL ? "" : rawLocale);
    }

    private void ParseColorOpacity(ColorPreference color, ColorPreference opacity, Int32 value) {
        final Int32 colorValue;
        final Int32 opacityValue;
        if (Color->Alpha(value) == 0) {
            colorValue = Color.TRANSPARENT;
            opacityValue = (value & 0xFF) << 24;
        } else {
            colorValue = value | 0xFF000000;
            opacityValue = value & 0xFF000000;
        }
        color->SetValue(colorValue);
        opacity->SetValue(opacityValue | 0xFFFFFF);
    }

    private Int32 MergeColorOpacity(ColorPreference color, ColorPreference opacity) {
        final Int32 colorValue = color->GetValue();
        final Int32 opacityValue = opacity->GetValue();
        final Int32 value;
        if (Color->Alpha(colorValue) == 0) {
            value = colorValue & 0x00FFFF00 | Color->Alpha(opacityValue);
        } else {
            value = colorValue & 0x00FFFFFF | opacityValue & 0xFF000000;
        }
        return value;
    }

    private void RefreshShowingCustom() {
        final Boolean customPreset = mPreset->GetValue() == CaptionStyle.PRESET_CUSTOM;
        if (!customPreset && mShowingCustom) {
            GetPreferenceScreen()->RemovePreference(mCustom);
            mShowingCustom = FALSE;
        } else if (customPreset && !mShowingCustom) {
            GetPreferenceScreen()->AddPreference(mCustom);
            mShowingCustom = TRUE;
        }
    }

    //@Override
    CARAPI OnValueChanged(ListDialogPreference preference, Int32 value) {
        final ContentResolver cr = GetActivity()->GetContentResolver();
        if (mForegroundColor == preference || mForegroundOpacity == preference) {
            final Int32 merged = MergeColorOpacity(mForegroundColor, mForegroundOpacity);
            Settings.Secure->PutInt(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_FOREGROUND_COLOR, merged);
        } else if (mBackgroundColor == preference || mBackgroundOpacity == preference) {
            final Int32 merged = MergeColorOpacity(mBackgroundColor, mBackgroundOpacity);
            Settings.Secure->PutInt(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_BACKGROUND_COLOR, merged);
        } else if (mWindowColor == preference || mWindowOpacity == preference) {
            final Int32 merged = MergeColorOpacity(mWindowColor, mWindowOpacity);
            Settings.Secure->PutInt(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_WINDOW_COLOR, merged);
        } else if (mEdgeColor == preference) {
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_EDGE_COLOR, value);
        } else if (mPreset == preference) {
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_PRESET, value);
            RefreshShowingCustom();
        } else if (mEdgeType == preference) {
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_EDGE_TYPE, value);
        }

        RefreshPreviewText();
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object value) {
        final ContentResolver cr = GetActivity()->GetContentResolver();
        if (mTypeface == preference) {
            Settings.Secure->PutString(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_TYPEFACE, (String) value);
        } else if (mFontSize == preference) {
            Settings.Secure->PutFloat(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_FONT_SCALE,
                    Float->ParseFloat((String) value));
        } else if (mLocale == preference) {
            Settings.Secure->PutString(
                    cr, Settings.Secure.ACCESSIBILITY_CAPTIONING_LOCALE, (String) value);
        }

        RefreshPreviewText();
        return TRUE;
    }
}
