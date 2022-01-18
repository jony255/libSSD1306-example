# Machine files

[Machine files](https://mesonbuild.com/Machine-files.html) are `meson`'s
solution to expressing various build configurations. In short, you
independently describe each aspect of your configuration in machine files and,
come build time, layer the relavent machine files on top of one another.
`libSSD1306` was designed with this in mind.

This example project uses 3 machine files.

## Machine files used in this example project

There are two machine files (i.e. layers) necessary to this project along with
an optional one that can provide size savings in the final compiled binary.

The order in which they show up on this list are the order in which they should
be layered.

### arm-none-eabi-gcc.ini (toolchain)

This file defines the binaries provided by the `arm-none-eabi` toolchain.
It currently defaults to whatever appears first in your `$PATH` and is not
tied to a specific version/path. Change it to whatever works best for you.

### embedded-common-flags.ini (optional optimization)

This file defines common flags used in embedded projects. Currently, the only
flag defined for use in other machine files is `c_args_separate_sections`. This
places each function or data item in it's own section.

This is most useful when statically linking the library into your project and
you tell the linker to garbage collect unused sections. This will throw out
unused sections (i.e. functions) from your binary.

### bluepill.ini (target machine)

This files defines all of the bluepill specific compiler/`meson` flags of the project
and uses the flag defined in `embedded-common-flags.ini`. Additionally, it defines
the target machine's environment. `libSSD1306` doesn't make use of this information
but getting in the habit of defining this section can prove to be beneficial in some
of your future projects which incorporate `meson`.

## Using a different toolchain

`libopencm3` has only been tested with the `arm-none-eabi` toolchain.
Therefore, I would recommend sticking to the same toolchain in projects that
incorporate `libopencm3`. However, lets say that wasn't the case. Let's say
it has been successfully built with `llvm`. In that case, due to the layering
we've been doing, the only change that would need to be reflected in our setup
would be the first (`arm-none-eabi-gcc.ini`) machine file. In it, we would
describe the `llvm` binaries used to compile our project and that's pretty much it!
