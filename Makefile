build: tema0.exe

tema0.exe: hashtable.obj  
	link -nologo hashtable.obj /out:tema0.exe 
hashtable.obj:
	cl hashtable.c 
clean:
	del /Q /F *.obj tema0.exe
