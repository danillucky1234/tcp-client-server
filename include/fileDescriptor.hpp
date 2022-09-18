#pragma once

struct FileDescriptor {
private:
	int fileDescriptor;

public:
	FileDescriptor() { this->fileDescriptor = -1; }
	FileDescriptor(int fd) { this->fileDescriptor = fd; }
	int get() const { return this->fileDescriptor; }
	void set(int fd) { this->fileDescriptor = fd; }
};
