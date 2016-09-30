elog("====ViewConfigurationCompat.js====begin====");

module.exports = function(aoElastos, aoActivity){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    var IView__VISIBLE = 0x00000000;

//--------common definition----end----

//--------.java----begin----

/*
 * Copyright (C) 2011 The Android Open Source Project
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

// package android.support.v4.view;

// import android.view.ViewConfiguration;

/**
 * Helper for accessing features in {@link ViewConfiguration}
 * introduced after API level 4 in a backwards compatible fashion.
 */
// public class ViewConfigurationCompat {
class ViewConfigurationCompat {
    /**
     * Interface for the full API.
     */
    // interface ViewConfigurationVersionImpl {
    //     public int getScaledPagingTouchSlop(ViewConfiguration config);
    //     public boolean hasPermanentMenuKey(ViewConfiguration config);
    // }
    static get ViewConfigurationVersionImpl() {
        return class {};
    }

    /**
     * Interface implementation that doesn't use anything about v4 APIs.
     */
    // static class BaseViewConfigurationVersionImpl implements ViewConfigurationVersionImpl {
    //     @Override
    //     public int getScaledPagingTouchSlop(ViewConfiguration config) {
    //         return config.getScaledTouchSlop();
    //     }

    //     @Override
    //     public boolean hasPermanentMenuKey(ViewConfiguration config) {
    //         // Pre-HC devices will always have a menu button
    //         return true;
    //     }
    // }
    static get BaseViewConfigurationVersionImpl() {
        return class _ extends this.ViewConfigurationVersionImpl {
            GetScaledPagingTouchSlop(config) {
                return config.GetScaledTouchSlop();
            }

            HasPermanentMenuKey(config) {
                // Pre-HC devices will always have a menu button
                return true;
            }
        };
    }


    // class ViewConfigurationCompatFroyo {
    //     public static int getScaledPagingTouchSlop(ViewConfiguration config) {
    //         return config.getScaledPagingTouchSlop();
    //     }
    // }
    static get ViewConfigurationCompatFroyo() {
        return class _ {
            static GetScaledPagingTouchSlop(config) {
                return config.GetScaledPagingTouchSlop();
            }
        }
    }

    /**
     * Interface implementation for devices with at least v8 APIs.
     */
    // static class FroyoViewConfigurationVersionImpl extends BaseViewConfigurationVersionImpl {
    //     @Override
    //     public int getScaledPagingTouchSlop(ViewConfiguration config) {
    //         return ViewConfigurationCompatFroyo.getScaledPagingTouchSlop(config);
    //     }
    // }
    static get FroyoViewConfigurationVersionImpl() {
        return class _ extends this.BaseViewConfigurationVersionImpl {
            //@Override
            GetScaledPagingTouchSlop(config) {
                return ViewConfigurationCompat.ViewConfigurationCompatFroyo.GetScaledPagingTouchSlop(config);
            }
        };
    }

    /**
     * Interface implementation for devices with at least v11 APIs.
     */
    // static class HoneycombViewConfigurationVersionImpl extends FroyoViewConfigurationVersionImpl {
    //     @Override
    //     public boolean hasPermanentMenuKey(ViewConfiguration config) {
    //         // There is no way to check on Honeycomb so we assume false
    //         return false;
    //     }
    // }
    static get HoneycombViewConfigurationVersionImpl() {
        return class _ extends this.FroyoViewConfigurationVersionImpl {
            //@Override
            HasPermanentMenuKey(config) {
                // There is no way to check on Honeycomb so we assume false
                return false;
            }
        };
    }

    /**
     * Interface implementation for devices with at least v14 APIs.
     */
    // static class IcsViewConfigurationVersionImpl extends HoneycombViewConfigurationVersionImpl {
    //     @Override
    //     public boolean hasPermanentMenuKey(ViewConfiguration config) {
    //         return ViewConfigurationCompatICS.hasPermanentMenuKey(config);
    //     }
    // }
    static get IcsViewConfigurationVersionImpl() {
        return class _ extends this.HoneycombViewConfigurationVersionImpl {
            //@Override
            HasPermanentMenuKey(config) {
                return ViewConfigurationCompatICS.HasPermanentMenuKey(config);
            }
        }
    }

    /**
     * Select the correct implementation to use for the current platform.
     */
    // static final ViewConfigurationVersionImpl IMPL;
    // static {
    //     if (android.os.Build.VERSION.SDK_INT >= 14) {
    //         IMPL = new IcsViewConfigurationVersionImpl();
    //     } else if (android.os.Build.VERSION.SDK_INT >= 11) {
    //         IMPL = new HoneycombViewConfigurationVersionImpl();
    //     } else if (android.os.Build.VERSION.SDK_INT >= 8) {
    //         IMPL = new FroyoViewConfigurationVersionImpl();
    //     } else {
    //         IMPL = new BaseViewConfigurationVersionImpl();
    //     }
    // }
    static get IMPL() {
        var SDK_INT = 21;    //android.os.Build.VERSION.SDK_INT
        var _IMPL;
        if (SDK_INT >= 14) {
            _IMPL = new this.IcsViewConfigurationVersionImpl();
        } else if (SDK_INT >= 11) {
            _IMPL = new HoneycombViewConfigurationVersionImpl();
        } else if (SDK_INT >= 8) {
            _IMPL = new FroyoViewConfigurationVersionImpl();
        } else {
            _IMPL = new BaseViewConfigurationVersionImpl();
        }
        ViewConfigurationCompat._IMPL = _IMPL;
        return (ViewConfigurationCompat._IMPL = _IMPL);
    }

    // -------------------------------------------------------------------

    /**
     * Call {@link ViewConfiguration#getScaledPagingTouchSlop()}.
     * If running on a pre-{@link android.os.Build.VERSION_CODES#FROYO} device,
     * returns {@link ViewConfiguration#getScaledTouchSlop()}.
     */
    // public static int getScaledPagingTouchSlop(ViewConfiguration config) {
    //     return IMPL.getScaledPagingTouchSlop(config);
    // }
    static GetScaledPagingTouchSlop(config) {
        elog("====ViewConfigurationCompat::GetScaledPagingTouchSlop====begin====");
        return this.IMPL.GetScaledPagingTouchSlop(config);
    }

    /**
     * Report if the device has a permanent menu key available to the user, in a backwards
     * compatible way.
     */
    // public static boolean hasPermanentMenuKey(ViewConfiguration config) {
    //     return IMPL.hasPermanentMenuKey(config);
    // }
    static HasPermanentMenuKey(config) {
        elog("====ViewConfigurationCompat::HasPermanentMenuKey====begin====");
        return this.IMPL.HasPermanentMenuKey(config);
    }
}   //class ViewConfigurationCompat


//--------.java----end----

    return ViewConfigurationCompat;
};  //module.exports

elog("====ViewConfigurationCompat.js====end====");