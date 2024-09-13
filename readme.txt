This is the readme file for Traylauncher source. 
There is also a Chinese version of this file, "readme_cn.txt".

Traylauncher is a portable file launcher for windows.

To build from source, you may use Code::Blocks IDE or VisualC++ 2010 express.
The CB folder contains the Code::Blocks (http://www.codeblocks.org/) project file.
The VisualStudio folder contains the VC++ project files.
The src folder contains all the source code and resources.

VC2008 express version should also work, 
You can add all files in "src" folder and adjust some project settings.
The VC2010 project was converted from a 2008 project which had been converted from a VC2005 project.

Traylauncher menu command editor uses wxWidgets(www.wxwidgets.org).
Traylauncher uses pugixml to handle xml file format.

Some Note about version control:
	I created Traylauncher a few years ago, before I knew of version control and SVN (subversion).
	So the earlier code are not under version control.
	I used SVN later, until I decided to make it open-source.
	After read a tutorial (HgInit.com), I thought I'd prefer Mercurial.

	I did a "Hg convert", and it worked, until I "update". It's the filenames.
	I'm Chinese, and some files in the svn repository have Chinese 
	filenames which Mercurial does NOT support (on windows).
	Then I changed the filenames to English and "commit" (to SVN) and "Hg convert" again.
	Now it works, but not for older versions.

	So, "update" to older versions will NOT work well,
	and these older versions do NOT have this readme file either.

Chao Li, 2010-06-26
