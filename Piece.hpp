#ifndef CHESS_ENGINE_PIECE_HPP
#define CHESS_ENGINE_PIECE_HPP

#include <optional>
#include <iosfwd>
#include <unordered_map>

enum class PieceColor
{
    White,
    Black
};

enum class PieceType
{
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

class Piece
{
public:

    using Optional = std::optional<Piece>;

    Piece(PieceColor color, PieceType type);
    Piece(PieceColor color, PieceType type, char symbol);

    static Optional fromSymbol(char symbol);

    PieceColor color() const;
    PieceType type() const;
    void setType(PieceType type);
    char symbol() const;

private:
    PieceColor pieceColor;
    PieceType pieceType;
    char pieceSymbol;
};

char fromPieceTypeToChar(const PieceType &pieceType);

bool operator==(const Piece &lhs, const Piece &rhs);

std::ostream &operator<<(std::ostream &os, const Piece &piece);

std::ostream &operator<<(std::ostream &os, const PieceType &pieceType);

// Invert a color (White becomes Black and vice versa)
PieceColor operator!(PieceColor color);

#endif
