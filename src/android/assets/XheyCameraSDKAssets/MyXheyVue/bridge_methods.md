桥接方法清单（从官方 bundle 提取）

说明：下列方法在官方 H5 bundle 中通过 `nt.O.callNativeWithPromise(method, params)` 调用；参数形态基于代码片段提取，供调试与构造 payload 使用。

常用拍照/合成相关：
- `prepareCaptureForComposite()`
  - 无参数；用于让 native 进入合成准备状态。

- `compositeWatermarksOnCapture(payload)`
  - payload: { watermarks: Array<WatermarkItem>, exifInfo?: Object, truephotoInfos?: Array }
  - 返回：base64 或 字符串/对象；若包含 `{code}` 则为错误语义。

- `captureStillImageWithWatermark(payload)`
  - payload: 同上（包含 watermarks/exif/truephoto/quality/saveToGallery 等）。
  - 返回：base64 图像或对象（images/fileUris/image 字段）。

- `captureStillImage()`
  - 无参数或简单参数；返回 base64 或 image URL。

安全/签名/加密相关：
- `aesEncrypt(string)`
  - 用于加密 EXIF.userComment（bundle 中对 `getUserCommentObject` 的返回值进行 `aesEncrypt`）。

- `generateSecurityCode({ timestamp, latitude, longitude })`
  - 返回 `{code, signature}` 或字符串 code。

录像/水印相关：
- `updateRecordWatermark(array)` — 更新录像时的离屏水印内容。
- `startRecording(params)`
- `stopRecording()`

相机控制与配置：
- `startPreview()` / `stopPreview()`
- `switchCamera()`
- `setFlashMode(mode)`
- `setZoomFactor(value)`
- `captureStillImage`、`captureStillImageWithWatermark`
- `setFocusAndExposureAtPoint({x, y})`
- `setExposureTargetBias(value)`
- `openAppSettingPage()`

EXIF / watermark 构造要点（来自 bundle）:
- `watermarks` 元素通常为 `{ rect: {x,y,width,height}, watermarkId, frame, rotation? }`。
- `exifInfo` 包含 `artist`, `dateTime`, `latitude`, `longitude`, `userComment`（userComment 常先 `aesEncrypt(encodeURIComponent(JSON.stringify(...)))`）。
- `truephotoInfos` 是一个数组，官方 enum: TIMESTAMP=0, LOCATION=1, SECURITY_CODE=2；元素形态 `{ type: 0|1|2, data: string, signature: string }`。

桥接低层细节（官方模块行为）:
- 官方实现会生成 callbackId: `cb_${Date.now()}_${Math.random()}`，并将回调注册到 `_nativeCallbacks`。
- 在 Android 环境中，官方会调用：
  - `window.androidCallbackHandler.callNativeWithPromise(callbackId, method, JSON.stringify(params))`（可见于 `index.fb865daa.js`）
- native 完成后会调用：
  - `window.nt.O.nativePromiseCallback(callbackId, result, error)` 或 `window.nativePromiseCallback(callbackId, result, error)`，JS 端需删除回调并 resolve/reject。

调试提示：
- 如果页面报 `bridge_timeout`，说明在页面加载时官方模块尚未注入；可延长等待或使用后台轮询（已在 `app.js` 中实现）。
- 若 native 调用使用不同参数顺序（有少数桥接实现期望 `method, params, callbackId`），`app.js` 中已尝试两种顺序的调用以兼容。

文件位置：
- 官方 bundle（可参考）：
  - `src/android/assets/XHeyCameraSDKAssets/js/index.fb865daa.js`（包含模块 `91304` 实现）
  - `src/android/assets/XHeyCameraSDKAssets/js/entry-wps.b9931a1f.js`（入口，调用多个 native 方法）
  - `src/android/assets/XHeyCameraSDKAssets/js/441.18cd33f9.js`（提供 `prepareCaptureForComposite` / `compositeWatermarksOnCapture` 的封装）

后续：
- 我可以继续 prettify 这些文件并把 `watermarkModel` / `watermarkContentExtension` 的默认字段精确提取到 `buildCompositePayload` 中。如果需要，我会把这些默认值直接写回 `MyXheyVue/app.js`。