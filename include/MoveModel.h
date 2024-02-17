#pragma once

#include <QAbstractListModel>
#include <optional>
#include <vector>
#include <QString>

#include "util.hxx"

class MoveModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum Role {
    RoleFirst = Qt::UserRole + 1,
    RoleSecond,
    RoleSelected
  };

  struct MoveModelDataEntry {
    chess::util::bits::HashedMove move;
    chess::Color made_by;
    chess::MoveResult result;
  };

private:

  struct MoveModelData {
    std::optional<QString> first;
    std::optional<QString> second;

    operator bool() {
      return first && second;
    }
  };

  std::vector<MoveModelData> _data;

protected:

  virtual QHash<int, QByteArray> roleNames() const override;

signals:
  void itemAdded(int index, QString first, QString second);

public:

  explicit MoveModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  void addEntry(const MoveModelDataEntry&);

  void removeLast() {
    beginResetModel();

    if (_data.size() > 1) {
      _data.back() = {};
    }

    endResetModel();
  }

  void clear() {
    beginResetModel();

    _data.clear();
    _data.push_back( { std::nullopt, std::nullopt } );

    endResetModel();
  }

};
