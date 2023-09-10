def _py_download(ctx):
    """
    Downloads a Python distribution and registers a toolchain target.
  
    Args:
        ctx: Repository context.
    """
    pass

py_download = repository_rule(
    implementation = _py_download,
    attrs = {
        "urls": attr.string_list(
            mandatory = True,
            doc = "String list of mirror URLs where the Python distribution can be downloaded.",
        ),
        "sha256": attr.string(
            mandatory = True,
            doc = "Expected SHA-256 sum of the archive.",
        ),
        "os": attr.string(
            mandatory = True,
            values = ["darwin", "linux", "windows"],
            doc = "Host operating system.",
        ),
        "arch": attr.string(
            mandatory = True,
            values = ["x86_64"],
            doc = "Host architecture.",
        ),
        "_interpreter_path": attr.string(
            default = "bin/python3",
            doc = "Path you'd expect the python interpreter binary to live.",
        ),
        "_build_tpl": attr.label(
            default = "//build:BUILD.dist.bazel.tpl",
            doc = "Label denoting the BUILD file template that get's installed in the repo.",
        )
    },
)
