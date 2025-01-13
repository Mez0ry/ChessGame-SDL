#ifndef __BOARD_HPP__
#define __BOARD_HPP__
#include <vector>
#include <functional>
#include <unordered_map>
#include <queue>
#include <array>

#include "FenNotations.hpp"
#include "ChessModes.hpp"

#include "ObjectSize.hpp"

#include "Window.hpp"
#include "Renderer.hpp"

#include "Memento.hpp"
#include "KeyFrame.hpp"

/**@brief pieces/entities */
#include "Piece.hpp"

/**@brief commands */
class IEntityCommand;
class MoveTo;

class EventHandler;

constexpr const char* g_starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class Board{
private:
    Core::Ref<Renderer> m_Renderer;
    Core::Ref<Window> m_Window;
public:
    Board(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window);
    ~Board();

    void Setup(ObjectSize board_size,Core::Ref<IChessModes> chess_mode,const std::string& board_path,ObjectSize one_square_src_size);

    void OnCreate();
    void OnResize([[maybe_unused]] const Core::Ref<Window> window);
    void OnDestroy();

    void HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler);
    void Update(float dt);
    void Render(const Core::Ref<Renderer> renderer);
public:
    Vec2i GetBoardTopLeft() const;
    /**
     * @brief get the board size
     * @returns amount of squares by width and height
     */
    ObjectSize GetBoardSize() const;

    ObjectSize GetOneSquareSize() const;

public:
    bool IsOnBoard( const Core::Ref<IEntity>& entity) const;
    bool IsOnBoard( const Vec2i& pos) const;

    void LoadBoardFromFen(const char* fen);
    
    void SetTextureEntityPosition(const Core::Ref<IEntity>& entity);
public:
/**
 * @brief adds piece to the board
*/
    void AddPiece(const Core::Ref<Piece> piece);
/**
 * @brief removes piece from the board
*/
    void RemovePiece(const Core::Ref<Piece> piece);

    std::vector<Core::Ref<Piece>>& GetPieces();

/**
 * @brief kills the Piece
*/
    void KillPieceAt(const Vec2i& square);

/**
 * @brief searches for the piece
 * @returns on success Piece otherwise nullptr
 * @param pred predicate to be used in 'find' algo
*/
    Core::Ref<Piece> FindPiece(const std::function<bool(const Core::Ref<Piece>)>& pred) const;

    bool SquareIsOccupiedByEnemy(const Vec2i& square_pos, Team curr_team);

    bool SquareIsOccupied(const Vec2i& square_pos) const;

    static Vec2i GetRelativePos(Board &board, const Vec2i& board_pos);
    
    static Vec2i GetRelativePos(Board& board,const Core::Ref<IEntity> entity);

public:
    void GenerateLegalMoves(const Core::Ref<Piece> piece);
    void GenerateRookLegalMoves(const Core::Ref<Piece> piece);
    void GenerateBishopLegalMoves(const Core::Ref<Piece> piece);

    void ForEachPiece(const std::function<void(const Core::Ref<Piece>)>& callback);

    bool IsMakeableMove(const Core::Ref<Piece> piece,Vec2i move_to);
    void MakeMove(const Core::Ref<Piece> piece, Vec2i move);
private:
    void SetupPieceTextures();

private:
    std::vector<Core::Ref<Piece>> m_Pieces;
    std::vector<Core::Ref<IEntityCommand>> m_EntityCommands;
    Team m_TeamToMove;
private:
    Texture m_WhiteSquareTexture, m_BlackSquareTexture;
    ObjectSize m_BoardSize; // squares size
    ObjectSize m_OneSquareSize,m_OneSquareSizeSrc;
    ObjectSize m_EntityAdjustedSize;
    ObjectSize m_FullBoardSize;
    Vec2i m_BoardTopLeft;
private: // Smooth stuff
    Stellar::KeyFrame m_BoardTopLeftKF;
    Stellar::KeyFrame m_PieceOpacityKF;
    Memento<Vec2i> m_BoardTopLeftMemento;
private:
    Core::Ref<IChessModes> m_pChessMode;
    Core::Ref<IFenNotation> m_pFenNotation;
private:
};

#endif //!__BOARD_HPP__