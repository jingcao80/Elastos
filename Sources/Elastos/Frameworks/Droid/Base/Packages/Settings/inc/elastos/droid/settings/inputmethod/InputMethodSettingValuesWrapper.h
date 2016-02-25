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

package com.android.settings.inputmethod;

using Elastos::Droid::Internal::Inputmethod::IInputMethodUtils;
using Elastos::Droid::Internal::Inputmethod::InputMethodUtils::IInputMethodSettings;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

/**
 * This class is a wrapper for InputMethodSettings. You need to refresh internal states
 * manually on some events when "InputMethodInfo"s and "InputMethodSubtype"s can be
 * changed.
 */
// TODO: Consolidate this with {@link InputMethodAndSubtypeUtil}.
class InputMethodSettingValuesWrapper {
    private static const String TAG = InputMethodSettingValuesWrapper.class->GetSimpleName();

    private static volatile InputMethodSettingValuesWrapper sInstance;
    private final ArrayList<InputMethodInfo> mMethodList = new ArrayList<>();
    private final HashMap<String, InputMethodInfo> mMethodMap = new HashMap<>();
    private final InputMethodSettings mSettings;
    private final InputMethodManager mImm;
    private final HashSet<InputMethodInfo> mAsciiCapableEnabledImis = new HashSet<>();

    static InputMethodSettingValuesWrapper GetInstance(Context context) {
        if (sInstance == NULL) {
            synchronized(TAG) {
                if (sInstance == NULL) {
                    sInstance = new InputMethodSettingValuesWrapper(context);
                }
            }
        }
        return sInstance;
    }

    private static Int32 GetDefaultCurrentUserId() {
        try {
            return ActivityManagerNative->GetDefault()->GetCurrentUser().id;
        } catch (RemoteException e) {
            Slogger::W(TAG, "Couldn't get current user ID; guessing it's 0", e);
        }
        return 0;
    }

    // Ensure singleton
    private InputMethodSettingValuesWrapper(Context context) {
        mSettings = new InputMethodSettings(context->GetResources(), context->GetContentResolver(),
                mMethodMap, mMethodList, GetDefaultCurrentUserId());
        mImm = (InputMethodManager) context->GetSystemService(Context.INPUT_METHOD_SERVICE);
        RefreshAllInputMethodAndSubtypes();
    }

    void RefreshAllInputMethodAndSubtypes() {
        synchronized(mMethodMap) {
            mMethodList->Clear();
            mMethodMap->Clear();
            final List<InputMethodInfo> imms = mImm->GetInputMethodList();
            mMethodList->AddAll(imms);
            for (InputMethodInfo imi : imms) {
                mMethodMap->Put(imi->GetId(), imi);
            }
            UpdateAsciiCapableEnabledImis();
        }
    }

    // TODO: Add a cts to ensure at least one AsciiCapableSubtypeEnabledImis exist
    private void UpdateAsciiCapableEnabledImis() {
        synchronized(mMethodMap) {
            mAsciiCapableEnabledImis->Clear();
            final List<InputMethodInfo> enabledImis = mSettings->GetEnabledInputMethodListLocked();
            for (final InputMethodInfo imi : enabledImis) {
                final Int32 subtypeCount = imi->GetSubtypeCount();
                for (Int32 i = 0; i < subtypeCount; ++i) {
                    final InputMethodSubtype subtype = imi->GetSubtypeAt(i);
                    if (InputMethodUtils.SUBTYPE_MODE_KEYBOARD->EqualsIgnoreCase(subtype->GetMode())
                            && subtype->IsAsciiCapable()) {
                        mAsciiCapableEnabledImis->Add(imi);
                        break;
                    }
                }
            }
        }
    }

    List<InputMethodInfo> GetInputMethodList() {
        synchronized(mMethodMap) {
            return mMethodList;
        }
    }

    CharSequence GetCurrentInputMethodName(Context context) {
        synchronized(mMethodMap) {
            final InputMethodInfo imi = mMethodMap->Get(mSettings->GetSelectedInputMethod());
            if (imi == NULL) {
                Logger::W(TAG, "Invalid selected imi: " + mSettings->GetSelectedInputMethod());
                return "";
            }
            final InputMethodSubtype subtype = mImm->GetCurrentInputMethodSubtype();
            return InputMethodUtils->GetImeAndSubtypeDisplayName(context, imi, subtype);
        }
    }

    Boolean IsAlwaysCheckedIme(InputMethodInfo imi, Context context) {
        final Boolean isEnabled = IsEnabledImi(imi);
        synchronized(mMethodMap) {
            if (mSettings->GetEnabledInputMethodListLocked()->Size() <= 1 && isEnabled) {
                return TRUE;
            }
        }

        final Int32 enabledValidSystemNonAuxAsciiCapableImeCount =
                GetEnabledValidSystemNonAuxAsciiCapableImeCount(context);
        if (enabledValidSystemNonAuxAsciiCapableImeCount > 1) {
            return FALSE;
        }

        if (enabledValidSystemNonAuxAsciiCapableImeCount == 1 && !isEnabled) {
            return FALSE;
        }

        if (!InputMethodUtils->IsSystemIme(imi)) {
            return FALSE;
        }
        return IsValidSystemNonAuxAsciiCapableIme(imi, context);
    }

    private Int32 GetEnabledValidSystemNonAuxAsciiCapableImeCount(Context context) {
        Int32 count = 0;
        final List<InputMethodInfo> enabledImis;
        synchronized(mMethodMap) {
            enabledImis = mSettings->GetEnabledInputMethodListLocked();
        }
        for (final InputMethodInfo imi : enabledImis) {
            if (IsValidSystemNonAuxAsciiCapableIme(imi, context)) {
                ++count;
            }
        }
        if (count == 0) {
            Logger::W(TAG, "No \"enabledValidSystemNonAuxAsciiCapableIme\"s found.");
        }
        return count;
    }

    Boolean IsEnabledImi(InputMethodInfo imi) {
        final List<InputMethodInfo> enabledImis;
        synchronized(mMethodMap) {
            enabledImis = mSettings->GetEnabledInputMethodListLocked();
        }
        for (final InputMethodInfo tempImi : enabledImis) {
            if (tempImi->GetId()->Equals(imi->GetId())) {
                return TRUE;
            }
        }
        return FALSE;
    }

    Boolean IsValidSystemNonAuxAsciiCapableIme(InputMethodInfo imi, Context context) {
        if (imi->IsAuxiliaryIme()) {
            return FALSE;
        }
        if (InputMethodUtils->IsValidSystemDefaultIme(TRUE /* isSystemReady */, imi, context)) {
            return TRUE;
        }
        if (mAsciiCapableEnabledImis->IsEmpty()) {
            Logger::W(TAG, "ascii capable subtype enabled imi not found. Fall back to English"
                    + " Keyboard subtype.");
            return InputMethodUtils->ContainsSubtypeOf(imi, Locale.ENGLISH->GetLanguage(),
                    InputMethodUtils.SUBTYPE_MODE_KEYBOARD);
        }
        return mAsciiCapableEnabledImis->Contains(imi);
    }
}
