#include "Piece.hpp"

#include <ostream>
#include <cctype>

Piece::Piece(PieceColor color, PieceType type):
    pieceColor(color), pieceType(type)
{

}

Piece::Optional Piece::fromSymbol(char symbol)
{
    switch (symbol)
    {
        case 'P':
            return Piece(PieceColor::White, PieceType::Pawn);
        case 'p':
            return Piece(PieceColor::Black, PieceType::Pawn);
        case 'N':
            return Piece(PieceColor::White, PieceType::Knight);
        case 'n':
            return Piece(PieceColor::Black, PieceType::Knight);
        case 'B':
            return Piece(PieceColor::White, PieceType::Bishop);
        case 'b':
            return Piece(PieceColor::Black, PieceType::Bishop);
        case 'R':
            return Piece(PieceColor::White, PieceType::Rook);
        case 'r':
            return Piece(PieceColor::Black, PieceType::Rook);
        case 'Q':
            return Piece(PieceColor::White, PieceType::Queen);
        case 'q':
            return Piece(PieceColor::Black, PieceType::Queen);
        case 'K':
            return Piece(PieceColor::White, PieceType::Queen);
        case 'k':
            return Piece(PieceColor::Black, PieceType::King);
        default:
            return std::nullopt;
    }

}

PieceColor Piece::color() const
{
    return this->pieceColor;
}

PieceType Piece::type() const
{
    return this->pieceType;
}

bool operator==(const Piece &lhs, const Piece &rhs)
{
    if (lhs.color() == rhs.color() && lhs.type() == rhs.type())
        return true;
    else return false;
}

std::ostream &operator<<(std::ostream &os, const Piece &piece)
{
    char piece_char;
    switch (piece.type())
    {
        case PieceType::Pawn:
            piece_char = 'p';
            break;
        case PieceType::Knight:
            piece_char = 'n';
            break;
        case PieceType::Bishop:
            piece_char = 'b';
            break;
        case PieceType::Rook:
            piece_char = 'r';
            break;
        case PieceType::Queen:
            piece_char = 'q';
            break;
        case PieceType::King:
            piece_char = 'k';
            break;
    }
    if (piece.color() == PieceColor::White)
    {
        piece_char = std::toupper(piece_char);
    }
    return os << piece_char;
}

PieceColor operator!(PieceColor color)
{
    if (color == PieceColor::White)
        return PieceColor::Black;
    else return PieceColor::White;
}
