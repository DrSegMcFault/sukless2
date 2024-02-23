#include "MoveModel.h"

/******************************************************************************
 *
 * Method: MoveModel()
 *
 *****************************************************************************/
MoveModel::MoveModel(QObject *parent)
  : QAbstractListModel(parent)
{
  _data.push_back({std::nullopt, std::nullopt, true});
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int MoveModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return _data.size();
}

/******************************************************************************
 *
 * Method: roleNames()
 *
 *****************************************************************************/
QHash<int, QByteArray> MoveModel::roleNames() const
{
  static QHash<int, QByteArray> roles = {
    { RoleFirst, "first" },
    { RoleSecond, "second" },
    { RoleSelected, "selected" }
  };

  return roles;
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant MoveModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return {};

  const auto& item = _data[index.row()];

  switch (role) {

    case RoleFirst:
      return item.first ? *item.first : QString("");

    case RoleSecond:
      return item.second ? *item.second : QString("");

    case RoleSelected:
      return QVariant(item.selected).toBool();

    default:
      return {};
  }
}

/******************************************************************************
 *
 * Method: onMoveConfirmed()
 *
 *****************************************************************************/
void MoveModel::onMoveConfirmed(chess::HashedMove m, chess::Color c, chess::MoveResult b)
{
  MoveModelDataEntry e {m, c, b};
  addEntry(e);
}

/******************************************************************************
 *
 * Method: addEntry()
 *
 *****************************************************************************/
void MoveModel::addEntry(const MoveModelDataEntry& entry) {
  beginResetModel();

  QString str;

  str = QString::fromStdString(chess::to_string(entry.move));

  if (entry.result == chess::MoveResult::Check) {
    str.append("+");
  }
  else if (entry.result == chess::MoveResult::Checkmate) {
    str.append("#");
  }

  if (entry.made_by == chess::White) {

    if (_data.back()) {
      _data.push_back( { str, std::nullopt } );
    }
    else {
      _data.back().first.emplace(str);
    }

  } else {
    _data.back().second = str;
  }

  for (auto i : chess::util::range(_data.size())) {
    _data[i].selected = false;
  }

  _data.back().selected = true;

  _active_index = _data.size();
  _active_first = _data.back().first.value_or(QString(""));
  _active_second = _data.back().second.value_or(QString(""));

  emit activeIndexChanged();
  emit activeFirstChanged();
  emit activeSecondChanged();

  endResetModel();
}
