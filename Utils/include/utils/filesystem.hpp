#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace ze::utils::filesystem {
	struct FileStat {
		bool is_file;
		bool is_directory;
		size_t size;
	};

	using Path = std::filesystem::path;

	class FileSystem {
	public:
		FileSystem() = default;
		virtual ~FileSystem() = default;

		virtual FileStat stat_of_file(const Path& path) = 0;
		virtual bool is_file(const Path& path) = 0;
		virtual bool is_directory(const Path& path) = 0;
		virtual bool exists(const Path& path) = 0;
		virtual bool create_directory(const Path& path) = 0;
		virtual std::vector<uint8_t> read_chunk(const Path& path, size_t offset, size_t count) = 0;
		virtual void write_file(const Path& path, const std::vector<uint8_t>& data) = 0;
		virtual void remove(const Path& path) = 0;
		virtual void set_external_storage_directory(const std::string& dir) = 0;
		virtual const Path& external_storage_directory() const = 0;
		virtual const Path& temp_directory() const = 0;

		void write_file(const Path& path, const std::string& data);
		std::string read_file_string(const Path& path);
		std::vector<uint8_t> read_file_binary(const Path& path);
	};

	using FileSystemPtr = std::shared_ptr<FileSystem>;

	void init();

	FileSystemPtr get();

} // namespace ze::utils::filesystem