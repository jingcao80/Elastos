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

#ifndef __ELASTOS_DROID_OS_ANDROIDPARCELUTILS_H__
#define __ELASTOS_DROID_OS_ANDROIDPARCELUTILS_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/content/pm/ComponentInfo.h"
#include "elastos/droid/content/pm/PackageItemInfo.h"
#include "elastos/droid/net/Uri.h"
#include <binder/Parcel.h>

using Elastos::Droid::App::IActivityManagerTaskDescription;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IContentProviderHolder;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IIUiAutomationConnection;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::ComponentInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IConfigurationInfo;
using Elastos::Droid::Content::Pm::IFeatureInfo;
using Elastos::Droid::Content::Pm::IFeatureGroupInfo;
using Elastos::Droid::Content::Pm::IInstrumentationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPathPermission;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::PackageItemInfo;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Hardware::Display::IIDisplayManagerCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Net::AbstractPart;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IIGraphicsStatsCallback;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IIWindowSessionCallback;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowSession;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnection;
using Elastos::Droid::View::Accessibility::IIAccessibilityManagerClient;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::TextService::ISpellCheckerSubtype;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::LayoutAlignment;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Os {

class AndroidParcelUtils
{
public:
    // read methods
    static CARAPI_(Byte) ReadByte(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(Int32) ReadInt32(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) CreateInt32Array(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(Int64) ReadInt64(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(Float) ReadFloat(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(String) ReadString(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<String> >) CreateStringArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IInterface>) ReadValue(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IParcelable>) ReadParcelable(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ICharSequence>) Read_CHAR_SEQUENCE(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IFileDescriptor>) ReadFileDescriptor(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IMap>) ReadHashMap(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IActivityOptions>) ReadActivityOptions(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IContentProviderHolder>) ReadContentProviderHolder(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IInstrumentationWatcher>) ReadIInstrumentationWatcher(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IIUiAutomationConnection>) ReadIUiAutomationConnection(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPendingIntent>) ReadPendingIntent(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IProfilerInfo>) ReadProfilerInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IResultInfo>) ReadResultInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IList>) CreateResultInfoList(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IClipData>) ReadClipData(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IComponentName>) ReadComponentName(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IIntent>) ReadIntent(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IIntentFilter>) ReadIntentFilter(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IActivityInfo>) ReadActivityInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IActivityInfo*> >) CreateActivityInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IApplicationInfo>) ReadApplicationInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IConfigurationInfo>) ReadConfigurationInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IConfigurationInfo*> >) CreateConfigurationInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IFeatureInfo>) ReadFeatureInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IFeatureInfo*> >) CreateFeatureInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IFeatureGroupInfo>) ReadFeatureGroupInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IFeatureGroupInfo*> >) CreateFeatureGroupInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IInstrumentationInfo>) ReadInstrumentationInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IInstrumentationInfo*> >) CreateInstrumentationInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPackageInfo>) ReadPackageInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPathPermission>) ReadPathPermission(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IPathPermission*> >) CreatePathPermissionArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPermissionInfo>) ReadPermissionInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IPermissionInfo*> >) CreatePermissionInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IProviderInfo>) ReadProviderInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IProviderInfo*> >) CreateProviderInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IList>) CreateProviderInfoList(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IResolveInfo>) ReadResolveInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IServiceInfo>) ReadServiceInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IServiceInfo*> >) CreateServiceInfoArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ISignature>) ReadSignature(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<ISignature*> >) CreateSignatureArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IColorStateList>) ReadColorStateList(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ICompatibilityInfo>) ReadCompatibilityInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IConfiguration>) ReadConfiguration(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IRect>) ReadRect(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IIVoiceInteractor>) ReadIVoiceInteractor(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IList>) CreateReferrerIntentList(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IInputBindResult>) ReadInputBindResult(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IUri>) ReadUri(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IBinder>) ReadStrongBinder(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IBundle>) ReadBundle(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IParcelFileDescriptor>) ReadParcelFileDescriptor(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPatternMatcher>) ReadPatternMatcher(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr< ArrayOf<IPatternMatcher*> >) CreatePatternMatcherArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IPersistableBundle>) ReadPersistableBundle(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ISparseArray>) ReadSparseArray(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IDisplayInfo>) ReadDisplayInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IInputChannel>) ReadInputChannel(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ISurface>) ReadSurface(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<IWindowSession>) ReadWindowSession(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ISpellCheckerInfo>) ReadSpellCheckerInfo(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(AutoPtr<ISpellCheckerSubtype>) ReadSpellCheckerSubtype(
        /* [in] */ const android::Parcel& source);

    static CARAPI_(void) ReadSpellCheckerSubtypeList(
        /* [in] */ const android::Parcel& source,
        /* [in] */ IList* list);

    // write methods;
    static CARAPI_(void) WriteInt32(
        /* [in] */ android::Parcel& data,
        /* [in] */ Int32 value);

    static CARAPI_(void) WriteInt64(
        /* [in] */ android::Parcel& data,
        /* [in] */ Int64 value);

    static CARAPI_(void) WriteFloat(
        /* [in] */ android::Parcel& data,
        /* [in] */ Float value);

    static CARAPI_(void) WriteString(
        /* [in] */ android::Parcel& data,
        /* [in] */ const String& value);

    static CARAPI_(void) WriteValue(
        /* [in] */ android::Parcel& data,
        /* [in] */ IInterface* value);

    static CARAPI_(void) WriteParcelable(
        /* [in] */ android::Parcel& data,
        /* [in] */ IParcelable* value);

    static CARAPI_(void) Write_CHAR_SEQUENCE(
        /* [in] */ android::Parcel& data,
        /* [in] */ ICharSequence* value);

    static CARAPI_(void) WriteActivityManagerTaskDescription(
        /* [in] */ android::Parcel& data,
        /* [in] */ IActivityManagerTaskDescription* value);

    static CARAPI_(void) WriteIApplicationThread(
        /* [in] */ android::Parcel& data,
        /* [in] */ IApplicationThread* app);

    static CARAPI_(void) WriteContentProviderHolderList(
        /* [in] */ android::Parcel& data,
        /* [in] */ IList* value);

    static CARAPI_(void) WriteConfiguration(
        /* [in] */ android::Parcel& data,
        /* [in] */ IConfiguration* config);

    static CARAPI_(void) WriteIIContentObserver(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIContentObserver* observer);

    static CARAPI_(void) WriteBitmap(
        /* [in] */ android::Parcel& data,
        /* [in] */ IBitmap* value);

    static CARAPI_(void) WriteRect(
        /* [in] */ android::Parcel& data,
        /* [in] */ IRect* value);

    static CARAPI_(void) WriteIIDisplayManagerCallback(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIDisplayManagerCallback* cb);

    static CARAPI_(void) WriteUri(
        /* [in] */ android::Parcel& data,
        /* [in] */ IUri* value);

    static CARAPI_(void) WriteStrongBinder(
        /* [in] */ android::Parcel& data,
        /* [in] */ IBinder* value);

    static CARAPI_(void) WriteBundle(
        /* [in] */ android::Parcel& data,
        /* [in] */ IBundle* value);

    static CARAPI_(void) WritePersistableBundle(
        /* [in] */ android::Parcel& data,
        /* [in] */ IPersistableBundle* value);

    static CARAPI_(void) WriteSparseArray(
        /* [in] */ android::Parcel& data,
        /* [in] */ ISparseArray* value);

    static CARAPI_(void) WriteIIGraphicsStatsCallback(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIGraphicsStatsCallback* cb);

    static CARAPI_(void) WriteIIWindow(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIWindow* window);

    static CARAPI_(void) WriteIIWindowSessionCallback(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIWindowSessionCallback* cb);

    static CARAPI_(void) WriteWindowManagerLayoutParams(
        /* [in] */ android::Parcel& data,
        /* [in] */ IWindowManagerLayoutParams* params);

    static CARAPI_(void) WriteAccessibilityEvent(
        /* [in] */ android::Parcel& data,
        /* [in] */ IAccessibilityEvent* event);

    static CARAPI_(void) WriteIIAccessibilityInteractionConnection(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIAccessibilityInteractionConnection* connection);

    static CARAPI_(void) WriteIIAccessibilityManagerClient(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIAccessibilityManagerClient* client);

    static CARAPI_(void) WriteEditorInfo(
        /* [in] */ android::Parcel& data,
        /* [in] */ IEditorInfo* value);

    static CARAPI_(void) WriteIIInputContext(
        /* [in] */ android::Parcel& data,
        /* [in] */ IIInputContext* inputContext);

    static CARAPI_(void) WriteIInputMethodClient(
        /* [in] */ android::Parcel& data,
        /* [in] */ IInputMethodClient* client);

private:
    static CARAPI_(void) ReadMapInternal(
        /* [in] */ const android::Parcel& source,
        /* [in] */ IHashMap* map,
        /* [in] */ Int32 N);

    static CARAPI_(AutoPtr< ArrayOf<ILocale*> >) ForLanguageTags(
        /* [in] */ const String& languageTags);

    static CARAPI_(void) ReadComponentInfo(
        /* [in] */ const android::Parcel& source,
        /* [in] */ ComponentInfo* info);

    static CARAPI_(void) ReadPackageItemInfo(
        /* [in] */ const android::Parcel& source,
        /* [in] */ PackageItemInfo* info);

    static CARAPI_(void) WriteAbstractPart(
        /* [in] */ android::Parcel& data,
        /* [in] */ AbstractPart* value);

    static CARAPI_(LayoutAlignment) GetLayoutAlignmentFromString(
        /* [in] */ const String& alignmentStr);

    static CARAPI_(void) ReadSpan(
        /* [in] */ const android::Parcel& source,
        /* [in] */ ISpannable* sp,
        /* [in] */ IInterface* o);

    static CARAPI_(void) WriteArrayMapInternal(
        /* [in] */ android::Parcel& data,
        /* [in] */ IArrayMap* value);

    static CARAPI_(void) WriteWhere(
        /* [in] */ android::Parcel& dta,
        /* [in] */ ISpanned* sp,
        /* [in] */ IInterface* o);

private:
    static const String TAG;
    static const Int32 VAL_NULL;
    static const Int32 VAL_STRING;
    static const Int32 VAL_INTEGER;
    static const Int32 VAL_MAP;
    static const Int32 VAL_BUNDLE;
    static const Int32 VAL_PARCELABLE;
    static const Int32 VAL_SHORT;
    static const Int32 VAL_LONG;
    static const Int32 VAL_FLOAT;
    static const Int32 VAL_DOUBLE;
    static const Int32 VAL_BOOLEAN;
    static const Int32 VAL_CHARSEQUENCE;
    static const Int32 VAL_LIST;
    static const Int32 VAL_SPARSEARRAY;
    static const Int32 VAL_BYTEARRAY;
    static const Int32 VAL_STRINGARRAY;
    static const Int32 VAL_IBINDER;
    static const Int32 VAL_PARCELABLEARRAY;
    static const Int32 VAL_OBJECTARRAY;
    static const Int32 VAL_INTARRAY;
    static const Int32 VAL_LONGARRAY;
    static const Int32 VAL_BYTE;
    static const Int32 VAL_SERIALIZABLE;
    static const Int32 VAL_SPARSEBOOLEANARRAY;
    static const Int32 VAL_BOOLEANARRAY;
    static const Int32 VAL_CHARSEQUENCEARRAY;
    static const Int32 VAL_PERSISTABLEBUNDLE;
    static const Int32 VAL_SIZE;
    static const Int32 VAL_SIZEF;
    static const Int32 VAL_DOUBLEARRAY;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ANDROIDPARCELUTILS_H__
