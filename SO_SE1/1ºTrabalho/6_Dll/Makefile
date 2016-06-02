#http://msdn.microsoft.com/en-us/library/dd9y37ha.aspx
ALL = 6_DLL.dll app.exe
CFLAGS = /Zi
all: $(ALL)

app.exe: app.obj 6_DLL.dll
	cl $(CFLAGS) app.obj /link 6_DLL.lib  /DYNAMICBASE:NO

6_DLL.dll: 6_DLL.obj
	link /OUT:$@ /DLL $(**) /DYNAMICBASE:NO

6_DLL.obj: 6_DLL.cpp 
	cl $(CFLAGS) /c /D DLL_PROCVMEM_EXPORTS $(**)


%.obj: %.cpp
	cl $(CFLAGS) $(**)
		
clean:
	del /Q $(ALL) *.obj *.pdb *.suo *.ilk *.lib *.dll *.exp *.sln *.vcxproj* *.sdf
