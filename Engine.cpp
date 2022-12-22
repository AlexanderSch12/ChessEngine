#include "Engine.hpp"

#include <utility>
#include <iostream>
#include <random>


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

    //ztable
    std::random_device randomDevice;
    std::mt19937_64 gen64bit(randomDevice());
    std::uniform_int_distribution<unsigned long long> dis(0, UINT64_MAX);

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            zTable[i][j] = dis(gen64bit);
        }
    }
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
    transpositions.clear();
    auto pv = PrincipalVariation();
    pv.mate = false;
//    int maxScore;
    std::vector legalMoves = Board::MoveVec();
    board.pseudoLegalMoves(legalMoves);

    if (legalMoves.empty())
    {
        if (board.isKingCheck(board.getBoardTurn())) pv.mate = true;
        else pv.setScore(0);

        return pv;
    }

    for (int depth = 1; depth <= 5; depth++)
    {
        int alpha = neg_inf;
        int beta = inf;
//        maxScore = neg_inf - 1;
//        for (Move &move: legalMoves)
//        {
//            auto pv_buf = PrincipalVariation();
//            PreviousState prev_state{};
//            board.makeMoveSaveState(move, prev_state);
        negamax(board, depth - 1, -beta, -alpha, pv);
//            board.reverseMove(prev_state);
//            if (eval > maxScore)
//            {
//                maxScore = eval;
//                pv.moves().clear();
//                pv.moves().push_back(move);
//                pv.mate = pv_buf.mate;
//                for (const auto &move_buf: pv_buf)
//                {
//                    pv.moves().emplace_back(move_buf);
//                }
//            }
//
//            if (maxScore > alpha)
//            {
//                alpha = maxScore;
//            }
//            if (alpha >= beta) break;
//        }
//
//    }
//    pv.setScore(maxScore);
    }
    return pv;
    (void) time;
}

void Engine::setHashSize(std::size_t)
{}

int Engine_::getIndex(int piece)
{
    int index = 0;
    switch (Board::getType(piece))
    {
        case Board::pawn:
            index = 0;
            break;
        case Board::knight:
            index = 1;
            break;
        case Board::bishop:
            index = 2;
            break;
        case Board::rook:
            index = 3;
            break;
        case Board::queen:
            index = 4;
            break;
        case Board::king:
            index = 5;
            break;
        default:
            index = 0;
            break;
    }
    if (Board::getColor(piece) == Board::white) index += 6;
    return index;
}

unsigned long long int Engine_::zHash(Board &board)
{
    unsigned long long int zHash = 0;
    for (int i = 0; i < 64; i++)
    {
        auto piece = board.board()[i];
        if (!Board::isEmpty(piece))
        {
            zHash ^= zTable[i][getIndex(piece)];
        }
    }
    return zHash;
}

//void Engine_::sortMoves(std::vector<Move> &moves)
//{
//
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

int Engine_::negamax(Board &board, int depth, int alpha, int beta, PrincipalVariation &pv)
{
    auto hash = zHash(board);
    if (transpositions.find(hash) != transpositions.end())
    {
        auto item = transpositions.at(hash);
        if (item.depth >= depth)
        {
            switch (item.type_)
            {
                case 1:
                    return item.eval;
                case 2:
                    if (item.eval > alpha) alpha = item.eval;
                    break;
                case 3:
                    if (item.eval < beta) beta = item.eval;
                    break;
            }
            if(alpha >= beta) return item.eval;
        }
    }

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

    int type = 3;
    unsigned bestMove_i = 0;
    auto pv_buf = PrincipalVariation();
    for (unsigned i = 0 ; i<legalMoves.size() ; i++)
    {
        PreviousState prev_state{};
        board.makeMoveSaveState(legalMoves[i], prev_state);
        auto eval = -negamax(board, depth - 1, -beta, -alpha,pv_buf);
        board.reverseMove(prev_state);

        if (eval >= beta)
        {
            HashInfo info(legalMoves[i],depth,beta,2);
            transpositions.insert({hash,info});
            return beta;
        }
        if (eval > alpha)
        {
            bestMove_i = i;
            type = 1;
            alpha = eval;
            pv.moves().clear();
            pv.moves().push_back(legalMoves[i]);
            pv.mate = pv_buf.mate;
            for (const auto &move_buf: pv_buf)
            {
                pv.moves().emplace_back(move_buf);
            }
        }
    }
    HashInfo info(legalMoves[bestMove_i],depth,alpha,type);
    transpositions.insert({hash,info});
    return alpha;
}

HashInfo::HashInfo(Move move, int d , int e, int type)
:defaultSize(0),bestMove(std::move(move)),depth(d),eval(e),type_(type),minSize(0),maxSize(0)
{

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
        auto piece = board.board()[move.to().index()];
        if (!Board::isEmpty(piece) && Board::getColor(piece) != board.getBoardTurn())
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

