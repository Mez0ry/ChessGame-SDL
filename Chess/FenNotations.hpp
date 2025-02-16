#ifndef __FEN_NOTATIONS_HPP__
#define __FEN_NOTATIONS_HPP__
#include <vector>
#include <memory>
#include "PieceType.hpp"
#include <array>
#include "Piece.hpp"
#include <string_view>
namespace Core{
    template <typename T>
    using Ref = std::shared_ptr<T>;
};

class Piece;
class Board;

class IFenNotation{
public:
    /**
     * @brief
     * @param fen
     * @param pieces
     * @param board_size
     */
    virtual void SetupCustomBoardPosition(const std::string& fen,Board& board) = 0;
    virtual void SetupStartingBoardPos(Board& board) = 0;

private: 
};

class ForsythEdwards : public IFenNotation{
    public:
    ForsythEdwards() = default;
    ~ForsythEdwards() = default;

    void SetupCustomBoardPosition(const std::string& fen, Board& board) override;
    void SetupStartingBoardPos(Board& board);
    private:
    void ParsePiecePositions(std::string_view pieces_fen,std::vector<Core::Ref<Piece>>& pieces, Vec2i& new_pos);

    private:
    const std::array<char,12> m_PiecesCharArr = {'p', 'r', 'n', 'b', 'q','k',
                                              'P', 'R', 'N', 'B','Q','K'}; 
    const char* m_StartingBoardPosFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
};
#endif //!__FEN_NOTATIONS_HPP__