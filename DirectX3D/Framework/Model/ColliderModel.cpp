#include "Framework.h"

ColliderModel::ColliderModel(string name) : name(name)
{
    ReadMaterial();
    ReadMesh();

    worldBuffer = new WorldBuffer();
}

ColliderModel::~ColliderModel()
{
 
    for (Material* material : materials)
        delete material;

    for (ModelMesh* mesh : meshes)
        delete mesh;
    for (ModelVertex* vertex : vecvertex)
    {
        if (nullptr != vertex)
            delete[] vertex;
    }
    for (UINT* indi : vecuint)
    {
        if (nullptr != indi)
            delete[] indi;
    }

    delete worldBuffer;
}

void ColliderModel::Render()
{
    worldBuffer->Set(world);
    worldBuffer->SetVS(0);

    for (ModelMesh* mesh : meshes)
        mesh->Render();

    for (Collider* collider : colliders)
    {
        //collider->Render();
    }
}

void ColliderModel::GUIRender()
{
    Transform::GUIRender();

    for (Material* material : materials)
        material->GUIRender();

    for (Collider* collider : colliders)
        collider->GUIRender();
}

void ColliderModel::UpdateWorld()
{
    Transform::UpdateWorld();

    for (Collider* collider : colliders)
    {
        collider->UpdateWorld();
    }
}

void ColliderModel::SetShader(wstring file)
{
    for (Material* material : materials)
        material->SetShader(file);
}

void ColliderModel::AddCollider(Collider* collider)
{
    colliders.push_back(collider);
}

vector<Collider*>& ColliderModel::GetColliders()
{
    return colliders;
}

void ColliderModel::ReadMaterial()
{
    string file = "Models/Materials/" + name + "/" + name + ".mats";

    BinaryReader* reader = new BinaryReader(file);

    if (reader->IsFailed())
        assert(false);

    UINT size = reader->UInt();

    materials.reserve(size);

    FOR(size)
    {
        Material* material = new Material();
      
        material->Load(reader->String());

        materials.push_back(material);
    }

    delete reader;
}

void ColliderModel::ReadMesh()
{
    string file = "Models/Meshes/" + name + ".mesh";

    BinaryReader* reader = new BinaryReader(file);

    if (reader->IsFailed())
    {
        delete reader;
        assert(false);

    }
    UINT size = reader->UInt();

    meshes.reserve(size);

    FOR(size)
    {
        ModelMesh* mesh = new ModelMesh(reader->String());
        mesh->SetMaterial(materials[reader->UInt()]);

        UINT vertexCount = reader->UInt();
        ModelVertex* vertices = new ModelVertex[vertexCount];
        reader->Byte((void**)&vertices, sizeof(ModelVertex) * vertexCount);
        vecvertex.push_back(vertices);
        UINT indexCount = reader->UInt();
        UINT* indices = new UINT[indexCount];
        reader->Byte((void**)&indices, sizeof(UINT) * indexCount);
        vecuint.push_back(indices);
        mesh->CreateMesh(vertices, vertexCount, indices, indexCount);

        meshes.push_back(mesh);
    }

    size = reader->UInt();
    nodes.resize(size);
    for (NodeData& node : nodes)
    {
        node.index = reader->Int();
        node.name = reader->String();
        node.parent = reader->Int();
        node.transform = reader->Matrix();
    }

    size = reader->UInt();
    bones.resize(size);
    for (BoneData& bone : bones)
    {
        bone.index = reader->Int();
        bone.name = reader->String();
        bone.offset = reader->Matrix();

        boneMap[bone.name] = bone.index;
    }

    delete reader;
}
