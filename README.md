# ExeLoader

A npl dll plugin for loading and running external executable file.

## Usage

将 ExeLoader.dll 安装在 `plugins/` 目录下。

通过 activate 来调用 ExeLoader，其中的参数
- exe_path，是需要调用的外部可执行文件的路径
- input，是传输给可执行文件的标准输入内容
- callback，在可执行文件执行结束之后，会通过这个地址，通过 activate 传递执行的结果。e.g. "Mod/PyRuntime/Transpiler.lua"

```lua
NPL.activate("plugins/ExeLoader.dll", {
    exe_path = external_exe_file_path,
    input = stdin_for_exe_file,
    callback = npl_callback_file_path
})
```

在 npl_callback_file_path 中，注册 activate 方法，准备接收可执行文件的执行结果。

返回的 msg 有 3 个属性，
- runtime_error，bool 值，在底层通过 C++ 调用 exe_file 的时候有没有出现错误
- exe_error，bool 值，可执行文件是否正常退出，通过退出码是否为 0 来判断
- output，string 值，可执行文件的标准输出内容

```lua
local function activate()
    if msg then
        local runtime_error = msg["runtime_error"]
        local exe_error = msg["exe_error"]
        local output = msg["output"]

        if runtime_error then
        else
            if exe_error then
            else
            end
        end
    end
end

NPL.this(activate)
```

## Dev

clone 代码

```
$ git clone https://github.com/tatfook/ExeLoader.git
```

通过 cmake-gui 生成 VS sln 解决方案。

![](./cmake.jpg)

VS 打开 sln 解决方案，编译，生成 dll 在 `target/` 目录下

