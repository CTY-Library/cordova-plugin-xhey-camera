package org.xhey.cordova.camera;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.JavascriptInterface;
import android.webkit.WebView;

import com.xhey.xheycamerasdk.CameraActivity;

import java.lang.reflect.Method;
import android.content.Context;
import android.content.ContextWrapper;
import android.app.Activity;
import org.json.JSONObject;

public class CustomCameraActivity extends CameraActivity {
    private static final String TAG = "CustomCameraActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // injection will be attempted in onResume as view hierarchy is ready
    }

    @Override
    public void onResume() {
        super.onResume();
        try {
            View decor = getWindow() != null ? getWindow().getDecorView() : null;
            WebView wv = findWebView(decor);
            if (wv != null) {
                try {
                    // expose a robust bridge object as androidCallbackHandler
                    wv.addJavascriptInterface(new AndroidCallbackHandler(wv), "androidCallbackHandler");
                    Log.i(TAG, "Injected androidCallbackHandler into WebView");
                } catch (Exception e) {
                    Log.w(TAG, "Failed to addJavascriptInterface", e);
                }
            } else {
                Log.i(TAG, "No WebView found to inject bridge");
            }
        } catch (Exception e) {
            Log.w(TAG, "injectBridge failed", e);
        }
    }

    private WebView findWebView(View v) {
        if (v == null) return null;
        if (v instanceof WebView) return (WebView) v;
        if (v instanceof ViewGroup) {
            ViewGroup vg = (ViewGroup) v;
            for (int i = 0; i < vg.getChildCount(); i++) {
                View child = vg.getChildAt(i);
                WebView w = findWebView(child);
                if (w != null) return w;
            }
        }
        return null;
    }

    // Minimal bridge: attempts to route JS calls to visible SDK methods via reflection,
    // and always calls back into JS using window.nativePromiseCallback(callbackId, result, error)
    private static class AndroidCallbackHandler {
        private final WebView webView;

        AndroidCallbackHandler(WebView webView) {
            this.webView = webView;
        }

        @JavascriptInterface
        public void callNativeWithPromise(final String a, final String b, final String c) {
            // Support both signatures from JS:
            // 1) (callbackId, method, params)
            // 2) (method, params, callbackId)
            String callbackId = null;
            String method = null;
            String params = null;
            try {
                // Heuristic: callbackId typically contains 'cb_' prefix in our JS stubs
                if (a != null && a.startsWith("cb_")) {
                    callbackId = a; method = b; params = c;
                } else if (c != null && c.startsWith("cb_")) {
                    callbackId = c; method = a; params = b;
                } else if (b != null && b.startsWith("cb_")) {
                    callbackId = b; method = a; params = c;
                } else {
                    // Fallback: assume original order
                    callbackId = a; method = b; params = c;
                }
            } catch (Exception exparse) {
                callbackId = a; method = b; params = c;
            }
            Log.i(TAG, "callNativeWithPromise(js) invoked: method=" + method + " cb=" + callbackId + " rawArgs=[" + a + "," + b + "," + c + "]");
            // make final copies for use inside inner classes
            final String cbFinal = callbackId;
            final String methodFinal = method;
            final String paramsFinal = params;
            // Try to invoke known CameraActivity instance methods via reflection
            new Thread(new Runnable() {
                @Override
                public void run() {
                    String result = null;
                    String error = null;
                    try {
                        // Attempt to get CameraActivity instance via static getInstance()
                        Class<?> cls = Class.forName("com.xhey.xheycamerasdk.CameraActivity");
                        Object inst = null;
                        try {
                            Method gi = cls.getMethod("getInstance");
                            inst = gi.invoke(null);
                        } catch (NoSuchMethodException ns) {
                            // ignore
                        }

                        // If the SDK does not provide a global getInstance(), try to discover
                        // the Activity instance from the WebView context as a fallback.
                        if (inst == null) {
                            try {
                                Context ctx = webView.getContext();
                                while (ctx instanceof ContextWrapper) {
                                    if (ctx instanceof Activity) { inst = ctx; break; }
                                    ctx = ((ContextWrapper) ctx).getBaseContext();
                                }
                                if (inst == null && ctx instanceof Activity) inst = ctx;
                                if (inst != null) Log.i(TAG, "reflection: obtained Activity instance from WebView context");
                            } catch (Throwable tctx) {
                                // ignore
                            }
                        }

                        // Map some common JS method names to candidate Java methods
                        String[] candidates = new String[] {
                            "captureStillImageWithWatermark", "captureStillImage", "takePhoto", "capture", "doCapture"
                        };
                        boolean invoked = false;
                        if (inst != null) {
                            for (String cand : candidates) {
                                try {
                                    Method m = inst.getClass().getMethod(cand);
                                    Object r = m.invoke(inst);
                                    if (r != null) result = String.valueOf(r);
                                    invoked = true;
                                    break;
                                } catch (NoSuchMethodException nm) {
                                    // try with payload param
                                    try {
                                        Method m2 = inst.getClass().getMethod(cand, String.class);
                                        Object r2 = m2.invoke(inst, paramsFinal);
                                        if (r2 != null) result = String.valueOf(r2);
                                        invoked = true;
                                        break;
                                    } catch (NoSuchMethodException nm2) {
                                        // continue
                                    }
                                }
                            }
                        }

                        if (!invoked) {
                            // fallback: if method is simple 'aesEncrypt' we cannot compute here; return params as-is
                            if ("aesEncrypt".equals(methodFinal)) {
                                result = paramsFinal;
                            } else {
                                error = "method_not_handled:" + methodFinal;
                            }
                        }
                    } catch (Throwable t) {
                        Log.w(TAG, "reflection dispatch failed", t);
                        error = "dispatch_error:" + t.getMessage();
                    }

                    final String rfinal = result;
                    final String efinal = error;
                    // Call back into JS on UI thread
                    webView.post(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                try {
                                    String cbJson = JSONObject.quote(cbFinal == null ? "" : cbFinal);
                                    String resJson = (rfinal == null) ? "null" : JSONObject.quote(rfinal);
                                    String errJson = (efinal == null) ? "null" : JSONObject.quote(efinal);
                                    String js = "try{window.nativePromiseCallback(" + cbJson + "," + resJson + "," + errJson + ");}catch(e){console.error('bridge callback failed',e);} ";
                                    Log.i(TAG, "evaluating JS callback: " + js);
                                    webView.evaluateJavascript(js, new android.webkit.ValueCallback<String>(){
                                        @Override
                                        public void onReceiveValue(String value) {
                                            Log.i(TAG, "evaluateJavascript result: " + value);
                                        }
                                    });
                                } catch (Exception je) {
                                    Log.w(TAG, "json quote failed, fallback", je);
                                    String safeCb = cbFinal == null ? "" : cbFinal.replace("'", "\\'");
                                    String safeRes = rfinal == null ? "" : rfinal.replace("'", "\\'");
                                    String safeErr = efinal == null ? "null" : "'" + efinal.replace("'", "\\'") + "'";
                                    String js = "try{window.nativePromiseCallback('" + safeCb + "','" + safeRes + "'," + safeErr + ");}catch(e){console.error('bridge callback failed',e);} ";
                                    Log.i(TAG, "evaluating JS callback (fallback): " + js);
                                    webView.evaluateJavascript(js, new android.webkit.ValueCallback<String>(){
                                        @Override
                                        public void onReceiveValue(String value) {
                                            Log.i(TAG, "evaluateJavascript fallback result: " + value);
                                        }
                                    });
                                }
                            } catch (Exception ex) {
                                Log.w(TAG, "callback to JS failed", ex);
                            }
                        }
                    });
                }
            }).start();
        }

        // no alternative overloads — single entry handles both JS call orders
    }
}
