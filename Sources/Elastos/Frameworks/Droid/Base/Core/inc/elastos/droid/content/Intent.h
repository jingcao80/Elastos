
#ifndef __ELASTOS_DROID_CONTENT_INTENT_H__
#define __ELASTOS_DROID_CONTENT_INTENT_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashSet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Content {

class Intent
    : public Object
    , public IIntent
{
public:
    CAR_INTERFACE_DECL()

    Intent();

    virtual ~Intent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIntent* intent);

    CARAPI constructor(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean all);

    CARAPI constructor(
        /* [in] */ const String& action);

    CARAPI constructor(
        /* [in] */ const String& action,
        /* [in] */ IUri* uri);

    CARAPI constructor(
        /* [in] */ IContext* packageContext,
        /* [in] */ const ClassID& clsId);

    CARAPI constructor(
        /* [in] */ const String& action,
        /* [in] */ IUri* uri,
        /* [in] */ IContext* packageContext,
        /* [in] */ IClassInfo* cls);
    /**
     * Convenience function for creating a {@link #ACTION_CHOOSER} Intent.
     *
     * <p>Builds a new {@link #ACTION_CHOOSER} Intent that wraps the given
     * target intent, also optionally supplying a title.  If the target
     * intent has specified {@link #FLAG_GRANT_READ_URI_PERMISSION} or
     * {@link #FLAG_GRANT_WRITE_URI_PERMISSION}, then these flags will also be
     * set in the returned chooser intent, with its ClipData set appropriately:
     * either a direct reflection of {@link #getClipData()} if that is non-null,
     * or a new ClipData build from {@link #getData()}.
     *
     * @param target The Intent that the user will be selecting an activity
     * to perform.
     * @param title Optional title that will be displayed in the chooser.
     * @return Return a new Intent object that you can hand to
     * {@link Context#startActivity(Intent) Context.startActivity()} and
     * related methods.
     */
    static CARAPI_(AutoPtr<IIntent>) CreateChooser(
        /* [in] */ IIntent* target,
        /* [in] */ ICharSequence* title);

    CARAPI Clone(
        /* [out] */ IInterface** intent);

    CARAPI CloneImpl(
        /* [in] */ IIntent* intent);

    CARAPI CloneFilter(
        /* [out] */ IIntent** intent);

    /**
     * Create an intent to launch the main (root) activity of a task.  This
     * is the Intent that is started when the application's is launched from
     * Home.  For anything else that wants to launch an application in the
     * same way, it is important that they use an Intent structured the same
     * way, and can use this function to ensure this is the case.
     *
     * <p>The returned Intent has the given Activity component as its explicit
     * component, {@link #ACTION_MAIN} as its action, and includes the
     * category {@link #CATEGORY_LAUNCHER}.  This does <em>not</em> have
     * {@link #FLAG_ACTIVITY_NEW_TASK} set, though typically you will want
     * to do that through {@link #addFlags(int)} on the returned Intent.
     *
     * @param mainActivity The main activity component that this Intent will
     * launch.
     * @return Returns a newly created Intent that can be used to launch the
     * activity as a main application entry.
     *
     * @see #setClass
     * @see #setComponent
     */
    static CARAPI_(AutoPtr<IIntent>) MakeMainActivity(
        /* [in] */ IComponentName* mainActivity);

    /**
     * Make an Intent for the main activity of an application, without
     * specifying a specific activity to run but giving a selector to find
     * the activity.  This results in a final Intent that is structured
     * the same as when the application is launched from
     * Home.  For anything else that wants to launch an application in the
     * same way, it is important that they use an Intent structured the same
     * way, and can use this function to ensure this is the case.
     *
     * <p>The returned Intent has {@link #ACTION_MAIN} as its action, and includes the
     * category {@link #CATEGORY_LAUNCHER}.  This does <em>not</em> have
     * {@link #FLAG_ACTIVITY_NEW_TASK} set, though typically you will want
     * to do that through {@link #addFlags(int)} on the returned Intent.
     *
     * @param selectorAction The action name of the Intent's selector.
     * @param selectorCategory The name of a category to add to the Intent's
     * selector.
     * @return Returns a newly created Intent that can be used to launch the
     * activity as a main application entry.
     *
     * @see #setSelector(Intent)
     */
    static CARAPI_(AutoPtr<IIntent>) MakeMainSelectorActivity(
        /* [in] */ const String& selectorAction,
        /* [in] */ const String& selectorCategory);

    /**
     * Make an Intent that can be used to re-launch an application's task
     * in its base state.  This is like {@link #makeMainActivity(ComponentName)},
     * but also sets the flags {@link #FLAG_ACTIVITY_NEW_TASK} and
     * {@link #FLAG_ACTIVITY_CLEAR_TASK}.
     *
     * @param mainActivity The activity component that is the root of the
     * task; this is the activity that has been published in the application's
     * manifest as the main launcher icon.
     *
     * @return Returns a newly created Intent that can be used to relaunch the
     * activity's task in its root state.
     */
    static CARAPI_(AutoPtr<IIntent>) MakeRestartActivityTask(
        /* [in] */ IComponentName* mainActivity);

    /**
     * Call {@link #parseUri} with 0 flags.
     * @deprecated Use {@link #parseUri} instead.
     */
    static CARAPI GetIntent(
        /* [in] */ const String& uri,
        /* [out] */ IIntent** intent);

    /**
     * Create an intent from a URI.  This URI may encode the action,
     * category, and other intent fields, if it was returned by
     * {@link #toUri}.  If the Intent was not generate by toUri(), its data
     * will be the entire URI and its action will be ACTION_VIEW.
     *
     * <p>The URI given here must not be relative -- that is, it must include
     * the scheme and full path.
     *
     * @param uri The URI to turn into an Intent.
     * @param flags Additional processing flags.  Either 0 or
     * {@link #URI_INTENT_SCHEME}.
     *
     * @return Intent The newly created Intent object.
     *
     * @throws URISyntaxException Throws URISyntaxError if the basic URI syntax
     * it bad (as parsed by the Uri class) or the Intent data within the
     * URI is invalid.
     *
     * @see #toUri
     */
    static CARAPI ParseUri(
        /* [in] */ const String& uri,
        /* [in] */ Int32 flags,
        /* [out] */ IIntent** intent);

    static CARAPI GetIntentOld(
        /* [in] */ const String& uri,
        /* [out] */ IIntent** intent);

    CARAPI GetAction(
        /* [out] */ String* action);

    CARAPI GetData(
        /* [out] */ IUri** data);

    CARAPI GetDataString(
        /* [out] */ String* dataString);

    CARAPI GetScheme(
        /* [out] */ String* scheme);

    CARAPI GetType(
        /* [out] */ String* type);

    CARAPI ResolveType(
        /* [in] */ IContext* context,
        /* [out] */ String* type);

    CARAPI ResolveType(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ String* type);

    CARAPI ResolveTypeIfNeeded(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ String* type);

    CARAPI HasCategory(
        /* [in] */ const String& category,
        /* [out] */ Boolean* hasCategory);

    CARAPI GetCategories(
        /* [out, callee] */ ArrayOf<String>** categories);

    CARAPI GetSelector(
        /* [out] */ IIntent** intent);

    CARAPI GetClipData(
        /* [out] */ IClipData** clipData);

    CARAPI GetContentUserHint(
        /* [out] */ Int32* hint);

    CARAPI SetExtrasClassLoader(
        /* [in] */ IClassLoader* loader);

    CARAPI HasExtra(
        /* [in] */ const String& name,
        /* [out] */ Boolean* hasExtra);

    CARAPI HasFileDescriptors(
        /* [out] */ Boolean* hasFileDescriptors);

    CARAPI SetAllowFds(
        /* [in] */ Boolean allowFds);

    CARAPI GetExtra(
        /* [in] */ const String& name,
        /* [out] */ IInterface** obj);

    CARAPI GetBooleanExtra(
        /* [in] */ const String& name,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* value);

    CARAPI GetByteExtra(
        /* [in] */ const String& name,
        /* [in] */ Byte defaultValue,
        /* [out] */ Byte* value);

    CARAPI GetInt16Extra(
        /* [in] */ const String& name,
        /* [in] */ Int16 defaultValue,
        /* [out] */ Int16* value);

    CARAPI GetCharExtra(
        /* [in] */ const String& name,
        /* [in] */ Char32 defaultValue,
        /* [out] */ Char32* value);

    CARAPI GetInt32Extra(
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* value);

    CARAPI GetInt64Extra(
        /* [in] */ const String& name,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* value);

    CARAPI GetFloatExtra(
        /* [in] */ const String& name,
        /* [in] */ Float defaultValue,
        /* [out] */ Float* value);

    CARAPI GetDoubleExtra(
        /* [in] */ const String& name,
        /* [in] */ Double defaultValue,
        /* [out] */ Double* value);

    CARAPI GetStringExtra(
        /* [in] */ const String& name,
        /* [out] */ String* value);

    CARAPI GetCharSequenceExtra(
        /* [in] */ const String& name,
        /* [out] */ ICharSequence** value);

    CARAPI GetParcelableExtra(
        /* [in] */ const String& name,
        /* [out] */ IParcelable** value);

    CARAPI GetParcelableArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<IParcelable*>** value);

    CARAPI GetParcelableArrayListExtra(
        /* [in] */ const String& name,
        /* [out] */ IArrayList** value);

    CARAPI GetSerializableExtra(
        /* [in] */ const String& name,
        /* [out] */ ISerializable** value);

    CARAPI GetInteger32ArrayListExtra(
        /* [in] */ const String& name,
        /* [out] */ IArrayList** value);

    CARAPI GetStringArrayListExtra(
        /* [in] */ const String& name,
        /* [out] */ IArrayList** value);

    CARAPI GetCharSequenceArrayListExtra(
        /* [in] */ const String& name,
        /* [out] */ IArrayList** value);

    CARAPI GetBooleanArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Boolean>** value);

    CARAPI GetByteArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Byte>** value);

    CARAPI GetInt16ArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Int16>** value);

    CARAPI GetCharArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Char32>** value);

    CARAPI GetInt32ArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Int32>** value);

    CARAPI GetInt64ArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Int64>** value);

    CARAPI GetFloatArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Float>** value);

    CARAPI GetDoubleArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Double>** value);

    CARAPI GetStringArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<String>** value);

    CARAPI GetCharSequenceArrayExtra(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<ICharSequence*>** value);

    CARAPI GetBundleExtra(
        /* [in] */ const String& name,
        /* [out] */ IBundle** value);

    CARAPI GetExtra(
        /* [in] */ const String& name,
        /* [in] */ IInterface* defaultValue,
        /* [out] */ IInterface** obj);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI IsExcludingStopped(
        /* [out] */ Boolean* isStopped);

    CARAPI GetPackage(
        /* [out] */ String* packageName);

    CARAPI GetComponent(
        /* [out] */ IComponentName** componentName);

    CARAPI GetSourceBounds(
        /* [out] */ IRect** sourceBounds);

    CARAPI ResolveActivity(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IComponentName** componentName);

    CARAPI ResolveActivityInfo(
        /* [in] */ IPackageManager* pm,
        /* [in] */ Int32 flags,
        /* [out] */ IActivityInfo** result);

    /**
     * Special function for use by the system to resolve service
     * intents to system apps.  Throws an exception if there are
     * multiple potential matches to the Intent.  Returns null if
     * there are no matches.
     * @hide
     */
    CARAPI ResolveSystemService(
        /* [in] */ IPackageManager* pm,
        /* [in] */ Int32 flags,
        /* [out] */ IComponentName** componentName);

    CARAPI SetAction(
        /* [in] */ const String& action);

    CARAPI SetData(
        /* [in] */ IUri* data);

    CARAPI SetDataAndNormalize(
        /* [in] */ IUri* data);

    CARAPI SetType(
        /* [in] */ const String& type);

    CARAPI SetTypeAndNormalize(
        /* [in] */ const String& type);

    CARAPI SetDataAndType(
        /* [in] */ IUri* data,
        /* [in] */ const String& type);

    CARAPI SetDataAndTypeAndNormalize(
        /* [in] */ IUri* data,
        /* [in] */ const String& type);

    CARAPI AddCategory(
        /* [in] */ const String& category);

    CARAPI RemoveCategory(
        /* [in] */ const String& category);

    CARAPI SetSelector(
        /* [in] */ IIntent* selector);

    CARAPI SetClipData(
        /* [in] */ IClipData* clip);

    /**
     * This is NOT a secure mechanism to identify the user who sent the intent.
     * When the intent is sent to a different user, it is used to fix uris by adding the userId
     * who sent the intent.
     * @hide
     */
    CARAPI SetContentUserHint(
        /* [in] */ Int32 hint);

    CARAPI PutBooleanExtra(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI PutByteExtra(
        /* [in] */ const String& name,
        /* [in] */ Byte value);

    CARAPI PutCharExtra(
        /* [in] */ const String& name,
        /* [in] */ Char32 value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ Int16 value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ Int64 value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ Float value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ Double value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ICharSequence* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ IParcelable* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<IParcelable*>* value);

    CARAPI PutParcelableArrayListExtra(
        /* [in] */ const String& name,
        /* [in] */ IArrayList* value);

    CARAPI PutInteger32ArrayListExtra(
        /* [in] */ const String& name,
        /* [in] */ IArrayList* value);

    CARAPI PutStringArrayListExtra(
        /* [in] */ const String& name,
        /* [in] */ IArrayList* value);

    CARAPI PutCharSequenceArrayListExtra(
        /* [in] */ const String& name,
        /* [in] */ IArrayList* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ISerializable* value);

    CARAPI PutBooleanArrayExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Boolean>* value);

    CARAPI PutByteArrayExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Int16>* value);

    CARAPI PutCharArrayExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Char32>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Int32>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Int64>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Float>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Double>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<ICharSequence*>* value);

    CARAPI PutExtra(
        /* [in] */ const String& name,
        /* [in] */ IBundle* value);

    CARAPI PutExtras(
        /* [in] */ IIntent* src);

    CARAPI PutExtras(
        /* [in] */ IBundle* extras);

    CARAPI ReplaceExtras(
        /* [in] */ IIntent* src);

    CARAPI ReplaceExtras(
        /* [in] */ IBundle* extras);

    CARAPI RemoveExtra(
        /* [in] */ const String& name);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI AddFlags(
        /* [in] */ Int32 flags);

    CARAPI SetPackage(
        /* [in] */ const String& packageName);

    CARAPI SetComponent(
        /* [in] */ IComponentName* component);

    CARAPI SetClassName(
        /* [in] */ IContext* packageContext,
        /* [in] */ const String& className);

    CARAPI SetClassName(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);

    CARAPI SetClass(
        /* [in] */ IContext* packageContext,
        /* [in] */ const ClassID& cls);

    CARAPI SetSourceBounds(
        /* [in] */ IRect* r);

    CARAPI FillIn(
        /* [in] */ IIntent* other,
        /* [in] */ Int32 flags,
        /* [out] */ Int32* result);

    CARAPI FilterEquals(
        /* [in] */ IIntent* other,
        /* [out] */ Boolean* isEqual);

    CARAPI FilterHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ToInsecureString(
        /* [out] */ String* str);

    CARAPI ToInsecureStringWithClip(
        /* [out] */ String* str);

    CARAPI ToShortString(
        /* [in] */ Boolean secure,
        /* [in] */ Boolean comp,
        /* [in] */ Boolean extras,
        /* [in] */ Boolean clip,
        /* [out] */ String* str);

    CARAPI ToShortString(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Boolean secure,
        /* [in] */ Boolean comp,
        /* [in] */ Boolean extras,
        /* [in] */ Boolean clip);

    CARAPI ToURI(
        /* [out] */ String* str);

    CARAPI ToUri(
        /* [in] */ Int32 flags,
        /* [out] */ String* str);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    static CARAPI ParseIntent(
        /* [in] */ IResources* resources,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IIntent** intent);

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* out);

    static AutoPtr<IIntent> RestoreFromXml(
        /* [in] */ IXmlPullParser* in);

    static CARAPI_(String) NormalizeMimeType(
        /* [in] */ const String& type);

    /**
     * Prepare this {@link Intent} to leave an app process.
     *
     * @hide
     */
    CARAPI PrepareToLeaveProcess();

    /**
     * @hide
     */
    CARAPI PrepareToEnterProcess();

    /**
     * @hide
     */
    CARAPI FixUris(
        /* [in] */ Int32 contentUserHint);

    CARAPI MigrateExtraStreamToClipData(
        /* [out] */ Boolean* result);

    CARAPI_(void) ToUriInner(
        /* [in] */ StringBuilder& uri,
        /* [in] */ const String& scheme,
        /* [in] */ Int32 flags);

    CARAPI IsDocument(
        /* [out] */ Boolean* result);
private:
    static CARAPI_(AutoPtr<IClipDataItem>) MakeClipItem(
        /* [in] */ ArrayOf<IUri*>* streams,
        /* [in] */ ArrayOf<ICharSequence*>* texts,
        /* [in] */ ArrayOf<String>* htmlTexts,
        /* [in] */ Int32 which);

    CARAPI_(AutoPtr<IClassInfo>) TransformClassInfo(
        /* [in] */ const ClassID& objId);

private:
    static const String ATTR_ACTION;// = "action";
    static const String TAG_CATEGORIES;// = "categories";
    static const String ATTR_CATEGORY;// = "category";
    static const String TAG_EXTRA;// = "extra";
    static const String ATTR_TYPE;// = "type";
    static const String ATTR_COMPONENT;// = "component";
    static const String ATTR_DATA;// = "data";
    static const String ATTR_FLAGS;// = "flags";

private:
    static const String TAG;

    String mAction;
    AutoPtr<IUri> mData;
    String mType;
    String mPackage;
    AutoPtr<IComponentName> mComponent;
    Int32 mFlags;
    AutoPtr<HashSet<String> > mCategories;//ArraySet
    AutoPtr<IBundle> mExtras;
    AutoPtr<IRect> mSourceBounds;
    AutoPtr<IIntent> mSelector;
    AutoPtr<IClipData> mClipData;
    Int32 mContentUserHint;
};

} // Content
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTENT_INTENT_H__
