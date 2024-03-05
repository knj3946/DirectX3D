#pragma once

class MatrixBuffer : public ConstBuffer
{
public:
    MatrixBuffer() : ConstBuffer(&matrix, sizeof(Matrix))
    {
        matrix = XMMatrixIdentity();
    }

    void Set(Matrix value)
    {
        matrix = XMMatrixTranspose(value);
    }

private:
    Matrix matrix;
};

class WorldBuffer : public ConstBuffer
{
private:
    struct Data
    {
        Matrix world = XMMatrixIdentity();
        
        int type = 0;

        float padding[3];
    };

public:
    WorldBuffer() : ConstBuffer(&data, sizeof(Data))
    {
    }

    void Set(Matrix value)
    {
        data.world = XMMatrixTranspose(value);
    }

    void SetType(int type)
    {
        data.type = type;
    }

private:
    Data data;
};

class ViewBuffer : public ConstBuffer
{
private:
    struct Data
    {
        Matrix view;
        Matrix invView;
    };

public:
    ViewBuffer() : ConstBuffer(&data, sizeof(Data))
    {
        data.view = XMMatrixIdentity();
        data.invView = XMMatrixIdentity();
    }

    void Set(Matrix view, Matrix invView)
    {
        data.view = XMMatrixTranspose(view);
        data.invView = XMMatrixTranspose(invView);
    }

private:
    Data data;
};

class ColorBuffer : public ConstBuffer
{
public:
    ColorBuffer() : ConstBuffer(&color, sizeof(Float4))
    {
    }

    Float4& Get() { return color; }

private:
    Float4 color = { 1, 1, 1, 1 };
};

class IntValueBuffer : public ConstBuffer
{
public:
    IntValueBuffer() : ConstBuffer(values, sizeof(int) * 4)
    {
    }

    int* Get() { return values; }

private:
    int values[4] = {};
};

class FloatValueBuffer : public ConstBuffer
{
public:
    FloatValueBuffer() : ConstBuffer(values, sizeof(float) * 4)
    {
    }

    float* Get() { return values; }

private:
    float values[4] = {};
};

class LightBuffer : public ConstBuffer
{
public:
    struct Light
    {
        Float4 color = { 1, 1, 1, 1 };
        Float3 direction = { 0, -1, 1 };
        int type = 0;

        Float3 pos = { 0, 0, 0 };
        float range = 100.0f;

        float inner = 55.0f;
        float outer = 70.0f;
        float length = 50.0f;
        int active = 1;
    };
private:
    struct Data
    {
        Light lights[MAX_LIGHT];

        UINT lightCount = 1;
        Float3 ambientLight = { 0.1f, 0.1f, 0.1f };
        Float3 ambientCeil = { 0.1f, 0.1f, 0.1f };
        float padding;
    };

public:
    LightBuffer() : ConstBuffer(&data, sizeof(Data))
    {
    }

    Data& Get() { return data; }

private:
    Data data;
};

class WeatherBuffer : public ConstBuffer
{
private:
    struct Data
    {
        Float3 velocity = { 0, -1, 0 };
        float distance = 100.0f;

        Float4 color = { 1, 1, 1, 1 };

        Float3 origin = {};
        float time = 0.0f;

        Float3 size = { 50, 50, 50 };
        float turbulence = 5.0f;
    };
public:
    WeatherBuffer() : ConstBuffer(&data, sizeof(Data))
    {
    }

    Data& Get() { return data; }

private:
    Data data;
};

class FogBuffer : public ConstBuffer
{
private:

    //안개 버퍼 (실제로 쓰이는 안개의) 가장 단순한 안개 형태를 구현한 것
    // -> 이셰이더가 설정된 물체를 뿌옇게 보이게 한다

    struct Data 
    {        
        Float4 color = { 1, 1, 1, 1 }; // 안개의 색깔

        float start = 0.0f; // 안개가 시작되는 곳
        float random = 1.0f; // 안개가 강해지는 곳
                             //위 두 숫자를 나눗셈에 써서 뿌얘지는 정도를 정한다
        float padding[2]; // ->>
    };
public:
    FogBuffer() : ConstBuffer(&data, sizeof(Data))
    {
    }

    Data& Get() { return data; }

private:
    Data data;
};