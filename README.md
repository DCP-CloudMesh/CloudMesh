# CloudMesh

## Dependencies
Follow this to install bazel - https://bazel.build/install

### Third Party Dependencies

We can install ZeroMQ for cpp as follows. Run the following starting from the `CloudMesh/third_party/` folder.

#### Tailscale

Tailscale is used to setup a P2P vpn network to connect the machines. Instructions to install can be found here:
https://tailscale.com/kb/1347/installation

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

## Compilation (Non Local - Multiple Machines)

To compile **BOOTSTRAP**, **PROVIDER** and **REQUESTER**, ensure that the `BOOTSTRAP_HOST` env variable is set, which can be done using the following command:
```
export BOOTSTRAP_HOST=___
```
The `BOOTSTRAP_PORT` env variable can also be set (unset is default to 8080).

Then run the following commands  to compile the source code:
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
(Uses port 8080)

### Provider
```
./bazel-bin/provider -p <port number>
```
Example:
```
./bazel-bin/provider -p 8081
```
Program arguments can be viewed with
```
./bazel-bin/provider -h
```

### Requester

```
./bazel-bin/requester -w <# workers> -e <# epochs> -p <port number> -m <mode ('c' or 'r')>
```
Request to compute task example
```
./bazel-bin/requester -w 3 -e 10 -p 8082 -m c
```
Request to receive results example
```
./bazel-bin/requester -p 8082 -m r
```
The program execution for receiving results must use the same port as the execution of the compute request.

Program arguments can be viewed with
```
./bazel-bin/requester -h
```

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

### How to build proto python file
Run the following command in project root directory:
```
protoc --python_out=ml <.proto file location>
```
Example:
```
protoc --python_out=ml proto/payload.proto
protoc --python_out=ml proto/utility.proto
```


## VSCode Include Path (to get rid of red squiggly lines)
To include the dependency build files in VSCode, add the following to the includePath for Cpp:
```json
"C_Cpp.default.includePath": [
  "${workspaceFolder}/bazel-PeerToPeer/external/{your_target_directory}"
]
```
