workspace(name = "magnon")

# Setup hermetic clang toolchain
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

BAZEL_TOOLCHAIN_TAG = "0.8.2"
BAZEL_TOOLCHAIN_SHA = "0fc3a2b0c9c929920f4bed8f2b446a8274cad41f5ee823fd3faa0d7641f20db0"

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = BAZEL_TOOLCHAIN_SHA,
    strip_prefix = "bazel-toolchain-{tag}".format(tag = BAZEL_TOOLCHAIN_TAG),
    canonical_id = BAZEL_TOOLCHAIN_TAG,
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

# Setup hermetic python toolchain
load("//internal:python_interpreter.bzl", "py_download")
py_download(
    name = "py_darwin_x86_64",
    arch = "x86_64",
    os = "darwin",
    sha256 = "fc0d184feb6db61f410871d0660d2d560e0397c36d08b086dfe115264d1963f4",
    urls = ["https://github.com/indygreg/python-build-standalone/releases/download/20211017/cpython-3.10.0-x86_64-apple-darwin-install_only-20211017T1616.tar.gz"],
)
py_download(
    name = "py_linux_x86_64",
    arch = "x86_64",
    os = "linux",
    sha256 = "eada875c9b39cc4bf4a055dd8f5188e99c0c90dd5deb05b6c213f49482fe20a6",
    urls = ["https://github.com/indygreg/python-build-standalone/releases/download/20211017/cpython-3.10.0-x86_64-unknown-linux-gnu-install_only-20211017T1616.tar.gz"],
)
py_download(
    name = "py_darwin_arm64",
    arch = "arm64",
    os = "darwin",
    sha256 = "1409acd9a506e2d1d3b65c1488db4e40d8f19d09a7df099667c87a506f71c0ef",
    urls = ["https://github.com/indygreg/python-build-standalone/releases/download/20220227/cpython-3.10.2+20220227-aarch64-apple-darwin-install_only.tar.gz"],
)
register_toolchains(
    "@py_darwin_x86_64//:toolchain",
    "@py_linux_x86_64//:toolchain",
    "@py_darwin_arm64//:toolchain",
)

http_archive(
    name = "eigen",
    build_file = "//build_external:eigen.BUILD",
    sha256 = "8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72",
    strip_prefix = "eigen-3.4.0",
    urls = ["https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz"],
)

http_archive(
  name = "range_v3",
  url = "https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.zip",
  sha256 = "cbcb96beda464e71d293c07dec89ef5c0790ca83d37b0e199890893019441044",
  strip_prefix = "range-v3-0.12.0",
)

http_archive(
  name = "nlohmann_json",
    build_file = "//build_external:json.BUILD",
  url = "https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz",
  sha256 = "d69f9deb6a75e2580465c6c4c5111b89c4dc2fa94e3a85fcd2ffcd9a143d9273",
  strip_prefix = "json-3.11.2",
)

git_repository(
    name = "fmt",
    commit = "757564f5cd2fa78b634dd698c63dbf069818e6fd",
    remote = "https://github.com/fmtlib/fmt",
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
)

# Protobuf library.
http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-4.24.2",
    sha256 = "e218d1afcfedf7fbf80456f867539d7a5775569a3fa8e1b90856b9e4d2866ae9",
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

load("@rules_python//python:pip.bzl", "pip_install")

pip_install(
    name = "pip_deps",
    requirements = "//:requirements.txt",
)
load("@pip_deps//:requirements.bzl", "install_deps")

install_deps()


# Add pybind11.
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-fd7d88857cca3d7435b06f3ac6abab77cd9983b2",
  sha256 = "ca401da77da9712bb585595796c4d8ec5253e55292f5ecf7773db1b33b26715e",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/fd7d88857cca3d7435b06f3ac6abab77cd9983b2.zip"],
)
# We still require the pybind library.
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.11",
  urls = ["https://github.com/pybind/pybind11/archive/v2.11.tar.gz"],
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(
  name = "local_config_python",
)


# Boost Libraries
http_archive(
    name = "com_github_nelhage_rules_boost",
    url = "https://github.com/nelhage/rules_boost/archive/54aaeeac21382cb433f5630fc966395ee5447b2b.tar.gz",
    strip_prefix = "rules_boost-54aaeeac21382cb433f5630fc966395ee5447b2b",
    sha256 = "f5d7f3943d60b86d848d9bfbe6636914d887f11adf1f714c487f0a767d5bae79",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()
