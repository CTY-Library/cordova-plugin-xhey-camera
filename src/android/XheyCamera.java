package org.xhey.cordova.camera;

import org.apache.cordova.*;
import org.json.*;

import java.util.Iterator;
import java.util.Map;
import java.util.HashMap;
import android.content.Intent;
import android.app.Activity;
import org.json.JSONArray;
import android.util.Base64;
import java.lang.reflect.Method;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class XheyCamera extends CordovaPlugin {
    private CallbackContext callbackContext;
    private static final int REQUEST_CODE = 10001;
    private String activityClassName = "com.xhey.xheycamerasdk.CameraActivity";
    private JSONObject savedConfig = null;
    private Map<String,String> keyMap = null;
    private JSONObject lastOptions = null;

    private void ensureKeyMap() {
        if (keyMap != null) return;
        keyMap = new HashMap<String,String>();
        // default logical -> SDK key mapping (most SDKs use same keys)
        keyMap.put("appid", "appid");
        keyMap.put("secretKey", "secretKey");
        keyMap.put("maxImageCount", "maxImageCount");
        keyMap.put("resourceDir", "resourceDir");
        keyMap.put("needPhotoConfirm", "needPhotoConfirm");
        keyMap.put("groupWatermarkId", "groupWatermarkId");
        keyMap.put("title", "title");
        keyMap.put("logoUrl", "logoUrl");
        keyMap.put("customInputItems", "customInputItems");
        keyMap.put("cameraPosition", "cameraPosition");
        // SDK literal keys detected in classes.jar — prefer these when present
        keyMap.put("userCommentObject", "userComment");
        // SDK uses "antiFake" in UserComment; accept both common names
        keyMap.put("antiFakeCode", "antiFake");
        keyMap.put("antiFake", "antiFake");
        // flashMode appears in JsBridge/c0 classes
        keyMap.put("flashMode", "flashMode");
        // extended/optional SDK parameters (logical -> expected SDK extra key)
        keyMap.put("antiFakeCode", "antiFakeCode");
        keyMap.put("watermarkContent", "watermarkContent");
        keyMap.put("watermarkTemplateId", "watermarkTemplateId");
        keyMap.put("watermarkPosition", "watermarkPosition");
        keyMap.put("watermarkText", "watermarkText");
        keyMap.put("watermarkImageUrl", "watermarkImageUrl");
        keyMap.put("watermarkOpacity", "watermarkOpacity");
        keyMap.put("watermarkScale", "watermarkScale");
        keyMap.put("watermarkRotation", "watermarkRotation");
        keyMap.put("watermarkOffsetX", "watermarkOffsetX");
        keyMap.put("watermarkOffsetY", "watermarkOffsetY");
        keyMap.put("includeTimestamp", "includeTimestamp");
        keyMap.put("timestampFormat", "timestampFormat");
        keyMap.put("includeGps", "includeGps");
        keyMap.put("locationLatitude", "locationLatitude");
        keyMap.put("locationLongitude", "locationLongitude");
        keyMap.put("altitude", "altitude");
        keyMap.put("burstCount", "burstCount");
        keyMap.put("burstInterval", "burstInterval");
        keyMap.put("imageQuality", "imageQuality");
        keyMap.put("outputFormat", "outputFormat");
        keyMap.put("saveToGallery", "saveToGallery");
        keyMap.put("enableCompression", "enableCompression");
        keyMap.put("compressionQuality", "compressionQuality");
        keyMap.put("needFaceDetect", "needFaceDetect");
        keyMap.put("verifyMode", "verifyMode");
        keyMap.put("customMetaFields", "customMetaFields");
        keyMap.put("cameraOrientation", "cameraOrientation");
        keyMap.put("flashMode", "flashMode");
        keyMap.put("enableHDR", "enableHDR");
        keyMap.put("mirrorFrontCamera", "mirrorFrontCamera");
        keyMap.put("continuousShot", "continuousShot");
    }

    private String mapKey(String logical) {
        ensureKeyMap();
        if (keyMap.containsKey(logical)) return keyMap.get(logical);
        return logical;
    }

    private void putExtraWithKeyMap(Intent intent, String logical, String value) {
        intent.putExtra(mapKey(logical), value);
    }

    @Override
    public boolean execute(String action, JSONArray args, CallbackContext callbackContext) throws JSONException {
        if ("configure".equals(action)) {
            // save persistent config (non-sensitive)
            JSONObject cfg = args.optJSONObject(0);
            if (cfg != null) {
                savedConfig = cfg;
                callbackContext.success("configured");
            } else {
                callbackContext.error("no_config");
            }
            return true;
        }

        if ("takePhoto".equals(action)) {
            this.callbackContext = callbackContext;
            JSONObject options = args.optJSONObject(0);
            if (options != null) {
                activityClassName = options.optString("activityClassName", activityClassName);
            }

            try {
                Activity activity = cordova.getActivity();
                Intent intent = new Intent();
                // Use explicit class name so user can override if SDK package differs
                intent.setClassName(activity.getPackageName(), activityClassName);

                // credentials from resources (installed via plugin.xml)
                int idApp = activity.getResources().getIdentifier("xhey_appid", "string", activity.getPackageName());
                int idSec = activity.getResources().getIdentifier("xhey_secret_key", "string", activity.getPackageName());
                String appidVal = null;
                String secretVal = null;
                if (idApp != 0) appidVal = activity.getString(idApp);
                if (idSec != 0) secretVal = activity.getString(idSec);
                // Fallback: try reading from application meta-data in AndroidManifest (if present)
                if ((appidVal == null || appidVal.length() == 0) || (secretVal == null || secretVal.length() == 0)) {
                    try {
                        android.content.pm.ApplicationInfo ai = activity.getPackageManager().getApplicationInfo(activity.getPackageName(), android.content.pm.PackageManager.GET_META_DATA);
                        if (ai != null && ai.metaData != null) {
                            if ((appidVal == null || appidVal.length() == 0) && ai.metaData.containsKey("xhey_appid")) {
                                Object v = ai.metaData.get("xhey_appid");
                                if (v != null) appidVal = String.valueOf(v);
                            }
                            if ((secretVal == null || secretVal.length() == 0) && ai.metaData.containsKey("xhey_secret_key")) {
                                Object v = ai.metaData.get("xhey_secret_key");
                                if (v != null) secretVal = String.valueOf(v);
                            }
                        }
                    } catch (Exception e) {
                        // ignore and continue
                    }
                }
                if (appidVal != null && appidVal.length() > 0) putExtraWithKeyMap(intent, "appid", appidVal);
                if (secretVal != null && secretVal.length() > 0) putExtraWithKeyMap(intent, "secretKey", secretVal);

                // merge savedConfig and runtime options (runtime overrides saved)
                JSONObject merged = new JSONObject();
                if (savedConfig != null) {
                    for (Iterator<String> it = savedConfig.keys(); it.hasNext(); ) {
                        String k = it.next(); merged.put(k, savedConfig.opt(k));
                    }
                }
                if (options != null) {
                    for (Iterator<String> it = options.keys(); it.hasNext(); ) {
                        String k = it.next(); merged.put(k, options.opt(k));
                    }
                }

                // remember last runtime options for onActivityResult handling
                lastOptions = merged;

                // apply merged config to intent extras
                for (Iterator<String> it = merged.keys(); it.hasNext(); ) {
                    String k = it.next(); Object v = merged.opt(k);
                    if (v == null) continue;
                    if (v instanceof Number) {
                        intent.putExtra(mapKey(k), ((Number)v).intValue());
                    } else if (v instanceof Boolean) {
                        intent.putExtra(mapKey(k), (Boolean)v);
                    } else {
                        intent.putExtra(mapKey(k), String.valueOf(v));
                    }
                }

                // ensure resourceDir present
                intent.putExtra(mapKey("resourceDir"), "file:///android_asset/XheyCameraSDKAssets");

                cordova.setActivityResultCallback(this);
                cordova.getActivity().startActivityForResult(intent, REQUEST_CODE);
                return true;
            } catch (Exception e) {
                callbackContext.error(e.getMessage());
                return false;
            }
        } else if ("startPreview".equals(action)) {
            JSONObject options = args.optJSONObject(0);
            if (options != null) {
                activityClassName = options.optString("activityClassName", activityClassName);
            }
            try {
                Activity activity = cordova.getActivity();
                Intent intent = new Intent();
                intent.setClassName(activity.getPackageName(), activityClassName);
                intent.putExtra("resourceDir", "file:///android_asset/XheyCameraSDKAssets");
                intent.putExtra("previewMode", true);
                String pos = "back";
                if (options != null && options.has("cameraPosition")) pos = options.optString("cameraPosition", pos);
                intent.putExtra("cameraPosition", pos);
                cordova.getActivity().startActivity(intent);
                callbackContext.success("preview_started");
            } catch (Exception e) {
                callbackContext.error(e.getMessage());
            }
            return true;
        } else if ("stopPreview".equals(action)) {
            try {
                // Try to dismiss SDK activity via reflection
                Class<?> cls = Class.forName(activityClassName);
                // try static dismiss()
                try {
                    Method dismiss = cls.getMethod("dismiss");
                    dismiss.invoke(null);
                    callbackContext.success("preview_stopped");
                    return true;
                } catch (NoSuchMethodException e) {
                    // try getInstance().dismiss()
                    try {
                        Method getInst = cls.getMethod("getInstance");
                        Object inst = getInst.invoke(null);
                        if (inst != null) {
                            Method dismiss2 = inst.getClass().getMethod("dismiss");
                            dismiss2.invoke(inst);
                            callbackContext.success("preview_stopped");
                            return true;
                        }
                    } catch (Exception ex) {
                        // fallthrough
                    }
                }

                // fallback: try JsBridge.close() then send broadcast to request close
                try {
                    Class<?> js = Class.forName("com.xhey.xheycamerasdk.JsBridge");
                    try {
                        Method close = js.getMethod("close");
                        close.invoke(null);
                        callbackContext.success("preview_stopped_via_jsbridge");
                        return true;
                    } catch (NoSuchMethodException nm) {
                        // ignore
                    }
                } catch (Exception ex) {
                    // ignore
                }
                Activity activity = cordova.getActivity();
                Intent b = new Intent("com.xhey.ACTION_CLOSE_CAMERA");
                activity.sendBroadcast(b);
                callbackContext.success("preview_stop_requested");
            } catch (Exception e) {
                callbackContext.error(e.getMessage());
            }
            return true;
        } else if ("switchCamera".equals(action)) {
            JSONObject options = args.optJSONObject(0);
            String pos = "back";
            if (options != null && options.has("cameraPosition")) pos = options.optString("cameraPosition", pos);
            try {
                // try static switch method
                Class<?> cls = Class.forName(activityClassName);
                try {
                    Method m = cls.getMethod("switchCamera", String.class);
                    m.invoke(null, pos);
                    callbackContext.success("switched");
                    return true;
                } catch (NoSuchMethodException e) {
                    // try instance method
                    try {
                        Method getInst = cls.getMethod("getInstance");
                        Object inst = getInst.invoke(null);
                        if (inst != null) {
                            Method m2 = inst.getClass().getMethod("switchCamera", String.class);
                            m2.invoke(inst, pos);
                            callbackContext.success("switched");
                            return true;
                        }
                    } catch (Exception ex) {
                        // fallthrough
                    }
                }

                // fallback: try JsBridge static method, then broadcast
                try {
                    Class<?> js = Class.forName("com.xhey.xheycamerasdk.JsBridge");
                    try {
                        Method jsSwitch = js.getMethod("switchCamera", String.class);
                        jsSwitch.invoke(null, pos);
                        callbackContext.success("switched_via_jsbridge");
                        return true;
                    } catch (NoSuchMethodException nm) {
                        // try close-like API
                    }
                } catch (Exception ex) {
                    // ignore
                }
                Activity activity = cordova.getActivity();
                Intent b = new Intent("com.xhey.ACTION_SWITCH_CAMERA");
                b.putExtra("cameraPosition", pos);
                activity.sendBroadcast(b);
                callbackContext.success("switch_requested");
            } catch (Exception e) {
                callbackContext.error(e.getMessage());
            }
            return true;
        }
        return false;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent intent) {
        if (requestCode != REQUEST_CODE) return;
        if (callbackContext == null) return;

        if (resultCode == Activity.RESULT_OK) {
            try {
                // Try to reflectively call static getResult() on the SDK activity class
                Class<?> cls = Class.forName(activityClassName);
                Method getResult = null;
                try {
                    getResult = cls.getMethod("getResult");
                } catch (NoSuchMethodException e) {
                    // ignore
                }

                if (getResult != null) {
                    Object resultObj = getResult.invoke(null);
                    if (resultObj != null) {
                        // Try to read "images" field or getImages() method
                        List<String> base64Images = new ArrayList<>();
                        List<String> fileUris = new ArrayList<>();
                        try {
                            // images is expected to be List<byte[]>
                            java.lang.reflect.Field imagesField = resultObj.getClass().getField("images");
                            Object imagesObj = imagesField.get(resultObj);
                            if (imagesObj instanceof List) {
                                List<?> imagesList = (List<?>) imagesObj;
                                int idx = 0;
                                for (Object img : imagesList) {
                                    if (img instanceof byte[]) {
                                        byte[] bytes = (byte[]) img;
                                        // determine returnType from lastOptions
                                        String returnType = "base64";
                                        if (lastOptions != null) returnType = lastOptions.optString("returnType", "base64");
                                        if ("file".equals(returnType)) {
                                            try {
                                                Activity act = cordova.getActivity();
                                                File cache = act.getCacheDir();
                                                File sub = new File(cache, "xhey_camera");
                                                if (!sub.exists()) sub.mkdirs();
                                                File outFile = new File(sub, String.valueOf(System.currentTimeMillis()) + "_" + (idx++) + ".jpg");
                                                FileOutputStream fos = new FileOutputStream(outFile);
                                                fos.write(bytes);
                                                fos.close();
                                                fileUris.add("file://" + outFile.getAbsolutePath());
                                            } catch (IOException ioex) {
                                                // fallback to base64 on error
                                                base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
                                            }
                                        } else {
                                            base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
                                        }
                                    }
                                }
                            }
                        } catch (NoSuchFieldException nsf) {
                            // try getImages()
                            try {
                                Method getImages = resultObj.getClass().getMethod("getImages");
                                Object imagesObj = getImages.invoke(resultObj);
                                if (imagesObj instanceof List) {
                                    List<?> imagesList = (List<?>) imagesObj;
                                    int idx = 0;
                                    for (Object img : imagesList) {
                                        if (img instanceof byte[]) {
                                            byte[] bytes = (byte[]) img;
                                            String returnType = "base64";
                                            if (lastOptions != null) returnType = lastOptions.optString("returnType", "base64");
                                            if ("file".equals(returnType)) {
                                                try {
                                                    Activity act = cordova.getActivity();
                                                    File cache = act.getCacheDir();
                                                    File sub = new File(cache, "xhey_camera");
                                                    if (!sub.exists()) sub.mkdirs();
                                                    File outFile = new File(sub, String.valueOf(System.currentTimeMillis()) + "_" + (idx++) + ".jpg");
                                                    FileOutputStream fos = new FileOutputStream(outFile);
                                                    fos.write(bytes);
                                                    fos.close();
                                                    fileUris.add("file://" + outFile.getAbsolutePath());
                                                } catch (IOException ioex) {
                                                    base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
                                                }
                                            } else {
                                                base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
                                            }
                                        }
                                    }
                                }
                            } catch (Exception ex) {
                                // fallthrough
                            }
                        }

                        if (!base64Images.isEmpty() || !fileUris.isEmpty()) {
                            // collect optional metadata
                                org.json.JSONObject meta = new org.json.JSONObject();
                                try {
                                    Field ucoField = null;
                                    try { ucoField = resultObj.getClass().getField("userCommentObject"); } catch (Exception e) { ucoField = null; }
                                    Object ucoObj = null;
                                    if (ucoField != null) {
                                        ucoObj = ucoField.get(resultObj);
                                    } else {
                                        try {
                                            Method getUco = resultObj.getClass().getMethod("getUserCommentObject");
                                            ucoObj = getUco.invoke(resultObj);
                                        } catch (Exception ex) {
                                            ucoObj = null;
                                        }
                                    }
                                    if (ucoObj != null) {
                                        Object serialized = objectToJson(ucoObj, 0);
                                        if (serialized instanceof JSONObject) meta.put("userCommentObject", (JSONObject)serialized);
                                        else if (serialized instanceof JSONArray) meta.put("userCommentObject", (JSONArray)serialized);
                                        else meta.put("userCommentObject", String.valueOf(serialized));
                                    }
                                } catch (Exception e) {
                                    // ignore
                                }

                                JSONObject out = new JSONObject();
                                if (!base64Images.isEmpty()) {
                                    JSONArray arr = new JSONArray();
                                    for (String b : base64Images) arr.put(b);
                                    out.put("images", arr);
                                }
                                if (!fileUris.isEmpty()) {
                                    JSONArray farr = new JSONArray();
                                    for (String u : fileUris) farr.put(u);
                                    out.put("fileUris", farr);
                                }
                                if (meta.length() > 0) out.put("meta", meta);
                                callbackContext.success(out);
                                // clear lastOptions
                                lastOptions = null;
                            return;
                        }
                    }
                }

                // Fallback: no images found, return OK
                callbackContext.success("OK");
            } catch (Exception e) {
                callbackContext.error("REFLECT_ERROR:" + e.getMessage());
            }
        } else if (resultCode == Activity.RESULT_CANCELED) {
            callbackContext.error("CANCELED");
        } else {
            callbackContext.error("UNKNOWN_RESULT_" + resultCode);
        }
    }

    // Helper: reflectively convert an object into JSONObject/JSONArray/primitive using limited recursion
    private Object objectToJson(Object obj, int depth) {
        if (obj == null) return null;
        if (depth > 6) return String.valueOf(obj);
        try {
            if (obj instanceof String || obj instanceof Number || obj instanceof Boolean) return obj;
            if (obj instanceof byte[]) return Base64.encodeToString((byte[])obj, Base64.NO_WRAP);
            if (obj instanceof List) {
                JSONArray a = new JSONArray();
                for (Object it : (List)obj) a.put(objectToJson(it, depth+1));
                return a;
            }
            JSONObject json = new JSONObject();
            // public fields
            for (Field f : obj.getClass().getFields()) {
                try {
                    Object v = f.get(obj);
                    json.put(f.getName(), objectToJson(v, depth+1));
                } catch (Exception e) { }
            }
            // getters
            for (Method m : obj.getClass().getMethods()) {
                String name = m.getName();
                if (m.getParameterTypes().length != 0) continue;
                if (name.equals("getClass")) continue;
                String prop = null;
                if (name.startsWith("get") && name.length() > 3) prop = Character.toLowerCase(name.charAt(3)) + name.substring(4);
                else if (name.startsWith("is") && name.length() > 2) prop = Character.toLowerCase(name.charAt(2)) + name.substring(3);
                if (prop == null) continue;
                if (json.has(prop)) continue; // prefer field over getter if both exist
                try {
                    Object v = m.invoke(obj);
                    json.put(prop, objectToJson(v, depth+1));
                } catch (Exception e) { }
            }
            return json;
        } catch (Exception e) {
            return String.valueOf(obj);
        }
    }
}
