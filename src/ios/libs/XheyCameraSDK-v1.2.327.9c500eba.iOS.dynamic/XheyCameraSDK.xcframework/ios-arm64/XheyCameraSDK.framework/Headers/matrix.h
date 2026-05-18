#pragma once

namespace xhey::camera {

class Matrix {
public:
    Matrix();
    Matrix(const Matrix &other);
    Matrix &operator=(const Matrix &other);
    ~Matrix();

    void SetIdentity();
    void Scale(float x, float y, float z);
    void Rotate(float angle, float x, float y, float z);
    void Translate(float x, float y, float z);

    // 矩阵乘法
    void Multiply(const Matrix &other);
    Matrix operator*(const Matrix &other) const;

    // 投影矩阵
    void SetPerspective(float fovy, float aspect, float near, float far);
    void SetOrtho(float left, float right, float bottom, float top, float near, float far);
    void SetFrustum(float left, float right, float bottom, float top, float near, float far);

    // 视图矩阵
    void SetLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY,
                   float upZ);

    // 返回矩阵数据指针（OpenGL列主序）
    float *Data() { return data_; }
    const float *Data() const { return data_; }

    // 获取特定元素
    float &operator()(int row, int col) { return data_[col * 4 + row]; }
    const float &operator()(int row, int col) const { return data_[col * 4 + row]; }

private:
    void Multiply(const float *mat);
    void Normalize(float &x, float &y, float &z);
    void CrossProduct(float ax, float ay, float az, float bx, float by, float bz, float &cx, float &cy, float &cz);

private:
    float data_[16]; // OpenGL使用列主序存储
};

} // namespace xhey::camera
