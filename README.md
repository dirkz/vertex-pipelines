# Pipelines

Sample vertex pipelines, for various platforms.

~~Let's start with DirectX 12~~. Vulkan is breathing down my neck. Let's tackle that.

## Status of DirectX 12

`dx12` was the last effort before I had to switch to vulkan.

`orig_dx12` is a previous, more complete version of a directx 12 pipeline.
It needs to be updated to find the directx utility headers, but otherwise should work.
Some concepts there are slightly outdated/based on false assumptions,
i.e. I initially thought that it's best to have separate command lists
instead of just reusing one or two with different allocators.

## Useful git commands

The DirectX-Headers, and possibly other libraries that I want to have tightly integrated
are listed via git submodules, and ssh URLs are used wherever possible.

Initial clone:

`git clone --recurse-submodules git@github.com:dirkz/vertex-pipelines.git`

Initializing the submodules after the initial clone has already been done
(so that you have the exact same version that was lastly used during development):

`git submodule update --init`

