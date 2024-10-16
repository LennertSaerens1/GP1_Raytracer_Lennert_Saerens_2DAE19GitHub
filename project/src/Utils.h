#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//ray.direction.Normalize();
			float A = Vector3::Dot(ray.direction, ray.direction);
			float B = Vector3::Dot(2 * ray.direction, ray.origin - sphere.origin);
			float C = Vector3::Dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - sphere.radius * sphere.radius;
			float Discriminant{ (B * B - 4 * A * C) }; //Calculating the discriminant with calculated A,B and C

			if (Discriminant <= 0) // if dicriminant is greater or equal there is no intersection (see theory)
			{
				hitRecord.didHit = false;
			}
			else
			{
				float t{ (-B - sqrt(Discriminant)) / 2 * A };
				if (t < ray.min) t = (-B + sqrt(Discriminant)) / 2 * A; // we always take the subtraction unless t < tMin
				if (t > ray.min && t < ray.max)
				{

					hitRecord.didHit = true;
					hitRecord.t = t;
					hitRecord.origin = ray.origin + t * ray.direction;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.normal = (hitRecord.origin - sphere.origin) / sphere.radius;
				}


			}

			//throw std::runtime_error("Not Implemented Yet");
			return hitRecord.didHit;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//ray.direction.Normalize();
			float t = Vector3::Dot(plane.origin - ray.origin, plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			//if (Vector3::Dot(ray.direction, plane.normal) != 0)
			{
				if (t > ray.min && t < ray.max)
				{
					hitRecord.didHit = true;
					hitRecord.normal = plane.normal;
					hitRecord.t = t;
					//hitRecord.origin = ray.origin + (Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3{}.Dot(ray.direction, plane.normal)) * ray.direction;
					//hitRecord.origin = plane.origin;
					hitRecord.materialIndex = plane.materialIndex;
					hitRecord.origin = ray.origin + t * ray.direction;
				}
				else
				{
					hitRecord.didHit = false;

				}
			}
			//else hitRecord.didHit = false;
			//throw std::runtime_error("Not Implemented Yet");
			return hitRecord.didHit;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//throw std::runtime_error("Not Implemented Yet");
			float dot{ Vector3::Dot(triangle.normal, ray.direction) };
			Vector3 a{ triangle.v1 - triangle.v0 };
			Vector3 b{ triangle.v2 - triangle.v0 };
			//Vector3 normal = Vector3::Cross(a, b);
			if (Vector3::Dot(triangle.normal, ray.direction) == 0) return false;

			Vector3 l{ triangle.v0 - ray.origin };
			float t{ Vector3::Dot(l,triangle.normal)/ Vector3::Dot(ray.direction,triangle.normal) };
			if (t < ray.min || t > ray.max)
				return false;

			Vector3 P = ray.origin + ray.direction * t;

			switch (triangle.cullMode)
			{
			case TriangleCullMode::BackFaceCulling:
				if (dot > 0 && ignoreHitRecord == false) return false;
				if (dot < 0 && ignoreHitRecord == true) return false;
				break;
			case TriangleCullMode::FrontFaceCulling:
				if (dot < 0 && ignoreHitRecord == false) return false;
				if (dot > 0 && ignoreHitRecord == true) return false;
				break;
			case TriangleCullMode::NoCulling:
				break;
			}

			Vector3 e0{ triangle.v1 - triangle.v0 };
			Vector3 e1{ triangle.v2 - triangle.v1 };
			Vector3 e2{ triangle.v0 - triangle.v2 };
			Vector3 p0{ P - triangle.v0 };
			Vector3 p1{ P - triangle.v1 };
			Vector3 p2{ P - triangle.v2 };

			if (Vector3::Dot(Vector3::Cross(e0, p0), triangle.normal) < 0) return false;
			if (Vector3::Dot(Vector3::Cross(e1, p1), triangle.normal) < 0) return false;
			if (Vector3::Dot(Vector3::Cross(e2, p2), triangle.normal) < 0) return false;
			hitRecord.didHit = true;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.origin = P;
			hitRecord.normal = triangle.normal;
			hitRecord.t = t;
			return true;
			
			/*if (!ignoreHitRecord)
			{
				bool toCheck{ (triangle.cullMode == TriangleCullMode::BackFaceCulling && dot < 0.f) || (triangle.cullMode == TriangleCullMode::FrontFaceCulling && dot > 0.f) || (triangle.cullMode == TriangleCullMode::NoCulling) };*/

				/*if (toCheck)
				{*/
					
			/*	}
				
			}*/
			/*else
			{
				bool toCheck{ (triangle.cullMode == TriangleCullMode::BackFaceCulling && dot > 0.f) || (triangle.cullMode == TriangleCullMode::FrontFaceCulling && dot < 0.f) || (triangle.cullMode == TriangleCullMode::NoCulling) };

				if (toCheck)
				{

					Vector3 e0{ triangle.v1 - triangle.v0 };
					Vector3 e1{ triangle.v2 - triangle.v1 };
					Vector3 e2{ triangle.v0 - triangle.v2 };
					Vector3 p0{ P - triangle.v0 };
					Vector3 p1{ P - triangle.v1 };
					Vector3 p2{ P - triangle.v2 };

					if (Vector3::Dot(Vector3::Cross(e0, p0), triangle.normal) < 0) return false;
					if (Vector3::Dot(Vector3::Cross(e1, p1), triangle.normal) < 0) return false;
					if (Vector3::Dot(Vector3::Cross(e2, p2), triangle.normal) < 0) return false;
					hitRecord.didHit = true;
					hitRecord.materialIndex = triangle.materialIndex;
					hitRecord.origin = P;
					hitRecord.normal = triangle.normal;
					hitRecord.t = t;
					return true;

				}
			}*/
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			for (int idx{}; idx < mesh.indices.size(); idx += 3)
			{
				Triangle tempTriangle(mesh.transformedPositions[mesh.indices[idx]],
					mesh.transformedPositions[mesh.indices[idx + 1]],
					mesh.transformedPositions[mesh.indices[idx + 2]]);
				tempTriangle.normal = mesh.transformedNormals[idx/3];
				tempTriangle.cullMode = mesh.cullMode;
				tempTriangle.materialIndex = mesh.materialIndex;
				if (HitTest_Triangle(tempTriangle, ray, hitRecord, ignoreHitRecord)) return true;
			}
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			Vector3 shadowRay{light.origin - origin};
			//throw std::runtime_error("Not Implemented Yet");
			return {shadowRay};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			float bottom{ Vector3::Dot(light.origin - target, light.origin - target) };
			ColorRGB color{ light.color * (light.intensity / (bottom)) };
			return { color };
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}