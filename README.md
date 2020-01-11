# shadowalk

A simple x64dbg plugin which can do

![alt text](https://github.com/trichimtrich/shadowalk/raw/master/demo.gif)

#### Instantly run

Skip all normal/hardware/memory breakpoint and run straight to a selected address

Has option to ignore `System breakpoint` too

#### Instantly restart

Restart and do **Instantly run**

## Installation

Simply copy 
- `shadowalk.dp32` to `x32/plugins`
- `shadowalk.dp64` to `x64/plugins`

## DevEnv

- *Optional*: Update `pluginsdk` to latest version
- Copy `release` in x64dbg snapshot to `shadowalk/` ProjectDir
- Dir-tree
```
└───shadowalk
    ├───bin
    ├───pluginsdk
    │   ├───dbghelp
    │   ├───DeviceNameResolver
    │   ├───jansson
    │   ├───lz4
    │   ├───TitanEngine
    │   └───XEDParse
    ├───release
    │   ├───shadowalk.tlog
    │   ├───translations
    │   ├───x32
    │   │   ├───db
    │   │   ├───memdumps
    │   │   ├───platforms
    │   │   └───plugins
    │   └───x64
    │       ├───db
    │       ├───memdumps
    │       ├───platforms
    │       └───plugins
```
