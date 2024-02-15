#pragma once

#include <QAbstractListModel>
#include <optional>
#include <vector>
#include <QString>

#include "util.hxx"

using namespace chess;

class MoveModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum Role {
    RoleFirst = Qt::UserRole + 1,
    RoleSecond
  };

  struct MoveModelDataEntry {
    util::bits::HashedMove move;
    Color made_by;
    MoveResult result;
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
