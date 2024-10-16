//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	

	//const float fov{ (camera.fovAngle)*PI/180};
	const float fovScale{ tan(camera.fovAngle * TO_RADIANS / 2) };

	float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			float xNdc{ (2.f * (px + 0.5f) / m_Width - 1) * aspectRatio * fovScale };
			float yNdc{ (1.f - 2 * ((py + 0.5f) / m_Height)) * fovScale };
			Vector3 rayDirection{ xNdc, yNdc ,1.f };
			rayDirection.Normalize();
			rayDirection = camera.CalculateCameraToWorld().TransformVector(rayDirection);

			

			Ray vieuwRay{ camera.origin, rayDirection };
			/*float gradient = px / static_cast<float>(m_Width);
			gradient += py / static_cast<float>(m_Width);
			gradient /= 2.0f;*/

			ColorRGB finalColor{};

			HitRecord closestHit{};
			pScene->GetClosestHit(vieuwRay, closestHit);
			//Update Color in Buffer
			if (closestHit.didHit)
			{
				
					for (int idx{}; idx < lights.size(); ++idx)
					{
						
						
						Ray shadowRay{ closestHit.origin + 0.01f * closestHit.normal,LightUtils::GetDirectionToLight(lights[idx], closestHit.origin).Normalized()};
						//shadowRay.direction.Normalize();
						shadowRay.max = LightUtils::GetDirectionToLight(lights[idx], closestHit.origin).Magnitude();
						float observedArea{ Vector3::Dot(closestHit.normal, shadowRay.direction) };
						float shadowValue{};
						if (m_ShadowsEnabled && pScene->DoesHit(shadowRay)) shadowValue = 0.5f;
						else shadowValue = 1.0f;
						
						if (observedArea > 0)
						{
							
								if (m_CurrentLightingMode == LightingMode::ObservedArea)
								{

									finalColor += ColorRGB(observedArea, observedArea, observedArea)* shadowValue;

								}
								if (m_CurrentLightingMode == LightingMode::Radiance)
								{
									finalColor += LightUtils::GetRadiance(lights[idx], closestHit.origin) * shadowValue;
								}
								if (m_CurrentLightingMode == LightingMode::Combined)
								{
									
									finalColor += materials[closestHit.materialIndex]->Shade(closestHit, shadowRay.direction, vieuwRay.direction)
										* observedArea 
										* LightUtils::GetRadiance(lights[idx], closestHit.origin)
										* shadowValue;

								}
								if (m_CurrentLightingMode == LightingMode::BRDF)
								{
									finalColor += materials[closestHit.materialIndex]->Shade(closestHit, shadowRay.direction, vieuwRay.direction) * shadowValue;

								}
								//else if ()
								/*if (m_ShadowsEnabled)
								{
									if (pScene->DoesHit(shadowRay))
									{
										finalColor *= 0.5f;
									}
								}*/
							
						}
						
					}
				

				

			}
			

			finalColor.MaxToOne();


			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void dae::Renderer::Update()
{
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	if (pKeyboardState[SDL_SCANCODE_F2])
	{
		ToggleShadows();
	}
	if (pKeyboardState[SDL_SCANCODE_F3])
	{
		CycleLightingMode();
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	if (m_CurrentLightingMode == LightingMode::Combined)
	{
		m_CurrentLightingMode = LightingMode::ObservedArea;
	}
	else
	{
		m_CurrentLightingMode = static_cast<LightingMode>(static_cast<int>(m_CurrentLightingMode) + 1);
	}
}
