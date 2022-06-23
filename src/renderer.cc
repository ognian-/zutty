/* This file is part of Zutty.
 * Copyright (C) 2020 Tom Szilagyi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * See the file LICENSE for the full license.
 */

#include "renderer.h"

#include <cassert>

namespace zutty
{
   Renderer::Renderer (shared_state& sh_state,
                       const std::function <void ()>& initDisplay,
                       const std::function <void ()>& swapBuffers_,
                       Fontpack* fontpk)
      : sh_state_{sh_state}
      , swapBuffers {swapBuffers_}
      , nextFrame{sh_state}
      , thr (&Renderer::renderThread, this, initDisplay, fontpk)
   {
   }

   Renderer::~Renderer ()
   {
      std::unique_lock <std::mutex> lk (sh_state_.renderer_mtx_);
      done = true;
      nextFrame.seqNo = ++seqNo;
      lk.unlock ();
      sh_state_.renderer_cond_.notify_one ();
      thr.join ();
   }

   void
   Renderer::update (const Frame& frame)
   {
      std::unique_lock <std::mutex> lk (sh_state_.renderer_mtx_);
      nextFrame = frame;
      nextFrame.seqNo = ++seqNo;
      lk.unlock ();
      sh_state_.renderer_cond_.notify_one ();
   }

   void
   Renderer::renderThread (const std::function <void ()>& initDisplay,
                           Fontpack* fontpk)
   {
      initDisplay ();

      charVdev = std::make_unique <CharVdev> (fontpk);

      Frame lastFrame{sh_state_};
      bool delta = false;

      while (1)
      {
         std::unique_lock <std::mutex> lk (sh_state_.renderer_mtx_);
         sh_state_.renderer_cond_.wait (lk,
                    [&] ()
                    {
                       return lastFrame.seqNo != nextFrame.seqNo;
                    });

         if (done)
            return;

         if (lastFrame.seqNo + 1 != nextFrame.seqNo)
            delta = false;

         lastFrame = nextFrame;
         
         if (charVdev->resize (lastFrame.winPx, lastFrame.winPy))
            delta = false;

         {
            CharVdev::Mapping m = charVdev->getMapping ();
            assert (m.nCols == lastFrame.nCols);
            assert (m.nRows == lastFrame.nRows);

            if (delta)
               lastFrame.deltaCopyCells (m.cells);
            else
               lastFrame.fullCopyCells (m.cells);
         }

         charVdev->setDeltaFrame (delta);
         charVdev->setCursor (lastFrame.getCursor ());
         charVdev->setSelection (lastFrame.getSnappedSelection ());

         if (lastFrame.seqNo == nextFrame.seqNo)
         {
            charVdev->draw ();
            swapBuffers ();
            delta = true;
         }
         else
         {
            // skip drawing outdated frame; force full redraw next time
            delta = false;
         }
      }
   }

} // namespace zutty
