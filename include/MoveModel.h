#pragma once

#include <QAbstractListModel>
#include <optional>
#include <vector>
#include <QString>
#include "engine/ChessTypes.hxx"

class MoveModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(qint32 activeIndex MEMBER _active_index NOTIFY activeIndexChanged)
  Q_PROPERTY(QString activeFirst MEMBER _active_first NOTIFY activeFirstChanged)
  Q_PROPERTY(QString activeSecond MEMBER _active_second NOTIFY activeSecondChanged)

public:

  enum Role {
    RoleFirst = Qt::UserRole + 1,
    RoleSecond,
    RoleSelected
  };

  struct MoveModelDataEntry {
    chess::HashedMove move;
    chess::Color made_by;
    chess::MoveResult result;
  };

private:

  struct MoveModelData {
    std::optional<QString> first;
    std::optional<QString> second;
    bool selected = true;

    operator bool() {
      return first && second;
    }
  };

  std::vector<MoveModelData> _data;

  qint32 _active_index = 0;
  QString _active_first = "";
  QString _active_second = "";

protected:

  virtual QHash<int, QByteArray> roleNames() const override;

signals:
  void activeIndexChanged();
  void activeFirstChanged();
  void activeSecondChanged();

public slots:
  void onMoveConfirmed(chess::HashedMove, chess::Color, chess::MoveResult);

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

    _active_index = 0;
    _active_first = "";
    _active_second = "";

    _data.clear();
    _data.push_back( { std::nullopt, std::nullopt, true} );

    endResetModel();
  }

  size_t dataCount() { return _data.size(); }

  void setSelected(size_t index) {
    beginResetModel();

    for (size_t i = 0; i < _data.size(); i++) {
      if (i == index) {
        _data[i].selected = true;
        _active_index = index + 1;
        _active_first = _data[i].first.value_or("");
        _active_second = _data[i].second.value_or("");
      } else {
        _data[i].selected = false;
      }
    }

    emit activeIndexChanged();
    emit activeFirstChanged();
    emit activeSecondChanged();

    endResetModel();
  }

};
