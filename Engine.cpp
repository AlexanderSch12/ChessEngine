#include "Engine.hpp"

#include <utility>
#include <iostream>
#include <algorithm>


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
    int maxScore;
    std::vector legalMoves = Board::MoveVec();
    board.pseudoLegalMoves(legalMoves);

    if (legalMoves.empty())
    {
        if (board.isKingCheck(board.getBoardTurn())) pv.mate = true;
        else pv.setScore(0);

        return pv;
    }

    orderMoves(legalMoves,board);

    for(int depth = 1 ; depth <=5 ; depth++)
    {
        int alpha = neg_inf;
        int beta = inf;
        maxScore = neg_inf-1;
        for(Move& move : legalMoves)
        {
            auto pv_buf = PrincipalVariation();
            PreviousState prev_state{};
            board.makeMoveSaveState(move, prev_state);
            auto eval = -negamax(board, depth - 1, -beta, -alpha, pv_buf);
            board.reverseMove(prev_state);
            if(eval>maxScore)
            {
                maxScore = eval;
                pv.moves().clear();
                pv.moves().push_back(move);
                pv.mate = pv_buf.mate;
                for (const auto &move_buf: pv_buf)
                {
                    pv.moves().emplace_back(move_buf);
                }
            }

            if (maxScore > alpha)
            {
                alpha = maxScore;
            }
            if (alpha >= beta) break;
        }

    }
    pv.setScore(maxScore);
    return pv;

    (void) time;
}

void Engine::setHashSize(std::size_t)
{}

int Engine_::evaluate(Board &board)
{
    int mg[2];
    int eg[2];
    int currentGamePhase = 0;
//    int material[2];

    // i = 0 (black)
    // i = 1 (white)
    for (int i = 0; i < 2; i++)
    {
        mg[i] = 0;
        eg[i] = 0;
//        material[i] = 0;
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

           // int pieceValue = eg_value[pieceType-1];

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
    int pesto = (mgScore * mgPhase + egScore * egPhase) / 24;

    return pesto;
}

int Engine_::negamax(Board &board, int depth, int alpha, int beta, PrincipalVariation &pv)
{
    if (depth == 0)
    {
        pv.moves().clear();
        pv.mate = false;
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

    orderMoves(legalMoves,board);

    auto pv_buf = PrincipalVariation();
    for (Move &move: legalMoves)
    {
        PreviousState prev_state{};
        board.makeMoveSaveState(move, prev_state);
        auto eval = -negamax(board, depth - 1, -beta, -alpha, pv_buf);
        board.reverseMove(prev_state);

        if (eval >= beta) return beta;
        if (eval > alpha)
        {
            alpha = eval;
            pv.moves().clear();
            pv.moves().push_back(move);
            pv.mate = pv_buf.mate;
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
    orderMoves(legalMoves,board);

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

void Engine_::orderMoves(std::vector<Move>& moves, Board board)
{
    for (auto &move: moves)
    {
        PreviousState state{};
        board.makeMoveSaveState(move, state);
        move.setScore(evaluate(board));
        board.reverseMove(state);
    }
    std::sort(moves.begin(), moves.end());
}
