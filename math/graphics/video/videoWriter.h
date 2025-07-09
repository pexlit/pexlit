#include <cstdio>
#include <vector>
#include <cstdint>
// includes for setmode
#include <fcntl.h>
#include <io.h>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

struct videoWriter {
	veci2 size;
	fp fps;
	stdPath outputFilePath;
	FILE* pipe;
	videoWriter(cveci2& size, cfp& fps, const stdPath outputFilePath) :size(size), fps(fps), outputFilePath(outputFilePath) {
		if (size.x & 1 || size.y & 1)
			throw "size not divisable by 2";
		std::string command = std::format("ffmpeg -f rawvideo -pix_fmt rgb24 -video_size {}x{} -r {} -i - -vcodec libx264 -qp 18 -x264opts opencl -pix_fmt yuv420p -r 60 \"{}\"",
			size.x, size.y, fps,
			outputFilePath.string()
		);
		pipe = popen(command.c_str(), "wb");
		if (!pipe) {
			std::perror("popen");
		}
		_setmode(2, _O_BINARY);
	}

	void addFrame(const texture& newFrame) const {
		if (newFrame.size != size)
			throw "wrong dimensions";
		array2d<colorRGB> frame{ newFrame.size, false };
		colorRGB* destPtr = frame.end();
		for (color* sourcePtr = newFrame.begin(); sourcePtr < newFrame.end(); ) {
			destPtr -= size.x;
			color* sourceEndPtr = sourcePtr + size.x;
			std::transform(sourcePtr, sourceEndPtr, destPtr, [](color in) { return colorRGB(in.b(), in.g(), in.r()); });
			sourcePtr = sourceEndPtr;
		}
		

		// Write raw frame to ffmpeg’s stdin
		fwrite((byte*)frame.baseArray, 1, (byte*)frame.end() - (byte*)frame.begin(), pipe);
	}
	~videoWriter() {

		// Flush and close
		fflush(pipe);
		pclose(pipe);
	}
};