
#ifndef __ELASTOSGECKOWRAPPER_H__
#define __ELASTOSGECKOWRAPPER_H__

//#include "Elastos.Framework.Media.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

typedef void* eobject;

typedef struct GeckoEventGetAction
{
    //IPoint
    int (*IPointGetX)(eobject eobj);
    int (*IPointGetY)(eobject eobj);

    //IRect
    int (*IRectGetBottom)(eobject eobj);
    int (*IRectGetLeft)(eobject eobj);
    int (*IRectGetTop)(eobject eobj);
    int (*IRectGetRight)(eobject eobj);

    //GeckoSurfaceView
    int (*GeckoSurfaceViewBeginDrawing)(eobject eobj);
    eobject (*GeckoSurfaceViewGetSoftwareDrawBitmap)(eobject eobj);
    eobject (*GeckoSurfaceViewGetSoftwareDrawBuffer)(eobject eobj);
    void (*GeckoSurfaceViewEndDrawing)(eobject eobj);
    void (*GeckoSurfaceViewDraw2D)(eobject eobj, eobject bitmap, int width, int height);
    void (*GeckoSurfaceViewDraw2DEx)(eobject eobj, eobject buffer, int stride);
    eobject (*GeckoSurfaceViewGetSurfaceHolder)(eobject eobj);
    eobject (*GeckoSurfaceViewGetSurface)(eobject eobj);

    //ILocation
    double (*ILocationGetLatitude)(eobject eobj);
    double (*ILocationGetLongitude)(eobject eobj);
    double (*ILocationGetAltitude)(eobject eobj);
    float (*ILocationGetAccuracy)(eobject eobj);
    float (*ILocationGetBearing)(eobject eobj);
    float (*ILocationGetSpeed)(eobject eobj);
    long long (*ILocationGetTime)(eobject eob);

    //IAddress
    const char* (*IAddressGetAdminArea)(eobject eobj);
    const char* (*IAddressGetCountryCode)(eobject eobj);
    const char* (*IAddressGetCountryName)(eobject eobj);
    const char* (*IAddressGetFeatureName)(eobject eobj);
    const char* (*IAddressGetLocality)(eobject eobj);
    const char* (*IAddressGetPostalCode)(eobject eobj);
    const char* (*IAddressGetPremises)(eobject eobj);
    const char* (*IAddressGetSubAdminArea)(eobject eobj);
    const char* (*IAddressGetSubLocality)(eobject eobj);
    const char* (*IAddressGetSubThoroughfare)(eobject eobj);
    const char* (*IAddressGetThoroughfare)(eobject eobj);

    //GeckoEvent
    int (*GeckoEventGetAction)(eobject eobj);
    int (*GeckoEventGetType)(eobject eobj);
    long long (*GeckoEventGetTime)(eobject eobj);
    eobject (*GeckoEventGetPointArray)(eobject eobj);
    eobject (*GeckoEventGetPointRadiiArray)(eobject eobj);
    eobject (*GeckoEventGetPointIndiciesArray)(eobject eobj);
    eobject (*GeckoEventGetRect)(eobject eobj);
    int (*GeckoEventGetFlags)(eobject eobj);
    int (*GeckoEventGetMetaState)(eobject eobj);
    int (*GeckoEventGetKeyCode)(eobject eobj);
    int (*GeckoEventGetUnicodeChar)(eobject eobj);
    int (*GeckoEventGetOffset)(eobject eobj);
    int (*GeckoEventGetCount)(eobject eobj);
    int (*GeckoEventGetPointerIndex)(eobject eobj);
    int (*GeckoEventGetRangeType)(eobject eobj);
    int (*GeckoEventGetRangeStyles)(eobject eobj);
    int (*GeckoEventGetRangeForeColor)(eobject eobj);
    int (*GeckoEventGetRangeBackColor)(eobject eobj);
    double (*GeckoEventGetAlpha)(eobject eobj);
    double (*GeckoEventGetBeta)(eobject eobj);
    double (*GeckoEventGetGamma)(eobject eobj);
    double (*GeckoEventGetX)(eobject eobj);
    double (*GeckoEventGetY)(eobject eobj);
    double (*GeckoEventGetZ)(eobject eobj);
    double (*GeckoEventGetBandwidth)(eobject eobj);
    double (*GeckoEventGetDistance)(eobject eobj);
    bool (*GeckoEventGetCanBeMetered)(eobject eobj);

    const char* (*GeckoEventGetCharacters)(eobject eobj);
    const char* (*GeckoEventGetCharactersExtra)(eobject eobj);
    eobject (*GeckoEventGetLocation)(eobject eobj);
    eobject (*GeckoEventGetAddress)(eobject eobj);

    //GeckoAppShell
    void (*GeckoAppShellNotifyIME)(eobject eobj, int type, int state);
    void (*GeckoAppShellNotifyIMEEnabled)(eobject eobj,
                                int state, const char* typeHint,
                                const char* actionHint, bool landscapeFS);
    void (*GeckoAppShellNotifyIMEChange)(eobject eobj,
                                const char* text, int start, int end, int newEnd);
    void (*GeckoAppShellNotifyScreenShot)(eobject data, int tabId, int width, int height);
    void (*GeckoAppShellAcknowledgeEventSync)(eobject eobj);
    void (*GeckoAppShellEnableDeviceMotion)(eobject eobj, bool enable);
    void (*GeckoAppShellEnableLocation)(eobject eobj, bool enable);
    void (*GeckoAppShellEnableSensor)(eobject eobj, int aSensortype);
    void (*GeckoAppShellDisableSensor)(eobject eobj, int aSensortype);
    void (*GeckoAppShellReturnIMEQueryResult)(eobject eobj,
                                const char* result, int selStart, int selLen);
    void (*GeckoAppShellOnAppShellReady)(eobject eobj);
    void (*GeckoAppShellScheduleRestart)(eobject eobj);
    void (*GeckoAppShellCreateShortcut)(eobject eobj,
                                const char* aTitle, const char* aURI, const char* aIconData, const char* aType);
    void (*GeckoAppShellOnXreExit)(eobject eobj);
    bool (*GeckoAppShellOpenUriExternal)(eobject eobj,
                                const char* uriSpec, const char* mimeType,
                                const char* packageName, const char* className,
                                const char* action, const char* title);
    const char* (*GeckoAppShellGetMimeTypeFromExtensions)(
                                eobject eobj, const char* fileExt);
    const char* (*GeckoAppShellGetExtensionFromMimeType)(
                                eobject eobj, const char* mimeType);
    void (*GeckoAppShellMoveTaskToBack)(eobject eobj);
    const char* (*GeckoAppShellGetClipboardText)(eobject eobj);
    void (*GeckoAppShellSetClipboardText)(eobject eobj, const char* text);
    void (*GeckoAppShellShowAlertNotification)(eobject eobj, const char* imageUrl,
                                const char* alertTitle, const char* alertText,
                                const char* alertCookie, const char* alertName);
    void (*GeckoAppShellAlertsProgressListener_OnProgress)(
                                eobject eobj, const char* alertName, long long progress,
                                long long progressMax, const char* alertText);
    void (*GeckoAppShellAlertsProgressListener_OnCancel)(
                                eobject eobj, const char* alertName);
    int (*GeckoAppShellGetDPI)(eobject eobj);
    const char* (*GeckoAppShellShowFilePicker)(eobject eobj, const char* filters);
    void (*GeckoAppShellSetFullScreen)(eobject eobj, bool fullScreen);
    void (*GeckoAppShellHideProgressDialog)(eobject eobj);
    void (*GeckoAppShellPerformHapticFeedback)(eobject eobj, bool isLongPress);
    void (*GeckoAppShellVibrate1)(eobject eobj, long long milliseconds);
    void (*GeckoAppShellVibrate2)(eobject eobj, long long *pattern, int size, int repeat);
    void (*GeckoAppShellCancelVibrate)(eobject eobj);
    bool (*GeckoAppShellIsNetworkLinkUp)(eobject eobj);
    bool (*GeckoAppShellIsNetworkLinkKnown)(eobject eobj);
    void (*GeckoAppShellSetSelectedLocale)(eobject eobj, const char* aLocale);
    void (*GeckoAppShellShowInputMethodPicker)(eobject eobj);
    void (*GeckoAppShellSetKeepScreenOn)(eobject eobj, bool on);
    void (*GeckoAppShellScanMedia)(eobject eobj,
                                const char* file, const char* mimeType);
    eobject (*GeckoAppShellGetIconForExtension)(eobject eobj,
                                const char* fileExt, int iconSize);
    bool (*GeckoAppShellGetShowPasswordSetting)(eobject eobj);
    bool (*GeckoAppShellGetAccessibilityEnabled)(eobject eobj);
    void (*GeckoAppShellAddPluginView)(eobject view, double x, double y, double w, double h);
    void (*GeckoAppShellRemovePluginView)(eobject view);
    void (*GeckoAppShellPostToJavaThread)(eobject eobj, bool mainThread);
    eobject (*GeckoAppShellInitCamera)(eobject eobj, const char* aContentType, int aCamera, int aWidth, int aHeight);
    void (*GeckoAppShellCloseCamera)(eobject eobj);
    void (*GeckoAppShellCheckUriVisited)(eobject eobj, const char* uri);
    void (*GeckoAppShellMarkUriVisited)(eobject eobj, const char* uri);
    void (*GeckoAppShellEnableBatteryNotifications)(eobject eobj);
    void (*GeckoAppShellDisableBatteryNotifications)(eobject eobj);

    eobject (*GeckoAppShellGetCurrentBatteryInformation)(eobject eobj);
    int (*GeckoAppShellGetNumberOfMessagesForText)(const char* aText);
    void (*GeckoAppShellSendMessage)(const char* aNumber, const char* aMessage,int aRequestId, long long aProcessId);
    int (*GeckoAppShellSaveSentMessage)(const char* aRecipient, const char* aBody, int aDate);
    void (*GeckoAppShellGetMessage)(int aMessageId, int aRequestId, long long aProcessId);
    void (*GeckoAppShellDeleteMessage)(int aMessageId, int aRequestId, long long aProcessId);
    void (*GeckoAppShellCreateMessageList)(long long aStartDate, long long aEndDate,
            eobject aNumbers, int aNumbersCount, int aDeliveryState, bool aReverse, int aRequestId,
            long long aProcessId);
    void (*GeckoAppShellGetNextMessageInList)(int aListId, int aRequestId, long long aProcessId);
    void (*GeckoAppShellClearMessageList)(int aListId);
    bool (*GeckoAppShellIsTablet)();
    void (*GeckoAppShellSetPreventPanning)(bool aPreventPanning);
    eobject (*GeckoAppShellGetCurrentNetworkInformation)();
    void (*GeckoAppShellEmitGeckoAccessibilityEvent)(int eventType, eobject textList,
        const char* description, bool enabled, bool checked, bool password);
    const char* (*GeckoAppShellHandleGeckoMessage)(const char* message);
    void (*GeckoAppShellEnableNetworkNotifications)();
    void (*GeckoAppShellDisableNetworkNotifications)();
    eobject (*GeckoAppShellGetSystemColors)(eobject eobj);

    eobject (*GeckoAppShellGetHandlersForURL)(eobject eobj,
                                const char* URL, const char* action);
    eobject (*GeckoAppShellGetHandlersForMimeType)(eobject eobj,
                                const char* mimeType, const char* action);
    void (*GeckoAppShellInstallWebApplication)(
                                eobject eobj, const char* uri,
                                const char* title, const char* iconData);
    void (*GeckoAppShellFireAndWaitForTracerEvent)(eobject eobj);

    //AudioTrack
    eobject (*AudioTrackConstructor)(int streamType, int sampleRateInHz, int channelConfig, int audioFormat, int bufferSizeInBytes, int mode);
    int (*AudioTrackWrite)(eobject object, eobject audioData, int offsetInBytes, int sizeInBytes);
    void (*AudioTrackPlay)(eobject object);
    void (*AudioTrackPause)(eobject object);
    void (*AudioTrackStop)(eobject object);
    void (*AudioTrackFlush)(eobject object);
    void (*AudioTrackReleaseEx)(eobject object);
    int (*AudioTrackGetMinBufferSize)(eobject object, int sampleRateInHz, int channelConfig, int audioFormat);
    int (*AudioTrackGetPlaybackHeadPosition)(eobject object);
    int (*AudioTrackSetStereoVolume)(eobject object, float leftVolume, float rightVolume);

    //ArrayOf<T>
    eobject (*NewArrayOfString)(int size);
    eobject (*NewArrayOfByte)(int size);
    int (*ArrayOfByteGetLength)(eobject eobj);
    int (*ArrayOfIntGetLength)(eobject eobj);
    int (*ArrayOfFloatGetLength)(eobject eobj);
    int (*ArrayOfStringGetLength)(eobject eobj);
    int (*ArrayOfEObjectGetLength)(eobject eobj);
    unsigned char* (*ArrayOfByteGetPayload)(eobject eobj);
    int* (*ArrayOfIntGetPayload)(eobject eobj);
    const char* (*ArrayOfGetContent)(eobject eobj, int index);
    void (*ArrayOfStringSetContent)(eobject eobj, int index, const char* value);
    int (*ArrayOfIntGetContent)(eobject eobj, int index);
    double (*ArrayOfDoubleGetContent)(eobject eobj, int index);
    float (*ArrayOfFloatGetContent)(eobject eobj, int index);
    eobject (*ArrayOfEObjectGetContent)(eobject eobj, int index);
    void (*ArrayOfByteFree)(eobject eobj);
    void (*ArrayOfIntFree)(eobject eobj);
    void (*ArrayOfFloatFree)(eobject eobj);
    void (*ArrayOfStringFree)(eobject eobj);
    void (*ArrayOfEObjectFree)(eobject eobj);

    void (*FreeEObject)(eobject eobj);
    void (*FreeCObject)(eobject eobj);
    void (*FreeCharBuffer)(char* str);
    void (*SetByteArrayRegion)(eobject eobj, int index, int len, unsigned char * buffer);

    //Bitmap
    int (*IBitmapGetInfo)(eobject eobj, void* info);
    int (*IBitmapLockPixels)(eobject eobj, void** addrPtr);
    int (*IBitmapUnlockPixels)(eobject eobj);

    //buffer
    eobject (*GetDirectBufferAddress)(eobject obj);
    int (*GetDirectBufferCapacity)(eobject obj);

    //suface
    eobject (*ANativeWindowFromSurface)(eobject eobj);


} EGEnv;

extern EGEnv* gEGEnv;

inline EGEnv* JNI()
{
    return gEGEnv;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOSGECKOWRAPPER_H__
