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
import java.io.OutputStream;
import android.content.res.AssetManager;
import android.content.ContentValues;
import android.content.ContentResolver;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;

public class XheyCamera extends CordovaPlugin {
    private static final String TAG = "XheyCameraPlugin";
    private static final String BUILD_TAG = "20260521-item-fields-v4";
    private CallbackContext callbackContext;
    private static final int REQUEST_CODE = 10001;
    private static final int REQUEST_PERMISSION_WRITE_STORAGE = 10002;
    private String activityClassName = "com.xhey.xheycamerasdk.CameraActivity";
    private JSONObject savedConfig = null;
    private Map<String,String> keyMap = null;
    private JSONObject lastOptions = null;
    private Intent pendingCameraIntent = null;

    private boolean isNullOrEmpty(String v) {
        return v == null || v.trim().length() == 0;
    }

    private String firstNonEmpty(String... values) {
        if (values == null) return null;
        for (String v : values) {
            if (!isNullOrEmpty(v)) return v.trim();
        }
        return null;
    }

    private boolean assetExists(Activity activity, String path) {
        if (activity == null || isNullOrEmpty(path)) return false;
        AssetManager am = activity.getAssets();
        try {
            java.io.InputStream is = am.open(path);
            if (is != null) is.close();
            return true;
        } catch (IOException e) {
            return false;
        }
    }

    private String resolveResourceDir(Activity activity) {
        if (assetExists(activity, "XheyCameraSDKAssets/index.html")) {
            return "file:///android_asset/XheyCameraSDKAssets";
        }
        if (assetExists(activity, "www/XheyCameraSDKAssets/index.html")) {
            return "file:///android_asset/www/XheyCameraSDKAssets";
        }
        // Keep original default for compatibility.
        return "file:///android_asset/XheyCameraSDKAssets";
    }

    private String readMetaDataString(Activity activity, String... keys) {
        if (activity == null || keys == null || keys.length == 0) return null;
        try {
            android.content.pm.ApplicationInfo appInfo = activity.getPackageManager().getApplicationInfo(
                activity.getPackageName(),
                android.content.pm.PackageManager.GET_META_DATA
            );
            if (appInfo == null || appInfo.metaData == null) return null;

            for (String key : keys) {
                if (isNullOrEmpty(key)) continue;

                // Keep the same style as: appInfo.metaData.getString("...")
                String value = appInfo.metaData.getString(key);
                if (!isNullOrEmpty(value)) return value.trim();

                // Compatible fallback when meta-data value is @string resource id.
                Object raw = appInfo.metaData.get(key);
                if (raw instanceof Integer) {
                    try {
                        String res = activity.getString(((Integer) raw).intValue());
                        if (!isNullOrEmpty(res)) return res.trim();
                    } catch (Exception ignore) {
                        // ignore and continue
                    }
                } else if (raw != null) {
                    String s = String.valueOf(raw);
                    if (!isNullOrEmpty(s)) return s.trim();
                }
            }
        } catch (Exception e) {
            // ignore and continue
        }
        return null;
    }

    private void ensureActivityClassAvailable() throws ClassNotFoundException {
        Class.forName(activityClassName);
    }

    private String getReturnType() {
        if (lastOptions == null) return "base64";
        return lastOptions.optString("returnType", "base64");
    }

    private boolean shouldSaveToGallery() {
        if (lastOptions == null) return true;
        return lastOptions.optBoolean("saveToGallery", true);
    }

    private String getGalleryRelativePath() {
        String fallback = Environment.DIRECTORY_DOWNLOADS + "/XheyCamera";
        if (lastOptions == null) return fallback;
        String configured = firstNonEmpty(
            lastOptions.optString("downloadRelativePath", null),
            lastOptions.optString("galleryRelativePath", null)
        );
        if (isNullOrEmpty(configured)) return fallback;
        String normalized = configured.trim().replace('\\', '/');
        while (normalized.startsWith("/")) normalized = normalized.substring(1);
        while (normalized.endsWith("/")) normalized = normalized.substring(0, normalized.length() - 1);
        if (isNullOrEmpty(normalized)) return fallback;
        return normalized;
    }

    private boolean isDownloadRelativePath(String relativePath) {
        if (isNullOrEmpty(relativePath)) return false;
        String p = relativePath.toLowerCase();
        String d = Environment.DIRECTORY_DOWNLOADS.toLowerCase();
        return p.equals(d) || p.startsWith(d + "/");
    }

    private void notifyMediaScanner(Activity activity, Uri uri) {
        if (activity == null || uri == null) return;
        try {
            activity.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
        } catch (Exception ignore) {
            // ignore
        }
    }

    private byte[] decodeBase64ImageString(String raw) {
        if (isNullOrEmpty(raw)) return null;
        String v = raw.trim();
        int comma = v.indexOf(',');
        if (v.startsWith("data:") && comma > 0) {
            v = v.substring(comma + 1);
        }
        try {
            return Base64.decode(v, Base64.DEFAULT);
        } catch (Exception e) {
            return null;
        }
    }

    private byte[] extractImageBytes(Object img) {
        if (img == null) return null;
        if (img instanceof byte[]) return (byte[]) img;
        if (img instanceof String) return decodeBase64ImageString((String) img);

        if (img instanceof JSONObject) {
            JSONObject jo = (JSONObject) img;
            byte[] b = decodeBase64ImageString(jo.optString("image", null));
            if (b != null) return b;
            b = decodeBase64ImageString(jo.optString("data", null));
            if (b != null) return b;
            b = decodeBase64ImageString(jo.optString("base64", null));
            if (b != null) return b;
        }

        if (img instanceof Map) {
            try {
                Map<?, ?> m = (Map<?, ?>) img;
                Object v = m.get("image");
                if (v != null) {
                    byte[] b = decodeBase64ImageString(String.valueOf(v));
                    if (b != null) return b;
                }
                v = m.get("data");
                if (v != null) {
                    byte[] b = decodeBase64ImageString(String.valueOf(v));
                    if (b != null) return b;
                }
                v = m.get("base64");
                if (v != null) {
                    byte[] b = decodeBase64ImageString(String.valueOf(v));
                    if (b != null) return b;
                }
            } catch (Exception ignore) {
                // ignore and continue
            }
        }

        // SDK may return list items like { image: "data:image/jpeg;base64,..." }
        try {
            Field imageField = null;
            try {
                imageField = img.getClass().getField("image");
            } catch (Exception e) {
                imageField = null;
            }
            if (imageField != null) {
                Object val = imageField.get(img);
                if (val instanceof byte[]) return (byte[]) val;
                if (val != null) {
                    byte[] out = decodeBase64ImageString(String.valueOf(val));
                    if (out != null) return out;
                }
            }
        } catch (Exception ignore) {
            // ignore and continue
        }

        try {
            Method getImage = null;
            try {
                getImage = img.getClass().getMethod("getImage");
            } catch (Exception e) {
                getImage = null;
            }
            if (getImage != null) {
                Object val = getImage.invoke(img);
                if (val instanceof byte[]) return (byte[]) val;
                if (val != null) {
                    byte[] out = decodeBase64ImageString(String.valueOf(val));
                    if (out != null) return out;
                }
            }
        } catch (Exception ignore) {
            // ignore and continue
        }

        return null;
    }

    private void appendImageBytesFromAny(Object src, List<byte[]> out) {
        if (src == null || out == null) return;
        if (src instanceof List) {
            for (Object it : (List<?>) src) {
                appendImageBytesFromAny(it, out);
            }
            return;
        }
        if (src instanceof JSONArray) {
            JSONArray arr = (JSONArray) src;
            for (int i = 0; i < arr.length(); i++) {
                appendImageBytesFromAny(arr.opt(i), out);
            }
            return;
        }
        if (src instanceof JSONObject) {
            JSONObject jo = (JSONObject) src;
            byte[] b = extractImageBytes(jo);
            if (b != null && b.length > 0) {
                out.add(b);
                return;
            }
            Object nested = jo.opt("images");
            if (nested != null) appendImageBytesFromAny(nested, out);
            nested = jo.opt("capturedImages");
            if (nested != null) appendImageBytesFromAny(nested, out);
            nested = jo.opt("onCapturedImages");
            if (nested != null) appendImageBytesFromAny(nested, out);
            return;
        }

        byte[] b = extractImageBytes(src);
        if (b != null && b.length > 0) {
            out.add(b);
            return;
        }

        // JSON string/object string fallback (common in SDK bridges)
        if (src instanceof String) {
            String s = ((String) src).trim();
            if (s.startsWith("[") && s.endsWith("]")) {
                try {
                    appendImageBytesFromAny(new JSONArray(s), out);
                    return;
                } catch (Exception ignore) {
                    // ignore and continue
                }
            }
            if (s.startsWith("{") && s.endsWith("}")) {
                try {
                    appendImageBytesFromAny(new JSONObject(s), out);
                    return;
                } catch (Exception ignore) {
                    // ignore and continue
                }
            }
            // raw base64/data-url case
            b = decodeBase64ImageString(s);
            if (b != null && b.length > 0) {
                out.add(b);
            }
            return;
        }

        // Reflective single-item fallback: try well-known image field names
        Object nested = getObjectFieldValue(src, "image");
        if (nested == null) nested = getObjectFieldValue(src, "imageData");
        if (nested == null) nested = getObjectFieldValue(src, "data");
        if (nested == null) nested = invokeNoArgMethod(src, "getImage");
        if (nested == null) nested = invokeNoArgMethod(src, "getImageData");
        if (nested != null) {
            appendImageBytesFromAny(nested, out);
            if (!out.isEmpty()) return;
        }

        // Last resort: scan ALL declared fields of the item object for byte[] or base64 String
        // This handles obfuscated field names and SDK-internal item classes.
        try {
            Field[] allFields = src.getClass().getDeclaredFields();
            Log.i(TAG, "Scanning item fields, class=" + src.getClass().getName() + " fieldCount=" + allFields.length);
            for (Field df : allFields) {
                try {
                    df.setAccessible(true);
                    Object v = df.get(src);
                    if (v == null) continue;
                    Log.i(TAG, "  item field=" + df.getName() + " type=" + v.getClass().getName());
                    if (v instanceof byte[]) {
                        byte[] bytes = (byte[]) v;
                        if (bytes.length > 200) {
                            out.add(bytes);
                            return;
                        }
                    } else if (v instanceof String) {
                        byte[] bytes = decodeBase64ImageString((String) v);
                        if (bytes != null && bytes.length > 200) {
                            out.add(bytes);
                            return;
                        }
                    } else if (v instanceof List || v instanceof JSONArray) {
                        appendImageBytesFromAny(v, out);
                        if (!out.isEmpty()) return;
                    }
                } catch (Exception ignore) {
                    // ignore individual field errors
                }
            }
        } catch (Exception ignore) {
            // ignore
        }
    }

    private List<byte[]> extractCapturedImages(Object resultObj, Intent intent) {
        List<byte[]> out = new ArrayList<byte[]>();
        if (resultObj != null) {
            // Step 1: try well-known named fields / methods
            String[] fieldCandidates = new String[] {
                "images", "capturedImages", "onCapturedImages", "imageList", "result", "data"
            };
            for (String f : fieldCandidates) {
                Object v = getObjectFieldValue(resultObj, f);
                if (v != null) appendImageBytesFromAny(v, out);
            }

            String[] methodCandidates = new String[] {
                "getImages", "getCapturedImages", "getOnCapturedImages", "getImageList", "getResult", "getData"
            };
            for (String m : methodCandidates) {
                Object v = invokeNoArgMethod(resultObj, m);
                if (v != null) appendImageBytesFromAny(v, out);
            }

            // Step 2: enumerate ALL declared fields (handles obfuscated/private field names)
            // This is the key path for SDKs that obfuscate field names (a, b, c, ...).
            if (out.isEmpty()) {
                try {
                    Field[] declaredFields = resultObj.getClass().getDeclaredFields();
                    Log.i(TAG, "CameraActivity$Result declared fields count=" + declaredFields.length);
                    for (Field df : declaredFields) {
                        try {
                            df.setAccessible(true);
                            Object v = df.get(resultObj);
                            String typeName = v == null ? "null" : v.getClass().getName();
                            Log.i(TAG, "  field=" + df.getName() + " type=" + typeName);
                            if (v != null) {
                                appendImageBytesFromAny(v, out);
                            }
                        } catch (Exception ignore) {
                            // ignore individual field errors
                        }
                    }
                    Log.i(TAG, "After declared-fields scan, images found=" + out.size());
                } catch (Exception ignore) {
                    // ignore
                }
            }

            // Step 3: also scan superclass declared fields
            if (out.isEmpty()) {
                try {
                    Class<?> sup = resultObj.getClass().getSuperclass();
                    if (sup != null && !sup.equals(Object.class)) {
                        Field[] supFields = sup.getDeclaredFields();
                        Log.i(TAG, "Superclass " + sup.getName() + " declared fields count=" + supFields.length);
                        for (Field df : supFields) {
                            try {
                                df.setAccessible(true);
                                Object v = df.get(resultObj);
                                String typeName = v == null ? "null" : v.getClass().getName();
                                Log.i(TAG, "  super field=" + df.getName() + " type=" + typeName);
                                if (v != null) {
                                    appendImageBytesFromAny(v, out);
                                }
                            } catch (Exception ignore) {
                                // ignore
                            }
                        }
                    }
                } catch (Exception ignore) {
                    // ignore
                }
            }

            // Step 4: last resort — serialize all public fields/getters to JSON tree and scan
            if (out.isEmpty()) {
                try {
                    Object tree = objectToJson(resultObj, 0);
                    appendImageBytesFromAny(tree, out);
                } catch (Exception ignore) {
                    // ignore
                }
            }
        }

        // Intent extras fallback
        try {
            if (intent != null && intent.getExtras() != null) {
                android.os.Bundle extras = intent.getExtras();
                for (String key : extras.keySet()) {
                    Object val = extras.get(key);
                    appendImageBytesFromAny(val, out);
                }
            }
        } catch (Exception ignore) {
            // ignore
        }

        return out;
    }

    private Object getObjectFieldValue(Object obj, String fieldName) {
        if (obj == null || isNullOrEmpty(fieldName)) return null;
        try {
            Field f = obj.getClass().getField(fieldName);
            return f.get(obj);
        } catch (Exception ignore) {
            // try declared field next
        }
        try {
            Field f2 = obj.getClass().getDeclaredField(fieldName);
            f2.setAccessible(true);
            return f2.get(obj);
        } catch (Exception ignore) {
            return null;
        }
    }

    private Object invokeNoArgMethod(Object obj, String methodName) {
        if (obj == null || isNullOrEmpty(methodName)) return null;
        try {
            Method m = obj.getClass().getMethod(methodName);
            return m.invoke(obj);
        } catch (Exception ignore) {
            // try declared method next
        }
        try {
            Method m2 = obj.getClass().getDeclaredMethod(methodName);
            m2.setAccessible(true);
            return m2.invoke(obj);
        } catch (Exception ignore) {
            return null;
        }
    }

    private String buildOutputFileName(int idx) {
        return "xhey_" + System.currentTimeMillis() + "_" + idx + ".jpg";
    }

    private String buildExternalAbsolutePathHint(String relativePath, String fileName) {
        try {
            String base = Environment.getExternalStorageDirectory().getAbsolutePath();
            String rel = isNullOrEmpty(relativePath) ? (Environment.DIRECTORY_DOWNLOADS + "/XheyCamera") : relativePath;
            rel = rel.replace('\\', '/');
            while (rel.startsWith("/")) rel = rel.substring(1);
            return base + "/" + rel + "/" + fileName;
        } catch (Exception e) {
            return null;
        }
    }

    private String saveToCache(Activity activity, byte[] bytes, int idx) {
        try {
            File cache = activity.getCacheDir();
            File sub = new File(cache, "xhey_camera");
            if (!sub.exists()) sub.mkdirs();
            File outFile = new File(sub, String.valueOf(System.currentTimeMillis()) + "_" + idx + ".jpg");
            FileOutputStream fos = new FileOutputStream(outFile);
            fos.write(bytes);
            fos.close();
            return "file://" + outFile.getAbsolutePath();
        } catch (Exception e) {
            return null;
        }
    }

    private String saveToGallery(Activity activity, byte[] bytes, int idx, String fileName) {
        ContentResolver resolver = activity.getContentResolver();
        String relativePath = getGalleryRelativePath();
        ContentValues baseValues = new ContentValues();
        baseValues.put(MediaStore.MediaColumns.DISPLAY_NAME, fileName);
        baseValues.put(MediaStore.MediaColumns.MIME_TYPE, "image/jpeg");
        baseValues.put(MediaStore.MediaColumns.DATE_ADDED, System.currentTimeMillis() / 1000L);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            baseValues.put(MediaStore.MediaColumns.RELATIVE_PATH, relativePath);
            baseValues.put(MediaStore.MediaColumns.IS_PENDING, 1);
        }

        java.util.ArrayList<Uri> collections = new java.util.ArrayList<Uri>();
        boolean targetDownloads = isDownloadRelativePath(relativePath);
        if (targetDownloads) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                collections.add(MediaStore.Downloads.getContentUri(MediaStore.VOLUME_EXTERNAL_PRIMARY));
                // OEM fallback: some ROMs are more permissive with Images volume.
                collections.add(MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL_PRIMARY));
            }
            collections.add(MediaStore.Downloads.EXTERNAL_CONTENT_URI);
            collections.add(MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                collections.add(MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL_PRIMARY));
            }
            collections.add(MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        }

        for (Uri targetCollection : collections) {
            Uri uri = null;
            OutputStream os = null;
            try {
                ContentValues insertValues = new ContentValues(baseValues);
                uri = resolver.insert(targetCollection, insertValues);
                if (uri == null) continue;
                os = resolver.openOutputStream(uri);
                if (os == null) {
                    try { resolver.delete(uri, null, null); } catch (Exception ignore) { }
                    continue;
                }
                os.write(bytes);
                os.flush();
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    ContentValues done = new ContentValues();
                    done.put(MediaStore.MediaColumns.IS_PENDING, 0);
                    resolver.update(uri, done, null, null);
                }
                try { resolver.notifyChange(uri, null); } catch (Exception ignore) { }
                notifyMediaScanner(activity, uri);
                Log.i(TAG, "Saved image to media store: " + uri + ", relativePath=" + relativePath + ", collection=" + targetCollection);

                return uri.toString();
            } catch (Exception e) {
                if (uri != null) {
                    try { resolver.delete(uri, null, null); } catch (Exception ignore) { }
                }
                Log.w(TAG, "saveToGallery failed on collection: " + targetCollection, e);
            } finally {
                if (os != null) {
                    try { os.close(); } catch (Exception ignore) { }
                }
            }
        }

        // If MediaStore insert failed (common on <Q devices or missing permission),
        // fall back to writing directly to DCIM and scan to make it visible in gallery.
        try {
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
                // Need WRITE_EXTERNAL_STORAGE on pre-Q devices. If not granted, this will fail.
                java.lang.String state = Environment.getExternalStorageState();
                if (!Environment.MEDIA_MOUNTED.equals(state)) return null;
                java.io.File downloads = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
                java.io.File dir = new java.io.File(downloads, "XheyCamera");
                if (!dir.exists()) dir.mkdirs();
                java.io.File outFile = new java.io.File(dir, fileName);
                java.io.FileOutputStream fos = new java.io.FileOutputStream(outFile);
                fos.write(bytes);
                fos.close();
                // Ensure gallery picks it up
                try {
                    android.media.MediaScannerConnection.scanFile(activity, new String[]{outFile.getAbsolutePath()}, new String[]{"image/jpeg"}, null);
                } catch (Exception ignore) { }
                Log.i(TAG, "Saved image to legacy gallery path: " + outFile.getAbsolutePath());
                return "file://" + outFile.getAbsolutePath();
            }
        } catch (Exception ex) {
            Log.e(TAG, "legacy saveToGallery fallback failed", ex);
        }

        return null;
    }

    private int collectImageResult(Activity act, byte[] bytes, int idx, List<String> base64Images, List<String> fileUris, List<String> galleryUris, List<String> downloadUris, List<String> downloadPaths, List<String> saveErrors) {
        String returnType = getReturnType();
        boolean saveToGallery = shouldSaveToGallery();
        String outputFileName = buildOutputFileName(idx);
        String relativePath = getGalleryRelativePath();

        if ("file".equals(returnType)) {
            String fileUri = saveToGallery ? saveToGallery(act, bytes, idx, outputFileName) : saveToCache(act, bytes, idx);
            if (fileUri == null) {
                // fallback to cache, then base64
                fileUri = saveToCache(act, bytes, idx);
            }
            if (fileUri != null) {
                fileUris.add(fileUri);
                if (saveToGallery && fileUri.startsWith("content://")) {
                    galleryUris.add(fileUri);
                    downloadUris.add(fileUri);
                    String pathHint = buildExternalAbsolutePathHint(relativePath, outputFileName);
                    if (!isNullOrEmpty(pathHint)) downloadPaths.add(pathHint);
                } else if (saveToGallery && fileUri.startsWith("file://")) {
                    // Legacy fallback path may still succeed on older devices.
                    downloadPaths.add(fileUri.substring("file://".length()));
                }
            } else {
                base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
                if (saveToGallery) saveErrors.add("save_failed:file_mode");
            }
        } else {
            base64Images.add(Base64.encodeToString(bytes, Base64.NO_WRAP));
            if (saveToGallery) {
                String galleryUri = saveToGallery(act, bytes, idx, outputFileName);
                if (galleryUri != null) {
                    galleryUris.add(galleryUri);
                    downloadUris.add(galleryUri);
                    String pathHint = buildExternalAbsolutePathHint(relativePath, outputFileName);
                    if (!isNullOrEmpty(pathHint)) downloadPaths.add(pathHint);
                } else {
                    saveErrors.add("save_failed:base64_mode");
                }
            }
        }
        return idx + 1;
    }

    private boolean needsLegacyWritePermission() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && Build.VERSION.SDK_INT < Build.VERSION_CODES.Q;
    }

    private boolean hasWriteExternalPermission() {
        if (!needsLegacyWritePermission()) return true;
        Activity activity = cordova.getActivity();
        return activity.checkSelfPermission(android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == android.content.pm.PackageManager.PERMISSION_GRANTED;
    }

    private void requestWriteExternalPermission() {
        if (!needsLegacyWritePermission()) return;
        cordova.requestPermission(this, REQUEST_PERMISSION_WRITE_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE);
    }

    private void launchCameraIntent(Intent intent) throws Exception {
        cordova.setActivityResultCallback(this);
        try {
            cordova.getActivity().startActivityForResult(intent, REQUEST_CODE);
        } catch (android.content.ActivityNotFoundException anf) {
            int lastDot2 = activityClassName.lastIndexOf('.');
            if (lastDot2 > 0) {
                String targetPackage = activityClassName.substring(0, lastDot2);
                intent.setClassName(targetPackage, activityClassName);
                cordova.getActivity().startActivityForResult(intent, REQUEST_CODE);
            } else {
                throw anf;
            }
        }
    }

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
                // The activityClassName is a fully-qualified class name (e.g. com.xhey.xheycamerasdk.CameraActivity).
                // Use the host app package name as the component package so the merged
                // manifest entries for the SDK activity are resolved correctly.
                intent.setClassName(activity.getPackageName(), activityClassName);

                // credentials: first try Cordova plugin preferences, then resources, then manifest meta-data
                String appidVal = null;
                String secretVal = null;
                try {
                    if (this.preferences != null) {
                        // Try common preference keys used by different plugins/projects
                        appidVal = this.preferences.getString("APP_ID", null);
                        if (appidVal == null || appidVal.length() == 0) appidVal = this.preferences.getString("APPID", null);
                        if (appidVal == null || appidVal.length() == 0) appidVal = this.preferences.getString("appid", null);

                        secretVal = this.preferences.getString("SECRET_KEY", null);
                        if (secretVal == null || secretVal.length() == 0) secretVal = this.preferences.getString("SECRETKEY", null);
                        if (secretVal == null || secretVal.length() == 0) secretVal = this.preferences.getString("secret_key", null);
                    }
                } catch (Exception e) {
                    // ignore
                }
                // If not found in preferences, try resource strings (legacy behavior)
                if (appidVal == null || appidVal.length() == 0) {
                    int idApp = activity.getResources().getIdentifier("xhey_appid", "string", activity.getPackageName());
                    if (idApp != 0) appidVal = activity.getString(idApp);
                }
                if (secretVal == null || secretVal.length() == 0) {
                    int idSec = activity.getResources().getIdentifier("xhey_secret_key", "string", activity.getPackageName());
                    if (idSec != 0) secretVal = activity.getString(idSec);
                }
                // Fallback: try reading from application meta-data in AndroidManifest (if present)
                if ((appidVal == null || appidVal.length() == 0) || (secretVal == null || secretVal.length() == 0)) {
                    if (isNullOrEmpty(appidVal)) {
                        appidVal = readMetaDataString(activity,
                            "xhey_appid",
                            "APP_ID",
                            "APPID"
                        );
                    }
                    if (isNullOrEmpty(secretVal)) {
                        secretVal = readMetaDataString(activity,
                            "xhey_secret_key",
                            "SECRET_KEY",
                            "SECRETKEY"
                        );
                    }
                }
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
                Log.i(TAG, "Plugin build: " + BUILD_TAG);

                // Log merged options for debugging required-field / truephoto issues
                try {
                    Log.i(TAG, "Merged options: " + merged.toString());
                } catch (Exception ignore) { }

                // If credentials are not found from preferences/resources/meta-data,
                // allow runtime options/configure() values as a final source.
                if (isNullOrEmpty(appidVal)) {
                    appidVal = firstNonEmpty(
                        merged.optString("appid", null),
                        merged.optString("appId", null),
                        merged.optString("APPID", null),
                        merged.optString("APP_ID", null)
                    );
                }
                if (isNullOrEmpty(secretVal)) {
                    secretVal = firstNonEmpty(
                        merged.optString("secretKey", null),
                        merged.optString("secret_key", null),
                        merged.optString("SECRET_KEY", null),
                        merged.optString("SECRETKEY", null)
                    );
                }

                // Fail fast with a clear error instead of letting SDK crash with NPE.
                if (isNullOrEmpty(appidVal) || isNullOrEmpty(secretVal)) {
                    callbackContext.error("MISSING_CREDENTIALS: Please configure APPID and SECRET_KEY in config.xml plugin variables or pass appid/secretKey in options.");
                    return true;
                }

                putExtraWithKeyMap(intent, "appid", appidVal);
                putExtraWithKeyMap(intent, "secretKey", secretVal);
                // If caller requests to skip TruePhoto verification for testing
                try {
                    if (merged.optBoolean("skipTruePhoto", false)) {
                        // use mapped key for verifyMode; SDK may interpret 0 as disabled
                        try {
                            intent.putExtra(mapKey("verifyMode"), 0);
                            Log.i(TAG, "skipTruePhoto=true -> set verifyMode=0 on intent");
                        } catch (Exception ex) {
                            Log.w(TAG, "failed to set verifyMode extra", ex);
                        }
                    }
                } catch (Exception ignore) { }

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

                // Log final intent extras to help inspect what SDK receives
                try {
                    android.os.Bundle extras = intent.getExtras();
                    if (extras != null) {
                        for (String key : extras.keySet()) {
                            try { Log.i(TAG, "Intent extra: " + key + "=" + String.valueOf(extras.get(key))); } catch (Exception ex) { }
                        }
                    } else {
                        Log.i(TAG, "Intent has no extras");
                    }
                } catch (Exception ignore) { }

                // ensure resourceDir present
                String runtimeResourceDir = merged.optString("resourceDir", null);
                String resourceDir = isNullOrEmpty(runtimeResourceDir)
                    ? resolveResourceDir(activity)
                    : runtimeResourceDir;
                intent.putExtra(mapKey("resourceDir"), resourceDir);

                // Prevent crash when manifest contains activity but AAR class is not packaged.
                try {
                    ensureActivityClassAvailable();
                } catch (ClassNotFoundException cnf) {
                    callbackContext.error("SDK_ACTIVITY_CLASS_NOT_FOUND: " + activityClassName + ". Please verify AAR dependency packaging.");
                    return true;
                }

                if (shouldSaveToGallery() && !hasWriteExternalPermission()) {
                    pendingCameraIntent = intent;
                    requestWriteExternalPermission();
                    return true;
                }
                launchCameraIntent(intent);
                return true;
            } catch (Exception e) {
                Log.e(TAG, "takePhoto failed", e);
                String msg = e.getMessage();
                if (msg == null) msg = e.toString();
                callbackContext.error("TAKE_PHOTO_ERROR: " + msg);
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
                // ensure we target the SDK activity's package (not host app package)
                // Use host app package so the merged manifest resolves the component.
                intent.setClassName(activity.getPackageName(), activityClassName);
                String runtimeResourceDir = null;
                if (options != null) runtimeResourceDir = options.optString("resourceDir", null);
                String resourceDir = isNullOrEmpty(runtimeResourceDir)
                    ? resolveResourceDir(activity)
                    : runtimeResourceDir;
                intent.putExtra("resourceDir", resourceDir);
                intent.putExtra("previewMode", true);

                try {
                    ensureActivityClassAvailable();
                } catch (ClassNotFoundException cnf) {
                    callbackContext.error("SDK_ACTIVITY_CLASS_NOT_FOUND: " + activityClassName + ". Please verify AAR dependency packaging.");
                    return true;
                }

                String pos = "back";
                if (options != null && options.has("cameraPosition")) pos = options.optString("cameraPosition", pos);
                intent.putExtra("cameraPosition", pos);
                try {
                    cordova.getActivity().startActivity(intent);
                } catch (android.content.ActivityNotFoundException anf) {
                    int lastDot2 = activityClassName.lastIndexOf('.');
                    if (lastDot2 > 0) {
                        String targetPackage = activityClassName.substring(0, lastDot2);
                        intent.setClassName(targetPackage, activityClassName);
                        cordova.getActivity().startActivity(intent);
                    } else {
                        throw anf;
                    }
                }
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
        Log.i(TAG, "onActivityResult entered, requestCode=" + requestCode + ", resultCode=" + resultCode + ", build=" + BUILD_TAG);

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
                        Log.i(TAG, "SDK getResult object type=" + resultObj.getClass().getName());
                        // Try to read "images" field or getImages() method
                        List<String> base64Images = new ArrayList<>();
                        List<String> fileUris = new ArrayList<>();
                        List<String> galleryUris = new ArrayList<>();
                        List<String> downloadUris = new ArrayList<>();
                        List<String> downloadPaths = new ArrayList<>();
                        List<String> saveErrors = new ArrayList<>();
                        List<byte[]> capturedBytes = extractCapturedImages(resultObj, intent);
                        if (!capturedBytes.isEmpty()) {
                            Log.i(TAG, "Parsed images count=" + capturedBytes.size());
                            int idx = 0;
                            Activity act = cordova.getActivity();
                            for (byte[] bytes : capturedBytes) {
                                if (bytes != null && bytes.length > 0) {
                                    Log.i(TAG, "Parsed image bytes=" + bytes.length);
                                    idx = collectImageResult(act, bytes, idx, base64Images, fileUris, galleryUris, downloadUris, downloadPaths, saveErrors);
                                } else {
                                    saveErrors.add("parse_failed:image_item");
                                }
                            }
                        } else {
                            Log.w(TAG, "Could not parse images from result object and intent extras");
                            saveErrors.add("parse_failed:all_sources");
                        }

                        if (!base64Images.isEmpty() || !fileUris.isEmpty()) {
                            // collect optional metadata
                                org.json.JSONObject meta = new org.json.JSONObject();
                                try {
                                    Object ucoObj = getObjectFieldValue(resultObj, "userCommentObject");
                                    if (ucoObj == null) ucoObj = invokeNoArgMethod(resultObj, "getUserCommentObject");
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
                                if (!galleryUris.isEmpty()) {
                                    JSONArray garr = new JSONArray();
                                    for (String u : galleryUris) garr.put(u);
                                    out.put("galleryUris", garr);
                                }
                                if (!downloadUris.isEmpty()) {
                                    JSONArray darr = new JSONArray();
                                    for (String u : downloadUris) darr.put(u);
                                    out.put("downloadUris", darr);
                                }
                                if (!downloadPaths.isEmpty()) {
                                    JSONArray parr = new JSONArray();
                                    for (String p : downloadPaths) parr.put(p);
                                    out.put("downloadPaths", parr);
                                }
                                out.put("savedRelativePath", getGalleryRelativePath());
                                out.put("pluginBuildTag", BUILD_TAG);
                                if (!saveErrors.isEmpty()) {
                                    JSONArray serr = new JSONArray();
                                    for (String s : saveErrors) serr.put(s);
                                    out.put("saveErrors", serr);
                                }
                                if (meta.length() > 0) out.put("meta", meta);

                                // On Android 10+, saving to shared storage should not silently fail.
                                // If caller asked saveToGallery but nothing was persisted, return explicit error.
                                if (shouldSaveToGallery() && downloadUris.isEmpty() && downloadPaths.isEmpty()) {
                                    callbackContext.error("SAVE_TO_DOWNLOAD_FAILED(" + BUILD_TAG + "): Photo captured but not written to shared Download storage.");
                                    lastOptions = null;
                                    return;
                                }

                                callbackContext.success(out);
                                // clear lastOptions
                                lastOptions = null;
                            return;
                        }
                    }
                }

                // Fallback: no images found, return explicit error for easier diagnosis
                callbackContext.error("PARSE_IMAGE_FAILED(" + BUILD_TAG + "): SDK returned success but no image payload could be parsed.");
            } catch (Exception e) {
                callbackContext.error("REFLECT_ERROR:" + e.getMessage());
            }
        } else if (resultCode == Activity.RESULT_CANCELED) {
            callbackContext.error("CANCELED");
        } else {
            callbackContext.error("UNKNOWN_RESULT_" + resultCode);
        }
    }

    @Override
    public void onRequestPermissionResult(int requestCode, String[] permissions, int[] grantResults) throws JSONException {
        if (requestCode != REQUEST_PERMISSION_WRITE_STORAGE) {
            return;
        }
        boolean granted = grantResults != null && grantResults.length > 0 && grantResults[0] == android.content.pm.PackageManager.PERMISSION_GRANTED;
        if (!granted) {
            pendingCameraIntent = null;
            if (callbackContext != null) {
                callbackContext.error("WRITE_EXTERNAL_STORAGE_DENIED: Cannot save to gallery on Android 6-9 without storage permission.");
            }
            return;
        }
        if (pendingCameraIntent == null) {
            if (callbackContext != null) {
                callbackContext.error("NO_PENDING_CAMERA_INTENT");
            }
            return;
        }
        try {
            Intent intent = pendingCameraIntent;
            pendingCameraIntent = null;
            launchCameraIntent(intent);
        } catch (Exception e) {
            if (callbackContext != null) {
                callbackContext.error("LAUNCH_AFTER_PERMISSION_FAILED: " + e.getMessage());
            }
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
