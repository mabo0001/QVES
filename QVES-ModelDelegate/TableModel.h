#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include "qves-modeldelegate_global.h"
#include <QList>
#include "XYData.h"

class QVESMODELDELEGATESHARED_EXPORT TableModel: public QAbstractTableModel
{
public:
    enum class DataType{Field, Splice, Calculated, Model};

private:
    Q_OBJECT

    QList<XYData*> mTable;
    DataType mTypeOfData;

public:
    TableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setDataTable(const QList<XYData*> &table, DataType type);

signals:
    void myTableChanged(const QModelIndex &index, const int dataType, const double oldValue, const double newValue);
    void restoreSelection(const QModelIndex &index);
};

#endif // TABLEMODEL_H
