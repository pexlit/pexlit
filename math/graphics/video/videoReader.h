#pragma once
#include "math/graphics/video/VideoSerializer.h"
#include <array/wstringFunctions.h>
struct videoReader : public VideoSerializer {
	size_t frameCount;
	videoReader(const stdPath& inputFilePath) : VideoSerializer(inputFilePath) {
		//first, get essential video info
		std::string infoCommand = std::format("ffprobe -show_format -show_streams -count_frames -i \"{}\" -o -",
			inputFilePath.string()
		);
		pipe = popen(infoCommand.c_str(), "rb");
		if (!pipe) {
			std::perror("popen");
		}

		std::string info = "";
		constexpr size_t bufferSize = 0x100;
		char buffer[bufferSize];
		size_t batchSize;
		do {
			batchSize = fread(buffer, 1, bufferSize, pipe);
			info += std::string(buffer, batchSize);

		} while (batchSize == bufferSize);
		//now read lines and get essential data
		auto lines = split_string(info, std::string("\n"));
		for (std::string line : lines) {
			auto parts = split_string(line, std::string("="));
			std::string member = parts[0];
			if (member == "width") {
				size.x = std::stoi(parts[1]);
			}
			else if (member == "height") {
				size.y = std::stoi(parts[1]);
			}
			else if (member == "r_frame_rate") {
				auto divisionParts = split_string(parts[1], std::string("/"));
				fps = (fp)(std::stod(divisionParts[0]) / std::stod(divisionParts[1]));
			}
			else if (member == "nb_read_frames") {
				frameCount = std::stoll(parts[1]);
			}
			else if (member == "[/STREAM]") {
				//pick the first stream and stop reading once the end tag has been reached
				break;
			}
		}
		fflush(pipe);
		pclose(pipe);

		//now open the actual stream
		//we specify bgra but we can specify any channel in any order

		std::string streamCommand = std::format("ffmpeg -i \"{}\" -f image2pipe -pix_fmt bgra -vcodec rawvideo -",
			inputFilePath.string()
		);
		pipe = popen(streamCommand.c_str(), "rb");

		if (!pipe) {
			std::perror("popen");
		}
		_setmode(2, _O_BINARY);


	}
	//gets the next frame from the stream
	bool getFrame(texture& buffer) const {
		if (buffer.size != size) {
			//resize
			buffer = texture(size, false);
		}
		size_t desiredCount = size.x * size.y * sizeof(color);
		size_t count = fread(buffer.baseArray, 1, desiredCount, pipe);
		if (count != desiredCount)
			return false;
		buffer.Flip();
		return true;
	}
};