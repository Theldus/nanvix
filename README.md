# Nanvix monolithic
Nanvix is a 32-bit Unix-like operating system designed to be simple, lightweight,
yet fully featured, and capable of running a variety of modern software.

## A BIT OF HISTORY 
[![License: GPLv3](https://img.shields.io/badge/License-GPLv3-orange.svg)](https://opensource.org/licenses/gpl-3-0)

Nanvix originated around 2011 as a graduation project by Pedro H. Penna, with 
the aim of becoming a didactic operating system to be used as supplementary 
material in Operating Systems courses. It succeeded remarkably well: a system 
that was simple yet modern, and much easier to understand than Minix.

My contributions to Nanvix began in late 2015, when Pedro aimed to transform it 
into a research operating system. We wanted to add several features and make it 
robust enough for manycore and high-performance environments. Thus, we started 
with a fork of the educational version, which we called the monolithic version 
(for lack of a better name). Several changes were made on top of the 
educational version, which had already diverged significantly from the original 
educational system.

However, the 'nanvix-monolithic' still retained its educational roots and its 
nature as a general-purpose OS, making it increasingly difficult to reconcile 
these aspects. This led to the development of several new experimental kernels, 
built from scratch with different approaches (such as microkernel and 
multikernel), and ported to new architectures, this time taking into account 
our past experiences with the monolithic version and avoiding repeating the 
same mistakes.

To cut a long story short, my daily contributions to the Nanvix project ended 
in 2019. This repository is my personal fork of the monolithic version from 
that period, which contains most of my contributions to the Nanvix project.

Unfortunately, it seems that the original repository (with over 1,000 commits) 
was deleted, making this possibly the only remaining fork of the monolithic 
version, or at least the only one still maintained.

## ABOUT THIS FORK
Contrary to its original intent of nanvix-monolithic, this fork aims to pick up 
where development left off, focusing on my efforts to clean up the kernel, 
improve its organization, and add new features.

The main goal is for this repository/fork to represent what the educational 
version could have been with enhancements—essentially a general-purpose OS 
with new features and capabilities. It may not be as simple and didactic as its 
origins, but it’s still straightforward enough for OS development beginners 
to grasp.

This branch/fork (dev) _was_ a direct fork from
[nanvix/monolithic-kernel (master)](https://github.com/nanvix/monolithic-kernel),
but with differences, including but not limited to:

- No Thread/Multi-Core support (commit `#68b32b5` removed)
- Focus on LiveCD, with a 'huge' INITRD of 64 MiB
- New system calls
- Bug Fixes
- C/C++ Support
- OS Specific Toolchain
- Ported programs:
  - GCC 6.4.0
  - Binutils 2.27
  - [Kilo](https://github.com/antirez/kilo) Text Editor
  - LuaJIT 2.1.0-beta3
  - PDCurses 3.9-4bc97e2
  - Nano v2.7.2
  - [Nyancat](https://github.com/klange/nyancat)
  - Python 3.6.15
  - more coming soon
- OR1K not currently supported¹ (checkout `#bdf4f27` for a working version)

This fork/branch is just a playground where I eventually (very rarely) intend
to add some things that I think are cool without worrying about upstream.

Therefore, this branch has no guarantees of compatibility with
nanvix-monolithic although isolated commits should work, with little or no
change, depending on the subject.

And of course, don't forget to check the related repositories:
[nanvix-legacy (the educational ver)](https://github.com/nanvix/nanvix-legacy)
and the Nanvix [organization](https://github.com/nanvix), all the
development and exciting things are there, ;-).

### Notes:
¹: The latest changes were more architecture-dependent than I would like,
and it is complicated to maintain two architectures alone. Therefore,
support for or1k is not a priority, since its use is limited to virtual
machines and FPGAs (the latter being absurdly incredible and which
fortunately I had the opportunity to work with), which sadly few have
access to.

## BUILDING AND RUNNING

Nanvix currently supports x86-based PC platforms. You can run it
either in a real platform or a real machine.

To properly build Nanvix, you need a Linux like programming
environment, the x86 GNU C Compiler, and the x86 GNU Binutils.

If you are running a Debian-based Linux distribution, you can run
the following commands.

- To clone this repository (default folder name is nanvix):
  ```bash
  $ cd path/to/clone
  $ git clone https://github.com/Theldus/nanvix.git -b dev [folder-name]
  ```

- To get the development environment setup:
  ```bash
  $ cd path/folder-name
  $ sudo bash tools/dev/setup-toolchain.sh           # (distro specific, currently Ubuntu and Arch)
  $ sudo bash tools/dev/arch/setup-toolchain-i386.sh
  $ sudo bash tools/dev/setup-qemu.sh
  ```

Don't forget that after running `setup-toolchain-i386.sh`, you need to
set up the environment variables in your ~/.bashrc, as informed at the
end of the script execution, usually something like:

  ```bash
  export TARGET=i386                                                       # Target architecture
  export PATH=$PATH:/usr/local/nanvix-toolchain/bin                        # Toolchain binaries
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/nanvix-toolchain/lib  # MPC, MPFR and GMP libs
  ```

- To build Nanvix:
  ```bash
  $ cd path/folder-name
  $ make nanvix > /dev/null
  $ make image
  ```

- To run Nanvix on a virtual machine:
  ```bash
  $ cd path/folder-name
  $ bash tools/run/run-qemu.sh
  ```

## PORTS

The programs ported to Nanvix are separated from the main build-tree, so
to compile them you can:

  ```bash
  $ cd path/folder-name
  $ make ports # make ports-clean will clean-up your ports environment
  ```
or you may want to do per project:
  ```bash
  $ cd path/folder-name/src/ubin/PORTS/ported-program
  $ bash build.sh # bash clean.sh will clean-up your ports environment
  ```

after that you can build Nanvix (if not yet) and make an ISO image with:
  ```bash
  $ make nanvix > /dev/null
  $ make image
  ```

## LICENSE AND MAINTAINERS
This fork is a free software that is under the GPL V3 license, originally 
created by Pedro H. Penna, and now maintained by me, Davidson Francis.
