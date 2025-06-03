#include "utils/filesystem.hpp"

#include <cassert>

#include "std_filesystem.hpp"

namespace ze::utils::filesystem {

	static FileSystemPtr fs_ptr = nullptr;

	void init() {
		fs_ptr = std::make_shared<StdFileSystem>();
	}

	FileSystemPtr get() {
		assert(fs_ptr && "Filesystem not initialized!");
		return fs_ptr;
	}

	void FileSystem::write_file(const Path& path, const std::string& data) {
		write_file(path, std::vector<uint8_t>(data.begin(), data.end()));
	}

	std::string FileSystem::read_file_string(const Path& path) {
		auto binary = read_file_binary(path);
		return { binary.begin(), binary.end() };
	}

	std::vector<uint8_t> FileSystem::read_file_binary(const Path& path) {
		auto stat = stat_of_file(path);
		return read_chunk(path, 0, stat.size);
	}
} // namespace ze::utils::filesystem