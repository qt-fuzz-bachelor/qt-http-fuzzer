# Fuzzing Implementation for Qt HTTP Server

This project implements a fuzzing framework for `QHttpServer` that exercises the server using both structured and unstructured HTTP/1.1 inputs. It generates valid, protocol‑aware requests to explore edge cases in request handling, as well as malformed and random data to stress‑test robustness. The goal is to evaluate server stability, correctness, and resilience against malformed input and potential security issues.

## Table of Contents

* [Build and Run Instructions](#build-and-run-instructions)
   * [Linux](#linux)
   * [macos](#macos)
   * [windows](#windows)
* [Internal Practices](#internal-practices)


---

## Build and Run Instructions

This document describes how to reproduce the build and instrumentation setup used during the fuzzing campaigns.

## Linux

### Prerequisites

Set the base directory used for building Qt:

```bash
export QTDEV=/path/to/qtdev
```

The build process consists of:

1. Fetching Qt source code
2. Building host tools
3. Building an instrumented Qt version used for fuzzing
4. Building the fuzzing harness

LibFuzzer requires Qt to be built using `clang` and `clang++`.

---

### Install Dependencies

```bash
sudo apt update && sudo apt install -y \
    cmake git ninja-build clang lld afl++ \
    libgl1-mesa-dev libegl1-mesa-dev libglu1-mesa-dev \
    freeglut3-dev mesa-common-dev
```

---

### Fetch Qt Source Code

```bash
mkdir -p $QTDEV

git clone git://code.qt.io/qt/qt5.git $QTDEV/src

cd $QTDEV/src

./init-repository -f \
    --module-subset=qtbase,qthttpserver,qttools \
    --no-resolve-deps \
    --no-optional-deps
```

---

### Build Host Tools with Clang

```bash
export CC=clang
export CXX=clang++

mkdir -p $QTDEV/host-build
cd $QTDEV/host-build

$QTDEV/src/configure -nomake tests -nomake examples \
    -developer-build -debug

ninja host_tools
```

---

### Build Qt with AFL++ Instrumentation

```bash
export CC=afl-clang-fast
export CXX=afl-clang-fast++

mkdir -p $QTDEV/afl-build
cd $QTDEV/afl-build

$QTDEV/src/configure -nomake tests -nomake examples -nomake doc-snippets \
    -no-gui -no-widgets -developer-build -debug \
    -qt-host-path $QTDEV/host-build/qtbase

ninja
```

Qt is built twice:

* A host build for development tools
* An instrumented build used during fuzzing

These builds are linked together when compiling the final harness binaries.

---

### Build the Fuzzing Harness

#### Clone Repository

```bash
git clone https://github.com/qt-fuzz-bachelor/qt-http-fuzzer.git

cd qt-http-fuzzer
```

#### Configure Build

```bash
cmake -S . -B afl-build \
    -DCMAKE_C_COMPILER=afl-clang-fast \
    -DCMAKE_CXX_COMPILER=afl-clang-fast++ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH="$QTDEV/afl-build/qtbase;$QTDEV/host-build/qtbase" \
    -DQT_HOST_PATH=$QTDEV/host-build/qtbase \
    -DFUZZER_ENGINE=afl \
    -DFUZZ_HTTP_PARSER=ON
```

#### Build Harness

```bash
cmake --build afl-build
```

---

### Optional AFL++ Instrumentation Modes

Additional instrumentation configurations require separate Qt and harness builds.

Set the desired environment variables before building.

#### Enable ASan

```bash
export AFL_USE_ASAN=1
```

#### Enable CMPLOG

```bash
export AFL_LLVM_CMPLOG=1
```

#### Enable LAF-Intel

```bash
export AFL_LLVM_LAF_SPLIT_SWITCHES=1
export AFL_LLVM_LAF_TRANSFORM_COMPARES=1
export AFL_LLVM_LAF_SPLIT_COMPARES=1
```

---

## macOS

The macOS build process follows the same overall steps as Linux.

Because AppleClang does not include LibFuzzer support, an external LLVM toolchain must be installed.

---

### Install Dependencies

```bash
brew install cmake git ninja afl++ llvm@18

xcode-select --install

sudo xcode-select --switch /Library/Developer/CommandLineTools
```

---

### Configure LLVM Environment

```bash
export PATH="/opt/homebrew/opt/llvm@18/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm@18/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm@18/include"
export DYLD_LIBRARY_PATH="/opt/homebrew/opt/llvm@18/lib"
```

---

### Remaining Steps

Follow the same Qt and harness build steps as described for Linux, using the LLVM toolchain instead of AppleClang.

---

## Windows

The Windows build process uses:

* WinAFL
* DynamoRIO
* Visual Studio

The setup was tested on Windows 10.

---

### Base Directory

Run all commands in the **x64 Native Tools Command Prompt for Visual Studio**.

```bat
set QTDEV="C:\path\to\qtdev"
```

---

### Install Dependencies

1. Install Git
2. Install CMake

   * A version below 3.5 is required by WinAFL
3. Install DynamoRIO

   * Version `cronbuild-11.91.20491` was used
4. Install Visual Studio Community 2022

   * Select the **Desktop development with C++** workload
   * Ensure MSVC v143 is installed

---

### Build WinAFL

#### Clone Repository

```bat
git clone https://github.com/googleprojectzero/winafl.git

cd winafl
```

#### Initialize Submodules

Required when using Intel PT.

```bat
git submodule update --init --recursive
```

#### Configure Build

```bat
cmake -S . -B build ^
    -G "Visual Studio 17 2022" ^
    -A x64 ^
    -DDynamoRIO_DIR="C:\Users\Test\Documents\DynamoRIO-Windows-X\cmake" ^
    -DINTELPT=1 ^
    -DUSE_COLOR=1
```

#### Build

```bat
cmake --build build --config Release
```

---

### Build Qt on Windows

#### Fetch Qt Source

```bat
mkdir %QTDEV%

cd %QTDEV%

git clone git://code.qt.io/qt/qt5.git %QTDEV%\src

cd %QTDEV%\src

.\init-repository -f ^
    --module-subset=qtbase,qthttpserver,qttools ^
    --no-resolve-deps ^
    --no-optional-deps
```

---

### Build Qt

```bat
mkdir %QTDEV%\build

cd %QTDEV%\build

%QTDEV%\src\configure.bat -developer-build ^
    -nomake tests -nomake examples -nomake doc-snippets -no-gui -no-widgets

ninja
```

---

### Configure PATH

Add the following directories to the system `PATH`:

#### Qt

```text
<qtdev>\build\qtbase\bin
```

#### WinAFL

```text
<winafl>\build\bin\Release
```

Restart the terminal after updating environment variables.

---

### Build the Fuzzing Harness

#### Clone Repository

```bat
git clone https://github.com/qt-fuzz-bachelor/qt-http-fuzzer.git

cd qt-http-fuzzer
```

#### Configure Build

```bat
cmake -S . -B build ^
    -G "Visual Studio 17 2022" ^
    -DFUZZ_HTTP_PARSER=ON
```

#### Build

```bat
cmake --build build --config Release
```

---

### Run WinAFL

Run the following command from:

```text
qt-http-fuzzer\build\harnesses\Release
```

```bat
afl-fuzz.exe ^
    -i C:\Users\Test\Documents\qt-http-fuzzer\corpus ^
    -o C:\Users\Test\Documents\qt-http-fuzzer\outputs ^
    -D C:\Users\Test\Documents\DynamoRIO-Windows-X\bin64 ^
    -w C:\Users\Test\Documents\winafl\build\bin\Release\winafl.dll ^
    -t 2000 ^
    -- ^
    -coverage_module Qt6HttpServerd.dll ^
    -target_module fuzz_parser_winafl.exe ^
    -target_method target ^
    -nargs 2 ^
    -covtype edge ^
    -- ^
    fuzz_parser_winafl.exe @@
```

---

### References

* Qt: [Qt Official Website](https://www.qt.io?utm_source=chatgpt.com)
* AFL++: [AFL++ GitHub Repository](https://github.com/AFLplusplus/AFLplusplus?utm_source=chatgpt.com)
* WinAFL: [WinAFL GitHub Repository](https://github.com/googleprojectzero/winafl?utm_source=chatgpt.com)
* DynamoRIO: [DynamoRIO Website](https://dynamorio.org?utm_source=chatgpt.com)


## Internal practices

### Setting up Pre-commit Hooks
To ensure code quality and commit message consistency, we use `pre-commit` hooks. Follow these steps to install the necessary tools and enable the hooks:

Run the following command to install the required tools.

#### 1. Install dependencies
##### On macOS:
Run the following command to install the required tools via Homebrew:
```bash
brew install pre-commit cpplint cppcheck npm

npm install --save-dev @commitlint/cli @commitlint/config-conventional
```
##### On Windows:
Commands are ment to be ran in a PowerShell terminal
1. **Install Python**
```bash
winget install Python.Python.3.14
```
&nbsp; Which includes `pip`, which we will use later. <br>
&nbsp; Find module path, should be somewhere around `C:\Users\Username\Appadata\Local\Python\PythonCore-version\Scripts` <br>
&nbsp; Add it as an environment variable to the System Variable **Path**, this lets us run module commands in the terminal. <br>
&nbsp; Re-open the terminal so the changes take effect.

2. **Install Node.js**
   Download and install [Node.js](https://nodejs.org/), which provides `npm`.

3. **Install dependencies**
   Open **Command Prompt** or **PowerShell** and run:
```bash
pip install pre-commit cpplint
npm install -g commitlint @commitlint/config-conventional
winget install cppcheck.cppcheck
```

##### On Ubuntu/Linux:
Use your distribution’s package manager to install cppcheck, npm, and Python packages. For example, on Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y cppcheck npm python3-pip
pip3 install pre-commit cpplint
npm install -g commitlint @commitlint/config-conventional
```

#### 2. Initialize pre-commit hooks in your repository
After installing dependencies, run the following commands inside your project folder:
```bash
pre-commit autoupdate
pre-commit install
pre-commit install --hook-type commit-msg
```


### Commit Message Format

Commit messages should follow this structure:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```


#### Type (required)

The `type` describes the purpose of the commit. Common types include:

| Type     | Meaning                                                        |
|----------|----------------------------------------------------------------|
| `feat`   | A new feature                                                  |
| `fix`    | A bug fix                                                     |
| `chore`  | Changes to the build process, tooling, or maintenance (no production code) |
| `docs`   | Documentation only changes                                    |
| `style`  | Code style, formatting, missing semicolons, etc. (does *not* affect logic) |
| `refactor` | Code change that neither fixes a bug nor adds a feature     |
| `perf`   | Code changes that improve performance                        |
| `test`   | Adding or fixing tests                                        |
