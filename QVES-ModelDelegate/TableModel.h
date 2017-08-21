#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include "qves-modeldelegate_global.h"
#include <QList>
#include "ModelDataTable.h"
class QVESMODELDELEGATESHARED_EXPORT TableModel: public QAbstractTableModel
{
    Q_OBJECT

    QList<ModelDataTable*> mTable;

public:
    TableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    void setTableFromVES(const QList<ModelDataTable*> &table);
};

#endif // TABLEMODEL_H