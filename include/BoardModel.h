#pragma once

#include <QAbstractListModel>
#include <QList>
#include <array>
#include <iostream>
#include <optional>
#include <memory>
#include <unordered_map>
#include <QUrl>
#include <vector>
#include <QString>

#include "util.hxx"
#include "Game.hxx"

using namespace chess;
class BoardModel : public QAbstractListModel
{
    Q_OBJECT

public:

  enum Role {
    RolePiece = Qt::UserRole + 1,
    RoleIcon,
    RoleIndex,
    RolePossibleMove,
    RoleRankLabel,
    RoleFileLabel
  };

  enum class Rotation {
    ViewFromWhite,
    ViewFromBlack
  };

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:

    std::array<std::optional<Piece>, 64> _data = {};
    std::vector<uint8_t> _possible_moves = {};
    Rotation _visual_rotation = Rotation::ViewFromWhite;
    std::shared_ptr<Game> _game;

    [[nodiscard]] int toInternalIndex(int index) const {
      int row = index / 8;
      int column = index % 8;

      switch (_visual_rotation) {
        case Rotation::ViewFromWhite:
         return  (7 - row) * 8 + column;
        case Rotation::ViewFromBlack:
         return  row * 8 + (7 - column);
      }
    }
    const std::unordered_map<int, QString> _white_rank_map = {
        { 0,   QString("1") },
        { 8,   QString("2") },
        { 16,  QString("3") },
        { 24,  QString("4") },
        { 32,  QString("5") },
        { 40,  QString("6") },
        { 48,  QString("7") },
        { 56,  QString("8") }
    };

    const std::unordered_map<int, QString> _white_file_map = {
        { 0,  QString("a") },
        { 1,  QString("b") },
        { 2,  QString("c") },
        { 3,  QString("d") },
        { 4,  QString("e") },
        { 5,  QString("f") },
        { 6,  QString("g") },
        { 7,  QString("h") }
    };

    const std::unordered_map<int, QString> _black_file_map = {
        { 63,  QString("h") },
        { 62,  QString("g") },
        { 61,  QString("f") },
        { 60,  QString("e") },
        { 59,  QString("d") },
        { 58,  QString("c") },
        { 57,  QString("b") },
        { 56,  QString("a") },
    };

    const std::unordered_map<int, QString> _black_rank_map = {
        { 63,   QString("8") },
        { 55,   QString("7") },
        { 47,  QString("6") },
        { 39,  QString("5") },
        { 31,  QString("4") },
        { 23,  QString("3") },
        { 15,  QString("2") },
        { 7,  QString("1") }
    };


    const std::unordered_map<chess::Piece, QUrl> _icons = {
        { Piece::w_pawn, QUrl("qrc:/resources/pawn_white.png") },
        { Piece::w_knight, QUrl("qrc:/resources/knight_white.png") },
        { Piece::w_bishop, QUrl("qrc:/resources/bishop_white.png") },
        { Piece::w_rook, QUrl("qrc:/resources/rook_white.png") },
        { Piece::w_queen, QUrl("qrc:/resources/queen_white.png") },
        { Piece::w_king, QUrl("qrc:/resources/king_white.png") },
        { Piece::b_pawn, QUrl("qrc:/resources/pawn_black.png") },
        { Piece::b_knight, QUrl("qrc:/resources/knight_black.png") },
        { Piece::b_bishop, QUrl("qrc:/resources/bishop_black.png") },
        { Piece::b_rook, QUrl("qrc:/resources/rook_black.png") },
        { Piece::b_queen, QUrl("qrc:/resources/queen_black.png") },
        { Piece::b_king, QUrl("qrc:/resources/king_black.png")}
    };


public:
    explicit BoardModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void toggleRotation() {
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

    Q_INVOKABLE void move(int from, int to) {
       beginResetModel();

       auto source = static_cast<uint8_t>(toInternalIndex(from));
       auto target = static_cast<uint8_t>(toInternalIndex(to));

       switch (_game->try_move({source, target}) ) {
         case MoveResult::Success:
         {
           qDebug() << "Successfull move\n";
           _data = _game->get_current_board();
           break;
         }

         case MoveResult::Illegal:
         {
           qDebug() << "BoardManager::Illegal Move!\n";
           break;
         }

         case MoveResult::Check:
         case MoveResult::Checkmate:
         case MoveResult::Stalemate:
         case MoveResult::Draw:
           Q_ASSERT(false);
           break;
       }

       endResetModel();
    }

    Q_INVOKABLE void boardClicked(int index) {
       beginResetModel();

       auto i = toInternalIndex(index);
       _possible_moves = _game->get_pseudo_legal_moves(i);

       endResetModel();
    }
};
