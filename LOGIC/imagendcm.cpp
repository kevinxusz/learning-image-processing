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
    statusDcmFileFormat = fileformat.loadFile(fileName);

    if (dicomImage != NULL)
    {
        if (dicomImage->getStatus() == EIS_Normal)
        {
            this->identification="DCM";
            this->imageType="DCM";
            this->colorDepth=pow(2,(dicomImage->getDepth()-1))-1;
            this->height=dicomImage->getWidth();
            this->width=dicomImage->getHeight();


            qDebug() <<"DICOM depth: "<<dicomImage->getDepth();
            qDebug() <<"Color Depth: "<<colorDepth;
            qDebug()<< "frameCount: "<<dicomImage->getFrameCount();

            dicomImage->getMinMaxValues(minDensity, maxDensity);

            qDebug()<<"Min Value: "<<minDensity;
            qDebug()<<"Max Value: "<<maxDensity;

            //Lookup Table
            lutSize=abs(minDensity)+abs(maxDensity)+1;
            QTextStream (stdout)<<"lut size"<<lutSize<<endl;

            lut = new int [lutSize];
            for (int i = 0; i < lutSize; ++i){
                lut[i]=i-abs(minDensity);
//                qDebug()<<"LUT["<<i<<"]: "<<lut[i];
            }

            matrixImagenP = new int**[height];
            for (int i=0; i < height; i++)
                matrixImagenP[i]=new int*[width];

            for(int i=0; i<height; i++){
                for(int j=0; j<width; j++){
                    matrixImagenP[i][j]=&lut[getDensity(j,i)+abs(minDensity)];
                }
            }

            applyWindowLevel(400,40);
            generateHistogram();
            generateQImage();

            status = true;
        } else
            status=false;
    }

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

    // Reset the lut Table;
    for (int i = 0; i < lutSize; ++i){
        lut[i]=i-abs(minDensity);
    }

    for (int i = 0; i < lutSize; ++i){
        lut[i]=pixelWindowLevel(lut[i], window, level);
    }
    generateQImage();
}

int ImagenDCM::pixelWindowLevel(int pixelValue, int window, int center){

    double ymin = 0;
    double ymax = 255;
    double y;

    if(pixelValue <= center - 0.5 - (window-1)/2)
        y = ymin;
    else if (pixelValue > center - 0.5 + (window-1)/2)
        y = ymax;
    else
        y = ((pixelValue - (center - 0.5)) / (window-1) + 0.5) * (ymax - ymin)+ ymin;

    return y;
}

int ImagenDCM::appyCalibrationFunction(int pixelValue, int rescaleSlope, int rescaleIntercept){ // Not it use!
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
    histogram = new Histogram(height, width, 255, matrix);
}

// export
void ImagenDCM::saveImage(QString filename){

    if (!filename.contains(".pgm")) {
        filename=filename.section(".",0,0)+".pgm";
    }

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


