#include <DX3D/All.hpp>

int main(const int argc, const char* args[]) {

	dx3d::Game game({ {1280, 720}, dx3d::Logger::LogLevel::Info } );
	game.Run();

	return 0;
}
