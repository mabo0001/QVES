#include "OldProject.h"
#include "VES.h"
#include <QString>
#include "LocationData.h"
#include <QVariant>

void OldProject::readOldSev(QXmlStreamReader *reader, Project *newProject)
{
    VES newVes(nullptr);
    bool read = true;
    reader->readNextStartElement();

    while (read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "nombre"){
                newVes.setName(reader->readElementText());
            } else if (reader->name() == "vacio") {
                QString empty = reader->readElementText().toLower();
                if (empty == "true"){
                    newProject->addVES(newVes);
                    return;
                }
            } else if (reader->name() == "coord") {
                readOldCoordinates(reader, newVes);
            } else if (reader->name() == "listaPuntoCampo") {
                newVes.setFieldData(readOldListaPuntoCampo(reader, "listaPuntoCampo"));
            } else if (reader->name() == "listaModelos") {
                QList<InversionModel> tempList;
                bool modelRead = true;
                while (modelRead) {
                    if (reader->name() == "listaModelos" && reader->tokenType() == QXmlStreamReader::TokenType::EndElement) {
                        modelRead = false;
                    } else {
                        InversionModel im = readOldModelo(reader);
                        tempList.append(im);
                    }
                    reader->readNext();
                }
                newVes.setModels(tempList);
            } else if (reader->name() == "modeloSeleccionado") {
                QVariant temp = reader->readElementText();
                newVes.setCurrentIndexModel(temp.toInt());
            } else {
                reader->readNextStartElement();
            }
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == "SEV"){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }
    }

    newVes.createSplices();
    for (int i = 0; i < newVes.models().count(); i++) {
        newVes.models()[i].updateModelError(newVes.splices());
    }
    newProject->addVES(newVes);
}

void OldProject::readOldCoordinates(QXmlStreamReader *reader, VES &ves)
{
    LocationData *newLoc = new LocationData(&ves);
    QVariant tempVariant;

    bool read = true;
    reader->readNextStartElement();
    while (read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "coordX"){
                tempVariant = reader->readElementText();
                newLoc->setLocalX(tempVariant.toDouble());
            } else if (reader->name() == "coordY") {
                tempVariant = reader->readElementText();
                newLoc->setLocalY(tempVariant.toDouble());
            } else if (reader->name() == "coordZ") {
                tempVariant = reader->readElementText();
                newLoc->setZ(tempVariant.toDouble());
            } else if (reader->name() == "lat") {
                tempVariant = reader->readElementText();
                newLoc->setDecimalLatitude(tempVariant.toDouble());
            } else if (reader->name() == "lng") {
                tempVariant = reader->readElementText();
                newLoc->setDecimalLongitude(tempVariant.toDouble());
            }
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == "coord"){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }
    }
    ves.setLocation(newLoc);
}

QList<BasicData> OldProject::readOldListaPuntoCampo(QXmlStreamReader *reader, const QString &stopName)
{
    QList<BasicData> list;
    QVariant tempVariant;

    bool read = true;
    reader->readNextStartElement();
    while (read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "puntoDato"){
                reader->readNextStartElement();
                BasicData bd;
                if (reader->name() == "distAB2") {
                    tempVariant = reader->readElementText();
                    bd.setAb2Distance(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "res") {
                    tempVariant = reader->readElementText();
                    bd.setResistivity(tempVariant.toDouble());
                }
                list.append(bd);
            }
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == stopName){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }
    }

    return list;
}

InversionModel OldProject::readOldModelo(QXmlStreamReader *reader)
{
    InversionModel tempModel;
    QVariant tempVariant;

    bool read = true;
    reader->readNextStartElement();
    while (read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "nombre"){
                tempModel.setName(reader->readElementText());
            } else if (reader->name() == "tipo") {
                tempVariant = reader->readElementText();
                if (tempVariant.toInt() == 0) {
                    tempModel.setUsedAlgorithm(InversionModel::InversionAlgorithm::Zohdy);
                } else {
                    tempModel.setUsedAlgorithm(InversionModel::InversionAlgorithm::Vfsa);
                }
            } else if (reader->name() == "listaPuntoCalculado"){
                tempModel.setCalculatedData(readOldListaPuntoCampo(reader, "listaPuntoCalculado"));
            } else if (reader->name() == "listaModeloSEVs"){
                tempModel.setModelData(readOldModeloSEVs(reader));
            }
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == "modelo"){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }
    }

    return tempModel;
}

QList<ModelData> OldProject::readOldModeloSEVs(QXmlStreamReader *reader)
{
    QList<ModelData> list;
    QVariant tempVariant;

    bool read = true;
    reader->readNextStartElement();
    while (read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "puntoModelo"){
                reader->readNextStartElement();
                ModelData md;
                if (reader->name() == "desdeM") {
                    tempVariant = reader->readElementText();
                    md.setFrom(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "hastaM") {
                    tempVariant = reader->readElementText();
                    md.setUntil(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "resCalc") {
                    tempVariant = reader->readElementText();
                    md.setResistivity(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "espesor") {
                    tempVariant = reader->readElementText();
                    md.setThickness(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "profundidad") {
                    tempVariant = reader->readElementText();
                    md.setDepth(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "resistenciaTrasnveral") {
                    tempVariant = reader->readElementText();
                    md.setTransverseResistence(tempVariant.toDouble());
                    reader->readNextStartElement();
                }
                if (reader->name() == "conductanciaLongitudinal") {
                    tempVariant = reader->readElementText();
                    md.setLongitudinalConductance(tempVariant.toDouble());
                }
                list.append(md);
            }
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == "listaModeloSEVs"){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }
    }

    return list;
}

OldProject::OldProject(QObject *parent) : QObject(parent)
{
    QString testFile = "proy_nuevo.sev"; //"patron.sev";
    Project *newProj = readOldProject(testFile);
}

Project *OldProject::readOldProject(const QString &filename)
{
    QFile file(filename);
    QXmlStreamReader *reader;
    if (file.exists() && file.open(QFile::ReadOnly | QFile::Text)){
        reader = new QXmlStreamReader(file.readAll());
    }else {
        //return;
    }


    Project *newProject = new Project;
    //newProject()->setParent(this->parent());
    reader->readNextStartElement();
    reader->readNextStartElement();
    bool read = true;
    while (!reader->atEnd() && read) {
        switch (reader->tokenType()) {
        case QXmlStreamReader::TokenType::Characters:
            reader->readNext();
            break;
        case QXmlStreamReader::TokenType::StartElement:
            if (reader->name() == "listaSEVs"){
                reader->readNextStartElement();
                bool vesRead = true;
                while (vesRead) {
                    if (reader->name() == "listaSEVs" && reader->tokenType() == QXmlStreamReader::TokenType::EndElement) {
                        vesRead = false;
                    } else {
                        readOldSev(reader, newProject);
                    }
                    reader->readNext();
                }
            } else if (reader->name() == "nombre") {
                newProject->setName(reader->readElementText());
            } else if (reader->name() == "sevSeleccionado") {
                QVariant index = reader->readElementText();
                newProject->setCurrentIndex(index.toInt());
            } else {
                reader->readNextStartElement();
            }
        case QXmlStreamReader::TokenType::EndElement:
            if (reader->name() == "proyectoSEVs"){
                read = false;
            }
            reader->readNext();
            break;
        default:
            reader->readNext();
            break;
        }

        //        if (reader->name() == "listaSEVs"){
        //            reader->readNextStartElement();
        //            while (!reader->atEnd()) {
        //                if (reader->name() == "SEV"){
        //                    readOldSev(reader, newProject);
        //                }
        //            }
        //            reader->readNext();

        //        } else if (reader->name() == "nombre") {
        //            newProject->setName(reader->readElementText());
        //        } else if (reader->name() == "sevSeleccionado") {
        //            QVariant index = reader->readElementText();
        //            newProject->setCurrentIndex(index.toInt());
        //        }
        //        reader->readNext();
        //        } else {
        //            reader->readNextStartElement();
        //        }

        //        red = reader->name().toString();
        //        reader->readNext();
        //        red = reader->name().toString();
    }

    return newProject;
}
