# Ports

This folder is intended to store all external sources and/or build scripts
for programs/libraries that have been successfully ported to Nanvix.

The structure is very simple and follows below:
```
PORTS/
├── build.sh
├── clean.sh
└── my_program_port
    ├── build    (optional, created with build.sh and deleted with clean.sh)
    ├── binaries (should only exist after build.sh and should be deleted with
    │   └── bin   clean.sh)
    │       └── my_port_binary
    ├── build.sh
    ├── clean.sh
    └── src      (optional, only if the source is small enough (up to 1 MiB),
	          otherwise, build.sh should download the source)
```
Scripts build.sh and clean.sh are mandatory and as the name suggests, build
and clean respectively. 

## File structure

### build.sh
This script is very straightforward, it does whatever it takes to generate a
functional build of the program for Nanvix. Since each program can have its
particulars, build.sh should be able to solve them all: whether by downloading
external sources, applying patches, using special build flags, and so on.
(see existing ones for reference).

If the source code exceeds 1 MiB (extracted) build.sh should download them
and put the extracted source code into src/ folder (although in this case,
src/ folder should not be committed).

The script should also generate a 'binaries/' folder at the end of the build
process and it should 
contain the directory tree that matches the filesystem structure.

Due to the Minix filesystem constraints, it is also highly desirable to the
build.sh `strip` everything inside binaries/ folder.

### clean.sh
The clean.sh script is also pretty straightforward: it cleans up everything
the build.sh script did and leaves the source tree  (from Nanvix) the same as
it was before, in general, just delete the binaries. (see existing ones for
reference). The script should delete the binaries/ folder (and build/ as well,
if one) as one of the steps of the cleaning process.

### build/
Temporary folder, stores objects and binary files. This folder basically holds
the result files after a `./configure`. It is (optionally) created with build.sh
and must be removed with clean.sh.

### binaries/
Temporary folder, holds the final binaries. This folder should _never_ be
committed and should _only_ exist after successful invocation in build.sh
script (`make image` copies all the binaries folders, after all). Conversely,
clean.sh script _should_ delete this folder.

### src/
If the extracted source code is below 1 MiB, Nanvix can ship the program
source code, otherwise, src/ _should_ be a temporary folder created by
build.sh and _should_ be deleted when clean.sh is invoked.

## Build process
The building process is very simple and can be invoked from the main
makefile: `make ports` / `make ports-clean`, from the PORTS/ folder:
`bash src/ubin/PORTS/build.sh` or project-basis, e.g:
`bash src/ubin/PORTS/gcc/build.sh`.

All builds were successfully tested and build on:
- Slackware 14.2 (GCC 5.3.0)
- Ubuntu 18.04.3 (GCC 7.4.0)
