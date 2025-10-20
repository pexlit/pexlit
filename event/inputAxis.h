#include <globalFunctions.h>
struct AxisData {
	int axis;
	fp value;
};

struct InputProcessor {

	std::unordered_map<vk, AxisData> axes;
	std::vector<fp> axisValues;

	//pressed: false -> released
	void processKey(cvk& key, cbool& pressed) {
		auto it = axes.find(key);
		//if pressed, 1, else -1
		cfp& multiplier = ((fp)pressed) * (fp)2 - (fp)1;
		if (it != axes.end()) {
			axisValues[it->second.axis] += it->second.value * multiplier;
		}
	}
	InputProcessor(std::vector<std::vector<vk>> axes = std::vector<std::vector<vk>>{
		{
			//neg, pos
			//-x, +x
			{vk::A, vk::D},
			//-y, +y
		{vk::S, vk::W},
		//-z, +z
	{vk::Q, vk::E}
	}
		}) {
		int axisIndex = 0;
		for (auto axis : axes) {
			this->axes[axis[0]] = AxisData(axisIndex, -1);
			this->axes[axis[1]] = AxisData(axisIndex, 1);
			axisIndex++;
		}
		axisValues.resize(axes.size());
	}
};