#ifndef DATAPANEL_H
#define DATAPANEL_H

#include <QWidget>
#include "TableModel.h"
#include <QStringList>
#include "TableDelegate.h"
#include "QItemSelectionModel"
#include "QVESModelDelegate.h"

namespace Ui {
class DataPanel;
}

class DataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DataPanel(QVESModelDelegate *delegate, QWidget *parent = nullptr);
    ~DataPanel();

private:
    Ui::DataPanel *ui;
    TableModel::DataType mSelectedData;
    TableDelegate *myDelegate;
    QItemSelectionModel *selectionModel;
    QVESModelDelegate *mainDelegate;
    bool mResetSelection;
    QModelIndex mPreviousIndex;

public slots:
    void setMyModel();
    void loadVESNames();
    void loadModelNames();
    void changeShowedData();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void restoreSelection(const QModelIndex &index);

signals:
    void currentVESIndexChanged(int index);
    void currentVESModelIndexChanged(int index);
    void showedDataChanged(const TableModel::DataType dataType);
    void rowSelectionChanged(const QList<int> indices, TableModel::DataType dataType);

private slots:
//    void on_comboBoxCurrentVes_currentIndexChanged(int index);
//    void on_comboBoxVesModel_currentIndexChanged(int index);
};

#endif // DATAPANEL_H
