#include <napi.h>

#include "os/common.h"

namespace nodeAudioVolumeMixer {
    namespace nodeBridge {
        using Napi::String;
        using Napi::CallbackInfo;
        using Napi::Number;
        using Napi::Boolean;
        using Napi::Object;
        using Napi::Array;

        void ThrowError(Napi::Env *env, const char *message) {
            size_t messageSize = strlen(message);
            Napi::Error::New(*env, message).ThrowAsJavaScriptException();
        }

        Napi::Value IsMasterMuted(const CallbackInfo &args) {
            Napi::Env env = args.Env();
            bool isMuted = nodeAudioVolumeMixer::IsMasterMuted();

            return Boolean::New(env, isMuted);
        }

        Napi::Value MuteMaster(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Args Check
            if(args.Length() <= 0) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsBoolean()) {
                ThrowError(&env, "The argument must be a bool.");
                return env.Null();
            }

            bool isMute = args[0].As<Boolean>().ToBoolean();
            nodeAudioVolumeMixer::MuteMaster(isMute);
            return env.Null();
        }

        Napi::Value GetMasterVolumeLevelScalar(const CallbackInfo & args) {
            Napi::Env env = args.Env();
            float volumeScale = nodeAudioVolumeMixer::GetMasterVolumeLevelScalar();

            return Number::New(env, volumeScale);
        }

        Napi::Value SetMasterVolumeLevelScalar(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Args Check
            if(args.Length() <= 0) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsNumber()) {
                ThrowError(&env, "The argument must be a number.");
                return env.Null();
            }

            float volumeScale = args[0].As<Number>().FloatValue();
            nodeAudioVolumeMixer::SetMasterVolumeLevelScalar(volumeScale);
            return env.Null();
        }



        Napi::Value GetAudioSessionProcesses(const CallbackInfo & args) {
            Napi::Env env = args.Env();
            std::vector<nodeAudioVolumeMixer::Process> processes = nodeAudioVolumeMixer::GetAudioSessionProcesses();

            Array arr = Array::New(env, processes.size());
            for(int i=0; i < processes.size(); i++) {
                Object obj = Object::New(env);
                obj.Set("pid", Number::New(env, processes[i].pid));
                obj.Set("name", processes[i].name);
                arr.Set(i, obj);
            }

            return arr;
        }

        Napi::Value IsAudioSessionMuted(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Check Args
            if(args.Length() <= 0) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsNumber()) {
                ThrowError(&env, "The argument must be a number.");
                return env.Null();
            }

            // Args
            int32_t pid = args[0].As<Number>().Int32Value();

            bool isMuted = nodeAudioVolumeMixer::IsAudioSessionMuted(pid);
            return Boolean::New(env, isMuted);
        }

        Napi::Value SetAudioSessionMute(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Check Args
            if(args.Length() <= 1) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsNumber() || !args[1].IsBoolean()) {
                ThrowError(&env, "The argument must be a number and boolean.");
                return env.Null();
            }

            // Args
            int32_t pid = args[0].As<Number>().Int32Value();
            bool isMute = args[1].As<Boolean>().ToBoolean();

            nodeAudioVolumeMixer::SetAudioSessionMute(pid, isMute);
            return env.Null();
        }

        /**
         * @return 0-1.0
         */
        Napi::Value GetAudioSessionVolumeLevelScalar(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Check Args
            if(args.Length() <= 0) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsNumber()) {
                ThrowError(&env, "The argument must be a number.");
                return env.Null();
            }

            // Args
            int32_t pid = args[0].As<Number>().Int32Value();

            float volumeScale = nodeAudioVolumeMixer::GetAudioSessionVolumeLevelScalar(pid);
            return Number::New(env, volumeScale);
        }

        /**
         * @param volumeScale 0-1.0
         */
        Napi::Value SetAudioSessionVolumeLevelScalar(const CallbackInfo & args) {
            Napi::Env env = args.Env();

            // Check Args
            if(args.Length() <= 1) {
                ThrowError(&env, "The argument missing.");
                return env.Null();
            } else if(!args[0].IsNumber() || !args[1].IsNumber()) {
                ThrowError(&env, "The argument must be a number and number.");
                return env.Null();
            }

            // Args
            int32_t pid = args[0].As<Number>().Int32Value();
            float volumeScale = args[1].As<Number>().FloatValue();

            nodeAudioVolumeMixer::SetAudioSessionVolumeLevelScalar(pid, volumeScale);
            return env.Null();
        }


        Object Initialize(Napi::Env env, Napi::Object exports) {
            exports.Set(String::New(env, "isMasterMuted"), Napi::Function::New(env, IsMasterMuted));
            exports.Set(String::New(env, "muteMaster"), Napi::Function::New(env, MuteMaster));
            exports.Set(String::New(env, "getMasterVolumeLevelScalar"), Napi::Function::New(env, GetMasterVolumeLevelScalar));
            exports.Set(String::New(env, "setMasterVolumeLevelScalar"), Napi::Function::New(env, SetMasterVolumeLevelScalar));

            exports.Set(String::New(env, "getAudioSessionProcesses"), Napi::Function::New(env, GetAudioSessionProcesses));
            exports.Set(String::New(env, "isAudioSessionMuted"), Napi::Function::New(env, IsAudioSessionMuted));
            exports.Set(String::New(env, "setAudioSessionMute"), Napi::Function::New(env, SetAudioSessionMute));
            exports.Set(String::New(env, "getAudioSessionVolumeLevelScalar"), Napi::Function::New(env, GetAudioSessionVolumeLevelScalar));
            exports.Set(String::New(env, "setAudioSessionVolumeLevelScalar"), Napi::Function::New(env, SetAudioSessionVolumeLevelScalar));

            return exports;
        }

        NODE_API_MODULE(NODE_GYP_MODULE_NAME, Initialize);
    }
}