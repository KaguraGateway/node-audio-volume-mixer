# node-audio-volume-mixer
"node-audio-volume-mixer" is a library that allows you to control volume in Node.js.
Currently, only Windows is supported.
Pull requests for MacOS and Linux support are welcome!
It supports TypeScript types.

## Installation
To install, use npm.
`npm install node-audio-volume-mixer`


## Examples
### JavaScript
```javascript
const { NodeAudioVolumeMixer } = require("node-audio-volume-mixer");

// Is the master volume muted?
console.log(`The master volume mute: ${NodeAudioVolumeMixer.isMasterMuted()}`);

// Set the mute state of the master volume.
// Mute
NodeAudioVolumeMixer.muteMaster(true);
// UnMute
NodeAudioVolumeMixer.muteMaster(false);


// Get the master volume.
NodeAudioVolumeMixer.getMasterVolumeLevelScalar();

// Set the master volume to 50%.
NodeAudioVolumeMixer.setMasterVolumeLevelScalar(0.5);


// Get a audio session.
const sessions = NodeAudioVolumeMixer.getAudioSessionProcesses();

// Find Google Chrome by session.
const session = sessions.find((value) => {
    return value.name === "chrome.exe";
});

// Get the mute state of Google Chrome.
console.log(`Google Chrome: ${NodeAudioVolumeMixer.isAudioSessionMuted(session.pid)}`);

// Set the mute state of Google Chrome
// Mute
NodeAudioVolumeMixer.setAudioSessionMute(session.pid, true);
// UnMute
NodeAudioVolumeMixer.setAudioSessionMute(session.pid, false);

// Get the volume of Google Chrome.
// This volume is relative to the master volume.
console.log(`The volume of Google Chrome: ${NodeAudioVolumeMixer.getAudioSessionVolumeLevelScalar(session.pid)}`);

// Set the volume of Google Chrome to 50%.
// This volume is relative to the master volume.
NodeAudioVolumeMixer.setAudioSessionVolumeLevelScalar(session.pid, 0.5);
```


### TypeScript
```typescript
import { NodeAudioVolumeMixer } from "node-audio-volume-mixer";

// Is the master volume muted?
console.log(`The master volume mute: ${NodeAudioVolumeMixer.isMasterMuted()}`);

// Set the mute state of the master volume.
// Mute
NodeAudioVolumeMixer.muteMaster(true);
// UnMute
NodeAudioVolumeMixer.muteMaster(false);


// Get the master volume.
NodeAudioVolumeMixer.getMasterVolumeLevelScalar();

// Set the master volume to 50%.
NodeAudioVolumeMixer.setMasterVolumeLevelScalar(0.5);


// Get a audio session.
const sessions = NodeAudioVolumeMixer.getAudioSessionProcesses();

// Find Google Chrome by session.
const session = sessions.find((value) => {
    return value.name === "chrome.exe";
});

// Get the mute state of Google Chrome.
console.log(`Google Chrome: ${NodeAudioVolumeMixer.isAudioSessionMuted(session.pid)}`);

// Set the mute state of Google Chrome
// Mute
NodeAudioVolumeMixer.setAudioSessionMute(session.pid, true);
// UnMute
NodeAudioVolumeMixer.setAudioSessionMute(session.pid, false);

// Get the volume of Google Chrome.
// This volume is relative to the master volume.
console.log(`The volume of Google Chrome: ${NodeAudioVolumeMixer.getAudioSessionVolumeLevelScalar(session.pid)}`);

// Set the volume of Google Chrome to 50%.
// This volume is relative to the master volume.
NodeAudioVolumeMixer.setAudioSessionVolumeLevelScalar(session.pid, 0.5);
```


## LICENSE
Licensed under the MIT
Copyright (c) 2021 Kagura Gateway