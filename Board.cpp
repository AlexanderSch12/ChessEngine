#include "Board.hpp"

#include <ostream>
#include <cassert>
#include <cmath>

const std::unordered_map<int, PieceType> Board::intToPieceType{
        {pawn, PieceType::Pawn}, {knight, PieceType::Knight}, {bishop, PieceType::Bishop}, {rook, PieceType::Rook},
        {queen, PieceType::Queen}, {king, PieceType::King}
};

Board::Board() :
        board(64, 0)
{

}

void Board::setPiece(const Square &square, const Piece::Optional &piece)
{
    int boardPiece = pieceTypeIntoInt(piece->type());

    // Set color bit is piece = White
    board[square.index()] = (piece->color() == PieceColor::White) ? (boardPiece |= 1 << 3)
                                                                  : boardPiece;  // Could be: boardPiece += white;
}

int Board::pieceTypeIntoInt(const PieceType &pieceType)
{
    switch (pieceType)
    {
        case PieceType::Pawn:
            return pawn;
        case PieceType::Knight:
            return knight;
        case PieceType::Bishop:
            return bishop;
        case PieceType::Rook:
            return rook;
        case PieceType::Queen:
            return queen;
        case PieceType::King:
            return king;
        default:
            return empty;
    }
}

Piece::Optional Board::piece(const Square &square) const
{
    int boardPiece = board[square.index()];

    // Check if square is empty (!= 0)
    if (boardPiece != 0)
    {
        PieceColor color;
        PieceType type;
        // Check if color bit = 1 (White)
        if ((boardPiece >> 3) & 1)
        {
            color = PieceColor::White;
            type = Board::intToPieceType.at(boardPiece &= ~(1 << 3));
        } else
        {
            color = PieceColor::Black;
            type = Board::intToPieceType.at(boardPiece);
        }
        return Piece(color, type);
    } else return std::nullopt;
}

void Board::setTurn(PieceColor turn)
{
    turn_ = turn;
}

PieceColor Board::turn() const
{
    return turn_;
}

void Board::setCastlingRights(CastlingRights cr)
{
    (void) cr;
}

CastlingRights Board::castlingRights() const
{
    return CastlingRights::None;
}

void Board::setEnPassantSquare(const Square::Optional &square)
{
    (void) square;
}

Square::Optional Board::enPassantSquare() const
{
    return std::nullopt;
}

void Board::makeMove(const Move &move)
{
    board[move.to().index()] = board[move.from().index()];
    board[move.from().index()] = empty;
    if(move.promotion().has_value())  board[move.to().index()] = move.promotion().value()
}

void Board::pseudoLegalMoves(MoveVec &moves) const
{
    (void) moves;
}

void Board::pseudoLegalMovesFrom(const Square &from,
                                 Board::MoveVec &moves) const
{
    (void) from;
    (void) moves;
}

std::ostream &operator<<(std::ostream &os, const Board &board)
{
    (void) board;
    return os;
}
