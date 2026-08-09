#include <iostream>
#include <DX3D/All.hpp>

int main() {

	try {
		dx3d::Game game({ {1280, 720}, dx3d::Logger::LogLevel::Info });
		game.Run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Runtime Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Invalid Argument Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...) {
		std::cerr << "Unknown exception occurred." << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
