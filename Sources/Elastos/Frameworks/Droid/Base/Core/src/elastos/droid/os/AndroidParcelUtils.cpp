
#include "elastos/droid/os/AndroidParcelUtils.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/app/CApplicationThread.h"
#include "elastos/droid/app/CContentProviderHolder.h"
#include "elastos/droid/app/CResultInfo.h"
#include "elastos/droid/app/IApplicationThread.h"
#include "elastos/droid/app/IInstrumentationWatcher.h"
#include "elastos/droid/app/IIUiAutomationConnection.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/IContentProvider.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/content/pm/CPackageInfo.h"
#include "elastos/droid/content/pm/CProviderInfo.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/database/CContentObserverTransport.h"
#include "elastos/droid/database/IContentObserver.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/hardware/display/IDisplayManagerCallback.h"
#include "elastos/droid/hardware/display/CDisplayManagerCallback.h"
#include "elastos/droid/hardware/input/CInputDeviceIdentifier.h"
#include "elastos/droid/hardware/input/CInputManagerInputDevicesChangedListener.h"
#include "elastos/droid/hardware/input/IInputDevicesChangedListener.h"
#include "elastos/droid/internal/app/IVoiceInteractor.h"
#include "elastos/droid/internal/view/CInputBindResult.h"
#include "elastos/droid/internal/view/IInputContextCallback.h"
#include "elastos/droid/internal/view/IInputConnectionWrapper.h"
#include "elastos/droid/internal/view/IInputMethodSession.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/os/NativeBinder.h"
#include "elastos/droid/text/CAnnotation.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/style/CAlignmentSpanStandard.h"
#include "elastos/droid/text/style/CForegroundColorSpan.h"
#include "elastos/droid/text/style/CRelativeSizeSpan.h"
#include "elastos/droid/text/style/CScaleXSpan.h"
#include "elastos/droid/text/style/CStrikethroughSpan.h"
#include "elastos/droid/text/style/CUnderlineSpan.h"
#include "elastos/droid/text/style/CStyleSpan.h"
#include "elastos/droid/text/style/CBulletSpan.h"
#include "elastos/droid/text/style/CQuoteSpan.h"
#include "elastos/droid/text/style/CLeadingMarginSpanStandard.h"
#include "elastos/droid/text/style/CURLSpan.h"
#include "elastos/droid/text/style/CBackgroundColorSpan.h"
#include "elastos/droid/text/style/CTypefaceSpan.h"
#include "elastos/droid/text/style/CSuperscriptSpan.h"
#include "elastos/droid/text/style/CSubscriptSpan.h"
#include "elastos/droid/text/style/CAbsoluteSizeSpan.h"
#include "elastos/droid/text/style/CTextAppearanceSpan.h"
#include "elastos/droid/text/style/CSuggestionSpan.h"
#include "elastos/droid/text/style/CSpellCheckSpan.h"
#include "elastos/droid/text/style/CSuggestionRangeSpan.h"
#include "elastos/droid/text/style/CEasyEditSpan.h"
#include "elastos/droid/text/style/CLocaleSpan.h"
#include "elastos/droid/text/style/CTtsSpan.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/CAbsSavedStateHelper.h"
#include "elastos/droid/view/CAccessibilityInteractionConnection.h"
#include "elastos/droid/view/CDisplayInfo.h"
#include "elastos/droid/view/CInputChannel.h"
#include "elastos/droid/view/CInputDevice.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/CSurface.h"
#include "elastos/droid/view/CViewRootImplW.h"
#include "elastos/droid/view/CWindowManagerGlobalSessionCallback.h"
#include "elastos/droid/view/IWindow.h"
#include "elastos/droid/view/IWindowSession.h"
#include "elastos/droid/view/IWindowSessionCallback.h"
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerClient.h"
#include "elastos/droid/view/accessibility/IAccessibilityInteractionConnection.h"
#include "elastos/droid/view/accessibility/IAccessibilityManagerClient.h"
#include "elastos/droid/view/inputmethod/CCursorAnchorInfo.h"
#include "elastos/droid/view/inputmethod/CExtractedTextRequest.h"
#include "elastos/droid/view/inputmethod/CIInputMethodClient.h"
#include "elastos/droid/view/inputmethod/CSparseRectFArray.h"
#include "elastos/droid/view/inputmethod/IInputContext.h"
#include "elastos/droid/view/inputmethod/IInputMethodClient.h"
#include "elastos/droid/view/textservice/CSpellCheckerInfo.h"
#include "elastos/droid/view/textservice/CSpellCheckerSubtype.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityOptions;
using Elastos::Droid::App::CApplicationThread;
using Elastos::Droid::App::CContentProviderHolder;
using Elastos::Droid::App::CResultInfo;
using Elastos::Droid::App::IApplicationThreadStub;
using Elastos::Droid::App::IInstrumentationWatcherProxy;
using Elastos::Droid::App::IIUiAutomationConnectionProxy;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentProviderProxy2;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::CPackageInfo;
using Elastos::Droid::Content::Pm::CProviderInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::CCompatibilityInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Database::CContentObserverTransport;
using Elastos::Droid::Database::IContentObserverStub;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Hardware::Display::CDisplayManagerCallback;
using Elastos::Droid::Hardware::Display::IDisplayManagerCallbackStub;
using Elastos::Droid::Hardware::Input::CInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::CInputManagerInputDevicesChangedListener;
using Elastos::Droid::Hardware::Input::IInputDevicesChangedListenerStub;
using Elastos::Droid::Internal::App::IVoiceInteractorProxy;
using Elastos::Droid::Internal::View::CInputBindResult;
using Elastos::Droid::Internal::View::IInputContextCallbackProxy;
using Elastos::Droid::Internal::View::IInputConnectionWrapper;
using Elastos::Droid::Internal::View::IInputMethodSessionProxy;
using Elastos::Droid::Net::HierarchicalUri;
using Elastos::Droid::Net::IHierarchicalUri;
using Elastos::Droid::Net::IOpaqueUri;
using Elastos::Droid::Net::IStringUri;
using Elastos::Droid::Text::ALIGN_NONE;
using Elastos::Droid::Text::ALIGN_NORMAL;
using Elastos::Droid::Text::ALIGN_OPPOSITE;
using Elastos::Droid::Text::ALIGN_CENTER;
using Elastos::Droid::Text::ALIGN_LEFT;
using Elastos::Droid::Text::ALIGN_RIGHT;
using Elastos::Droid::Text::CAnnotation;
using Elastos::Droid::Text::IAnnotation;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Style::IAlignmentSpan;
using Elastos::Droid::Text::Style::CAlignmentSpanStandard;
using Elastos::Droid::Text::Style::IAlignmentSpanStandard;
using Elastos::Droid::Text::Style::CForegroundColorSpan;
using Elastos::Droid::Text::Style::IForegroundColorSpan;
using Elastos::Droid::Text::Style::CRelativeSizeSpan;
using Elastos::Droid::Text::Style::IRelativeSizeSpan;
using Elastos::Droid::Text::Style::CScaleXSpan;
using Elastos::Droid::Text::Style::IScaleXSpan;
using Elastos::Droid::Text::Style::CStrikethroughSpan;
using Elastos::Droid::Text::Style::IStrikethroughSpan;
using Elastos::Droid::Text::Style::CUnderlineSpan;
using Elastos::Droid::Text::Style::IUnderlineSpan;
using Elastos::Droid::Text::Style::CStyleSpan;
using Elastos::Droid::Text::Style::IStyleSpan;
using Elastos::Droid::Text::Style::CBulletSpan;
using Elastos::Droid::Text::Style::IBulletSpan;
using Elastos::Droid::Text::Style::CQuoteSpan;
using Elastos::Droid::Text::Style::IQuoteSpan;
using Elastos::Droid::Text::Style::CLeadingMarginSpanStandard;
using Elastos::Droid::Text::Style::ILeadingMarginSpan;
using Elastos::Droid::Text::Style::CURLSpan;
using Elastos::Droid::Text::Style::IURLSpan;
using Elastos::Droid::Text::Style::CBackgroundColorSpan;
using Elastos::Droid::Text::Style::IBackgroundColorSpan;
using Elastos::Droid::Text::Style::CTypefaceSpan;
using Elastos::Droid::Text::Style::ITypefaceSpan;
using Elastos::Droid::Text::Style::CSuperscriptSpan;
using Elastos::Droid::Text::Style::ISuperscriptSpan;
using Elastos::Droid::Text::Style::CSubscriptSpan;
using Elastos::Droid::Text::Style::ISubscriptSpan;
using Elastos::Droid::Text::Style::CAbsoluteSizeSpan;
using Elastos::Droid::Text::Style::IAbsoluteSizeSpan;
using Elastos::Droid::Text::Style::CTextAppearanceSpan;
using Elastos::Droid::Text::Style::ITextAppearanceSpan;
using Elastos::Droid::Text::Style::CSuggestionSpan;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Text::Style::CSpellCheckSpan;
using Elastos::Droid::Text::Style::ISpellCheckSpan;
using Elastos::Droid::Text::Style::CSuggestionRangeSpan;
using Elastos::Droid::Text::Style::ISuggestionRangeSpan;
using Elastos::Droid::Text::Style::CEasyEditSpan;
using Elastos::Droid::Text::Style::IEasyEditSpan;
using Elastos::Droid::Text::Style::CLocaleSpan;
using Elastos::Droid::Text::Style::ILocaleSpan;
using Elastos::Droid::Text::Style::CTtsSpan;
using Elastos::Droid::Text::Style::ITtsSpan;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::CAbsSavedStateHelper;
using Elastos::Droid::View::CAccessibilityInteractionConnection;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::View::CInputChannel;
using Elastos::Droid::View::CInputDevice;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CViewRootImplW;
using Elastos::Droid::View::CWindowManagerGlobalSessionCallback;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::IAbsSavedStateHelper;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindowStub;
using Elastos::Droid::View::IWindowSessionProxy;
using Elastos::Droid::View::IWindowSessionCallbackStub;
using Elastos::Droid::View::ThreadedRenderer;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerClient;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnectionStub;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerClientStub;
using Elastos::Droid::View::InputMethod::CCursorAnchorInfo;
using Elastos::Droid::View::InputMethod::CExtractedTextRequest;
using Elastos::Droid::View::InputMethod::CIInputMethodClient;
using Elastos::Droid::View::InputMethod::CSparseRectFArray;
using Elastos::Droid::View::InputMethod::IInputContextStub;
using Elastos::Droid::View::InputMethod::IInputMethodClientStub;
using Elastos::Droid::View::TextService::CSpellCheckerInfo;
using Elastos::Droid::View::TextService::CSpellCheckerSubtype;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::IO::CFileDescriptor;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

class AndroidParcelWrapper
    : public Object
    , public IAndroidParcelWrapper
    , public IParcel
{
public:
    CAR_INTERFACE_DECL();

    AndroidParcelWrapper(
        /* [in] */ android::Parcel* data,
        /* [in] */ Boolean releaseAndroidParcel)
        : mData(data)
        , mReleaseAndroidParcel(releaseAndroidParcel)
    {}

    ~AndroidParcelWrapper()
    {
        if (mReleaseAndroidParcel && mData != NULL) {
            delete mData;
        }
    }

    CARAPI Marshall(
        /* [out, callee] */ ArrayOf<Byte>** bytes)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI Unmarshall(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI AppendFrom(
        /* [in] */ IParcel* parcel,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI HasFileDescriptors(
        /* [out] */ Boolean* result)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadByte(
        /* [out] */ Byte* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteByte(
        /* [in] */ Byte value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadBoolean(
        /* [out] */ Boolean* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteBoolean(
        /* [in] */ Boolean value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadChar(
        /* [out] */ Char32* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteChar(
        /* [in] */ Char32 value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadInt16(
        /* [out] */ Int16* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteInt16(
        /* [in] */ Int16 value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadInt32(
        /* [out] */ Int32* value)
    {
        VALIDATE_NOT_NULL(value);
        *value = mData->readInt32();
        return NOERROR;
    }

    CARAPI WriteInt32(
        /* [in] */ Int32 value)
    {
        AndroidParcelUtils::WriteInt32(*mData, value);
        return NOERROR;
    }

    CARAPI ReadInt64(
        /* [out] */ Int64* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteInt64(
        /* [in] */ Int64 value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadFloat(
        /* [out] */ Float* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteFloat(
        /* [in] */ Float value)
    {
        AndroidParcelUtils::WriteFloat(*mData, value);
        return NOERROR;
    }

    CARAPI ReadDouble(
        /* [out] */ Double* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteDouble(
        /* [in] */ Double value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadString(
        /* [out] */ String* str)
    {
        VALIDATE_NOT_NULL(str);
        *str = AndroidParcelUtils::ReadString(*mData);
        return NOERROR;
    }

    CARAPI WriteString(
        /* [in] */ const String& str)
    {
        AndroidParcelUtils::WriteString(*mData, str);
        return NOERROR;
    }

    CARAPI ReadStruct(
        /* [out] */ Handle32* address)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteStruct(
        /* [in] */ Handle32 value,
        /* [in] */ Int32 size)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadEMuid(
        /* [out] */ EMuid* id)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteEMuid(
        /* [in] */ const EMuid& id)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadEGuid(
        /* [out] */ EGuid* id)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteEGuid(
        /* [in] */ const EGuid& id)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadArrayOf(
        /* [out] */ Handle32* array)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteArrayOf(
        /* [in] */ Handle32 array)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadArrayOfString(
        /* [out, callee] */ ArrayOf<String>** array)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteArrayOfString(
        /* [in] */ ArrayOf<String>* array)
    {
        AndroidParcelUtils::WriteStringArray(*mData, array);
        return NOERROR;
    }

    CARAPI ReadInterfacePtr(
        /* [out] */ Handle32* itfPtr)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI WriteInterfacePtr(
        /* [in] */ IInterface* value)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    // Place a file descriptor into the parcel.  The given fd must remain
    // valid for the lifetime of the parcel.
    CARAPI WriteFileDescriptor(
        /* [in] */ Int32 fd)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    // Place a file descriptor into the parcel.  A dup of the fd is made, which
    // will be closed once the parcel is destroyed.
    CARAPI WriteDupFileDescriptor(
        /* [in] */ Int32 fd)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    // Retrieve a file descriptor from the parcel.  This returns the raw fd
    // in the parcel, which you do not own -- use dup() to get your own copy.
    CARAPI ReadFileDescriptor(
        /* [out] */ Int32* fd)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI ReadValue(
        /* [out] */ IInterface** value)
    {
        VALIDATE_NOT_NULL(value);
        AutoPtr<IInterface> obj = AndroidParcelUtils::ReadValue(*mData);
        *value = obj;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }

    CARAPI Clone(
        /* [in] */ IParcel* srcParcel)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI GetDataPosition(
        /* [out] */ Int32* position)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI SetDataPosition(
        /* [in] */ Int32 position)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI GetDataSize(
        /* [out] */ Int32* size)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI SetDataSize(
        /* [in] */ Int32 size)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI GetDataCapacity(
        /* [out] */ Int32* capacity)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI SetDataCapacity(
        /* [in] */ Int32 capacity)
    {
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CARAPI GetDataPayload(
        /* [out] */ Handle32* buffer)
    {
        *buffer = (Handle32)mData;
        return NOERROR;
    }

private:
    android::Parcel* mData;
    Boolean mReleaseAndroidParcel;
};

CAR_INTERFACE_IMPL_2(AndroidParcelWrapper, Object, IAndroidParcelWrapper, IParcel);


//==============================================================
// AndroidParcelUtils
//==============================================================

const String AndroidParcelUtils::TAG("AndroidParcelUtils");
const Int32 AndroidParcelUtils::VAL_NULL = -1;
const Int32 AndroidParcelUtils::VAL_STRING = 0;
const Int32 AndroidParcelUtils::VAL_INTEGER = 1;
const Int32 AndroidParcelUtils::VAL_MAP = 2;
const Int32 AndroidParcelUtils::VAL_BUNDLE = 3;
const Int32 AndroidParcelUtils::VAL_PARCELABLE = 4;
const Int32 AndroidParcelUtils::VAL_SHORT = 5;
const Int32 AndroidParcelUtils::VAL_LONG = 6;
const Int32 AndroidParcelUtils::VAL_FLOAT = 7;
const Int32 AndroidParcelUtils::VAL_DOUBLE = 8;
const Int32 AndroidParcelUtils::VAL_BOOLEAN = 9;
const Int32 AndroidParcelUtils::VAL_CHARSEQUENCE = 10;
const Int32 AndroidParcelUtils::VAL_LIST  = 11;
const Int32 AndroidParcelUtils::VAL_SPARSEARRAY = 12;
const Int32 AndroidParcelUtils::VAL_BYTEARRAY = 13;
const Int32 AndroidParcelUtils::VAL_STRINGARRAY = 14;
const Int32 AndroidParcelUtils::VAL_IBINDER = 15;
const Int32 AndroidParcelUtils::VAL_PARCELABLEARRAY = 16;
const Int32 AndroidParcelUtils::VAL_OBJECTARRAY = 17;
const Int32 AndroidParcelUtils::VAL_INTARRAY = 18;
const Int32 AndroidParcelUtils::VAL_LONGARRAY = 19;
const Int32 AndroidParcelUtils::VAL_BYTE = 20;
const Int32 AndroidParcelUtils::VAL_SERIALIZABLE = 21;
const Int32 AndroidParcelUtils::VAL_SPARSEBOOLEANARRAY = 22;
const Int32 AndroidParcelUtils::VAL_BOOLEANARRAY = 23;
const Int32 AndroidParcelUtils::VAL_CHARSEQUENCEARRAY = 24;
const Int32 AndroidParcelUtils::VAL_PERSISTABLEBUNDLE = 25;
const Int32 AndroidParcelUtils::VAL_SIZE = 26;
const Int32 AndroidParcelUtils::VAL_SIZEF = 27;
const Int32 AndroidParcelUtils::VAL_DOUBLEARRAY = 28;

Byte AndroidParcelUtils::ReadByte(
    /* [in] */ const android::Parcel& source)
{
    return (Byte)(ReadInt32(source) & 0xff);
}

Int32 AndroidParcelUtils::ReadInt32(
    /* [in] */ const android::Parcel& source)
{
    return source.readInt32();
}

AutoPtr< ArrayOf<Int32> > AndroidParcelUtils::CreateInt32Array(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N >= 0 && N <= (source.dataAvail() >> 2)) {
        AutoPtr< ArrayOf<Int32> > val = ArrayOf<Int32>::Alloc(N);
        for (Int32 i = 0; i < N; i++) {
            (*val)[i] = ReadInt32(source);
        }
        return val;
    }
    else {
        return NULL;
    }
}

Int64 AndroidParcelUtils::ReadInt64(
    /* [in] */ const android::Parcel& source)
{
    return source.readInt64();
}

Float AndroidParcelUtils::ReadFloat(
    /* [in] */ const android::Parcel& source)
{
    return source.readFloat();
}

String AndroidParcelUtils::ReadString(
    /* [in] */ const android::Parcel& source)
{
    size_t len;
    const char16_t* str = source.readString16Inplace(&len);
    if (str == NULL) return String(NULL);

    String strData(android::String8(str).string());
    return strData;
}

AutoPtr< ArrayOf<String> > AndroidParcelUtils::CreateStringArray(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr< ArrayOf<String> > array;

    Int32 length = ReadInt32(source);
    if (length > 0) {
        array = ArrayOf<String>::Alloc(length);

        for (Int32 i = 0; i < length; i++) {
            (*array)[i] = ReadString(source);
        }
    }

    return array;
}

AutoPtr<IInterface> AndroidParcelUtils::ReadValue(
    /* [in] */ const android::Parcel& source)
{
    Int32 type = ReadInt32(source);

    switch(type) {
    case VAL_NULL:
        return NULL;

    case VAL_STRING: {
        String value = ReadString(source);
        AutoPtr<ICharSequence> strObj;
        CString::New(value, (ICharSequence**)&strObj);
        return strObj;
    }

    case VAL_INTEGER: {
        Int32 value = ReadInt32(source);
        AutoPtr<IInteger32> i32Obj;
        CInteger32::New(value, (IInteger32**)&i32Obj);
        return i32Obj;
    }

    case VAL_MAP:
        assert(0);
        return NULL;

    case VAL_PARCELABLE:
        return ReadParcelable(source);

    case VAL_SHORT:
        assert(0);
        return NULL;

    case VAL_LONG:
        assert(0);
        return NULL;

    case VAL_FLOAT:
        assert(0);
        return NULL;

    case VAL_DOUBLE:
        assert(0);
        return NULL;

    case VAL_BOOLEAN:
        assert(0);
        return NULL;

    case VAL_CHARSEQUENCE:
        assert(0);
        return NULL;

    case VAL_LIST:
        assert(0);
        return NULL;

    case VAL_BOOLEANARRAY:
        assert(0);
        return NULL;

    case VAL_BYTEARRAY:
        assert(0);
        return NULL;

    case VAL_STRINGARRAY:
        assert(0);
        return NULL;

    case VAL_CHARSEQUENCEARRAY:
        assert(0);
        return NULL;

    case VAL_IBINDER:
        return ReadStrongBinder(source);

    case VAL_OBJECTARRAY:
        assert(0);
        return NULL;

    case VAL_INTARRAY:
        assert(0);
        return NULL;

    case VAL_LONGARRAY:
        assert(0);
        return NULL;

    case VAL_BYTE:
        assert(0);
        return NULL;

    case VAL_SERIALIZABLE:
        assert(0);
        return NULL;

    case VAL_PARCELABLEARRAY:
        assert(0);
        return NULL;

    case VAL_SPARSEARRAY: {
        AutoPtr<ISparseArray> sa = AndroidParcelUtils::ReadSparseArray(source);
        return sa;
    }

    case VAL_SPARSEBOOLEANARRAY:
        assert(0);
        return NULL;

    case VAL_BUNDLE: {
        AutoPtr<IBundle> b = AndroidParcelUtils::ReadBundle(source);
        return b;
    }

    case VAL_PERSISTABLEBUNDLE:
        assert(0);
        return NULL;

    case VAL_SIZE:
        assert(0);
        return NULL;

    case VAL_SIZEF:
        assert(0);
        return NULL;

    case VAL_DOUBLEARRAY:
        assert(0);
        return NULL;

    default:
        Logger::E(TAG, "Unknown type code %d", type);
        return NULL;
    }
}

AutoPtr<IParcelable> AndroidParcelUtils::ReadParcelable(
    /* [in] */ const android::Parcel& source)
{
    String creator = ReadString(source);
    if (creator.IsNull()) {
        return NULL;
    }
    if (creator.Equals("android.graphics.Rect")) {
        return IParcelable::Probe(ReadRect(source));
    }
    else if (creator.Equals("android.view.Display$HdrCapabilities")) {
        Int32 types = ReadInt32(source);
        // mSupportedHdrTypes = new int[types];
        for (Int32 i = 0; i < types; ++i) {
            Int32 supportedHdrTypes = ReadInt32(source);
        }
        Float maxLuminance = ReadFloat(source);
        Float maxAverageLuminance = ReadFloat(source);
        Float minLuminance = ReadFloat(source);
        return NULL;
    }
    else if (creator.Equals("android.view.AbsSavedState")) {
        AutoPtr<IParcelable> superState = ReadParcelable(source);
        assert(superState == NULL);
        AutoPtr<IAbsSavedStateHelper> helper;
        CAbsSavedStateHelper::AcquireSingleton((IAbsSavedStateHelper**)&helper);
        AutoPtr<IAbsSavedState> state;
        helper->GetEMPTY_STATE((IAbsSavedState**)&state);
        return IParcelable::Probe(state);
    }
    else if (creator.Equals("android.widget.TextView$SavedState")) {
        Logger::D(TAG, "Read Elastos.Droid.Widget.CTextViewSavedState not implemented");
        return NULL;
    }
    else if (creator.Equals("android.widget.CompoundButton$SavedState")) {
        Logger::D(TAG, "Read Elastos.Droid.Widget.CCompoundButtonSavedState not implemented");
        return NULL;
    }
    else if (creator.Equals("andriod.widget.AbsListView$SavedState")) {
        Logger::D(TAG, "Read Elastos.Droid.Widget.CAbsListViewSavedState not implemented");
        return NULL;
    }
    else if (creator.Equals("android.support.v4.view.ViewPager$SavedState")) {
        Logger::D(TAG, "Read Elastos.Droid.Support.V4.View.CViewPagerSavedState not implemented");
        return NULL;
    }
    else {
        Logger::D(TAG, "[ReadParcelable] Read %s not implemented", creator.string());
        return NULL;
    }

}

AutoPtr<ICharSequence> AndroidParcelUtils::Read_CHAR_SEQUENCE(
    /* [in] */ const android::Parcel& source)
{
    Int32 kind = ReadInt32(source);

    String string = ReadString(source);
    if (string.IsNull()) {
        return NULL;
    }

    AutoPtr<ICharSequence> cs;
    CString::New(string, (ICharSequence**)&cs);
    if (kind == 1) {
        return cs;
    }

    AutoPtr<ISpannableString> sp;
    CSpannableString::New(cs, (ISpannableString**)&sp);

    while (TRUE) {
        kind = ReadInt32(source);

        if (kind == 0) {
            break;
        }

        switch(kind) {
        case ITextUtils::ALIGNMENT_SPAN: {
            LayoutAlignment alignment = GetLayoutAlignmentFromString(ReadString(source));
            AutoPtr<IAlignmentSpan> span;
            CAlignmentSpanStandard::New(alignment, (IAlignmentSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::FOREGROUND_COLOR_SPAN: {
            Int32 color = ReadInt32(source);
            AutoPtr<IForegroundColorSpan> span;
            CForegroundColorSpan::New(color, (IForegroundColorSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::RELATIVE_SIZE_SPAN: {
            Float proportion = ReadFloat(source);
            AutoPtr<IRelativeSizeSpan> span;
            CRelativeSizeSpan::New(proportion, (IRelativeSizeSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::SCALE_X_SPAN: {
            Float proportion = ReadFloat(source);
            AutoPtr<IScaleXSpan> span;
            CScaleXSpan::New(proportion, (IScaleXSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::STRIKETHROUGH_SPAN: {
            AutoPtr<IStrikethroughSpan> span;
            CStrikethroughSpan::New((IStrikethroughSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::UNDERLINE_SPAN: {
            AutoPtr<IUnderlineSpan> span;
            CUnderlineSpan::New((IUnderlineSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::STYLE_SPAN: {
            Int32 style = ReadInt32(source);
            AutoPtr<IStyleSpan> span;
            CStyleSpan::New(style, (IStyleSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::BULLET_SPAN: {
            Int32 gapWidth = ReadInt32(source);
            Boolean wantColor = ReadInt32(source) != 0;
            Int32 color = ReadInt32(source);
            AutoPtr<IBulletSpan> span;
            if (color == 0) {
                CBulletSpan::New(gapWidth, (IBulletSpan**)&span);
            }
            else {
                CBulletSpan::New(gapWidth, color, (IBulletSpan**)&span);
            }
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::QUOTE_SPAN: {
            Int32 color = ReadInt32(source);
            AutoPtr<IQuoteSpan> span;
            CQuoteSpan::New(color, (IQuoteSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::LEADING_MARGIN_SPAN: {
            Int32 first = ReadInt32(source);
            Int32 rest = ReadInt32(source);
            AutoPtr<ILeadingMarginSpan> span;
            CLeadingMarginSpanStandard::New(first, rest, (ILeadingMarginSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::URL_SPAN: {
            String url = ReadString(source);
            AutoPtr<IURLSpan> span;
            CURLSpan::New(url, (IURLSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::BACKGROUND_COLOR_SPAN: {
            Int32 color = ReadInt32(source);
            AutoPtr<IBackgroundColorSpan> span;
            CBackgroundColorSpan::New(color, (IBackgroundColorSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::TYPEFACE_SPAN: {
            String family = ReadString(source);
            AutoPtr<ITypefaceSpan> span;
            CTypefaceSpan::New(family, (ITypefaceSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::SUPERSCRIPT_SPAN: {
            AutoPtr<ISuperscriptSpan> span;
            CSuperscriptSpan::New((ISuperscriptSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::SUBSCRIPT_SPAN: {
            AutoPtr<ISubscriptSpan> span;
            CSubscriptSpan::New((ISubscriptSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::ABSOLUTE_SIZE_SPAN: {
            Int32 size = ReadInt32(source);
            Boolean dip = ReadInt32(source) != 0;
            AutoPtr<IAbsoluteSizeSpan> span;
            CAbsoluteSizeSpan::New(size, dip, (IAbsoluteSizeSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::TEXT_APPEARANCE_SPAN: {
            String family = ReadString(source);
            Int32 style = ReadInt32(source);
            Int32 size = ReadInt32(source);
            AutoPtr<IColorStateList> color;
            if (ReadInt32(source) != 0) {
                color = ReadColorStateList(source);
            }
            AutoPtr<IColorStateList> linkColor;
            if (ReadInt32(source) != 0) {
                linkColor = ReadColorStateList(source);
            }
            AutoPtr<ITextAppearanceSpan> span;
            CTextAppearanceSpan::New(family, style, size, color, linkColor,
                    (ITextAppearanceSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::ANNOTATION: {
            String key = ReadString(source);
            String value = ReadString(source);
            AutoPtr<IAnnotation> span;
            CAnnotation::New(key, value, (IAnnotation**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::SUGGESTION_SPAN: {
            AutoPtr<CSuggestionSpan> span;
            CSuggestionSpan::NewByFriend((CSuggestionSpan**)&span);
            span->mSuggestions = CreateStringArray(source);
            span->mFlags = ReadInt32(source);
            span->mLocaleString = ReadString(source);
            // don't have mLanguageTag
            String languageTag = ReadString(source);
            span->mNotificationTargetClassName = ReadString(source);
            span->mNotificationTargetPackageName = ReadString(source);
            span->mHashCode = ReadInt32(source);
            span->mEasyCorrectUnderlineColor = ReadInt32(source);
            span->mEasyCorrectUnderlineThickness = ReadFloat(source);
            span->mMisspelledUnderlineColor = ReadInt32(source);
            span->mMisspelledUnderlineThickness = ReadFloat(source);
            span->mAutoCorrectionUnderlineColor = ReadInt32(source);
            span->mAutoCorrectionUnderlineThickness = ReadFloat(source);
            ReadSpan(source, ISpannable::Probe(sp), (ISuggestionSpan*)span.Get());
            break;
        }

        case ITextUtils::SPELL_CHECK_SPAN: {
            Boolean spellCheckInProgress = ReadInt32(source) != 0;
            AutoPtr<ISpellCheckSpan> span;
            CSpellCheckSpan::New((ISpellCheckSpan**)&span);
            span->SetSpellCheckInProgress(spellCheckInProgress);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::SUGGESTION_RANGE_SPAN: {
            Int32 backgroundColor = ReadInt32(source);
            AutoPtr<ISuggestionRangeSpan> span;
            CSuggestionRangeSpan::New((ISuggestionRangeSpan**)&span);
            span->SetBackgroundColor(backgroundColor);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::EASY_EDIT_SPAN: {
            AutoPtr<IPendingIntent> intent = ReadPendingIntent(source);
            Boolean deleteEnabled = ReadByte(source) == 1;
            AutoPtr<IEasyEditSpan> span;
            CEasyEditSpan::New(intent, (IEasyEditSpan**)&span);
            span->SetDeleteEnabled(deleteEnabled);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::LOCALE_SPAN: {
            String languageTags = ReadString(source);
            AutoPtr< ArrayOf<ILocale*> > locales = ForLanguageTags(languageTags);
            AutoPtr<ILocale> locale;
            if (locales != NULL) {
                locale = (*locales)[0];
            }
            AutoPtr<ILocaleSpan> span;
            CLocaleSpan::New(locale, (ILocaleSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        case ITextUtils::TTS_SPAN: {
            String type = ReadString(source);
            AutoPtr<IPersistableBundle> args = ReadPersistableBundle(source);
            AutoPtr<ITtsSpan> span;
            CTtsSpan::New(type, args, (ITtsSpan**)&span);
            ReadSpan(source, ISpannable::Probe(sp), span.Get());
            break;
        }

        // case ACCESSIBILITY_CLICKABLE_SPAN:
        //     readSpan(p, sp, new AccessibilityClickableSpan(p));
        //     break;

        // case ACCESSIBILITY_URL_SPAN:
        //     readSpan(p, sp, new AccessibilityURLSpan(p));
        //     break;

        default:
            Logger::E(TAG, "bogus span encoding %d", kind);
            return NULL;

        }
    }

    return ICharSequence::Probe(sp);
}

AutoPtr<IFileDescriptor> AndroidParcelUtils::ReadFileDescriptor(
    /* [in] */ const android::Parcel& source)
{
    Int32 fd = source.readFileDescriptor();
    if (fd < 0) return NULL;
    fd = dup(fd);
    if (fd < 0) return NULL;
    AutoPtr<IFileDescriptor> fdObj;
    CFileDescriptor::New((IFileDescriptor**)&fdObj);
    fdObj->SetDescriptor(fd);
    return fdObj;
}

AutoPtr<IMap> AndroidParcelUtils::ReadHashMap(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr<IHashMap> m;
    CHashMap::New(N, (IHashMap**)&m);
    ReadMapInternal(source, m, N);
    return IMap::Probe(m);
}

AutoPtr<IActivityOptions> AndroidParcelUtils::ReadActivityOptions(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IBundle> result = AndroidParcelUtils::ReadBundle(source);
    AutoPtr<IActivityOptions> options;
    CActivityOptions::New(result, (IActivityOptions**)&options);
    return options;
}

AutoPtr<IContentProviderHolder> AndroidParcelUtils::ReadContentProviderHolder(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CContentProviderHolder> holder;
    CContentProviderHolder::NewByFriend((CContentProviderHolder**)&holder);

    holder->mInfo = ReadProviderInfo(source);
    holder->mProvider = new IContentProviderProxy2(source.readStrongBinder());
    holder->mConnection = ReadStrongBinder(source);
    holder->mNoReleaseNeeded = ReadInt32(source) != 0;

    return holder.Get();
}

AutoPtr<IInstrumentationWatcher> AndroidParcelUtils::ReadIInstrumentationWatcher(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IInstrumentationWatcher> watcher =
            new IInstrumentationWatcherProxy(source.readStrongBinder());
    return watcher;
}

AutoPtr<IIUiAutomationConnection> AndroidParcelUtils::ReadIUiAutomationConnection(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IIUiAutomationConnection> connection =
            new IIUiAutomationConnectionProxy(source.readStrongBinder());
    return connection;
}

AutoPtr<IPendingIntent> AndroidParcelUtils::ReadPendingIntent(
    /* [in] */ const android::Parcel& source)
{
    String className = ReadString(source);
    android::sp<android::IBinder> proxy = source.readStrongBinder();
    if (proxy == NULL) return NULL;
    assert(0);
    return NULL;
}

AutoPtr<IProfilerInfo> AndroidParcelUtils::ReadProfilerInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr<IResultInfo> AndroidParcelUtils::ReadResultInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CResultInfo> info;
    CResultInfo::NewByFriend((CResultInfo**)&info);

    info->mResultWho = ReadString(source);
    info->mRequestCode = ReadInt32(source);
    info->mResultCode = ReadInt32(source);
    if (ReadInt32(source) != 0) {
        info->mData = ReadIntent(source);
    }
    return info.Get();
}

AutoPtr<IList> AndroidParcelUtils::CreateResultInfoList(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr<IList> l;
    CArrayList::New(N, (IList**)&l);
    while (N > 0) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IResultInfo> info = ReadResultInfo(source);
            l->Add(info.Get());
        }
        else {
            l->Add(NULL);
        }
    }
    return l;
}

AutoPtr<IClipData> AndroidParcelUtils::ReadClipData(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr<IComponentName> AndroidParcelUtils::ReadComponentName(
    /* [in] */ const android::Parcel& source)
{
    String package = ReadString(source);
    String klass = ReadString(source);
    AutoPtr<IComponentName> component;
    CComponentName::New(package, klass, (IComponentName**)&component);
    return component;
}

AutoPtr<IIntent> AndroidParcelUtils::ReadIntent(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CIntent> intent;
    CIntent::NewByFriend((CIntent**)&intent);

    intent->mAction = ReadString(source);
    intent->mData = ReadUri(source);
    intent->mType = ReadString(source);
    intent->mFlags = ReadInt32(source);
    intent->mPackage = ReadString(source);
    intent->mComponent = ReadComponentName(source);

    if (ReadInt32(source) != 0) {
        intent->mSourceBounds = ReadRect(source);
    }

    Int32 N = ReadInt32(source);
    if (N > 0) {
        intent->mCategories = new HashSet<String>();
        for (Int32 i = 0; i < N; i++) {
            intent->mCategories->Insert(ReadString(source));
        }
    }

    if (ReadInt32(source) != 0) {
        intent->mSelector = ReadIntent(source);
    }

    if (ReadInt32(source) != 0) {
        intent->mClipData = ReadClipData(source);
    }
    intent->mContentUserHint = ReadInt32(source);
    intent->mExtras = ReadBundle(source);

    return intent;
}

AutoPtr<IIntentFilter> AndroidParcelUtils::ReadIntentFilter(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    AutoPtr<IIntentFilter> filter;
    return filter;
}

AutoPtr<IActivityInfo> AndroidParcelUtils::ReadActivityInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CActivityInfo> info;
    CActivityInfo::NewByFriend((CActivityInfo**)&info);

    // PackageItemInfo
    ReadPackageItemInfo(source, info.Get());

    // ComponentInfo
    ReadComponentInfo(source, info.Get());

    // ActivityInfo
    info->mTheme = ReadInt32(source);
    info->mLaunchMode = ReadInt32(source);
    info->mDocumentLaunchMode = ReadInt32(source);
    info->mPermission = ReadString(source);
    info->mTaskAffinity = ReadString(source);
    info->mTargetActivity = ReadString(source);
    info->mFlags = ReadInt32(source);
    info->mScreenOrientation = ReadInt32(source);
    info->mConfigChanges = ReadInt32(source);
    info->mSoftInputMode = ReadInt32(source);
    info->mUiOptions = ReadInt32(source);
    info->mParentActivityName = ReadString(source);
    info->mPersistableMode = ReadInt32(source);
    info->mMaxRecents = ReadInt32(source);
    // don't have mLockTaskLaunchMode
    Int32 lockTaskLaunchMode = ReadInt32(source);
    if (ReadInt32(source) == 1) {
        // windowLayout = new WindowLayout(source);
        Int32 width = ReadInt32(source);
        Float widthFraction = ReadFloat(source);
        Int32 height = ReadInt32(source);
        Float heightFraction = ReadFloat(source);
        Int32 gravity = ReadInt32(source);
        Int32 minWidth = ReadInt32(source);
        Int32 minHeight = ReadInt32(source);
    }
    // don't have mResizeMode
    Int32 resizeMode = ReadInt32(source);
    // don't have mRequestedVrComponent
    String requestedVrComponent = ReadString(source);
    // don't have mRotationAnimation
    Int32 rotationAnimation = ReadInt32(source);
    // don't have mColorMode
    Int32 colorMode = ReadInt32(source);
    // don't have mMaxAspectRatio
    Float maxAspectRatio = ReadFloat(source);

    return info.Get();
}

AutoPtr< ArrayOf<IActivityInfo*> > AndroidParcelUtils::CreateActivityInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IActivityInfo*> > l = ArrayOf<IActivityInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IActivityInfo> ai = ReadActivityInfo(source);
            l->Set(i, ai);
        }
    }
    return l;
}

AutoPtr<IApplicationInfo> AndroidParcelUtils::ReadApplicationInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CApplicationInfo> info;
    CApplicationInfo::NewByFriend((CApplicationInfo**)&info);

    // PackageItemInfo
    ReadPackageItemInfo(source, info.Get());

    // CApplicationInfo
    info->mTaskAffinity = ReadString(source);
    info->mPermission = ReadString(source);
    info->mProcessName = ReadString(source);
    info->mClassName = ReadString(source);
    info->mTheme = ReadInt32(source);
    info->mFlags = ReadInt32(source);
    // don't have mPrivateFlags member
    Int32 privateFlags = ReadInt32(source);
    info->mRequiresSmallestWidthDp = ReadInt32(source);
    info->mCompatibleWidthLimitDp = ReadInt32(source);
    info->mLargestWidthLimitDp = ReadInt32(source);
    if (ReadInt32(source) != 0) {
        // don't have mStorageUuid and mVolumeUuid
        ReadInt64(source);
        ReadInt64(source);
    }
    info->mScanSourceDir = ReadString(source);
    info->mScanPublicSourceDir = ReadString(source);
    info->mSourceDir = ReadString(source);
    info->mPublicSourceDir = ReadString(source);
    // don't have mSplitNames
    AutoPtr< ArrayOf<String> > splitNames = CreateStringArray(source);
    info->mSplitSourceDirs = CreateStringArray(source);
    info->mSplitPublicSourceDirs = CreateStringArray(source);
    // don't have mSplitDependencies
    AutoPtr<ISparseArray> splitDependencies = ReadSparseArray(source);
    info->mNativeLibraryDir = ReadString(source);
    info->mSecondaryNativeLibraryDir = ReadString(source);
    info->mNativeLibraryRootDir = ReadString(source);
    info->mNativeLibraryRootRequiresIsa = ReadInt32(source) != 0;
    info->mPrimaryCpuAbi = ReadString(source);
    info->mSecondaryCpuAbi = ReadString(source);
    info->mResourceDirs = CreateStringArray(source);
    info->mSeinfo = ReadString(source);
    // don't have mSeinfoUser
    String seinfoUser = ReadString(source);
    info->mSharedLibraryFiles = CreateStringArray(source);
    info->mDataDir = ReadString(source);
    // don't have mDeviceProtectedDataDir
    String deviceProtectedDataDir = ReadString(source);
    String credentialProtectedDataDir = ReadString(source);
    info->mUid = ReadInt32(source);
    // don't have mMinSdkVersion
    Int32 minSdkVersion = ReadInt32(source);
    info->mTargetSdkVersion = ReadInt32(source);
    info->mVersionCode = ReadInt32(source);
    info->mEnabled = ReadInt32(source) != 0;
    info->mEnabledSetting = ReadInt32(source);
    info->mInstallLocation = ReadInt32(source);
    info->mManageSpaceActivityName = ReadString(source);
    info->mBackupAgentName = ReadString(source);
    info->mDescriptionRes = ReadInt32(source);
    info->mUiOptions = ReadInt32(source);
    // don't have mFullBackupContent
    Int32 fullBackupContent = ReadInt32(source);
    // don't have mNetworkSecurityConfigRes
    Int32 networkSecurityConfigRes = ReadInt32(source);
    // don't have mCategory
    Int32 category = ReadInt32(source);
    // don't have mTargetSandboxVersion
    Int32 targetSandboxVersion = ReadInt32(source);

    return info.Get();
}

AutoPtr<IConfigurationInfo> AndroidParcelUtils::ReadConfigurationInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IConfigurationInfo*> > AndroidParcelUtils::CreateConfigurationInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IConfigurationInfo*> > l = ArrayOf<IConfigurationInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IConfigurationInfo> ci = ReadConfigurationInfo(source);
            l->Set(i, ci);
        }
    }
    return l;
}

AutoPtr<IFeatureInfo> AndroidParcelUtils::ReadFeatureInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IFeatureInfo*> > AndroidParcelUtils::CreateFeatureInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IFeatureInfo*> > l = ArrayOf<IFeatureInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IFeatureInfo> fi = ReadFeatureInfo(source);
            l->Set(i, fi);
        }
    }
    return l;
}

AutoPtr<IFeatureGroupInfo> AndroidParcelUtils::ReadFeatureGroupInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IFeatureGroupInfo*> > AndroidParcelUtils::CreateFeatureGroupInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IFeatureGroupInfo*> > l = ArrayOf<IFeatureGroupInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IFeatureGroupInfo> fgi = ReadFeatureGroupInfo(source);
            l->Set(i, fgi);
        }
    }
    return l;
}

AutoPtr<IInstrumentationInfo> AndroidParcelUtils::ReadInstrumentationInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IInstrumentationInfo*> > AndroidParcelUtils::CreateInstrumentationInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IInstrumentationInfo*> > l = ArrayOf<IInstrumentationInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IInstrumentationInfo> ii = ReadInstrumentationInfo(source);
            l->Set(i, ii);
        }
    }
    return l;
}

AutoPtr<IPackageInfo> AndroidParcelUtils::ReadPackageInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CPackageInfo> info;
    CPackageInfo::NewByFriend((CPackageInfo**)&info);

    info->mPackageName = ReadString(source);
    info->mSplitNames = CreateStringArray(source);
    info->mVersionCode = ReadInt32(source);
    info->mVersionName = ReadString(source);
    // don't have mBaseRevisionCode
    Int32 baseRevisionCode = ReadInt32(source);
    // don't have mSplitRevisionCodes
    AutoPtr< ArrayOf<Int32> > SplitRevisionCodes = CreateInt32Array(source);
    info->mSharedUserId = ReadString(source);
    info->mSharedUserLabel = ReadInt32(source);
    Int32 hasApp = ReadInt32(source);
    if (hasApp != 0) {
        info->mApplicationInfo = ReadApplicationInfo(source);
    }
    info->mFirstInstallTime = ReadInt64(source);
    info->mLastUpdateTime = ReadInt64(source);
    info->mGids = CreateInt32Array(source);
    info->mActivities = CreateActivityInfoArray(source);
    info->mReceivers = CreateActivityInfoArray(source);
    info->mServices = CreateServiceInfoArray(source);
    info->mProviders = CreateProviderInfoArray(source);
    info->mInstrumentation = CreateInstrumentationInfoArray(source);
    info->mPermissions = CreatePermissionInfoArray(source);
    info->mRequestedPermissions = CreateStringArray(source);
    info->mRequestedPermissionsFlags = CreateInt32Array(source);
    info->mSignatures = CreateSignatureArray(source);
    info->mConfigPreferences = CreateConfigurationInfoArray(source);
    info->mReqFeatures = CreateFeatureInfoArray(source);
    info->mFeatureGroups = CreateFeatureGroupInfoArray(source);
    info->mInstallLocation = ReadInt32(source);
    info->mCoreApp = ReadInt32(source) != 0;
    info->mRequiredForAllUsers = ReadInt32(source) != 0;
    info->mRestrictedAccountType = ReadString(source);
    info->mRequiredAccountType = ReadString(source);
    info->mOverlayTarget = ReadString(source);
    // don't have mIsStaticOverlay
    Boolean isStaticOverlay = ReadInt32(source) != 0;
    // don't have mOverlayPriority
    Int32 overlayPriority = ReadInt32(source);

    return info.Get();
}

AutoPtr<IPathPermission> AndroidParcelUtils::ReadPathPermission(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IPathPermission*> > AndroidParcelUtils::CreatePathPermissionArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IPathPermission*> > l = ArrayOf<IPathPermission*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            l->Set(i, ReadPathPermission(source));
        }
    }
    return l;
}

AutoPtr<IPermissionInfo> AndroidParcelUtils::ReadPermissionInfo(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IPermissionInfo*> > AndroidParcelUtils::CreatePermissionInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IPermissionInfo*> > l = ArrayOf<IPermissionInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IPermissionInfo> pi = ReadPermissionInfo(source);
            l->Set(i, pi);
        }
    }
    return l;
}

AutoPtr<IProviderInfo> AndroidParcelUtils::ReadProviderInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CProviderInfo> info;
    CProviderInfo::NewByFriend((CProviderInfo**)&info);

    // PackageItemInfo
    ReadPackageItemInfo(source, info.Get());

    // ComponentInfo
    ReadComponentInfo(source, info.Get());

    // ProviderInfo
    info->mAuthority = ReadString(source);
    info->mReadPermission = ReadString(source);
    info->mWritePermission = ReadString(source);
    info->mGrantUriPermissions = ReadInt32(source) != 0;
    info->mUriPermissionPatterns = CreatePatternMatcherArray(source);
    info->mPathPermissions = CreatePathPermissionArray(source);
    info->mMultiprocess = ReadInt32(source) != 0;
    info->mInitOrder = ReadInt32(source);
    info->mFlags = ReadInt32(source);
    info->mIsSyncable = ReadInt32(source) != 0;

    return info.Get();
}

AutoPtr< ArrayOf<IProviderInfo*> > AndroidParcelUtils::CreateProviderInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IProviderInfo*> > l = ArrayOf<IProviderInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IProviderInfo> pi = ReadProviderInfo(source);
            l->Set(i, pi);
        }
    }
    return l;
}

AutoPtr<IList> AndroidParcelUtils::CreateProviderInfoList(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr<IList> l;
    CArrayList::New(N, (IList**)&l);
    while (N > 0) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IProviderInfo> info = ReadProviderInfo(source);
            l->Add(info.Get());
        }
        else {
            l->Add(NULL);
        }
    }
    return l;
}

AutoPtr<IResolveInfo> AndroidParcelUtils::ReadResolveInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CResolveInfo> info;
    CResolveInfo::NewByFriend((CResolveInfo**)&info);
    switch (ReadInt32(source)) {
        case 1:
            info->mActivityInfo = ReadActivityInfo(source);
            break;
        case 2:
            info->mServiceInfo = ReadServiceInfo(source);
            break;
        case 3:
            info->mProviderInfo = ReadProviderInfo(source);
            break;
        default:
            Logger::W(TAG, "Missing ComponentInfo!");
            break;
    }
    if (ReadInt32(source) != 0) {
        info->mFilter = ReadIntentFilter(source);
    }
    info->mPriority = ReadInt32(source);
    info->mPreferredOrder = ReadInt32(source);
    info->mMatch = ReadInt32(source);
    info->mSpecificIndex = ReadInt32(source);
    info->mLabelRes = ReadInt32(source);
    info->mNonLocalizedLabel = Read_CHAR_SEQUENCE(source);
    info->mIcon = ReadInt32(source);
    info->mResolvePackageName = ReadString(source);
    info->mTargetUserId = ReadInt32(source);
    info->mSystem = ReadInt32(source) != 0;
    info->mNoResourceId = ReadInt32(source) != 0;
    // don't have mIconResourceId
    Int32 iconResourceId = ReadInt32(source);
    // don't have mHandleAllWebDataURI
    Boolean handleAllWebDataURI = ReadInt32(source) != 0;
    // don't have mInstantAppAvailable and mIsInstantAppAvailable
    Boolean isInstantAppAvailable = ReadInt32(source) != 0;
    Boolean instantAppAvailable = isInstantAppAvailable;
    return info.Get();
}

AutoPtr<IServiceInfo> AndroidParcelUtils::ReadServiceInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CServiceInfo> info;
    CServiceInfo::NewByFriend((CServiceInfo**)&info);

    // PackageItemInfo
    ReadPackageItemInfo(source, info.Get());

    // ComponentInfo
    ReadComponentInfo(source, info.Get());

    // ServiceInfo
    info->mPermission = ReadString(source);
    info->mFlags = ReadInt32(source);

    return info.Get();
}

AutoPtr< ArrayOf<IServiceInfo*> > AndroidParcelUtils::CreateServiceInfoArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IServiceInfo*> > l = ArrayOf<IServiceInfo*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IServiceInfo> si = ReadServiceInfo(source);
            l->Set(i, si);
        }
    }
    return l;
}

AutoPtr<ISignature> AndroidParcelUtils::ReadSignature(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<ISignature*> > AndroidParcelUtils::CreateSignatureArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<ISignature*> > l = ArrayOf<ISignature*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            AutoPtr<ISignature> sig = ReadSignature(source);
            l->Set(i, sig);
        }
    }
    return l;
}

AutoPtr<IColorStateList> AndroidParcelUtils::ReadColorStateList(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    AutoPtr< ArrayOf<Int32Array > > stateSpecs = ArrayOf<Int32Array >::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr< ArrayOf<Int32> > stateSpec = CreateInt32Array(source);
        stateSpecs->Set(i, stateSpec);
    }
    AutoPtr< ArrayOf<Int32> > colors = CreateInt32Array(source);
    AutoPtr<IColorStateList> colorStateList;
    CColorStateList::New((ArrayOf<Handle32>*)stateSpecs.Get(), colors,
            (IColorStateList**)&colorStateList);
    return colorStateList;
}

AutoPtr<ICompatibilityInfo> AndroidParcelUtils::ReadCompatibilityInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CCompatibilityInfo> info;
    CCompatibilityInfo::NewByFriend((CCompatibilityInfo**)&info);

    info->mCompatibilityFlags = ReadInt32(source);
    info->mApplicationDensity = ReadInt32(source);
    info->mApplicationScale = ReadFloat(source);
    info->mApplicationInvertedScale = ReadFloat(source);

    return info.Get();
}

AutoPtr<IConfiguration> AndroidParcelUtils::ReadConfiguration(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CConfiguration> config;
    CConfiguration::NewByFriend((CConfiguration**)&config);
    config->mFontScale = ReadFloat(source);
    config->mMcc = ReadInt32(source);
    config->mMnc = ReadInt32(source);

    const Int32 localeListSize = ReadInt32(source);
    if (localeListSize > 0) {
        AutoPtr< ArrayOf<ILocale*> > localeArray = ArrayOf<ILocale*>::Alloc(localeListSize);
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        for (Int32 i = 0; i < localeListSize; i++) {
            AutoPtr<ILocale> locale;
            helper->ForLanguageTag(ReadString(source), (ILocale**)&locale);
            localeArray->Set(i, locale.Get());
        }
        config->mLocale = (*localeArray)[0];
    }

    config->mUserSetLocale = ReadInt32(source) == 1;
    config->mTouchscreen = ReadInt32(source);
    config->mKeyboard = ReadInt32(source);
    config->mKeyboardHidden = ReadInt32(source);
    config->mHardKeyboardHidden = ReadInt32(source);
    config->mNavigation = ReadInt32(source);
    config->mNavigationHidden = ReadInt32(source);
    config->mOrientation = ReadInt32(source);
    config->mScreenLayout = ReadInt32(source);
    // don't have mColorMode
    Int32 colorMode = ReadInt32(source);
    config->mUiMode = ReadInt32(source);
    config->mScreenWidthDp = ReadInt32(source);
    config->mScreenHeightDp = ReadInt32(source);
    config->mSmallestScreenWidthDp = ReadInt32(source);
    config->mDensityDpi = ReadInt32(source);
    config->mCompatScreenWidthDp = ReadInt32(source);
    config->mCompatScreenHeightDp = ReadInt32(source);
    config->mCompatSmallestScreenWidthDp = ReadInt32(source);
    // don't have mAppBounds
    AutoPtr<IRect> appBounds = IRect::Probe(ReadValue(source));
    // don't have mAssetsSeq
    Int32 assetsSeq = ReadInt32(source);
    config->mSeq = ReadInt32(source);

    return config.Get();
}

AutoPtr<IRect> AndroidParcelUtils::ReadRect(
    /* [in] */ const android::Parcel& source)
{
    Int32 left = ReadInt32(source);
    Int32 top = ReadInt32(source);
    Int32 right = ReadInt32(source);
    Int32 bottom = ReadInt32(source);

    AutoPtr<IRect> rect;
    CRect::New(left, top, right, bottom, (IRect**)&rect);
    return rect;
}

AutoPtr<IIVoiceInteractor> AndroidParcelUtils::ReadIVoiceInteractor(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IIVoiceInteractor> vi =
            new IVoiceInteractorProxy(source.readStrongBinder());
    return vi;
}

AutoPtr<IList> AndroidParcelUtils::CreateReferrerIntentList(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr<IList> l;
    CArrayList::New(N, (IList**)&l);
    while (N > 0) {
        if (ReadInt32(source) != 0) {
            AutoPtr<IIntent> intent = ReadIntent(source);
            String referrer = ReadString(source);
            l->Add(intent.Get());
        }
        else {
            l->Add(NULL);
        }
    }
    return l;
}

AutoPtr<IInputBindResult> AndroidParcelUtils::ReadInputBindResult(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CInputBindResult> result;
    CInputBindResult::NewByFriend((CInputBindResult**)&result);

    result->mMethod = new IInputMethodSessionProxy(source.readStrongBinder());
    if (ReadInt32(source) != 0) {
        result->mChannel = ReadInputChannel(source);
    }
    else {
        result->mChannel = NULL;
    }
    result->mId = ReadString(source);
    result->mSequence = ReadInt32(source);
    result->mUserActionNotificationSequenceNumber = ReadInt32(source);

    return result.Get();
}

AutoPtr<IIInputContextCallback> AndroidParcelUtils::ReadIInputContextCallback(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IIInputContextCallback> cb =
            new IInputContextCallbackProxy(source.readStrongBinder());
    return cb;
}

AutoPtr<IUri> AndroidParcelUtils::ReadUri(
    /* [in] */ const android::Parcel& source)
{
    Int32 type = ReadInt32(source);
    switch(type) {
        case 0: return NULL;
        case 1: {
            assert(0);
            return NULL;
        }
        case 2: {
            assert(0);
            return NULL;
        }
        case 3: {
            assert(0);
            return NULL;
        }
        default: {
            Logger::E(TAG, "Unknown type code %d", type);
            return NULL;
        }
    }
}

AutoPtr<IBinder> AndroidParcelUtils::ReadStrongBinder(
    /* [in] */ const android::Parcel& source)
{
    const android::sp<android::IBinder>& b = source.readStrongBinder();
    if (b == NULL) return NULL;
    AutoPtr<IBinder> binder = DroidObjectForIBinder(b);
    return binder;
}

AutoPtr<IBundle> AndroidParcelUtils::ReadBundle(
    /* [in] */ const android::Parcel& source)
{
    Int32 length = ReadInt32(source);
    if (length < 0) {
        return NULL;
    }

    AutoPtr<CBundle> bundle;
    CBundle::NewByFriend((CBundle**)&bundle);
    bundle->mMap = NULL;

    Int32 magic = ReadInt32(source);
    if (magic != CBundle::BUNDLE_MAGIC) {
        Logger::E(TAG, "Bad magic number for Bundle: 0x%08x", magic);
        return NULL;
    }

    Int32 offset = source.dataPosition();
    source.setDataPosition(offset + length);

    android::Parcel* p = new android::Parcel();
    p->setDataPosition(0);
    p->appendFrom(&source, offset, length);
    p->setDataPosition(0);
    AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(p, TRUE);
    bundle->mParcelledData = wrapper;

    return bundle.Get();
}

AutoPtr<IParcelFileDescriptor> AndroidParcelUtils::ReadParcelFileDescriptor(
    /* [in] */ const android::Parcel& source)
{
    Int32 hasCommChannel = source.readInt32();
    AutoPtr<IFileDescriptor> fd = ReadFileDescriptor(source);
    AutoPtr<IFileDescriptor> commChannel;
    if (hasCommChannel != 0) {
        commChannel = ReadFileDescriptor(source);
    }
    AutoPtr<IParcelFileDescriptor> pfd;
    CParcelFileDescriptor::New(fd, commChannel, (IParcelFileDescriptor**)&pfd);
    return pfd;
}

AutoPtr<IPatternMatcher> AndroidParcelUtils::ReadPatternMatcher(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr< ArrayOf<IPatternMatcher*> > AndroidParcelUtils::CreatePatternMatcherArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr< ArrayOf<IPatternMatcher*> > l = ArrayOf<IPatternMatcher*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        if (ReadInt32(source) != 0) {
            l->Set(i, ReadPatternMatcher(source));
        }
    }
    return l;
}

AutoPtr<IPersistableBundle> AndroidParcelUtils::ReadPersistableBundle(
    /* [in] */ const android::Parcel& source)
{
    assert(0);
    return NULL;
}

AutoPtr<ISparseArray> AndroidParcelUtils::ReadSparseArray(
    /* [in] */ const android::Parcel& source)
{
    Int32 N = ReadInt32(source);
    if (N < 0) {
        return NULL;
    }
    AutoPtr<CSparseArray> sa;
    CSparseArray::NewByFriend(N, (CSparseArray**)&sa);
    while (N > 0) {
        Int32 key = ReadInt32(source);
        AutoPtr<IInterface> value = ReadValue(source);
        sa->Append(key, value);
        N--;
    }
    return sa.Get();
}

AutoPtr<IDisplayInfo> AndroidParcelUtils::ReadDisplayInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CDisplayInfo> info;
    CDisplayInfo::NewByFriend((CDisplayInfo**)&info);

    info->mLayerStack = ReadInt32(source);
    info->mFlags = ReadInt32(source);
    info->mType = ReadInt32(source);
    info->mAddress = ReadString(source);
    info->mName = ReadString(source);
    info->mAppWidth = ReadInt32(source);
    info->mAppHeight = ReadInt32(source);
    info->mSmallestNominalAppWidth = ReadInt32(source);
    info->mSmallestNominalAppHeight = ReadInt32(source);
    info->mLargestNominalAppWidth = ReadInt32(source);
    info->mLargestNominalAppHeight = ReadInt32(source);
    info->mLogicalWidth = ReadInt32(source);
    info->mLogicalHeight = ReadInt32(source);
    info->mOverscanLeft = ReadInt32(source);
    info->mOverscanTop = ReadInt32(source);
    info->mOverscanRight = ReadInt32(source);
    info->mOverscanBottom = ReadInt32(source);
    info->mRotation = ReadInt32(source);
    // don't hava mModeId
    Int32 modeId = ReadInt32(source);
    // don't have mDefaultModeId
    Int32 defaultModeId = ReadInt32(source);
    Int32 nModes = ReadInt32(source);
    // supportedModes = new Display.Mode[nModes];
    for (Int32 i = 0; i < nModes; i++) {
        // read Display.Mode
        Int32 modeId = ReadInt32(source);
        Int32 width = ReadInt32(source);
        Int32 height = ReadInt32(source);
        Float refreshRate = ReadFloat(source);
        if (i == 0) {
            info->mRefreshRate = refreshRate;
        }
    }
    // don't have mColorMode
    Int32 colorMode = ReadInt32(source);
    Int32 nColorModes = ReadInt32(source);
    // supportedColorModes = new int[nColorModes];
    for (Int32 i = 0; i < nColorModes; i++) {
        Int32 supportedColorMode = ReadInt32(source);
    }
    // don't have mHdrCapabilities
    AutoPtr<IInterface> hdrCapabilities = ReadParcelable(source);
    info->mLogicalDensityDpi = ReadInt32(source);
    info->mPhysicalXDpi = ReadFloat(source);
    info->mPhysicalYDpi = ReadFloat(source);
    info->mAppVsyncOffsetNanos = ReadInt64(source);
    info->mPresentationDeadlineNanos = ReadInt64(source);
    info->mState = ReadInt32(source);
    info->mOwnerUid = ReadInt32(source);
    info->mOwnerPackageName = ReadString(source);
    // don't have mUniqueId
    String uniqueId = ReadString(source);
    // don't have mRemoveMode
    Int32 removeMode = ReadInt32(source);

    return info.Get();
}

AutoPtr<IInputChannel> AndroidParcelUtils::ReadInputChannel(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(const_cast<android::Parcel*>(&source), FALSE);
    AutoPtr<IInputChannel> channel;
    CInputChannel::New((IInputChannel**)&channel);
    IParcelable::Probe(channel)->ReadFromParcel(wrapper);
    return channel;
}

AutoPtr<IInputDevice> AndroidParcelUtils::ReadInputDevice(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CInputDevice> device;
    CInputDevice::NewByFriend((CInputDevice**)&device);

    device->mId = ReadInt32(source);
    device->mGeneration = ReadInt32(source);
    device->mControllerNumber = ReadInt32(source);
    device->mName = ReadString(source);
    device->mVendorId = ReadInt32(source);
    device->mProductId = ReadInt32(source);
    device->mDescriptor = ReadString(source);
    device->mIsExternal = ReadInt32(source) != 0;
    device->mSources = ReadInt32(source);
    device->mKeyboardType = ReadInt32(source);
    device->mKeyCharacterMap = ReadKeyCharacterMap(source);
    device->mHasVibrator = ReadInt32(source) != 0;
    // don't have mHasMicrophone member
    Boolean hasMicrophone = ReadInt32(source) != 0;
    device->mHasButtonUnderPad = ReadInt32(source) != 0;
    AutoPtr<IInputDeviceIdentifier> identifier;
    CInputDeviceIdentifier::New(device->mDescriptor, device->mVendorId, device->mProductId,
            (IInputDeviceIdentifier**)&identifier);
    device->mIdentifier = identifier;

    const Int32 MAX_RANGES = 1000;
    Int32 numRanges = ReadInt32(source);
    if (numRanges > MAX_RANGES) {
        numRanges = MAX_RANGES;
    }

    for (Int32 i = 0; i < numRanges; i++) {
        device->AddMotionRange(ReadInt32(source), ReadInt32(source),
                ReadFloat(source), ReadFloat(source), ReadFloat(source),
                ReadFloat(source), ReadFloat(source));
    }

    return device.Get();
}

AutoPtr<IKeyCharacterMap> AndroidParcelUtils::ReadKeyCharacterMap(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(const_cast<android::Parcel*>(&source), FALSE);
    AutoPtr<IKeyCharacterMap> map;
    CKeyCharacterMap::New((IKeyCharacterMap**)&map);
    IParcelable::Probe(map)->ReadFromParcel(wrapper);
    return map;
}

AutoPtr<ISurface> AndroidParcelUtils::ReadSurface(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CSurface> surface;
    CSurface::NewByFriend((CSurface**)&surface);

    surface->Lock();

    surface->mName = ReadString(source);
    Boolean isSingleBuffered = ReadInt32(source) != 0;
    AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(const_cast<android::Parcel*>(&source), FALSE);
    Int64 ptr;
    surface->NativeReadFromParcel(wrapper, &ptr);
    surface->SetNativeObjectLocked(ptr);

    surface->Unlock();

    return surface.Get();
}

AutoPtr<IWindowSession> AndroidParcelUtils::ReadWindowSession(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IWindowSession> session = new IWindowSessionProxy(source.readStrongBinder());
    return session;
}

AutoPtr<IExtractedTextRequest> AndroidParcelUtils::ReadExtractedTextRequest(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<IExtractedTextRequest> request;
    CExtractedTextRequest::New((IExtractedTextRequest**)&request);
    request->SetToken(ReadInt32(source));
    request->SetFlags(ReadInt32(source));
    request->SetHintMaxLines(ReadInt32(source));
    request->SetHintMaxChars(ReadInt32(source));
    return request;
}

AutoPtr<ISpellCheckerInfo> AndroidParcelUtils::ReadSpellCheckerInfo(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CSpellCheckerInfo> info;
    CSpellCheckerInfo::NewByFriend((CSpellCheckerInfo**)&info);
    info->mLabel = ReadInt32(source);
    info->mId = ReadString(source);
    info->mSettingsActivityName = ReadString(source);
    info->mService = ReadResolveInfo(source);
    ReadSpellCheckerSubtypeList(source, IList::Probe(info->mSubtypes));
    return info.Get();
}

AutoPtr<ISpellCheckerSubtype> AndroidParcelUtils::ReadSpellCheckerSubtype(
    /* [in] */ const android::Parcel& source)
{
    AutoPtr<CSpellCheckerSubtype> subtype;
    CSpellCheckerSubtype::NewByFriend((CSpellCheckerSubtype**)&subtype);

    subtype->mSubtypeNameResId = ReadInt32(source);
    String s = ReadString(source);
    subtype->mSubtypeLocale = !s.IsNull() ? s : String("");
    s = ReadString(source);
    // don't have mSubtypeLanguageTag
    String subtypeLanguageTag = !s.IsNull() ? s : String("");
    s = ReadString(source);
    subtype->mSubtypeExtraValue = !s.IsNull() ? s : String("");
    // don't have mSubtypeId
    Int32 subtypeId = ReadInt32(source);
    subtype->mSubtypeHashCode = CSpellCheckerSubtype::HashCodeInternal(
            subtype->mSubtypeLocale, subtype->mSubtypeExtraValue);
    return subtype.Get();
}

void AndroidParcelUtils::ReadSpellCheckerSubtypeList(
    /* [in] */ const android::Parcel& source,
    /* [in] */ IList* list)
{
    Int32 M;
    list->GetSize(&M);
    Int32 N = ReadInt32(source);
    Int32 i = 0;
    for (; i < M && i < N; i++) {
        if (ReadInt32(source) != 0) {
            list->Set(i, ReadSpellCheckerSubtype(source));
        }
        else {
            list->Set(i, NULL);
        }
    }
    for (; i < N; i++) {
        if (ReadInt32(source) != 0) {
            list->Add(ReadSpellCheckerSubtype(source));
        }
        else {
            list->Add(NULL);
        }
    }
    for (; i < M; i++) {
        list->Remove(N);
    }
}

void AndroidParcelUtils::WriteInt32(
    /* [in] */ android::Parcel& data,
    /* [in] */ Int32 value)
{
    data.writeInt32(value);
}

void AndroidParcelUtils::WriteInt32Array(
    /* [in] */ android::Parcel& data,
    /* [in] */ ArrayOf<Int32>* value)
{
    if (value != NULL) {
        Int32 N = value->GetLength();
        WriteInt32(data, N);
        for (Int32 i=0; i<N; i++) {
            WriteInt32(data, (*value)[i]);
        }
    }
    else {
        WriteInt32(data, -1);
    }
}

void AndroidParcelUtils::WriteInt64(
    /* [in] */ android::Parcel& data,
    /* [in] */ Int64 value)
{
    data.writeInt64(value);
}

void AndroidParcelUtils::WriteFloat(
    /* [in] */ android::Parcel& data,
    /* [in] */ Float value)
{
    data.writeFloat(value);
}

void AndroidParcelUtils::WriteFloatArray(
    /* [in] */ android::Parcel& data,
    /* [in] */ ArrayOf<Float>* value)
{
    if (value != NULL) {
        Int32 N = value->GetLength();
        WriteInt32(data, N);
        for (Int32 i=0; i<N; i++) {
            WriteFloat(data, (*value)[i]);
        }
    }
    else {
        WriteInt32(data, -1);
    }
}

void AndroidParcelUtils::WriteString(
    /* [in] */ android::Parcel& data,
    /* [in] */ const String& value)
{
    if (!value.IsNull()) {
        android::String16 str16(value.string());
        data.writeString16(str16.string(), str16.size());
    }
    else {
        data.writeString16(NULL, 0);
    }
}

void AndroidParcelUtils::WriteStringArray(
    /* [in] */ android::Parcel& data,
    /* [in] */ ArrayOf<String>* value)
{
    if (value != NULL) {
        Int32 N = value->GetLength();
        WriteInt32(data, N);
        for (Int32 i=0; i<N; i++) {
            WriteString(data, (*value)[i]);
        }
    }
    else {
        WriteInt32(data, -1);
    }
}

void AndroidParcelUtils::WriteValue(
    /* [in] */ android::Parcel& data,
    /* [in] */ IInterface* value)
{
    if (value == NULL) {
        WriteInt32(data, VAL_NULL);
    }
    else if (IString::Probe(value) != NULL) {
        WriteInt32(data, VAL_STRING);
        String str;
        ICharSequence::Probe(value)->ToString(&str);
        WriteString(data, str);
    }
    else if (IInteger32::Probe(value) != NULL) {
        WriteInt32(data, VAL_INTEGER);
        Int32 val;
        IInteger32::Probe(value)->GetValue(&val);
        WriteInt32(data, val);
    }
    else if (IMap::Probe(value) != NULL) {
        assert(0);
    }
    else if (IBundle::Probe(value) != NULL) {
        WriteInt32(data, VAL_BUNDLE);
        WriteBundle(data, IBundle::Probe(value));
    }
    else if (IParcelable::Probe(value) != NULL) {
        WriteInt32(data, VAL_PARCELABLE);
        WriteParcelable(data, IParcelable::Probe(value));
    }
    else if (ISparseArray::Probe(value) != NULL) {
        WriteInt32(data, VAL_SPARSEARRAY);
        WriteSparseArray(data, ISparseArray::Probe(value));
    }
    else {
        assert(0);
    }
}

void AndroidParcelUtils::WriteParcelable(
    /* [in] */ android::Parcel& data,
    /* [in] */ IParcelable* value)
{
    if (value == NULL) {
        WriteString(data, String(NULL));
        return;
    }

    String name = Object::GetFullClassName(value);

    if (name.Equals("Elastos.Droid.View.CAbsSavedState")) {
        WriteString(data, String("android.view.AbsSavedState"));
        AutoPtr<IParcelable> state;
        IAbsSavedState::Probe(value)->GetSuperState((IParcelable**)&state);
        WriteParcelable(data, state);
        return;
    }
    else if (name.Equals("Elastos.Droid.Widget.CTextViewSavedState")) {
        WriteString(data, String("android.widget.TextView$SavedState"));
        Logger::D(TAG, "Write Elastos.Droid.Widget.CTextViewSavedState not implemented");
        return;
    }
    else if (name.Equals("Elastos.Droid.Widget.CCompoundButtonSavedState")) {
        WriteString(data, String("android.widget.CompoundButton$SavedState"));
        Logger::D(TAG, "Write Elastos.Droid.Widget.CCompoundButtonSavedState not implemented");
        return;
    }
    else if (name.Equals("Elastos.Droid.Widget.CAbsListViewSavedState")) {
        WriteString(data, String("android.widget.AbsListView$SavedState"));
        Logger::D(TAG, "Write Elastos.Droid.Widget.CAbsListViewSavedState not implemented");
        return;
    }
    else if (name.Equals("Elastos.Droid.View.InputMethod.CSparseRectFArray")) {
        WriteString(data, String("android.view.inputmethod.SparseRectFArray"));
        WriteSparseRectFArray(data, ISparseRectFArray::Probe(value));
        return;
    }
    else if (name.Equals("Elastos.Droid.Support.V4.View.CViewPagerSavedState")) {
        WriteString(data, String("android.support.v4.view.ViewPager$SavedState"));
        Logger::D(TAG, "Write Elastos.Droid.Support.V4.View.CViewPagerSavedState not implemented");
        return;
    }
    else {
        WriteString(data, name);
        Logger::D(TAG, "[WriteParcelable] Write %s not implemented", name.string());
    }
}

void AndroidParcelUtils::Write_CHAR_SEQUENCE(
    /* [in] */ android::Parcel& data,
    /* [in] */ ICharSequence* value)
{
    if (value == NULL) {
        WriteInt32(data, 1);
        WriteString(data, String(NULL));
        return;
    }

    if (ISpanned::Probe(value) != NULL) {
        WriteInt32(data, 0);
        String str;
        value->ToString(&str);
        WriteString(data, str);

        ISpanned* sp = ISpanned::Probe(value);
        Int32 len;
        value->GetLength(&len);
        AutoPtr< ArrayOf<IInterface*> > os;
        sp->GetSpans(0, len, EIID_IInterface, (ArrayOf<IInterface*>**)&os);

        for (Int32 i = 0; i < os->GetLength(); i++) {
            AutoPtr<IInterface> o = (*os)[i];
            AutoPtr<IInterface> prop = (*os)[i];

            if (ICharacterStyle::Probe(prop)) {
                AutoPtr<ICharacterStyle> underLying;
                ICharacterStyle::Probe(prop)->GetUnderlying((ICharacterStyle**)&underLying);
                prop = underLying;
            }

            if (IParcelableSpan::Probe(prop)) {
                IParcelableSpan* ps = IParcelableSpan::Probe(prop);
                Int32 spanTypeId;
                ps->GetSpanTypeId(&spanTypeId);
                if (spanTypeId < ITextUtils::FIRST_SPAN || spanTypeId > ITextUtils::LAST_SPAN) {
                    Logger::E(TAG, "External class \"%s\" is attempting to use the frameworks-only ParcelableSpan interface",
                            Object::GetClassName(ps).string());
                }
                else {
                    if (spanTypeId != ITextUtils::UNDERLINE_SPAN &&
                        spanTypeId != ITextUtils::SUGGESTION_SPAN) {
                        assert(0);
                    }
                    WriteInt32(data, spanTypeId);
                    AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(&data, FALSE);
                    IParcelable::Probe(ps)->WriteToParcel(wrapper);
                    WriteWhere(data, sp, o);
                }
            }
        }

        WriteInt32(data, 0);
    }
    else {
        WriteInt32(data, 1);
        String str;
        value->ToString(&str);
        WriteString(data, str);
    }
}

void AndroidParcelUtils::WriteActivityManagerTaskDescription(
    /* [in] */ android::Parcel& data,
    /* [in] */ IActivityManagerTaskDescription* value)
{
    String label;
    value->GetLabel(&label);
    if (label.IsNull()) {
        WriteInt32(data, 0);
    }
    else {
        WriteInt32(data, 1);
        WriteString(data, label);
    }
    AutoPtr<IBitmap> icon;
    value->GetInMemoryIcon((IBitmap**)&icon);
    if (icon == NULL) {
        WriteInt32(data, 0);
    }
    else {
        WriteInt32(data, 1);
        WriteBitmap(data, icon);
    }
    Int32 colorPrimary;
    value->GetPrimaryColor(&colorPrimary);
    WriteInt32(data, colorPrimary);
    Int32 colorBackground = IColor::WHITE;
    WriteInt32(data, colorBackground);
    Int32 statusBarColor = IColor::BLACK;
    WriteInt32(data, statusBarColor);
    Int32 navigationBarColor = IColor::BLACK;
    WriteInt32(data, navigationBarColor);
    String iconFilename;
    value->GetIconFilename(&iconFilename);
    if (iconFilename.IsNull()) {
        WriteInt32(data, 0);
    }
    else {
        WriteInt32(data, 1);
        WriteString(data, iconFilename);
    }
}

void AndroidParcelUtils::WriteIApplicationThread(
    /* [in] */ android::Parcel& data,
    /* [in] */ IApplicationThread* app)
{
    android::sp<android::IBinder> appBinder;
    if (app != NULL) {
        appBinder = ((CApplicationThread*)app)->mBBinder;
        if (appBinder == NULL) {
            appBinder = new IApplicationThreadStub(app);
            ((CApplicationThread*)app)->mBBinder = appBinder.get();
        }
    }
    data.writeStrongBinder(appBinder);
}

void AndroidParcelUtils::WriteContentProviderHolderList(
    /* [in] */ android::Parcel& data,
    /* [in] */ IList* value)
{
    if (value == NULL) {
        WriteInt32(data, -1);
        return;
    }
    Int32 N;
    value->GetSize(&N);
    Int32 i = 0;
    WriteInt32(data, N);
    while (i < N) {
        assert(0);
    }
}

void AndroidParcelUtils::WriteConfiguration(
    /* [in] */ android::Parcel& data,
    /* [in] */ IConfiguration* config)
{
    CConfiguration* configObj = (CConfiguration*)config;

    WriteFloat(data, configObj->mFontScale);
    WriteInt32(data, configObj->mMcc);
    WriteInt32(data, configObj->mMnc);

    WriteInt32(data, 1);
    String langTag;
    configObj->mLocale->ToLanguageTag(&langTag);
    WriteString(data, langTag);

    if (configObj->mUserSetLocale) {
        WriteInt32(data, 1);
    }
    else {
        WriteInt32(data, 0);
    }
    WriteInt32(data, configObj->mTouchscreen);
    WriteInt32(data, configObj->mKeyboard);
    WriteInt32(data, configObj->mKeyboardHidden);
    WriteInt32(data, configObj->mHardKeyboardHidden);
    WriteInt32(data, configObj->mNavigation);
    WriteInt32(data, configObj->mNavigationHidden);
    WriteInt32(data, configObj->mOrientation);
    WriteInt32(data, configObj->mScreenLayout);
    // don't have mColorMode
    Int32 colorMode = 0;
    WriteInt32(data, colorMode);
    WriteInt32(data, configObj->mUiMode);
    WriteInt32(data, configObj->mScreenWidthDp);
    WriteInt32(data, configObj->mScreenHeightDp);
    WriteInt32(data, configObj->mSmallestScreenWidthDp);
    WriteInt32(data, configObj->mDensityDpi);
    WriteInt32(data, configObj->mCompatScreenWidthDp);
    WriteInt32(data, configObj->mCompatScreenHeightDp);
    WriteInt32(data, configObj->mCompatSmallestScreenWidthDp);
    // don't have mAppBounds
    AutoPtr<IRect> appBounds;
    WriteValue(data, appBounds);
    // don't have mAssetsSeq
    Int32 assetsSeq = 0;
    WriteInt32(data, assetsSeq);
    WriteInt32(data, configObj->mSeq);
}

void AndroidParcelUtils::WriteIIContentObserver(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIContentObserver* observer)
{
    android::sp<android::IBinder> obBinder;
    if (observer != NULL) {
        obBinder = ((CContentObserverTransport*)observer)->mBBinder;
        if (obBinder == NULL) {
            obBinder = new IContentObserverStub(observer);
            ((CContentObserverTransport*)observer)->mBBinder = obBinder.get();
        }
    }
    data.writeStrongBinder(obBinder);
}

void AndroidParcelUtils::WriteBitmap(
    /* [in] */ android::Parcel& data,
    /* [in] */ IBitmap* value)
{
    assert(0);
}

void AndroidParcelUtils::WriteRect(
    /* [in] */ android::Parcel& data,
    /* [in] */ IRect* value)
{
    Int32 left, top, right, bottom;
    value->Get(&left, &top, &right, &bottom);
    WriteInt32(data, left);
    WriteInt32(data, top);
    WriteInt32(data, right);
    WriteInt32(data, bottom);
}

void AndroidParcelUtils::WriteIIDisplayManagerCallback(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIDisplayManagerCallback* cb)
{
    android::sp<android::IBinder> cbBinder;
    if (cb != NULL) {
        cbBinder = ((CDisplayManagerCallback*)cb)->mBBinder;
        if (cbBinder == NULL) {
            cbBinder = new IDisplayManagerCallbackStub(cb);
            ((CDisplayManagerCallback*)cb)->mBBinder = cbBinder.get();
        }
    }
    data.writeStrongBinder(cbBinder);
}

void AndroidParcelUtils::WriteIInputDevicesChangedListener(
    /* [in] */ android::Parcel& data,
    /* [in] */ IInputDevicesChangedListener* listener)
{
    android::sp<android::IBinder> listenerBinder;
    if (listener != NULL) {
        listenerBinder = ((CInputManagerInputDevicesChangedListener*)listener)->mBBinder;
        if (listenerBinder == NULL) {
            listenerBinder = new IInputDevicesChangedListenerStub(listener);
            ((CInputManagerInputDevicesChangedListener*)listener)->mBBinder = listenerBinder.get();
        }
    }
    data.writeStrongBinder(listenerBinder);
}

void AndroidParcelUtils::WriteIIInputContext(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIInputContext* inputContext)
{
    android::sp<android::IBinder> ctxBinder;
    if (inputContext != NULL) {
        ctxBinder = ((IInputConnectionWrapper*)inputContext)->mBBinder;
        if (ctxBinder == NULL) {
            ctxBinder = new IInputContextStub(inputContext);
            ((IInputConnectionWrapper*)inputContext)->mBBinder = ctxBinder.get();
        }
    }
    data.writeStrongBinder(ctxBinder);
}

void AndroidParcelUtils::WriteIInputMethodClient(
    /* [in] */ android::Parcel& data,
    /* [in] */ IInputMethodClient* client)
{
    android::sp<android::IBinder> clientBinder;
    if (client != NULL) {
        clientBinder = ((CIInputMethodClient*)client)->mBBinder;
        if (clientBinder == NULL) {
            clientBinder = new IInputMethodClientStub(client);
            ((CIInputMethodClient*)client)->mBBinder = clientBinder.get();
        }
    }
    data.writeStrongBinder(clientBinder);
}

void AndroidParcelUtils::WriteUri(
    /* [in] */ android::Parcel& data,
    /* [in] */ IUri* value)
{
    if (value == NULL) {
        WriteInt32(data, 0);
    }
    else {
        if (IStringUri::Probe(value) != NULL) {
            assert(0);
        }
        else if (IOpaqueUri::Probe(value) != NULL) {
            assert(0);
        }
        else if (IHierarchicalUri::Probe(value) != NULL) {
            HierarchicalUri* uriObj = (HierarchicalUri*)IHierarchicalUri::Probe(value);
            WriteInt32(data, 3);
            WriteString(data, uriObj->mScheme);
            WriteAbstractPart(data, uriObj->mAuthority);
            WriteAbstractPart(data, uriObj->mPath);
            WriteAbstractPart(data, uriObj->mQuery);
            WriteAbstractPart(data, uriObj->mFragment);
        }
        else {
            assert(0);
        }
    }
}

void AndroidParcelUtils::WriteStrongBinder(
    /* [in] */ android::Parcel& data,
    /* [in] */ IBinder* value)
{
    android::sp<android::IBinder> b = IBinderForDroidObject(value);
    data.writeStrongBinder(b);
}

void AndroidParcelUtils::WriteBundle(
    /* [in] */ android::Parcel& data,
    /* [in] */ IBundle* value)
{
    if (value == NULL) {
        WriteInt32(data, -1);
        return;
    }

    CBundle* bundle = (CBundle*)value;

    if (bundle->mParcelledData != NULL) {
        if (bundle->mParcelledData == CBundle::EMPTY_PARCEL) {
            WriteInt32(data, 0);
        }
        else {
            assert(0);
        }
        return;
    }

    Int32 size;
    if (bundle->mMap == NULL || (bundle->mMap->GetSize(&size), size <= 0)) {
        WriteInt32(data, 0);
        return;
    }
    Int32 lengthPos = data.dataPosition();
    WriteInt32(data, -1);
    WriteInt32(data, CBundle::BUNDLE_MAGIC);

    Int32 startPos = data.dataPosition();
    WriteArrayMapInternal(data, bundle->mMap);
    int endPos = data.dataPosition();

    data.setDataPosition(lengthPos);
    Int32 length = endPos - startPos;
    data.writeInt32(length);
    data.setDataPosition(endPos);
}

void AndroidParcelUtils::WritePersistableBundle(
    /* [in] */ android::Parcel& data,
    /* [in] */ IPersistableBundle* value)
{
    assert(0);
}

void AndroidParcelUtils::WriteResultReceiver(
    /* [in] */ android::Parcel& data,
    /* [in] */ IResultReceiver* value)
{
    assert(0);
}

void AndroidParcelUtils::WriteSparseArray(
    /* [in] */ android::Parcel& data,
    /* [in] */ ISparseArray* val)
{
    if (val == NULL) {
        WriteInt32(data, -1);
        return;
    }
    Int32 N;
    val->GetSize(&N);
    WriteInt32(data, N);
    Int32 i = 0;
    while (i < N) {
        Int32 index;
        val->KeyAt(i, &index);
        WriteInt32(data, index);
        AutoPtr<IInterface> value;
        val->ValueAt(i, (IInterface**)&value);
        WriteValue(data, value);
        i++;
    }
}

void AndroidParcelUtils::WriteIIGraphicsStatsCallback(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIGraphicsStatsCallback* cb)
{
    android::sp<android::IBinder> cbBinder;
    if (cb != NULL) {
        cbBinder = (ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub*)cb;
    }
    data.writeStrongBinder(cbBinder);
}

void AndroidParcelUtils::WriteIIWindow(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIWindow* window)
{
    android::sp<android::IBinder> winBinder;
    if (window != NULL) {
        winBinder = ((CViewRootImplW*)window)->mBBinder;
        if (winBinder == NULL) {
            winBinder = new IWindowStub(window);
            ((CViewRootImplW*)window)->mBBinder = winBinder.get();
        }
    }
    data.writeStrongBinder(winBinder);
}

void AndroidParcelUtils::WriteIIWindowSessionCallback(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIWindowSessionCallback* cb)
{
    android::sp<android::IBinder> cbBinder;
    if (cb != NULL) {
        cbBinder = ((CWindowManagerGlobalSessionCallback*)cb)->mBBinder;
        if (cbBinder == NULL) {
            cbBinder = new IWindowSessionCallbackStub(cb);
            ((CWindowManagerGlobalSessionCallback*)cb)->mBBinder = cbBinder.get();
        }
    }
    data.writeStrongBinder(cbBinder);
}

void AndroidParcelUtils::WriteWindowManagerLayoutParams(
    /* [in] */ android::Parcel& data,
    /* [in] */ IWindowManagerLayoutParams* params)
{
    Int32 i32Val;
    IViewGroupLayoutParams::Probe(params)->GetWidth(&i32Val);
    WriteInt32(data, i32Val);
    IViewGroupLayoutParams::Probe(params)->GetHeight(&i32Val);
    WriteInt32(data, i32Val);
    params->GetX(&i32Val);
    WriteInt32(data, i32Val);
    params->GetY(&i32Val);
    WriteInt32(data, i32Val);
    params->GetType(&i32Val);
    WriteInt32(data, i32Val);
    params->GetFlags(&i32Val);
    WriteInt32(data, i32Val);
    params->GetPrivateFlags(&i32Val);
    WriteInt32(data, i32Val);
    params->GetSoftInputMode(&i32Val);
    WriteInt32(data, i32Val);
    params->GetGravity(&i32Val);
    WriteInt32(data, i32Val);
    Float fVal;
    params->GetHorizontalMargin(&fVal);
    WriteFloat(data, fVal);
    params->GetVerticalMargin(&fVal);
    WriteFloat(data, fVal);
    params->GetFormat(&i32Val);
    WriteInt32(data, i32Val);
    params->GetWindowAnimations(&i32Val);
    WriteInt32(data, i32Val);
    params->GetAlpha(&fVal);
    WriteFloat(data, fVal);
    params->GetDimAmount(&fVal);
    WriteFloat(data, fVal);
    params->GetScreenBrightness(&fVal);
    WriteFloat(data, fVal);
    params->GetButtonBrightness(&fVal);
    WriteFloat(data, fVal);
    params->GetRotationAnimation(&i32Val);
    WriteInt32(data, i32Val);
    AutoPtr<IBinder> token;
    params->GetToken((IBinder**)&token);
    if (IIWindow::Probe(token) != NULL) {
        WriteIIWindow(data, IIWindow::Probe(token));
    }
    else {
        WriteStrongBinder(data, token);
    }
    String packageName;
    params->GetPackageName(&packageName);
    WriteString(data, packageName);
    AutoPtr<ICharSequence> title;
    params->GetTitle((ICharSequence**)&title);
    Write_CHAR_SEQUENCE(data, title);
    params->GetScreenOrientation(&i32Val);
    WriteInt32(data, i32Val);
    params->GetPreferredRefreshRate(&fVal);
    WriteFloat(data, fVal);
    Int32 preferredDisplayModeId = 0;
    WriteInt32(data, preferredDisplayModeId);
    params->GetSystemUiVisibility(&i32Val);
    WriteInt32(data, i32Val);
    params->GetSubtreeSystemUiVisibility(&i32Val);
    WriteInt32(data, i32Val);
    Boolean bVal;
    params->GetHasSystemUiListeners(&bVal);
    WriteInt32(data, bVal ? 1 : 0);
    params->GetInputFeatures(&i32Val);
    WriteInt32(data, i32Val);
    Int64 i64Val;
    params->GetUserActivityTimeout(&i64Val);
    WriteFloat(data, i64Val);
    AutoPtr<IRect> surfaceInsets;
    params->GetSurfaceInsets((IRect**)&surfaceInsets);
    Int32 left, top, right, bottom;
    surfaceInsets->Get(&left, &top, &right, &bottom);
    WriteInt32(data, left);
    WriteInt32(data, top);
    WriteInt32(data, right);
    WriteInt32(data, bottom);
    Boolean hasManualSurfaceInsets = FALSE;
    WriteInt32(data, hasManualSurfaceInsets ? 1 : 0);
    Boolean preservePreviousSurfaceInsets = TRUE;
    WriteInt32(data, preservePreviousSurfaceInsets ? 1 : 0);
    Int32 needsMenuKey = 0;
    WriteInt32(data, needsMenuKey);
    Int32 accessibilityIdOfAnchor = -1;
    WriteInt32(data, accessibilityIdOfAnchor);
    AutoPtr<ICharSequence> accessibilityTitle;
    Write_CHAR_SEQUENCE(data, accessibilityTitle);
    Int64 hideTimeoutMilliseconds = -1;
    WriteInt64(data, hideTimeoutMilliseconds);
}

void AndroidParcelUtils::WriteAccessibilityEvent(
    /* [in] */ android::Parcel& data,
    /* [in] */ IAccessibilityEvent* event)
{
    CAccessibilityEvent* eventObj = (CAccessibilityEvent*)event;

    WriteInt32(data, eventObj->IsSealed() ? 1 : 0);
    WriteInt32(data, eventObj->mEventType);
    WriteInt32(data, eventObj->mMovementGranularity);
    WriteInt32(data, eventObj->mAction);
    WriteInt32(data, eventObj->mContentChangeTypes);
    Write_CHAR_SEQUENCE(data, eventObj->mPackageName);
    WriteInt64(data, eventObj->mEventTime);
    WriteInt32(data, eventObj->mConnectionId);

    WriteInt32(data, eventObj->mBooleanProperties);
    WriteInt32(data, eventObj->mCurrentItemIndex);
    WriteInt32(data, eventObj->mItemCount);
    WriteInt32(data, eventObj->mFromIndex);
    WriteInt32(data, eventObj->mToIndex);
    WriteInt32(data, eventObj->mScrollX);
    WriteInt32(data, eventObj->mScrollY);
    WriteInt32(data, eventObj->mMaxScrollX);
    WriteInt32(data, eventObj->mMaxScrollY);
    WriteInt32(data, eventObj->mAddedCount);
    WriteInt32(data, eventObj->mRemovedCount);
    Write_CHAR_SEQUENCE(data, eventObj->mClassName);
    Write_CHAR_SEQUENCE(data, eventObj->mContentDescription);
    Write_CHAR_SEQUENCE(data, eventObj->mBeforeText);
    // TODO: parcel.writeParcelable(record.mParcelableData, flags);
    WriteString(data, String(NULL));
    // TODO: parcel.writeList(record.mText);
    WriteInt32(data, -1);
    WriteInt32(data, eventObj->mSourceWindowId);
    // create copy of the node here because the node would be recycled just after it is written
    // to parcel
    // TODO: parcel.writeParcelable(record.mSourceNode != null ?
    //         AccessibilityNodeInfo.obtain(record.mSourceNode) : null, flags);
    WriteString(data, String(NULL));
    WriteInt32(data, eventObj->mSealed ? 1 : 0);

    // TODO:
    // Write the records.
    // final int recordCount = getRecordCount();
    // parcel.writeInt(recordCount);
    // for (int i = 0; i < recordCount; i++) {
    //     AccessibilityRecord record = mRecords.get(i);
    //     writeAccessibilityRecordToParcel(record, parcel, flags);
    // }
    WriteInt32(data, 0);
}

void AndroidParcelUtils::WriteIIAccessibilityInteractionConnection(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIAccessibilityInteractionConnection* connection)
{
    android::sp<android::IBinder> connBinder;
    if (connection != NULL) {
        connBinder = ((CAccessibilityInteractionConnection*)connection)->mBBinder;
        if (connBinder == NULL) {
            connBinder = new IAccessibilityInteractionConnectionStub(connection);
            ((CAccessibilityInteractionConnection*)connection)->mBBinder = connBinder.get();
        }
    }
    data.writeStrongBinder(connBinder);
}

void AndroidParcelUtils::WriteIIAccessibilityManagerClient(
    /* [in] */ android::Parcel& data,
    /* [in] */ IIAccessibilityManagerClient* client)
{
    android::sp<android::IBinder> clientBinder;
    if (client != NULL) {
        clientBinder = ((CAccessibilityManagerClient*)client)->mBBinder;
        if (clientBinder == NULL) {
            clientBinder = new IAccessibilityManagerClientStub(client);
            ((CAccessibilityManagerClient*)client)->mBBinder = clientBinder.get();
        }
    }
    data.writeStrongBinder(clientBinder);
}

void AndroidParcelUtils::WriteCursorAnchorInfo(
    /* [in] */ android::Parcel& data,
    /* [in] */ ICursorAnchorInfo* info)
{
    CCursorAnchorInfo* infoObj = (CCursorAnchorInfo*)info;
    Int32 hashCode;
    infoObj->GetHashCode(&hashCode);
    WriteInt32(data, hashCode);
    WriteInt32(data, infoObj->mSelectionStart);
    WriteInt32(data, infoObj->mSelectionEnd);
    WriteInt32(data, infoObj->mComposingTextStart);
    Write_CHAR_SEQUENCE(data, infoObj->mComposingText);
    WriteInt32(data, infoObj->mInsertionMarkerFlags);
    WriteFloat(data, infoObj->mInsertionMarkerHorizontal);
    WriteFloat(data, infoObj->mInsertionMarkerTop);
    WriteFloat(data, infoObj->mInsertionMarkerBaseline);
    WriteFloat(data, infoObj->mInsertionMarkerBottom);
    WriteParcelable(data, IParcelable::Probe(infoObj->mCharacterBoundsArray));
    AutoPtr<ArrayOf<Float> > matrixArray = ArrayOf<Float>::Alloc(9);
    infoObj->mMatrix->GetValues(matrixArray);
    WriteFloatArray(data, matrixArray);
}

void AndroidParcelUtils::WriteEditorInfo(
    /* [in] */ android::Parcel& data,
    /* [in] */ IEditorInfo* value)
{
    Int32 i32Val;
    value->GetInputType(&i32Val);
    WriteInt32(data, i32Val);
    value->GetImeOptions(&i32Val);
    WriteInt32(data, i32Val);
    String strVal;
    value->GetPrivateImeOptions(&strVal);
    WriteString(data, strVal);
    AutoPtr<ICharSequence> actionLabel;
    value->GetActionLabel((ICharSequence**)&actionLabel);
    Write_CHAR_SEQUENCE(data, actionLabel);
    value->GetActionId(&i32Val);
    WriteInt32(data, i32Val);
    value->GetInitialSelStart(&i32Val);
    WriteInt32(data, i32Val);
    value->GetInitialSelEnd(&i32Val);
    WriteInt32(data, i32Val);
    value->GetInitialCapsMode(&i32Val);
    WriteInt32(data, i32Val);
    AutoPtr<ICharSequence> hintText;
    value->GetHintText((ICharSequence**)&hintText);
    Write_CHAR_SEQUENCE(data, hintText);
    AutoPtr<ICharSequence> label;
    value->GetLabel((ICharSequence**)&label);
    Write_CHAR_SEQUENCE(data, label);
    value->GetPackageName(&strVal);
    WriteString(data, strVal);
    value->GetFieldId(&i32Val);
    WriteInt32(data, i32Val);
    value->GetFieldName(&strVal);
    WriteString(data, strVal);
    AutoPtr<IBundle> extras;
    value->GetExtras((IBundle**)&extras);
    WriteBundle(data, extras);
    // LocaleList.getEmptyLocaleList().writeToParcel(dest, flags);
    WriteString(data, String(""));
    // dest.writeStringArray(contentMimeTypes);
    WriteInt32(data, -1);
}

void AndroidParcelUtils::WriteExtractedText(
    /* [in] */ android::Parcel& data,
    /* [in] */ IExtractedText* value)
{
    AutoPtr<ICharSequence> text;
    value->GetText((ICharSequence**)&text);
    Write_CHAR_SEQUENCE(data, text);
    Int32 i32Val;
    value->GetStartOffset(&i32Val);
    WriteInt32(data, i32Val);
    value->GetPartialStartOffset(&i32Val);
    WriteInt32(data, i32Val);
    value->GetPartialEndOffset(&i32Val);
    WriteInt32(data, i32Val);
    value->GetSelectionStart(&i32Val);
    WriteInt32(data, i32Val);
    value->GetSelectionEnd(&i32Val);
    WriteInt32(data, i32Val);
    value->GetFlags(&i32Val);
    WriteInt32(data, i32Val);
}

void AndroidParcelUtils::WriteSparseRectFArray(
    /* [in] */ android::Parcel& data,
    /* [in] */ ISparseRectFArray* value)
{
    CSparseRectFArray* array = (CSparseRectFArray*)value;
    WriteInt32Array(data, array->mKeys);
    WriteFloatArray(data, array->mCoordinates);
    WriteInt32Array(data, array->mFlagsArray);
}

void AndroidParcelUtils::WriteSuggestionSpanArray(
    /* [in] */ android::Parcel& data,
    /* [in] */ ArrayOf<ISuggestionSpan*>* value)
{
    if (value != NULL) {
        Int32 N = value->GetLength();
        WriteInt32(data, N);
        for (Int32 i = 0; i < N; i++) {
            ISuggestionSpan* span = (*value)[i];
            if (span != NULL) {
                WriteInt32(data, 1);
                AutoPtr<IParcel> wrapper = new AndroidParcelWrapper(&data, FALSE);
                IParcelable::Probe(span)->WriteToParcel(wrapper);
            }
            else {
                WriteInt32(data, 0);
            }
        }
    }
    else {
        WriteInt32(data, -1);
    }
}

void AndroidParcelUtils::ReadMapInternal(
    /* [in] */ const android::Parcel& source,
    /* [in] */ IHashMap* map,
    /* [in] */ Int32 N)
{
    while (N > 0) {
        AutoPtr<IInterface> key = ReadValue(source);
        AutoPtr<IInterface> value = ReadValue(source);
        AutoPtr<IInterface> oldValue;
        map->Put(key, value, (IInterface**)&oldValue);
        N--;
    }
}

AutoPtr< ArrayOf<ILocale*> > AndroidParcelUtils::ForLanguageTags(
    /* [in] */ const String& languageTags)
{
    assert(0);
    return NULL;
}

void AndroidParcelUtils::ReadComponentInfo(
    /* [in] */ const android::Parcel& source,
    /* [in] */ ComponentInfo* info)
{
    Boolean hasApplicationInfo = ReadInt32(source) != 0;
    if (hasApplicationInfo) {
        info->mApplicationInfo = ReadApplicationInfo(source);
    }
    info->mProcessName = ReadString(source);
    // don't have mSplitName
    String splitName = ReadString(source);
    info->mDescriptionRes = ReadInt32(source);
    info->mEnabled = ReadInt32(source) != 0;
    info->mExported = ReadInt32(source) != 0;
    // don't have mEncryptionAware and mDirectBootAware
    Boolean encryptionAware, directBootAware;
    encryptionAware = directBootAware = ReadInt32(source) != 0;
}

void AndroidParcelUtils::ReadPackageItemInfo(
    /* [in] */ const android::Parcel& source,
    /* [in] */ PackageItemInfo* info)
{
    info->mName = ReadString(source);
    info->mPackageName = ReadString(source);
    info->mLabelRes = ReadInt32(source);
    info->mNonLocalizedLabel = Read_CHAR_SEQUENCE(source);
    info->mIcon = ReadInt32(source);
    info->mLogo = ReadInt32(source);
    info->mMetaData = ReadBundle(source);
    info->mBanner = ReadInt32(source);
    info->mShowUserIcon = ReadInt32(source);
}

void AndroidParcelUtils::WriteAbstractPart(
    /* [in] */ android::Parcel& data,
    /* [in] */ AbstractPart* value)
{
    Boolean hasEncoded = !value->mEncoded.Equals(AbstractPart::NOT_CACHED);
    Boolean hasDecoded = !value->mDecoded.Equals(AbstractPart::NOT_CACHED);

    if (hasEncoded && hasDecoded) {
        WriteInt32(data, 0);
        WriteString(data, value->mEncoded);
        WriteString(data, value->mDecoded);
    }
    else if (hasEncoded) {
        WriteInt32(data, 1);
        WriteString(data, value->mEncoded);
    }
    else if (hasDecoded) {
        WriteInt32(data, 2);
        WriteString(data, value->mDecoded);
    }
}

LayoutAlignment AndroidParcelUtils::GetLayoutAlignmentFromString(
    /* [in] */ const String& alignmentStr)
{
    if (alignmentStr.Equals("ALIGN_NORMAL")) {
        return ALIGN_NORMAL;
    }
    else if (alignmentStr.Equals("ALIGN_OPPOSITE")) {
        return ALIGN_OPPOSITE;
    }
    else if (alignmentStr.Equals("ALIGN_CENTER")) {
        return ALIGN_CENTER;
    }
    else if (alignmentStr.Equals("ALIGN_LEFT")) {
        return ALIGN_LEFT;
    }
    else if (alignmentStr.Equals("ALIGN_RIGHT")) {
        return ALIGN_RIGHT;
    }
    return ALIGN_NONE;
}

void AndroidParcelUtils::ReadSpan(
    /* [in] */ const android::Parcel& source,
    /* [in] */ ISpannable* sp,
    /* [in] */ IInterface* o)
{
    Int32 start = ReadInt32(source);
    Int32 end = ReadInt32(source);
    Int32 flags = ReadInt32(source);
    sp->SetSpan(o, start, end, flags);
}

void AndroidParcelUtils::WriteArrayMapInternal(
    /* [in] */ android::Parcel& data,
    /* [in] */ IArrayMap* val)
{
    if (val == NULL) {
        WriteInt32(data, -1);
        return;
    }
    Int32 N;
    val->GetSize(&N);
    WriteInt32(data, N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> key, value;
        val->GetKeyAt(i, (IInterface**)&key);
        String keyStr;
        ICharSequence::Probe(key)->ToString(&keyStr);
        WriteString(data, keyStr);
        val->GetValueAt(i, ((IInterface**)&value));
        WriteValue(data, value);
    }
}

void AndroidParcelUtils::WriteWhere(
    /* [in] */ android::Parcel& data,
    /* [in] */ ISpanned* sp,
    /* [in] */ IInterface* o)
{
    Int32 start, end, flags;
    sp->GetSpanStart(o, &start);
    sp->GetSpanEnd(o, &end);
    sp->GetSpanFlags(o, &flags);
    WriteInt32(data, start);
    WriteInt32(data, end);
    WriteInt32(data, flags);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
