
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "../Platform/platform.h"
#include "../Chess/chess.h"

namespace Engine {
    
int moveGenerationTest(Chess::Board board, int depth);
Chess::Move getFirstMove(Chess::Board board);

} // namespace Engine


#endif // _ENGINE_H_
