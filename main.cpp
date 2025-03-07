
#include <Windows.h>

#include "memory.h"
#include "offsets.h"

enum {
    fl_ground = 129,
    fl_ground_duck = 131,
    fl_water = 641,
    fl_water_duck = 643
};

int main()
{
    auto mem = memory("left4dead2.exe");

    const auto client_dll = mem.get_module_base_address("client.dll");

    while (true) {

        const auto player = mem.read_process_memory<std::uintptr_t>(client_dll + offset::player_offset);
        const unsigned int flags = mem.read_process_memory<std::uint32_t>(player + offset::flags_offset);
        const auto force_jump = client_dll + offset::jump_offset;

        if (GetAsyncKeyState(VK_SPACE))
            (flags == fl_ground || flags == fl_ground_duck || flags == fl_water || flags == fl_water_duck) ? mem.write_process_memory<int>(force_jump, 5) : mem.write_process_memory<int>(force_jump, 4);

    }
}

