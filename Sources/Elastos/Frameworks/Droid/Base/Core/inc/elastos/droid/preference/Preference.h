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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC Preference
    : public Object
    , public IPreference
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    Preference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    virtual CARAPI SetIntent(
        /* [in] */ IIntent* intent);

    virtual CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    virtual CARAPI SetFragment(
        /* [in] */ const String& fragment);

    virtual CARAPI GetFragment(
        /* [out] */ String* fragment);

    virtual CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    virtual CARAPI PeekExtras(
        /* [out] */ IBundle** extras);

    virtual CARAPI SetLayoutResource(
        /* [in] */ Int32 layoutResId);

    virtual CARAPI GetLayoutResource(
        /* [out] */ Int32* layoutResId);

    virtual CARAPI SetWidgetLayoutResource(
        /* [in] */ Int32 widgetLayoutResId);

    virtual CARAPI GetWidgetLayoutResource(
        /* [out] */ Int32* widgetLayoutResId);

    virtual CARAPI GetView(
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

protected:
    virtual CARAPI OnCreateView(
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI OnBindView(
        /* [in] */ IView* view);

public:
    virtual CARAPI SetOrder(
        /* [in] */ Int32 order);

    virtual CARAPI GetOrder(
        /* [out] */ Int32* order);

    virtual CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    virtual CARAPI SetTitle(
        /* [in] */ Int32 titleResId);

    virtual CARAPI GetTitleRes(
        /* [out] */ Int32* resId);

    virtual CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    virtual CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    virtual CARAPI SetIcon(
        /* [in] */ Int32 iconResId);

    virtual CARAPI GetIcon(
        /* [out] */ IDrawable** drawable);

    virtual CARAPI GetSummary(
        /* [out] */ ICharSequence** summary);

    virtual CARAPI SetSummary(
        /* [in] */ ICharSequence* summary);

    virtual CARAPI SetSummary(
        /* [in] */ Int32 summaryResId);

    virtual CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsEnabled(
        /* [out] */ Boolean* isEnable);

    virtual CARAPI SetSelectable(
        /* [in] */ Boolean selectable);

    virtual CARAPI IsSelectable(
        /* [out] */ Boolean* isSelectable);

    virtual CARAPI SetShouldDisableView(
        /* [in] */ Boolean shouldDisableView);

    virtual CARAPI GetShouldDisableView(
        /* [out] */ Boolean* shouldDisableView);

protected:
    CARAPI GetId(
        /* [out] */ Int64* id);

    virtual CARAPI OnClick();

public:
    virtual CARAPI SetKey(
        /* [in] */ const String& key);

    virtual CARAPI GetKey(
        /* [out] */ String* key);

protected:
    CARAPI RequireKey();

public:
    virtual CARAPI HasKey(
        /* [out] */ Boolean* hasKey);

    virtual CARAPI IsPersistent(
        /* [out] */ Boolean* isPersistent);

    virtual CARAPI ShouldPersist(
        /* [out] */ Boolean* shouldPersist);

    virtual CARAPI SetPersistent(
        /* [in] */ Boolean persistent);

protected:
    virtual CARAPI CallChangeListener(
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* shouldSetValue);

    /**
     * Returns whether the preference can be found in persistent storage
     * @hide
     */
    virtual CARAPI IsPersisted(
        /* [out] */ Boolean* result);

public:
    virtual CARAPI SetOnPreferenceChangeListener(
        /* [in] */ IPreferenceOnPreferenceChangeListener* onPreferenceChangeListener);

    virtual CARAPI GetOnPreferenceChangeListener(
        /* [out] */ IPreferenceOnPreferenceChangeListener** listener);

    virtual CARAPI SetOnPreferenceClickListener(
        /* [in] */ IPreferenceOnPreferenceClickListener* onPreferenceClickListener);

    virtual CARAPI GetOnPreferenceClickListener(
        /* [out] */ IPreferenceOnPreferenceClickListener** listener);

    virtual CARAPI PerformClick(
        /* [in] */ IPreferenceScreen* preferenceScreen);

    /**
     * Allows a Preference to intercept key events without having focus.
     * For example, SeekBarPreference uses this to intercept +/- to adjust
     * the progress.
     * @return True if the Preference handled the key. Returns false by default.
     * @hide
     */
    virtual CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI GetContext(
        /* [out] */ IContext** context);

    virtual CARAPI GetSharedPreferences(
        /* [out] */ ISharedPreferences** preference);

    virtual CARAPI GetEditor(
        /* [out] */ ISharedPreferencesEditor** editor);

    virtual CARAPI ShouldCommit(
        /* [out] */ Boolean* shouldCommit);

    virtual CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

    CARAPI SetOnPreferenceChangeInternalListener(
        /* [in] */ IPreferenceOnPreferenceChangeInternalListener* listener);

    virtual CARAPI NotifyChanged();

    virtual CARAPI NotifyHierarchyChanged();

    virtual CARAPI GetPreferenceManager(
        /* [out] */ IPreferenceManager** manager);

    virtual CARAPI OnAttachedToHierarchy(
        /* [in] */ IPreferenceManager* preferenceManager);

    virtual CARAPI OnAttachedToActivity();

    virtual CARAPI FindPreferenceInHierarchy(
        /* [in] */ const String& key,
        /* [out] */ IPreference** preference);

    virtual CARAPI NotifyDependencyChange(
        /* [in] */ Boolean disableDependents);

    virtual CARAPI OnDependencyChanged(
        /* [in] */ IPreference* dependency,
        /* [in] */ Boolean disableDependent);

    /**
     * Called when the implicit parent dependency changes.
     *
     * @param parent The Preference that this Preference depends on.
     * @param disableChild Set true to disable this Preference.
     */
    virtual CARAPI OnParentChanged(
        /* [in] */ IPreference* parent,
        /* [in] */ Boolean disableChild);

    virtual CARAPI ShouldDisableDependents(
        /* [out] */ Boolean* shouldDisableDependents);

    virtual CARAPI SetDependency(
        /* [in] */ const String& dependencyKey);

    virtual CARAPI GetDependency(
        /* [out] */ String* dependencyKey);

    virtual CARAPI OnPrepareForRemoval();

    virtual CARAPI SetDefaultValue(
        /* [in] */ IInterface* defaultValue);

    virtual CARAPI OnSetInitialValue(
        /* [in] */ Boolean restorePersistedValue,
        /* [in] */ IInterface* defaultValue);

    virtual CARAPI PersistString(
        /* [in] */ const String& value,
        /* [out] */ Boolean* isPersistString);

    virtual CARAPI GetPersistedString(
        /* [in] */ const String& defaultReturnValue,
        /* [out] */ String* str);

    virtual CARAPI PersistStringSet(
        /* [in] */ ISet* values,
        /* [out] */ Boolean* isPersistStringSet);

    virtual CARAPI GetPersistedStringSet(
        /* [in] */ ISet* defaultReturnValue,
        /* [out] */ ISet** value);

    virtual CARAPI PersistInt32(
        /* [in] */ Int32 value,
        /* [out] */ Boolean* isPersist);


    virtual CARAPI GetPersistedInt32(
        /* [in] */ Int32 defaultReturnValue,
        /* [out] */ Int32* value);

    virtual CARAPI PersistFloat(
        /* [in] */ Float value,
        /* [out] */ Boolean* isPersist);

    virtual CARAPI GetPersistedFloat(
        /* [in] */ Float defaultReturnValue,
        /* [out] */ Float* value);

    virtual CARAPI PersistInt64(
        /* [in] */ Int64 value,
        /* [out] */ Boolean* isPersist);

    virtual CARAPI GetPersistedInt64(
        /* [in] */ Int64 defaultReturnValue,
        /* [out] */ Int64* value);

    virtual CARAPI PersistBoolean(
        /* [in] */ Boolean value,
        /* [out] */ Boolean* isPersist);

    virtual CARAPI GetPersistedBoolean(
        /* [in] */ Boolean defaultReturnValue,
        /* [out] */ Boolean* value);

    virtual CARAPI CanRecycleLayout(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    virtual CARAPI GetFilterableString(
        /* [out] */ String* fsb);

    virtual CARAPI SaveHierarchyState(
        /* [in] */ IBundle* container);

    virtual CARAPI DispatchSaveInstanceState(
        /* [in] */ IBundle* container);

    virtual CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    virtual CARAPI RestoreHierarchyState(
        /* [in] */ IBundle* container);

    virtual CARAPI DispatchRestoreInstanceState(
        /* [in] */ IBundle* container);

    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    /**
     * Makes sure the view (and any children) get the enabled state changed.
     */
    ECO_LOCAL CARAPI_(void) SetEnabledStateOnViews(
        /* [in] */ IView* v,
        /* [in] */ Boolean enabled);

    ECO_LOCAL CARAPI RegisterDependency();

    ECO_LOCAL CARAPI_(void) UnregisterDependency();

    ECO_LOCAL CARAPI RegisterDependent(
        /* [in] */ Preference* dependent);

    ECO_LOCAL CARAPI_(void) UnRegisterDependent(
        /* [in] */ Preference* dependent);

    ECO_LOCAL CARAPI DispatchSetInitialValue();

    ECO_LOCAL CARAPI_(void) TryCommit(
        /* [in] */ ISharedPreferencesEditor* editor);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IPreferenceManager> mPreferenceManager;

    /**
     * Set when added to hierarchy since we need a unique ID within that
     * hierarchy.
     */
    Int64 mId;

    AutoPtr<IPreferenceOnPreferenceChangeListener> mOnChangeListener;
    AutoPtr<IPreferenceOnPreferenceClickListener> mOnClickListener;

    Int32 mOrder;
    AutoPtr<ICharSequence> mTitle;
    Int32 mTitleRes;
    AutoPtr<ICharSequence> mSummary;
    /**
     * mIconResId is overridden by mIcon, if mIcon is specified.
     */
    Int32 mIconResId;
    AutoPtr<IDrawable> mIcon;
    String mKey;
    AutoPtr<IIntent> mIntent;
    String mFragment;
    AutoPtr<IBundle> mExtras;
    Boolean mEnabled;
    Boolean mSelectable;
    Boolean mRequiresKey;
    Boolean mPersistent;
    String mDependencyKey;
    AutoPtr<IInterface> mDefaultValue;
    Boolean mDependencyMet;
    Boolean mParentDependencyMet;

    /**
     * @see #setShouldDisableView(boolean)
     */
    Boolean mShouldDisableView;

    Int32 mLayoutResId;
    Int32 mWidgetLayoutResId;
    Boolean mCanRecycleLayout;
    AutoPtr<IPreferenceOnPreferenceChangeInternalListener> mListener;
    AutoPtr< List<AutoPtr<IPreference> > > mDependents;
    Boolean mBaseMethodCalled;
};

} // Preference
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCE_H__
