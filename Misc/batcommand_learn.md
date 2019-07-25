
* 设置变量名字  
  set xxx=dddd
这样就设置了xxx的名字为ddddd了，可以通过 echo %xxx% 把他打印出来，这个命令很有用，例如添加环境变量path的内容常常写成  
set PATH=%PATH%;ADDPATH;   
这样就在path右面添加了了ADDPATH。
  
* 退出当前脚本  
  EXIT [/B] [exitCode]
  这个/b是有用，不然就是直接退出cmd了， 
  exit /b 2

* for循环字符串截取  
 for /F "delims=: tokens=2" %%I in ("%first_ip%") do (
  echo %%I > %ipfilename%
)
这个还是比较有用的，他通过delims后面的字符去分割字符串，tokens=2表示取第二个。不要tokens是1，而且分割的字符是没有:的。

* 从文件读取数据保存在变量中
  set /P first_ip= < %ipfilename%
  这个/p比较重要，他表示输入数据，可以从文件输入，也可以从控制台输入。
  set /P first_ip= input File:
  这里的input File:是提示你输入，最后他的值是输入的内容。

* 去掉空格
  %abc: =%这个命令就能去掉他的所有空格。如果只左边的可以用 %abc:~1%去掉左边一个字符，%abc:~0,1%得到坐标一个字符。 %abc:~-1%得到右边一个字符。
  %abc:~0,-1%去掉右边一个字符。
 if [%localip:~0,7%] == [1234567] (
	echo "11"
) else (
	if [%localip:~0,6%] == [123456] (
		echo "dd"
	)
)
这个就是取出前面7个字符，

* 压入目录，退出目录
  pushd ..\xxxx2
	start xxxx.bat
	popd
 这个比较有用，他可以把当前目录压入，然后进入到指定目录，执行结束后有返回回来。