#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"

#include <optional>
#include <iosfwd>
#include <vector>

class Board
{
public:

    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;

    Board();

    void setPiece(const Square &square, const Piece::Optional &piece);

    Piece::Optional piece(const Square &square) const;

    void setTurn(PieceColor turn);

    PieceColor turn() const;

    void setCastlingRights(CastlingRights cr);

    CastlingRights castlingRights() const;

    void setEnPassantSquare(const Square::Optional &square);

    Square::Optional enPassantSquare() const;

    void makeMove(const Move &move);

    void pseudoLegalMoves(MoveVec &moves) const;

    void pseudoLegalMovesFrom(const Square &from, MoveVec &moves) const;

    static int pieceTypeIntoInt(const PieceType &pieceType);

private:
    std::vector<int> board;

    // 0 | 000
    // Color bit | type bits
    const static int empty = 0, pawn = 1, knight = 2, bishop = 3, rook = 4, queen = 5, king = 6;
    // White = 1, Black = 0;
    const static int white = 8;
    const static std::unordered_map<int , PieceType> intToPieceType;

    PieceColor turn_;
};

std::ostream &operator<<(std::ostream &os, const Board &board);

#endif
