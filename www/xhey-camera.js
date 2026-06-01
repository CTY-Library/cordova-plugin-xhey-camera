var exec = require('cordova/exec');

var XheyCamera = {
  // 直接打开拍照页（使用安装时注入的 APPID/SECRET_KEY）
  // signature: takePhoto(success, error, options)
  // options.returnType: 'base64' | 'file' (default 'base64')
  // options.useCustomUI: boolean (default false) - 是否使用自定义水印UI
  // options.resourceDir: string - 自定义UI资源目录路径
  takePhoto: function(success, error, options) {
    options = options || {};
    if (typeof options.saveToGallery === 'undefined') options.saveToGallery = true;
    if (typeof options.useCustomUI === 'undefined') options.useCustomUI = false;
    exec(success, error, 'XheyCamera', 'takePhoto', [options]);
  },

  // 连拍：count 为拍照张数，回调返回 Base64 字符串数组或 file url（由 options.returnType 控制）
  // signature: takeBurst(count, success, error, options)
  // options.useCustomUI: boolean (default false) - 是否使用自定义水印UI
  // options.resourceDir: string - 自定义UI资源目录路径
  takeBurst: function(count, success, error, options) {
    options = options || {};
    if (typeof options.saveToGallery === 'undefined') options.saveToGallery = true;
    if (typeof options.useCustomUI === 'undefined') options.useCustomUI = false;
    options.maxImageCount = count || 1;
    // indicate continuous shot intent to native SDK when possible
    if (typeof options.continuousShot === 'undefined') options.continuousShot = true;

    // Internal single-retry on TruePhoto-like failures: if first call fails
    // with a TruePhoto error, retry once with skipTruePhoto.
    var didRetry = false;
    function onErrorOnce(err) {
      try {
        var msg = String(err || '');
        if (!didRetry && (msg.indexOf('TruePhoto') !== -1 || msg.indexOf('NO_IMAGES_RETRIED') !== -1 || msg.indexOf('TIME') !== -1)) {
          didRetry = true;
          var retryOpts = Object.assign({}, options, { skipTruePhoto: true });
          exec(success, function(err2){
            // if still error, forward original error
            error(err2);
          }, 'XheyCamera', 'takePhoto', [retryOpts]);
          return;
        }
      } catch (e) { }
      // otherwise forward error
      error(err);
    }

    exec(success, onErrorOnce, 'XheyCamera', 'takePhoto', [options]);
  }
  ,

  // 启动实时预览（在宿主页面上会打开 SDK 的预览界面）
  // options.useCustomUI: boolean (default false) - 是否使用自定义水印UI
  // options.resourceDir: string - 自定义UI资源目录路径
  startPreview: function(options, success, error) {
    options = options || {};
    if (typeof options.useCustomUI === 'undefined') options.useCustomUI = false;
    exec(success, error, 'XheyCamera', 'startPreview', [options]);
  },

  // 停止实时预览
  stopPreview: function(success, error) {
    exec(success, error, 'XheyCamera', 'stopPreview', []);
  },

  // 切换前后摄像头，position: 'front'|'back'
  switchCamera: function(position, success, error) {
    exec(success, error, 'XheyCamera', 'switchCamera', [{ cameraPosition: position }]);
  }
};

// 配置（可选）: 存储一些非敏感配置到原生层，用于后续调用
XheyCamera.configure = function(config, success, error) {
  exec(success, error, 'XheyCamera', 'configure', [config || {}]);
};

module.exports = XheyCamera;