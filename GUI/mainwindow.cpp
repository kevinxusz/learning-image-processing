//**********************************************************
// INTRODUCCION AL PROCESAMIENTO DIGITAL DE IMÁGENES
// LEARNING_IMAGE_PROCESSING
//
// ARCHIVO: mainwindow.cpp
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    mainController= new MainController();
    ui->widget_options=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Buttons Events
void MainWindow::on_pButton_LoadImage_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "../LEARNING_IMAGE_PROCESSING/IMAGES", tr("Image Files (*)"));
    if (mainController->loadImage(filename)) {

        // Changes on PushButtons:
        ui->pButton_LoadImage->setEnabled(false);
        ui->pButton__AdjustImageSize->setEnabled(true);
        ui->pButton__NormalSize->setEnabled(true);

        // Changes on labels
        ui->label_Density->setEnabled(true);
        ui->label_Dimensions->setEnabled(true);
        ui->label_ImageType->setEnabled(true);

        // Set text on Labels with image info
        ui->label_DimensionsValue->setText("W: "+QString::number(mainController->getImage()->getColumnNumber())+"P"+
                                           "  H: "+QString::number(mainController->getImage()->getRowNumber())+"P");
        ui->label_DensityValue->setText(QString::number(log2(mainController->getImage()->getColorDensity()+1))+" Bits");
        ui->label_ImageTypeValue->setText(mainController->getImage()->getImageType());

        // Set Image to label_Imagen
        displayedImage=mainController->getQImage();

        // Display Image in Original Size
        on_pButton__NormalSize_clicked();

        ShowHistogram();

    } else {
        QMessageBox msgBox(this);
        msgBox.setText("Sorry, but the selected file is not supported");
        msgBox.exec();
    }
}

void MainWindow::on_pButton__AdjustImageSize_clicked()
{
    ui->label_Imagen->setGeometry(QRect(0, 0, 733, 550));
    ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, 733, 550));
    ui->label_Imagen->setPixmap(QPixmap::fromImage(this->displayedImage->scaled(QSize(733, 550),Qt::KeepAspectRatio)));
}

void MainWindow::on_pButton__NormalSize_clicked()
{

    if(displayedImage->width()>ui->label_Imagen->width() && displayedImage->height()>ui->label_Imagen->height()){
        ui->label_Imagen->setGeometry(QRect(0, 0, displayedImage->width(), displayedImage->height()));
        ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, displayedImage->width(), displayedImage->height()));
    }else if (displayedImage->width()>ui->label_Imagen->width()) {
        ui->label_Imagen->setGeometry(QRect(0, 0, displayedImage->width(), ui->label_Imagen->height()));
        ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, displayedImage->width(), ui->scrollAreaWidgetContents->height()));
    }else if (displayedImage->height()>ui->label_Imagen->height()) {
        ui->label_Imagen->setGeometry(QRect(0, 0, ui->label_Imagen->width(), displayedImage->height()));
        ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, ui->scrollAreaWidgetContents->width(), displayedImage->height()));
    }else{
        ui->label_Imagen->setGeometry(QRect(0, 0, 733, 550));
        ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, 733, 550));
    }
    ui->label_Imagen->setPixmap(QPixmap::fromImage(*displayedImage));
}

// MenuBar Events

// File Menu
void MainWindow::on_actionNew_Job_triggered()
{
    // Changes on PushButtons:
    ui->pButton_LoadImage->setEnabled(true);
    ui->pButton__AdjustImageSize->setEnabled(false);
    ui->pButton__NormalSize->setEnabled(false);

    // Changes on labels
    ui->label_Density->setEnabled(false);
    ui->label_Dimensions->setEnabled(false);
    ui->label_ImageType->setEnabled(false);

    // changes in the text on Labels:
    ui->label_DensityValue->setText("");
    ui->label_DimensionsValue->setText("");
    ui->label_ImageTypeValue->setText("");

    // Erase Image
    ui->label_Imagen->setPixmap(0);
    ui->label_Histogram->setPixmap(0);
    ui->label_Imagen->setGeometry(QRect(0, 0, 733, 550));
    ui->scrollAreaWidgetContents->setGeometry(QRect(0, 0, 733, 550));

    // delete widget_options
    ui->widget_options->deleteLater();

    mainController->newJob();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

// Edit Menu
void MainWindow::on_actionUndo_triggered()
{
    if (mainController->undo()) {
        displayedImage=mainController->getQImage();
        on_pButton__NormalSize_clicked();
    }else{
        QMessageBox msgBox2(this);
        msgBox2.setText("Sorry, but pero no se puede ir pa tras");
        msgBox2.exec();
    }

}

// Preprocessing Menu
void MainWindow::on_actionResize_triggered()
{
    if (ui->widget_options!=0) {
        ui->widget_options->deleteLater();
    }
    ui->widget_options = new ResizeQwidget(ui->centralWidget, mainController, this);
    ui->widget_options->setObjectName(QString::fromUtf8("widget_resize"));
    ui->widget_options->setGeometry(QRect(770, 70, 270, 331));
    ui->widget_options->setVisible(true);

}

void MainWindow::on_actionChange_Color_Depth_triggered()
{

    if (ui->widget_options!=0) {
        ui->widget_options->deleteLater();
    }
    ui->widget_options = new ColorDepthQwidget(ui->centralWidget, mainController, this);
    ui->widget_options->setObjectName(QString::fromUtf8("widget_colorDepth"));
    ui->widget_options->setGeometry(QRect(770, 70, 270, 331));
    ui->widget_options->setVisible(true);
}

void MainWindow::on_actionConver_to_GrayScale_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setText("do you want to give the same weight to all color channels?");
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int method = msgBox.exec();
    QImage *result=mainController->convertToGrayscale(method);

    if (result!=0) {
        displayResults(result);
        ShowHistogram();
    } else {
        QMessageBox msgBox2(this);
        msgBox2.setText("Sorry, but the conversion is just for color images");
        msgBox2.exec();
    }
}

// Histogram Menu

// Tomography Menu

// Help Menu
void MainWindow::on_actionAbout_triggered()
{
    DialogAbout about(this);
    about.setModal(true);
    about.exec();
}


// Other Methods
void MainWindow::displayResults(QImage *result)
{
    displayedImage=result;
    on_pButton__NormalSize_clicked();
    ui->label_DimensionsValue->setText("W: "+QString::number(mainController->getImage()->getColumnNumber())+"P"+
                                       "  H: "+QString::number(mainController->getImage()->getRowNumber())+"P");
    ui->label_DensityValue->setText(QString::number(log2(mainController->getImage()->getColorDensity()+1))+" Bits");
    ui->label_ImageTypeValue->setText(mainController->getImage()->getImageType());
}

void MainWindow::ShowHistogram(){
    if (mainController->getImage()->getImageType().toUpper()=="PGM") {
        histogram = mainController->generateHistogram();
        ui->label_Histogram->setPixmap(QPixmap::fromImage(histogram->scaled(QSize(250,100), Qt::IgnoreAspectRatio)));
    }
}


