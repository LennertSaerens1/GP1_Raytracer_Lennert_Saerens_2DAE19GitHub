#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

//#include <iostream>
#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}


		Vector3 origin{};
		float fovAngle{ 90.f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			right = (Vector3::Cross(Vector3::UnitY, forward).Normalized());
			up = (Vector3::Cross(forward, right).Normalized());
			Vector4 row1{ right.x,right.y,right.z,0 };
			Vector4 row2{ up.x,up.y,up.z,0 };
			Vector4 row3{ forward.x,forward.y,forward.z,0 };
			Vector4 row4{ origin.x,origin.y,origin.z,1 };
			Matrix OBN{ row1,row2,row3,row4 };
			//throw std::runtime_error("Not Implemented Yet");
			return {OBN};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			float movementSpeed{ 10.f };
			if (pKeyboardState[ SDL_SCANCODE_W])
			{
				origin += forward*deltaTime* movementSpeed;
			}
			if (pKeyboardState[ SDL_SCANCODE_S])
			{
				origin -= forward * deltaTime * movementSpeed;
			}
			if (pKeyboardState[ SDL_SCANCODE_D])
			{
				origin += right * deltaTime * movementSpeed;
			}
			if (pKeyboardState[ SDL_SCANCODE_A])
			{
				origin -= right * deltaTime * movementSpeed;
			}

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const float sensitivityX = 0.1f; // Yaw sensitivity
			const float sensitivityY = 0.1f; // Pitch sensitivity

			//std::cout << mouseState << std::endl;
			if(mouseState == (SDL_BUTTON_LEFT | SDL_BUTTON_X1) || mouseState == (SDL_BUTTON_LEFT | SDL_BUTTON_RIGHT))//if ((SDL_BUTTON(mouseState) == SDL_BUTTON(SDL_BUTTON_LEFT)) && isRightPressed)
			{
				origin += up * mouseY * deltaTime;
			}
			else if (mouseState == SDL_BUTTON_LEFT)
			{
				origin += forward * mouseY * deltaTime;
			}
			else if ((mouseState == SDL_BUTTON_RIGHT)|| (mouseState== SDL_BUTTON_X1))
			{
				// Calculate yaw and pitch based on mouse movement
				float yawDelta = -mouseX * sensitivityX * deltaTime;
				float pitchDelta = -mouseY * sensitivityY * deltaTime;

				// Update total yaw and pitch
				totalYaw += yawDelta;
				totalPitch += pitchDelta;

				// Construct rotation matrices for yaw and pitch
				Matrix yawRotation = Matrix::CreateRotationY(totalYaw);
				Matrix pitchRotation = Matrix::CreateRotationX(totalPitch);

				// Combine rotations to get the new forward vector
				forward = (yawRotation * pitchRotation).TransformVector(Vector3::UnitZ);
				forward.Normalize();

				// Recalculate the right and up vectors based on the new forward vector
				right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
				up = Vector3::Cross(forward, right).Normalized();
			}
			

			// Recalculate the camera-to-world matrix
			cameraToWorld = CalculateCameraToWorld();
			//throw std::runtime_error("Not Implemented Yet");
		}
	};
}
