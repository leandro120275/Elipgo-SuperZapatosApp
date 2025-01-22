#ifndef ARTICLEENTITY_H
#define ARTICLEENTITY_H

#include <QtCore>

class ArticleEntity
{
private:
    qint64 id;
    QString name;
    QString description;
    qreal price;
    qint64 total_in_shelf;
    qint64 total_in_vault;
    qint64 store_id;

    static qint64 nextId();

public:
    ArticleEntity();
    ArticleEntity(const QString& name, const QString& description, const qreal& price, const qint64& total_in_shelf,
                  const qint64& total_in_vault, const qint64& store_id);

    QJsonObject toJson() const;

    qint64 getId() const;
    void setId(qint64 newId);
    QString getName() const;
    void setName(const QString &newName);
    QString getDescription() const;
    void setDescription(const QString &newDescription);
    qreal getPrice() const;
    void setPrice(qreal newPrice);
    qint64 getTotal_in_shelf() const;
    void setTotal_in_shelf(qint64 newTotal_in_shelf);
    qint64 getTotal_in_vault() const;
    void setTotal_in_vault(qint64 newTotal_in_vault);
    qint64 getStore_id() const;
    void setStore_id(qint64 newStore_id);
};

#endif // ARTICLEENTITY_H
