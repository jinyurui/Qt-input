# Qt-input
Qt虚拟键盘输入法,类与Qt的自带的虚拟键盘使用。
编译，
首先，编译Googlepinyin为静态库，并替换virtual工程中的lib
然后，编译virtual工程,并拷贝最终dll
最后，编译test工程

运行，
在text的app.exe目录，拷贝dict整个文件夹，新建文件夹为platforminputcontexts并将virtual的dll拷贝到该文件夹，即可运行测试该输入法
