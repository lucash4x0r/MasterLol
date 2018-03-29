#define NOMINMAX
#include "utils.hpp"


HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

typedef HRESULT(__stdcall* fn_EndScene)(IDirect3DDevice* pdevice);
fn_EndScene oEndScene;

namespace Utils
{
	DWORD LolBase = 0;
	DWORD pLocalPlayer = 0;
	DWORD pCursor = 0;

	void Init()
	{
		LolBase = (DWORD)GetModuleHandle(NULL);
		pLocalPlayer = *(DWORD*)(Utils::LolBase + oLocalPlayer);
		pCursor = *(DWORD*)(Utils::LolBase + oCursor);
	}

	void Dump()
	{
		cout << "League of Legends base is : ";
		cout << showbase // show the 0x prefix
			<< internal // fill between the prefix and the number
			<< setfill('0'); // fill with 0s

		cout << hex << Utils::LolBase << dec << endl << endl; //Print LolBase address

		cObject* localPlayer = Entity::getLocalPlayer();
		cout << "===== LocalPlayer ===== : " << endl;
		cout << "Type : " << Entity::GetObjectType(localPlayer) << endl;
		cout << "Position : " << localPlayer->vPos.x << ", " << localPlayer->vPos.y << ", " << localPlayer->vPos.z << endl;
		cout << "Hp : " << localPlayer->currHp << "\t Max Hp : " << localPlayer->maxHp << endl << endl;

		Cursor* cursor = Utils::getCursor();
		cout << "===== Cursor : =====" << endl;
		cout << "Position : " << cursor->vPos.x << ", " << cursor->vPos.y << ", " << cursor->vPos.z << endl;
	}

	Cursor* getCursor()
	{
		return (Cursor*)(Utils::pCursor);
	}

	void MoveTo(MoveType moveType, cObject* target)
	{
		switch (moveType)
		{
		case MoveType::move :
			hooks::callIssueOrder((DWORD)(Utils::LolBase + fnIssueOrder), Entity::getLocalPlayer(), MoveType::move,
				&getCursor()->vPos, 0, 0, 0, 1);
			break;
		case MoveType::attack :
			hooks::callIssueOrder((DWORD)(Utils::LolBase + fnIssueOrder), Entity::getLocalPlayer(), MoveType::attack,
				&target->vPos, target, 0, 0, 1);
			break;
		default:
			break;
		}
	}

	bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
			if (*szMask == 'x' && *pData != *bMask)
				return false;

		return (*szMask) == NULL;
	}
	DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
	{
		for (DWORD i = 0; i < dwLen; i++)
			if (bCompare((BYTE*)(dwAddress + i), bMask, szMask))
				return (DWORD)(dwAddress + i);

		return 0;
	}

	HRESULT __stdcall Hooked_EndScene(IDirect3DDevice* pdevice)
	{
		return oEndScene(pdevice);
	}

	void Test()
	{
		DWORD EndSceneaddy = 0;
		DWORD* vtbl = 0;
		DWORD table = FindPattern((DWORD)GetModuleHandle(L"d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
		memcpy(&vtbl, (void*)(table + 2), 4);
		EndSceneaddy = vtbl[42];

		if (EndSceneaddy)
		{
			cout << endl << "EndSceneAddress : ";
			cout <<hex
				 << showbase // show the 0x prefix
				 << internal // fill between the prefix and the number
				 << setfill('0'); // fill with 0s
			cout << EndSceneaddy << endl;

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			// this will hook the function
			DetourAttach(&(LPVOID&)EndSceneaddy, &Hooked_EndScene);
			DetourTransactionCommit();
		}
	}

    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
    void AttachConsole()
    {
        _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
        _old_err = GetStdHandle(STD_ERROR_HANDLE);
        _old_in  = GetStdHandle(STD_INPUT_HANDLE);

        ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

        _out     = GetStdHandle(STD_OUTPUT_HANDLE);
        _err     = GetStdHandle(STD_ERROR_HANDLE);
        _in      = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleMode(_out,
            ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(_in,
            ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
            ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
    }

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole()
    {
        if(_out && _err && _in) {
            FreeConsole();

            if(_old_out)
                SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
            if(_old_err)
                SetStdHandle(STD_ERROR_HANDLE, _old_err);
            if(_old_in)
                SetStdHandle(STD_INPUT_HANDLE, _old_in);
        }
    }

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...)
    {
        if(!_out) 
            return false;

        char buf[1024];
        va_list va;

        va_start(va, fmt);
        _vsnprintf_s(buf, 1024, fmt, va);
        va_end(va);

        return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
    }

	bool NextLine()
	{
		return ConsolePrint("\n");
	}

    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey()
    {
        if(!_in)
            return false;

        auto key = char{ 0 };
        auto keysread = DWORD{ 0 };

        ReadConsoleA(_in, &key, 1, &keysread, nullptr);

        return key;
    }


    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules)
    {
        bool signaled[32] = { 0 };
        bool success = false;

        std::uint32_t totalSlept = 0;

        if(timeout == 0) {
            for(auto& mod : modules) {
                if(GetModuleHandleW(std::data(mod)) == NULL)
                    return WAIT_TIMEOUT;
            }
            return WAIT_OBJECT_0;
        }

        if(timeout < 0)
            timeout = INT32_MAX;

        while(true) {
            for(auto i = 0u; i < modules.size(); ++i) {
                auto& module = *(modules.begin() + i);
                if(!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
                    signaled[i] = true;

                    //
                    // Checks if all modules are signaled
                    //
                    bool done = true;
                    for(auto j = 0u; j < modules.size(); ++j) {
                        if(!signaled[j]) {
                            done = false;
                            break;
                        }
                    }
                    if(done) {
                        success = true;
                        goto exit;
                    }
                }
            }
            if(totalSlept > std::uint32_t(timeout)) {
                break;
            }
            Sleep(10);
            totalSlept += 10;
        }

    exit:
        return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }
}