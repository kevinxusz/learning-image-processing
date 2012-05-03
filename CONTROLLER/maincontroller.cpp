//**********************************************************
// INTRODUCCION AL PROCESAMIENTO DIGITAL DE IMÁGENES
// LEARNING_IMAGE_PROCESSING
//
// ARCHIVO: maincontroller.cpp
//
// FECHA INICIACION: Marzo de 2012
//
// AUTORES:
// Gustavo Adolfo Rodriguez         0932979-3743
// gustalibreros@hotmail.com
//
// Edwin Fernando Muñoz             0910398-3743
// edwinfernandomudelgado@hotmail.com
//
// ESCUELA DE INGENIERIA DE SISTEMAS Y COMPUTACION
// UNIVERSIDAD DEL VALLE
//**********************************************************

#include "maincontroller.h"

MainController::MainController(){
    this->imagen=0;
    this->oldImage=0;
    this->displayedImage=0;
    this->oldDisplayedImage=0;
}

MainController::~MainController(){    
}

// LOAD IMAGE
bool MainController::loadImage(QString filename){

    ImageFile archivo(filename);
    if(archivo.readImageContents()){
        QString imageType = filename.right(3).toUpper();

        if(imageType == "PPM"){
            imagen = new ImagenPPM(archivo.getImageContents());
        }else{
            imagen = new ImagenPGM(archivo.getImageContents());
        }
        displayedImage=new QImage(filename);
        return true;
    }else{
        return false;
    }
}

QImage* MainController::generateHistogram(){
    histogram = new Histogram (static_cast<ImagenPGM*>(imagen));
    ImagenPGM *imageHistogram=histogram->getHistogram();
    imageHistogram->exportar("histogram");
    return new QImage("histogram.pgm");
}

// Image Transfomations
bool MainController::changeSize(int density){

    if(imagen!=0){
        oldImage=imagen;
        imagen=oldImage->changeSize(density);
        imagen->exportar("temp");
        oldDisplayedImage=displayedImage;
        displayedImage=new QImage("temp."+imagen->getImageType().toLower());
        return true;
    }else{
        return false;
    }
}

bool MainController::changeColorDepth(int depth){
    if(imagen!=0){
        oldImage=imagen;
        imagen=oldImage->changeColorDepth(depth);
        imagen->exportar("temp");
        oldDisplayedImage=displayedImage;
        displayedImage=new QImage("temp."+imagen->getImageType().toLower());
        return true;
    }else{
        return false;
    }
}

void MainController::convertToGrayscale(int method){

    oldImage=imagen;
    imagen=static_cast<ImagenPPM*>(oldImage)->convertToGrayScale(method);
    imagen->exportar("temp");
    oldDisplayedImage=displayedImage;
    displayedImage=new QImage("temp."+imagen->getImageType().toLower());
}

bool MainController::bimodalSegmentaion(int T){
    if(imagen!=0){
        oldImage=imagen;
        imagen=static_cast<ImagenPGM*>(oldImage)->bimodalSegmentaion(T);
        imagen->exportar("temp");
        oldDisplayedImage=displayedImage;
        displayedImage=new QImage("temp."+imagen->getImageType().toLower());
        return true;
    }else{
        return false;
    }
}

bool MainController::isodataSegmentation(){
    return bimodalSegmentaion(histogram->calculateThresholdIsodata());
}

// Getters
Image* MainController::getImage(){
    return imagen;
}
QImage* MainController::getQImage(){
    return displayedImage;
}

// Other Methods
void MainController::newJob(){
    imagen=0;
    oldImage=0;
    displayedImage=0;
    oldDisplayedImage=0;

    delete[] matrixImagenP;
    delete[] lut;

    for (int i=0; i < imagen->height; i++)
        matrixImagenP[i]=0;

    for(int i=0; i<imagen->height; i++){
        for(int j=0; j<imagen->width; j++){
            matrixImagenP[i][j]=0;
        }
    }
}

bool MainController::undo(){
    if (oldImage!=0) {
        imagen=oldImage;
        displayedImage=oldDisplayedImage;
        return true;
    }else{
        return false;
    }

}

