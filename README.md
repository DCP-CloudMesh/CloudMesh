# CloudMesh

## Dependencies
Follow this to install bazel - https://bazel.build/install

## Key Files -
- BUILD file - Contains the build instructions for the targets.
- MODULE.bazel file - Contains the module name and the dependencies.

## Compilation

To compile **BOOTSTRAP**, **PROVIDER** and **REQUESTER**, run the following commands:
```
bazel build //... --experimental_google_legacy_api
```

## Compilation (Local)

To compile **BOOTSTRAP**, **PROVIDER** and **REQUESTER** locally, run the following commands:
```
bazel build //... --define local=true --experimental_google_legacy_api
```

## Execution

To execute, run the following commands:
### Bootstrap
```
./bazel-bin/bootstrap
```
(8080 is reserved for bootstrap port so peers know where to connect)

### Provider
```
./bazel-bin/provider [8080]
``` 
(8080 is the default port, optional parameter)

### Requester
```
./bazel-bin/requester [8080 [r | c]]
```
`8080` is the default port, optional parameter\
`r` is an optional parameter to request to receive the result of the computation (use same port as original request execution)
`c` is an optional parameter to request to provide the computation
## Clean

## To clean the build files:
```
bazel clean
```

## How to add a new external dependency
Add the external dependency in MODULE.bazel file. This will now be visible to all the targets in the module.\
For example: 
```
http_archive(
    name = "com_github_nlohmann_json",
    urls = ["https://github.com/nlohmann/json/archive/v3.11.3.tar.gz"],
    sha256 = "0d8ef5af7f9794e3263480193c491549b2ba6cc74bb018906202ada498a79406",
    strip_prefix = "json-3.11.3",
)
```

## How to add a new proto file
1. Create your proto file in the `proto` directory. (similar to `payload.proto`)
2. Add the following to the `BUILD` file in the root folder. Replace `{name}` with the name of your proto file.
```
proto_library(
    name = "{name}_proto",
    srcs = ["proto/{name}.proto"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "{name}_cc_proto",
    deps = [":{name}_proto"],
    visibility = ["//visibility:public"],
)
```
3. Edit the `include_files` cc_library target in the `BUILD` file in the root folder to include the new proto file.
```
cc_library(
    name = "include_files",
    hdrs = glob(["include/**/*.h"]),
    deps = [
        "//:utility_cc_proto",
        "//:payload_cc_proto",
        ... (other proto libraries)
        "//:{name}_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
```
4. Run the build command to auto generate the new proto files.
5. Import the new proto file by adding this line to the top of the file:
```
#include "proto/{name}.pb.h"
```

## VSCode Include Path (to get rid of red squiggly lines)
To include the dependency build files in VSCode, add the following to the includePath for Cpp:
```json
"C_Cpp.default.includePath": [
  "${workspaceFolder}/bazel-PeerToPeer/external/{your_target_directory}"
]
```
