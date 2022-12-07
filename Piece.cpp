#include "Piece.hpp"

#include <ostream>
#include <iostream>
#include <cctype>

Piece::Piece(PieceColor color, PieceType type) :
        pieceColor(color), pieceType(type),
        pieceSymbol(color == PieceColor::White ? toupper(fromPieceTypeToChar(type)) : fromPieceTypeToChar(type))
{
}

Piece::Piece(PieceColor color, PieceType type, char symbol) :
        pieceColor(color), pieceType(type),
        pieceSymbol(symbol)
{
}

Piece::Optional Piece::fromSymbol(char symbol)
{
    PieceColor color = isupper(symbol) ? PieceColor::White : PieceColor::Black;
    switch (tolower(symbol))
    {
        case 'p':
            return Piece(color, PieceType::Pawn, symbol);
        case 'n':
            return Piece(color, PieceType::Knight, symbol);
        case 'b':
            return Piece(color, PieceType::Bishop, symbol);
        case 'r':
            return Piece(color, PieceType::Rook, symbol);
        case 'q':
            return Piece(color, PieceType::Queen, symbol);
        case 'k':
            return Piece(color, PieceType::King, symbol);
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

char Piece::symbol() const
{
    return pieceSymbol;
}

char fromPieceTypeToChar(const PieceType &pieceType)
{
    switch (pieceType)
    {
        case PieceType::Pawn:
            return 'p';
        case PieceType::Knight:
            return 'n';
        case PieceType::Bishop:
            return 'b';
        case PieceType::Rook:
            return 'r';
        case PieceType::Queen:
            return 'q';
        case PieceType::King:
            return 'k';
        default:
            throw std::invalid_argument("Not existing PieceType");
    }
}

bool operator==(const Piece &lhs, const Piece &rhs)
{
    return lhs.color() == rhs.color() && lhs.type() == rhs.type();
}

std::ostream &operator<<(std::ostream &os, const Piece &piece)
{
    return os << piece.symbol();
}

std::ostream &operator<<(std::ostream &os, const PieceType &pieceType)
{
    return os << fromPieceTypeToChar(pieceType);
}

PieceColor operator!(PieceColor color)
{
    if (color == PieceColor::White)
        return PieceColor::Black;
    else return PieceColor::White;
}
