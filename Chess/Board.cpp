#include "Board.hpp"
#include "Lerp.hpp"
#include "Easing.hpp"

#include <cctype>
#include <cstdlib>

#include "AABB.hpp"
#include <unordered_map>
#include "EventHandler.hpp"

#include "MoveTo.hpp"

Board::Board(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window) : m_TeamToMove(Team::WHITE), m_Renderer(renderer),m_Window(window)
{
    
}

Board::~Board()
{
    
}

void Board::Setup(ObjectSize board_size,Core::Ref<IChessModes> chess_mode,const std::string& board_path,ObjectSize one_square_src_size)
{
    m_BoardSize = board_size;
    m_pChessMode = chess_mode;
    m_pFenNotation = chess_mode->GetFenStrategy();

    auto[win_w,win_h] = m_Window->GetWindowSize();

    m_OneSquareSizeSrc = one_square_src_size;
    m_EntityAdjustedSize = ObjectSize(m_OneSquareSizeSrc.GetWidth() * 0.8f, m_OneSquareSizeSrc.GetHeight() * 0.8f);
    
    m_WhiteSquareTexture.LoadTexture(m_Renderer,board_path);

    m_WhiteSquareTexture.SetRect<SourceRect>({0,0},one_square_src_size);
    
    m_BlackSquareTexture.LoadTexture(m_Renderer,board_path);
    m_BlackSquareTexture.SetRect<SourceRect>({one_square_src_size.GetWidth(),0},one_square_src_size);
    m_BlackSquareTexture.SetSize(one_square_src_size);
    
    m_BoardTopLeftKF.Setup(4,[&](float t){
        auto memento_state = m_BoardTopLeftMemento.GetState();
        int dy = Stellar::Lerp(m_BoardTopLeft.y, memento_state.y,Stellar::Easing::EaseInSine(t));

        m_BoardTopLeft.y = dy;
    });

    m_PieceOpacityKF.Setup(0.25f,[&](float t){
        uint8_t alpha = Stellar::Lerp(0, 256,Stellar::Easing::EaseInSine(t));
        for(auto& piece : m_Pieces){
            SDL_SetTextureAlphaMod(piece->GetTexture(),alpha);
        }
    });

    m_pFenNotation->SetupCustomBoardPosition("8/2p3p1/8/3B1p2/3N4/1p6/8/2R3Q1 w - - 0 1",*this);
    SetupPieceTextures();

    OnResize(m_Window);

    m_BoardTopLeft.x = m_BoardTopLeftMemento.GetState().x;
    m_BoardTopLeft.y = -m_FullBoardSize.GetHeight();

    m_CaptureMoveHighlightTexture.LoadTexture(m_Renderer,"resources/game/square_highlight/attackable_square.png");
    m_LegalMoveHighlightTexture.LoadTexture(m_Renderer,"resources/game/square_highlight/available_move.png");

    m_CaptureMoveHighlightTexture.SetSize(m_OneSquareSizeSrc);
    m_LegalMoveHighlightTexture.SetSize(m_OneSquareSizeSrc);
}

void Board::OnCreate()
{
    for(auto& piece : m_Pieces){
        this->GenerateLegalMoves(piece);
    }
}

void Board::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
    auto[win_w,win_h] = window->GetWindowSize();
    auto max_size = std::min(win_w,win_h) * 0.9f;

    m_OneSquareSize = ObjectSize(max_size / m_BoardSize.GetWidth(),(max_size / m_BoardSize.GetHeight()));

    m_FullBoardSize = m_OneSquareSize * m_BoardSize;
    Vec2i tmp;

    tmp.x = ((win_w -  max_size) / 2);
    tmp.y = ((win_h -  max_size) / 2);

    m_BoardTopLeftMemento.Save(tmp);
    m_BoardTopLeft.x = tmp.x;

    if(m_BoardTopLeftKF.IsFinished()){
        m_BoardTopLeft = tmp;
    }
    
    m_BlackSquareTexture.SetSize(m_OneSquareSize);
    m_WhiteSquareTexture.SetSize(m_OneSquareSize);

    m_EntityAdjustedSize = ObjectSize(m_OneSquareSize.GetWidth() * 0.8f,m_OneSquareSize.GetHeight() * 0.8f);

    for(auto& piece : m_Pieces){
        if(!piece && piece->IsKilled()) 
            continue;

        piece->SetSize(m_EntityAdjustedSize);
        SetTextureEntityPosition(piece);
    }

    m_CaptureMoveHighlightTexture.SetSize(m_OneSquareSize);
    m_LegalMoveHighlightTexture.SetSize(m_OneSquareSize);

}

void Board::OnDestroy()
{
    
}

void Board::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
   const auto& cursor_pos = MouseInput::GetMousePosition();
   static Vec2i initial_drag_pos;

    if(MouseInput::GetMouseState(MouseState::MOUSE_BUTTON_DOWN)){
        if(MouseInput::IsPressed(SDL_BUTTON_LEFT)){
            for(auto& piece : m_Pieces){
                if(!piece || piece->IsKilled())
                    continue;
                
                auto& piece_texture = piece->GetTexture();

                bool is_on_object = AABB::PointIsOnObject(cursor_pos,piece_texture.GetRect());
                if(is_on_object){
                    initial_drag_pos = piece->GetTexture().GetPosition();
                    piece->Drag();
                }
            }
        }
    }

    if(MouseInput::GetMouseState(MouseState::MOUSE_BUTTON_UP)){

        if(MouseInput::IsReleased(SDL_BUTTON_LEFT)){
            Vec2i diff = cursor_pos - m_BoardTopLeft;

            Vec2i move_to = {diff.x / m_OneSquareSize.GetWidth(),diff.y / m_OneSquareSize.GetHeight()};
            
            for(auto& piece : m_Pieces){
                if(!piece || piece->IsKilled())
                    continue;

                if(piece->IsDragging()){
                    Vec2i curr_relative_pos = GetRelativePos((*this),move_to);

                    Move move;
                    move.piece_to_move = piece;
                    move.move_from = initial_drag_pos;
                    move.move_to = curr_relative_pos;

                    move.board_move_to = move_to;
                    move.board_move_from = piece->GetPosition();
                    
                    auto piece_at = GetPieceAt(move_to);
                    if(piece_at && piece_at != piece->GetTeam()){
                        move.piece_to_kill = piece_at;
                    }

                    move.frame_duration = 1.f;

                    move.easing_type = Stellar::Easing::EaseInSine;

                    m_EntityCommands.push_back(Core::CreateRef<MoveTo>((*this),piece,move));
                    piece->StopDragging();
                }
            }
        }
    }
}

void Board::Update(float dt)
{
    m_BoardTopLeftKF.Update(dt);

    if(m_BoardTopLeftKF.GetElapsedFrames() > 2.0f){
        m_PieceOpacityKF.Update(dt);
    }
   
    m_EntityCommands.erase(std::remove_if(m_EntityCommands.begin(),m_EntityCommands.end(),[&](Core::Ref<IEntityCommand> command){
        CommandStatus status = command->Execute(dt);

        return (status == CommandStatus::FAILED || status == CommandStatus::SUCCESS);
    }),m_EntityCommands.end());
}

void Board::Render(const Core::Ref<Renderer> renderer)
{
    for(uint8_t row = 0; row < static_cast<uint8_t>(m_BoardSize.GetHeight()); row++){
        for(uint8_t col = 0; col < static_cast<uint8_t>(m_BoardSize.GetWidth()); col++){
            Vec2i new_pos = {0,0};
            
            if(~(col ^ row) & 1){
                new_pos.x = m_BoardTopLeft.x + ((m_WhiteSquareTexture.GetSize().GetWidth()) * col);
                new_pos.y = m_BoardTopLeft.y + ((m_WhiteSquareTexture.GetSize().GetHeight()) * row);

                m_WhiteSquareTexture.SetRect(new_pos,m_OneSquareSize);
                renderer->Render(m_WhiteSquareTexture);
            }else{
                new_pos.x = m_BoardTopLeft.x + (m_BlackSquareTexture.GetSize().GetWidth() * col);
                new_pos.y = m_BoardTopLeft.y + (m_BlackSquareTexture.GetSize().GetHeight() * row);

                m_BlackSquareTexture.SetRect(new_pos,m_OneSquareSize);
                renderer->Render(m_BlackSquareTexture);
            }
            
        }
    }

    for(auto& piece : m_Pieces){
        if(IsOnBoard(piece) && piece && !piece->IsKilled()){
            if(m_BoardTopLeftKF.GetElapsedFrames() > 0.7f && !m_BoardTopLeftKF.IsFinished()){
                SetTextureEntityPosition(piece);
            }else if(piece->IsDragging()){
                auto& legal_moves = piece->GetLegalMoves();
                auto& capture_moves = piece->GetCaptureMoves();
                
                for(auto& legal_mv : legal_moves){
                    HighlightSquare(legal_mv,m_LegalMoveHighlightTexture);
                }

                for(auto& capture_mv : capture_moves){
                    HighlightSquare(capture_mv,m_CaptureMoveHighlightTexture);
                }

            }
            piece->Render(renderer);
        }
    }

}

Vec2i Board::GetBoardTopLeft() const
{
    return m_BoardTopLeft;
}

ObjectSize Board::GetBoardSize() const
{
    return m_BoardSize;
}

ObjectSize Board::GetOneSquareSize() const
{
    return m_OneSquareSize;
}

bool Board::IsOnBoard(const Core::Ref<IEntity> &entity) const
{
    const auto& entity_pos = entity->GetPosition();
    return (IsOnBoard(entity_pos));
}

bool Board::IsOnBoard(const Vec2i& pos) const
{
   return (pos.y < m_BoardSize.GetHeight() && pos.x < m_BoardSize.GetWidth() && pos.y > -1 && pos.x > -1);
}

void Board::SetTextureEntityPosition(const Core::Ref<IEntity> &entity)
{
    if(!entity || !IsOnBoard(entity)){
        return;
    }

    auto& entity_texture = entity->GetTexture();
    
    Vec2i relative_pos = GetRelativePos((*this),entity);
    
    entity_texture.SetPosition(relative_pos);
}

void Board::SetTexturePosAtSquare(Board& board, Texture& texture,const Vec2i& square)
{
    Vec2i board_tl = board.GetBoardTopLeft();
    ObjectSize one_square_size = board.GetOneSquareSize();
    
    Vec2i pos_on_board;
    
    pos_on_board.x = board_tl.x + square.x * one_square_size.GetWidth();
    pos_on_board.y = board_tl.y + square.y * one_square_size.GetHeight();

    texture.SetPosition(pos_on_board);
}

void Board::SetCurrentTurn(Team team) {
    m_TeamToMove = team;
}

void Board::AddPiece(const Core::Ref<Piece> piece) {
    piece->SetSize(m_EntityAdjustedSize);
    
    m_Pieces.push_back(piece);
}

void Board::RemovePiece(const Core::Ref<Piece> piece) {
    m_Pieces.erase(std::remove_if(m_Pieces.begin(),m_Pieces.end(),[&](const Core::Ref<Piece> img_piece){
        return (img_piece == piece);
    }),m_Pieces.end());
}

std::vector<Core::Ref<Piece>> &Board::GetPieces()
{
    return m_Pieces;
}

void Board::KillPieceAt(const Vec2i& square) {
    auto piece = FindPiece([&](const Core::Ref<Piece> piece){
        return (piece->GetPosition() == square);
    });

    if(piece && !piece->IsKilled()){
        piece->Kill();
    }
    
}

Core::Ref<Piece> Board::FindPiece(const std::function<bool(const Core::Ref<Piece>)>& pred) const
{
    auto it = std::find_if(m_Pieces.begin(),m_Pieces.end(),pred);
    return (it != m_Pieces.end() ? (*it) : nullptr);
}

Core::Ref<Piece> Board::GetPieceAt(const Vec2i &square)
{
    return FindPiece([&](const Core::Ref<Piece> piece){
        return (piece->GetPosition() == square);
    });
}

bool Board::SquareIsOccupiedByEnemy(const Vec2i &square_pos, Team curr_team)
{
    Team enemy_team = (curr_team == Team::BLACK) ? Team::WHITE : Team::BLACK;

    return (FindPiece([&](const Core::Ref<Piece> img_piece){
                return (img_piece->GetPosition() == square_pos && img_piece->GetTeam() == enemy_team);
            }) != nullptr);
}

bool Board::SquareIsOccupied(const Vec2i &square_pos) const
{
   return (FindPiece([&](const Core::Ref<Piece> piece) -> bool{
        return (piece->GetPosition() == square_pos);
    })) != nullptr;
}

void Board::HighlightSquare(const Vec2i &square, Texture &texture)
{
    if(!IsOnBoard(square)){
        STELLAR_ERROR("square that you are trying to highlight is out of board");
    }

    if(SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND)){
        STELLAR_ERROR("Failed to set texture blend mode");
    }
    
    SetTexturePosAtSquare((*this),texture,square);
    m_Renderer->Render(texture);
}

Vec2i Board::GetRelativePos(Board &board, const Vec2i &board_pos)
{
    Vec2i board_tl = board.GetBoardTopLeft();
    
    ObjectSize one_square_size = board.GetOneSquareSize();
    
    Vec2i relative_pos;
    
    relative_pos.x = board_tl.x + board_pos.x * one_square_size.GetWidth();
    relative_pos.y = board_tl.y + board_pos.y * one_square_size.GetHeight();
    
    ObjectSize offset(one_square_size.GetWidth() * 0.2f, one_square_size.GetHeight() * 0.2f);

    relative_pos.x += (offset.GetWidth() / 2);
    relative_pos.y += (offset.GetHeight() / 2);

    return relative_pos;
}

Vec2i Board::GetRelativePos(Board &board, const Core::Ref<IEntity> entity)
{
    Vec2i ent_pos = entity->GetPosition();
    ObjectSize one_square_size = board.GetOneSquareSize();

    Vec2i relative_ent_pos = GetRelativePos(board,ent_pos);
    
    return relative_ent_pos;
}

void Board::GenerateLegalMoves(const Core::Ref<Piece> piece) {
    PieceType piece_type = piece->GetPieceType();
    Team piece_team = piece->GetTeam();
    std::vector<Vec2i>& piece_legal_moves = piece->GetLegalMoves();
    std::vector<Vec2i>& piece_capture_moves = piece->GetCaptureMoves();

    Vec2i piece_pos = piece->GetPosition();

    Team enemy_team = (piece_team == Team::BLACK) ? Team::WHITE : Team::BLACK;

    if(!piece_legal_moves.empty()){
        piece_legal_moves.clear();
    }
    if(!piece_capture_moves.empty()){
        piece_capture_moves.clear();
    }

    switch(piece_type){
        case PieceType::PAWN:{
            uint8_t seventh_rank = m_BoardSize.GetHeight() - 2;
            uint8_t first_rank = 1;

            Vec2i move_forward = {piece_pos.x, (piece_team == Team::BLACK) ? piece_pos.y - 1 : piece_pos.y + 1 };
            bool forward_square_is_occupied = SquareIsOccupied(move_forward);

            if(!forward_square_is_occupied){
                piece_legal_moves.push_back(move_forward);
            }

            if(piece_pos.y == seventh_rank  || piece_pos.y == first_rank || !forward_square_is_occupied){
                Vec2i special_first_move = {piece_pos.x, (piece_team == Team::BLACK) ? piece_pos.y - 2 : piece_pos.y + 2 };
                if(!SquareIsOccupied(special_first_move))
                    piece_legal_moves.push_back({piece_pos.x, (piece_team == Team::BLACK) ? piece_pos.y - 2 : piece_pos.y + 2 });
            }
            
            
            Vec2i tl_square{piece_pos.x - 1,(piece_team == Team::BLACK) ? piece_pos.y - 1 : piece_pos.y + 1},
                  tr_square{piece_pos.x + 1, (piece_team == Team::BLACK) ? piece_pos.y - 1 : piece_pos.y + 1};
            
            auto piece_at_tl = GetPieceAt(tl_square);
            auto piece_at_tr = GetPieceAt(tr_square);
            
            if(piece_at_tl && piece_at_tl != piece_team){
                piece_legal_moves.push_back(tl_square);
                piece_capture_moves.push_back(tl_square);
            }
            
            if(piece_at_tr && piece_at_tr != piece_team){
                piece_legal_moves.push_back(tr_square);
                piece_capture_moves.push_back(tr_square);
            }

            //@TODO ENPASSANT
            break;
        }

        case PieceType::ROOK:{
            GenerateRookLegalMoves(piece);
            break;
        }

        case PieceType::BISHOP:{
            GenerateBishopLegalMoves(piece);
            break;
        }

        case PieceType::KNIGHT:{

           constexpr std::array<Vec2i,8> unchecked_knight_pos = {Vec2i( - 2,1),
                                                                Vec2i(-2,-1),
                                                                Vec2i(2,1),
                                                                Vec2i(2, -1),
                                                                Vec2i(-1,2),
                                                                Vec2i(1,2),
                                                                Vec2i(-1,-2),
                                                                Vec2i(1,- 2)
           };

            for(auto& offset: unchecked_knight_pos){
                Vec2i possible_pos = piece_pos + offset;
                if(IsOnBoard(possible_pos)){
                    auto piece_at = GetPieceAt(possible_pos);

                    if(piece_at && piece_at != piece_team){
                        piece_capture_moves.push_back(possible_pos);
                        piece_legal_moves.push_back(possible_pos);
                    }else if(piece_at && piece_at == piece_team){
                        
                    }else{
                        piece_legal_moves.push_back(possible_pos);
                    }
                }
            }
            break;
        }
        case PieceType::QUEEN:{
            
            GenerateRookLegalMoves(piece);
            GenerateBishopLegalMoves(piece);
            break;
        }
        
        case PieceType::KING:{

            break;
        }
    }//!switch
}

void Board::GenerateRookLegalMoves(const Core::Ref<Piece> piece) {
    PieceType piece_type = piece->GetPieceType();
    Team piece_team = piece->GetTeam();
    std::vector<Vec2i>& piece_legal_moves = piece->GetLegalMoves();
    std::vector<Vec2i>& piece_capture_moves = piece->GetCaptureMoves();
    Vec2i piece_pos = piece->GetPosition();

    Team enemy_team = (piece_team == Team::BLACK) ? Team::WHITE : Team::BLACK;

    constexpr uint8_t mask[4] = {
        1 << 0,
        1 << 1,
        1 << 2,
        1 << 3
    };

    Vec2i moves_left = {piece_pos.x - 1,piece_pos.y}, // mask[0]
          moves_right = {piece_pos.x + 1,piece_pos.y}, // mask[1]
          moves_up = {piece_pos.x, piece_pos.y - 1}, // mask[2]
          moves_down = {piece_pos.x, piece_pos.y + 1}; // mask[3]

    uint8_t flags = 0;

    for(;((flags ^ (mask[0] | mask[1] | mask[2] | mask[3])) != 0); moves_left.x -= 1, moves_right.x += 1, moves_up.y -= 1, moves_down.y += 1){
        auto piece_at_ml = GetPieceAt(moves_left),
             piece_at_mr = GetPieceAt(moves_right),
             piece_at_mu = GetPieceAt(moves_up), 
             piece_at_md = GetPieceAt(moves_down);

        if(!IsOnBoard(moves_left)){
            flags |= mask[0];
        }else if(piece_at_ml && (flags & mask[0]) == 0){
            if(piece_at_ml != piece_team){
                piece_capture_moves.push_back(moves_left);
                piece_legal_moves.push_back(moves_left);
            }
            flags |= mask[0];
        }else{
            if((flags & mask[0]) == 0){
                piece_legal_moves.push_back(moves_left);
            }
        }

        if(!IsOnBoard(moves_right)){
            flags |= mask[1];
        }else if(piece_at_mr && (flags & mask[1]) == 0){
            if(piece_at_mr != piece_team){
                piece_capture_moves.push_back(moves_right);
                piece_legal_moves.push_back(moves_right);
            }
            flags |= mask[1];
        }
        else{
            if((flags & mask[1]) == 0){
                piece_legal_moves.push_back(moves_right);
            }
        }

        if(!IsOnBoard(moves_up)){
            flags |= mask[2];
        }else if(piece_at_mu && (flags & mask[2]) == 0){
            if(piece_at_mu != piece_team){
                piece_capture_moves.push_back(moves_up);
                piece_legal_moves.push_back(moves_up);
            }

            flags |= mask[2];
        }
        else{
            if((flags & mask[2]) == 0){
                piece_legal_moves.push_back(moves_up);
            }
        }

        if(!IsOnBoard(moves_down)){
            flags |= mask[3];
        }else if(piece_at_md && (flags & mask[3]) == 0){
            if(piece_at_md != piece_team){
                piece_capture_moves.push_back(moves_down);
                piece_legal_moves.push_back(moves_down);
            }

            flags |= mask[3];
        }
        else{
            if((flags & mask[3]) == 0){
                piece_legal_moves.push_back(moves_down);
            }
        }
    }
    
}

void Board::GenerateBishopLegalMoves(const Core::Ref<Piece> piece) {
     PieceType piece_type = piece->GetPieceType();
    Team piece_team = piece->GetTeam();
    std::vector<Vec2i>& piece_legal_moves = piece->GetLegalMoves();
    std::vector<Vec2i>& piece_capture_moves = piece->GetCaptureMoves();
    Vec2i piece_pos = piece->GetPosition();

    Team enemy_team = (piece_team == Team::BLACK) ? Team::WHITE : Team::BLACK;

    constexpr uint8_t mask[4] = {
        1 << 0,
        1 << 1,
        1 << 2,
        1 << 3
    };

    Vec2i moves_tl = {piece_pos.x - 1,piece_pos.y - 1}, // mask[0]
        moves_tr = {piece_pos.x + 1,piece_pos.y - 1}, // mask[1]
        moves_bl = {piece_pos.x - 1, piece_pos.y + 1}, // mask[2]
        moves_br = {piece_pos.x + 1, piece_pos.y + 1}; // mask[3]

    uint8_t flags = 0;

    Vec2i towards_tl_dir = (moves_tl - piece_pos),
        towards_tr_dir = (moves_tr - piece_pos),
        towards_bl_dir = (moves_bl - piece_pos),
        towards_br_dir = (moves_br - piece_pos);

    for(;((flags ^ (mask[0] | mask[1] | mask[2] | mask[3])) != 0); moves_tl += towards_tl_dir, moves_tr += towards_tr_dir,moves_bl += towards_bl_dir,moves_br += towards_br_dir){
        auto piece_at_tl = GetPieceAt(moves_tl),
             piece_at_tr = GetPieceAt(moves_tr),
             piece_at_bl= GetPieceAt(moves_bl), 
             piece_at_br = GetPieceAt(moves_br);

        if(!IsOnBoard(moves_tl)){
            flags |= mask[0];
        }else if(piece_at_tl && (flags & mask[0]) == 0){
            if(piece_at_tl != piece_team){
                piece_capture_moves.push_back(moves_tl);
                piece_legal_moves.push_back(moves_tl);
            }
            flags |= mask[0];
        }else{
            if((flags & mask[0]) == 0){
                piece_legal_moves.push_back(moves_tl);
            }
        }

        if(!IsOnBoard(moves_tr)){
            flags |= mask[1];
        }else if(piece_at_tr && (flags & mask[1]) == 0){
            if(piece_at_tr != piece_team){
                piece_capture_moves.push_back(moves_tr);
                piece_legal_moves.push_back(moves_tr);
            }
            flags |= mask[1];
        }
        else{
            if((flags & mask[1]) == 0){
                piece_legal_moves.push_back(moves_tr);
            }
        }

        if(!IsOnBoard(moves_bl)){
            flags |= mask[2];
        }else if(piece_at_bl && (flags & mask[2]) == 0){
            if(piece_at_bl != piece_team){
                piece_capture_moves.push_back(moves_bl);
                piece_legal_moves.push_back(moves_bl);
            }
            flags |= mask[2];
        }
        else{
            if((flags & mask[2]) == 0){
                piece_legal_moves.push_back(moves_bl);
            }
        }

        if(!IsOnBoard(moves_br)){
            flags |= mask[3];
        }else if(piece_at_br && (flags & mask[3]) == 0){
            if(piece_at_br != piece_team){
                piece_capture_moves.push_back(moves_br);
                piece_legal_moves.push_back(moves_br);
            }
            flags |= mask[3];
        }
        else{
            if((flags & mask[3]) == 0){
                piece_legal_moves.push_back(moves_br);
            }
        }
    }

}

void Board::ForEachAlivePiece(const std::function<void(const Core::Ref<Piece>)> &callback)
{
    for(auto& piece : m_Pieces){
        if(piece->IsKilled()) continue;
        
        callback(piece);
    }
}

bool Board::IsMakeableMove(const Core::Ref<Piece> piece, Vec2i move_to) {
    if(!IsOnBoard(move_to)) 
        return false;
  return (piece->IsLegalMove(move_to) || piece->IsCaptureMove(move_to));
}

void Board::MakeMove(const Core::Ref<Piece> piece, Move& move)
{
    piece->SetPosition(move.board_move_to);
    m_BoardMoves.push(move);
}

void Board::UnmakeMove() {
    if(m_BoardMoves.empty()) 
        return;

    const auto& move = m_BoardMoves.top();
    const auto& piece_to_move = move.piece_to_move;
    const auto& piece_to_revive = move.piece_to_kill;

    if(!piece_to_move)
        return;
    
    piece_to_move->SetPosition(move.board_move_from);
    SetTextureEntityPosition(piece_to_move);

    if(piece_to_revive){
        piece_to_revive->Revive();
    }

    ForEachAlivePiece([&](const Core::Ref<Piece> piece){
        GenerateLegalMoves(piece);
    });

    m_BoardMoves.pop();
}

void Board::SetupPieceTextures() {
    const std::string resource_pawn_name = "piece1.png";
    const std::string resources_knight_name = "piece2.png";
    const std::string resources_bishop_name = "piece3.png";
    const std::string resources_queen_name = "piece4.png";
    const std::string resources_king_name = "piece5.png";
    const std::string resources_rook_name = "piece6.png";
    
    const std::string black_pieces_path_dir = "resources/game/pieces/b/b_";
    const std::string white_pieces_path_dir = "resources/game/pieces/w/w_";

    const std::string b_pawn_texture_path = black_pieces_path_dir + resource_pawn_name;
    const std::string b_rook_texture_path = black_pieces_path_dir + resources_rook_name;
    const std::string b_bishop_texture_path = black_pieces_path_dir + resources_bishop_name;
    const std::string b_knight_texture_path = black_pieces_path_dir + resources_knight_name;
    const std::string b_queen_texture_path = black_pieces_path_dir + resources_queen_name;
    const std::string b_king_texture_path = black_pieces_path_dir + resources_king_name;

    const std::string w_pawn_texture_path = white_pieces_path_dir + resource_pawn_name;
    const std::string w_rook_texture_path = white_pieces_path_dir + resources_rook_name;
    const std::string w_bishop_texture_path = white_pieces_path_dir + resources_bishop_name;
    const std::string w_knight_texture_path = white_pieces_path_dir + resources_knight_name;
    const std::string w_queen_texture_path = white_pieces_path_dir + resources_queen_name;
    const std::string w_king_texture_path = white_pieces_path_dir + resources_king_name;

    std::array<Piece,12> pieces_array = {Piece(m_Renderer,b_pawn_texture_path,PieceType::PAWN,Team::BLACK),
                                         Piece(m_Renderer,b_rook_texture_path,PieceType::ROOK,Team::BLACK),
                                         Piece(m_Renderer,b_bishop_texture_path,PieceType::BISHOP,Team::BLACK),
                                         Piece(m_Renderer,b_knight_texture_path,PieceType::KNIGHT,Team::BLACK),
                                         Piece(m_Renderer,b_queen_texture_path,PieceType::QUEEN,Team::BLACK),
                                         Piece(m_Renderer,b_king_texture_path,PieceType::KING,Team::BLACK),

                                         Piece(m_Renderer,w_pawn_texture_path,PieceType::PAWN,Team::WHITE),
                                         Piece(m_Renderer,w_rook_texture_path,PieceType::ROOK,Team::WHITE),
                                         Piece(m_Renderer,w_bishop_texture_path,PieceType::BISHOP,Team::WHITE),
                                         Piece(m_Renderer,w_knight_texture_path,PieceType::KNIGHT,Team::WHITE),
                                         Piece(m_Renderer,w_queen_texture_path,PieceType::QUEEN,Team::WHITE),
                                         Piece(m_Renderer,w_king_texture_path,PieceType::KING,Team::WHITE)
                                        };
    
    for(auto& piece : pieces_array){
        SDL_SetTextureAlphaMod(piece.GetTexture(),0);
    }
    
    for(auto& piece : m_Pieces){
        if(!piece) continue;
        auto& piece_texture = piece->GetTexture();

        auto it = std::find_if(pieces_array.begin(),pieces_array.end(),[&](const Piece& img_piece){
            return (*piece == img_piece);
        });

        if(it != pieces_array.end()){
            piece_texture.ShareSDLTexture((*it).GetTexture());
        }
        
    }
}
