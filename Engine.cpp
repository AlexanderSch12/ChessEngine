#include "Engine.hpp"

#include <utility>
#include <iostream>


Engine_::Engine_(std::string name, std::string version, std::string author) :
        name_(std::move(name)), version_(std::move(version)), author_(std::move(author))
{
    // white = +6
    int piece, type, index;
    for (type = Board::pawn - 1, piece = 0; type <= Board::king - 1; piece += 1, type++)
    {
        for (index = 0; index < 64; index++)
        {
            mg_table[piece][index] = mg_value[type] + mg_pesto_table[type][(index) ^ 56];
            eg_table[piece][index] = eg_value[type] + eg_pesto_table[type][(index) ^ 56];
            mg_table[piece + 6][index] = mg_value[type] + mg_pesto_table[type][index];
            eg_table[piece + 6][index] = eg_value[type] + eg_pesto_table[type][index];
        }
    }

//    std::mt19937 mt(01234567);
//    for(int i = 0 ; i<64 ; i++)
//    {
//        for(int j = 0 ; j < 12 ; j++)
//        {
//            std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
//            zTable[i][j] = dist(mt);
//        }
//    }
}

std::string Engine_::name() const
{
    return name_;
}

std::string Engine_::version() const
{
    return version_;
}

std::string Engine_::author() const
{
    return author_;
}

void Engine_::newGame()
{}

std::optional<HashInfo> Engine::hashInfo() const
{
    return std::nullopt;
}

PrincipalVariation Engine_::pv(Board &board, const TimeInfo::Optional &time)
{
    auto pv = PrincipalVariation();
    pv.mate = false;
    negamax(board, 5, neg_inf, inf, pv);//,pos);

    return pv;
    (void) time;
}

void Engine::setHashSize(std::size_t)
{}

//int Engine_::getIndex(int piece)
//{
//    int index = 0;
//    switch (Board::getType(piece))
//    {
//        case Board::pawn:
//            index = 0;
//            break;
//        case Board::knight:
//            index = 1;
//            break;
//        case Board::bishop:
//            index = 2;
//            break;
//        case Board::rook:
//            index = 3;
//            break;
//        case Board::queen:
//            index = 4;
//            break;
//        case Board::king:
//            index = 5;
//            break;
//        default:
//            index = 0;
//            break;
//    }
//
//    if(Board::getColor(piece) == Board::white) index += 6;
//
//    return index;
//
//}

//unsigned long long int Engine_::getZHash(Board& board)
//{
//    unsigned long long int hash = 0;
//    for (int i = 0 ; i<64 ; i++)
//    {
//        auto piece = board.board()[i];
//        if(!Board::isEmpty(piece))
//        {
//            auto index = getIndex(piece);
//            hash ^= zTable[i][index];
//        }
//    }
//    return hash;
//}


int Engine_::evaluate(Board &board)
{
    int mg[2];
    int eg[2];
    int currentGamePhase = 0;

    // i = 0 (black)
    // i = 1 (white)
    for (int i = 0; i < 2; i++)
    {
        mg[i] = 0;
        eg[i] = 0;
    }

    for (int i = 0; i < 64; ++i)
    {
        int piece = board.board()[i];
        if (!Board::isEmpty(piece))
        {
            int pieceType = Board::getType(piece);
            int pieceColor = Board::getColor(piece);

            int tableColor = pieceColor == Board::white ? 1 : 0;
            int tablePiece = pieceColor == Board::white ? pieceType + 5 : pieceType - 1;
            mg[tableColor] += mg_table[tablePiece][i];
            eg[tableColor] += eg_table[tablePiece][i];
            currentGamePhase += gamePhase[tablePiece];
        }
    }

    int turn = board.getBoardTurn() == Board::white ? 1 : 0;
    int otherTurn = turn == 1 ? 0 : 1;
    int mgScore = mg[turn] - mg[otherTurn];
    int egScore = eg[turn] - eg[otherTurn];
    int mgPhase = currentGamePhase;
    if (mgPhase > 24) mgPhase = 24;
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / 24;
}

//int Engine_::materialScore(Board &board)
//{
//    int turn = board.getBoardTurn();
//    int score = 0;
//    for(int i = 0 ; i<64 ;i++)
//    {
//        auto piece = board.board()[i];
//        if(!Board::isEmpty(piece))
//        {
//            int pieceValue;
//            switch (Board::getType(piece))
//            {
//                case Board::pawn:
//                    pieceValue = pawnValue;
//                    break;
//                case Board::knight:
//                    pieceValue = knightValue;
//                    break;
//                case Board::bishop:
//                    pieceValue = bishopValue;
//                    break;
//                case Board::rook:
//                    pieceValue = rookValue;
//                    break;
//                case Board::queen:
//                    pieceValue = queenValue;
//                    break;
//                default:
//                    pieceValue = 0;
//            }
//            if(Board::getColor(piece) == turn) score += pieceValue;
//            else score -= pieceValue;
//        }
//    }
//    return score;
//}

int Engine_::negamax(Board &board, int depth, int alpha, int beta,
                     PrincipalVariation &pv)//, std::unordered_set<unsigned long long int>& positions)
{
    if (depth == 0)
    {
        pv.moves().clear();
        return evaluate(board);//quiescenceEvaluate(board, alpha, beta);
    }

    std::vector legalMoves = Board::MoveVec();
    board.pseudoLegalMoves(legalMoves);

    if (legalMoves.empty())
    {
        if (board.isKingCheck(board.getBoardTurn()))
        {
            pv.mate = true;
            return neg_inf;
        } else return 0;
    }

    auto pv_buf = PrincipalVariation();
    for (Move &move: legalMoves)
    {
        PreviousState prev_state{};
        board.makeMoveSaveState(move, prev_state);

//        auto zBoard = getZHash(board);
//        if(positions.count(zBoard) >= 3)
//        {
//            board.reverseMove(prev_state);
//            return 0;
//        }
//        positions.insert(zBoard);


        auto eval = -negamax(board, depth - 1, -beta, -alpha, pv_buf);//,positions);


        // positions.erase(zBoard);
        board.reverseMove(prev_state);

        if (eval >= beta) return beta;
        if (eval > alpha)
        {
            alpha = eval;
            pv.moves().clear();
            pv.moves().push_back(move);
            for (const auto &move_buf: pv_buf)
            {
                pv.moves().emplace_back(move_buf);
            }
        }
    }
    return alpha;
}

int Engine_::quiescenceEvaluate(Board &board, int alpha, int beta)
{
    auto eval = evaluate(board);
    if (eval >= beta) return beta;
    if (eval > alpha) alpha = eval;

    auto legalMoves = Board::MoveVec();
    board.pseudoLegalMoves(legalMoves);

    auto captureMoves = Board::MoveVec();

    // Filter to captureOnly moves
    for (Move &move: legalMoves)
    {
        if (board.board()[move.to().index()] != Board::empty)
        {
            captureMoves.push_back(move);
        }
    }

    for (Move &move: captureMoves)
    {
        PreviousState prevState{};
        board.makeMoveSaveState(move, prevState);
        eval = -quiescenceEvaluate(board, -beta, -alpha);
        board.reverseMove(prevState);
        if (eval >= beta) return beta;
        if (eval > alpha)
        {
            alpha = eval;
        }
    }
    return alpha;
}
