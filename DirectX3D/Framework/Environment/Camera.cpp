#include "Framework.h"

Camera::Camera()
{
    tag = "Camera";

    viewBuffer = new ViewBuffer();
    viewBuffer->SetVS(1);

    Load();

    prevMousePos = mousePos;
}

Camera::~Camera()
{
    delete viewBuffer;

    Save();
}

void Camera::Update()
{
    projection = Environment::Get()->GetProjection();
    Frustum();

    if (target)
        FollowMode();
    else
        FreeMode();
    ShakeTime();
    UpdateWorld();
    if (!DoingShake)
        m_vRestorePos = Pos();
    view = XMMatrixInverse(nullptr, world);
    viewBuffer->Set(view, world);
}

void Camera::GUIRender()
{
    if (ImGui::TreeNode("CameraOption"))
    {
        ImGui::DragFloat("MoveSpeed", &moveSpeed);
        ImGui::DragFloat("RotSpeed", &rotSpeed);

        if (target && ImGui::TreeNode("TargetOption"))
        {
            ImGui::DragFloat("Distance", &distance, 0.1f);
            ImGui::DragFloat("Height", &height, 0.1f);
            ImGui::DragFloat3("FocusOffset", (float*)&focusOffset, 0.1f);

            float degree = XMConvertToDegrees(rotY);
            ImGui::DragFloat("RotY", &degree, 0.1f);
            rotY = XMConvertToRadians(degree);

            ImGui::DragFloat("MoveDamping", &moveDamping, 0.1f);
            ImGui::DragFloat("RotDamping", &rotDamping, 0.1f);

            ImGui::Checkbox("LookAtTargetX", &isLookAtTargetX);
            ImGui::Checkbox("LookAtTargetY", &isLookAtTargetY);
            
            ImGui::InputText("File", file, 128);

            if(ImGui::Button("Save"))
                TargetOptionSave(file);
            ImGui::SameLine();
            if (ImGui::Button("Load"))
                TargetOptionLoad(file);

            ImGui::TreePop();
        }

        Transform::GUIRender();

        ImGui::TreePop();
    }
}

void Camera::SetView()
{  
    viewBuffer->SetVS(1);
    viewBuffer->SetPS(1);
}

Vector3 Camera::ScreenToWorld(Vector3 screenPos)
{
    return XMVector3TransformCoord(screenPos, world);
}

Vector3 Camera::WorldToScreen(Vector3 worldPos)
{
    Vector3 screenPos;

    screenPos = XMVector3TransformCoord(worldPos, view); // 월드좌표에 뷰 행렬을 곱한다 -> 뷰상에서의 좌표가 나옴
    screenPos = XMVector3TransformCoord(screenPos, projection); // 뷰상에서의 좌표에 프로젝션 행렬을 곱한다 -> 프로젝션 상에서의 좌표가 나옴
    //NDC : -1 ~ 1

    screenPos = (screenPos + Vector3::One()) * 0.5f;//0~1

    screenPos.x *= WIN_WIDTH;
    screenPos.y *= WIN_HEIGHT;

    return screenPos;
}

Ray Camera::ScreenPointToRay(Vector3 screenPoint)
{
    Vector3 screenSize(WIN_WIDTH, WIN_HEIGHT, 1.0f);

    Vector2 point;
    point.x = (screenPoint.x / screenSize.x) * 2.0f - 1.0f;
    point.y = (screenPoint.y / screenSize.y) * 2.0f - 1.0f;    

    Float4x4 temp;
    XMStoreFloat4x4(&temp, projection);

    screenPoint.x = point.x / temp._11;
    screenPoint.y = point.y / temp._22;
    screenPoint.z = 1.0f;

    screenPoint = XMVector3TransformNormal(screenPoint, world);

    Ray ray;
    ray.pos = Pos();
    ray.dir = screenPoint.GetNormalized();

    return ray;
}

Vector3 Camera::ScreenPointToRayDir(Vector3 screenPoint)
{
    Vector3 screenSize(WIN_WIDTH, WIN_HEIGHT, 1.0f);

    Vector2 point;
    point.x = (screenPoint.x / screenSize.x) * 2.0f - 1.0f;
    point.y = (screenPoint.y / screenSize.y) * 2.0f - 1.0f;

    Float4x4 temp;
    XMStoreFloat4x4(&temp, projection);

    screenPoint.x = point.x / temp._11;
    screenPoint.y = point.y / temp._22;
    screenPoint.z = 1.0f;

    screenPoint = XMVector3TransformNormal(screenPoint, world);

    Ray ray;
    ray.pos = Pos();
    ray.dir = screenPoint.GetNormalized();


    return  ray.dir;
}

void Camera::LookAtTarget()
{    
    rotMatrix = XMMatrixRotationY(target->Rot().y + rotY);

    Vector3 forward = XMVector3TransformNormal(Vector3::Forward(), rotMatrix);

    Pos() = target->GlobalPos() + forward * -distance;
    Pos().y += height;    

    Vector3 offset = XMVector3TransformCoord(focusOffset, rotMatrix);
    Vector3 targetPos = target->GlobalPos() + offset;

    Vector3 dir = (targetPos - Pos()).GetNormalized();
    forward = Vector3(dir.x, 0.0f, dir.z).GetNormalized();

    Rot().x = acos(Dot(forward, dir));
    Rot().y = atan2(dir.x, dir.z);
}

void Camera::FreeMode()
{
    Vector3 delta = mousePos - prevMousePos;
    prevMousePos = mousePos;

    if (KEY_PRESS(VK_RBUTTON))
    {
        if (KEY_PRESS('W'))
            Pos() += Forward() * moveSpeed * DELTA;
        if (KEY_PRESS('S'))
            Pos() += Back() * moveSpeed * DELTA;
        if (KEY_PRESS('A'))
            Pos() += Left() * moveSpeed * DELTA;
        if (KEY_PRESS('D'))
            Pos() += Right() * moveSpeed * DELTA;
        if (KEY_PRESS('E'))
            Pos() += Down() * moveSpeed * DELTA;
        if (KEY_PRESS('Q'))
            Pos() += Up() * moveSpeed * DELTA;

        Rot().x -= delta.y * rotSpeed * DELTA;
        Rot().y += delta.x * rotSpeed * DELTA;
    }
}

void Camera::FollowMode()
{   
    destRot = Lerp(destRot, target->Rot().y, rotDamping * DELTA);    
    rotMatrix = XMMatrixRotationY(destRot + rotY);

    Vector3 forward = XMVector3TransformNormal(Vector3::Forward(), rotMatrix);
    if (KEY_DOWN(VK_RBUTTON))
    {
        DoingShake = true;
    }

    destPos = target->GlobalPos() + forward * -distance;
    destPos.y += height;

    Pos() = Lerp(Pos(), destPos, moveDamping * DELTA);
    //Pos() = destPos;

    Vector3 offset = XMVector3TransformCoord(focusOffset, rotMatrix);
    Vector3 targetPos = target->GlobalPos() + offset;

    Vector3 dir = (targetPos - Pos()).GetNormalized();
    forward = Vector3(dir.x, 0.0f, dir.z).GetNormalized();

    if (isLookAtTargetX)
    {
        Rot().x = acos(Dot(forward, dir));        
    }    
    if (isLookAtTargetY)
    {
        Rot().y = atan2(dir.x, dir.z);
    }
}

void Camera::Frustum()
{
    Float4x4 VP;
    XMStoreFloat4x4(&VP, view * projection);

    //Left
    a = VP._14 + VP._11;
    b = VP._24 + VP._21;
    c = VP._34 + VP._31;
    d = VP._44 + VP._41;
    planes[0] = XMVectorSet(a, b, c, d);

    //Right
    a = VP._14 - VP._11;
    b = VP._24 - VP._21;
    c = VP._34 - VP._31;
    d = VP._44 - VP._41;
    planes[1] = XMVectorSet(a, b, c, d);

    //Bottom
    a = VP._14 + VP._12;
    b = VP._24 + VP._22;
    c = VP._34 + VP._32;
    d = VP._44 + VP._42;
    planes[2] = XMVectorSet(a, b, c, d);

    //Top
    a = VP._14 - VP._12;
    b = VP._24 - VP._22;
    c = VP._34 - VP._32;
    d = VP._44 - VP._42;
    planes[3] = XMVectorSet(a, b, c, d);

    //Near
    a = VP._14 + VP._13;
    b = VP._24 + VP._23;
    c = VP._34 + VP._33;
    d = VP._44 + VP._43;
    planes[4] = XMVectorSet(a, b, c, d);

    //Far
    a = VP._14 - VP._13;
    b = VP._24 - VP._23;
    c = VP._34 - VP._33;
    d = VP._44 - VP._43;
    planes[5] = XMVectorSet(a, b, c, d);

    FOR(6)
        planes[i] = XMPlaneNormalize(planes[i]);
}

void Camera::TargetOptionSave(string file)
{
    string path = "TextData/Camera/" + file + ".cam";

    BinaryWriter* writer = new BinaryWriter(path);

    writer->Float(distance);
    writer->Float(height);
    writer->Float(moveDamping);
    writer->Float(rotDamping);
    writer->Float(rotY);
    writer->Vector(focusOffset);
    writer->Bool(isLookAtTargetX);
    writer->Bool(isLookAtTargetY);

    delete writer;
}

void Camera::TargetOptionLoad(string file)
{
    string path = "TextData/Camera/" + file + ".cam";

    BinaryReader* reader = new BinaryReader(path);

    distance = reader->Float();
    height = reader->Float();
    moveDamping = reader->Float();
    rotDamping = reader->Float();
    rotY = reader->Float();
    focusOffset = reader->Vector();
    isLookAtTargetX = reader->Bool();
    isLookAtTargetY = reader->Bool();

    delete reader;
}

bool Camera::ContainPoint(Vector3 point)
{
    FOR(6)
    {
        Vector3 dot = XMPlaneDotCoord(planes[i], point);

        if (dot.x < 0.0f)
            return false;
    }

    return true;
}

bool Camera::ContainPoint(Vector3 center, float radius)
{
    Vector3 edge; // 가장자리
    Vector3 dot; //점곱에 의한 재계산 벡터

    FOR(6)
    {

        //1
        edge.x = center.x - radius;
        edge.y = center.y - radius;
        edge.z = center.z - radius;

        dot = XMPlaneDotCoord(planes[i], edge);
                                  // 바깥- 평면 안+
        if (dot.x > 0) continue;  //         |  
                                  //점이 안쪽인가?
                                  //안쪽이면 다음 평면계산


        //2
        edge.x = center.x + radius;
        edge.y = center.y - radius;
        edge.z = center.z - radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;


        //3
        edge.x = center.x - radius;
        edge.y = center.y + radius;
        edge.z = center.z - radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;

        //4
        edge.x = center.x - radius;
        edge.y = center.y - radius;
        edge.z = center.z + radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;

        //5
        edge.x = center.x + radius;
        edge.y = center.y + radius;
        edge.z = center.z - radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;

        //6
        edge.x = center.x + radius;
        edge.y = center.y - radius;
        edge.z = center.z + radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;

        //7
        edge.x = center.x - radius;
        edge.y = center.y + radius;
        edge.z = center.z + radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;

        //8
        edge.x = center.x + radius;
        edge.y = center.y + radius;
        edge.z = center.z + radius;

        dot = XMPlaneDotCoord(planes[i], edge);
        if (dot.x > 0) continue;


        //하나의 평면에 각 점을 비교했을때 안쪽이었던 점이 하나도 없음 ->> 볼것도 없이 무조건 바깥임
        
        //이렇게 8번의 컨티뉴를 통과하고 여기까지 왔다면
        //->dot을 8번이나 계산을 해줬는데 법선값이 한번도 0보다 큰 적이 없었다
        //->center와 radius로 정의된 대상 영역이 현재 카메라의 프러스텀 밖으로 나갔다

        return false;
    }

    //여기까지 왔다면 : 반복문을 6번이나, 검사를 48번이나 했는데 한번도 폴스가 없었다
    //각 평면에서 안쪽이었던점이 최소 1개 이상씩 있음(continue가 작동했으므로) >> 모든 평면에서 안쪽인 점이 있다?

    return true; //이것보다 참인 경우는 없다 = 해당 영역의 최소 일부는 반드시 프러스텀 안에 있다
}

 void Camera::ShakeTime()
{
    if (!DoingShake)return;
    float fShakeFactor = 5.f;
    if (shakestrength < 3.f * 3.141592 * fShakeFactor)
    {
        Pos().y = m_vRestorePos.y + 3.f * sin(shakestrength);
        Pos().z = m_vRestorePos.z + 3.f * -sin(shakestrength);

        shakestrength += fShakeFactor;
    }
    else
    {
        Pos() = m_vRestorePos;
        shakestrength = 0.0f;
        DoingShake = FALSE;
        //m_tEffectType = CAMERA_EFFECT_TYPE::NONE;
    }

   
}
