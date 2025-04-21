#include "FileChangeChecker.h"

FileChangeChecker::FileChangeChecker(const std::string & filePath)
	: filePath(filePath)
	, lastModifiedTime(std::filesystem::last_write_time(filePath)) { }

void FileChangeChecker::setOnFileChangeCallback(const std::function<void()> & callback) {
	onFileChangeCallback = callback;
}

bool FileChangeChecker::hasFileChanged() {
	auto currentModifiedTime = std::filesystem::last_write_time(filePath);
	if (currentModifiedTime != lastModifiedTime) {
		lastModifiedTime = currentModifiedTime;

		// Notify via callback if set
		if (onFileChangeCallback) {
			onFileChangeCallback();
		}

		return true;
	}
	return false;
}
