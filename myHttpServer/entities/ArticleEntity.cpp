#include "ArticleEntity.h"

qint64 ArticleEntity::nextId()
{
    static qint64 lastId = 0;
    return lastId++;
}

ArticleEntity::ArticleEntity()
{
}

ArticleEntity::ArticleEntity(const QString& name, const QString& description, const qreal& price, const qint64& total_in_shelf,
                             const qint64& total_in_vault, const qint64& store_id)
{
    id = ArticleEntity::nextId();
    this->store_id = store_id;
    this->name = name;
    this->description = description;
    this->price = price;
    this->total_in_shelf = total_in_shelf;
    this->total_in_vault = total_in_vault;
}

QJsonObject ArticleEntity::toJson() const
{
    return QJsonObject{
        { "id", id },
        { "name", name },
        { "description", description},
        { "price", price},
        { "total_in_shelf", total_in_shelf},
        { "total_in_vault", total_in_vault},
        { "store_id", store_id}
    };
}

qint64 ArticleEntity::getId() const
{
    return id;
}

void ArticleEntity::setId(qint64 newId)
{
    id = newId;
}

QString ArticleEntity::getName() const
{
    return name;
}

void ArticleEntity::setName(const QString &newName)
{
    name = newName;
}

QString ArticleEntity::getDescription() const
{
    return description;
}

void ArticleEntity::setDescription(const QString &newDescription)
{
    description = newDescription;
}

qreal ArticleEntity::getPrice() const
{
    return price;
}

void ArticleEntity::setPrice(qreal newPrice)
{
    price = newPrice;
}

qint64 ArticleEntity::getTotal_in_shelf() const
{
    return total_in_shelf;
}

void ArticleEntity::setTotal_in_shelf(qint64 newTotal_in_shelf)
{
    total_in_shelf = newTotal_in_shelf;
}

qint64 ArticleEntity::getTotal_in_vault() const
{
    return total_in_vault;
}

void ArticleEntity::setTotal_in_vault(qint64 newTotal_in_vault)
{
    total_in_vault = newTotal_in_vault;
}

qint64 ArticleEntity::getStore_id() const
{
    return store_id;
}

void ArticleEntity::setStore_id(qint64 newStore_id)
{
    store_id = newStore_id;
}

