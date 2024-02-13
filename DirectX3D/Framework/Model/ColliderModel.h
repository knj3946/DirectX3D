#pragma once
class ColliderModel : public Transform
{
public:
    ColliderModel(string name);
    ~ColliderModel();

    void Render();
    void GUIRender();

    void UpdateWorld();

    void SetShader(wstring file);

    void AddCollider(Collider* collider);
private:
    void ReadMaterial();
    void ReadMesh();

protected:
    string name;

    vector<Material*> materials;
    vector<ModelMesh*> meshes;
    vector<NodeData> nodes;
    vector<BoneData> bones;

    map<string, UINT> boneMap;

    WorldBuffer* worldBuffer;

    vector<Collider*> colliders;
};