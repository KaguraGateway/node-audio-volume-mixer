interface ProcessStruct {
    pid: number;
    name: string;
}

interface NodeAudioVolumeMixer {
    /**
     * @return mute true=mute, false=unmute
     */
    isMasterMuted(): boolean;
    /**
    * @param mute true=mute, false=unmute
    */
    muteMaster(mute: boolean): void;
    /**
     * @return 0-1.0
     */
    getMasterVolumeLevelScalar(): number;
    /**
     * @param volumeScale 0-1.0
     */
    setMasterVolumeLevelScalar(volumeScale: number): void;



    getAudioSessionProcesses(): Array<ProcessStruct>;
    /**
    * @return true=mute, false=unmute
    */
    isAudioSessionMuted(pid: number): boolean;
    /**
    * @param mute true=mute, false=unmute
    */
    setAudioSessionMute(pid: number, mute: boolean): void;
    /**
     * @return 0-1.0
     * Ratio to master volume.
     */
    getAudioSessionVolumeLevelScalar(pid: number): number;
    /**
     * @param volumeScale 0-1.0
     * Ratio to master volume.
     */
    setAudioSessionVolumeLevelScalar(pid: number, volumeScale: number): void;
}

export const NodeAudioVolumeMixer: NodeAudioVolumeMixer = require("../build/Release/addon.node");