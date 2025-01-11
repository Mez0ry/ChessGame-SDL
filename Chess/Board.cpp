#include "Board.hpp"
#include "Lerp.hpp"
#include "Easing.hpp"

#include <cctype>
#include <cstdlib>

#include "AABB.hpp"
#include <unordered_map>
#include "EventHandler.hpp"

Board::Board(ObjectSize board_size) : m_TeamToMove(Team::WHITE)
{
    m_BoardSize = board_size;
}


Board::Board(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,const std::string& path,ObjectSize one_square_src_size, ObjectSize board_size) : m_TeamToMove(Team::WHITE), m_Renderer(renderer), m_Window(window), m_BoardSize(board_size)
{
    Setup(renderer,window,path,one_square_src_size);
}

Board::~Board()
{
    
}

void Board::Setup(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,const std::string& path,ObjectSize one_square_src_size)
{
    auto[win_w,win_h] = window->GetWindowSize();

    m_OneSquareSizeSrc = one_square_src_size;
    m_EntityAdjustedSize = ObjectSize(m_OneSquareSizeSrc.GetWidth() * 0.8f, m_OneSquareSizeSrc.GetHeight() * 0.8f);
    
    m_WhiteSquareTexture.LoadTexture(renderer,path);

    m_WhiteSquareTexture.SetRect<SourceRect>({0,0},one_square_src_size);

    m_BlackSquareTexture.LoadTexture(renderer,path);
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

    OnResize(window);

    m_BoardTopLeft.x = m_BoardTopLeftMemento.GetState().x;
    m_BoardTopLeft.y = -m_FullBoardSize.GetHeight();
}

void Board::OnCreate()
{
    
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

}

void Board::OnDestroy()
{
    
}

void Board::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
    const auto& cursor_pos = MouseInput::GetMousePosition();

    //@TODO  lerp from start_drag_pos to final_entity_pos
   static Vec2i initial_drag_pos, target_entity_pos;

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

                    if(IsMakeableMove(piece,move_to)){
                        MakeMove(piece,move_to);

                        Vec2i curr_relative_pos = GetRelativePos((*this),move_to);
                        piece->ApplySmoothMove(initial_drag_pos,curr_relative_pos,move_to,1.f,Stellar::Easing::EaseInOutSine);
                    }

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
    
    for(auto& piece : m_Pieces){
        if(!piece || piece->IsKilled())
            continue;
        
        piece->Update(dt);
    }
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
        if(piece && !piece->IsKilled()){
            if(!piece->IsSmoothMoving()){
                SetTextureEntityPosition(piece);
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

bool Board::EntityIsOutOfBorder(const Core::Ref<IEntity> entity) const
{
   const auto& entity_pos = entity->GetPosition();
   auto brd_tl_state = m_BoardTopLeftMemento.GetState();

   Rect rect(brd_tl_state,m_FullBoardSize);
   
   return (AABB::PointIsOnObject(entity_pos,rect));
}

//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
void Board::LoadBoardFromFen(const char* fen)
{  
    Vec2i new_pos;
    new_pos.x = 0;
    new_pos.y = m_BoardSize.GetHeight() - 1;

    char row_symbol = '/';
    int read_idx = 0;

    std::unordered_map<Core::Ref<Piece>,char> handled_pieces;

    auto is_piece_symbol = [&](char symbol) -> bool{
        auto it = m_cPiecesMap.find(symbol);
        return (it != m_cPiecesMap.end());
    };
    
    size_t space_cnt = 0;
    for(int i = 0; i < strlen(fen);++i){
        if(fen[i] == ' '){
            ++space_cnt;
        }

        auto requirement = [&](const Core::Ref<Piece> piece) -> bool{
            auto piece_type = piece->GetPieceType();
            auto team = piece->GetTeam();

            char piece_type_symbol = (team == Team::WHITE) ? toupper(static_cast<char>(piece->GetPieceType()) ) : static_cast<char>(piece->GetPieceType()) ;
            return (fen[i] == piece_type_symbol && handled_pieces.count(piece) == 0);
        };


        auto founded_piece = FindPiece(requirement);

        if(std::isdigit(fen[i])){
            int digit = std::atoi(&fen[i]);
            new_pos.x += digit;
        }else if(fen[i] == '/'){
            new_pos.x = 0;
            --new_pos.y;
        }else if(founded_piece && space_cnt < 1){
            founded_piece->SetPosition(new_pos);
            ++new_pos.x;
            
            founded_piece->Revive();
            handled_pieces.insert(std::make_pair(founded_piece,fen[i]));
        }

    }
 
}

std::string Board::GenerateBoardFen()
{
    std::string fen_res;

    return fen_res;
}

void Board::SetTextureEntityPosition(const Core::Ref<IEntity> &entity)
{
    if(!entity || EntityIsOutOfBorder(entity)){
        return;
    }

    auto& entity_texture = entity->GetTexture();
    
    Vec2i relative_pos = GetRelativePos((*this),entity);
    
    entity_texture.SetPosition(relative_pos);
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

bool Board::IsMakeableMove(const Core::Ref<Piece> piece, Vec2i move_to)
{
    return true;
}

void Board::MakeMove(const Core::Ref<Piece> piece, Vec2i move)
{
    //piece->SetPosition(move);

}
