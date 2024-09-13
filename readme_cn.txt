这是Traylauncher源代码的自述文件。对应的英文版是 "readme.txt"。

Traylauncher 是一个免安装的Windows程序启动程序。

你可以用 Code::Blocks 或者 VisualC++ 2010 express 编译工程。
CB文件夹中是 Code::Blocks (http://www.codeblocks.org/) 的工程文件。
VisualStudio 文件夹是 VC++ 工程文件。
src 文件夹是全部的代码和资源。

使用VC2008应该也是可以的，需要手动建立工程的进行设置。
目前的工程就是从VC2005-VC2008-VC2010这样一路升级转换过来的。

菜单命令编辑器使用了wxWidgets库(www.wxwidgets.org)。
Xml格式处理使用pugixml库(pugixml.org)。

版本库说明：
	我几年前刚刚开始开发Traylauncher的时候，还不熟悉版本控制和SVN(subversion)。
	因此早期的代码是没有版本控制的。
	之后我使用了 SVN 来管理代码，它很棒。
	当我准备开源traylauncher的时候，我又转向了水银（mercurial）。
	选择水银这主要源于这里的介绍 "http://HgInit.com" 。

	不过，水银对中文名支持存在问题（在Windows系统），我后来把文件名改成英文，
	但是更新到早期版本还是会有问题的。早期版本也没有本自述文件。
	
李超，2010年6月26日
