#include "QVESModelDelegate.h"
#include <QtMath>

QString QVESModelDelegate::projectFileName() const
{
    return mProjectFileName;
}

QString QVESModelDelegate::projectPath() const
{
    return mProjectPath;
}

QString QVESModelDelegate::modelError() const
{
    return mCurrentVES->currentModel()->errorString();
}

double QVESModelDelegate::chartMinX() const
{
    return mChartMinX;
}

double QVESModelDelegate::chartMinY() const
{
    return mChartMinY;
}

double QVESModelDelegate::chartMaxX() const
{
    return mChartMaxX;
}

double QVESModelDelegate::chartMaxY() const
{
    return mChartMaxY;
}

QString QVESModelDelegate::vesName() const
{
    return mCurrentVES->name();
}

void QVESModelDelegate::readVESNames()
{
    mVESNames.clear();
    foreach (const VES &ves, mCurrentProject->vess()) {
        mVESNames.append(ves.name());
    }
}

void QVESModelDelegate::readModelNames()
{
    mModelNames.clear();
    foreach (const auto &item, mCurrentVES->models()) {
        mModelNames.append(item.name());
    }
}

void QVESModelDelegate::selectModelForTable()
{
    switch (mShowedTableData) {
    case TableModel::DataType::Field:
        mCurrentModel = mFieldModel;
        break;
    case TableModel::DataType::Splice:
        mCurrentModel = mSpliceModel;
        break;
    case TableModel::DataType::Calculated:
       mCurrentModel = mCalculatedModel;
        break;
    case TableModel::DataType::Model:
       mCurrentModel = mTableModeledModel;
        break;
    }

    connect(mCurrentModel, &TableModel::myTableChanged, this, &QVESModelDelegate::updateVESData);
    emit tableModelChanged();
}

QVESModelDelegate::QVESModelDelegate(QObject *parent) : QObject(parent)
{
    mCurrentProject = nullptr;
    mCurrentVES = nullptr;
    mCore = new VESCore(this);
    mCurrentModel = new TableModel(this);
    mShowedTableData = TableModel::DataType::Field;

    mProjectFileName = mProjectPath = "";
    mFieldModel = nullptr;
    mSpliceModel = nullptr;
    mCalculatedModel = nullptr;
    mTableModeledModel = nullptr;
    mChartModeledModel = nullptr;

    connect(mCore, &VESCore::projectLoaded, this, &QVESModelDelegate::changeCurrentProject);
}

TableModel *QVESModelDelegate::currentModel()
{
    return mCurrentModel;
}

TableModel *QVESModelDelegate::fieldModel()
{
    return mFieldModel;
}

TableModel *QVESModelDelegate::spliceModel()
{
    return mSpliceModel;
}

TableModel *QVESModelDelegate::calculatedModel()
{
    return mCalculatedModel;
}

TableModel *QVESModelDelegate::tableModeledModel()
{
    return mTableModeledModel;
}

TableModel *QVESModelDelegate::chartModeledModel()
{
    return mChartModeledModel;
}

int QVESModelDelegate::currentVESIndex() const
{
    return mCurrentVESIndex;
}

int QVESModelDelegate::currentVESModelIndex() const
{
    return mCurrentVESModelIndex;
}

void QVESModelDelegate::changeCurrentProject()
{
    disconnect(mCurrentProject, &Project::currentVESChanged, this, &QVESModelDelegate::changeCurrentVES);
    mCurrentProject = mCore->project();
    connect(mCurrentProject, &Project::currentVESChanged, this, &QVESModelDelegate::changeCurrentVES);
    mCurrentVESIndex = mCurrentProject->currentIndex();
    mProjectFileName = mCore->projectFileName();
    mProjectPath = mCore->projectPath();
    readVESNames();
    emit projectChanged();
    changeCurrentVES();
}

void QVESModelDelegate::changeCurrentVES()
{
    mCurrentVES = mCurrentProject->currentVES();
    mCurrentVESModelIndex = mCurrentVES->currentIndexModel();
    readModelNames();
    setDataTableModel();
    connect(this, &QVESModelDelegate::carryOutZohdyInversion, mCurrentVES, &VES::zohdyInversion);
    connect(mCurrentVES, &VES::selectedModelChanged, this, &QVESModelDelegate::updateVESModels);
    emit vesChanged();
}

void QVESModelDelegate::setDataTableModel()
{
    QList<ModelDataTable *> tempTable;
    QList<ModelDataTable *> tempTable2;

    mChartMinX = pow(10, floor(log10(mCurrentVES->minX())));
    mChartMinY = pow(10, floor(log10(mCurrentVES->minY())));
    mChartMaxX = pow(10, ceil(log10(mCurrentVES->maxX())));
    mChartMaxY = pow(10, ceil(log10(mCurrentVES->maxY())));

    mFieldModel = new TableModel(this);
    foreach (const auto &item, mCurrentVES->fieldData()) {
        ModelDataTable *value = new ModelDataTable(item.ab2Distance(), item.resistivity());
        tempTable.append(value);
    }
    mFieldModel->setTableFromVES(tempTable, TableModel::DataType::Field);

    tempTable.clear();
    mSpliceModel = new TableModel(this);
    foreach (const auto &item, mCurrentVES->splices()) {
        ModelDataTable *value = new ModelDataTable(item.ab2Distance(), item.resistivity());
        tempTable.append(value);
    }
    mSpliceModel->setTableFromVES(tempTable, TableModel::DataType::Splice);

    tempTable.clear();
    mCalculatedModel = new TableModel(this);
    foreach (const auto &item, mCurrentVES->currentModel()->calculatedData()) {
        ModelDataTable *value = new ModelDataTable(item.ab2Distance(), item.resistivity());
        tempTable.append(value);
    }
    mCalculatedModel->setTableFromVES(tempTable, TableModel::DataType::Calculated);

    tempTable.clear();
    mTableModeledModel = new TableModel(this);
    mChartModeledModel = new TableModel(this);
    foreach (const auto &item, mCurrentVES->currentModel()->model()) {
        ModelDataTable *value0 = new ModelDataTable(item.depth(), item.resistivity());
        tempTable2.append(value0);

        if (item.from() == 0.0){
            ModelDataTable *value1 = new ModelDataTable(mChartMinX, item.resistivity());
            tempTable.append(value1);
        } else {
            ModelDataTable *value1 = new ModelDataTable(item.from(), item.resistivity());
            tempTable.append(value1);
        }
        if (item.until() == qInf()){
            ModelDataTable *value2 = new ModelDataTable(mChartMaxX, item.resistivity());
            tempTable.append(value2);
        } else {
            ModelDataTable *value2 = new ModelDataTable(item.until(), item.resistivity());
            tempTable.append(value2);
        }

    }
    mTableModeledModel->setTableFromVES(tempTable2, TableModel::DataType::Model);
    mChartModeledModel->setTableFromVES(tempTable, TableModel::DataType::Model);

   selectModelForTable();
}

void QVESModelDelegate::openProject(const QString &filename)
{
    mCore->openProject(filename);

}

void QVESModelDelegate::saveAsProject(const QString &filename)
{
    mCore->saveProject(filename);
}

void QVESModelDelegate::saveProject()
{
    mCore->saveProject();
}

void QVESModelDelegate::showedTableDataChanged(const TableModel::DataType dt)
{
    mShowedTableData = dt;
    selectModelForTable();
}

QStringList QVESModelDelegate::vesNames() const
{
    return mVESNames;
}

QStringList QVESModelDelegate::modelNames() const
{
    return mModelNames;
}

void QVESModelDelegate::updateVESData(const QModelIndex &index) const
{
    double tempValue = mCurrentModel->data(index, Qt::DisplayRole).toDouble();
    int dt = static_cast<int>(mShowedTableData);

    mCore->changeDataForCurrentVES(index.row(), index.column(), dt, tempValue);

//    switch (mShowedTableData) {
//    case TableModel::DataType::Field:
//        if (index.column() == 0){
//            mCurrentVES->fieldData()[index.row()].setAb2Distance(mCurrentModel->data(index, Qt::DisplayRole).toDouble());
//        }else if (index.column() == 1){
//            mCurrentVES->fieldData()[index.row()].setResistivity(mCurrentModel->data(index, Qt::DisplayRole).toDouble());
//        } else {
//            return;
//        }

//        break;
//    default:
//        break;
//    }
}

void QVESModelDelegate::selectedVESChanged(int index)
{
    mCurrentProject->setCurrentIndex(index);
}

void QVESModelDelegate::changeCurrentModel(int index)
{
    mCurrentVES->selectModel(index);
}

void QVESModelDelegate::updateVESModels()
{
    readModelNames();
    setDataTableModel();
    emit vesChanged();
}
