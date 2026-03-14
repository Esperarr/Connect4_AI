本目录下需要有 CompeteResult、dll、SourceCode、TempResult、TestCases 五个目录。

1. 可以直接在 SourceCode 里名为 Strategy 的文件夹中的代码进行编写。项目源代码文件可以包括自定义的 .cpp 和 .h 文件。

2. TestCases 目录下存放测试样例。所有测试样例需以两位数编号，例如 02.dll、04.dll、06.dll、08.dll；100.dll 以 00.dll 表示。

3. compile.bat：将项目源代码编译为可执行 dll 文件。。

   - 需修改

     ```bat
     call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
     ```

     为自己电脑上的 Visual Studio 安装路径。

1. compete.bat：对抗测试，每次仅会测试最新的 dll。

2. stat.py：统计得分。

3. run.bat：按顺序执行 compile.bat、compete.bat 和 stat.py。

---

TL;DR：

- 对 SourceCode\Strategy 里的项目代码进行代码编写

- 修改 compile.bat 中的：

  - 需修改

      ```bat
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
      ```

      为自己电脑上的 Visual Studio 软件路径。

- 直接运行 run.bat 即可。