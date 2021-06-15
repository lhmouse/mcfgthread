|CI            |Category                   |Build on OS   |Host OS       |Build Status     |
|:-------------|:--------------------------|:-------------|:-------------|:----------------|
|**Travis CI** |:1st_place_medal:Primary   |Ubuntu Trusty |Windows Vista |[![Build Status](https://travis-ci.org/lhmouse/mcfgthread.svg?branch=master)](https://travis-ci.org/lhmouse/mcfgthread)  |
|**Tea CI**    |:1st_place_medal:Primary   |Wine          |Windows Vista |[![Build Status](https://tea-ci.org/api/badges/lhmouse/mcfgthread/status.svg)](https://tea-ci.org/lhmouse/mcfgthread)  |

### 简介
### Introduction

MCF Gthread 是为 Windows 7 及更高版本的 Windows 设计的高效 Gthread 实现，用于移植 GCC 的用途。  
MCF Gthread is a Gthread implementation for Windows 7 and above that meets the requirements of Gthread interfaces, playing a role as a replacement for the (currently working but smelly and inefficient) gthr-win32 implementation when porting GCC.  

C++0x 线程支持处于实验阶段。  
目前包含一次性初始化、互斥体、递归互斥体、条件变量和线程局部存储的功能已经实现。  
The C++0x thread support is currently experimental.  
All the other features including once-initialization, mutex, recursive mutex, condition variable and thread local storage (TLS) have been implemented.  

该项目是从 [MCF CRT](https://github.com/lhmouse/MCF/tree/master/MCFCRT) 中 fork 出来的。  
This project was forked from [MCF CRT](https://github.com/lhmouse/MCF/tree/master/MCFCRT).  

** WARNING **  
该项目目前仅在 x86 和 x64 下开发以及测试，其它处理器架构未有验证。  
This project is developed and tested under x86 and x64 and hasn't been tested on other CPU architectures.  

该项目使用了很多未有微软文档的系统调用来保证运行效率，因此并无法保证在未来的 Windows 版本中仍然能够正常工作。  
作者对该项目不提供担保，风险需要您自己承担。  
This project uses a number of undocumented NT system calls for efficiency reasons and may be broken in future Windows versions.  
The author gives no warranty for this project. Use it at your own risk.  
** END WARNING **  

### 构建
### How to build

我们建议你在 MSYS2 中编译这个项目。你可以在 [这里](https://msys2.github.io/) 下载。  
We suggest you build this project in MSYS2, which can be obtained [here](https://msys2.github.io/).  

如果你刚刚安装 MSYS2，你需要一些基本的开发工具包。如果你已经有 MSYS2 的开发环境请跳过这一步：  
If you have just installed a fresh MSYS2, you would have to install a few development packages. You may skip this step if you have already had MSYS2 development environment set up:  

```bash
$ pacman -Sy
$ pacman -S base-devel mingw-w64-{i686,x86_64}-toolchain --noconfirm
$ pacman -R gdb # Usually you should remove it because it is for MSYS2 programs and not for native programs.
                # You may install native GDB using the following command:
                #     pacman -S mingw-w64-{i686,x86_64}-gdb
```

假设当前目录是在该项目中，在 bash 中运行以下命令：  
Assuming the working directory is this project, run the following commands in bash:  

```bash
$ mkdir -p m4
$ autoreconf -i
$ ./configure
$ make -j4
$ ls *.dll.a
```

如果构建是成功的，你将会看到这个文件：  
If the DLL is built successfully, you will see the follow file:  

```text
libmcfgthread.dll.a
```

现在看看这个 DLL 是否正常工作：  
Now check whether it is working:  

```bash
$ gcc -std=c11 test/test.c -otest.exe -Isrc/env -L. -lmcfgthread
$ PATH=".:$PATH" ./test.exe
```

如果一切顺利，你将会看到类似这样的输出（TLS key 的值是机器相关的，而 TLS 的析构函数是乱序执行的）：  
If everything goes well you will see something like this (the value of the TLS key is machine-dependent and TLS destructors are executed out-of-order):  

```text
key = 00342D20
constructed tls data 0
waiting for thread 0
constructed tls data 1
constructed tls data 2
constructed tls data 3
destructing tls data 1
destructing tls data 3
destructing tls data 2
destructing tls data 0
waiting for thread 1
waiting for thread 2
waiting for thread 3
counter = 4000000
```

现在我们看到所有的 TLS 都被正确创建并析构，而受到互斥体保护的 counter 上并未出现数据竞争。祝贺你！  
Now we can tell that all TLS slots have been constructed and destructed without leaks and the counter protected by a mutex has suffered from no data races. Cheers!  

### IRC channel:

<ircs://irc.libera.chat:6697/#mcfproj>
