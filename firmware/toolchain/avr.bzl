def _get_deps_attr(ctx, attr):
  deps = list()
  if hasattr(ctx.attr, "deps"):
    for x in ctx.attr.deps:
      deps += getattr(x.avr, attr)
  return deps

def _get_transitive_libs(ctx):
  return _get_deps_attr(ctx, "libs")

def _get_transitive_hdrs(ctx):
  hdr_files = _get_deps_attr(ctx, "hdrs")
  hdr_files.extend(ctx.files.hdrs)
  return hdr_files

def _deepcopy_dict_internal(data):
  if type(data) == "list":
    result = []
    for item in data:
      result.append(item)
  elif type(data) == "tuple":
    aux = []
    for item in data:
      aux.append(item)
    result = tuple(aux)
  else:
    result = data
  return result

def _deepcopy_dict(data):
  result = {}
  for key, value in data.items():
    result[key] = _deepcopy_dict_internal(value)
  return result

def _get_avr_attrs(**attrs):
  avr_attrs = _deepcopy_dict(attrs)
  if "deps" in avr_attrs.keys():
    avr_attrs["deps"] = [x + "_avr" if ((x.startswith("//") or x.startswith(":")) and not x.endswith("_avr")) else x for x in avr_attrs["deps"]]
  return avr_attrs

def _get_standard_arguments():
  return [
    "-Os",
    "-mmcu=atmega32u4",
    "-std=c++17",
    "-Wall",
    "-Wno-main",
    "-Wundef",
    "-Werror",
    "-Wfatal-errors",
    "-Wl,--relax,--gc-sections",
    "-g",
    "-gdwarf-2",
    "-funsigned-char",
    "-funsigned-bitfields",
    "-fpack-struct",
    "-fshort-enums",
    "-ffunction-sections",
    "-fdata-sections",
    "-iquote", "."]

def _avr_library_impl(ctx):
  objs = []
  srcs_list = depset(ctx.files.srcs).to_list()
  hdrs_list = _get_transitive_hdrs(ctx)
  objs_outputs_path = "_objs/" + ctx.label.name + "/"

  for src_file in ctx.files.srcs:
    basename = src_file.basename.rpartition('.')[0]
    obj_file = ctx.actions.declare_file(objs_outputs_path + basename + ".o")
    ctx.actions.run(
      inputs = [src_file] + hdrs_list,
      outputs = [obj_file],
      mnemonic = "BuildAVRObject",
      executable = ctx.executable._compiler,
      arguments = _get_standard_arguments() + [src_file.path, "-o", obj_file.path, "-c"],
    )
    objs.append(obj_file)

  lib = ctx.actions.declare_file("lib" + ctx.label.name + ".a")
  ctx.actions.run(
    inputs = objs + hdrs_list,
    outputs = [lib],
    mnemonic = "BuildAVRLibrary",
    executable = ctx.executable._archiver,
    arguments = ["rcs"] + [lib.path] + [x.path for x in objs]
  )

  return struct(
    avr = struct(
      hdrs = hdrs_list,
      libs = [lib] + _get_transitive_libs(ctx),
    ),
    files = depset([lib]),
  )

def _avr_binary_impl(ctx):
  libs = _get_deps_attr(ctx, "libs")
  link_args = []
  link_args.extend([x.path for x in ctx.files.srcs])
  link_args.extend(["-o", ctx.outputs.binary.path])
  link_args.extend([x.path for x in libs])
  for src in ctx.files.srcs:
    action_inputs = [src]
    action_inputs.extend(ctx.files.hdrs)
    action_inputs.extend(libs)
    action_inputs.extend(_get_transitive_hdrs(ctx))
    
    ctx.actions.run(
      inputs = action_inputs,
      outputs = [ctx.outputs.binary],
      mnemonic = "LinkAVRBinary",
      executable = ctx.executable._compiler,
      arguments = _get_standard_arguments() + link_args,
    )
  return DefaultInfo(executable = ctx.outputs.binary)

def _avr_hex_impl(ctx):
  ctx.actions.run_shell(
    inputs = [ctx.file.src],
    tools = [ctx.executable._objcopy],
    outputs = [ctx.outputs.hex],
    command = "%s -R .eeprom -O ihex %s %s" % ( \
      ctx.executable._objcopy.path,
      ctx.file.src.path,
      ctx.outputs.hex.path,
    )
  )

avr_build_content = """
package(default_visibility = ["//visibility:public"])

filegroup(
  name = "avr_compiler",
  srcs = ["avr-g++"],
)

filegroup(
  name = "avr_ar",
  srcs = ["avr-ar"],
)

filegroup(
  name = "avr_objcopy",
  srcs = ["avr-objcopy"],
)
"""

def avr_tools_repository():
  native.new_local_repository(
    name = "avrtools",
    path = "/usr/local/bin",
    build_file_content = avr_build_content,
  )

_avr_pure_library = rule(
  _avr_library_impl,
  attrs = {
    "_compiler": attr.label(
      default = Label("@avrtools//:avr_compiler"),
      allow_single_file = True,
      executable = True,
      cfg = "host",
    ),
    "_archiver": attr.label(
      default = Label("@avrtools//:avr_ar"),
      allow_single_file = True,
      executable = True,
      cfg = "host",
    ),
    "srcs": attr.label_list(allow_files = [".cpp"]),
    "hdrs": attr.label_list(allow_files = [".h"]),
    "deps": attr.label_list(),
  },
)

def avr_pure_library(name, **attrs):
  _avr_pure_library(name = name + "_avr", **_get_avr_attrs(**attrs))

def avr_library(name, **attrs):
  native.cc_library(name = name, **attrs)
  avr_pure_library(name = name, **attrs)

_avr_binary = rule(
  _avr_binary_impl,
  executable = True,
  attrs = {
    "_compiler": attr.label(
      default = Label("@avrtools//:avr_compiler"),
      allow_single_file = True,
      executable = True,
      cfg = "host",
    ),
    "srcs": attr.label_list(allow_files = [".cpp"]),
    "hdrs": attr.label_list(allow_files = [".h"]),
    "deps": attr.label_list(),
  },
  outputs = {
    "binary": "%{name}.elf",
  },
)

def avr_binary(name, **attrs):
  _avr_binary(name = name, **_get_avr_attrs(**attrs))

avr_hex = rule(
  _avr_hex_impl,
  attrs = {
    "src": attr.label(mandatory=True, allow_single_file=True),
    "_objcopy": attr.label(
      default = Label("@avrtools//:avr_objcopy"),
      allow_single_file = True,
      executable = True,
      cfg = "host",
    ),
  },
  outputs = {
    "hex": "%{name}.hex",
  },
)
