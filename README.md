# cordova-plugin-xhey-camera

轻量的 Cordova 插件，封装 今日水印 XheyCamera SDK 的拍照、连拍与预览功能。

https://docs.xhey.top/docs/camerasdk



## 概览

提供在 Cordova Web 层调用本地 SDK 的简单接口：

- `takePhoto(success, error, options)`：单张拍照。
- `takeBurst(count, success, error, options)`：连拍。
- `startPreview` / `stopPreview` / `switchCamera`：预览控制。

## 安装

```bash
cordova plugin add /path/to/cordova-plugin-xhey-camera --variable APPID="your_appid" --variable SECRET_KEY="your_secret"
```

或在 `config.xml` 中加入：

```xml
<preference name="APPID" value="your_appid" />
<preference name="SECRET_KEY" value="your_secret" />
```

## 快速示例

```js
// 单张（默认 base64）
XheyCamera.takePhoto(res=>console.log(res), err=>console.error(err));

// 连拍 3 张（返回 file URI）
XheyCamera.takeBurst(3, res=>console.log(res), err=>console.error(err), { returnType: 'file' });
```

## API 概览

- `configure(options)`：保存非敏感默认配置（例如 `groupWatermarkId`）。
- `takePhoto(success, error, options)`。
- `takeBurst(count, success, error, options)`（会设置 `maxImageCount=count`）。
- `startPreview(options, success, error)` / `stopPreview` / `switchCamera(position, success, error)`。

## 常用 options（摘要）

- `returnType`: `'base64'`（默认）或 `'file'`。
- `saveToGallery`: `true|false`（默认 `true`）。
- `maxImageCount`: 连拍最大图片数。
- `continuousShot`: 优化连拍模式（默认 `true`）。
- `skipTruePhoto`: 测试/兼容标志，`true` 可绕过 SDK 的 TruePhoto 校验（仅调试）。

详细参数见 `www/xhey-camera.js`。

## 连拍（Burst）简要说明

- 用法：`XheyCamera.takeBurst(count, success, error, options)`。
- 插件会把 `count` 传为 `maxImageCount`，JS 层在检测到 TruePhoto 相关错误时会做一次自动重试（重试时可设置 `skipTruePhoto`）。
- 原生层在无图时也会尝试带 `verifyMode=0` 的重试。
- 注意：`skipTruePhoto`/`verifyMode=0` 会降低真照片校验安全性，仅用于调试或短期兼容。

## 返回结构（摘要）

成功回调一般返回包含下列字段的对象：`images`（base64）、`fileUris`（文件 URI）、`galleryUris`、`downloadPaths`、`savedRelativePath`、`saveErrors`、`meta.userCommentObject`（拍照元信息）。

失败返回字符串或对象错误，常见码示例：`SAVE_TO_DOWNLOAD_FAILED`。

## 调试建议

抓取设备日志并筛选关键标签：

```bash
adb logcat | grep -E "XheyCameraPlugin|XHCameraSDK|JsBridge|chromium"
```

关注：`Merged options`、`Intent extra`、TruePhoto 相关错误、`NO_IMAGES_RETRIED_WITH_VERIFYMODE_0`。

如需我协助分析日志，请提供含关键词的截取内容。

## 贡献与许可

欢迎提交 issue / PR。许可证：MIT。

---

更新时间: 2026-05-21
