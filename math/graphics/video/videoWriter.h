#pragma once
#include "math/graphics/video/VideoSerializer.h"

struct videoWriter : public VideoSerializer {
	videoWriter(cveci2& size, cfp& fps, const stdPath outputFilePath) :VideoSerializer(outputFilePath) {
		this->size = size;
		this->fps = fps;

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
		//flip upside down and convert to bgr
		for (color* nativePtr = newFrame.begin(); nativePtr < newFrame.end(); ) {
			filePtr -= size.x;
			color* nativeEndPtr = nativePtr + size.x;
			std::transform(nativePtr, nativeEndPtr, filePtr, [](color in) { return colorRGB(in.b(), in.g(), in.r()); });
			nativePtr = nativeEndPtr;
		}
		

		// Write raw frame to ffmpeg’s stdin
		fwrite((byte*)frame.baseArray, 1, (byte*)frame.end() - (byte*)frame.begin(), pipe);
	}
};