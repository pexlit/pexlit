#pragma once
#include <cstdio>
#include <vector>
#include <cstdint>
#include <filesystem/filemanager.h>
// includes for setmode
#include <fcntl.h>
#include <io.h>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif
struct VideoSerializer {
	veci2 size = veci2();
	fp fps = 0;
	stdPath filePath;
	FILE* pipe = nullptr;
	VideoSerializer(const stdPath& filePath) : filePath(filePath) {

	}

	~VideoSerializer() {

		// Flush and close
		fflush(pipe);
		pclose(pipe);
	}
};