#include "Scene.h"
#include "Utils.h"
#include "Material.h"
#include "iostream"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord hitRecord{};
		if (m_PlaneGeometries.size() >= 1)
		{
			for (int idx{ 0 }; idx < m_PlaneGeometries.size(); ++idx)
			{
				GeometryUtils::HitTest_Plane(m_PlaneGeometries[idx], ray, hitRecord);
				if (hitRecord.t < closestHit.t && hitRecord.didHit) closestHit = hitRecord;
			}
		}

		if (m_SphereGeometries.size() >= 1)
		{
			/*GeometryUtils::HitTest_Sphere(m_SphereGeometries[0], ray, hitRecord);
			closestHit = hitRecord;*/
			for (int idx{ 0 }; idx < m_SphereGeometries.size(); ++idx)
			{
				GeometryUtils::HitTest_Sphere(m_SphereGeometries[idx], ray, hitRecord);
				if (hitRecord.t < closestHit.t && hitRecord.didHit) closestHit = hitRecord;
			}
		}

		//if (m_Triangles.size() >= 1)
		//{
		//	/*GeometryUtils::HitTest_Sphere(m_SphereGeometries[0], ray, hitRecord);
		//	closestHit = hitRecord;*/
		//	for (int idx{ 0 }; idx < m_Triangles.size(); ++idx)
		//	{
		//		GeometryUtils::HitTest_Triangle(m_Triangles[idx], ray, hitRecord);
		//		if (hitRecord.t < closestHit.t && hitRecord.didHit) closestHit = hitRecord;
		//	}
		//}
		
		if (m_TriangleMeshGeometries.size() >= 1)
		{
			for (int idx{ 0 }; idx < m_TriangleMeshGeometries.size(); ++idx)
			{
				GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[idx], ray, hitRecord);
				if (hitRecord.t < closestHit.t && hitRecord.didHit) closestHit = hitRecord;
			}
		}
		//throw std::runtime_error("Not Implemented Yet");
	}

	bool Scene::DoesHit(const Ray& ray) const
	{

		HitRecord hitRecord{};
		bool hit{ false };

		for (int idx{}; idx < m_PlaneGeometries.size(); ++idx)
		{
			if (GeometryUtils::HitTest_Plane(m_PlaneGeometries[idx], ray, hitRecord) && hitRecord.t > ray.min && hitRecord.t < ray.max) hit = true;
		}
		for (int idx{}; idx < m_SphereGeometries.size(); ++idx)
		{
			if (GeometryUtils::HitTest_Sphere(m_SphereGeometries[idx], ray, hitRecord) && hitRecord.t > ray.min && hitRecord.t < ray.max) hit = true;
		}
		/*for (int idx{}; idx < m_Triangles.size(); ++idx)	
		{
			if (GeometryUtils::HitTest_Triangle(m_Triangles[idx], ray, hitRecord, true) && hitRecord.t > ray.min && hitRecord.t < ray.max) hit = true;
		}*/
		for (int idx{}; idx < m_TriangleMeshGeometries.size(); ++idx)
		{
				if (GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[idx], ray, hitRecord, true) && hitRecord.t > ray.min && hitRecord.t < ray.max) hit = true;			
		}

		return hit;
		//if (m_PlaneGeometries.size() >= 1)
		//{
		//	for (int idx{ 0 }; idx < m_PlaneGeometries.size(); ++idx)
		//	{
		//		GeometryUtils::HitTest_Plane(m_PlaneGeometries[idx], normalisedRay, hitRecord);
		//		if (hitRecord.didHit && hitRecord.t < ray.direction.Magnitude() && hitRecord.t > 0.01f)
		//		{
		//			//std::cout << hitRecord.t << std::endl;
		//			return true;
		//		}
		//	}
		//	
		//}
		//if (m_SphereGeometries.size() >= 1)
		//{
		//	for (int idx{ 0 }; idx < m_SphereGeometries.size(); ++idx)
		//	{
		//		GeometryUtils::HitTest_Sphere(m_SphereGeometries[idx], normalisedRay, hitRecord);
		//		if (hitRecord.didHit && hitRecord.t < ray.direction.Magnitude() && hitRecord.t > 0.01f)
		//		{
		//			return true;
		//		}
		//	}
		//}
		//throw std::runtime_error("Not Implemented Yet");
		
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W2::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -1.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}
#pragma endregion
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));

		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));

		// Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		const auto matLambertPhong1 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 3.f));
		const auto matLambertPhong2 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 15.f));
		const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 50.f));

		//AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matLambertPhong1);
		//AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matLambertPhong2);
		//AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matLambertPhong3);

		// Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GraySmoothPlastic);

		// Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .8f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });
	}
	void Scene_W4::Initialize()
	{
		
		
			m_Camera.origin = { 0.f, 1.f, -5.f };
			m_Camera.fovAngle = 45.f;

			// Materials
			const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
			const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));
			//const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::White, 0.5f, 0.5f, 50.f));


			// Planes
			AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);  // BACK
			AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);    // BOTTOM
			AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);  // TOP
			AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);   // RIGHT
			AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);   // LEFT

			// Triangle (Temp)
			/*auto triangle = Triangle({ -0.75f, 0.5f, -0.f }, { -0.75f, 2.f, 0.f }, { 0.75f, 0.5f, 0.f });
			triangle.cullMode = TriangleCullMode::NoCulling;
			triangle.materialIndex = matLambert_White;
			m_Triangles.emplace_back(triangle);*/

			//Triangle Mesh
			pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
			Utils::ParseOBJ("Resources/simple_cube.obj",
				pMesh->positions,
				pMesh->normals,
				pMesh->indices);

			pMesh->Scale({ 0.7f, 0.7f, 0.7f });
			pMesh->Translate({ 0.f,1.5f, 0.f });

			pMesh->UpdateTransforms();

			// Lights
			AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });    // Backlight
			AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .8f, .45f });  // Front Light Left
			AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });  // Front Light Right
		
	}

	void Scene_W4::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);
		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();		
	}

	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = {0,3,-9 };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));

		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({.49f, 0.57f,0.57 },1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White,1.f));

		AddPlane(Vector3(0.f, 0.f, 10.f), Vector3(0.f, 0.f, -1.f), matLambert_GrayBlue); //BACK
		AddPlane(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3(0.f, 10.f, 0.f), Vector3(0.f, -1.f, 0.f), matLambert_GrayBlue); //TOP
		AddPlane(Vector3(5.f, 0.f, 0.f), Vector3(-1.f, 0.f, 0.f), matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3(-5.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f), matLambert_GrayBlue); //LEFT

		AddSphere(Vector3(-1.75f, 1.f, 0.f), .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3(0.f, 1.f, 0.f), .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3(1.75f, 1.f, 0.f), .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3(-1.75f, 3.f, 0.f), .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3(0.f, 3.f, 0.f), .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3(1.75f, 3.f, 0.f), .75f, matCT_GraySmoothPlastic);

		const Triangle baseTriangle = { Vector3{-0.75,1.5f,0.f}, Vector3{0.75f,0.f,0.f},Vector3{-0.75f,0.f,0.f} };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({-1.75f,4.5f,0.f});
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f,4.5f,0.f });
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f,4.5f,0.f });
		m_Meshes[2]->UpdateTransforms();

		AddPointLight(Vector3(0.f, 5.f, 5.f), 50.f, ColorRGB(1.f, 0.61f, 0.45f)); //Backlight
		AddPointLight(Vector3(-2.5f, 5.f, -5.f), 70.f, ColorRGB(1.f, 0.8f, 0.45f)); //Front Light Left
		AddPointLight(Vector3(2.5f, 2.5f, -5.f), 50.f, ColorRGB(0.34f, 0.47f, 0.68f));

	}


	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;

		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}
}
