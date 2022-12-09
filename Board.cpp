#include "Board.hpp"

#include <ostream>


Board::Board() :
        board_(64, std::nullopt)
{

}

void Board::setPiece(const Square &square, const Piece::Optional &piece)
{
    board_[square.index()] = piece;
}


Piece::Optional Board::piece(const Square &square) const
{
    return board_[square.index()];
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
    board_[from] = std::nullopt;
    if (move.promotion().has_value())
    {
        board_[to]->setType(move.promotion().value());
    }
}

std::vector<Piece::Optional> Board::board() const
{
    return board_;
}

void Board::pseudoLegalMoves(MoveVec &moves) const
{
//    for(int i = 0 ; i<64 ; i++)
//    {
//        moves;
//    }
    (void) moves;
}

void Board::pseudoLegalMovesFrom(const Square &from, Board::MoveVec &moves) const
{
    Square::Index position = from.index();
    Piece::Optional opPiece = board_[position];
    if (!opPiece.has_value()) return;

    Square::Coordinate col = from.file();
    Square::Coordinate row = from.rank();
    Piece piece = opPiece.value();
    switch (piece.type())
    {
        case PieceType::King:
        {
            if (col < 7) addMove(moves, position, position + 1); // Right
            if (col > 0) addMove(moves, position, position - 1); // Left
            if (row < 7) addMove(moves, position, position + 8); // Up
            if (row > 0) addMove(moves, position, position - 8); // Down
            if (col < 7 && row < 7) addMove(moves, position, position + 9); // Right - Up
            if (col > 0 && row < 7) addMove(moves, position, position + 7); // Left - Up
            if (col < 7 && row > 0) addMove(moves, position, position - 7); // Right - Down
            if (col > 0 && row > 0) addMove(moves, position, position - 9); // Left - Down
            // TODO: Add CastlingRights
            break;
        }
        case PieceType::Pawn:
            if (turn_ == PieceColor::White)
            {
                if (isEmpty(position + 8)) addMovePawn(moves, position, position + 8);
                if (isEmpty(position + 16) && isEmpty(position + 8) && row == 1)
                    addMovePawn(moves, position, position + 16);
                if (col < 7 && board_[position + 9].has_value() &&
                    board_[position + 9].value().color() != turn_)
                    addMovePawn(moves, position, position + 9);
                if (col > 0 && board_[position + 7].has_value() &&
                    board_[position + 7].value().color() != turn_)
                    addMovePawn(moves, position, position + 7);
                // TODO: Add EnPassant
            } else
            {
                if (isEmpty(position - 8)) addMovePawn(moves, position, position - 8);
                if (isEmpty(position - 16) && isEmpty(position - 8) && row == 1)
                    addMovePawn(moves, position, position - 16);
                if (col < 7 && board_[position - 7].has_value() &&
                    board_[position - 7].value().color() != turn_)
                    addMovePawn(moves, position, position - 7);
                if (col > 0 && board_[position - 9].has_value() &&
                    board_[position - 9].value().color() != turn_)
                    addMovePawn(moves, position, position - 9);
                // TODO: Add EnPassant
            }
            break;
        case PieceType::Knight:
            break;
        case PieceType::Queen:
        case PieceType::Rook:
            break;
        case PieceType::Bishop:
            break;

    }
    (void) moves;
}

void Board::addMove(Board::MoveVec &moves, Square::Index from, Square::Index to) const
{
    Piece::Optional pieceTo = board_[to];
    if (isEmpty(to) || (pieceTo.has_value() && pieceTo->color() != turn_))
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
        moves.push_back(Move(Square(from), Square(to), PieceType::Rook));
        moves.push_back(Move(Square(from), Square(to), PieceType::Knight));
        moves.push_back(Move(Square(from), Square(to), PieceType::Bishop));
        moves.push_back(Move(Square(from), Square(to), PieceType::Queen));
    }
}

bool Board::isEmpty(Square::Index index) const
{
    return !board_[index].has_value();
}

std::ostream &operator<<(std::ostream &os, const Board &board)
{
    // TODO: Make nicer
    for (int i = 63; 0 <= i; i--)
    {
        if (board.board()[i].has_value()) os << board.board()[i].value().symbol() << " ";
        else os << ". ";
        if (i != 0 && (i % 8) == 0) os << std::endl;
    }
    return os;
}

