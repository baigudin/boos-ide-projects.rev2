/** 
 * Processor board initialization.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2016-2017 Sergey Baigudin
 * @license   http://baigudin.software/license/
 * @link      http://baigudin.software
 */
#include "boos.Board.hpp"

extern int _main();

/**
 * Initializes a processor board.
 *
 * @param config the operating system configuration.
 * @return true if no errors have been occurred.
 */
bool Board::init(const ::Configuration config)
{
  _main();
  return false;
}

/**
 * Deinitializes a processor board.
 */
void Board::deinit()
{
}
