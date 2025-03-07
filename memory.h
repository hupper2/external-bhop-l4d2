#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstring> 
#include <vector>


class memory
{
private:
	std::uintptr_t process_id = 0;
	HANDLE process_handle = nullptr;

public:
	memory(const char* process_name) noexcept
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot != INVALID_HANDLE_VALUE) {
			PROCESSENTRY32 process_entry;
			process_entry.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(snapshot, &process_entry)) {
				do {
					if (_stricmp(process_entry.szExeFile, process_name) == 0) {
						process_id = process_entry.th32ProcessID;
						process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
						break;
					}
				} while (Process32Next(snapshot, &process_entry));
			}
			CloseHandle(snapshot);
		}

	}

	~memory()
	{
		if (process_handle)
			CloseHandle(process_handle);
	}

	const std::uintptr_t get_module_base_address(const char* module_name) const noexcept
	{
		uintptr_t module_base_address = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
		if (snapshot != INVALID_HANDLE_VALUE) {
			MODULEENTRY32 moduleEntry;
			moduleEntry.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(snapshot, &moduleEntry)) {
				do {
					if (_stricmp(moduleEntry.szModule, module_name) == 0) {
						module_base_address = (uintptr_t)moduleEntry.modBaseAddr;
						break;
					}
				} while (Module32Next(snapshot, &moduleEntry));
			}
			CloseHandle(snapshot);
		}
		return module_base_address;

	}

	template <typename T>
	constexpr const T read_process_memory(const std::uintptr_t& address) const noexcept
	{
		T value = { };
		ReadProcessMemory(process_handle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr void write_process_memory(const std::uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(process_handle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}


};
