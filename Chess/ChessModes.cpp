#include "ChessModes.hpp"
#include "FenNotations.hpp"

Core::Ref<ForsythEdwards> RapidMode::GetFenStrategy() const
{
    return Core::CreateRef<ForsythEdwards>();    
}
