#include "Board.hpp"
#include "Lerp.hpp"
#include "Easing.hpp"

Board::Board(ObjectSize board_size)
{
    m_BoardSize = board_size;
}


Board::Board(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,const std::string& path,ObjectSize one_square_src_size, ObjectSize board_size) : m_Renderer(renderer), m_Window(window)
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

    OnResize(window);

    m_BoardTopLeft.x = m_BoardTopLeftMemento.GetState().x;
    m_BoardTopLeft.y = -m_FullBoardSize.GetHeight();
}

void Board::LoadSquareTexture(const std::string& path,ObjectSize square_src_size)
{
    
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

    m_BlackSquareTexture.SetSize(m_OneSquareSize);
    m_WhiteSquareTexture.SetSize(m_OneSquareSize);
}

void Board::OnDestroy()
{
    
}

void Board::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
    
}

void Board::Update(float dt)
{
    m_BoardTopLeftKF.Update(dt);
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
}