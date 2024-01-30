#include "MoveModel.h"

/******************************************************************************
 *
 * Method: MoveModel()
 *
 *****************************************************************************/
MoveModel::MoveModel(QObject *parent)
    : QAbstractListModel(parent)
{
  _data.push_back({std::nullopt, std::nullopt});
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
    { RoleSecond, "second" }
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
    auto str_target = *util::fen::index_to_algebraic(target_square);

    if (!(piece == Piece::w_pawn || piece == Piece::b_pawn))
    {
      str += static_cast<QChar>(std::toupper(util::fen::piece_to_char(piece)));
    }

    if (auto alg = util::fen::index_to_algebraic(source_square))
    {
      // only name the file if its a pawn capture
      if ((piece == Piece::w_pawn || piece == Piece::b_pawn) && capture)
      {
        str += util::fen::index_to_algebraic(source_square).value().at(0);
      }
    }

    if (capture) {
      str.append("x");
    }

    str.append(str_target);

    if (util::toul(promoted_to) != 0) {
      str.append("=");
      str += util::fen::piece_to_char(promoted_to);
    }
  }

  if (entry.result == MoveResult::Check) {
    str.append("+");
  }
  else if (entry.result == MoveResult::Checkmate) {
    str.append("#");
  }

  if (entry.made_by == Color::white) {

    if (_data.back()) {
      _data.push_back( { str, std::nullopt } );
    }
    else {
      _data.back().first.emplace(str);
    }

  } else {
    _data.back().second = str;
  }

  endResetModel();
}