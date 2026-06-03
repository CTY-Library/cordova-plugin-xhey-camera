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
// 单张（默认 base64 编码，保存到相册）
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
- `useCustomUI`: `true|false`（默认 `false`）- 是否使用自定义水印UI。
- `resourceDir`: `string` - 自定义UI资源目录路径（配合 `useCustomUI` 使用）。

详细参数见 `www/xhey-camera.js`。

## 自定义水印UI (TODO-待完善:需要官方SDK定制开发)

插件支持使用自定义的水印UI界面，通过以下参数控制：

### 参数说明

- **useCustomUI**: `boolean`（默认 `false`）
  - 设置为 `true` 时启用自定义水印UI
  - 默认使用SDK内置的UI界面
  - **Android**: 当启用时，会使用 `CustomCameraActivity` 替代默认的 `CameraActivity`
  - **iOS**: 通过 SDK 配置传递 `useCustomUI` 参数，SDK 会根据参数选择相应的UI

- **resourceDir**: `string`
  - 自定义UI资源目录的路径
  - 如果不指定，会自动按优先级查找：
    - Android: `www/XheyCameraSDKAssets/MyXheyVue/index.html` → `www/XheyCameraSDKAssets/index.html` → `XheyCameraSDKAssets`
    - iOS: `XheyCameraSDKAssets/MyXheyVue/index.html` → `XheyCameraSDKAssets/index.html` → `XheyCameraSDKResource.bundle`

### 工作原理

**Android 端**：
- `useCustomUI = false`: 使用 `com.xhey.xheycamerasdk.CameraActivity`（SDK默认Activity）
- `useCustomUI = true`: 使用 `org.xhey.cordova.camera.CustomCameraActivity`（自定义Activity，支持JS桥接）
- 自动解析资源目录，优先使用自定义UI资源

**iOS 端**：
- 通过 SDK 配置对象传递 `useCustomUI` 和 `resourceDir` 参数
- SDK 内部根据参数选择相应的UI界面
- 自动解析资源目录，优先使用自定义UI资源

### 使用示例

```javascript
// 启用自定义水印UI拍照
XheyCamera.takePhoto(
  (result) => {
    console.log('拍照成功', result);
  },
  (error) => {
    console.error('拍照失败', error);
  },
  {
    useCustomUI: true,
    resourceDir: 'path/to/custom/ui',
    returnType: 'base64',
    saveToGallery: true
  }
);

// 连拍模式使用自定义UI
XheyCamera.takeBurst(
  3,
  (result) => {
    console.log('连拍成功', result);
  },
  (error) => {
    console.error('连拍失败', error);
  },
  {
    useCustomUI: true,
    resourceDir: 'path/to/custom/ui',
    returnType: 'file'
  }
);

// 预览模式使用自定义UI
XheyCamera.startPreview(
  {
    useCustomUI: true,
    resourceDir: 'path/to/custom/ui'
  },
  () => {
    console.log('预览已启动');
  },
  (error) => {
    console.error('预览启动失败', error);
  }
);
```

### 注意事项

1. **默认行为**: `useCustomUI` 默认为 `false`，使用SDK内置UI，保持向后兼容性

2. **资源目录**: 
   - Android: 资源需要放在 `assets` 目录下
   - iOS: 资源需要打包到应用bundle中

3. **自定义UI开发**: 需要根据SDK文档开发自定义UI界面，确保与SDK接口兼容

4. **测试建议**: 在启用自定义UI前，先使用默认UI测试基本功能正常

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

更新时间: 2026-06-01
