# libcake

Librairie que je développe et que j'utilise dans mes autres projets.

## Compilation
Linux :<br>
`gcc *.c include/underground/src/*.c -D_REENTRANT -pthread -o cake`<br>
Windows :<br>
`gcc *.c include\underground\src\*.c -o cake.exe -municode`<br>

> Certaines fonctions nécessitent d'avoir `openssl`, dans le cas où SSL est utilisé, `-DCAKE_SSL=1 -lssl` doivent être ajoutés dans la commande.<br>
*Et `-lWs2_32` sur Windows.*<br>
Si crypto est utilisé, `-DPIKA_CRYPTO=1 -lcrypto` doivent être ajoutés dans la commande.

