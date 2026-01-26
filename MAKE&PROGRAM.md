# MagLoc V3.0 环境配置教程

本文档指导在全新Windows PC环境下配置STM32F722项目的编译、烧录和调试环境。

---

## 目录

1. [软件安装](#1-软件安装)
2. [环境变量配置](#2-环境变量配置)
3. [VSCode配置](#3-vscode配置)
4. [编译配置说明](#4-编译配置说明)
5. [编译项目](#5-编译项目)
6. [烧录程序](#6-烧录程序)
7. [调试程序](#7-调试程序)
8. [多PC协作](#8-多pc协作)
9. [常见问题](#9-常见问题)

---

## 1. 软件安装

### 1.1 安装 STM32CubeCLT

1. 下载地址：https://www.st.com/zh/development-tools/stm32cubeclt.html

2. 运行安装程序，建议安装路径：
   ```
   C:\ST\STM32CubeCLT_1.20.0
   ```

3. 安装完成后，目录结构应包含：
   ```
   C:\ST\STM32CubeCLT_1.20.0\
   ├── CMake\                          # CMake构建工具
   ├── GNU-tools-for-STM32\            # ARM交叉编译工具链
   ├── Ninja\                          # Ninja构建工具
   ├── STM32CubeProgrammer\            # 烧录工具
   ├── STLink-gdb-server\              # GDB服务器（调试用）
   ├── STLinkServer\                   # ST-LINK服务器
   └── STMicroelectronics_CMSIS_SVD\   # 调试符号文件
   ```

### 1.2 安装 VSCode

1. 下载地址：https://code.visualstudio.com/

2. 安装后，启动VSCode

### 1.3 安装 VSCode 扩展

在VSCode中安装以下扩展：

| 扩展名称 | 用途 |
|---------|------|
| **Cortex-Debug** | STM32调试支持 |
| **CMake** | CMake项目支持 |
| **CMake Tools** | CMake构建工具 |

安装方法：
1. 按 `Ctrl+Shift+X` 打开扩展面板
2. 搜索扩展名称
3. 点击"安装"

---

## 2. 环境变量配置

### 2.1 添加 STM32CLT_PATH 变量

1. 按 `Win + X`，选择"系统"
2. 点击"高级系统设置"
3. 点击"环境变量"
4. 在"用户变量"区域点击"新建"：

   | 变量名 | 变量值 |
   |--------|--------|
   | `STM32CLT_PATH` | `C:\ST\STM32CubeCLT_1.20.0` |

### 2.2 添加到 PATH 变量

编辑 `Path` 变量，添加以下路径：

```
C:\ST\STM32CubeCLT_1.20.0\GNU-tools-for-STM32\bin
C:\ST\STM32CubeCLT_1.20.0\CMake\bin
C:\ST\STM32CubeCLT_1.20.0\Ninja\bin
C:\ST\STM32CubeCLT_1.20.0\STM32CubeProgrammer\bin
```

### 2.3 验证环境变量

打开新的命令提示符（CMD），执行以下命令验证：

```cmd
echo %STM32CLT_PATH%
```

应输出：`C:\ST\STM32CubeCLT_1.20.0`

```cmd
arm-none-eabi-gcc --version
cmake --version
ninja --version
```

所有命令都应显示版本信息。

---

## 3. VSCode配置

### 3.1 打开项目

1. 启动VSCode
2. 选择 `文件` → `打开文件夹`
3. 选择项目根目录：`TheBox_v3_cmake`

### 3.2 配置文件说明

项目使用 **CMake Presets** 管理多种编译配置，配置文件位于 `CMakePresets.json`：

| 配置 | 说明 | 输出目录 |
|------|------|----------|
| **Debug** | 调试版本 | `build/Debug/` |
| **Release** | 发布版本 | `build/Release/` |
| **RelWithDebInfo** | 优化+调试信息 | `build/RelWithDebInfo/` |
| **MinSizeRel** | 最小体积 | `build/MinSizeRel/` |

### 3.3 VSCode 配置文件

`.vscode/launch.json` - 调试配置，启动时会提示选择构建配置

`.vscode/tasks.json` - 任务配置，包含以下任务：
- `CMake: Configure [配置名]` - 配置指定构建类型
- `CMake: Build [配置名]` - 编译指定构建类型
- `Flash: [配置名]` - 烧录指定构建类型的输出

### 3.4 重启VSCode

配置完成后，**必须重启VSCode**使环境变量生效。

---

## 4. 编译配置说明

### 4.1 优化等级配置位置

编译优化等级在 `cmake/gcc-arm-none-eabi.cmake` 文件中配置（第31-38行）：

```cmake
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0")
```

### 4.2 各配置对比

| 配置 | 优化选项 | 调试符号 | 特点 | 适用场景 |
|------|----------|----------|------|----------|
| **Debug** | `-O0` | `-g3` | 无优化，完整调试信息 | 开发调试 |
| **Release** | `-Os` | `-g0` | 优化体积，无调试信息 | 正式发布 |
| **RelWithDebInfo** | `-O2` | `-g` | 优化速度，含调试信息 | 性能测试+调试 |
| **MinSizeRel** | `-Os` | `-g0` | 最小体积，无调试信息 | Flash空间紧张 |

### 4.3 GCC 优化选项说明

| 选项 | 优化目标 | 代码大小 | 执行速度 | 调试友好度 |
|------|----------|----------|----------|------------|
| `-O0` | 无优化 | 大 | 慢 | ⭐⭐⭐ 最佳 |
| `-O1` | 基础优化 | 中等 | 中等 | ⭐⭐ 一般 |
| `-O2` | 优化速度 | 较大 | 快 | ⭐ 较差 |
| `-O3` | 最大速度 | 大 | 最快 | ⭐ 差 |
| `-Os` | 优化体积 | 小 | 中等 | ⭐ 较差 |
| `-Og` | 调试优化 | 中等 | 中等 | ⭐⭐⭐ 推荐 |

### 4.4 修改优化等级

编辑 `cmake/gcc-arm-none-eabi.cmake` 第31-38行，修改对应的选项：

```cmake
# 示例：将 RelWithDebInfo 改为 -O3 优化
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
```

修改后需重新配置CMake：
```cmd
cmake --preset Debug          # 或其他配置
cmake --build --preset Debug
```

---

## 5. 编译项目

### 5.1 方式一：使用任务（推荐）

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 `Tasks: Run Task`
3. 选择要使用的配置：
   - `CMake: Configure Debug` - 配置Debug
   - `CMake: Build Debug` - 编译Debug
   - 同理适用于 Release/RelWithDebInfo/MinSizeRel

### 5.2 方式二：使用命令行

```cmd
# 配置项目
cmake --preset Debug

# 编译项目
cmake --build --preset Debug

# 其他配置
cmake --preset Release
cmake --build --preset Release
```

### 5.3 编译输出

编译成功后，输出文件位于对应的配置目录：

```
build/
├── Debug/
│   ├── TheBox_V3_0.elf    # ELF可执行文件（调试用）
│   ├── TheBox_V3_0.hex    # Intel HEX格式（烧录用）
│   ├── TheBox_V3_0.bin    # 二进制文件
│   └── TheBox_V3_0.map    # 内存映射文件
├── Release/
│   └── ...
└── RelWithDebInfo/
    └── ...
```

编译结束时会显示内存使用情况：

```
Memory region         Used Size  Region Size  %age Used
     RAM:       98696 B       256 KB     37.65%
   FLASH:       54844 B       512 KB     10.46%
```

---

## 6. 烧录程序

### 6.1 硬件连接

1. 使用ST-LINK连接PC和STM32开发板
2. 连接关系：
   - ST-LINK SWDIO → STM32 SWDIO
   - ST-LINK SWCLK → STM32 SWCLK
   - ST-LINK GND → STM32 GND
   - ST-LINK 3.3V → 可选（如果开发板未供电）

### 6.2 使用任务烧录

1. 先编译对应配置（见第5节）
2. 按 `Ctrl+Shift+P` 打开命令面板
3. 输入 `Tasks: Run Task`
4. 选择 `Flash: Debug` 或其他配置的烧录任务

### 6.3 使用命令行烧录

```cmd
# 烧录Debug版本
STM32_Programmer_CLI.exe -c port=SWD -w build\Debug\TheBox_V3_0.hex 0x08000000 -v -rst

# 烧录Release版本
STM32_Programmer_CLI.exe -c port=SWD -w build\Release\TheBox_V3_0.hex 0x08000000 -v -rst
```

参数说明：
- `-c port=SWD` - 使用SWD接口连接
- `-w` - 写入文件
- `0x08000000` - Flash起始地址（STM32F722）
- `-v` - 验证
- `-rst` - 烧录后复位

### 6.4 烧录成功提示

```
Memory programming completed.
Start address used : 0x08000000
Memory programming completed.
```

---

## 7. 调试程序

### 7.1 启动调试

1. 确保已编译项目（见第5节）
2. 确保ST-LINK已连接
3. 按 `F5` 或点击调试面板的"运行"按钮
4. 在弹出的配置选择中选择要调试的构建类型（Debug/Release等）

### 7.2 调试功能

| 功能 | 快捷键 | 说明 |
|------|--------|------|
| 继续 | F5 | 继续执行程序 |
| 单步跳过 | F10 | 执行下一行（不进入函数） |
| 单步进入 | F11 | 进入函数内部 |
| 单步跳出 | Shift+F11 | 跳出当前函数 |
| 重启 | Ctrl+Shift+F5 | 重新启动调试 |
| 停止 | Shift+F5 | 停止调试 |

### 7.3 断点设置

- 在代码行号左侧点击即可设置断点
- 断点命中时程序会暂停，可以查看变量值
- **注意**：优化版本（Release/MinSizeRel）可能无法正确设置断点

### 7.4 查看变量

- **局部变量**：调试面板 → "变量" → "局部变量"
- **监视表达式**：调试面板 → "监视" → 添加表达式
- **内存查看**：调试面板 → "内存" → 输入地址
- **外设寄存器**：通过SVD文件自动解析（需使用Debug或RelWithDebInfo）

---

## 8. 多PC协作

### 8.1 .gitignore 配置说明

项目已配置 `.gitignore` 以支持多台 PC 协作开发：

| 文件类型 | 是否同步 | 说明 |
|---------|---------|------|
| `build/**/CMakeCache.txt` | ❌ 不同步 | CMake 缓存（含 PC 特定路径） |
| `build/**/CMakeFiles/` | ❌ 不同步 | CMake 生成的配置文件 |
| `build/**/.cmake/` | ❌ 不同步 | CMake API 文件 |
| `build/**/.ninja_*` | ❌ 不同步 | Ninja 构建日志 |
| `build/**/build.ninja` | ❌ 不同步 | Ninja 构建文件 |
| `build/**/*.elf` | ✅ 同步 | 可执行文件（调试用） |
| `build/**/*.hex` | ✅ 同步 | Intel HEX 格式（烧录用） |
| `build/**/*.bin` | ✅ 同步 | 二进制文件 |
| `build/**/*.map` | ✅ 同步 | 内存映射文件 |

### 8.2 首次拉取代码

在新 PC 上首次克隆项目后：

```bash
# 1. 克隆项目
git clone <repository-url> TheBox_v3_cmake
cd TheBox_v3_cmake

# 2. 配置 CMake（每台 PC 各自执行）
cmake --preset=RelWithDebInfo

# 3. 编译项目
cmake --build build --preset=RelWithDebInfo
```

### 8.3 日常协作流程

**PC-A 编译后提交：**
```bash
# 编译并提交烧录文件
cmake --build build --preset=RelWithDebInfo
git add build/RelWithDebInfo/TheBox_V3_0.elf
git add build/RelWithDebInfo/TheBox_V3_0.hex
git commit -m "build: update firmware"
git push
```

**PC-B 拉取更新：**
```bash
# 拉取代码和烧录文件
git pull

# 如果需要本地调试，重新编译
cmake --preset=RelWithDebInfo
cmake --build build --preset=RelWithDebInfo
```

### 8.4 注意事项

1. **路径冲突**：build 目录的配置文件已忽略，避免不同 PC 路径不同导致的冲突
2. **配置文件**：.gitignore 中排除的文件不会同步，每台 PC 各自维护
3. **烧录文件**：.elf/.hex 等文件可以提交，方便团队共享编译结果
4. **重新配置**：切换 PC 或修改工具链后，删除 build 目录重新配置

### 8.5 解决路径冲突

如果遇到 CMake 缓存路径错误：

```bash
# 删除 build 目录
rm -rf build/

# 重新配置
cmake --preset=RelWithDebInfo
cmake --build build --preset=RelWithDebInfo
```

---

## 9. 常见问题

### 8.1 编译错误

**问题**：`arm-none-eabi-gcc: command not found`

**解决**：
- 检查环境变量 PATH 是否包含 ARM 工具链路径
- 重启VSCode

**问题**：`CMake Error: The source directory does not exist`

**解决**：
- 删除 `build` 目录
- 重新运行配置命令

### 8.2 烧录错误

**问题**：`Error: No ST-LINK device detected`

**解决**：
1. 检查ST-LINK连接
2. 更新ST-LINK驱动
3. 更换USB端口

**问题**：`File not found "executable": ...TheBox_V3_0.elf`

**解决**：
- 确保已编译对应的配置
- 检查 `.elf` 文件是否存在于正确的配置目录中

**问题**：`Error: couldn't locate STM32CubeProgrammer`

**解决**：
- 检查 `STM32CLT_PATH` 环境变量是否正确设置
- 检查 `.vscode/launch.json` 中的 `stm32cubeprogrammer` 路径

### 8.3 调试错误

**问题**：`gdb is not running`

**解决**：
1. 检查 `.vscode/launch.json` 配置是否正确
2. 确保已安装 `cortex-debug` 扩展
3. 重启VSCode

**问题**：无法设置断点

**解决**：
- 确保编译的是 Debug 或 RelWithDebInfo 版本
- Release/MinSizeRel 版本因优化可能无法正常调试
- 检查 `.elf` 文件是否存在

**问题**：变量值显示不正确

**解决**：
- 使用 `-Og` 或 `-O0` 优化等级
- 避免使用 `-O2` 或 `-O3` 进行调试

### 8.4 环境变量不生效

**问题**：VSCode中无法读取环境变量

**解决**：
1. 完全关闭VSCode（不只是关闭窗口）
2. 重新打开VSCode
3. 如果仍不生效，重启电脑

### 8.5 切换配置后找不到文件

**问题**：切换配置后提示找不到 `.elf` 或 `.hex` 文件

**解决**：
- 每次切换配置后需要先编译该配置
- 或使用对应的编译/烧录任务（如 `Flash: Release`）

---

## 10. 附录

### 9.1 项目信息

| 项目 | 值 |
|------|-----|
| MCU | STM32F722RE |
| 核心 | ARM Cortex-M7 |
| Flash | 512 KB @ 0x08000000 |
| RAM | 256 KB @ 0x20000000 |
| 调试接口 | SWD |
| 烧录工具 | STM32CubeProgrammer |
| 调试工具 | ST-LINK GDB Server |

### 9.2 参考链接

- [STM32CubeCLT下载](https://www.st.com/zh/development-tools/stm32cubeclt.html)
- [Cortex-Debug文档](https://github.com/Marus/cortex-debug)
- [STM32F722数据手册](https://www.st.com/resource/en/datasheet/stm32f722re.pdf)
- [GCC优化选项说明](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)

### 9.3 目录结构

```
TheBox_v3_cmake/
├── Core/                 # 核心代码
│   ├── Inc/              # 头文件
│   └── Src/              # 源文件
├── Drivers/              # STM32 HAL驱动
├── Middlewares/          # USB设备库
├── USB_DEVICE/           # USB CDC实现
├── modbus_rtu/           # Modbus RTU协议层
├── ICM42688P/            # IMU驱动
├── usb_comm/             # USB通信封装层
├── cmake/                # CMake构建配置
│   └── gcc-arm-none-eabi.cmake  # 工具链和优化配置
├── build/                # 编译输出目录
│   ├── Debug/            # Debug配置输出
│   ├── Release/          # Release配置输出
│   ├── RelWithDebInfo/   # RelWithDebInfo配置输出
│   └── MinSizeRel/       # MinSizeRel配置输出
├── .vscode/              # VSCode配置
├── CMakeLists.txt        # 主CMake文件
├── CMakePresets.json     # CMake预设配置
└── make&program.md       # 本文档
```

### 9.4 内存地址映射

| 起始地址 | 结束地址 | 区域 | 大小 | 说明 |
|----------|----------|------|------|------|
| 0x08000000 | 0x0807FFFF | FLASH | 512 KB | 程序存储器 |
| 0x20000000 | 0x2003FFFF | RAM | 256 KB | 数据存储器 |
| 0x40000000 | 0x4001FFFF | 外设寄存器 | - | GPIO、UART等 |

---

**文档版本**: v2.1
**更新日期**: 2026-01-26
**更新内容**: 新增第8节"多PC协作"说明
