/* 
 * read_pas.cpp: Código para lectura de archivos .paa
 * provenientes de la captura de datos de un WCD con la
 * utilidad Pulse Acquire para Red Pitaya. 
 * 
 * Héctor Pérez, marzo 2021.
 * 
 * Para el curso de Laboratorio Avanzado
 * ECFM - USAC
 * 
 */

#include <cstdio>
#include <vector>

#include "TGraph.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TMath.h"

/* Header de la clase paa_file que maneja los archivos .paa */
#include "paa.hpp"

/* Header de la clase largs que maneja los argumentos de línea */
#include "largs.hpp"

int main(int argc, char* argv[])
{
    /* Primero se crea una instancia de la clase que hace el 
     * "parsing" de los argumentos de línea proporcionados 
     * al programa. */ 
    largs cmd_args(argc,argv);

    /* Si los argumentos de la línea de comandos no son adecuados se 
     * interrumpe la ejecución delprograma. */
    if( !cmd_args.Args_Ok() )
        return 1;

    /* Se asocia el archivo de entrada provisto en el argumento
     * de línea con una instancia de la clase paa_file. */
    paa_file mi_archivo( cmd_args.GetLArgInputFile() );

    /* Se verifica si la asociación del archivo fue exitosa, 
     * Si no, se sale del programa */
    if(!mi_archivo.Is_Open())
        return 1;

    /* Se crean dos vectores de enteros para almacenar los datos
     * del pulso que se va leer desde el archivo .paa */
    std::vector<int> * pulseData_v;
    std::vector<int> * pulseData_t = new std::vector<int>;

    /* Se obtienen los datos del pulso requerido. El pulso que a
     * obtener se especifica en la linea de comandos. */


    

    int vref = -270;
    int entr = mi_archivo.GetPulseEntries(); 
    int tmp1=0, tmp2=0;
    int cnt=0;
    int numDobpico = 0;
    TH1D *aHist = new TH1D("h1","Tiempos de decaimiento detectados",30,0,0);
    

    

    for (int j = 0; j < entr; j++)
    {  
        pulseData_v = mi_archivo.GetPulse( j );

    /* Se verifica que se hayan obtenido correctamente los datos
     * del pulso. */
        if(!pulseData_v)
            return 1;
        tmp1=0; 
        tmp2=0;
        cnt=0;
        for (int i = 0; i < pulseData_v->size() ; i++)
        {   
            if((pulseData_v->at(i)) < vref)
            {
                if(cnt==0)
                {
                    tmp1 = i;                
                    cnt = cnt + 1;
                    i=i+15; 
                }else if (cnt==1)
                {
                    tmp2 = i;                
                    cnt = cnt + 1;                    
                    i = entr + 1;
                }
            }
        }
        if(cnt==2)
        {
            numDobpico++;
            std::cout<< "Pulso #"<<j<<"\tDt = "<<((tmp2 - tmp1)*8)<<" ns\t"<<  numDobpico <<std::endl;
            aHist->Fill((tmp2 - tmp1)*8);
        }
        
    }
    // TF1 *fitExp = new TF1("fitExp","[0]*TMath::Exp(-x/[1])+[2]");
    // fitExp->SetParNames("A","T","C");
    // fitExp->SetParameters(10,220,10);

    // aHist->Fit("fitExp");
   
    
    // /* Se crea un canvas para alojar la gráfica */
	// TCanvas *ShowGraph = new TCanvas("aHist", "RP aqcuire tools", 600, 400);
    // aHist->Draw();
    
    // ShowGraph->Update();
    // /* Se almacena la gráfica en un archivo .pdf cuyo nombre se obtiene
    //  * de los argumentos de la línea de comando. */
	// ShowGraph->SaveAs( (cmd_args.GetLArgOutputFile()+".pdf").c_str() ,"pdf");
    
    /* Se crea un archivo .root que almacena el histograma para luego ser sumado con hadd en bash*/
    TFile *aFile= new TFile( (cmd_args.GetLArgOutputFile()+".root").c_str(),"RECREATE");

    aHist->Write();
    aFile->Close();
    
    return 0;

}

