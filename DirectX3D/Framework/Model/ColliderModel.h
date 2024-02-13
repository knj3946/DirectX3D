#pragma once
class ColliderModel : public Transform
{
public:
    ColliderModel(string name);
    ~ColliderModel();

    void Render();
    void GUIRender();

    void SetShader(wstring file);

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
};