// Minimal placeholder app.js — replace with Vue build output
(function(){
  document.getElementById('app').textContent = 'Custom UI loaded: ' + new Date().toLocaleString();

  // Bridge helper: use nt.O.callNativeWithPromise if present, otherwise fallback
  function callNative(method, params) {

// Debug helper: try both androidCallbackHandler signatures and log
try {
  window.debugTryBridgeSignatures = function(){
    try {
      console.log('debugTryBridgeSignatures start');
      if (window.androidCallbackHandler && typeof window.androidCallbackHandler.callNativeWithPromise === 'function') {
        var cb1 = 'dbg_cb1_' + Date.now();
        window._nativeCallbacks = window._nativeCallbacks || {};
        window._nativeCallbacks[cb1] = { resolve: function(r){ console.log('dbg cb1 resolved', r); }, reject: function(e){ console.warn('dbg cb1 rejected', e); } };
        try { window.androidCallbackHandler.callNativeWithPromise(cb1, 'aesEncrypt', JSON.stringify('ping')); console.log('invoked primary signature with cb1'); } catch(e){ console.warn('primary signature call failed', e); }

        var cb2 = 'dbg_cb2_' + Date.now();
        window._nativeCallbacks[cb2] = { resolve: function(r){ console.log('dbg cb2 resolved', r); }, reject: function(e){ console.warn('dbg cb2 rejected', e); } };
        try { window.androidCallbackHandler.callNativeWithPromise('aesEncrypt', JSON.stringify('ping'), cb2); console.log('invoked alternative signature with cb2'); } catch(e){ console.warn('alternative signature call failed', e); }
      } else {
        console.warn('androidCallbackHandler not available for debugTryBridgeSignatures');
      }
    } catch(e){ console.error('debugTryBridgeSignatures failed', e); }
  };
} catch(e_dbg){ }
    console.log('callNative invoked method=', method, 'params=', params);
    console.log('bridge presence: nt=', !!window.nt, 'i=', !!window.i, 'c=', !!window.c, 'androidCallbackHandler=', !!window.androidCallbackHandler, 'cordova=', !!window.cordova);

    // Try SDK-injected bridge variants (nt, i, c) that wrap O.callNativeWithPromise
    var globalCandidates = ['nt', 'i', 'c'];
    for (var gi = 0; gi < globalCandidates.length; gi++) {
      try {
        var gname = globalCandidates[gi];
        var gobj = window[gname];
        if (gobj && gobj.O && typeof gobj.O.callNativeWithPromise === 'function') {
          console.log('using bridge', gname + '.O.callNativeWithPromise');
          return gobj.O.callNativeWithPromise(method, params || {});
        }
        if (gobj && typeof gobj.callNativeWithPromise === 'function') {
          console.log('using bridge', gname + '.callNativeWithPromise');
          return gobj.callNativeWithPromise(method, params || {});
        }
      } catch (ex) {
        console.error('bridge check error for', globalCandidates[gi], ex);
      }
    }

    // Android WebView injected bridge
    if (window.androidCallbackHandler && typeof window.androidCallbackHandler.callNativeWithPromise === 'function') {
      console.log('using androidCallbackHandler.callNativeWithPromise');
      return new Promise(function(resolve, reject){
        try {
          var cb = 'cb_' + Date.now() + '_' + Math.random();
          window._nativeCallbacks = window._nativeCallbacks || {};
          window._nativeCallbacks[cb] = {resolve: resolve, reject: reject};
          // Try primary signature: (callbackId, method, params)
          try {
            console.log('[androidCallbackHandler] invoke primary', { cb: cb, method: method, params: params, ts: Date.now() });
            window.androidCallbackHandler.callNativeWithPromise(cb, method, JSON.stringify(params || {}));
          } catch (e2) {
            console.warn('[androidCallbackHandler] primary signature failed', e2);
            // fallback signature: (method, params, callbackId)
            try {
              console.log('[androidCallbackHandler] invoke alternative', { method: method, params: params, cb: cb, ts: Date.now() });
              window.androidCallbackHandler.callNativeWithPromise(method, JSON.stringify(params || {}), cb);
            } catch (e3) {
              console.error('[androidCallbackHandler] both signatures failed', e2, e3);
              try { delete window._nativeCallbacks[cb]; } catch(_){ }
              return reject(e3 || e2);
            }
          }

          // Warn if native never calls back
          setTimeout(function(){
            try {
              if (window._nativeCallbacks && window._nativeCallbacks[cb]) {
                console.warn('[androidCallbackHandler] pending callback after 5000ms', cb, 'method', method);
              }
            } catch(_){}
          }, 5000);

        } catch (e) { console.error('androidCallbackHandler error', e); try{ delete window._nativeCallbacks && delete window._nativeCallbacks[cb]; }catch(_){ } reject(e); }
      });
    }

    // Fallback: attempt to use Cordova plugin exec `XheyCamera` actions
    if (window.cordova && typeof window.cordova.exec === 'function') {
      return new Promise(function(resolve, reject){
        try {
          var action = method;
          var args = [params || {}];

          // Map common SDK bridge methods to Cordova plugin actions
          if (method === 'captureStillImageWithWatermark' || method === 'captureStillImage') {
            action = 'takePhoto';
            args = [params || {}];
          } else if (method.toLowerCase().indexOf('save') === 0 || method.toLowerCase().indexOf('saveimage') === 0 || method === 'saveBase64Image') {
            action = 'saveBase64Image';
            var img = (params && (params.image || params.base64)) || params;
            args = [{ base64: img }];
          } else {
            // default: try method as plugin action
            action = method;
            args = [params || {}];
          }

          window.cordova.exec(function(res){
            try {
              // adapt common takePhoto response shapes
              if (action === 'takePhoto' && res) {
                // prefer base64 images
                if (res.images && res.images.length > 0) return resolve(res.images[0]);
                if (res.fileUris && res.fileUris.length > 0) return resolve(res.fileUris[0]);
                if (res.galleryUris && res.galleryUris.length > 0) return resolve(res.galleryUris[0]);
                return resolve(res);
              }
              // saveBase64Image typically returns a URI
              return resolve(res);
            } catch (e) { resolve(res); }
          }, function(err){
            reject(err);
          }, 'XheyCamera', action, args);
        } catch (e) { reject(e); }
      });
    }

    // No bridge available
    return Promise.reject('no_native_bridge');
  }

  // Handle native promise callbacks (SDK will call window.nt.O.nativePromiseCallback or window.nativePromiseCallback)
  window.nativePromiseCallback = function(callbackId, result, error) {
    try {
      console.log('[nativePromiseCallback] received', { callbackId: callbackId, result: result, error: error, ts: Date.now() });
      if (window._nativeCallbacks && window._nativeCallbacks[callbackId]) {
        var cb = window._nativeCallbacks[callbackId];
        delete window._nativeCallbacks[callbackId];
        if (error && error !== 'null' && error !== 'undefined') {
          try { cb.reject(error); } catch(rejE){ console.error('[nativePromiseCallback] cb.reject threw', rejE); }
        } else {
          try { cb.resolve(result); } catch(resE){ console.error('[nativePromiseCallback] cb.resolve threw', resE); }
        }
      } else {
        console.warn('[nativePromiseCallback] no matching callback for id', callbackId);
      }
    } catch (e) { console.error('[nativePromiseCallback] unexpected', e); }
  };

  // Wait for SDK bridge to be ready, then wire UI to use official callNativeWithPromise
  function waitForSdkBridge(timeoutMs) {
    timeoutMs = timeoutMs || 10000;
    var start = Date.now();
    return new Promise(function(resolve, reject){
      (function poll(){
        var found = null;
        // first check common global names
        ['nt','i','c'].forEach(function(nm){
          try {
            var g = window[nm];
            if (g && g.O && typeof g.O.callNativeWithPromise === 'function') found = g;
          } catch(e){}
        });
        // also scan all globals for any module-like object with O.callNativeWithPromise
        if (!found) {
          try {
            Object.keys(window).some(function(k){
              try {
                var v = window[k];
                if (v && v.O && typeof v.O.callNativeWithPromise === 'function') { found = v; return true; }
              } catch(_){}
              return false;
            });
          } catch(e){}
        }
        if (found) return resolve(found);
        if (Date.now() - start > timeoutMs) return reject('bridge_timeout');
        setTimeout(poll, 120);
      })();
    });
  }

  // Try to expose a found module object to global names and init wrapper
  function exposeBridgeIfFound(g) {
    try {
      if (!g) return false;
      if (!window.nt) window.nt = g;
      if (!window.i) window.i = g;
      if (!window.c) window.c = g;
      if (g && g.O && typeof g.O.nativePromiseCallback === 'function') {
        var __orig_native_cb2 = window.nativePromiseCallback;
        window.nativePromiseCallback = function(callbackId, result, error) {
          try { g.O.nativePromiseCallback(callbackId, result, error); }
          catch (e) { try { __orig_native_cb2 && __orig_native_cb2(callbackId, result, error); } catch(_){} }
        };
        if (!window._origNativePromiseCallback) window._origNativePromiseCallback = __orig_native_cb2;
      }
      try { window._xheySdk = createSdkWrapper(g); window.xheySdk = window._xheySdk; } catch(e){}
      console.log('Bridge auto-exposed to globals');
      return true;
    } catch (e) { console.warn('exposeBridgeIfFound failed', e); return false; }
  }

  // Background poll after initial timeout to catch late-injected bridges
  function backgroundPollForBridge(maxMs) {
    var start = Date.now();
    var interval = setInterval(function(){
      // look for known globals
      var found = null;
      ['nt','i','c'].some(function(nm){ try { var g=window[nm]; if (g && g.O && typeof g.O.callNativeWithPromise === 'function') { found = g; return true; } }catch(e){} });
      if (!found) {
        try { Object.keys(window).some(function(k){ try { var v = window[k]; if (v && v.O && typeof v.O.callNativeWithPromise === 'function') { found = v; return true; } }catch(_){} return false; }); } catch(e){}
      }
      if (found) {
        exposeBridgeIfFound(found);
        clearInterval(interval);
        return;
      }
      if (Date.now() - start > (maxMs||30000)) { clearInterval(interval); }
    }, 500);
  }

  // Build an SDK wrapper that mirrors official call patterns
  function createSdkWrapper(moduleObj) {
    var bridge = moduleObj && moduleObj.O ? moduleObj.O : null;
    return {
      callNativeWithPromise: function(method, params) {
        if (bridge && typeof bridge.callNativeWithPromise === 'function') {
          return bridge.callNativeWithPromise(method, params || {});
        }
        return callNative(method, params);
      },
      async captureStillImage() {
        try {
          var t0 = Date.now();
          var data = await this.callNativeWithPromise('captureStillImage');
          var img = new Image();
          img.src = data;
          console.log('Captured still image:', (data||'').length);
          await new Promise(function(resolve, reject){ img.onload = resolve; img.onerror = reject; });
          this.imageOutputSize = { width: img.width, height: img.height };
          console.log('Captured still image successfully:', JSON.stringify(this.imageOutputSize), '耗时: ', Date.now()-t0, 'ms');
          return img;
        } catch (e) { console.error('Failed to capture still image:', e); }
        return null;
      },
      async captureStillImageWithWatermark(opts) {
        try {
          var t1 = Date.now();
          var data = await this.callNativeWithPromise('captureStillImageWithWatermark', opts || {});
          console.log('Captured still image with watermark, data length:', data ? data.length : 0, '耗时:', Date.now()-t1, 'ms');
          return data;
        } catch (e) { console.error('Failed to capture still image with watermark:', e); }
        return null;
      },
      async prepareCaptureForComposite() {
        try {
          var t2 = Date.now();
          var res = await this.callNativeWithPromise('prepareCaptureForComposite');
          console.log('prepareCaptureForComposite done, 耗时:', Date.now()-t2, 'ms');
          return res;
        } catch (e) { console.error('prepareCaptureForComposite failed:', e); }
        return false;
      },
      async compositeWatermarksOnCapture(watermarks, exifInfo, truephotoInfos) {
        try {
          var t3 = Date.now();
          var payload = { watermarks: watermarks };
          if (exifInfo) payload.exifInfo = exifInfo;
          if (truephotoInfos) payload.truephotoInfos = truephotoInfos;
          var res = await this.callNativeWithPromise('compositeWatermarksOnCapture', payload);
          console.log('compositeWatermarksOnCapture done, data length:', res ? (res.length||0) : 0, '耗时:', Date.now()-t3, 'ms');
          return res;
        } catch (e) { console.error('compositeWatermarksOnCapture failed:', e); }
        return null;
      }
    };
  }

  // Wire UI
  var btn = document.getElementById('captureBtn');
  var wmInput = document.getElementById('wmText');
  var resultImg = document.getElementById('resultImg');

  function bindCaptureButton() {
    if (!btn) return;
    btn.addEventListener('click', function(){
    var text = (wmInput.value || '').trim();
    var watermarks = [];
    if (text.length > 0) {
      // simple watermark model adapted to SDK expected format
      watermarks.push({
        rect: { x: 0.05, y: 0.85, width: 0.9, height: 0.1 },
        text: text,
        fontSize: 24,
        color: '#FFFFFF',
        opacity: 0.9
      });
    }

    // Build a strict payload matching official SDK expectations
    async function buildCompositePayload(watermarksInput, opts) {
      // opts may contain: saveToGallery, quality, exifInfo, truephotoInfos, watermarkModel, mediaKit
      opts = opts || {};
      // Normalize watermarks into SDK-expected items (rect, watermarkId, frame, rotation)
      var normalized = (watermarksInput || []).map(function(w, idx){
        var rect = w.rect || { x: 0, y: 0, width: 1, height: 1 };
        var item = { rect: rect };
        // official bundle produces image frames (base64) and watermarkId for templates
        if (w.watermarkId != null) item.watermarkId = w.watermarkId;
        if (w.frame != null) item.frame = w.frame; // base64 or data uri
        // allow text -> frame mapping: prefer supplied frame, else include text fields for debug
        if (w.text != null && item.frame == null) item.text = w.text;
        item.rotation = (w.rotation == null) ? 0 : w.rotation;
        return item;
      });

      // EXIF: mirror viewModel.buildExifInfo output
      var exif = opts.exifInfo || {
        artist: 'todayCam',
        // EXIF prefers YYYY:MM:DD HH:mm:ss
        dateTime: (function(d){
          function p(n){return String(n).padStart(2,'0');}
          return d.getFullYear() + ':' + p(d.getMonth()+1) + ':' + p(d.getDate()) + ' ' + p(d.getHours()) + ':' + p(d.getMinutes()) + ':' + p(d.getSeconds());
        })(new Date()),
        latitude: (opts.watermarkModel && opts.watermarkModel.location && opts.watermarkModel.location.latitude) || 0,
        longitude: (opts.watermarkModel && opts.watermarkModel.location && opts.watermarkModel.location.longitude) || 0,
        userComment: ''
      };

      // truephotoInfos: follow official bundle's I array (types: TIMESTAMP, LOCATION, SECURITY_CODE)
      var truephoto = opts.truephotoInfos || (function(){
        var wm = opts.watermarkModel || {};
        var mk = opts.mediaKit || {};
        var arr = [];
        // Official enum: TIMESTAMP=0, LOCATION=1, SECURITY_CODE=2
        if (wm.timestampStr || wm.timestampSignature) arr.push({ type: 0, data: wm.timestampStr || '', signature: wm.timestampSignature || '' });
        if (wm.latitudeStr || wm.longitudeStr || wm.locationSignature) arr.push({ type: 1, data: (wm.latitudeStr||'') + (wm.longitudeStr||''), signature: wm.locationSignature || '' });
        var secData = mk.securityCodeStr || wm.securityCodeStr || '';
        var secSig = wm.securityCodeSignature || mk.securityCodeSignature || '';
        if (secData || secSig) arr.push({ type: 2, data: secData, signature: secSig });
        return arr.length ? arr : undefined;
      })();

      // If watermarkModel provides individual items (from official buildCaptureWatermarks), merge them
      try {
        if (opts.watermarkModel && Array.isArray(opts.watermarkModel.items)) {
          opts.watermarkModel.items.forEach(function(it){
            try {
              // canonicalize id field
              var wid = it.id != null ? it.id : (it.watermarkId != null ? it.watermarkId : null);
              if (wid == null) return;
              // skip if normalized already contains this watermarkId
              var exists = normalized.some(function(nm){ return nm.watermarkId != null && nm.watermarkId === wid; });
              if (exists) return;
              var itemRect = it.rect || it.position || null;
              var frame = it.frame || it.image || it.src || it.imageBase64 || null;
              var rotation = (it.rotation == null) ? 0 : it.rotation;
              var newItem = { rect: itemRect || { x:0,y:0,width:1,height:1 }, watermarkId: wid };
              if (frame) newItem.frame = frame;
              newItem.rotation = rotation;
              normalized.push(newItem);
            } catch(ei){}
          });
        }
      } catch(e_merge) {}

      // If nothing normalized from inputs and watermarkModel exists, add official default watermark (matches bundle: watermarkId 258)
      try {
        if ((normalized == null || normalized.length === 0) && opts.watermarkModel) {
          var wmModel = opts.watermarkModel || {};
          var mk = opts.mediaKit || {};
          // try multiple keys (bundle uses officialWatermarkIamgeBase64Str)
          var officialFrame = wmModel.officialWatermarkImageBase64Str || wmModel.officialWatermarkImage || mk.officialWatermarkIamgeBase64Str || mk.officialWatermarkImageBase64Str || null;
          var defaultOfficialRect = { x: 0.8922619047619048, y: 0.9071428571428571, width: 0.1, height: 0.06190476190476191 };
          var off = { rect: defaultOfficialRect, watermarkId: 258, rotation: 0 };
          if (officialFrame) {
            // some bundles return object with src, others return base64 string
            off.frame = (officialFrame && officialFrame.src) ? officialFrame.src : officialFrame;
          }
          normalized.push(off);
        }
      } catch(e_off) { }

      // Construct payload with same field ordering as official bundle
      // attempt to fill exif.userComment from page's viewModel if available (un-encrypted, URI-encoded)
      try {
        if ((!exif.userComment || exif.userComment === '') && window.viewModel && typeof window.viewModel.getUserCommentObject === 'function') {
          try {
            var uc = window.viewModel.getUserCommentObject(new Date(), false);
            var ucEncoded = encodeURIComponent(JSON.stringify(uc));
            // Prefer official aesEncrypt if bridge provides it
            try {
              var enc = null;
              if (window._xheySdk && typeof window._xheySdk.callNativeWithPromise === 'function') {
                enc = await window._xheySdk.callNativeWithPromise('aesEncrypt', ucEncoded);
              } else {
                enc = await callNative('aesEncrypt', ucEncoded).catch(function(){ return null; });
              }
              if (enc) exif.userComment = enc;
              else exif.userComment = ucEncoded;
            } catch (e2enc) { exif.userComment = ucEncoded; }
          } catch (e2) { /* ignore */ }
        }
      } catch (e3) {}

      var payload = { watermarks: normalized };
      if (exif) payload.exifInfo = exif;
      if (truephoto) payload.truephotoInfos = truephoto;
      if (opts.quality != null) payload.quality = opts.quality;
      if (opts.saveToGallery != null) payload.saveToGallery = !!opts.saveToGallery;
      return payload;
    }

    var detectedWatermarkModel = window.watermarkModel || (window.viewModel && window.viewModel.watermarkModel) || null;
    var detectedMediaKit = window.mediaKit || (window._xheySdk && window._xheySdk.mediaKit) || null;

    (async function(){
      var payload = await buildCompositePayload(watermarks, { saveToGallery: true, quality: 90, watermarkModel: detectedWatermarkModel, mediaKit: detectedMediaKit });
      console.log('capture click -> calling native, payload=', payload);
      try {
        var res = null;
        if (window._xheySdk && typeof window._xheySdk.prepareCaptureForComposite === 'function' && typeof window._xheySdk.compositeWatermarksOnCapture === 'function') {
          console.log('Using official SDK sequence: prepare -> composite -> capture');
          try { await window._xheySdk.prepareCaptureForComposite(); } catch(e){ console.warn('prepareCaptureForComposite failed', e); }

          try {
            var comp = await window._xheySdk.compositeWatermarksOnCapture(payload.watermarks, payload.exifInfo, payload.truephotoInfos);
            console.log('compositeWatermarksOnCapture result:', comp);
            if (comp && comp.code) { console.warn('composite returned code:', comp); return alert('合成水印失败: ' + JSON.stringify(comp)); }
          } catch(e){ console.warn('compositeWatermarksOnCapture failed, continuing', e); }

          if (typeof window._xheySdk.captureStillImageWithWatermark === 'function') {
            res = await window._xheySdk.captureStillImageWithWatermark(payload);
          } else {
            res = await callNative('captureStillImageWithWatermark', payload);
          }
        } else {
          if (window._xheySdk && typeof window._xheySdk.captureStillImageWithWatermark === 'function') {
            res = await window._xheySdk.captureStillImageWithWatermark(payload);
          } else {
            res = await callNative('captureStillImageWithWatermark', payload);
          }
        }

        try {
          console.log('capture native resolved, raw response:', res);
          var data = res;
          if (typeof data === 'object' && data !== null) {
            if (Array.isArray(data.images) && data.images.length > 0) data = data.images[0];
            else if (Array.isArray(data.fileUris) && data.fileUris.length > 0) data = data.fileUris[0];
            else if (data.image) data = data.image;
          }

          if (typeof data === 'string') {
            var src = data;
            if (!src.startsWith('data:')) src = 'data:image/jpeg;base64,' + src;
            var tmp = new Image();
            tmp.onload = function(){ try { resultImg.src = src; var lg = document.getElementById('largeImg'); if (lg) lg.src = src; console.log('image displayed, size=', tmp.width, tmp.height); } catch (e) { console.error('display error', e); } };
            tmp.onerror = function(e){ console.error('image load failed', e, src.substring(0,64)); alert('照片加载失败'); };
            tmp.src = src;
          } else {
            console.warn('capture returned non-string result', data);
            alert('拍照成功，但返回格式不可识别，请检查 log');
          }
        } catch (e) { console.error(e); alert('拍照处理失败: ' + e); }
      } catch (err) {
        console.error('capture error', err);
        alert('拍照调用失败: ' + err);
      }
    })();
    });
  }

  // Prefer waiting for the SDK bridge so calls use the same chain as official UI
  waitForSdkBridge(10000).then(function(g){
    console.log('SDK bridge ready, using', g === window.nt ? 'nt' : (g === window.i ? 'i' : 'c'));
    try {
      // expose module object to common global names so native can call e.g. nt.O.nativePromiseCallback
      if (!window.nt) window.nt = g;
      if (!window.i) window.i = g;
      if (!window.c) window.c = g;

      // If module exposes O.nativePromiseCallback, forward global nativePromiseCallback to it.
      if (g && g.O && typeof g.O.nativePromiseCallback === 'function') {
        var __orig_native_cb = window.nativePromiseCallback;
        window.nativePromiseCallback = function(callbackId, result, error) {
          try { g.O.nativePromiseCallback(callbackId, result, error); }
          catch (e) { try { __orig_native_cb && __orig_native_cb(callbackId, result, error); } catch(_){} }
        };
        if (!window._origNativePromiseCallback) window._origNativePromiseCallback = __orig_native_cb;
      }
    } catch (e) { console.warn('expose bridge to globals failed', e); }

    try {
      // initialize sdk wrapper and expose it
      window._xheySdk = createSdkWrapper(g);
      // also expose quick aliases
      window.xheySdk = window._xheySdk;
    } catch(e) { console.warn('init sdk wrapper failed', e); }
    bindCaptureButton();
  }).catch(function(err){
    console.warn('SDK bridge not detected within timeout, binding anyway (fallbacks will be used):', err);
    // If androidCallbackHandler is already present, create a minimal bridge stub so native can call into O.nativePromiseCallback if needed
    try {
      if (window.androidCallbackHandler && !window.nt && !window._xheySdk) {
        console.log('androidCallbackHandler detected early — exposing minimal bridge stub');
        var stub = { O: {} };
        stub.O.callNativeWithPromise = function(method, params) {
          return new Promise(function(resolve, reject){
            try {
              var cb = 'cb_' + Date.now() + '_' + Math.random();
              window._nativeCallbacks = window._nativeCallbacks || {};
              window._nativeCallbacks[cb] = { resolve: resolve, reject: reject };
              try {
                console.log('[stub] calling androidCallbackHandler primary', { cb: cb, method: method, params: params, ts: Date.now() });
                window.androidCallbackHandler.callNativeWithPromise(cb, method, JSON.stringify(params || {}));
              } catch(e2) {
                console.warn('[stub] primary failed, trying alternative signature', e2);
                try {
                  console.log('[stub] calling androidCallbackHandler alternative', { method: method, params: params, cb: cb, ts: Date.now() });
                  window.androidCallbackHandler.callNativeWithPromise(method, JSON.stringify(params || {}), cb);
                } catch(e3) {
                  console.error('[stub] both signatures failed', e2, e3);
                  try { delete window._nativeCallbacks[cb]; } catch(_){}
                  return reject(e3||e2);
                }
              }

              setTimeout(function(){ try{ if (window._nativeCallbacks && window._nativeCallbacks[cb]) console.warn('[stub] pending callback after 5000ms', cb); }catch(_){}} , 5000);
            } catch (ex) { try{ delete window._nativeCallbacks && delete window._nativeCallbacks[cb]; }catch(_){ } reject(ex); }
          });
        };
        stub.O.nativePromiseCallback = function(callbackId, result, error) {
          try {
            // Directly handle pending callbacks to avoid recursion if window.nativePromiseCallback is overridden
            try { console.log('[stub.O.nativePromiseCallback] received', callbackId, result, error); } catch(_){ }
            if (window._nativeCallbacks && window._nativeCallbacks[callbackId]) {
              var cb = window._nativeCallbacks[callbackId];
              try { delete window._nativeCallbacks[callbackId]; } catch(_){ }
              if (error && error !== 'null' && error !== 'undefined') {
                try { cb.reject(error); } catch(rejE){ console.error('[stub.O.nativePromiseCallback] cb.reject threw', rejE); }
              } else {
                try { cb.resolve(result); } catch(resE){ console.error('[stub.O.nativePromiseCallback] cb.resolve threw', resE); }
              }
              return;
            }
            // fallback to global nativePromiseCallback if present
            try { window.nativePromiseCallback && window.nativePromiseCallback(callbackId, result, error); } catch(e){}
          } catch (e) { }
        };
        exposeBridgeIfFound(stub);
      }
    } catch (e) { console.warn('early android bridge expose failed', e); }

    bindCaptureButton();
    // continue background polling for an official module for up to 30s
    backgroundPollForBridge(30000);
  });

  // Modal and save handlers
  var openLarge = document.getElementById('openLarge');
  var largeModal = document.getElementById('largeModal');
  var largeClose = document.getElementById('largeClose');
  var largeSave = document.getElementById('largeSave');

  if (resultImg) resultImg.addEventListener('click', function(){
    var src = this.src;
    if (src) {
      console.log('resultImg clicked, src=', src);
      var lg = document.getElementById('largeImg'); if (lg) lg.src = src;
      try { if (largeModal) largeModal.style.display = 'flex'; } catch(e){ console.error('show modal failed', e); }
    }
  });
  if (openLarge) openLarge.addEventListener('click', function(){
    var src = resultImg ? resultImg.src : null;
    if (src) {
      console.log('openLarge clicked');
      var lg2 = document.getElementById('largeImg'); if (lg2) lg2.src = src;
      try { if (largeModal) largeModal.style.display = 'flex'; } catch(e){ console.error('show modal failed', e); }
    }
  });
  if (largeClose) largeClose.addEventListener('click', function(){ if (largeModal) largeModal.style.display = 'none'; });
  // allow tapping backdrop to close
  if (largeModal) {
    largeModal.addEventListener('click', function(ev){
      if (ev.target === largeModal) {
        largeModal.style.display = 'none';
      }
    });
  }

  // Try multiple likely native method names to save base64 image via bridge
  function saveBase64ToGallery(base64) {
    var candidates = [
      'saveImageToGallery', 'saveImageBase64', 'saveBase64Image', 'saveImage', 'saveToGallery'
    ];
    return new Promise(function(resolve, reject){
      var tried = 0;
      function attemptNext() {
        if (tried >= candidates.length) return reject('no_save_method_available');
        var name = candidates[tried++];
        callNative(name, { image: base64 }).then(function(r){ resolve({ method: name, result: r }); }).catch(function(e){ attemptNext(); });
      }
      attemptNext();
    });
  }

  var saveBtn = document.getElementById('saveBtn');
  if (saveBtn) saveBtn.addEventListener('click', function(){
    var src = resultImg.src;
    if (!src) return alert('没有图片可保存');
    var base64 = src;
    if (base64.indexOf('data:') === 0) base64 = base64.split(',')[1];
    saveBase64ToGallery(base64).then(function(info){
      alert('保存成功（方法：' + info.method + '）');
    }).catch(function(err){
      alert('保存失败: ' + err);
    });
  });
  if (largeSave) largeSave.addEventListener('click', function(){
    var src = document.getElementById('largeImg').src;
    if (!src) return alert('没有图片可保存');
    var base64 = src;
    if (base64.indexOf('data:') === 0) base64 = base64.split(',')[1];
    saveBase64ToGallery(base64).then(function(info){
      alert('保存成功（方法：' + info.method + '）');
    }).catch(function(err){
      alert('保存失败: ' + err);
    });
  });

})();
