源代码：https://bellard.org/stereovision-1.00.tgz

1. 安装XQuartz
2. 把X11和GL加入链接，可以建立两个软连接
```sh
$ sudo ln -s /opt/X11/include/GL /usr/local/include/GL
$ sudo ln -s /opt/X11/include/X11 /usr/local/include/X11
```
3. 运行自带的测试程序，由于未知原因display函数没有实现，没法看到结果，猜测是通过OpenGL渲染了个三维图？