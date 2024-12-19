# CloudMesh 

## Dependencies
Follow this to install bazel - https://bazel.build/install

### Third Party Dependencies

We can install ZeroMQ for cpp as follows. Run the following starting from the `CloudMesh/third_party/` folder.

#### Mac Dependencies

May need to install the following when troubleshooting issues

```bash
brew install gnutls
```

#### Window Dependencies

To be determined (???)

#### `libzmq` dependency
```bash
git clone https://github.com/zeromq/libzmq
cd libzmq
mkdir build
cd build
sudo cmake .. -DENABLE_DRAFT=ON -DENABLE_CURVE=OFF -DENABLE_WSS=OFF 
sudo make install
```

#### `cppzmq` dependency
```bash
git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
sudo cmake ..
sudo make install
```

We install ZeroMQ for Python using `pip install pyzmq`.

## Key Files -
- BUILD file - Contains the build instructions for the targets.
- MODULE.bazel file - Contains the module name and the dependencies.

## Compilation

To compile **BOOTSTRAP**, **PROVIDER** and **REQUESTER**, run the following commands:
### MacOS
```
bazel build //... --experimental_google_legacy_api --config=macos
```
### Linux
```
bazel build //... --experimental_google_legacy_api
```

## Compilation (Local)

To compile **BOOTSTRAP**, **PROVIDER** and **REQUESTER** locally, run the following commands:
### MacOS
```
bazel build //... --define local=true --experimental_google_legacy_api --config=macos
```
### Linux
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


### Resources

The peers assume data is located in the directory specified by
the `DATA_DIR` parameter in `utility.h`. All filenames and globbing 
options are taken w.r.t the `DATA_DIR` parameter.

## Tests
A few unit tests are available. They can be run with
```bash
bazel test //:utility_test
```

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
2. Run the build command to auto generate the new proto files.
3. Import the new proto file by adding this line to the top of the file:
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


protoc --python_out=ml proto/payload.proto  