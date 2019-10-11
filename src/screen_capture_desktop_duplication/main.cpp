#include "dxgi_adapters.h"
#include "dxgi_device.h"
#include "dxgi_output_duplication.h"
#include <fmt/printf.h>
#include <thread>

int main(int argc, const char* argv[])
{
	auto system_adapter = dxgi_system();
	const auto& adapters = system_adapter.get_adapters();

	// just as part of the experiment we just use the first adapter
	const auto adapter = adapters.front();

	auto device = dxgi_device(adapter.get_adapter());
	const auto& outputs = adapter.get_outputs();
	auto output = outputs.front();

	auto duplication = dxgi_output_duplication(device, output);
	duplication.duplicate_output();

	while(true)
	{
		const auto pFrame = duplication.next_frame(1000);
		const auto pData = (uint8_t*)pFrame->data();
		if (!pData)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			fmt::print("no data\n");
			duplication.release_frame();
			continue;;
		}
		assert(pData);

		for (int i = 0; i < 20; i++)
		{
			fmt::print("{:x} ", pData[i]);
		}

		fmt::print("\r");
		

		duplication.release_frame();
	}
	return 0;
}