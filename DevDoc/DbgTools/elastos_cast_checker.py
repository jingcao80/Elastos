# elastos_cast_checker.py
# encoding: UTF-8
# usages:
#        sudo chmod a+x elastos_cast_checker.py
#        python elastos_cast_checker.py scan_path log_filepath

import re
import os
import sys

def read_file(path):
    lines = []
    if (path.endswith('.cpp') or path.endswith('.h')):
        if(os.path.isfile(path)):
            handle = open(path, 'r')
            for line in handle:
                lines.append(line.strip())
            handle.close()
    return lines

def find_declare_match(param, line):
    pattern = re.compile(r'AutoPtr\s*<(.*)>\s*(.*)'+'[, ]'+param+'[; ,]')
    return pattern.search(line)


def check_declare_match(usedType, param, declLine):
    pattern = re.compile(r'AutoPtr\s*<\s*'+usedType+'\s*>\s*(.*)'+'[, ]'+param+'[; ,]')
    return pattern.search(declLine)


def find_declare_line(param, lines, lineIndex):
    if len(lines) == 0:
        return -1

    for i in range(lineIndex, 0, -1):
        line = lines[i]
        if (len(line) > 1) and (line.startswith("//") == False):
            match = find_declare_match(param, line)
            if match:
                #print line, 'match', match.group()
                return i;
    return -1


def check_match(firstLog, logFile, cppFilepath, usedMatch, usedLineNum, declLine, declLineNum, isHeader = True):
    usedType = usedMatch.group(2)
    param = usedMatch.group(4)
    matchInfo = usedMatch.group()

    match = check_declare_match(usedType, param, declLine)
    if match == None:
        if firstLog:
            firstLog = False
            logInfo ='\n>> process file: ' + cppFilepath + '\n'
            logFile.write(logInfo)
            print logInfo

        fileInfo = ''
        if isHeader:
            fileInfo = 'in .h file'
        logInfo = "   > error: invalid using of {0} at line {1:d}, it is declared as {2} '{3}' at line {4:d}.\n" \
            .format(matchInfo, usedLineNum + 1, declLine, fileInfo, declLineNum + 1)
        logFile.write(logInfo)
        print logInfo
    else:
        #print 'match ', matchInfo, declLine
        return firstLog


def process_declare_line_in_header(logFile, firstLog, cppFilepath, match, lines, lineNum, headerFilepath):
    headerLines = read_file(headerFilepath)
    param = match.group(4)
    matchInfo = match.group()

    declLineNum = find_declare_line(param, headerLines, len(headerLines)-1)
    if (declLineNum != -1):
        declLine = headerLines[declLineNum]
        #print 'declLine', declLine
        firstLog = check_match(firstLog, logFile, cppFilepath, match, lineNum, declLine, declLineNum)
    else:
        logInfo = ''
        if firstLog:
            firstLog = False
            logInfo ='\n>> process file: ' + cppFilepath + '\n'
            logFile.write(logInfo)
            print logInfo

        if param.startswith('m'):
            logInfo = "   = warning: declaration for {0} at line {1:d} not found! is it declared in super class's .h file?\n".format(matchInfo, lineNum + 1)
        else:
            logInfo = "   = warning: declaration for {0} at line {1:d} not found!\n".format(matchInfo, lineNum + 1)
        logFile.write(logInfo)
        print logInfo
    return firstLog


def process_file(path, logFile):
    if path.endswith('.cpp') == False:
        return

    firstLog = True;
    lines = read_file(path)
    lineNum = 0
    for eachLine in lines:
        if (len(eachLine) > 1) and (eachLine.startswith("//") == False):
            pattern = re.compile(r'(\()(I\w*)(\*\*\)&)([a-zA-Z]\w*)(\))')
            match = pattern.search(eachLine)
            if match:
                #print match.group() match.groups()
                #print match.group(2), match.group(4)
                usedType = match.group(2)
                param = match.group(4)

                # do not check weak-reference Resolve
                if usedType == 'IInterface' and eachLine.find('->Resolve(') != -1:
                    pass
                elif isIgnored(path, usedType, param) == True:
                    pass
                else:
                    declLineNum = find_declare_line(param, lines, lineNum)
                    if (declLineNum != -1):
                        declLine = lines[declLineNum]
                        #print 'declLine', declLine
                        firstLog = check_match(firstLog, logFile, path, match, lineNum, declLine, declLineNum, False)
                    else:
                        headerFilepath = path.replace("/src/", "/inc/").replace(".cpp", ".h")
                        firstLog = process_declare_line_in_header(logFile, firstLog, path, match, lines, lineNum, headerFilepath)
        lineNum = lineNum +1


def process_dir(path, logFile):
    listfile = os.listdir(path)
    for filename in listfile:
        filepath = path + '/' + filename
        if(os.path.isdir(filepath)):
            # exclude hidden dirs
            if(filename[0] == '.'):
                pass
            else:
                # skip bak dirs
                if filename != 'bak':
                    process_dir(filepath, logFile)
        elif(os.path.isfile(filepath)):
            process_file(filepath, logFile)

def summarize_log(logPath):
    if(os.path.isfile(logPath)):
        errorCount = 0
        warningCount = 0

        # summarize
        logFile = open(logPath, 'r')
        for line in logFile:
            line = line.strip()
            if line.startswith('> error:') == True:
                errorCount = errorCount + 1
            elif line.startswith('= warning:') == True:
                warningCount = warningCount + 1
        logFile.close()

        # log
        logFile = open(logPath, 'a')
        logInfo = '\nresults: {0:d} errors, {1:d} warnings.'.format(errorCount, warningCount)
        logFile.write(logInfo)
        print logInfo
        logFile.close()

def process(path, logPath):
    if(os.path.isfile(logPath)):
        os.remove(logPath)
    logFile = open(logPath, 'a')
    if(os.path.isdir(path)):
        process_dir(path, logFile)
    elif(os.path.isfile(path)):
        process_file(path, logFile)
    else:
        print 'invalid path:', path
    logFile.close()
    summarize_log(logPath)

def get_finename(path):
    index = path.rfind('/');
    if index != -1:
        index = path.rfind('/', 0, index)
    if index != -1:
        filename = path[index:len(path)]
    else:
        filename = path
    return filename

# def log_ignored(path, usedType, param):
#     if path.find('/webkit/') != -1:
#         return

#     key = generate_ignored_key(path, usedType, param)
#     if key in ignored_list:
#         return

#     ignored_file = '/home/kesalin/Elastos5/DevDoc/DbgTools/elastos_cast_checker.ignored'
#     logFile = open(ignored_file, 'a')
#     logFile.write('    \'' + key + '\',\n')
#     logFile.close()

def generate_ignored_key(path, usedType, param):
    filename = get_finename(path)
    key = '{0}#{1}#{2}'.format(filename, usedType, param);
    return key

def isIgnored(path, usedType, param):
    key = generate_ignored_key(path, usedType, param)
    return key in ignored_list

############################################################################################################
ignored_list = [
    '/userfunc/userfunc.cpp#IInterface#pClassObject',
    '/launcher2/AppsCustomizePagedView.cpp#IBitmapCache#mCachedShortcutPreviewBitmap',
    '/launcher2/AppsCustomizePagedView.cpp#IPaintCache#mCachedShortcutPreviewPaint',
    '/launcher2/AppsCustomizePagedView.cpp#ICanvasCache#mCachedShortcutPreviewCanvas',
    '/launcher2/AppsCustomizePagedView.cpp#ICanvasCache#mCachedAppWidgetPreviewCanvas',
    '/launcher2/AppsCustomizePagedView.cpp#IRectCache#mCachedAppWidgetPreviewSrcRect',
    '/launcher2/AppsCustomizePagedView.cpp#IRectCache#mCachedAppWidgetPreviewDestRect',
    '/launcher2/AppsCustomizePagedView.cpp#IPaintCache#mCachedAppWidgetPreviewPaint',
    '/launcher2/Launcher.cpp#IAppWidgetProviderInfo#appWidgetInfo',
    '/launcher2/Launcher.cpp#IBundle#appSearchData',
    '/pinyinime/CEnvironmentHelper.cpp#IEnvironment#mInstance',
    '/phone/CPhoneStatusBar.cpp#IStatusBarKeyguardViewManager#mStatusBarKeyguardViewManager',
    '/framework/CTestSuite.cpp#IInterface#each',
    '/firewall/StringFilter.cpp#IPatternMatcher#mPattern',
    '/accessibility/AccessibilityManagerService.cpp#ISparseArray#mPendingEvents',
    '/wm/CSession.cpp#ISurfaceSession#mSurfaceSession',
    '/p2p/WifiP2pServiceImpl.cpp#IEditText#pin',
    '/server/AlarmManagerService.cpp#IArrayMap#mFilterStats',
    '/pm/CPackageInstallerService.cpp#IRemoteCallbackList#mCallbacks',
    '/server/CVibratorService.cpp#IThread#mThread',
    '/app/WindowDecorActionBar.cpp#IWeakReference#mCustomView',
    '/app/ProcessStats.cpp#IArrayMap#mProcesses',
    '/widget/ActionBarContextView.cpp#IActionMenuPresenter#mActionMenuPresenter',
    '/widget/ActionBarView.cpp#IActionMenuPresenter#mActionMenuPresenter',
    '/inputmethodservice/CIInputMethodSessionWrapper.cpp#ISparseArray#mPendingEvents',
    '/net/ProxyDataTracker.cpp#INetworkInfo#mNetworkInfo',
    '/net/ProxyDataTracker.cpp#ILinkProperties#mLinkProperties',
    '/net/ProxyDataTracker.cpp#INetworkCapabilities#mNetworkCapabilities',
    '/net/DhcpResults.cpp#IInetAddress#mGateway',
    '/animation/Keyframe.cpp#IInteger32#mValue',
    '/animation/Keyframe.cpp#IFloat#mValue',
    '/view/InputEventReceiver.cpp#IMessageQueue#mMessageQueue',
    '/view/View.cpp#IInterpolator#mScrollBarInterpolator',
    '/animation/GridLayoutAnimationController.cpp#ILinearInterpolator#mInterpolator',
    '/animation/GridLayoutAnimationController.cpp#IRandom#mRandomizer',
    '/view/TextureView.cpp#IPaint#mLayerPaint',
    '/view/InputDevice.cpp#IArrayList#mMotionRanges',
    '/soundtrigger/CSoundTriggerKeyphraseSoundModel.cpp#IUUID#mUuid',
    '/soundtrigger/CSoundTriggerKeyphraseSoundModel.cpp#IUUID#mVendorUuid',
    '/media/WebVttRenderingWidget.cpp#ICaptioningManagerCaptionStyle#mCaptionStyle',
    '/media/TtmlRenderer.cpp#IXmlPullParser#mParser',
    '/media/SubtitleTrack.cpp#IInterface#mNextRunAtEndTimeMs',
    '/widget/TabHost.cpp#IView#mTabContent',
    '/widget/Spinner.cpp#IAlertDialog#mPopup',
    '/res/CResources.cpp#IFormatter#f',
    '/pm/PackageParser.cpp#IPermissionInfo#mInfo',
    '/pm/PackageParser.cpp#IPermissionGroupInfo#mInfo',
    '/voice/VoiceInteractionSession.cpp#IWeakReference#mSession',
    '/bluetooth/QBluetoothAdapter.cpp#IWeakReference#mAdapter',
    '/reflection/CClassInfo.cpp#IInterface#obj',
    '/reflection/CObjInfoList.cpp#IModuleInfo#iModInfo',
    '/reflection/CObjInfoList.cpp#IInterface#interfaceObj',
    '/reflection/CObjInfoList.cpp#IInterface#iMethodInfo',
    '/android_linux/CRemoteParcel.cpp#IInterface#parcelable',
    '/linux_rpc/CRemoteParcel.cpp#IInterface#parcelable',
    '/marshal/metainfos.cpp#IInterface#modInfo',
    '/math/BigIntegerTest.cpp#IBigInteger#xNumber',
    '/Quintet/test.cpp#IBigInteger#i1',
    '/url/CJarURLConnectionImpl.cpp#IURLConnection#mJarFileURLConnection',
    '/text/SimpleDateFormat.cpp#INumberFormat#mNumberFormat',
    '/text/SimpleDateFormat.cpp#ICalendar#mCalendar',
    '/text/RuleBasedCollator.cpp#IRuleBasedCollatorICU#mICUColl',
    '/net/CPlainDatagramSocketImpl.cpp#IFileDescriptor#mFd',
    '/net/CMulticastSocket.cpp#IDatagramSocketImpl#mImpl',
    '/net/PlainSocketImpl.cpp#IInetAddress#mAddress',
    '/utility/Collections.cpp#IListIterator#mIterator',
    '/utility/CTreeMap.cpp#IWeakReference#mKeySet',
    '/x500/CX500Principal.cpp#IName#mDn',
]

############################################################################################################

#results: 101 errors, 59 warnings.
scan_path = '/home/kesalin/Elastos5/Sources/'                                       # default sacn dir
log_filepath = '/home/kesalin/Elastos5/DevDoc/DbgTools/elastos_cast_checker.log'    # default log file path

print "python  :", sys.argv[0]
argc = len(sys.argv)
if argc == 3:
    scan_path = sys.argv[1]
    log_filepath = sys.argv[2]

print "process :", scan_path
print "log to  :", log_filepath
process(scan_path, log_filepath)

