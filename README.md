## WHAT IS NANVIX? [![Build Status](https://api.travis-ci.org/Theldus/nanvix.svg?branch=dev)](https://travis-ci.org/Theldus/nanvix)  [![Join us on Slack!](https://img.shields.io/badge/chat-on%20Slack-e01563.svg)](https://join.slack.com/t/nanvix/shared_invite/enQtMzY2Nzg5OTQ4NTAyLTAxMmYwOGQ0ZmU2NDg2NTJiMWU1OWVkMWJhMWY4NzMzY2E1NTIyMjNiOTVlZDFmOTcyMmM2NDljMTAzOGI1NGY)  

Nanvix is a Unix like operating system created by Pedro H. Penna to
address emerging manycore platforms. It targets cluster-based
architectures that rely on a distributed and shared memory
configuration, and it was designed from scratch to deliver cutting
edge performance, while enabling backward compatibility with
existing software. 

## ABOUT THIS FORK

This branch/fork (dev) is a direct fork from
[nanvix/monolithic-kernel (master)](https://github.com/nanvix/monolithic-kernel),
but with some differences that may or not appear in the upstream, some of them:

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
  - more coming soon
- OR1K not currently supported¹ (checkout `#bdf4f27` for a working version)

However, it is very important to make it clear that I do not intend to start
a "new project" with this or even 'compete' with the original repository.
This fork/branch is just a playground where I eventually (very rarely) intend
to add some things that I think are cool without worrying about upstream.

Therefore, this branch has no guarantees of compatibility with
nanvix-monolithic although isolated commits should work, with little or no
change, depending on the subject.

And of course, don't forget to check the related repositories:
[nanvix/nanvix](https://github.com/nanvix/nanvix),
[nanvix/monolithic-kernel](https://github.com/nanvix/monolithic-kernel)
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
  $ sudo bash tools/dev/setup-toolchain.sh
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
  $ sudo make image
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

Nanvix is a free software that is under the GPL V3 license and is
maintained by Pedro H. Penna. Any questions or suggestions send
him an email: <pedrohenriquepenna@gmail.com>

Join our mailing list at https://groups.google.com/d/forum/nanvix
