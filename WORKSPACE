workspace(name = "topomagnons")

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
    build_file = "//:eigen.BUILD",
    sha256 = "8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72",
    strip_prefix = "eigen-3.4.0",
    urls = ["https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz"],
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
    strip_prefix = "protobuf-4.23.4",
    sha256 = "5f3cd52d6e5062071d99da57a96ea87e39bc020d6d25748001d919c474a4d8ed",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v4.23.4.tar.gz"],
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
