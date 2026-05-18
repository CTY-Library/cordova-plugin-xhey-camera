var exec = require('cordova/exec');

var XheyCamera = {
  // 直接打开拍照页（使用安装时注入的 APPID/SECRET_KEY）
  // signature: takePhoto(success, error, options)
  // options.returnType: 'base64' | 'file' (default 'base64')
  takePhoto: function(success, error, options) {
    options = options || {};
    exec(success, error, 'XheyCamera', 'takePhoto', [options]);
  },

  // 连拍：count 为拍照张数，回调返回 Base64 字符串数组或 file url（由 options.returnType 控制）
  // signature: takeBurst(count, success, error, options)
  takeBurst: function(count, success, error, options) {
    options = options || {};
    options.maxImageCount = count || 1;
    exec(success, error, 'XheyCamera', 'takePhoto', [options]);
  }
  ,

  // 启动实时预览（在宿主页面上会打开 SDK 的预览界面）
  startPreview: function(options, success, error) {
    options = options || {};
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
