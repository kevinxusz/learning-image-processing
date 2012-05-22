//**********************************************************
// INTRODUCCION AL PROCESAMIENTO DIGITAL DE IMÁGENES
// LEARNING_IMAGE_PROCESSING
//
// ARCHIVO: image.cpp
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

#include "imagendcm.h"

ImagenDCM::ImagenDCM(const char *fileName){

    dicomImage = new DicomImage(fileName);

    QTextStream cout(stdout);
    if (dicomImage != NULL)
    {
        if (dicomImage->getStatus() == EIS_Normal)
        {
            this->identification="DCM";
            this->imageType="DCM";
            this->colorDepth=pow(2,(dicomImage->getDepth()-1))-1;
            this->height=dicomImage->getWidth();
            this->width=dicomImage->getHeight();

            //Display information about image

            cout <<"DICOM depth: "<<dicomImage->getDepth()<<endl;
            cout <<"Color Depth: "<<colorDepth<<endl;
            cout<< "frameCount: "<<dicomImage->getFrameCount()<<endl;

            dicomImage->getMinMaxValues(minDensity, maxDensity);

            cout<<"Min Value: "<<minDensity<<endl;
            cout<<"Max Value: "<<maxDensity<<endl;

            //Lookup Table
            lutSize=abs(minDensity)+abs(maxDensity)+1;
            QTextStream (stdout)<<"lut size"<<lutSize<<endl;

            lut = new int [lutSize];
            for (int i = 0; i < lutSize; ++i){
                lut[i]=i-abs(minDensity);
                QTextStream (stdout)<<"LUT["<<i<<"]: "<<lut[i]<<endl;
            }

            matrixImagenP = new int**[height];
            for (int i=0; i < height; i++)
                matrixImagenP[i]=new int*[width];

            QTextStream (stdout)<<"Matriz Inicializada"<<endl;

            for(int i=0; i<height; i++){
                for(int j=0; j<width; j++){
                    matrixImagenP[i][j]=&lut[getDensity(j,i)+abs(minDensity)];
                    //                    QTextStream (stdout)<<*matrixImagenP[i][j]<<" ";
                }
                //                QTextStream (stdout) <<""<<endl;
            }
            applyWindowLevel(400,40);
            //            generateHistogram();
            generateQImage();
            saveImage("temp");
            status = true;
        } else
            status=false;
    }
    cout<<"TERMINE"<<endl;
}


int ImagenDCM::getDensity(int x, int y)
{
    int iPos = x + (y*width);
    OFString str;
    const DiPixel *dmp = NULL;
    dmp = dicomImage->getInterData();
    void *pixelData = NULL;
    pixelData = (void *)dmp->getData();

    if(NULL == dmp)
    {
        return 0;
    }

    int result = 0;

    EP_Representation rep = dmp->getRepresentation();
    switch(rep)
    {
    case EPR_Sint32:
    {
        Sint32 *pixelNew1 = ((Sint32*)pixelData+iPos);
        result = *pixelNew1;
        break;
    }
    case EPR_Uint32:
    {
        Uint32 *pixelNew2 = ((Uint32*)pixelData+iPos);
        result = *pixelNew2;
        break;
    }
    case EPR_Sint16:
    {
        Sint16 *pixelNew3 = ((Sint16*)pixelData+iPos);
        result = *pixelNew3;
        break;
    }
    case EPR_Uint16:
    {
        Uint16 *pixelNew4 = ((Uint16*)pixelData+iPos);
        result = *pixelNew4;
        break;
    }
    case EPR_Uint8:
    {
        Uint8 *pixelNew5 = ((Uint8*)pixelData+iPos);
        result = *pixelNew5;
        break;
    }
    default:
    {
        Sint8 *pixelNew6 = ((Sint8*)pixelData+iPos);
        result = *pixelNew6;
        break;
    }
    }

    return result;
}

void ImagenDCM::applyWindowLevel(int window, int level){

    double min = level - ceil(window/2);
    double max = level + floor(window/2);
    double newPixelValue, pixelValue;

    QTextStream cout (stdout);
    cout<<"Min Value: "<<min<<endl;
    cout<<"Max Value: "<<max<<endl;

    for (int i = 0; i < lutSize; ++i){

        if (lut[i]>=min && lut[i]<=max) {
            cout<<"idex: "<<i<<"   value: "<<lut[i]<<" esta dentro de la ventana"<<endl;
            pixelValue=lut[i];
            newPixelValue= (pixelValue-min)*(255/(max-min));
            lut[i]=newPixelValue;
            cout<<"newPixelValue: "<<newPixelValue<<endl;
            cout<<"LUT["<<i<<"]: "<<lut[i]<<endl;
        } else{
            if(lut[i]>max){
                lut[i]=255;
            } else{
                lut[i]=0;
            }
        }
        cout<<"LUT["<<i<<"]: "<<lut[i]<<endl;
    }

    generateQImage();
}

int ImagenDCM::appyCalibrationFunction(int pixelValue, int rescaleSlope, int rescaleIntercept){
    return (pixelValue * rescaleSlope) + rescaleIntercept;
}

//Image processing
Image* ImagenDCM::changeSize(int factor){
    return this;
}

Image* ImagenDCM::changeColorDepth(int bits){
    return this;
}

// GUI Display
void ImagenDCM::generateQImage(){
    QVector<QRgb> colorTable;
    int aux;
    for (int i = 0; i < 256; ++i) {
        colorTable.append(qRgb(i,i,i));
    }

    qImage = new QImage(width, height, QImage::Format_Indexed8);
    qImage->setColorTable(colorTable);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            qImage->setPixel(j,i,*matrixImagenP[i][j]);
        }
    }
}

//Histogram
void ImagenDCM::generateHistogram(){

    int **matrix = new int*[height];
    for (int i=0; i < height; i++)
        matrix[i]=new int[width];

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            matrix[i][j]=*matrixImagenP[i][j];
        }
    }
    histogram = new Histogram(height, width, colorDepth, matrix);
}

// export
void ImagenDCM::saveImage(QString filename){

    if (!filename.contains(".pgm")) {
        filename=filename.section(".",0,0)+".pgm";
    }
    QTextStream (stdout) <<"filename:"<<filename<<endl;
    this->imageType="PGM";
    this->identification="P2";
    QFile temp(filename);
    if(temp.open(QFile::WriteOnly)){
        QTextStream fSalida(&temp);

        fSalida<<identification<<endl;
        fSalida<<"#LEARNING IMAGE PROCESSING by GUSTAVO & EDWIN AT UNIVALLE"<<endl;
        fSalida<<width<<" "<<height<<endl;
        fSalida<<"255"<<endl;
        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                fSalida<<*matrixImagenP[i][j]<<" ";
            }
            fSalida<<endl;
        }
    }
}


