#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

typedef DWORD(WINAPI* PFNPowerGetActiveScheme)(HKEY, GUID**);
typedef DWORD(WINAPI* PFNPowerSetActiveScheme)(HKEY, CONST GUID*);
typedef DWORD(WINAPI* PFNPowerWriteACValueIndex)(HKEY, CONST GUID*, CONST GUID*, CONST GUID*, DWORD);
typedef DWORD(WINAPI* PFNPowerReadACValue)(HKEY, CONST GUID*, CONST GUID*, CONST GUID*, PULONG, LPBYTE, LPDWORD);

struct {
  HMODULE Library;
  PFNPowerGetActiveScheme GetActiveScheme;
  PFNPowerSetActiveScheme SetActiveScheme;
  PFNPowerWriteACValueIndex WriteACValueIndex;
  PFNPowerReadACValue ReadACValue;
} PowrProf;

static int IsElevated() {
  HANDLE token = NULL;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
    MessageBoxW(NULL, L"Failed to open process token.", L"Error", MB_ICONERROR);
    ExitProcess(1);
  }
  TOKEN_ELEVATION elevation;
  DWORD dwSize;
  if (!GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
    CloseHandle(token);
    MessageBoxW(NULL, L"Failed to get token information.", L"Error", MB_ICONERROR);
    ExitProcess(2);
  }
  CloseHandle(token);
  return elevation.TokenIsElevated;
}

static void LoadPowrProf() {
  PowrProf.Library = LoadLibraryW(L"PowrProf.dll");
  if (PowrProf.Library) {
    PowrProf.GetActiveScheme = (PFNPowerGetActiveScheme)GetProcAddress(PowrProf.Library, "PowerGetActiveScheme");
    PowrProf.SetActiveScheme = (PFNPowerSetActiveScheme)GetProcAddress(PowrProf.Library, "PowerSetActiveScheme");
    PowrProf.WriteACValueIndex =
        (PFNPowerWriteACValueIndex)GetProcAddress(PowrProf.Library, "PowerWriteACValueIndex");
    PowrProf.ReadACValue = (PFNPowerReadACValue)GetProcAddress(PowrProf.Library, "PowerReadACValue");
    if (PowrProf.GetActiveScheme && PowrProf.SetActiveScheme && PowrProf.WriteACValueIndex &&
        PowrProf.ReadACValue) {
      return;
    }
  }

  MessageBoxW(NULL, L"Failed to load PowrProf.dll.", L"Error", MB_ICONERROR);
  ExitProcess(3);
}

static int IsDisabledAlready(const GUID* active_scheme_guid) {
  DWORD idle_disable;  // confirmed to be REG_DWORD
  DWORD buffer_size = sizeof(idle_disable);
  if (PowrProf.ReadACValue(NULL, active_scheme_guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                                &GUID_PROCESSOR_IDLE_DISABLE, NULL, (LPBYTE)&idle_disable,
                                &buffer_size) != ERROR_SUCCESS) {
    // If we cannot get status, then assume it is not disabled so we can disable it later.
    return 0;
  }

  return idle_disable == 0;
}

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance, _In_ LPWSTR cmdline, _In_ int cmdshow) {
  LoadPowrProf();

  if (!IsElevated()) {
    MessageBoxW(NULL, L"Please run this program as an administrator.", L"Error", MB_ICONERROR);
    return 4;
  }

  GUID* active_scheme_guid = NULL;
  if (PowrProf.GetActiveScheme(NULL, &active_scheme_guid) != ERROR_SUCCESS) {
    MessageBoxW(NULL, L"Failed to get the active power scheme.", L"Error", MB_ICONERROR);
    return 5;
  }

  if (IsDisabledAlready(active_scheme_guid)) {
    MessageBoxW(NULL, L"Processor idle is already disabled.", L"Information", MB_ICONINFORMATION);
    return 0;
  } else {
    // PowerCfg /SETACVALUEINDEX SCHEME_CURRENT SUB_PROCESSOR IDLEDISABLE 000
    if (PowrProf.WriteACValueIndex(NULL, active_scheme_guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                                        &GUID_PROCESSOR_IDLE_DISABLE, 0) != ERROR_SUCCESS) {
      MessageBoxW(NULL, L"Failed to set the AC value index.", L"Error", MB_ICONERROR);
      return 6;
    }

    // PowerCfg /SETACTIVE SCHEME_CURRENT
    if (PowrProf.SetActiveScheme(NULL, active_scheme_guid) != ERROR_SUCCESS) {
      MessageBoxW(NULL, L"Failed to set the active power scheme.", L"Error", MB_ICONERROR);
      return 7;
    }

    MessageBoxW(NULL, L"Successfully disabled processor idle.", L"Success", MB_ICONINFORMATION);
  }

  LocalFree(active_scheme_guid);
  FreeLibrary(PowrProf.Library);

  return 0;
}