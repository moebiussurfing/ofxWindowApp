#ifndef FILE_CHANGE_CHECKER_H
#define FILE_CHANGE_CHECKER_H

#include <filesystem>
#include <string>

class FileChangeChecker {
public:
	explicit FileChangeChecker(const std::string & filePath);
	bool hasFileChanged();

private:
	std::string filePath;
	std::filesystem::file_time_type lastModifiedTime;
	std::filesystem::file_time_type getLastModifiedTime() const;
};

#endif // FILE_CHANGE_CHECKER_H
