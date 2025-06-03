#include "utils/filesystem.hpp"

using namespace ze::utils;

int main() {
	filesystem::init();

	auto fs = filesystem::get();
	// windows default directory: C:\Users\{your_user_name}\AppData\Local\Temp
	const auto test_dir = fs->temp_directory() / "test_directory" / "test_utils";
	fs->create_directory(test_dir);
	fs->exists(test_dir);
	fs->is_directory(test_dir);


}