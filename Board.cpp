#include "Board.hpp"

#include <ostream>


Board::Board()
{
    for( int i = 0 ; i<64 ; i++) board_[i] = empty;
}

void Board::setPiece(const Square &square, const Piece::Optional &piece)
{
    board_[square.index()] = pieceToInt(piece);
}


Piece::Optional Board::piece(const Square &square) const
{
    int piece = board_[square.index()];
    if(isEmpty(piece)) return std::nullopt;

    PieceColor color = isWhite(piece) ? PieceColor::White : PieceColor::Black;
    PieceType type = intToPieceType(piece);


 return Piece(color,type);
}

void Board::setTurn(PieceColor turn)
{
    turn_ = turn == PieceColor::White ? white : black;
}

PieceColor Board::turn() const
{
    return isWhite(turn_) ? PieceColor::White : PieceColor::Black;
}

void Board::setCastlingRights(CastlingRights cr)
{
    cr_ = cr;
}

CastlingRights Board::castlingRights() const
{
    return cr_;
}

void Board::setEnPassantSquare(const Square::Optional &square)
{
    ep_ = square;
}

Square::Optional Board::enPassantSquare() const
{
    return ep_;
}

void Board::makeMove(const Move &move)
{
    unsigned to = move.to().index();
    unsigned from = move.from().index();

    board_[to] = board_[from];
    board_[from] = 0;
    if (move.promotion().has_value())
    {
        board_[to] = getColor(board_[to]) | pieceTypeToInt(move.promotion().value());
    }
}

const int* Board::board() const
{
    return board_;
}

void Board::pseudoLegalMoves(MoveVec &moves) const
{
    for (int index = 0; index < 64; index++)
    {
        auto piece = board_[index];
        if (!isEmpty(piece) && getColor(piece) == turn_)
        {
            pseudoLegalMovesFrom(Square(index), moves);
        }
    }
}

void Board::pseudoLegalMovesFrom(const Square &from, Board::MoveVec &moves) const
{
    Square::Index position = from.index();
    int piece = board_[position];
    if (isEmpty(piece)) return;

    Square::Coordinate col = from.file();
    Square::Coordinate row = from.rank();

    switch (getType(piece))
    {
        case king:
        {
            if (col > 0 && row < 7) addMove(moves, position, position + 7); // Left - Up
            if (row < 7) addMove(moves, position, position + 8); // Up
            if (col < 7 && row < 7) addMove(moves, position, position + 9); // Right - Up
            if (col < 7) addMove(moves, position, position + 1); // Right
            if (col < 7 && row > 0) addMove(moves, position, position - 7); // Right - Down
            if (row > 0) addMove(moves, position, position - 8); // Down
            if (col > 0 && row > 0) addMove(moves, position, position - 9); // Left - Down
            if (col > 0) addMove(moves, position, position - 1); // Left
            // TODO: Add CastlingRights
            break;
        }
        case pawn:
            if (turn_ == white)
            {
                if (isEmpty(board_[position + 8])) addMovePawn(moves, position, position + 8); // Move up
                if (isEmpty(board_[position + 16]) && isEmpty(board_[position + 8]) &&
                    row == 1) // Move up 2 starting position
                    addMovePawn(moves, position, position + 16);
                if (col > 0 && !isEmpty(board_[position + 7]) &&
                    getColor(board_[position + 7]) != turn_)
                    addMovePawn(moves, position, position + 7);
                if (col < 7 && !isEmpty(board_[position + 9]) &&
                        getColor(board_[position + 9]) != turn_)
                    addMovePawn(moves, position, position + 9);
                // TODO: Add EnPassant
            } else
            {
                if (isEmpty(board_[position - 8])) addMovePawn(moves, position, position - 8); // Move up
                if (isEmpty(board_[position - 16]) && isEmpty(board_[position - 8]) &&
                    row == 6) // Move up 2 starting position
                    addMovePawn(moves, position, position - 16);
                if (col > 0 && !isEmpty(board_[position - 9]) &&
                    getColor(board_[position - 9]) != turn_)
                    addMovePawn(moves, position, position - 9);
                if (col < 7 && isEmpty(board_[position - 7]) &&
                        getColor(board_[position - 7]) != turn_)
                    addMovePawn(moves, position, position - 7);
                // TODO: Add EnPassant
            }
            break;
        case knight:
            if (row <= 5 && col >= 1) addMove(moves, position, position + 15);
            if (row <= 5 && col <= 6) addMove(moves, position, position + 17);
            if (row <= 6 && col <= 5) addMove(moves, position, position + 10);
            if (row <= 6 && col >= 2) addMove(moves, position, position + 6);

            if (row >= 2 && col <= 6) addMove(moves, position, position - 15);
            if (row >= 2 && col >= 1) addMove(moves, position, position - 17);
            if (row >= 1 && col >= 2) addMove(moves, position, position - 10);
            if (row >= 1 && col <= 5) addMove(moves, position, position - 6);
            break;
        case queen:
        case bishop:
            for (unsigned i = position + 7; i % 8 > 0 && i / 8 < 7; i += 7) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position - 7; i % 8 < 7 && i / 8 > 0; i -= 7) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position + 9; i % 8 < 7 && i / 8 < 7; i += 9) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position - 9; i % 8 > 0 && i / 8 > 0; i -= 9) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            if (getType(piece) == bishop) break;
            [[fallthrough]];
        case rook:
            for (unsigned i = position + 8; i < position + row * 8; i += 8) // Up
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position - 8; i > position - row * 8; i -= 8) // Down
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position - 1; i >= position - col; i--) // Left
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (unsigned i = position + 1; i <= position + (7 - col); i++) // Right
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
        default:
            break;
    }
}

void Board::addMove(Board::MoveVec &moves, Square::Index from, Square::Index to) const
{
    int pieceTo = board_[to];
    if (isEmpty(pieceTo) || (!isEmpty(pieceTo) && getColor(pieceTo) != turn_))
    {
        moves.push_back(Move(from, Square(to)));
    }
}

void Board::addMovePawn(Board::MoveVec &moves, Square::Index from, Square::Index to)
{
    if (to >= 8 && to < 56)
        moves.push_back(Move(from, Square(to)));
    else
    {
        moves.push_back(Move(Square(from), Square(to), PieceType::Queen));
        moves.push_back(Move(Square(from), Square(to), PieceType::Rook));
        moves.push_back(Move(Square(from), Square(to), PieceType::Bishop));
        moves.push_back(Move(Square(from), Square(to), PieceType::Knight));
    }
}

bool Board::isEmpty(int piece)
{
    return piece == empty;
}

bool Board::isWhite(int piece)
{
    return (piece >> 3) & 1;
}

int Board::getColor(int piece)
{
    return (piece >> 3) & 1 ? white : black;
}

int Board::getType(int piece)
{
    return piece & typeMask;
}

PieceType Board::intToPieceType(int piece)
{
    switch (getType(piece))
    {
        case pawn:
            return PieceType::Pawn;
        case knight:
            return PieceType::Knight;
        case bishop:
            return PieceType::Bishop;
        case rook:
            return PieceType::Rook;
        case queen:
            return PieceType::Queen;
        case king:
            return PieceType::King;
        default:
            return PieceType::Empty;
    }
}

int Board::pieceTypeToInt(PieceType type)
{
    switch (type)
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
int Board::pieceToInt(const Piece::Optional &piece)
{
    if (!piece.has_value()) return 0;
    int pieceInt = pieceTypeToInt(piece.value().type());
    if (piece.value().color() == PieceColor::White) pieceInt |= 1 << 3;
    return pieceInt;
}

std::ostream &operator<<(std::ostream &os, const Board &board)
{
    // TODO: Make nicer
    for (int i = 56; 0 <= i; i-=8)
    {
        for (int j = 0 ; j < 8 ; j++)
        {
            auto piece = board.board()[i+j];
            if(piece == 0) os << ". ";
            else
            {
                PieceType type = Board::intToPieceType(piece);
                PieceColor color = Board::isWhite(piece) ? PieceColor::White : PieceColor::Black;
                os << Piece(color,type) << " ";
            }
            if (j == 7) os << std::endl;
        }
    }
    return os;
}

