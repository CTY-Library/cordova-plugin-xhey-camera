# Watermark / EXIF Payload Schema

此文档给出与 SDK 官方 bundle 等价的 watermark/exif payload 结构、TypeScript 类型和示例 JSON，方便在自定义页面中精确构造请求以调用 `prepareCaptureForComposite`、`compositeWatermarksOnCapture` 和 `captureStillImageWithWatermark`。

## 概要
- `watermarks`: 数组，每项描述一个可绘制的文字或图片水印。
- `exifInfo` (可选): 捕获图片的 EXIF 元数据（定位/方向/时间等）。
- `truephotoInfos` (可选): 针对“真图”校验或额外覆盖的元信息数组（某些 SDK 版本使用）。

## TypeScript 类型示例
```ts
type Rect = { x: number; y: number; width: number; height: number }; // 0..1 相对坐标

type WatermarkText = {
  type: 'text';
  rect: Rect;
  text: string;
  fontSize?: number; // px
  color?: string; // CSS 颜色
  opacity?: number; // 0..1
  rotation?: number; // degrees
  zIndex?: number;
};

type WatermarkImage = {
  type: 'image';
  rect: Rect;
  srcBase64?: string; // base64 data (no data: prefix) or remote URI
  opacity?: number;
  rotation?: number;
  zIndex?: number;
};

type Watermark = WatermarkText | WatermarkImage;

type ExifInfo = {
  orientation?: number; // 1..8
  gps?: { lat: number; lon: number; altitude?: number; accuracy?: number };
  timestamp?: string; // ISO 8601
  device?: { make?: string; model?: string; id?: string };
  [k: string]: any; // 兼容扩展字段
};

type TruephotoInfo = {
  id?: string;
  reason?: string;
  rect?: Rect; // 可选定位
  [k: string]: any;
};

type CompositePayload = {
  watermarks: Watermark[];
  exifInfo?: ExifInfo;
  truephotoInfos?: TruephotoInfo[];
  saveToGallery?: boolean; // SDK 可能识别该字段
  quality?: number; // 0..100
};
```

## JSON Schema（精简）
```json
{
  "type":"object",
  "properties":{
    "watermarks":{"type":"array","items":{"type":"object"}},
    "exifInfo":{"type":"object"},
    "truephotoInfos":{"type":"array","items":{"type":"object"}},
    "saveToGallery":{"type":"boolean"},
    "quality":{"type":"integer","minimum":0,"maximum":100}
  },
  "required":["watermarks"]
}
```

## 推荐实际构造（官方 bundle 里常见模式）
- 相对坐标使用 0..1（x,y,width,height），左上为 (0,0)，右下为 (1,1)。
- 文字水印通常传 `text,fontSize,color,opacity,rect`。
- 图片水印可传 `srcBase64`（去除 data: 前缀）或 `srcUri`，并包含 rect/opacity/rotation。

## 示例：最常用 payload
```json
{
  "watermarks":[
    {
      "type":"text",
      "rect":{"x":0.05,"y":0.85,"width":0.9,"height":0.1},
      "text":"示例水印",
      "fontSize":24,
      "color":"#FFFFFF",
      "opacity":0.9,
      "rotation":0
    }
  ],
  "saveToGallery":true,
  "quality":90
}
```

## 示例：含 EXIF 与真图信息
```json
{
  "watermarks":[ /* 如上 */ ],
  "exifInfo":{
    "orientation":1,
    "gps":{"lat":31.2304,"lon":121.4737},
    "timestamp":"2026-05-29T10:00:00Z",
    "device":{"make":"XPhone","model":"X1"}
  },
  "truephotoInfos":[{"id":"tp-1","reason":"verify"}],
  "saveToGallery":false
}
```

## 使用提示
- 调用 `prepareCaptureForComposite` 时不需要包含 payload；该方法常用于让 native 进入合成准备状态。
- 调用 `compositeWatermarksOnCapture` 时传入上述 `CompositePayload`，native 会返回合成结果（通常为 base64 字符串或对象）。
- 如果 `compositeWatermarksOnCapture` 返回结构化对象（含 code/message），请按照 SDK 的错误语义处理并在日志中记录完整对象以便对比官方 bundle。

## 我可以继续
- 将该 schema 直接内嵌进 `MyXheyVue/app.js` 的 `bindCaptureButton` 处以生成严格匹配的 payload（含字段顺序与默认值）。
- 或生成 TypeScript/JSON Schema 文件供编译时校验。

