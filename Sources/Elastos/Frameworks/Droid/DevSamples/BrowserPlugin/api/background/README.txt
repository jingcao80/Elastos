Now, this plugin need more work todo,
webkit doesn't draw the plugin on the webview,
reason for this is the region for the plugin is clipped out.
(external/webkit/Source/WebKit/elastos/plugins/PluginWidgetAndroid.cpp->PluginWidgetAndroid::setSurfaceClip->m_isSurfaceClippedOut
 rootecause need find out)

From the external/webkit/Source/WebKit/elastos/plugins/ANPSurfaceInterface.cpp
the surface should be a subclass of SurfaceView.

