#include "Board.hpp"
#include <ostream>

const int Board::knight_jumps[8] = {-17, -15, -10, -6, 6, 10, 15, 17};

Board::Board() :
        board_(64, 0), ep_(-1)
{
}

void Board::setPiece(const Square &square, const Piece::Optional &piece)
{
    int boardPiece = pieceToInt(piece);
    board_[square.index()] = boardPiece;
}


Piece::Optional Board::piece(const Square &square) const
{
    int piece = board_[square.index()];
    if (isEmpty(piece)) return std::nullopt;

    PieceColor color = isWhite(piece) ? PieceColor::White : PieceColor::Black;
    PieceType type = intToPieceType(piece);

    return Piece(color, type);
}

void Board::setTurn(PieceColor turn)
{
    turn_ = turn == PieceColor::White ? white : black;
}

PieceColor Board::turn() const
{
    return isWhite(turn_) ? PieceColor::White : PieceColor::Black;
}

int Board::getBoardTurn() const
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
    ep_ = square->index();
}

Square::Optional Board::enPassantSquare() const
{
    if(ep_ == -1) return std::nullopt;
    else return Square(ep_);
}

void Board::makeMove(const Move &move)
{
    PreviousState prevState;
    makeMoveSaveState(move,prevState);
}

PreviousState Board::makeMoveSaveState(const Move &move,PreviousState& state)
{
    int to = move.getTo();
    int from = move.getFrom();


    state.cr = cr_;
    state.turn = turn_;
    state.ep = ep_;
    state.from = from;
    state.to = to;
    state.movingPiece = board_[from];
    state.captured = board_[to];

    setCastling(from, to, false);
    setEnPassant(from, to, false);


    board_[to] = board_[from];
    board_[from] = empty;

    if (move.promotion().has_value())
    {
        board_[to] = getColor(board_[to]) | pieceTypeToInt(move.promotion().value());
    }
    turn_ == white ? turn_ = black : turn_ = white;
    return state;
}


void Board::reverseMove(PreviousState& state)
{
    int to = state.to;
    int from = state.from;

    turn_ = state.turn;

    board_[from] = state.movingPiece;
    board_[to] = state.captured;

    setCastling(from, to, true);
    setEnPassant(from, to, true);
    cr_ = state.cr;
    ep_ = state.ep;
}

void Board::setCastling(int from, int to, bool reverse)
{
    if (getType(board_[from]) == king)
    {
        if (getColor(board_[from]) == white)
        {
            if (from == 4)
            {
                if (!reverse) cr_ &= ~CastlingRights::White;
                if (to == 2)
                {
                    if (reverse)
                    {
                        board_[0] = board_[3];
                        board_[3] = empty;
                    } else
                    {
                        board_[3] = board_[0];
                        board_[0] = empty;
                    }

                } else if (to == 6)
                {
                    if (reverse)
                    {
                        board_[7] = board_[5];
                        board_[5] = empty;
                    } else
                    {
                        board_[5] = board_[7];
                        board_[7] = empty;
                    }
                }
            }
        } else
        {
            if (from == 60)
            {
                if (!reverse) cr_ &= ~CastlingRights::Black;
                if (to == 58)
                {
                    if (reverse)
                    {
                        board_[56] = board_[59];
                        board_[59] = empty;
                    } else
                    {
                        board_[59] = board_[56];
                        board_[56] = empty;
                    }
                } else if (to == 62 && getType(board_[63]) == rook)
                {
                    if (reverse)
                    {
                        board_[63] = board_[61];
                        board_[61] = empty;
                    } else
                    {
                        board_[61] = board_[63];
                        board_[63] = empty;
                    }
                }
            }
        }
    }

    if (!reverse)
    {
        rookCastled(from);
        rookCastled(to);
    }
}

void Board::rookCastled(int position)
{
    if (getType(board_[position]) == rook)
    {
        if (getColor(board_[position]) == white)
        {
            if (position == 0) cr_ &= ~CastlingRights::WhiteQueenside;
            else if (position == 7) cr_ &= ~CastlingRights::WhiteKingside;
        } else
        {
            if (position == 56) cr_ &= ~CastlingRights::BlackQueenside;
            else if (position == 63) cr_ &= ~CastlingRights::BlackKingside;
        }
    }
}

void Board::setEnPassant(int from, int to, bool reverse)
{
    if (getType(board_[from]) == pawn)
    {
        if (getColor(board_[from]) == white)
        {
            if (from / 8 == 1 && to - from == 16)
            {
                ep_ = from + 8;
            } else if (to == ep_)
            {
                if (reverse)
                {
                    board_[to - 8] = pawn & black;
                } else
                {
                    board_[to - 8] = empty;
                    removeEnPassant();
                }
            }
        } else
        {
            if (from / 8 == 6 && from - to == 16)
            {
                ep_ = from - 8;
            } else if (to == ep_)
            {
                if (reverse)
                {
                    board_[to + 8] = pawn & white;
                } else
                {
                    board_[to + 8] = empty;
                    removeEnPassant();
                }
            }
        }
    } else{
        removeEnPassant();
    }
}

void Board::removeEnPassant()
{
    ep_ = -1;
}

std::vector<int> Board::board() const
{
    return board_;
}

void Board::pseudoLegalMoves(MoveVec &moves)
{
    for (int index = 0; index < 64; index++)
    {
        auto piece = board_[index];
        if (!isEmpty(piece) && getColor(piece) == turn_)
        {
            pseudoLegalMovesFrom(Square(index), moves);
        }
    }
  //  moves = filterLegalMoves(moves);
}

std::vector<Move> Board::filterLegalMoves(MoveVec &moves)
{
    std::vector<Move> legal_moves;
    legal_moves.reserve(moves.size());
    for (auto move: moves)
    {
        PreviousState prevState{};
        makeMoveSaveState(move,prevState);
        if (!isKingCheck(prevState.turn)) legal_moves.push_back(move);
        reverseMove(prevState);
    }
    return legal_moves;
}

void Board::pseudoLegalMovesFrom(const Square &from, Board::MoveVec &moves)
{
    int position = from.index();
    int piece = board_[position];
    if (isEmpty(piece)) return;

    int col = from.file();
    int row = from.rank();

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

            if ((position == 4 && (cr_ & CastlingRights::White) != CastlingRights::None) && turn_ == white &&
                !isAttacked(board_[4]))
            {
                if ((cr_ & CastlingRights::WhiteQueenside) != CastlingRights::None)
                {
                    if (isEmpty(board_[1]) && isEmpty(board_[2]) && isEmpty(board_[3]) && !isAttacked(board_[2]) &&
                        !isAttacked(board_[3]) &&
                        getType(board_[0] == rook && getColor(board_[0]) == white))
                    {
                        addMove(moves, position, 2);
                    }
                }
                if ((cr_ & CastlingRights::WhiteKingside) != CastlingRights::None)
                {
                    if (isEmpty(board_[5]) && isEmpty(board_[6]) &&
                        !isAttacked(board_[5]) && !isAttacked(board_[6]) && !isAttacked(board_[4]) &&
                        getType(board_[7] == rook && getColor(board_[7]) == white))
                    {
                        addMove(moves, position, 6);
                    }
                }
            } else if (position == 60 && (cr_ & CastlingRights::Black) != CastlingRights::None && turn_ == black &&
                       !isAttacked(board_[60]))
            {
                if ((cr_ & CastlingRights::BlackQueenside) != CastlingRights::None)
                {
                    if (isEmpty(board_[59]) && isEmpty(board_[58]) && isEmpty(board_[57]) && !isAttacked(board_[59]) &&
                        !isAttacked(board_[58]) &&
                        getType(board_[56] == rook && getColor(board_[56]) == black))
                    {
                        addMove(moves, position, 58);
                    }
                }
                if ((cr_ & CastlingRights::BlackKingside) != CastlingRights::None)
                {
                    if (isEmpty(board_[61]) && isEmpty(board_[62]) && !isAttacked(board_[61]) &&
                        !isAttacked(board_[62]) &&
                        getType(board_[63] == rook && getColor(board_[63]) == white))
                    {
                        addMove(moves, position, 62);
                    }
                }
            }
            break;
        }
        case pawn:
            if (turn_ == white)
            {
                if (isEmpty(board_[position + 8])) addMovePawn(moves, position, position + 8); // Move up
                if (isEmpty(board_[position + 16]) && isEmpty(board_[position + 8]) &&
                    row == 1) // Move up 2 starting position
                    addMovePawn(moves, position, position + 16);
                if ((col > 0 && !isEmpty(board_[position + 7]) &&
                     getColor(board_[position + 7]) != turn_) || (ep_ == position + 7))
                    addMovePawn(moves, position, position + 7);
                if ((col < 7 && !isEmpty(board_[position + 9]) &&
                     getColor(board_[position + 9]) != turn_) || (ep_ == position + 9))
                    addMovePawn(moves, position, position + 9);
            } else
            {
                if (isEmpty(board_[position - 8])) addMovePawn(moves, position, position - 8); // Move up
                if (isEmpty(board_[position - 16]) && isEmpty(board_[position - 8]) &&
                    row == 6) // Move up 2 starting position
                    addMovePawn(moves, position, position - 16);
                if ((col > 0 && !isEmpty(board_[position - 9]) && getColor(board_[position - 9]) != turn_) ||
                    (ep_ == position - 9))
                    addMovePawn(moves, position, position - 9);
                if ((col < 7 && isEmpty(board_[position - 7]) && getColor(board_[position - 7]) != turn_) ||
                    (ep_ == position - 7))
                    addMovePawn(moves, position, position - 7);
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
            for (int i = position + 7; i % 8 != 7 && i < 64; i += 7) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position - 7; i % 8 != 0 && i >= 0; i -= 7) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position + 9; i % 8 != 0 && i <= 64; i += 9) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position - 9; i % 8 != 7 && i >= 0; i -= 9) //
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            if (getType(piece) == bishop) break;
            [[fallthrough]];
        case rook:
            for (int i = position + 8; i < 64; i += 8) // Up
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position - 8; i >= 0; i -= 8) // Down
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position - 1; i >= position - col; i--) // Left
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
            for (int i = position + 1; i <= position + (7 - col); i++) // Right
            {
                addMove(moves, position, i);
                if (!isEmpty(board_[i])) break;
            }
        default:
            break;
    }
}

void Board::addMove(Board::MoveVec &moves, Square::Index from, Square::Index to)
{
    int pieceTo = board_[to];
    if (isEmpty(pieceTo) || (!isEmpty(pieceTo) && getColor(pieceTo) != turn_))
    {
            Move move = Move(Square(from), Square(to));
            PreviousState prevState{};
            makeMoveSaveState(move,prevState);
            if (!isKingCheck(prevState.turn)) moves.push_back(move);
            reverseMove(prevState);
    }
}

void Board::addMovePawn(Board::MoveVec &moves, Square::Index from, Square::Index to)
{
    Move move = Move(from,to);
    PreviousState prevState{};
    makeMoveSaveState(move,prevState);
    if (!isKingCheck(prevState.turn))
    {
        if (to >= 8 && to < 56)
            moves.push_back(move);
        else
        {
            moves.push_back(Move(Square(from), Square(to), PieceType::Queen));
            moves.push_back(Move(Square(from), Square(to), PieceType::Rook));
            moves.push_back(Move(Square(from), Square(to), PieceType::Bishop));
            moves.push_back(Move(Square(from), Square(to), PieceType::Knight));
        }
    }
    reverseMove(prevState);

}

bool Board::isKingCheck(int color) const
{
    for (int pos = 0; pos < 64; pos++)
    {
        auto piece = board_[pos];
        if (getType(piece) == king && getColor(piece) == color)
        {
            return isAttacked(pos);
        }
    }
    return false;
}

bool Board::isAttacked(int attackedPosition) const
{
    int col = attackedPosition % 8;
    int attackedColor = getColor(board_[attackedPosition]);

    int attacker;
    for (int jump: knight_jumps)
    {
        attacker = board_[attackedPosition + jump];
        if (getType(attacker) == knight && getColor(attacker) != attackedColor) return true;
    }

    for (int i = attackedPosition + 8; i < 64; i += 8) // Up
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == rook ||
                    (attacker_type == king && i == attackedPosition + 8))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition - 8; i >= 0; i -= 8) // Down
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == rook ||
                    (attacker_type == king && i == attackedPosition - 8))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition - 1; i >= attackedPosition - col; i--) // Left
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == rook ||
                    (attacker_type == king && i == attackedPosition - 1))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition + 1; i <= attackedPosition + (7 - col); i++) // Right
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == rook ||
                    (attacker_type == king && i == attackedPosition + 1))
                {
                    return true;
                }
            }
            break;
        }
    }

    for (int i = attackedPosition + 7; i % 8 != 7 && i < 64; i += 7) //
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == bishop ||
                    ((attacker_type == king || (attacker_type == pawn && getColor(attacker) == black)) &&
                     i == attackedPosition + 7))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition - 7; i % 8 != 0 && i >= 0; i -= 7) //
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == bishop ||
                    ((attacker_type == king || (attacker_type == pawn && getColor(attacker) == white)) &&
                     i == attackedPosition - 7))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition + 9; i % 8 != 0 && i <= 64; i += 9) //
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == bishop ||
                    ((attacker_type == king || (attacker_type == pawn && getColor(attacker) == black)) &&
                     i == attackedPosition + 9))
                {
                    return true;
                }
            }
            break;
        }
    }
    for (int i = attackedPosition - 9; i % 8 != 7 && i >= 0; i -= 9) //
    {
        attacker = board_[i];
        if (!isEmpty(attacker))
        {
            if (getColor(attacker) != attackedColor)
            {
                int attacker_type = getType(attacker);
                if (attacker_type == queen || attacker_type == bishop ||
                    ((attacker_type == king || (attacker_type == pawn && getColor(attacker) == white)) &&
                     i == attackedPosition - 9))
                {
                    return true;
                }
            }
            break;
        }
    }
    return false;
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
    for (int i = 56; 0 <= i; i -= 8)
    {
        for (int j = 0; j < 8; j++)
        {
            auto piece = board.board()[i + j];
            if (piece == 0) os << ". ";
            else
            {
                PieceType type = Board::intToPieceType(piece);
                PieceColor color = Board::isWhite(piece) ? PieceColor::White : PieceColor::Black;
                os << Piece(color, type) << " ";
            }
            if (j == 7) os << std::endl;
        }
    }
    return os;
}


