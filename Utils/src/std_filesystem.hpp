#pragma once

#include "utils/filesystem.hpp"

namespace ze::utils::filesystem {
	class StdFileSystem final : public FileSystem {
	public:
		StdFileSystem(Path external_dir = std::filesystem::current_path(),
			Path temp_dir = std::filesystem::temp_directory_path())
			: external_storage_directory_(std::move(external_dir)),
			temp_directory_(std::move(temp_dir)) {
		}
		virtual ~StdFileSystem() = default;

		FileStat stat_of_file(const Path& path) override;
		bool is_file(const Path& path) override;
		bool is_directory(const Path& path) override;
		bool exists(const Path& path) override;
		bool create_directory(const Path& path) override;
		std::vector<uint8_t> read_chunk(const Path& path, size_t offset, size_t count) override;
		void write_file(const Path& path, const std::vector<uint8_t>& data) override;
		void remove(const Path& path) override;
		void set_external_storage_directory(const std::string& dir) override;
		const Path& external_storage_directory() const override;
		const Path& temp_directory() const override;
	private:
		Path external_storage_directory_;
		Path temp_directory_;
	};
} // ze::utils::filesystem