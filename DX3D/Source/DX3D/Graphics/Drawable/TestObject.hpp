#pragma once
#include <random>
#include <DX3D/Math/Math.hpp>
#include <DX3D/Graphics/Drawable/DrawableBase.hpp>

namespace dx3d {

	template<class T>
	class TestObject : public DrawableBase<T>
	{
	public:
		TestObject(
			const DrawableDesc& desc, 
			std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist
		)
			:
			DrawableBase<T>(desc),
			_r(rdist(rng)),
			_droll(ddist(rng)),
			_dpitch(ddist(rng)),
			_dyaw(ddist(rng)),
			_dphi(odist(rng)),
			_dtheta(odist(rng)),
			_dchi(odist(rng)),
			_chi(adist(rng)),
			_theta(adist(rng)),
			_phi(adist(rng))
		{}

		virtual ~TestObject() = default;

		void Update(float dt) noexcept {
			_roll = wrap_angle(_roll + (_droll * dt));
			_pitch = wrap_angle(_pitch + (_dpitch * dt));
			_yaw = wrap_angle(_yaw + (_dyaw * dt));
			_theta = wrap_angle(_theta + (_dtheta * dt));
			_phi = wrap_angle(_phi + (_dphi * dt));
			_chi = wrap_angle(_chi + (_dchi * dt));
		}

		virtual DirectX::XMMATRIX GetTransformXM() const noexcept override {
			return DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
				DirectX::XMMatrixTranslation(_r, 0.0f, 0.0f) *
				DirectX::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi);
		}
	
	protected:
		// positional
		float _r;
		float _roll = 0.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;
		float _theta;
		float _phi;
		float _chi;
		// speed (delta/s)
		float _droll;
		float _dpitch;
		float _dyaw;
		float _dtheta;
		float _dphi;
		float _dchi;
	};

} // namespace dx3d
