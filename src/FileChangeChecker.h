#ifndef FILE_CHANGE_CHECKER_H
#define FILE_CHANGE_CHECKER_H

#include <filesystem>
#include <functional>
#include <string>

class FileChangeChecker {
public:
	explicit FileChangeChecker(const std::string & filePath);

	// Set the callback function
	void setOnFileChangeCallback(const std::function<void()> & callback);

	bool hasFileChanged();

private:
	std::string filePath;
	std::filesystem::file_time_type lastModifiedTime;
	std::function<void()> onFileChangeCallback; // Callback function
};

#endif // FILE_CHANGE_CHECKER_H
