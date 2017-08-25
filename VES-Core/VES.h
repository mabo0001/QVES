#ifndef VES_H
#define VES_H

#include "ves-core_global.h"
#include <QObject>
#include "BasicData.h"
#include "SpliceData.h"
#include "LocationData.h"
#include "InversionModel.h"
#include <QList>
#include "Serializable.h"
#include <QDate>
#include "VfsaParameters.h"

class VESCORESHARED_EXPORT VES : public QObject, public Serializable
{
    Q_OBJECT

private:
    QString mName;
    QString mId;
    QString mFieldOperator;
    QString mEquipment;
    QString mComment;
    QDate mDate;
    QList<BasicData> mFieldData;
    QList<SpliceData> mSplices;
    LocationData *mLocation;
    QList<InversionModel> mModels;
    InversionModel *mCurrentModel;
    int mCurrentIndexModel;
    VfsaParameters *mPreviousParameters;
    VfsaParameters *mCurrentParameters;
    double mMinX;
    double mMinY;
    double mMaxX;
    double mMaxY;

    //Splices methods
    int searchSections();
    void correctSplices(int maxSections);
    void cleanDuplicatedSplices();

public:
    explicit VES(QObject *parent = nullptr);
    VES(const VES& ve);

    //Getters
    QString name() const;
    QString id() const;
    QString fieldOperator() const;
    QString equipment() const;
    QString comment() const;
    QString date() const;
    QList<BasicData> fieldData() const;
    QList<SpliceData> splices() const;
    LocationData *location() const;
    QList<InversionModel> models() const;
    InversionModel *currentModel() const;
    int currentIndexModel() const;
    VfsaParameters *previousParameters() const;
    VfsaParameters *currentParameters() const;
    double minX() const;
    double minY() const;
    double maxX() const;
    double maxY() const;

    //Setters
    void setName(const QString name);
    void setFieldOperator(const QString fieldOp);
    void setEquipment(const QString equip);
    void setComment(const QString comm);
    void setDate(const QString stringDate);
    void setLocation(LocationData *loc);
    void setCurrentIndexModel(const int value);
    void setFieldData(const QList<BasicData> &list);
    void setModels(const QList<InversionModel> &list);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    VES& operator =(const VES &rhs);

    void findMaxAndMin();

signals:

public slots:
    void createSplices();
    void updateFieldData(const int row, const int column, const double value);
};

Q_DECLARE_METATYPE(VES)

#endif // VES_H
