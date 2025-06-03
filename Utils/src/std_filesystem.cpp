#include "std_filesystem.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

namespace ze::utils::filesystem {

    FileStat StdFileSystem::stat_of_file(const Path& path) {
        std::error_code ec;

        auto fs_stat = std::filesystem::status(path, ec);
        if (ec) {
            return FileStat{
                false,
                false,
                0,
            };
        }

        auto size = std::filesystem::file_size(path, ec);
        if (ec) {
            size = 0;
        }

        return FileStat{
            fs_stat.type() == std::filesystem::file_type::regular,
            fs_stat.type() == std::filesystem::file_type::directory,
            size,
        };
    }

    bool StdFileSystem::is_file(const Path& path) {
        auto stat = stat_of_file(path);
        return stat.is_file;
    }

    bool StdFileSystem::is_directory(const Path& path) {
        auto stat = stat_of_file(path);
        return stat.is_directory;
    }

    bool StdFileSystem::exists(const Path& path) {
        auto stat = stat_of_file(path);
        return stat.is_directory || stat.is_file;
    }

    bool StdFileSystem::create_directory(const Path& path) {

        std::error_code ec;
        std::filesystem::create_directories(path, ec);
        if (ec) {
            throw std::runtime_error("failed to create directory: " + path.string());
        }
        return !ec;
    }

    std::vector<uint8_t> StdFileSystem::read_chunk(const Path& path, size_t offset, size_t count) {
        
        std::ifstream file{ path, std::ios::binary | std::ios::ate };

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + path.string());
        }

        auto size = stat_of_file(path).size;

        if (offset + count > size) {
            return {};
        }
        // read file binaries
        file.seekg(offset, std::ios::beg);
        std::vector<uint8_t> data(count);
        file.read(reinterpret_cast<char*>(data.data()), count);
        return data;
    }

    void StdFileSystem::write_file(const Path& path, const std::vector<uint8_t>& data) {
        auto parent_path = path.parent_path();
        if (!std::filesystem::exists(parent_path)) {
            create_directory(parent_path);
        }

        std::ofstream file{ path, std::ios::binary | std::ios::trunc };

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file to write: " + path.string());
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    void StdFileSystem::remove(const Path& path) {

        std::error_code ec;

        std::filesystem::remove_all(path, ec);

        if (ec) {
            throw std::runtime_error("failed to remove file: " + path.string());
        }
    }

    void StdFileSystem::set_external_storage_directory(const std::string& dir) {
        external_storage_directory_ = dir;
    }

    const Path& StdFileSystem::external_storage_directory() const {
        return external_storage_directory_;
    }

    const Path& StdFileSystem::temp_directory() const {
        return temp_directory_;
    }

} // namespace ze::utils::filesystem
