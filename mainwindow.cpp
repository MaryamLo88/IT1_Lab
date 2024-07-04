/**
  * @file mainwindow.cpp
  * @brief This file contains the function definitions for MainWindow class.
  */

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QRandomGenerator>



#include "mainwindow.h"
#include ".\ui_mainwindow.h"
#include "bigtown.h"
#include "dijkstra.h"
#include "dialognewcity.h"
#include "dialognewstreet.h"
#include "mapiofileinput.h"
#include "mapionrw.h"

/**
 * @brief Constructor for MainWindow class.
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    ui->cbRouteCity1->lineEdit()->setPlaceholderText("Anfangsstadt");
    ui->cbRouteCity2->lineEdit()->setPlaceholderText("Ziel");
    mapio = new MapIoNrw();
}

/**
 * @brief Default-Destructor for MainWindow class.
 * Deletes the ui instance.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Slot for pushButton clicked
 * This function prints out the content of the LineEdit-control.
 * In case the user entered a number, the number gets incremented by 4 and printed out.
 * It adds a square of 10px side-length at a random position (0-200px) on the GraphicsView.
 */
void MainWindow::on_pushButton_clicked()
{
    qDebug() << QString("Benutzereingabe: %1").arg(ui->lineEdit->text());

    bool ok = false;

    int val = ui->lineEdit->text().toInt(&ok);

    if(ok)
    {
        val += 4;
        qDebug() << QString("Die Zahl um 4 erhöht: %1").arg(val);
    }

    int pos_x = QRandomGenerator::global()->bounded(200);
    int pos_y = QRandomGenerator::global()->bounded(200);
    scene.addRect(pos_x, pos_y, 10, 10);

    ui->graphicsView->show();
}

/**
 * @brief Slot for Button testDrawCity clicked
 * This function tests the draw function of the City class by drawing two cities directly on the GraphicsView.
 */
void MainWindow::on_testDrawCity_clicked()
{
    BigTown stadt2 = BigTown("Aachen", 50, 150);
    BigTown stadt1 = BigTown("Dortmund", 100, 100);
    stadt1.draw(scene);
    stadt2.draw(scene);
}

/**
 * @brief Slot for Button testDrawMap clicked
 * This function tests the draw function of the Map class by adding three cities to the map and drawing the map.
 */
void MainWindow::on_testDrawMap_clicked()
{
    map.addCity(new BigTown("Dortmund", 100, 100));
    map.addCity(new BigTown("Aachen", 50, 150));
    map.addCity(new BigTown("Düsseldorf", 80, 120));
    cbRouteCityUpdate();

    map.draw(scene);
}

/**
 * @brief Slot for Button testDrawMapStreets clicked
 * This function tests the Map class by adding three cities and three streets to the map.
 * A fourth street links to a city not being added to the map. It should be rejected.
 * Finally the map is being drawn with the cities and streets.
 */
void MainWindow::on_testDrawMapStreets_clicked()
{
    BigTown* dortmund   = new BigTown("Dortmund",   80,  60);
    BigTown* aachen     = new BigTown("Aachen",     50, 150);
    BigTown* dusseldorf = new BigTown("Düsseldorf", 80, 120);
    BigTown* zurich     = new BigTown("Zürich",    120, 300);

    map.addCity(dortmund);
    map.addCity(aachen);
    map.addCity(dusseldorf);
    cbRouteCityUpdate();

    bool res1 = map.addStreet(new StateRoad(dortmund, dusseldorf));
    bool res2 = map.addStreet(new StateRoad(dusseldorf,   aachen));
    bool res3 = map.addStreet(new StateRoad(aachen,     dortmund));
    bool res4 = map.addStreet(new StateRoad(dusseldorf,   zurich));

    qDebug() << QString("Hinzufügen von   Dortmund - Düsseldorf (+): %1").arg(res1);
    qDebug() << QString("Hinzufügen von Düsseldorf - Aachen     (+): %1").arg(res2);
    qDebug() << QString("Hinzufügen von     Aachen - Dortmund   (+): %1").arg(res3);
    qDebug() << QString("Hinzufügen von Düsseldorf - Zürich     (-): %1").arg(res4);

    map.draw(scene);
}

/**
 * @brief Slot für Button testAbstractMap clicked
 * This function tests if the Map class implements the AbstractMap interface correctly.
 * (Template from L2P)
 */
void MainWindow::on_testAbstractMap_clicked()
{
    BigTown *a = new BigTown("a", 0, 0);
    BigTown *b = new BigTown("b", 0, 100);
    BigTown *c = new BigTown("c", 100, 300);
    StateRoad *s = new StateRoad(a, b);
    StateRoad *s2 = new StateRoad(b, c);


    qDebug() << "Maptest: Test der Karte hat begonnen";
    {
        qDebug() << "Maptest: Strassen ohne Städte";
        bool t1 = map.addStreet(s);
        if (t1) {
            qDebug() << "-Error: Strassen dürfen nicht vor Städten hinzugefügt werden.";
        }
    }

    {
        qDebug() << "MapTest: Strassen nach Städten";
        map.addCity(a);
        map.addCity(b);
        bool t1 = map.addStreet(s);
        if (!t1) {
            qDebug() << "-Error: Diese Strasse soll problemlos hinzugefügt werden können.";
        }
    }

    {
        qDebug() << "MapTest: Stadt finden";
        City* city = map.find_city("a");
        if (city != a)
            qDebug() << "-Error: Stadt a nicht gefunden.";

        city = map.find_city("b");
        if (city != b)
            qDebug() << "-Error: Stadt b nicht gefunden.";

        city = map.find_city("c");
        if (city != NULL)
            qDebug() << "-Error: Keine Städte gefunden.";
    }

    map.addCity(c);
    map.addStreet(s2);

    {
        qDebug() << "MapTest: Zweite Stadt finden (andere Seite)";
        const City *city = map.get_opposite_city(s, a);
        if (city != b)
            qDebug() << "-Error: Stadt auf der anderen Seite sollte b sein.";

        city = map.get_opposite_city(s, c);
        if (city != 0)
            qDebug() << "-Error: Die gegenseitige Stadt von einer Stadt, die nicht mit einer Strasse verbunden ist, sollte den Wert 0 haben.";
    }

    {
        qDebug() << "MapTest: Strassenlaenge";
        double l = map.get_length(s2);
        double expectedLength = 223.6;
        // compare doubles with 5% tolerance
        if (l < expectedLength * 0.95 || l > expectedLength *1.05)
            qDebug() << "-Error: Strassenlaenge ist nicht dem erwarteten Wert gleich.";

    }

    {
        qDebug() << "MapTest: Strassenliste aufrufen";
        AbstractMap::StreetList streetList1 = map.get_street_list(a);
        AbstractMap::StreetList streetList2 = map.get_street_list(b);
        if (streetList1.size() != 1) {
            qDebug() << "-Error: Eine Strasse zu Stadt a soll existieren.";
        }
        else if (*streetList1.begin() != s) {
                qDebug() << "-Error: Die falsche Strasse zu Stadt a gefunden.";
        }

        if (streetList2.size() != 2)
            qDebug() << "-Error: Zwei Strassen zu Stadt b sollen existieren.";
    }

    qDebug() << "MapTest: MapTest beendet.";
}


void MainWindow::on_testDijkstra_clicked()
{
    AbstractMap::StreetList path = Dijkstra::search(map, "Bonn", "Düsseldorf");

    AbstractMap::StreetList::const_iterator is;
    for(is = path.begin(); is != path.end(); is++)
    {
        qDebug() << "Strasse von " << (*is)->getCity1()->getName() << " nach " << (*is)->getCity2()->getName();
        (*is)->drawBlue(scene);
    }
}

/**
 * @brief Slot for Checkbox cbHideTests clicked
 * This function hides all the buttons (and labels, LineEdits) if the CheckBox cbHideTests is checked.
 */
void MainWindow::on_cbHideTests_clicked()
{
    if(ui->cbHideTests->isChecked())
    {
        ui->label->setVisible(false);
        ui->lineEdit->setVisible(false);
        ui->pushButton->setVisible(false);
        ui->testDrawCity->setVisible(false);
        ui->testDrawMap->setVisible(false);
        ui->testDrawMapStreets->setVisible(false);
        ui->testAbstractMap->setVisible(false);
        ui->testDijkstra->setVisible(false);
    }
    else
    {
        ui->label->setVisible(true);
        ui->lineEdit->setVisible(true);
        ui->pushButton->setVisible(true);
        ui->testDrawCity->setVisible(true);
        ui->testDrawMap->setVisible(true);
        ui->testDrawMapStreets->setVisible(true);
        ui->testAbstractMap->setVisible(true);
        ui->testDijkstra->setVisible(true);
    }
}

/**
 * @brief Slot for button cmdAddCity clicked
 * This function asks the user to enter the data of a new city in a dialog and adds the new city to the map.
 */
void MainWindow::on_cmdAddCity_clicked()
{
    DialogNewCity diag;

    int res = diag.exec();
    if(res == QDialog::Accepted) // Wait for dialog to be closed
    {
        map.addCity(diag.getNewCity());
        map.draw(scene);
        cbRouteCityAddItem(diag.getNewCity());
    }
}

/**
 * @brief Slot for button cmdAddStreet clicked
 * This function provides a dialog for the user to choose two cities to connect by a new street.
 * The new Street is being added to the map.
 */
void MainWindow::on_cmdAddStreet_clicked()
{
    DialogNewStreet diag;

    diag.setCities(map.getCities());

    int res = diag.exec();
    if(res == QDialog::Accepted)
    {
        map.addStreet(diag.getNewStreet());
        map.draw(scene);
    }

}


/**
 * @brief Slot for button cmdFillMap clicked
 * This function fills the map by using the MapIoNrw dummy.
 */
void MainWindow::on_cmdFillMap_clicked()
{
    mapio = new MapIoNrw();
    mapio->fillMap(map);
    map.draw(scene);
    cbRouteCityUpdate();
}

/**
 * @brief Slot for button cmdFindRoute clicked
 * This function looks for the two cities named in the corresponding line edits. It calculates a route between these cities by using the dijkstra algorithm.
 * It prints out the distance between two cities on the console.
 */
void MainWindow::on_cmdFindRoute_clicked()
{
    QString start = ui->cbRouteCity1->currentText();
    QString dest = ui->cbRouteCity2->currentText();

    if(start == dest)
    {
        qDebug() << "Startpunkt ist das Ziel.";
        return;
    }

    map.draw(scene); // redraw covering old routes

    AbstractMap::StreetList path = Dijkstra::search(map, start, dest);
    double distance = 0;

    AbstractMap::StreetList::const_iterator is;
    for(is = path.begin(); is != path.end(); is++)
    {
        (*is)->drawBlue(scene);          // draw the route
        distance += map.get_length(*is); // calculate the distance
    }

    if(ui->rbRoutingTime->isChecked())
    {
        QMessageBox::information(this, "Reisezeit", QString("Reisezeit zwischen %1 und %2 beträgt %3h.").arg(start).arg(dest).arg(distance), QMessageBox::Ok);
        qDebug() << "Reisezeit zwischen " << start << " und " << dest << ": " << distance;
    }
    else
    {
        QMessageBox::information(this, "Disatnz", QString("Distanz zwischen %1 und %2 beträgt %3km.").arg(start).arg(dest).arg(distance), QMessageBox::Ok);
        qDebug() << "Distanz zwischen " << start << " und " << dest << ": " << distance;
    }
}

/**
 * @brief Slot for button cmdClearMap clicked
 * This function clears the map and the GraphicsView.
 */
void MainWindow::on_cmdClearMap_clicked()
{
    map = Map();
    map.draw(scene);
    cbRouteCityClear();
}

/**
 * @brief Helper function to fill the ComboBoxes on MainWindow with the given City.
 * @param city City to be added
 */
void MainWindow::cbRouteCityAddItem(City* city)
{
    ui->cbRouteCity1->addItem(city->getName());
    ui->cbRouteCity2->addItem(city->getName());
}

/**
 * @brief Helper function to clear the ComboBoxes on MainWindow (remove all items).
 */
void MainWindow::cbRouteCityClear()
{
    ui->cbRouteCity1->clear();
    ui->cbRouteCity2->clear();
}

/**
 * @brief Helper function to update the ComboBoxes on MainWindow using the cities from the map.
 */
void MainWindow::cbRouteCityUpdate()
{
    cbRouteCityClear();
    AbstractMap::CityList cities = map.getCities();
    AbstractMap::CityList::const_iterator ic;

    for(ic = cities.begin(); ic != cities.end(); ic++)
    {
        cbRouteCityAddItem(*ic);
    }
}

/**
 * @brief Slot for button cmdMapIoFile clicked
 * This function asks the user for two files containing the cities and streets to add to the map.
 */
void MainWindow::on_cmdMapIoFile_clicked()
{
    QString defaultdir  = "C:/Users/XXXXXXXX/OneDrive/Desktop/Praktikum Informatik 1/StreetPlanner/txtFiles";
    QString filecities  = QFileDialog::getOpenFileName(this, "Open Cities File",  defaultdir, "Text files (*.txt)");
    QString filestreets = QFileDialog::getOpenFileName(this, "Open Streets File", defaultdir, "Text files (*.txt)");

    mapio = new MapIoFileinput(filecities, filestreets);
    mapio->fillMap(map);
    map.draw(scene);
    cbRouteCityUpdate();
}

/**
 * @brief Slot for button cmdSaveToDisk clicked
 * This function aks the user for two file to save the cities and streets to.
 */
void MainWindow::on_cmdSaveToDisk_clicked()
{
    QString defaultdir  = "C:/Users/XXXXXXXXXXXX/OneDrive/Desktop/Praktikum Informatik 1/StreetPlanner/txtFiles";
    QString filecities  = QFileDialog::getSaveFileName(this, "Save Cities File",  defaultdir, "Text files (*.txt)");
    QString filestreets = QFileDialog::getSaveFileName(this, "Save Streets File", defaultdir, "Text files (*.txt)");

    mapio = new MapIoFileinput(filecities, filestreets);
    mapio->saveMap(map, filecities, filestreets);
}

/**
 * @brief Slot for radiobutton RoutingDistance clicked
 * This function stores the new routing preference in the Map object.
 */
void MainWindow::on_rbRoutingDistance_clicked()
{
    map.setRoutingPreference(ui->rbRoutingTime->isChecked());
}

/**
 * @brief Slot for radiobutton RoutingTime clicked
 * This function stores the new routing preference in the Map object.
 */
void MainWindow::on_rbRoutingTime_clicked()
{
    map.setRoutingPreference(ui->rbRoutingTime->isChecked());
}
