#ifndef STOREENTITY_H
#define STOREENTITY_H

#include <QtCore>

class StoreEntity
{
private:
    qint64 id;
    QString name;
    QString address;

    static qint64 nextId();

public:
    StoreEntity();
    StoreEntity(const QString &name, const QString &address);

    QJsonObject toJson() const;

    qint64 getId() const;
    void setId(qint64 newId);
    QString getName() const;
    void setName(const QString &newName);
    QString getAddress() const;
    void setAddress(const QString &newAddress);
};

#endif // STOREENTITY_H
