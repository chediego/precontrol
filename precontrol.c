#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXARCH 10
#define LARGONOMB 25
#define MENORSUELDO -1
#define MAYORSUELDO 99999999
#define HORADIARIA 8
#define LINEAEMPLEADO 47
#define SALTOCARGO 40
#define SALTONOMBRE 10

struct nodo
{ char rut[11];
  int sueldo, horasextras, horas;
  int pos;
  struct nodo *sgte;
};

void Cerrar_Archivos(FILE **idarchivos, int nroarchivos);
int Comprobar_Existencia_Archivos(FILE **idarchivos, char nombres[MAXARCH][LARGONOMB], int nroarchivos);
void Busqueda_En_Archivo(FILE *archivo, int salto, char *string, int largostring);
struct nodo *Inicializa_Lista(int valormayor, int valormenor);
void Agregar_A_Lista(struct nodo *inicio, struct nodo datos);
void Liberar_Espacio_Lista( struct nodo *inicio);
void Calcular_Horas(FILE **idarchivos, struct nodo *inicio);
void Calcular_Sueldos(FILE **idarchivos, struct nodo *inicio);
void Ordenar_Lista(struct nodo *inicio);
void Imprimir_Informe(FILE **idarchivos, struct nodo *inicio);
void Imprimir_Valores_Lista(struct nodo *inicio);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
main()
{
	FILE *archivos[MAXARCH];
	struct nodo *cabecera;
	char nombresarchivos[MAXARCH][LARGONOMB] = {"empleados.dat","cargos.dat","asistencias.dat"};

	if( Comprobar_Existencia_Archivos(archivos, nombresarchivos, 3) )
	{ 
	  	cabecera = Inicializa_Lista(MAYORSUELDO,MENORSUELDO);
	  	Calcular_Horas(archivos, cabecera);
	  	Calcular_Sueldos(archivos, cabecera);
	  	Ordenar_Lista(cabecera);
	  	Imprimir_Informe(archivos, cabecera->sgte);	  	
	    Cerrar_Archivos(archivos, 3);
	  	Liberar_Espacio_Lista(cabecera);
	  	
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cerrar_Archivos(FILE **idarchivos, int nroarchivos)
{	int i;
	for(i=0; i<nroarchivos; i++)
	{	fclose(idarchivos[i]);
	}
}

int Comprobar_Existencia_Archivos(FILE **idarchivos, char nombres[MAXARCH][LARGONOMB], int nroarchivos)
{	int i;
	i=0;
	while((i<nroarchivos)&& ((idarchivos[i]=fopen(nombres[i], "r"))!=NULL))
	{	i++;
	};
	if(i == nroarchivos)
	{	return(1);
	}
	else
	{	printf("El archivo %s no existe!\n",nombres[i]);
		Cerrar_Archivos(idarchivos, i);
		return(0);
	}
}
struct nodo *Inicializa_Lista(int valormayor, int valormenor)
{	struct nodo *auxtemp;

	auxtemp = malloc(sizeof(struct nodo));
	auxtemp->sueldo = valormayor;
	auxtemp->sgte= malloc(sizeof(struct nodo));
	auxtemp->sgte->sueldo=valormenor;
	auxtemp->sgte->sgte= NULL;
	return(auxtemp);
}
void Busqueda_En_Archivo(FILE *archivo, int salto, char *string, int largostring)
{	char cantcaracteres[6];
	char stringleido[largostring+1];
	sprintf(cantcaracteres, "%%%ds", largostring); 
	rewind(archivo);
	fscanf(archivo, cantcaracteres, stringleido);
	while(strcasecmp(string, stringleido) != 0)
	{	fseek(archivo, salto, SEEK_CUR); 
		fscanf(archivo, cantcaracteres, stringleido);
	}
}
void Agregar_A_Lista(struct nodo *inicio, struct nodo datos)
{   struct nodo *previo, *aux;

    previo=inicio;
    while(previo->sgte->sgte!=NULL)
    { previo=previo->sgte;
    }
    aux=malloc(sizeof(struct nodo));
    strcpy(aux->rut, datos.rut);
    aux->horas=datos.horas;
    aux->horasextras=datos.horasextras;
    aux->pos = datos.pos;
    aux->sgte=previo->sgte;
    previo->sgte=aux;
}
void Liberar_Espacio_Lista( struct nodo *inicio)
{ struct nodo *aux;
 
   aux=inicio;
   while( aux->sgte != NULL)
   { inicio = inicio->sgte;
      free(aux);
      aux=inicio;
   }
   free(inicio); 
}
void Calcular_Horas(FILE **idarchivos, struct nodo *inicio)
{	struct nodo datos;
    char rutaux[11];
    int llegada, salida, pos, fecha;
  
    pos=0;
    
    fscanf(idarchivos[0],"%10s",datos.rut);
	while(!feof(idarchivos[0]))
	{  fseek(idarchivos[0],37,SEEK_CUR); 
	   datos.horas=0;
       datos.horasextras=0;
	   
	   rewind(idarchivos[2]);
	   fscanf(idarchivos[2],"%6d",&fecha);
	   while(!feof(idarchivos[2]) )
	   { fscanf(idarchivos[2],"%10s",rutaux); 
	     if( strcasecmp(datos.rut, rutaux)==0 )
	     {  fscanf(idarchivos[2],"%2d%2d",&llegada,&salida);
	        if( (salida-llegada)<=HORADIARIA ) 	          
	        { 
	      	 	datos.horas= datos.horas+ (salida-llegada);	      
	        }else
	        { 
	        	datos.horas= datos.horas+HORADIARIA;
	          	datos.horasextras= datos.horasextras+(salida-llegada)-8;
	        }
	     }else
	     { fseek(idarchivos[2],5,SEEK_CUR);
	     } 
	    fscanf(idarchivos[2],"%6d",&fecha);
	   }
	   datos.pos=pos;
	   
	   Agregar_A_Lista(inicio, datos);
	   pos++;
	   
	   fscanf(idarchivos[0],"%10s",datos.rut);  
	}
}
void Imprimir_Valores_Lista(struct nodo *inicio)
{	while(inicio->sgte != NULL)
	{	printf("%s %2d %2d %6d\n", inicio->rut, inicio->horas, inicio->horasextras, inicio->sueldo);
		inicio = inicio->sgte;
	}
}
void Calcular_Sueldos(FILE **idarchivos, struct nodo *inicio)
{ struct nodo *aux;
  char codcargo[7];
  int valorhora;
  
  aux=inicio->sgte;
  while(aux->sgte != NULL)
  { rewind(idarchivos[0]); 
    fseek(idarchivos[0], (LINEAEMPLEADO*aux->pos)+SALTOCARGO , SEEK_CUR);
    fscanf(idarchivos[0],"%6s",codcargo); 
    Busqueda_En_Archivo(idarchivos[1], 37, codcargo, 6);
    fseek(idarchivos[1],30,SEEK_CUR); 
    fscanf(idarchivos[1],"%6d",&valorhora);
    aux->sueldo= (aux->horas)*valorhora + (aux->horasextras)*(valorhora*2);
    aux=aux->sgte; 
  }

}
void Ordenar_Lista(struct nodo *inicio)
{	struct nodo *previomenor, *recorrido, *auxmenor;

	while(inicio->sgte->sgte->sgte != NULL)
	{	previomenor = inicio;
		recorrido = inicio->sgte;
		while(recorrido->sgte->sgte != NULL)
		{	if(recorrido->sgte->sueldo > previomenor->sgte->sueldo)
			{	previomenor = recorrido; 
			}else
			{ if( (recorrido->sgte->sueldo == previomenor->sgte->sueldo) && (recorrido->sgte->horasextras < previomenor->sgte->sueldo) )
			  { previomenor = recorrido;
			  }
			}
			recorrido = recorrido->sgte;
		}
		if(inicio != previomenor)
		{	auxmenor = previomenor->sgte;
			previomenor->sgte = previomenor->sgte->sgte;
			auxmenor->sgte = inicio->sgte;
			inicio->sgte = auxmenor;
		}
		inicio = inicio->sgte;
	}
}
void Imprimir_Informe(FILE **idarchivos, struct nodo *inicio)
{
   FILE *informe;
   char nombre[16], apellido[16];
   int totalextras, totalsueldo;
   
   totalextras=0;
   totalsueldo=0;
   
   informe=fopen("listadodesueltos.txt","w");
   fprintf(informe,"==================LISTADO DE SUELDOS DE EMPLEADOS========================\n\n");
   fprintf(informe,"RUT        APELLIDO        NOMBRE          HRS.NORM HRS.EXTR  SUELDO\n");   
   while(inicio->sgte != NULL)
   { 
    rewind(idarchivos[0]);
    fseek(idarchivos[0], LINEAEMPLEADO*inicio->pos+SALTONOMBRE, SEEK_CUR);
    fscanf(idarchivos[0],"%15s%15s",nombre,apellido);
    fprintf(informe,"%10s %-15s %-15s %8d %8d %7d\n",inicio->rut,apellido,nombre,inicio->horas,inicio->horasextras,inicio->sueldo);
    totalextras=totalextras+inicio->horasextras;
    totalsueldo=totalsueldo+inicio->sueldo; 
    inicio=inicio->sgte;
   }       
   fprintf(informe,"-----------------------------------------------------------------------\n");
   fprintf(informe,"                                               TOTALES: %3d %8d",totalextras,totalsueldo);



}

