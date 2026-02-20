# Fuzzing Implementation for Qt HTTP Server

This project implements a fuzzing framework for `QHttpServer` that exercises the server using both structured and unstructured HTTP/1.1 inputs. It generates valid, protocol‑aware requests to explore edge cases in request handling, as well as malformed and random data to stress‑test robustness. The goal is to evaluate server stability, correctness, and resilience against malformed input and potential security issues.

## Setting up Pre-commit Hooks
To ensure code quality and commit message consistency, we use `pre-commit` hooks. Follow these steps to install the necessary tools and enable the hooks:

Run the following command to install the required tools.

### 1. Install dependencies
#### On macOS:
Run the following command to install the required tools via Homebrew:
```bash
brew install pre-commit cpplint cppcheck npm

npm install --save-dev @commitlint/cli @commitlint/config-conventional
```
#### On Windows:
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

#### On Ubuntu/Linux:
Use your distribution’s package manager to install cppcheck, npm, and Python packages. For example, on Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y cppcheck npm python3-pip
pip3 install pre-commit cpplint
npm install -g commitlint @commitlint/config-conventional
```

### 2. Initialize pre-commit hooks in your repository
After installing dependencies, run the following commands inside your project folder:
```bash
pre-commit autoupdate
pre-commit install
pre-commit install --hook-type commit-msg
```


## Commit Message Format

Commit messages should follow this structure:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```


### Type (required)

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
