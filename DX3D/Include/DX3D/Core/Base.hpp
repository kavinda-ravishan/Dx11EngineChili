#pragma once

#include <DX3D/Core/Common.hpp>

namespace dx3d {

	class Base {
	public:
		explicit Base(const BaseDesc& desc);
		virtual ~Base();

	protected:
		Base(const Base&) = delete;
		Base(Base&&) = delete;
		Base& operator = (const Base&) = delete;
		Base& operator = (Base&&) = delete;
	
	protected:
		Logger& _logger;
	};

} // namespace dx3d
