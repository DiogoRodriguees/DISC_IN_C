#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "disco.h"

int main(){
    Disco* disco = disco_cria("dados", 770000);

    disco_grava(disco, "a.jpg"); // nome arquivo deve conter o caminho absoluto ou relativo do arquivo
    disco_grava(disco, "b.pdf"); 
    disco_grava(disco, "c.docx"); 
    disco_grava(disco, "d.png"); 
    disco_grava(disco, "foto.jpeg"); 
    disco_grava(disco, "mario.jpeg"); 
    disco_lista(disco);

    // disco_remove(disco, "b.pdf");
    disco_remove(disco, "c.docx");
    disco_remove(disco, "b.pdf");

    disco_grava(disco, "rapoza.jpeg"); 

    disco_recupera(disco, "d.png", "./recuperados/foto.png");
    disco_recupera(disco, "rapoza.jpeg", "./recuperados/arquivoMaior.png");
    disco_lista(disco);

    printf("\n * Finalizado * \n");
    return 0;
}