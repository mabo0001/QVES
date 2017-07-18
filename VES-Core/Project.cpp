#include "Project.h"

Project::Project(QObject *parent) : QObject(parent)
{

}

Project::Project(const Project &pr)
{
    mName = pr.name();
    mVESs = pr.vess();
    mCurrentVES = pr.currentVES();
    mCurrentIndex = pr.currentIndex();
    mCurrentePath = pr.currentePath();
    mSaved = pr.saved();
    this->setParent(pr.parent());
}

QString Project::name() const
{
    return mName;
}

QList<VES> Project::vess() const
{
    return mVESs;
}

VES *Project::currentVES() const
{
    return mCurrentVES;
}

int Project::currentIndex() const
{
    return mCurrentIndex;
}

QString Project::currentePath() const
{
    return mCurrentePath;
}

bool Project::saved() const
{
    return mSaved;
}

void Project::setName(const QString value)
{
    mName = value;
}

void Project::setCurrentePath(const QString value)
{
    mCurrentePath = value;
}

QVariant Project::toVariant() const
{
    QVariantMap map;
    map.insert("mName", mName);
    map.insert("mCurrentIndex", mCurrentIndex);
    map.insert("mCurrentePath", mCurrentePath);
    map.insert("mCurrentVES", &mCurrentVES);

    QVariantList list;
    for (const auto& v : mVESs) {
    list.append(v.toVariant());
    }
    map.insert("mVESs", list);

    return map;
}

void Project::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    mName = map.value("mName").toString();
    mCurrentIndex = map.value("mCurrentIndex").toInt();
    mCurrentePath = map.value("mCurrentePath").toString();
    mCurrentVES = new VES(this);
    mCurrentVES->fromVariant(map.value("mCurrentVES"));

    QVariantList list = map.value("mVESs").toList();
    for(const QVariant& data : list) {
        VES v;
        v.fromVariant(data);
        mVESs.append(v);
    }
}
