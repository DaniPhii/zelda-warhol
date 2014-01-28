/* 


    Copyright © 2014 Dani Poveda.


*/
#include <nds.h>
#include <stdio.h>
#include "warhol.h" // Imagen a cargar para aplicar el efecto.
#include "colores.h" // Selector de colores, para cambiar los colores de la imagen.
#include <maxmod9.h>
#include "soundbank.h" // Añade el audio que se reproducirá en bucle en la ejecución de la aplicación.
#include "soundbank_bin.h"
// void colores(int, int, int, int, u16, uint16, int);
int main(void) {
	consoleDemoInit();
	mmInitDefaultMem((mm_addr)soundbank_bin); // Se carga el audio a reproducir.
	mmLoad(MOD_STW);
	mmStart(MOD_STW, MM_PLAY_LOOP);
	// Configuración de pantalla principal y secundaria.
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG0_ACTIVE;
	// Activo los bancos de RAM virtual A, B y C.
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
	VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_BG;
  	VRAM_C_CR = VRAM_ENABLE	| VRAM_C_SUB_BG;
	// Configuración de los fondos de las dos pantallas.
	BGCTRL[2] = BG_MAP_BASE(0) | BgSize_B8_512x512;
  	BGCTRL_SUB[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);
	// Matriz de transformación.
	bgTransform[2]->xdx = 512;	
	bgTransform[2]->ydx = 0;
	bgTransform[2]->xdy = 0;
	bgTransform[2]->ydy = 512;
	bgTransform[2]->dx = 0;
	bgTransform[2]->dy = 0;
	// Copio el mapa, paleta y teselas del selector de colores a memoria.
  	memcpy(BG_PALETTE_SUB, coloresPal, coloresPalLen);
  	memcpy((void*)BG_MAP_RAM_SUB(0), coloresMap, coloresMapLen);
  	memcpy((void*)BG_TILE_RAM_SUB(1), coloresTiles, coloresTilesLen);
	u16 pal[] = {
	/* Columna 1 */	RGB15(31,31,31), RGB15(25,25,25), RGB15(20,20,20), RGB15(15,15,15), RGB15(10,10,10), RGB15(0,0,0),
	/* Columna 2 */	RGB15(31,25,25), RGB15(31,15,15), RGB15(31,5,5),   RGB15(25,0,0),   RGB15(20,0,0),   RGB15(10,0,0),
	/* Columna 3 */	RGB15(31,25,15), RGB15(31,15,5),  RGB15(31,10,0),  RGB15(25,10,0),  RGB15(20,8,0),   RGB15(10,4,0),
	/* Columna 4 */	RGB15(31,31,20), RGB15(31,31,5),  RGB15(28,28,0),  RGB15(24,24,0),  RGB15(20,20,0),  RGB15(10,10,0),
	/* Columna 5 */	RGB15(25,31,25), RGB15(15,31,15), RGB15(5,31,5),   RGB15(0,25,0),   RGB15(0,20,0),   RGB15(0,10,0),
	/* Columna 6 */	RGB15(20,31,31), RGB15(5,31,31),  RGB15(0,28,28),  RGB15(0,24,24),  RGB15(0,20,20),  RGB15(0,10,10),
	/* Columna 7 */	RGB15(25,25,31), RGB15(15,15,31), RGB15(5,5,31),   RGB15(0,0,25),   RGB15(0,0,20),   RGB15(0,0,10),
	/* Columna 8 */	RGB15(31,20,31), RGB15(31,5,31),  RGB15(28,0,28),  RGB15(24,0,24),  RGB15(20,0,20),  RGB15(10,0,10),
	};
	int i=0, j=0;
	static uint8 mapa[256*256]; // Este vector auxiliar contendrá la imagen que será impresa 4 veces en el fondo.
	static uint16 *selector = BG_MAP_RAM_SUB(0);
	static uint8 *memoria = warholBitmap;
	uint8 rojo=0, verde=0, azul=0, gris=0;
	uint16 pixel;
	// Este bucle 'for' obtiene el valor de gris de cada canal de cada píxel, y compara con el valor 20 para convertir 		   la imagen a solamente dos colores.
	for(i=0; i<256*256; i++) {
		pixel = warholPal[memoria[i]];
		rojo = pixel & 31;
		verde = (pixel >> 5) & 31;
		azul = (pixel >> 10) & 31;
		gris = rojo*0.3 + verde*0.59 + azul*0.11;
		if(gris>20)
			mapa[i]=0;
		else
			mapa[i]=1;
	}
	// Colores iniciales del efecto Warhol.
	BG_PALETTE[0]=pal[8];
	BG_PALETTE[1]=pal[11];
	BG_PALETTE[2]=pal[20];
	BG_PALETTE[3]=pal[43];
	BG_PALETTE[4]=pal[15];
	BG_PALETTE[5]=pal[41];
	BG_PALETTE[6]=pal[24];
	BG_PALETTE[7]=pal[47];
	// Algoritmo Warhol. Imprime una matriz de 2x2 imágenes con distintas configuraciones de color.
	for(i=0; i<256; i++) {
		DC_FlushRange(&mapa[i*256], 256); //  Actualizo la caché para asegurar que trabajo con los
						 //   datos deseados en cada una de las iteraciones del bucle.
		dmaCopy(&mapa[i*256], BG_GFX+(i*256), 256); // Copia el primer mapa, y así con los restantes.
		for(j=0; j<256; j++)
			mapa[i*256+j] += 2; // Incrementa la paleta para cambiar el color de la siguiente copia.
		dmaCopy(&mapa[i*256], (BG_GFX+128)+(i*256), 256);
		for(j=0;j<256; j++)
			mapa[i*256+j] += 2;
		dmaCopy(&mapa[i*256], (BG_GFX+(256*256))+(i*256), 256);
		for(j=0; j<256; j++)
			mapa[i*256+j] += 2;
		dmaCopy(&mapa[i*256], (BG_GFX+(256*256)+128)+(i*256), 256);
		for(j=0; j<256; j++)
			mapa[i*256+j] += 2;
	}
	int x=512, y=512, iniciox=0, inicioy=0, finx=0, finy=0; // Declaro las variables que necesitaré para
							   //  el control de teclas.
	static int k=512; // Velocidad del zoom.
	for(;;) {
		scanKeys();
    		unsigned keys = keysCurrent(); 
    		unsigned held = keysHeld();
    		unsigned up = keysUp();
    		unsigned down = keysDown();
    		touchPosition touch;
    		// La tecla L disminuye el zoom.
		if(keys & KEY_L && k<=512) {
      			bgTransform[2]->xdx = k;
      			bgTransform[2]->ydy = k;
			k++;
     		}	 
		// La tecla R aumenta el zoom.
		if(keys & KEY_R && k>=0) {
          		bgTransform[2]->xdx = k;
      			bgTransform[2]->ydy = k;
			k--;
		}
		// Teclas de dirección, que desplazan la imagen hacia la dirección deseada.
		// Hacia la izquierda.
		if(keys & KEY_LEFT && x>=0) {
      			bgTransform[2]->dx = x;	
			x -= 1024;
		}
		// Hacia la derecha.
		if(keys & KEY_RIGHT && x<=65536) {
      			bgTransform[2]->dx = x;
			x += 1024;
		}
		// Hacia arriba.
		if(keys & KEY_UP && y>=0) {
	      		bgTransform[2]->dy = y;
      			y -= 1024;
		}
		// Y hacia abajo.
		if(keys & KEY_DOWN && y<=65536) {
			bgTransform[2]->dy = y;
			y += 1024;
	    	}
		// Control de la pantalla táctil.
	 	if(down & KEY_TOUCH) {
      			touchRead(&touch);
      			iniciox = (touch.px-8)/24;
      			inicioy = (touch.py-8)/24;
    		}
    		if(held & KEY_TOUCH){
      			touchRead(&touch);
      			finx = (touch.px-8)/24;
      			finy = (touch.py-8)/16;
    		}
		// Lo que hacer una vez arrastrado y levantado el puntero de la pantalla táctil.
    		if(up & KEY_TOUCH) {
      			if(iniciox<8 && inicioy<6) {
				if(finx==9 && finy<8) {
					colores(iniciox, inicioy, finx, finy, pal, selector, k);
				}
			}
   		}
		swiWaitForVBlank();
	}
	return 0;
}
// El método "colores" copia las teselas de la celda seleccionada a la celda destino, y aplica el cambio en el fondo Warhol.
void colores(int iniciox, int inicioy, int finx, int finy, u16 pal[], uint16 *selector, int k) { 
  	int inicio = (inicioy*32*3)+(iniciox*3+33);
  	int fin = (finy*32*2)+(finx*3+33);
	int a = (iniciox*6)+inicioy;
  	for(k=0; k<3; k++) {
      		selector[fin+k] = selector[inicio+k];
      		selector[fin+k+32] = selector[inicio+k+64]; // Cambiamos el color de las celdas destino.
	}	
	BG_PALETTE[(int) finy]=pal[a]; // Cambiamos el color del Warhol
}
