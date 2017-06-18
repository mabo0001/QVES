#ifndef INVERSIONMODEL_H
#define INVERSIONMODEL_H

#include "ves-core_global.h"
#include <QObject>
#include "BasicData.h"
#include "ModelData.h"
#include "VfsaParameters.h"
#include <QList>
#include "Serializable.h"
#include <QVector>

class VESCORESHARED_EXPORT InversionModel : public QObject, public Serializable
{
    Q_OBJECT

public:
    enum class InversionAlgorithm{Zohdy, Vfsa, Manual, Unkown};
    enum class ZohdyFilters{Johansen, ONeill};

private:
    QString mName;
    QString mId;
    double mErrorResult;
    QString mErrorString;
    InversionAlgorithm mUsedAlgorithm;
    ZohdyFilters mZohdyFilter;
    QList<BasicData> mCalculatedData;
    QList<ModelData> mModel;
    VfsaParameters mPreviousParameters;
    VfsaParameters mCurrentParameters;

    //Zohdy inversion functions
    void chooseFilter(const InversionModel::ZohdyFilters filter, QVector<double> &a, double &w, double &s, double &dx);
    void calculateDepths(QList<ModelData> &model);
    void calculateThicknesses(QList<ModelData> &model);
    void TRS(const QList<BasicData> &field, QList<BasicData> &calculated, QList<ModelData> &model, const QVector<double> a, const double w, const double s, const double dx);
    double calculateModelError(const QList<BasicData> &fieldData, const QList<BasicData> &calculatedData) const;

public:
    explicit InversionModel(const QString &name, QObject *parent = nullptr);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;





    //Zohdy inversion functions
    void zohdyInversion(const QList<BasicData> &fieldData, const InversionModel::ZohdyFilters filter);


signals:

public slots:
};

#endif // INVERSIONMODEL_H