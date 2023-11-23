#include "BoardModel.h"
#include <iostream>

/******************************************************************************
 *
 * Method: BoardModel()
 *
 *****************************************************************************/
BoardModel::BoardModel(QObject *parent)
    : QAbstractListModel(parent)
{
    _game = std::make_shared<Game>();
    _data = _game->get_current_board();
    _possible_moves.reserve(55);
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

     case RoleFileLabel:
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


     default:
       return {};
   }
}
