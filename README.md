# cordova-plugin-xhey-camera

轻量的 Cordova 插件，封装今日水印（XheyCamera SDK）。

快速安装（推荐）

```bash
cordova plugin add /path/to/cordova-plugin-xhey-camera
```
 
在宿主应用的 `config.xml` 中通过 `preference` 设置凭证，插件及原生端会优先读取这些值：

```xml
<widget id="com.example.app" version="1.0.0" xmlns="http://www.w3.org/ns/widgets" xmlns:cdv="http://cordova.apache.org/ns/1.0">
  <!-- 在这里添加： -->
  <preference name="APPID" value="your_appid" />
  <preference name="SECRET_KEY" value="your_secret" />
</widget>
```

说明：
- Android：原生层会优先从 Cordova `preference` 读取 `APPID`/`SECRET_KEY`，若未设置再回退到旧的资源字段或 manifest meta-data。原来自动写入 `res/values` 的内联注入已移除，建议使用 `preference`。
- iOS：原生层会优先从 Cordova `preference` 读取 `APPID`/`SECRET_KEY`，若未设置再回退到 `Info.plist` 中的 `xhey_appid` / `xhey_secret_key`（如果你需要自动写入 Info.plist，请手动在宿主工程中添加对应键）。

完整 JS 使用示例（按步骤）

1) 安装并配置插件（仅一次）

```bash
cordova plugin add /path/to/cordova-plugin-xhey-camera --variable APPID="your_appid" --variable SECRET_KEY="your_secret"
```

2) 在页面中引入并初始化（可选全局配置）

```js
// 可选：保存一些非敏感的默认值到原生层
XheyCamera.configure({ groupWatermarkId: '74542b94-ef29-4fff-a9f3-68c5f2823c71', maxImageCount: 3 });
```

3) 单张拍照 — 获取 Base64（默认）

```js
// 默认返回 base64 数据
XheyCamera.takePhoto(function(result){
  if (result && result.images && result.images.length) {
    document.getElementById('img').src = 'data:image/jpeg;base64,' + result.images[0];
  }
  if (result && result.meta && result.meta.userCommentObject) {
    console.log('meta', result.meta.userCommentObject);
  }
}, function(err){ console.error('takePhoto error', err); });
```

4) 单张拍照 — 返回本地文件路径（`returnType: 'file'`）

```js
// 请求原生直接写入文件并返回 fileUris
XheyCamera.takePhoto(function(result){
  if (result && result.fileUris && result.fileUris.length) {
    const uri = result.fileUris[0];
    // 简单方式：直接赋值给 <img>（多数 Android WebView 支持 file://）
    document.getElementById('img').src = uri;

    // 更稳健的方式：使用 Cordova File API 读取为 Data URL（推荐，兼容性更好）
    if (window.resolveLocalFileSystemURL) {
      window.resolveLocalFileSystemURL(uri, function(fileEntry){
        fileEntry.file(function(file){
          const reader = new FileReader();
          reader.onloadend = function(){ document.getElementById('img').src = this.result; };
          reader.readAsDataURL(file);
        }, function(err){ console.error('fileEntry.file error', err); });
      }, function(err){ console.error('resolveLocalFileSystemURL error', err); });
    }
  } else if (result && result.images && result.images.length) {
    // 回落为 base64
    document.getElementById('img').src = 'data:image/jpeg;base64,' + result.images[0];
  }
}, function(err){ console.error('takePhoto error', err); }, { returnType: 'file' });
```

5) 连拍（burst）

```js
// 连拍 count 张，options 可传入 returnType 等
XheyCamera.takeBurst(3, function(result){ console.log('burst result', result); }, function(err){ console.error(err); }, { returnType: 'file' });
```

6) 预览与切换摄像头

```js
XheyCamera.startPreview({ cameraPosition: 'back' }, ()=>console.log('preview started'), (e)=>console.error(e));
XheyCamera.switchCamera('front', ()=>console.log('switched'), (e)=>console.error(e));
XheyCamera.stopPreview(()=>console.log('preview stopped'), (e)=>console.error(e));
```

7) 常用 runtime options（示例）

- `returnType`: `'base64'`（默认）或 `'file'`
- `maxImageCount`, `imageQuality`, `saveToGallery`, `locationLatitude`, `locationLongitude`, `needPhotoConfirm` 等

8) Promise / async 封装

```js
function takePhotoAsync(options){
  return new Promise((resolve,reject)=>{
    XheyCamera.takePhoto((res)=>resolve(res),(err)=>reject(err), options);
  });
}

(async ()=>{
  try {
    const r = await takePhotoAsync({ returnType: 'file' });
    console.log('photo result', r);
  } catch (e) { console.error(e); }
})();
```

返回数据说明

- 成功时：`{ images: ["<base64>"...], fileUris: ["file://..."...], meta: { userCommentObject: {...} } }`。
- 当 `returnType: 'file'` 时优先返回 `fileUris`（写入到应用缓存/临时目录）；若写入失败，可能回落返回 `images`（base64）。





返回结构（JS）

- 成功时：{ images: ["<base64>", ...], meta: { userCommentObject: {...} } }
- `userCommentObject` 包含拍照时间、经纬度、海拔、防伪码等（字段名示例：captureTimestampMs、latitude、longitude、altitude、antiFakeCode、rawJson）。

主要 API

- `configure(options)`：保存全局配置（例如 `groupWatermarkId`）。
- `takePhoto(success, error)`：拍照并返回结果。
- `takeBurst(count, success, error)`：连拍。
- `startPreview(options, success, error)`：启动预览。
- `stopPreview(success, error)`：停止预览。
- `switchCamera(position, success, error)`：切换摄像头，`position` 为 `'front'` 或 `'back'`。

常用可传参数（示例）

- `groupWatermarkId`、`maxImageCount`、`needPhotoConfirm`、`customInputItems`、`imageQuality`、`burstCount` 等。

平台说明

- Android：插件已内置 `src/android/libs/xheycamerasdk-release.aar` 与资源。`APPID`/`SECRET_KEY` 在安装时写入 `res/values/strings.xml`，运行时由原生读取并传给 SDK。
- iOS：把 SDK (`.framework`/`.xcframework`) 放入 `src/ios/libs/`。安装时传入 `APPID`/`SECRET_KEY` 会写入 `Info.plist`（键名：`xhey_appid` / `xhey_secret_key`）。仓库已包含 `src/ios/XheyCamera.h` / `src/ios/XheyCamera.m` 模板，已实现与 SDK 的基本交互与结果序列化。

注意与故障排查

- 权限：确保宿主应用已申请相机与定位权限（Android：`CAMERA`、`ACCESS_FINE_LOCATION`；iOS：`NSCameraUsageDescription`、`NSLocationWhenInUseUsageDescription`）。
- 若 SDK 的 Activity/VC 名称或 Intent/参数键与默认不符，可通过 `options.activityClassName`（Android）或修改 `src/ios/XheyCamera.m`（iOS）进行适配。
- 真机验证：请在真机上先调用 `takePhoto`/`takeBurst` 并把返回的 `meta.userCommentObject` JSON 发给我，我可以据此调整序列化与字段映射以兼容你使用的 SDK 版本。

贡献与许可

- 欢迎提交 issue/PR 来改进插件或适配不同的 SDK 版本。
- 许可证：MIT
