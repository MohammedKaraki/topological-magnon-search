workspace(name = "magnon")

# Setup hermetic clang toolchain
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

BAZEL_TOOLCHAIN_TAG = "0.8.2"

BAZEL_TOOLCHAIN_SHA = "0fc3a2b0c9c929920f4bed8f2b446a8274cad41f5ee823fd3faa0d7641f20db0"

http_archive(
    name = "com_grail_bazel_toolchain",
    canonical_id = BAZEL_TOOLCHAIN_TAG,
    sha256 = BAZEL_TOOLCHAIN_SHA,
    strip_prefix = "bazel-toolchain-{tag}".format(tag = BAZEL_TOOLCHAIN_TAG),
    url = "https://github.com/grailbio/bazel-toolchain/archive/refs/tags/{tag}.tar.gz".format(tag = BAZEL_TOOLCHAIN_TAG),
)

load("@com_grail_bazel_toolchain//toolchain:deps.bzl", "bazel_toolchain_dependencies")

bazel_toolchain_dependencies()

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    llvm_version = "15.0.6",
)

load("@llvm_toolchain//:toolchains.bzl", "llvm_register_toolchains")

llvm_register_toolchains()

# Python
RULES_PYTHON_VERSION = "0.23.1"

http_archive(
    name = "rules_python",
    sha256 = "84aec9e21cc56fbc7f1335035a71c850d1b9b5cc6ff497306f84cced9a769841",
    strip_prefix = "rules_python-{}".format(RULES_PYTHON_VERSION),
    url = "https://github.com/bazelbuild/rules_python/releases/download/{}/rules_python-{}.tar.gz".format(
        RULES_PYTHON_VERSION,
        RULES_PYTHON_VERSION,
    ),
)

load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_toolchains")

py_repositories()

python_register_toolchains(
    name = "python_3_11",
    # Available versions are listed in @rules_python//python:versions.bzl.
    # We recommend using the same version your team is already standardized on.
    python_version = "3.11",
)

load("@python_3_11//:defs.bzl", "interpreter")

http_archive(
    name = "eigen",
    build_file = "//build/external:eigen.BUILD",
    sha256 = "8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72",
    strip_prefix = "eigen-3.4.0",
    urls = ["https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz"],
)

http_archive(
    name = "range_v3",
    sha256 = "cbcb96beda464e71d293c07dec89ef5c0790ca83d37b0e199890893019441044",
    strip_prefix = "range-v3-0.12.0",
    url = "https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.zip",
)

http_archive(
    name = "nlohmann_json",
    build_file = "//build/external:json.BUILD",
    sha256 = "d69f9deb6a75e2580465c6c4c5111b89c4dc2fa94e3a85fcd2ffcd9a143d9273",
    strip_prefix = "json-3.11.2",
    url = "https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz",
)

http_archive(
    name = "pybind11_protobuf",
    sha256 = "cf81bc54676bf9ae64c85469f8d95c65863abf526eb49de01098b6478c194846",
    strip_prefix = "pybind11_protobuf-3d7834b607758bbd2e3d210c6c478453922f20c0",
    url = "https://github.com/pybind/pybind11_protobuf/archive/3d7834b607758bbd2e3d210c6c478453922f20c0.zip",
)

git_repository(
    name = "fmt",
    commit = "757564f5cd2fa78b634dd698c63dbf069818e6fd",
    patch_cmds = [
        "mv support/bazel/.bazelversion .bazelversion",
        "mv support/bazel/BUILD.bazel BUILD.bazel",
        "mv support/bazel/WORKSPACE.bazel WORKSPACE.bazel",
    ],
    # Windows-related patch commands are only needed in the case MSYS2 is not installed.
    # More details about the installation process of MSYS2 on Windows systems can be found here:
    # https://docs.bazel.build/versions/main/install-windows.html#installing-compilers-and-language-runtimes
    # Even if MSYS2 is installed the Windows related patch commands can still be used.
    patch_cmds_win = [
        "Move-Item -Path support/bazel/.bazelversion -Destination .bazelversion",
        "Move-Item -Path support/bazel/BUILD.bazel -Destination BUILD.bazel",
        "Move-Item -Path support/bazel/WORKSPACE.bazel -Destination WORKSPACE.bazel",
    ],
    remote = "https://github.com/fmtlib/fmt",
)

# Protobuf library.
http_archive(
    name = "com_google_protobuf",
    sha256 = "e218d1afcfedf7fbf80456f867539d7a5775569a3fa8e1b90856b9e4d2866ae9",
    strip_prefix = "protobuf-4.24.2",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v4.24.2.tar.gz"],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "gtest",
    sha256 = "ffa17fbc5953900994e2deec164bb8949879ea09b411e07f215bfbb1f87f4632",
    strip_prefix = "googletest-1.13.0",
    urls = ["https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip"],
)

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    sha256 = "99bc3106eb6ce5ffab3c31de8501d4d628de5f1acd74b8b563a876bd39a2e32f",
    strip_prefix = "bazel-compile-commands-extractor-b33a4b05c2287372c8e932c55ff4d3a37e6761ed",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/b33a4b05c2287372c8e932c55ff4d3a37e6761ed.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
# Run the tool with:
# bazel run @hedron_compile_commands//:refresh_all

http_archive(
    name = "rules_python",
    sha256 = "954aa89b491be4a083304a2cb838019c8b8c3720a7abb9c4cb81ac7a24230cea",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_python/releases/download/0.4.0/rules_python-0.4.0.tar.gz",
        "https://github.com/bazelbuild/rules_python/releases/download/0.4.0/rules_python-0.4.0.tar.gz",
    ],
)

load("@rules_python//python:pip.bzl", "pip_install", "pip_parse")

pip_parse(
    name = "pip_deps",
    python_interpreter_target = interpreter,
    requirements = "//:requirements.txt",
)

load("@pip_deps//:requirements.bzl", "install_deps")

install_deps()

# Add pybind11.
http_archive(
    name = "pybind11_bazel",
    sha256 = "ca401da77da9712bb585595796c4d8ec5253e55292f5ecf7773db1b33b26715e",
    strip_prefix = "pybind11_bazel-fd7d88857cca3d7435b06f3ac6abab77cd9983b2",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/fd7d88857cca3d7435b06f3ac6abab77cd9983b2.zip"],
)

# We still require the pybind library.
http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    sha256 = "b3e8c9dcc58356b92dbe5ce7fbc159130be937e754717e21e8a3eef86b583c45",
    strip_prefix = "pybind11-2.11",
    urls = ["https://github.com/pybind/pybind11/archive/v2.11.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(
    name = "local_config_python",
    python_interpreter_target = interpreter,
)

# Boost Libraries
http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "f5d7f3943d60b86d848d9bfbe6636914d887f11adf1f714c487f0a767d5bae79",
    strip_prefix = "rules_boost-54aaeeac21382cb433f5630fc966395ee5447b2b",
    url = "https://github.com/nelhage/rules_boost/archive/54aaeeac21382cb433f5630fc966395ee5447b2b.tar.gz",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

# Protobuf expects an //external:python_headers label which would contain the Python headers.
bind(
    name = "python_headers",
    actual = "@third_party//:python_headers",
)

new_local_repository(
    name = "third_party",
    build_file_content = """
alias(
    name = "python_headers",
    actual = "@local_config_python//:python_headers",
    visibility = ["//visibility:public"]
)
    """,
    path = "third_party",
)

# Fast & memory efficient hashtable based on robin hood hashing for C++11/14/17/20
http_archive(
    name = "robin_hood",
    build_file_content = """
cc_library(
    name = "robin_hood",
    hdrs = ["src/include/robin_hood.h"],
    visibility = ["//visibility:public"],
    includes = ["src/include"],
)
    """,
    sha256 = "6eae5b9e30351bc99f09f5031d77cd126d08af6ddb2052416d5129c0eba6855f",
    strip_prefix = "robin-hood-hashing-7697343363af4cc3f42cab17be49e6af9ab181e2",
    url = "https://github.com/martinus/robin-hood-hashing/archive/7697343363af4cc3f42cab17be49e6af9ab181e2.zip",
)

# buildifier is a tool for formatting bazel BUILD and .bzl files with a standard convention.

# buildifier is written in Go and hence needs rules_go to be built.
# See https://github.com/bazelbuild/rules_go for the up to date setup instructions.
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "6dc2da7ab4cf5d7bfc7c949776b1b7c733f05e56edc4bcd9022bb249d2e2a996",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.39.1/rules_go-v0.39.1.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.39.1/rules_go-v0.39.1.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_rules_dependencies")

go_rules_dependencies()

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains")

go_register_toolchains(version = "1.20.3")

http_archive(
    name = "com_github_bazelbuild_buildtools",
    sha256 = "ae34c344514e08c23e90da0e2d6cb700fcd28e80c02e23e4d5715dddcb42f7b3",
    strip_prefix = "buildtools-4.2.2",
    urls = [
        "https://github.com/bazelbuild/buildtools/archive/refs/tags/4.2.2.tar.gz",
    ],
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-98eb410c93ad059f9bba1bf43f5bb916fc92a5ea",
    urls = ["https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip"],
)
