#include "BoardModel.h"

/******************************************************************************
 *
 * Method: BoardModel()
 *
 *****************************************************************************/
BoardModel::BoardModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int BoardModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return _data.size();
}

/******************************************************************************
 *
 * Method: roleNames()
 *
 *****************************************************************************/
QHash<int, QByteArray> BoardModel::roleNames() const
{
  static QHash<int, QByteArray> roles = {
    { RolePiece, "piece" },
    { RoleIcon, "icon" },
    { RoleIndex, "idx"},
    { RolePossibleMove, "possible"},
    { RoleRankLabel, "rankLabel"},
    { RoleFileLabel, "fileLabel"}
  };

  return roles;
}

/******************************************************************************
 *
 * Method: init(ControlColor, engine_assist, ai_enable, difficulty)
 *
 *****************************************************************************/
void BoardModel::init(Color user, bool engine_assist, bool ai_enable, AIDifficulty diff)
{
  beginResetModel();

  _user_color = user;

  _ai_assist_enabled = engine_assist;
  _ai_enabled = ai_enable;
  _ai_strength = diff;

  _visual_rotation =
    _user_color == Color::white ? Rotation::ViewFromWhite : Rotation::ViewFromBlack;

  AIConfig cfg;
  cfg.controlling = user == Color::white ? Color::black : Color::white;
  cfg.difficulty = diff;

  _game = std::make_shared<BoardManager>(&_generator);
  _data = _game->get_current_board();

  _possible_moves.reserve(55);

  endResetModel();
}

/******************************************************************************
 *
 * Method: toggleRotation()
 *
 *****************************************************************************/
void BoardModel::toggleRotation()
{
  beginResetModel();

  switch (_visual_rotation) {
    case Rotation::ViewFromWhite:
      _visual_rotation = Rotation::ViewFromBlack;
      break;
    case Rotation::ViewFromBlack:
      _visual_rotation = Rotation::ViewFromWhite;
      break;
  }

  endResetModel();
}

/******************************************************************************
 *
 * Method: move(int from, int to)
 *
 *****************************************************************************/
void BoardModel::move(int from, int to) {
  beginResetModel();

  QUrl sound_to_play;

  auto source = static_cast<uint8_t>(toInternalIndex(from));
  auto target = static_cast<uint8_t>(toInternalIndex(to));

  auto h_move = _game->find_move(source, target);

  switch (_game->try_move( {source, target} )) {

    case MoveResult::Illegal:
    {
      sound_to_play = _illegal_sound;
      break;
    }

    case MoveResult::Valid:
    {
      _data = _game->get_current_board();
      sound_to_play = _move_sound;
      _moveModel.addEntry({ *h_move,
                            _game->get_side_to_move() == Color::white ? Color::black : Color::white,
                            MoveResult::Valid });
      break;
    }

    case MoveResult::Check:
    {
      _data = _game->get_current_board();
      sound_to_play = _move_sound;
      _moveModel.addEntry({ *h_move,
                            _game->get_side_to_move() == Color::white ? Color::black : Color::white,
                            MoveResult::Check });
      break;
    }

    case MoveResult::Checkmate:
    {
      _data = _game->get_current_board();
      sound_to_play = _game_end_sound;
      _moveModel.addEntry({ *h_move,
                            _game->get_side_to_move() == Color::white ? Color::black : Color::white,
                            MoveResult::Checkmate });

      emit checkmate(_game->get_side_to_move() == Color::black
                         ? QString(tr("White Wins!"))
                         : QString(tr("Black Wins!")));

      break;
    }

    case MoveResult::Stalemate:
    {
      _data = _game->get_current_board();
      sound_to_play = _game_end_sound;
      _moveModel.addEntry({ *h_move,
                            _game->get_side_to_move() == Color::white ? Color::black : Color::white,
                            MoveResult::Stalemate });

      emit checkmate(QString("Stalemate!"));
      break;
    }

    case MoveResult::Draw:
      Q_ASSERT(false);
      break;
  }

  endResetModel();

  if (!sound_to_play.isEmpty()) {
    emit playSound(sound_to_play);
  }
}

/******************************************************************************
 *
 * Method: boardClicked(int index)
 *
 *****************************************************************************/
void BoardModel::boardClicked(int index)
{
  beginResetModel();

  auto i = toInternalIndex(index);
  _possible_moves = _game->get_pseudo_legal_moves(i);

  endResetModel();
}

/******************************************************************************
 *
 * Method: reset()
 *
 *****************************************************************************/
void BoardModel::reset()
{
  beginResetModel();

  _moveModel.clear();
  _game.reset();
  _game = std::make_shared<BoardManager>(&_generator);
  _data = _game->get_current_board();

  endResetModel();
}


/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant BoardModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
     return {};

   int newIndex = toInternalIndex(index.row());
   const auto& item = _data[newIndex];

   switch (role) {

     case RolePiece:
       return item ? *item : Piece::All;

     case RoleIcon:
       return item ? _icons.at(*item) : QUrl("");

     case RolePossibleMove:
       return util::contains(_possible_moves, static_cast<uint8_t>(newIndex));

     case RoleIndex:
       return newIndex;

     case RoleRankLabel:
     {
       switch (_visual_rotation) {
         case Rotation::ViewFromWhite:
           if (auto it = _white_rank_map.find(newIndex);
                 it != _white_rank_map.end())
           {
             return _white_rank_map.at(newIndex);
           } else return QString();

         case Rotation::ViewFromBlack:
           if (auto it = _black_rank_map.find(newIndex);
                 it != _black_rank_map.end())
           {
             return _black_rank_map.at(newIndex);
           } else return QString();
       }
     }

     case RoleFileLabel:
     {
       switch (_visual_rotation) {
         case Rotation::ViewFromWhite:
           if (auto it = _white_file_map.find(newIndex);
                 it != _white_file_map.end())
           {
             return _white_file_map.at(newIndex);
           } else return QString();
           break;

         case Rotation::ViewFromBlack:
           if (auto it = _black_file_map.find(newIndex);
                 it != _black_file_map.end())
           {
             return _black_file_map.at(newIndex);
           } else return QString();
           break;
       }
     }

     default:
       return {};
   }
}
