#include <iostream>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <tchar.h>
#include <vector>


typedef std::basic_string<TCHAR, std::char_traits<TCHAR>> tstring;

namespace nodeAudioVolumeMixer {
    struct Process {
        int pid;
        tstring name;
    };

    bool IsMasterMuted();

    /**
     * @param mute true=mute, false=unmute
     */
    void MuteMaster(const bool mute);

    /**
     * @return 0-1.0
     */
    float GetMasterVolumeLevelScalar();

    /**
     * @param volumeScale 0-1.0
     */
    void SetMasterVolumeLevelScalar(const float volumeScale);



    std::vector<Process> GetAudioSessionProcesses();

    bool IsAudioSessionMuted(const int32_t pid);

    void SetAudioSessionMute(const int32_t pid, bool mute);

    /**
     * @return 0-1.0
     * Ratio to master volume.
     */
    float GetAudioSessionVolumeLevelScalar(const int32_t pid);

    /**
     * @param volumeScale 0-1.0
     * Ratio to master volume.
     */
    void SetAudioSessionVolumeLevelScalar(const int32_t pid, float volumeScale);
}