# Machine files

[Machine files](https://mesonbuild.com/Machine-files.html) are `meson`'s solution to expressing various build configurations.
`libSSD1306` was designed with this in mind. This example project uses 3 machine files.

## Machine files used in this example project

### arm-none-eabi-gcc.ini

This file defines the binaries provided by the `arm-none-eabi` toolchain.
It currently defaults to whatever appears first in your `$PATH` and is not
tied to a specific version/path. Change it to whatever works best for you.

### embedded-common-flags.ini

This file defines common flags used in embedded projects. Currently, the only
flag defined for use in other machine files is `c_args_separate_sections`. This
places each function or data item in it's own section.

This is most useful when statically linking the library into your project and
you tell the linker to garbage collect unused sections. This will throw out
unused sections (i.e. functions) from your binary.

### bluepill.ini

This files defines all of the bluepill specific compiler/`meson` flags of the project
and uses the flag defined in `embedded-common-flags.ini`. Additionally, it defines
the target machine's environment. `libSSD1306` doesn't make use of this information
but getting in the habit of defining this section can prove to be beneficial in some
of your future projects which incorporate `meson`.
