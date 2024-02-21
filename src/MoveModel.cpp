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
 * Method: addEntry()
 *
 *****************************************************************************/
void MoveModel::addEntry(const MoveModelDataEntry& entry) {
  beginResetModel();

  const auto&& [ source_square, target_square,
                 piece, promoted_to,
                 capture, double_push,
                 was_en_passant, castling ] = entry.move.explode();

  QString str;

  if (castling) {
    if (target_square == chess::G8 || target_square == chess::G1)
    {
      str = "O-O";
    }
    else {
      str = "O-O-O";
    }
  }
  else {
    auto str_target = *chess::util::fen::index_to_algebraic(target_square);

    if (!(piece == chess::WhitePawn || piece == chess::BlackPawn))
    {
      str += static_cast<QChar>(std::toupper(chess::util::fen::piece_to_char(piece)));
    }

    if (auto alg = chess::util::fen::index_to_algebraic(source_square))
    {
      // only name the file if its a pawn capture
      if ((piece == chess::WhitePawn || piece == chess::BlackPawn) && capture)
      {
        str += chess::util::fen::index_to_algebraic(source_square).value().at(0);
      }
    }

    if (capture) {
      str.append("x");
    }

    str.append(str_target);

    if (chess::util::toul(promoted_to) != 0) {
      str.append("=");
      str += chess::util::fen::piece_to_char(promoted_to);
    }
  }

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

  _current_count++;

  endResetModel();
}
