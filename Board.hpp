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

    void addMove(Board::MoveVec &moves, Square::Index from, Square::Index to) const;

    static void addMovePawn(Board::MoveVec &moves, Square::Index from, Square::Index to);

    static bool isEmpty(int piece);
    static bool isWhite(int piece);
    static int getColor(int piece);
    static int getType(int piece);
    static PieceType intToPieceType(int piece);
    static int pieceTypeToInt(PieceType type);
    static int pieceToInt(const Piece::Optional &piece);


    const int* board() const;

private:
    int board_[64];
    int turn_;
    CastlingRights cr_;
    Square::Optional ep_;
    static const int white = 8, black = 0;
    static const int empty = 0, pawn = 1, knight = 2, bishop = 3, rook = 4, queen = 5, king = 6;
    static const int typeMask = 0b0111;
};

std::ostream &operator<<(std::ostream &os, const Board &board);

#endif
