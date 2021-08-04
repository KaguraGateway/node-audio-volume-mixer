#include "common.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <Endpointvolume.h>
#include <psapi.h>
#include <functional>


namespace nodeAudioVolumeMixer {
    void UnInitAll(IMMDevice *pmmDevice, IAudioEndpointVolume *pAudioEndPointVol, IAudioSessionManager2 *pAudioSessionManager2) {
        if(pAudioSessionManager2)
            pAudioSessionManager2->Release();
        if(pAudioEndPointVol)
            pAudioEndPointVol->Release();
        if(pmmDevice)
            pmmDevice->Release();
        CoUninitialize();
    }

    void UnInit(IMMDevice *pmmDevice, IAudioEndpointVolume *pAudioEndPointVol) {
        UnInitAll(pmmDevice, pAudioEndPointVol, NULL);
    }

    void UnInit(IMMDeviceEnumerator *pmmEnum, IMMDevice *pmmDevice) {
        if(pmmDevice)
            pmmDevice->Release();
        if(pmmEnum)
            pmmEnum->Release();
        CoUninitialize();
    }

    bool IsFailed(HRESULT *hr, IMMDevice *pmmDevice, IAudioEndpointVolume *pAudioEndpointVol, IAudioSessionManager2 *pAudioSessionManager2) {
        if(FAILED(*hr)) {
            UnInitAll(pmmDevice, pAudioEndpointVol, pAudioSessionManager2);
            return true;
        }
        return false;
    }

    bool IsFailedInit(HRESULT *hr, IMMDeviceEnumerator *pmmEnum, IMMDevice *pmmDevice) {
        if(FAILED(*hr)) {
            UnInit(pmmEnum, pmmDevice);
            return true;
        }
        return false;
    }

    bool IsFailedInitAudioEndpointVolume(HRESULT *hr, IMMDevice *pmmDevice, IAudioEndpointVolume *pAudioEndpointVol) {
        if(FAILED(*hr)) {
            UnInitAll(pmmDevice, pAudioEndpointVol, NULL);
            return true;
        }
        return false;
    }

    bool IsFailedInitAudioSessionManager(HRESULT *hr, IMMDevice *pmmDevice, IAudioSessionManager2 *pAudioSessionManager2) {
        if(FAILED(*hr)) {
            UnInitAll(pmmDevice, NULL, pAudioSessionManager2);
            return true;
        }
        return false;
    }

    void Init(IMMDevice **pmmDevice) {
        HRESULT hr;
        IMMDeviceEnumerator *pmmEnum = NULL;

        // Init
        CoInitialize(NULL);
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void **)&pmmEnum);
        // Failed Check
        if(IsFailedInit(&hr, pmmEnum, *pmmDevice)) {
            printf("CoCreateInstance FAILED.\n");
            throw std::runtime_error("CoCreateInstance FAILED.");
        }

        // Get Current Output Device.
        hr = pmmEnum->GetDefaultAudioEndpoint(eRender, eConsole, pmmDevice);
        // Failed Check
        if(IsFailedInit(&hr, pmmEnum, *pmmDevice)) {
            printf("GetDefaultAudioEndpoint FAILED.\n");
            throw std::runtime_error("GetDefaultAudioEndpoint FAILED.");
        }

        // pmmEnum UnInit
        pmmEnum->Release();
    }

    /**
     * Must: After call Init
     */
    void InitAudioEndpointVolume(IMMDevice **pmmDevice, IAudioEndpointVolume **pAudioEndpointVol) {
        HRESULT hr;

        // Create Audio Object
        hr = (*pmmDevice)->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)pAudioEndpointVol);
        // Failed Check
        if(IsFailedInitAudioEndpointVolume(&hr, *pmmDevice, *pAudioEndpointVol)) {
            printf("Activate FAILED.");
            throw std::runtime_error("Activate FAILED.");
        }
    }

    /**
     * Must: After call Init
     */
    void InitAudioSessionManager2(IMMDevice **pmmDevice, IAudioSessionManager2 **pAudioSessionManager2) {
        HRESULT hr;

        // Create Audio Object
        hr = (*pmmDevice)->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)pAudioSessionManager2);
        // Failed Check
        if(IsFailedInitAudioSessionManager(&hr, *pmmDevice, *pAudioSessionManager2)) {
            throw std::runtime_error("Activate FAILED.");
        }
    }



    bool IsMasterMuted() {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioEndpointVolume *pAudioEndpointVol = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioEndpointVolume(&pmmDevice, &pAudioEndpointVol);

        // is Muted?
        int32_t pMasterMuted = 0;
        hr = pAudioEndpointVol->GetMute(&pMasterMuted);
        // Failed Check
        if(IsFailed(&hr, pmmDevice, pAudioEndpointVol, NULL)) {
            printf("Get Mute Failed.\n");
            throw std::runtime_error("Get Mute Failed.");
        }

        UnInit(pmmDevice, pAudioEndpointVol);

        return pMasterMuted != 0;
    }

    /**
     * @param args[0] bool isMute
     */
    void MuteMaster(const bool mute) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioEndpointVolume *pAudioEndpointVol = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioEndpointVolume(&pmmDevice, &pAudioEndpointVol);

        // Mute
        hr = pAudioEndpointVol->SetMute(mute, NULL);
        if(IsFailed(&hr, pmmDevice, pAudioEndpointVol, NULL)) {
            throw std::runtime_error("Set Mute Failed");
        }

        UnInit(pmmDevice, pAudioEndpointVol);
    }

    float GetMasterVolumeLevelScalar() {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioEndpointVolume *pAudioEndpointVol = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioEndpointVolume(&pmmDevice, &pAudioEndpointVol);

        float volumeScale = 0;
        hr = pAudioEndpointVol->GetMasterVolumeLevelScalar(&volumeScale);
        if(IsFailed(&hr, pmmDevice, pAudioEndpointVol, NULL)) {
            throw std::runtime_error("Get VolumeScalar Failed");
        }

        UnInit(pmmDevice, pAudioEndpointVol);

        return volumeScale;
    }

    void SetMasterVolumeLevelScalar(const float volumeScale) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioEndpointVolume *pAudioEndpointVol = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioEndpointVolume(&pmmDevice, &pAudioEndpointVol);

        // Mute
        hr = pAudioEndpointVol->SetMasterVolumeLevelScalar(volumeScale, NULL);
        if(IsFailed(&hr, pmmDevice, pAudioEndpointVol, NULL)) {
            throw std::runtime_error("Set VolumeScalar Failed");
        }

        UnInit(pmmDevice, pAudioEndpointVol);
    }

    tstring GetProcessNameFromPid(DWORD pid) {
        tstring processName;
        HANDLE handleProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if(handleProcess) {
            TCHAR tempProcessName[MAX_PATH];
            GetModuleBaseName(handleProcess, NULL, tempProcessName, MAX_PATH);
            CloseHandle(handleProcess);
            processName = tempProcessName;
        }
        return processName;
    }

    void UnInitAll(IMMDevice *pmmDevice, IAudioSessionManager2 *pAudioSessionManager2, IAudioSessionEnumerator *pAudioSessionEnum) {
        if(pAudioSessionManager2)
            pAudioSessionManager2->Release();
        if(pAudioSessionEnum)
            pAudioSessionEnum->Release();
        if(pmmDevice)
            pmmDevice->Release();
        CoUninitialize();
    }

    void UnInitSession(IAudioSessionControl *session, IAudioSessionControl2 *session2) {
        if(session)
            session->Release();
        if(session2)
            session2->Release();
    }

    bool IsFailed(HRESULT *hr, IMMDevice *pmmDevice, IAudioSessionManager2 *pAudioSessionManager2, IAudioSessionEnumerator *pAudioSessionEnum) {
        if(FAILED(*hr)) {
            UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);
            return true;
        }
        return false;
    }

    bool IsFailedSession(HRESULT *hr, IAudioSessionControl *session, IAudioSessionControl2 *session2) {
        if(FAILED(*hr)) {
            UnInitSession(session, session2);
            return true;
        }
        return false;
    }

    void InitAudioSessionEnum(IMMDevice **pmmDevice, IAudioSessionManager2 **pAudioSessionManager2, IAudioSessionEnumerator **pAudioSessionEnum) {
        HRESULT hr;
        hr = (*pAudioSessionManager2)->GetSessionEnumerator(pAudioSessionEnum);
        if(IsFailed(&hr, *pmmDevice, *pAudioSessionManager2, *pAudioSessionEnum)) {
            throw new std::runtime_error("Get SessionEnumerator Failed.");
        }
    }

    void GetAudioSessionCount(IMMDevice **pmmDevice, IAudioSessionManager2 **pAudioSessionManager2, IAudioSessionEnumerator **pAudioSessionEnum, int32_t *cbSessionCount) {
        HRESULT hr;
        hr = (*pAudioSessionEnum)->GetCount(cbSessionCount);
        if(IsFailed(&hr, *pmmDevice, *pAudioSessionManager2, *pAudioSessionEnum)) {
            throw std::runtime_error("Get SessionEnum->GetCount");
        }
    }

    /**
     * @return Count AudioSession
     */
    int32_t ForEachAudioSession(const std::function<void(DWORD & pid, IAudioSessionControl & session, IAudioSessionControl2 & session2, int32_t & index)>& callback, IMMDevice **pmmDevice, IAudioSessionManager2 **pAudioSessionManager2, IAudioSessionEnumerator **pAudioSessionEnum) {
        HRESULT hr;

        // Get Count
        int32_t cbSessionCount = NULL;
        GetAudioSessionCount(pmmDevice, pAudioSessionManager2, pAudioSessionEnum, &cbSessionCount);

        IAudioSessionControl *session = NULL;
        IAudioSessionControl2 *session2 = NULL;
        DWORD pid = 0;

        for(int32_t index = 0; index < cbSessionCount; index++) {
            hr = (*pAudioSessionEnum)->GetSession(index, &session);
            if(IsFailedSession(&hr, session, session2)) {
                continue;
            }
            session->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&session2);
            if(IsFailedSession(&hr, session, session2)) {
                continue;
            }

            hr = session2->GetProcessId(&pid);
            if(IsFailedSession(&hr, session, session2)) {
                continue;
            }

            callback(pid, *session, *session2, index);

            UnInitSession(session, session2);
        }

        return cbSessionCount;
    }

    std::vector<Process> GetAudioSessionProcesses() {
        IMMDevice *pmmDevice = NULL;
        IAudioSessionManager2 *pAudioSessionManager2 = NULL;
        IAudioSessionEnumerator *pAudioSessionEnum = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioSessionManager2(&pmmDevice, &pAudioSessionManager2);
        InitAudioSessionEnum(&pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        std::vector<Process> arr;
        tstring processName;

        ForEachAudioSession([&processName, &arr](DWORD pid, IAudioSessionControl &session, IAudioSessionControl2 & session2, int32_t index){
            processName = GetProcessNameFromPid(pid);

            Process obj;
            obj.name = processName;
            obj.pid = pid;
            arr.push_back(obj);
        }, &pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);

        return arr;
    }

    /**
     * @param pid ProcessID
     */
    bool IsAudioSessionMuted(const int32_t pid) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioSessionManager2 *pAudioSessionManager2 = NULL;
        IAudioSessionEnumerator *pAudioSessionEnum = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioSessionManager2(&pmmDevice, &pAudioSessionManager2);
        InitAudioSessionEnum(&pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        BOOL isMuted = false;
        ISimpleAudioVolume *audioVolume = NULL;

        ForEachAudioSession([&pid, &isMuted, &audioVolume, &hr](DWORD pid2, IAudioSessionControl &session, IAudioSessionControl2 & session2, int32_t index){
            // Same ProcessId
            if(pid == pid2) {
                hr  = session.QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&audioVolume);
                if(FAILED(hr)) {
                    if(audioVolume)
                        audioVolume->Release();
                    return;
                }

                audioVolume->GetMute(&isMuted);
                audioVolume->Release();
            }
        }, &pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);

        return isMuted != 0;
    }

    /**
     * @param pid ProcessID
     */
    void SetAudioSessionMute(const int32_t pid, const bool mute) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioSessionManager2 *pAudioSessionManager2 = NULL;
        IAudioSessionEnumerator *pAudioSessionEnum = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioSessionManager2(&pmmDevice, &pAudioSessionManager2);
        InitAudioSessionEnum(&pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        ISimpleAudioVolume *audioVolume = NULL;

        ForEachAudioSession([&pid, &mute, &audioVolume, &hr](DWORD pid2, IAudioSessionControl &session, IAudioSessionControl2 & session2, int32_t index){
            // Same ProcessId
            if(pid == pid2) {
                hr  = session.QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&audioVolume);
                if(FAILED(hr)) {
                    if(audioVolume)
                        audioVolume->Release();
                    return;
                }

                audioVolume->SetMute(mute, NULL);
                audioVolume->Release();
            }
        }, &pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);
    }

    float GetAudioSessionVolumeLevelScalar(const int32_t pid) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioSessionManager2 *pAudioSessionManager2 = NULL;
        IAudioSessionEnumerator *pAudioSessionEnum = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioSessionManager2(&pmmDevice, &pAudioSessionManager2);
        InitAudioSessionEnum(&pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        float volumeScale = NULL;
        ISimpleAudioVolume *audioVolume = NULL;

        ForEachAudioSession([&pid, &volumeScale, &audioVolume, &hr](DWORD pid2, IAudioSessionControl &session, IAudioSessionControl2 & session2, int32_t index){
            // Same ProcessId
            if(pid == pid2) {
                hr  = session.QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&audioVolume);
                if(FAILED(hr)) {
                    if(audioVolume)
                        audioVolume->Release();
                    return;
                }

                audioVolume->GetMasterVolume(&volumeScale);
                audioVolume->Release();
            }
        }, &pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);

        return volumeScale;
    }

    void SetAudioSessionVolumeLevelScalar(const int32_t pid, float volumeScale) {
        HRESULT hr;
        IMMDevice *pmmDevice = NULL;
        IAudioSessionManager2 *pAudioSessionManager2 = NULL;
        IAudioSessionEnumerator *pAudioSessionEnum = NULL;

        // Init
        Init(&pmmDevice);
        InitAudioSessionManager2(&pmmDevice, &pAudioSessionManager2);
        InitAudioSessionEnum(&pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        ISimpleAudioVolume *audioVolume = NULL;

        ForEachAudioSession([&pid, &volumeScale, &audioVolume, &hr](DWORD pid2, IAudioSessionControl &session, IAudioSessionControl2 & session2, int32_t index){
            // Same ProcessId
            if(pid == pid2) {
                hr  = session.QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&audioVolume);
                if(FAILED(hr)) {
                    if(audioVolume)
                        audioVolume->Release();
                    return;
                }

                audioVolume->SetMasterVolume(volumeScale, NULL);
                audioVolume->Release();
            }
        }, &pmmDevice, &pAudioSessionManager2, &pAudioSessionEnum);

        UnInitAll(pmmDevice, pAudioSessionManager2, pAudioSessionEnum);
    }
}