mkdir lib
mkdir obj
cl /c /Fo:obj\ /DEBUG /I include /I ..\..\glad\include /I "C:\Program Files\OpenSSL\include" include\libcake\src\*.c include\glad.c
link /DLL /OUT:lib\libcake.dll /LIBPATH:"C:\Program Files\OpenSSL\lib" obj\*.obj User32.lib Gdi32.lib opengl32.lib ws2_32.lib libssl.lib libcrypto.lib
lib /OUT:lib\libcake.lib /LIBPATH:"C:\Program Files\OpenSSL\lib" obj\*.obj