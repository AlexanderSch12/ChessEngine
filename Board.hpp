#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"

#include <optional>
#include <iosfwd>
#include <vector>

struct PreviousState{
    int captured;
    CastlingRights cr;
    int turn;
    int ep;
};

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
    int getBoardTurn() const;

    void setCastlingRights(CastlingRights cr);

    CastlingRights castlingRights() const;

    void setEnPassantSquare(const Square::Optional &square);

    Square::Optional enPassantSquare() const;

    void makeMove(const Move &move);
    PreviousState makeMoveSaveState(const Move &move);

    void reverseMove(const Move &move, PreviousState state);
    void setCastling(int from, int to, bool reverse);
    void rookCastled(int position);
    void setEnPassant(int from, int to, bool reverse);
    void removeEnPassant();

    void pseudoLegalMoves(MoveVec &moves);
    std::vector<Move> filterLegalMoves(MoveVec &moves);

    void pseudoLegalMovesFrom(const Square &from, MoveVec &moves) const;

    void addMove(Board::MoveVec &moves, Square::Index from, Square::Index to) const;

    static void addMovePawn(Board::MoveVec &moves, Square::Index from, Square::Index to);
    bool isKingCheck(int color) const;
    bool isAttacked(int attacked) const;

    static bool isEmpty(int piece);
    static bool isWhite(int piece);
    static int getColor(int piece);
    static int getType(int piece);
    static PieceType intToPieceType(int piece);
    static int pieceTypeToInt(PieceType type);
    static int pieceToInt(const Piece::Optional &piece);

    std::vector<int> board() const;

    static const int white = 8, black = 0;
    static const int empty = 0, pawn = 1, knight = 2, bishop = 3, rook = 4, queen = 5, king = 6;
    static const int typeMask = 0b0111;
    static const int knight_jumps[8];

private:
    std::vector<int> board_;
    int turn_;
    CastlingRights cr_;
    int ep_;


};

std::ostream &operator<<(std::ostream &os, const Board &board);

#endif
