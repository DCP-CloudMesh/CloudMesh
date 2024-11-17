load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_proto//proto:defs.bzl", "proto_library")

################################# define local and nolocal #################################
# Conditional compile definitions based on --define=local=<value> build flag
config_setting(
    name = "is_local",
    values = {"define": "local=true"},
)

# Set defines based on whether LOCAL is defined
local_defines = select({
    "//conditions:default": ["NOLOCAL=1"],  
    ":is_local": ["LOCAL=1"],               
})

################################# library BUILD target (files in src and include) #################################
cc_library(
    name = "include_files",
    hdrs = glob(["include/**/*.h"]),
    deps = [
        "//:utility_cc_proto",
        "//:payload_cc_proto",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "src_files",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["src/**/*.h"]),
    visibility = ["//visibility:public"],
    deps = [":include_files"],
    defines = local_defines, 
)

################################# proto BUILD target #################################
proto_library(
    name = "utility_proto",
    srcs = ["proto/utility.proto"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "utility_cc_proto",
    deps = [":utility_proto"],
    visibility = ["//visibility:public"],
)

proto_library(
    name = "payload_proto",
    srcs = ["proto/payload.proto"],
    deps = [":utility_proto"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "payload_cc_proto",
    deps = [":payload_proto"],
    visibility = ["//visibility:public"],
)

################################# main BUILD target #################################
cc_binary(
    name = "bootstrap",
    srcs = ["main.cpp"],
    defines = ["BOOTSTRAP=1"] + local_defines,
    deps = [":src_files"],
)

cc_binary(
    name = "provider",
    srcs = ["main.cpp"],
    defines = ["PROVIDER=1"] + local_defines,                    
    deps = [":src_files"],
)

cc_binary(
    name = "requester",
    srcs = ["main.cpp"],
    defines = ["REQUESTER=1"] + local_defines,                    
    deps = [":src_files"],
)