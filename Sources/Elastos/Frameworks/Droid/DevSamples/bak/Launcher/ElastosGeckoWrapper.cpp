
#include "ElastosGeckoWrapper.h"
#include "wrapper.h"
#include "GeckoAppShell.h"
#include "GeckoEvent.h"
#include <stdio.h>
#include "ElAudioTrack.h"
#include <skia/core/SkBitmap.h>

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

EGEnv* gEGEnv = NULL;

//IPoint
int IPointGetX(eobject eobj)
{
    if (!eobj) return -1;
    IPoint* point = (IPoint*)eobj;
    Int32 x = 0;
    point->GetX(&x);
    return x;
}

int IPointGetY(eobject eobj)
{
    if (!eobj) return -1;
    IPoint* point = (IPoint*)eobj;
    Int32 y = 0;
    point->GetY(&y);
    return y;
}

//IRect
int IRectGetBottom(eobject eobj)
{
    if (!eobj) return -1;
    IRect* pRect = (IRect*)eobj;
    Int32 bottom = -1;
    pRect->GetBottom(&bottom);
    return bottom;
}

int IRectGetLeft(eobject eobj)
{
    if (!eobj) return -1;
    IRect* pRect = (IRect*)eobj;
    Int32 left = -1;
    pRect->GetLeft(&left);
    return left;
}

int IRectGetTop(eobject eobj)
{
    if (!eobj) return -1;
    IRect* pRect = (IRect*)eobj;
    Int32 top = -1;
    pRect->GetTop(&top);
    return top;
}

int IRectGetRight(eobject eobj)
{
    if (!eobj) return -1;
    IRect* pRect = (IRect*)eobj;
    Int32 right = -1;
    pRect->GetRight(&right);
    return right;
}

//GeckoSurfaceView
int GeckoSurfaceViewBeginDrawing(eobject eobj)
{
    if (!eobj) return -1;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    Int32 result = -1;
    pSurfaceView->BeginDrawing(&result);
    return result;
}

eobject GeckoSurfaceViewGetSoftwareDrawBitmap(eobject eobj)
{
    if (!eobj) return NULL;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    IBitmap* bitmap = NULL;
    pSurfaceView->GetSoftwareDrawBitmap(&bitmap);
    assert(bitmap != NULL);
    return (void*)bitmap;
}

eobject GeckoSurfaceViewGetSoftwareDrawBuffer(eobject eobj)
{
    if (!eobj) return NULL;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    IBuffer* pByteBuffer = NULL;
    pSurfaceView->GetSoftwareDrawBuffer(&pByteBuffer);
    assert(pByteBuffer);
    return (void*)pByteBuffer;
}

void GeckoSurfaceViewEndDrawing(eobject eobj)
{
    if (!eobj) return;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    pSurfaceView->EndDrawing();
}

void GeckoSurfaceViewDraw2D(eobject eobj, eobject bitmap, int width, int height)
{
    if (!eobj) return;
    CGeckoSurfaceView* surfaceView = (CGeckoSurfaceView*)eobj;
    surfaceView->Draw2D((IBitmap*)bitmap, width, height);
}

void GeckoSurfaceViewDraw2D(eobject eobj, eobject buffer, int stride)
{
    if (!eobj) return;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    pSurfaceView->Draw2D((IByteBuffer*)buffer, stride);
}

eobject GeckoSurfaceViewGetSurfaceHolder(eobject eobj)
{
    if (!eobj) return NULL;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    ISurfaceHolder* pHolder = NULL;
    pSurfaceView->GetHolder(&pHolder);
    assert(pHolder);
    return (void*)pHolder;
}

eobject GeckoSurfaceViewGetSurface(eobject eobj)
{
    if (!eobj) return NULL;
    CGeckoSurfaceView* pSurfaceView = (CGeckoSurfaceView*)eobj;
    ISurface* pSurface = NULL;
    pSurfaceView->GetSurface(&pSurface);
    assert(pSurface);
    return (void*)pSurface;
}

eobject ANativeWindowFromSurface(eobject eobj)
{
    if (!eobj) return NULL;

    ISurface* pSurface = (ISurface*)eobj;;
    assert(pSurface);

    /*HANDLE ptr;
    surface->GetSurface(&ptr);
    mNativeWindow = (ANativeWindow*)(android::Surface*)ptr;*/
    HANDLE ptr;
    pSurface->GetSurface((HANDLE*)&ptr);
    return (void*)ptr;
}

//ILocation
double ILocationGetLatitude(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    double latitude;
    pLocation->GetLatitude(&latitude);
    return latitude;
}

double ILocationGetLongitude(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    double longitude;
    pLocation->GetLongitude(&longitude);
    return longitude;
}

double ILocationGetAltitude(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    double altitude;
    pLocation->GetAltitude(&altitude);
    return altitude;
}

float ILocationGetAccuracy(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    float accuracy;
    pLocation->GetAccuracy(&accuracy);
    return accuracy;
}

float ILocationGetBearing(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    float bearing;
    pLocation->GetBearing(&bearing);
    return bearing;
}

float ILocationGetSpeed(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    float speed;
    pLocation->GetSpeed(&speed);
    return speed;
}

long long ILocationGetTime(eobject eobj)
{
    if (!eobj) return -1;
    ILocation* pLocation = (ILocation*)eobj;
    Int64 time;
    pLocation->GetTime(&time);
    return time;
}

const char* String2Cstr(const String& str)
{
    if (str.IsNull()) return NULL;

    char* cstr = (char*)malloc(str.GetLength() + 1);
    strcpy(cstr, str.string());
    return cstr;
}

const char* IAddressGetAdminArea(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String area;
    pAddress->GetAdminArea(&area);
    return String2Cstr(area);
}

const char* IAddressGetCountryCode(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String countryCode;
    pAddress->GetCountryCode(&countryCode);
    return String2Cstr(countryCode);
}

const char* IAddressGetCountryName(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String countryName;
    pAddress->GetCountryName(&countryName);
    return String2Cstr(countryName);
}

const char* IAddressGetFeatureName(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String featureName;
    pAddress->GetFeatureName(&featureName);
    return String2Cstr(featureName);
}

const char* IAddressGetLocality(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String locality;
    pAddress->GetLocality(&locality);
    return String2Cstr(locality);
}

const char* IAddressGetPostalCode(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String postalCode;
    pAddress->GetPostalCode(&postalCode);
    return String2Cstr(postalCode);
}

const char* IAddressGetPremises(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String permises;
    pAddress->GetPremises(&permises);
    return String2Cstr(permises);
}

const char* IAddressGetSubAdminArea(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String subArea;
    pAddress->GetSubAdminArea(&subArea);
    return String2Cstr(subArea);
}

const char* IAddressGetSubLocality(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String subLocality;
    pAddress->GetSubLocality(&subLocality);
    return String2Cstr(subLocality);
}

const char* IAddressGetSubThoroughfare(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String subThoroughfare;
    pAddress->GetSubThoroughfare(&subThoroughfare);
    return String2Cstr(subThoroughfare);
}

const char* IAddressGetThoroughfare(eobject eobj)
{
    if (!eobj) return NULL;
    IAddress* pAddress = (IAddress*)eobj;
    String thoroughfare;
    pAddress->GetThoroughfare(&thoroughfare);
    return String2Cstr(thoroughfare);
}

//GeckoEvent
int GeckoEventGetAction(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* geckoEvent = (GeckoEvent*)eobj;
    return geckoEvent->mAction;
}

int GeckoEventGetType(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* geckoEvent = (GeckoEvent*)eobj;
    return geckoEvent->mType;
}

long long GeckoEventGetTime(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mTime;
}

eobject GeckoEventGetPointArray(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mPoints;
}

eobject GeckoEventGetPointRadiiArray(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mPointRadii;
}

eobject GeckoEventGetPointIndiciesArray(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mPointIndicies;
}

eobject GeckoEventGetRect(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    if (pGeckoEvent->mRect == NULL) return NULL;
    pGeckoEvent->mRect->AddRef();
    return pGeckoEvent->mRect.Get();
}

int GeckoEventGetFlags(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mFlags;
}

int GeckoEventGetMetaState(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mMetaState;
}

int GeckoEventGetKeyCode(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mKeyCode;
}

int GeckoEventGetUnicodeChar(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mUnicodeChar;
}

int GeckoEventGetOffset(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mOffset;
}

int GeckoEventGetCount(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mCount;
}

int GeckoEventGetPointerIndex(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mPointerIndex;
}

int GeckoEventGetRangeType(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mRangeType;
}

int GeckoEventGetRangeStyles(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mRangeStyles;
}

int GeckoEventGetRangeForeColor(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mRangeForeColor;
}

int GeckoEventGetRangeBackColor(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mRangeBackColor;
}

double GeckoEventGetAlpha(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mAlpha;
}

double GeckoEventGetBeta(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mBeta;
}

double GeckoEventGetGamma(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mGamma;
}

double GeckoEventGetX(eobject eobj)
{
    return IPointGetX(eobj);
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mX;
}

double GeckoEventGetY(eobject eobj)
{
    return IPointGetY(eobj);
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mY;
}

double GeckoEventGetZ(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mZ;
}

double GeckoEventGetBandwidth(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mBandwidth;
}

double GeckoEventGetDistance(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mDistance;
}

bool GeckoEventGetCanBeMetered(eobject eobj)
{
    if (!eobj) return -1;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return pGeckoEvent->mCanBeMetered;
}

const char* GeckoEventGetCharacters(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return String2Cstr(pGeckoEvent->mCharacters);
}

const char* GeckoEventGetCharactersExtra(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    return String2Cstr(pGeckoEvent->mCharactersExtra);
}

eobject GeckoEventGetLocation(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    pGeckoEvent->mLocation->AddRef();
    return pGeckoEvent->mLocation.Get();
}

eobject GeckoEventGetAddress(eobject eobj)
{
    if (!eobj) return NULL;
    GeckoEvent* pGeckoEvent = (GeckoEvent*)eobj;
    pGeckoEvent->mAddress->AddRef();
    return pGeckoEvent->mAddress.Get();
}

//GeckoAppShell
void GeckoAppShellNotifyIME(eobject eobj, int type, int state)
{
    //GeckoAppShell::NotifyIME(type, state);
}

void GeckoAppShellNotifyIMEEnabled(eobject eobj,
        int state, const char* typeHint,
        const char* actionHint, bool landscapeFS)
{
    String sTypeHint(typeHint);
    String sActionHint(actionHint);
    //GeckoAppShell::NotifyIMEEnabled(state, sTypeHint, sActionHint, landscapeFS);
}

void GeckoAppShellNotifyIMEChange(eobject eobj,
        const char* text, int start, int end, int newEnd)
{
    String sText(text);
    //GeckoAppShell::NotifyIMEChange(sText, start, end, newEnd);
}

void GeckoAppShellNotifyScreenShot(eobject data, int tabId, int width, int height)
{
    GeckoAppShell::NotifyScreenShot((IByteBuffer*)data, tabId, width, height);
}

void GeckoAppShellAcknowledgeEventSync(eobject eobj)
{
    GeckoAppShell::AcknowledgeEventSync();
}

void GeckoAppShellEnableDeviceMotion(eobject eobj, bool enable)
{
    GeckoAppShell::EnableDeviceMotion(enable);
}

void GeckoAppShellEnableLocation(eobject eobj, bool enable)
{
    GeckoAppShell::EnableLocation(enable);
}

void GeckoAppShellEnableSensor(eobject eobj, int aSensortype)
{
    GeckoAppShell::EnableSensor(aSensortype);
}

void GeckoAppShellDisableSensor(eobject eobj, int aSensortype)
{
    GeckoAppShell::DisableSensor(aSensortype);
}

void GeckoAppShellReturnIMEQueryResult(eobject eobj,
        const char* result, int selStart, int selLen)
{
    String sResult(result);
    GeckoAppShell::ReturnIMEQueryResult(sResult, selStart, selLen);
}

void GeckoAppShellOnAppShellReady(eobject eobj)
{
    GeckoAppShell::OnAppShellReady();
}

void GeckoAppShellScheduleRestart(eobject eobj)
{
    GeckoAppShell::ScheduleRestart();
}

void GeckoAppShellCreateShortcut(eobject eobj,
        const char* aTitle, const char* aURI, const char* aIconData, const char* aType)
{
    GeckoAppShell::CreateShortcut(String(aTitle), String(aURI), String(aIconData), String(aType));
}

void GeckoAppShellOnXreExit(eobject eobj)
{
    GeckoAppShell::OnXreExit();
}

bool GeckoAppShellOpenUriExternal(eobject eobj,
        const char* uriSpec, const char* mimeType,
        const char* packageName, const char* className,
        const char* action, const char* title)
{
    String sUriSpec(uriSpec);
    String sMimeType(mimeType);
    String sPackageName(packageName);
    String sClassName(className);
    String sAction(action);
    String sTitle(title);
    ECode ec = GeckoAppShell::OpenUriExternal(sUriSpec, sMimeType,
                 sPackageName, sClassName, sAction, sTitle);
    if (FAILED(ec)) return false;
    else return true;
}

const char* GeckoAppShellGetMimeTypeFromExtensions(eobject eobj, const char* fileExt)
{
    String sFileExt(fileExt);
    String mimeType;
    GeckoAppShell::GetMimeTypeFromExtensions(sFileExt, &mimeType);
    return String2Cstr(mimeType);
}

const char* GeckoAppShellGetExtensionFromMimeType(eobject eobj, const char* mimeType)
{
    String sMimeType(mimeType);
    String extension;
    GeckoAppShell::GetExtensionFromMimeType(sMimeType, &extension);
    return String2Cstr(extension);
}

void GeckoAppShellMoveTaskToBack(eobject eobj)
{
    GeckoAppShell::MoveTaskToBack();
}

const char* GeckoAppShellGetClipboardText(eobject eobj)
{
    String text;
    GeckoAppShell::GetClipboardText(&text);
    return String2Cstr(text);
}

void GeckoAppShellSetClipboardText(eobject eobj, const char* text)
{
    String sText(text);
    GeckoAppShell::SetClipboardText(sText);
}

void GeckoAppShellShowAlertNotification(eobject eobj, const char* imageUrl,
        const char* alertTitle, const char* alertText,
        const char* alertCookie, const char* alertName)
{
    String sUri(imageUrl);
    String sTitle(alertTitle);
    String sText(alertText);
    String sCookie(alertCookie);
    String sName(alertName);
    GeckoAppShell::ShowAlertNotification(sUri, sTitle, sText, sCookie, sName);
}

void GeckoAppShellAlertsProgressListener_OnProgress(
        eobject eobj, const char* alertName, long long progress,
        long long progressMax, const char* alertText)
{
    String sText(alertText);
    String sName(alertName);
    GeckoAppShell::AlertsProgressListener_OnProgress(
                    sName, progress, progressMax, sText);
}

void GeckoAppShellAlertsProgressListener_OnCancel(eobject eobj, const char* alertName)
{
    String sName(alertName);
    GeckoAppShell::AlertsProgressListener_OnCancel(sName);
}

int GeckoAppShellGetDPI(eobject eobj)
{
    Int32 dpi = -1;
    GeckoAppShell::GetDpi(&dpi);
    return dpi;
}

const char* GeckoAppShellShowFilePicker(eobject eobj, const char* filters)
{
    String sFilters(filters);
    String filePath;
    GeckoAppShell::ShowFilePicker(sFilters, &filePath);
    return String2Cstr(filePath);
}

void GeckoAppShellSetFullScreen(eobject eobj, bool fullScreen)
{
    GeckoAppShell::SetFullScreen(fullScreen);
}

void GeckoAppShellHideProgressDialog(eobject eobj)
{
    GeckoAppShell::HideProgressDialog();
}

void GeckoAppShellPerformHapticFeedback(eobject eobj, bool isLongPress)
{
    GeckoAppShell::PerformHapticFeedback(isLongPress);
}

void GeckoAppShellVibrate1(eobject eobj, long long milliseconds)
{
    GeckoAppShell::Vibrate(milliseconds);
}

void GeckoAppShellVibrate2(eobject eobj, long long *pattern, int size, int repeat)
{
    AutoPtr<ArrayOf<Int64> > aPattern = ArrayOf<Int64>::Alloc(size);
    for(int i=0;i<size;i++) {
        (*aPattern)[i] = pattern[i];
    }

    GeckoAppShell::Vibrate(*aPattern, repeat);
}

void GeckoAppShellCancelVibrate(eobject eobj)
{
    GeckoAppShell::CancelVibrate();
}

bool GeckoAppShellIsNetworkLinkUp(eobject eobj)
{
    Boolean isLinkUp = FALSE;
    GeckoAppShell::IsNetworkLinkUp(&isLinkUp);
    return isLinkUp;
}

bool GeckoAppShellIsNetworkLinkKnown(eobject eobj)
{
    Boolean isLinkKnown = FALSE;
    GeckoAppShell::IsNetworkLinkKnown(&isLinkKnown);
    return isLinkKnown;
}

void GeckoAppShellSetSelectedLocale(eobject eobj, const char* aLocale)
{
    String locale(aLocale);
    GeckoAppShell::SetSelectedLocale(locale);
}

void GeckoAppShellShowInputMethodPicker(eobject eobj)
{
    GeckoAppShell::ShowInputMethodPicker();
}

void GeckoAppShellSetKeepScreenOn(eobject eobj, bool on)
{
    GeckoAppShell::SetKeepScreenOn(on);
}

void GeckoAppShellScanMedia(eobject eobj, const char* file, const char* mimeType)
{
    String sFile(file);
    String sMimeType(mimeType);
    GeckoAppShell::ScanMedia(sFile, sMimeType);
}

eobject GeckoAppShellGetIconForExtension(eobject eobj, const char* fileExt, int iconSize)
{
    String sFileExt(fileExt);
    ArrayOf<Byte>* pIcons = NULL;
    GeckoAppShell::GetIconForExtension(sFileExt, iconSize, &pIcons);
    return (void*)pIcons;
}

bool GeckoAppShellGetShowPasswordSetting(eobject eobj)
{
    Boolean isShow;
    GeckoAppShell::GetShowPasswordSetting(&isShow);
    return isShow;
}

bool GeckoAppShellGetAccessibilityEnabled(eobject eobj)
{
    Boolean enable;
    GeckoAppShell::GetAccessibilityEnabled(&enable);
    return enable;
}

void GeckoAppShellAddPluginView(eobject view, double x, double y, double w, double h)
{
    GeckoAppShell::AddPluginView((IView*)view, x, y, w, h);
}

void GeckoAppShellRemovePluginView(eobject view)
{
    GeckoAppShell::RemovePluginView((IView*)view);
}

void GeckoAppShellPostToJavaThread(eobject eobj, bool mainThread)
{
    GeckoAppShell::PostToJavaThread(mainThread);
}

eobject GeckoAppShellInitCamera(
        eobject eobj, const char* aContentType, int aCamera, int aWidth, int aHeight)
{
    ArrayOf<Int32> * out;
    GeckoAppShell::InitCamera(String(aContentType), aCamera, aWidth, aHeight, &out);
    return out;
}

void GeckoAppShellCloseCamera(eobject eobj) {
    GeckoAppShell::CloseCamera();
}

void GeckoAppShellCheckUriVisited(eobject eobj, const char* uri) {
    GeckoAppShell::CheckUriVisited(String(uri));
}

void GeckoAppShellMarkUriVisited(eobject eobj, const char* uri) {
    GeckoAppShell::MarkUriVisited(String(uri));
}

void GeckoAppShellEnableBatteryNotifications(eobject eobj) {
    GeckoAppShell::EnableBatteryNotifications();
}

void GeckoAppShellDisableBatteryNotifications(eobject eobj) {
    GeckoAppShell::DisableBatteryNotifications();
}

eobject GeckoAppShellGetCurrentBatteryInformation(eobject eobj) {
    ArrayOf<Double>* info;
    GeckoAppShell::GetCurrentBatteryInformation(&info);
    return info;
}

int GeckoAppShellGetNumberOfMessagesForText(const char* aText) {
    int num;
    GeckoAppShell::GetNumberOfMessagesForText(String(aText), &num);
    return num;
}

void GeckoAppShellSendMessage(const char* aNumber, const char* aMessage,
    int aRequestId, long long aProcessId)
{
    GeckoAppShell::SendMessage(String(aNumber), String(aMessage),
        aRequestId, aProcessId);
}

int GeckoAppShellSaveSentMessage(const char* aRecipient, const char* aBody, int aDate)
{
    int errorCode;
    assert(0);
//    GeckoAppShell::SaveSentMessage((ICharSequence*)aRecipient, (ICharSequence*)aBody
//            , (IInteger32*)aDate, &errorCode);
    return errorCode;
}

void GeckoAppShellGetMessage(int aMessageId, int aRequestId, long long aProcessId) {
    GeckoAppShell::GetMessage(aMessageId, aRequestId, aProcessId);
}

void GeckoAppShellDeleteMessage(int aMessageId, int aRequestId, long long aProcessId) {
    GeckoAppShell::DeleteMessage(aMessageId, aRequestId, aProcessId);
}

void GeckoAppShellCreateMessageList(
        long long aStartDate, long long aEndDate, eobject aNumbers, int aNumbersCount,
        int aDeliveryState, bool aReverse, int aRequestId, long long aProcessId)
{
    GeckoAppShell::CreateMessageList(aStartDate, aEndDate, *((ArrayOf<String>*)aNumbers),
        aNumbersCount, aDeliveryState, aReverse, aRequestId, aProcessId);
}

void GeckoAppShellGetNextMessageInList(int aListId, int aRequestId, long long aProcessId)
{
    GeckoAppShell::GetNextMessageInList(aListId, aRequestId, aProcessId);
}

void GeckoAppShellClearMessageList(int aListId) {
    GeckoAppShell::ClearMessageList(aListId);
}

bool GeckoAppShellIsTablet() {
    Boolean isTablet;
    GeckoAppShell::IsTablet(&isTablet);
    return isTablet;
}

void GeckoAppShellSetPreventPanning(bool aPreventPanning) {
    GeckoAppShell::SetPreventPanning(aPreventPanning);
}

eobject GeckoAppShellGetCurrentNetworkInformation() {
    ArrayOf<Double>* info;
    GeckoAppShell::GetCurrentNetworkInformation(&info);
    return info;
}

void GeckoAppShellEmitGeckoAccessibilityEvent(int eventType, eobject textList,
        const char* description, bool enabled, bool checked, bool password) {
    GeckoAppShell::EmitGeckoAccessibilityEvent(eventType, *((ArrayOf<String> *)textList),
        String(description),enabled, checked, password);
}

const char* GeckoAppShellHandleGeckoMessage(const char* message) {
    String result;
    GeckoAppShell::HandleGeckoMessage(String(message), &result);
    return String2Cstr(result);
}

void GeckoAppShellEnableNetworkNotifications() {
    GeckoAppShell::EnableNetworkNotifications();
}

void GeckoAppShellDisableNetworkNotifications() {
    GeckoAppShell::DisableNetworkNotifications();
}

eobject GeckoAppShellGetSystemColors(eobject eobj) {
    ArrayOf<Int32>* colors = NULL;
    GeckoAppShell::GetSystemColors(&colors);
    return (void*)colors;
}

eobject GeckoAppShellGetHandlersForURL(eobject eobj, const char* URL, const char* action) {
    String sURL(URL);
    String sAction(action);
    ArrayOf<String>* pHandlers = NULL;
    GeckoAppShell::GetHandlersForURL(sURL, sAction, &pHandlers);
    return (void*)pHandlers;
}

eobject GeckoAppShellGetHandlersForMimeType(eobject eobj,
                            const char* mimeType, const char* action) {
    String sMimeType(mimeType);
    String sAction(action);
    ArrayOf<String>* pHandlers = NULL;
    GeckoAppShell::GetHandlersForMimeType(sMimeType, sAction, &pHandlers);
    return (void*)pHandlers;
}


void GeckoAppShellFireAndWaitForTracerEvent(eobject eobj) {
    GeckoAppShell::FireAndWaitForTracerEvent();
}

//AudioTrack
eobject AudioTrackConstructor(int streamType, int sampleRateInHz,
    int channelConfig, int audioFormat, int bufferSizeInBytes, int mode)
{
/*    AutoPtr<IAudioTrack> audio;
    CAudioTrack::New(streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode, (IAudioTrack**)&audio);
    return audio;*/
    ElAudioTrack* track = new ElAudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
    return track;
}

int AudioTrackWrite(eobject object, eobject audioData, int offsetInBytes, int sizeInBytes)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackWrite");
/*    int num;
    ((IAudioTrack*)object)->Write((ArrayOf<Byte>)(*audioData), offsetInBytes, sizeInBytes, &num);
    return num;*/
    return ((ElAudioTrack*)object)->Write(((ArrayOf<Byte>*)audioData)->GetPayload(), offsetInBytes, sizeInBytes);
}

void AudioTrackPlay(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackPlay");
    ((ElAudioTrack*)object)->Play();
}

void AudioTrackPause(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackPause");
    ((ElAudioTrack*)object)->Pause();
}

void AudioTrackStop(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackStop");
    ((ElAudioTrack*)object)->Stop();
}

void AudioTrackFlush(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackFlush");
    ((ElAudioTrack*)object)->Flush();
}

void AudioTrackRelease(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackReleaseEx");
    ((ElAudioTrack*)object)->Release();
}

int AudioTrackGetMinBufferSize(eobject object,
    int sampleRateInHz, int channelConfig, int audioFormat)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackGetMinBufferSize");
/*    Int32 size;
    ((IAudioTrack*)object)->GetMinBufferSize(sampleRateInHz,
        channelConfig, audioFormat, &size);
    return size;*/
    return ((ElAudioTrack*)object)->GetMinBufferSize(sampleRateInHz,
        channelConfig, audioFormat);
}

int AudioTrackGetPlaybackHeadPosition(eobject object)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackGetPlaybackHeadPosition");
/*    Int32 pos;
    ((IAudioTrack*)object)->GetPlaybackHeadPosition(&pos);
    return pos;*/
    return ((ElAudioTrack*)object)->GetPlaybackHeadPosition();
}

int AudioTrackSetStereoVolume(eobject object, float leftVolume, float rightVolume)
{
    PRINT_FILE_LINE_EX("--------- AudioTrackSetStereoVolume");
/*    Int32 code;
    ((IAudioTrack*)object)->SetStereoVolume(leftVolume, rightVolume, &code);
    return code;*/
    return ((ElAudioTrack*)object)->SetStereoVolume(leftVolume, rightVolume);
}


//ArrayOf<T>
eobject NewArrayOfString(int size)
{
    return ArrayOf<String>::Alloc(size);
}

eobject NewArrayOfByte(int size)
{
    return ArrayOf<Byte>::Alloc(size);
}

int ArrayOfByteGetLength(eobject eobj)
{
    if (!eobj) return -1;
    ArrayOf<Byte>* pArray = (ArrayOf<Byte>*)eobj;
    return pArray->GetLength();
}

int ArrayOfIntGetLength(eobject eobj)
{
    if (!eobj) return -1;
    ArrayOf<Int32>* pArray = (ArrayOf<Int32>*)eobj;
    return pArray->GetLength();
}

int ArrayOfFloatGetLength(eobject eobj)
{
    if (!eobj) return -1;
    ArrayOf<Float>* pArray = (ArrayOf<Float>*)eobj;
    return pArray->GetLength();
}

int ArrayOfStringGetLength(eobject eobj)
{
    if (!eobj) return -1;
    ArrayOf<String>* pArray = (ArrayOf<String>*)eobj;
    return pArray->GetLength();
}

int ArrayOfEObjectGetLength(eobject eobj)
{
    if (!eobj) return -1;
    ArrayOf<eobject>* pArray = (ArrayOf<eobject>*)eobj;
    return pArray->GetLength();
}

unsigned char* ArrayOfByteGetPayload(eobject eobj)
{
    if (!eobj) return NULL;
    ArrayOf<Byte>* pArray = (ArrayOf<Byte>*)eobj;
    return pArray->GetPayload();
}

int* ArrayOfIntGetPayload(eobject eobj)
{
    if (!eobj) return NULL;
    ArrayOf<Int32>* pArray = (ArrayOf<Int32>*)eobj;
    return pArray->GetPayload();
}

const char* ArrayOfGetContent(eobject eobj, int index)
{
    if (!eobj || index < 0) return NULL;
    ArrayOf<String>* pArray = (ArrayOf<String>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return NULL;
    return String2Cstr((*pArray)[index]);
}

void ArrayOfStringSetContent(eobject eobj, int index, const char* value)
{
    if (!eobj || index < 0) return;
    ArrayOf<String>* pArray = (ArrayOf<String>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return;
    (*pArray)[index] = String(value);
}

int ArrayOfIntGetContent(eobject eobj, int index)
{
    if (!eobj || index < 0) return 0;
    ArrayOf<Int32>* pArray = (ArrayOf<Int32>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return 0;
    return (*pArray)[index];
}

double ArrayOfDoubleGetContent(eobject eobj, int index)
{
    if (!eobj || index < 0) return 0;
    ArrayOf<Double>* pArray = (ArrayOf<Double>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return 0;
    return (*pArray)[index];
}


float ArrayOfFloatGetContent(eobject eobj, int index)
{
    if (!eobj || index < 0) return 0;
    ArrayOf<Float>* pArray = (ArrayOf<Float>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return 0;
    return (*pArray)[index];
}

eobject ArrayOfEObjectGetContent(eobject eobj, int index)
{
    if (!eobj || index < 0) return NULL;
    ArrayOf<eobject>* pArray = (ArrayOf<eobject>*)eobj;
    Int32 len = pArray->GetLength();
    if (index >= len) return NULL;
    return (eobject)(*pArray)[index];
}

void ArrayOfByteFree(eobject eobj)
{
    if (!eobj) return;
    ArrayOf<Byte>* pArray = (ArrayOf<Byte>*)eobj;
    ArrayOf<Byte>::Free(pArray);
}

void ArrayOfIntFree(eobject eobj)
{
    if (!eobj) return;
    ArrayOf<Int32>* pArray = (ArrayOf<Int32>*)eobj;
    ArrayOf<Int32>::Free(pArray);
}

void ArrayOfFloatFree(eobject eobj)
{
    if (!eobj) return;
    ArrayOf<Float>* pArray = (ArrayOf<Float>*)eobj;
    ArrayOf<Float>::Free(pArray);
}

void ArrayOfStringFree(eobject eobj)
{
    if (!eobj) return;
    ArrayOf<String>* pArray = (ArrayOf<String>*)eobj;
    Int32 len = pArray->GetLength();
    for (Int32 i = 0; i < len; i++) {
        (*pArray)[i] = NULL;
    }
    ArrayOf<String>::Free(pArray);
}

void ArrayOfEObjectFree(eobject eobj)
{
    if (!eobj) return;
    ArrayOf<eobject>* pArray = (ArrayOf<eobject>*)eobj;
    Int32 len = pArray->GetLength();
    for (Int32 i = 0; i < len; i++) {
        (*pArray)[i] = NULL;
    }
    ArrayOf<eobject>::Free(pArray);
}

void FreeCharBuffer(char* str)
{
    if (!str) return;
    free(str);
}

void SetByteArrayRegion(eobject eobj, int index, int len, unsigned char * buffer)
{
    ArrayOf<Byte> * array = (ArrayOf<Byte> *)eobj;
    for(int i = index; i< array->GetLength() && i< index + len; i++)
    {
        (*array)[i] = buffer[i-index];
    }
}

void FreeEObject(eobject eobj)
{
    if (eobj) ((IInterface*)eobj)->Release();
}

//TODO:temporary scheme, ElAudioTrack will be replaced by IAudioTrack.
void FreeCObject(eobject eobj)
{
    delete (ElAudioTrack*)eobj;
}

#define ANDROID_BITMAP_RESUT_SUCCESS            0
#define ANDROID_BITMAP_RESULT_BAD_PARAMETER     -1
#define ANDROID_BITMAP_RESULT_JNI_EXCEPTION     -2
#define ANDROID_BITMAP_RESULT_ALLOCATION_FAILED -3

enum AndroidBitmapFormat {
    ANDROID_BITMAP_FORMAT_NONE      = 0,
    ANDROID_BITMAP_FORMAT_RGBA_8888 = 1,
    ANDROID_BITMAP_FORMAT_RGB_565   = 4,
    ANDROID_BITMAP_FORMAT_RGBA_4444 = 7,
    ANDROID_BITMAP_FORMAT_A_8       = 8,
};

typedef struct {
    uint32_t    width;
    uint32_t    height;
    uint32_t    stride;
    int32_t     format;
    uint32_t    flags;      // 0 for now
} AndroidBitmapInfo;

int IBitmapGetInfo(
    /* [in] */ eobject eobj,
    /* [in] */ void* _info)
{
    if (NULL == eobj || NULL == _info) {
        return ANDROID_BITMAP_RESULT_BAD_PARAMETER;
    }

    SkBitmap* bm = NULL;
    ((IBitmap*)eobj)->GetNativeBitmap((HANDLE*)&bm);

    if (NULL == bm) {
        return ANDROID_BITMAP_RESULT_JNI_EXCEPTION;
    }

    AndroidBitmapInfo* info = (AndroidBitmapInfo*)_info;

    if (info) {
        info->width     = bm->width();
        info->height    = bm->height();
        info->stride    = bm->rowBytes();
        info->flags     = 0;

        switch (bm->config()) {
            case SkBitmap::kARGB_8888_Config:
                info->format = ANDROID_BITMAP_FORMAT_RGBA_8888;
                break;
            case SkBitmap::kRGB_565_Config:
                info->format = ANDROID_BITMAP_FORMAT_RGB_565;
                break;
            case SkBitmap::kARGB_4444_Config:
                info->format = ANDROID_BITMAP_FORMAT_RGBA_4444;
                break;
            case SkBitmap::kA8_Config:
                info->format = ANDROID_BITMAP_FORMAT_A_8;
                break;
            default:
                info->format = ANDROID_BITMAP_FORMAT_NONE;
                break;
        }
    }
    return ANDROID_BITMAP_RESUT_SUCCESS;
}

int IBitmapLockPixels(
    /* [in] */ eobject eobj,
    /* [in] */ void** addrPtr)
{
    if (NULL == eobj || NULL == addrPtr) {
        return ANDROID_BITMAP_RESULT_BAD_PARAMETER;
    }

    SkBitmap* bm = NULL;
    ((IBitmap*)eobj)->GetNativeBitmap((HANDLE*)&bm);

    if (NULL == bm) {
        return ANDROID_BITMAP_RESULT_JNI_EXCEPTION;
    }

    bm->lockPixels();
    void* addr = bm->getPixels();
    if (NULL == addr) {
        bm->unlockPixels();
        return ANDROID_BITMAP_RESULT_ALLOCATION_FAILED;
    }

    if (addrPtr) {
        *addrPtr = addr;
    }
    return ANDROID_BITMAP_RESUT_SUCCESS;
}

int IBitmapUnlockPixels(
    /* [in] */ eobject eobj)
{
    if (NULL == eobj) {
        return ANDROID_BITMAP_RESULT_BAD_PARAMETER;
    }

    SkBitmap* bm = NULL;
    ((IBitmap*)eobj)->GetNativeBitmap((HANDLE*)&bm);
    if (NULL == bm) {
        return ANDROID_BITMAP_RESULT_JNI_EXCEPTION;
    }

    bm->unlockPixels();
    return ANDROID_BITMAP_RESUT_SUCCESS;
}

void InitEGEnv() {

    if (gEGEnv) return;
    gEGEnv = new EGEnv();
    assert(gEGEnv != NULL);

    //IPoint
    gEGEnv->IPointGetX = IPointGetX;
    gEGEnv->IPointGetY = IPointGetY;

    //IRect
    gEGEnv->IRectGetBottom = IRectGetBottom;
    gEGEnv->IRectGetLeft = IRectGetLeft;
    gEGEnv->IRectGetTop = IRectGetTop;
    gEGEnv->IRectGetRight = IRectGetRight;

    //GeckoSurfaceView
    gEGEnv->GeckoSurfaceViewBeginDrawing = GeckoSurfaceViewBeginDrawing;
    gEGEnv->GeckoSurfaceViewGetSoftwareDrawBitmap = GeckoSurfaceViewGetSoftwareDrawBitmap;
    gEGEnv->GeckoSurfaceViewGetSoftwareDrawBuffer = GeckoSurfaceViewGetSoftwareDrawBuffer;
    gEGEnv->GeckoSurfaceViewEndDrawing = GeckoSurfaceViewEndDrawing;
    gEGEnv->GeckoSurfaceViewDraw2D = GeckoSurfaceViewDraw2D;
    gEGEnv->GeckoSurfaceViewDraw2DEx = GeckoSurfaceViewDraw2DEx;
    gEGEnv->GeckoSurfaceViewGetSurfaceHolder = GeckoSurfaceViewGetSurfaceHolder;
    gEGEnv->GeckoSurfaceViewGetSurface = GeckoSurfaceViewGetSurface;

    //ILocation
    gEGEnv->ILocationGetLatitude = ILocationGetLatitude;
    gEGEnv->ILocationGetLongitude = ILocationGetLongitude;
    gEGEnv->ILocationGetAltitude = ILocationGetAltitude;
    gEGEnv->ILocationGetAccuracy = ILocationGetAccuracy;
    gEGEnv->ILocationGetBearing = ILocationGetBearing;
    gEGEnv->ILocationGetSpeed = ILocationGetSpeed;
    gEGEnv->ILocationGetTime = ILocationGetTime;

    //IAddress
    gEGEnv->IAddressGetAdminArea = IAddressGetAdminArea;
    gEGEnv->IAddressGetCountryCode = IAddressGetCountryCode;
    gEGEnv->IAddressGetCountryName = IAddressGetCountryName;
    gEGEnv->IAddressGetFeatureName = IAddressGetFeatureName;
    gEGEnv->IAddressGetLocality = IAddressGetLocality;
    gEGEnv->IAddressGetPostalCode = IAddressGetPostalCode;
    gEGEnv->IAddressGetPremises = IAddressGetPremises;
    gEGEnv->IAddressGetSubAdminArea = IAddressGetSubAdminArea;
    gEGEnv->IAddressGetSubLocality = IAddressGetSubLocality;
    gEGEnv->IAddressGetSubThoroughfare = IAddressGetSubThoroughfare;
    gEGEnv->IAddressGetThoroughfare = IAddressGetThoroughfare;

    //GeckoEvent
    gEGEnv->GeckoEventGetAction = GeckoEventGetAction;
    gEGEnv->GeckoEventGetType = GeckoEventGetType;
    gEGEnv->GeckoEventGetTime = GeckoEventGetTime;
    gEGEnv->GeckoEventGetPointArray = GeckoEventGetPointArray;
    gEGEnv->GeckoEventGetPointRadiiArray = GeckoEventGetPointRadiiArray;
    gEGEnv->GeckoEventGetPointIndiciesArray = GeckoEventGetPointIndiciesArray;
    gEGEnv->GeckoEventGetRect = GeckoEventGetRect;
    gEGEnv->GeckoEventGetFlags = GeckoEventGetFlags;
    gEGEnv->GeckoEventGetMetaState = GeckoEventGetMetaState;
    gEGEnv->GeckoEventGetKeyCode = GeckoEventGetKeyCode;
    gEGEnv->GeckoEventGetUnicodeChar = GeckoEventGetUnicodeChar;
    gEGEnv->GeckoEventGetOffset = GeckoEventGetOffset;
    gEGEnv->GeckoEventGetCount = GeckoEventGetCount;
    gEGEnv->GeckoEventGetPointerIndex = GeckoEventGetPointerIndex;
    gEGEnv->GeckoEventGetRangeType = GeckoEventGetRangeType;
    gEGEnv->GeckoEventGetRangeStyles = GeckoEventGetRangeStyles;
    gEGEnv->GeckoEventGetRangeForeColor = GeckoEventGetRangeForeColor;
    gEGEnv->GeckoEventGetRangeBackColor = GeckoEventGetRangeBackColor;
    gEGEnv->GeckoEventGetAlpha = GeckoEventGetAlpha;
    gEGEnv->GeckoEventGetBeta = GeckoEventGetBeta;
    gEGEnv->GeckoEventGetGamma = GeckoEventGetGamma;
    gEGEnv->GeckoEventGetX = GeckoEventGetX;
    gEGEnv->GeckoEventGetY = GeckoEventGetY;
    gEGEnv->GeckoEventGetZ = GeckoEventGetZ;
    gEGEnv->GeckoEventGetBandwidth = GeckoEventGetBandwidth;
    gEGEnv->GeckoEventGetDistance = GeckoEventGetDistance;
    gEGEnv->GeckoEventGetCanBeMetered = GeckoEventGetCanBeMetered;
    gEGEnv->GeckoEventGetCharacters = GeckoEventGetCharacters;
    gEGEnv->GeckoEventGetCharactersExtra = GeckoEventGetCharactersExtra;

    gEGEnv->GeckoEventGetLocation = GeckoEventGetLocation;
    gEGEnv->GeckoEventGetAddress = GeckoEventGetAddress;

    //GeckoAppShell
    gEGEnv->GeckoAppShellNotifyIME = GeckoAppShellNotifyIME;
    gEGEnv->GeckoAppShellNotifyIMEEnabled = GeckoAppShellNotifyIMEEnabled;
    gEGEnv->GeckoAppShellNotifyIMEChange = GeckoAppShellNotifyIMEChange;
    gEGEnv->GeckoAppShellNotifyScreenShot = GeckoAppShellNotifyScreenShot;
    gEGEnv->GeckoAppShellAcknowledgeEventSync = GeckoAppShellAcknowledgeEventSync;
    gEGEnv->GeckoAppShellEnableDeviceMotion = GeckoAppShellEnableDeviceMotion;
    gEGEnv->GeckoAppShellEnableLocation = GeckoAppShellEnableLocation;
    gEGEnv->GeckoAppShellEnableSensor = GeckoAppShellEnableSensor;
    gEGEnv->GeckoAppShellDisableSensor = GeckoAppShellDisableSensor;
    gEGEnv->GeckoAppShellReturnIMEQueryResult = GeckoAppShellReturnIMEQueryResult;
    gEGEnv->GeckoAppShellOnAppShellReady = GeckoAppShellOnAppShellReady;
    gEGEnv->GeckoAppShellScheduleRestart = GeckoAppShellScheduleRestart;
    gEGEnv->GeckoAppShellCreateShortcut = GeckoAppShellCreateShortcut;
    gEGEnv->GeckoAppShellOnXreExit = GeckoAppShellOnXreExit;
    gEGEnv->GeckoAppShellOpenUriExternal = GeckoAppShellOpenUriExternal;
    gEGEnv->GeckoAppShellGetMimeTypeFromExtensions = GeckoAppShellGetMimeTypeFromExtensions;
    gEGEnv->GeckoAppShellGetExtensionFromMimeType = GeckoAppShellGetExtensionFromMimeType;
    gEGEnv->GeckoAppShellMoveTaskToBack = GeckoAppShellMoveTaskToBack;
    gEGEnv->GeckoAppShellGetClipboardText = GeckoAppShellGetClipboardText;
    gEGEnv->GeckoAppShellSetClipboardText = GeckoAppShellSetClipboardText;
    gEGEnv->GeckoAppShellShowAlertNotification = GeckoAppShellShowAlertNotification;
    gEGEnv->GeckoAppShellAlertsProgressListener_OnProgress = GeckoAppShellAlertsProgressListener_OnProgress;
    gEGEnv->GeckoAppShellAlertsProgressListener_OnCancel = GeckoAppShellAlertsProgressListener_OnCancel;
    gEGEnv->GeckoAppShellGetDPI = GeckoAppShellGetDPI;
    gEGEnv->GeckoAppShellShowFilePicker = GeckoAppShellShowFilePicker;
    gEGEnv->GeckoAppShellSetFullScreen = GeckoAppShellSetFullScreen;
    gEGEnv->GeckoAppShellHideProgressDialog = GeckoAppShellHideProgressDialog;
    gEGEnv->GeckoAppShellPerformHapticFeedback = GeckoAppShellPerformHapticFeedback;
    gEGEnv->GeckoAppShellVibrate1 = GeckoAppShellVibrate1;
    gEGEnv->GeckoAppShellVibrate2 = GeckoAppShellVibrate2;
    gEGEnv->GeckoAppShellIsNetworkLinkUp = GeckoAppShellIsNetworkLinkUp;
    gEGEnv->GeckoAppShellIsNetworkLinkKnown = GeckoAppShellIsNetworkLinkKnown;
    gEGEnv->GeckoAppShellSetSelectedLocale = GeckoAppShellSetSelectedLocale;
    gEGEnv->GeckoAppShellShowInputMethodPicker = GeckoAppShellShowInputMethodPicker;
    gEGEnv->GeckoAppShellSetKeepScreenOn = GeckoAppShellSetKeepScreenOn;
    gEGEnv->GeckoAppShellScanMedia = GeckoAppShellScanMedia;
    gEGEnv->GeckoAppShellGetIconForExtension = GeckoAppShellGetIconForExtension;
    gEGEnv->GeckoAppShellGetShowPasswordSetting = GeckoAppShellGetShowPasswordSetting;
    gEGEnv->GeckoAppShellGetAccessibilityEnabled = GeckoAppShellGetAccessibilityEnabled;
    gEGEnv->GeckoAppShellAddPluginView = GeckoAppShellAddPluginView;
    gEGEnv->GeckoAppShellRemovePluginView = GeckoAppShellRemovePluginView;
    gEGEnv->GeckoAppShellPostToJavaThread = GeckoAppShellPostToJavaThread;
    gEGEnv->GeckoAppShellInitCamera = GeckoAppShellInitCamera;
    gEGEnv->GeckoAppShellCloseCamera = GeckoAppShellCloseCamera;
    gEGEnv->GeckoAppShellCheckUriVisited = GeckoAppShellCheckUriVisited;
    gEGEnv->GeckoAppShellMarkUriVisited = GeckoAppShellMarkUriVisited;
    gEGEnv->GeckoAppShellEnableBatteryNotifications = GeckoAppShellEnableBatteryNotifications;
    gEGEnv->GeckoAppShellDisableBatteryNotifications = GeckoAppShellDisableBatteryNotifications;
    gEGEnv->GeckoAppShellGetCurrentBatteryInformation = GeckoAppShellGetCurrentBatteryInformation;
    gEGEnv->GeckoAppShellGetNumberOfMessagesForText = GeckoAppShellGetNumberOfMessagesForText;
    gEGEnv->GeckoAppShellSendMessage = GeckoAppShellSendMessage;
    gEGEnv->GeckoAppShellSaveSentMessage = GeckoAppShellSaveSentMessage;
    gEGEnv->GeckoAppShellGetMessage = GeckoAppShellGetMessage;
    gEGEnv->GeckoAppShellDeleteMessage = GeckoAppShellDeleteMessage;
    gEGEnv->GeckoAppShellCreateMessageList = GeckoAppShellCreateMessageList;
    gEGEnv->GeckoAppShellGetNextMessageInList = GeckoAppShellGetNextMessageInList;
    gEGEnv->GeckoAppShellClearMessageList = GeckoAppShellClearMessageList;
    gEGEnv->GeckoAppShellIsTablet = GeckoAppShellIsTablet;
    gEGEnv->GeckoAppShellSetPreventPanning = GeckoAppShellSetPreventPanning;
    gEGEnv->GeckoAppShellEmitGeckoAccessibilityEvent = GeckoAppShellEmitGeckoAccessibilityEvent;
    gEGEnv->GeckoAppShellHandleGeckoMessage = GeckoAppShellHandleGeckoMessage;
    gEGEnv->GeckoAppShellEnableNetworkNotifications = GeckoAppShellEnableNetworkNotifications;
    gEGEnv->GeckoAppShellDisableNetworkNotifications = GeckoAppShellDisableNetworkNotifications;
    gEGEnv->GeckoAppShellGetSystemColors = GeckoAppShellGetSystemColors;
    gEGEnv->GeckoAppShellScanMedia = GeckoAppShellScanMedia;
    gEGEnv->GeckoAppShellGetHandlersForURL = GeckoAppShellGetHandlersForURL;
    gEGEnv->GeckoAppShellGetHandlersForMimeType = GeckoAppShellGetHandlersForMimeType;

    //AudioTrack
    gEGEnv->AudioTrackConstructor = AudioTrackConstructor;
    gEGEnv->AudioTrackWrite = AudioTrackWrite;
    gEGEnv->AudioTrackPlay = AudioTrackPlay;
    gEGEnv->AudioTrackPause = AudioTrackPause;
    gEGEnv->AudioTrackStop = AudioTrackStop;
    gEGEnv->AudioTrackFlush = AudioTrackFlush;
    gEGEnv->AudioTrackReleaseEx = AudioTrackReleaseEx;
    gEGEnv->AudioTrackGetMinBufferSize = AudioTrackGetMinBufferSize;
    gEGEnv->AudioTrackGetPlaybackHeadPosition = AudioTrackGetPlaybackHeadPosition;
    gEGEnv->AudioTrackSetStereoVolume = AudioTrackSetStereoVolume;

    //ArrayOf<T>
    gEGEnv->NewArrayOfString = NewArrayOfString;
    gEGEnv->NewArrayOfByte = NewArrayOfByte;
    gEGEnv->ArrayOfByteGetLength = ArrayOfByteGetLength;
    gEGEnv->ArrayOfIntGetLength = ArrayOfIntGetLength;
    gEGEnv->ArrayOfFloatGetLength = ArrayOfFloatGetLength;
    gEGEnv->ArrayOfStringGetLength = ArrayOfStringGetLength;
    gEGEnv->ArrayOfEObjectGetLength = ArrayOfEObjectGetLength;
    gEGEnv->ArrayOfByteGetPayload = ArrayOfByteGetPayload;
    gEGEnv->ArrayOfIntGetPayload = ArrayOfIntGetPayload;
    gEGEnv->ArrayOfGetContent = ArrayOfGetContent;
    gEGEnv->ArrayOfStringSetContent = ArrayOfStringSetContent;
    gEGEnv->ArrayOfIntGetContent = ArrayOfIntGetContent;
    gEGEnv->ArrayOfDoubleGetContent = ArrayOfDoubleGetContent;
    gEGEnv->ArrayOfFloatGetContent = ArrayOfFloatGetContent;
    gEGEnv->ArrayOfEObjectGetContent = ArrayOfEObjectGetContent;
    gEGEnv->ArrayOfByteFree = ArrayOfByteFree;
    gEGEnv->ArrayOfIntFree = ArrayOfIntFree;
    gEGEnv->ArrayOfFloatFree = ArrayOfFloatFree;
    gEGEnv->ArrayOfStringFree = ArrayOfStringFree;
    gEGEnv->ArrayOfEObjectFree = ArrayOfEObjectFree;
    gEGEnv->SetByteArrayRegion = SetByteArrayRegion;

    gEGEnv->FreeEObject = FreeEObject;
    gEGEnv->FreeCObject = FreeCObject;
    gEGEnv->FreeCharBuffer = FreeCharBuffer;

    gEGEnv->IBitmapGetInfo = IBitmapGetInfo;
    gEGEnv->IBitmapLockPixels = IBitmapLockPixels;
    gEGEnv->IBitmapUnlockPixels = IBitmapUnlockPixels;

    gEGEnv->ANativeWindowFromSurface = ANativeWindowFromSurface;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
