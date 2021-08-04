{
    "targets": [
        {
            "target_name": "addon",
            "sources": ["./src/cpp/nodeBridge.cpp", "./src/cpp/os/windows.cpp"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'defines': [ 'NAPI_CPP_EXCEPTIONS' ]
        }
    ]
}