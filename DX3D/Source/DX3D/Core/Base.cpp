#include <DX3D/Core/Base.hpp>


dx3d::Base::Base(const BaseDesc& desc) : _logger(desc.logger) {}

dx3d::Base::~Base() {}

dx3d::Logger& dx3d::Base::GetLogger() const noexcept {
	return _logger;
}


dx3d::ClonableBase::ClonableBase(const BaseDesc& desc) : _logger(desc.logger) {}

dx3d::ClonableBase::~ClonableBase() {}

dx3d::Logger& dx3d::ClonableBase::GetLogger() const noexcept {
	return _logger;
}
