#pragma once

struct FileDescriptor {
private:
	int fileDescriptor;

public:
	int get() { return this->fileDescriptor; }
	void set(int fd) { this->fileDescriptor = fd; }
};
