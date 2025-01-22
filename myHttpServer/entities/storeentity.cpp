#include "StoreEntity.h"

qint64 StoreEntity::nextId()
{
    static qint64 lastId = 0;
    return lastId++;
}

StoreEntity::StoreEntity()
{

}

StoreEntity::StoreEntity(const QString &name, const QString &address) : name(name), address(address)
{
    id = StoreEntity::nextId();
}

QJsonObject StoreEntity::toJson() const
{
    return QJsonObject{
        { "id", id },
        { "name", name },
        { "address", address},
    };
}

qint64 StoreEntity::getId() const
{
    return id;
}

void StoreEntity::setId(qint64 newId)
{
    id = newId;
}

QString StoreEntity::getName() const
{
    return name;
}

void StoreEntity::setName(const QString &newName)
{
    name = newName;
}

QString StoreEntity::getAddress() const
{
    return address;
}

void StoreEntity::setAddress(const QString &newAddress)
{
    address = newAddress;
}
