/*******************************************************************************
* Author    :  Angus Johnson                                                   *
* Date      :  5 July 2024                                                     *
* Website   :  https://www.angusj.com                                          *
* Copyright :  Angus Johnson 2010-2024                                         *
* Purpose   :  FAST rectangular clipping                                       *
* License   :  https://www.boost.org/LICENSE_1_0.txt                           *
*******************************************************************************/

#ifndef CLIPPER_RECTCLIP_H
#define CLIPPER_RECTCLIP_H

#include "clipper2/clipper.core.h"
#include <queue>

namespace Clipper2Lib
{

  // Location: the order is important here, see StartLocsIsClockwise()
  enum class Location { Left, Top, Right, Bottom, Inside };

  template <typename T>
  class OutPt2;

  template <typename T>
  using OutPt2List = std::vector<OutPt2<T>*>;

  template <typename T>
  class OutPt2 {
  public:
      Point<T> pt;
      size_t owner_idx = 0;
      OutPt2List<T>* edge = nullptr;
      OutPt2* next = nullptr;
      OutPt2* prev = nullptr;
  };

  //------------------------------------------------------------------------------
  // RectClip64
  //------------------------------------------------------------------------------

  template <typename T>
  class RectClip {
  private:
      void ExecuteInternal(const Path<T>& path);
      Path<T> GetPath(OutPt2<T>*& op);

  protected:
      const Rect<T> rect_;
      const Path<T> rect_as_path_;
      const Point<T> rect_mp_;
      Rect<T> path_bounds_;
      std::deque<OutPt2<T>> op_container_;
      OutPt2List<T> results_; // each path can be broken into multiples
      OutPt2List<T> edges_[8]; // clockwise and counter-clockwise
      std::vector<Location> start_locs_;
      void CheckEdges();
      void TidyEdges(size_t idx, OutPt2List<T>& cw, OutPt2List<T>& ccw);
      void GetNextLocation(const Path<T>& path, Location& loc, size_t& i, size_t highI);
      OutPt2<T>* Add(Point<T> pt, bool start_new = false);
      void AddCorner(Location prev, Location curr);
      void AddCorner(Location& loc, bool isClockwise);

  public:
      explicit RectClip(const Rect<T>& rect)
          : rect_(rect)
          , rect_as_path_(rect.AsPath())
          , rect_mp_(rect.MidPoint())
      {
      }
      Paths<T> Execute(const Paths<T>& paths, const std::vector<Rect<T>>& bounds);
      Paths<T> Execute(const Paths<T>& paths);
  };

  //------------------------------------------------------------------------------
  // RectClipLines64
  //------------------------------------------------------------------------------
  template <typename T>
  class RectClipLines : public RectClip<T> {
  private:
      void ExecuteInternal(const Path<T>& path);
      Path<T> GetPath(OutPt2<T>*& op);

  protected:
      // since we are using a template we have to declare what we are using from parent class with the correct template type
      // otherwise we would have to call RectClip<T>::var each time in the respective functions
      using RectClip<T>::rect_;
      using RectClip<T>::rect_as_path_;
      using RectClip<T>::op_container_;
      using RectClip<T>::results_;
      using RectClip<T>::start_locs_;

      using RectClip<T>::Add;
      using RectClip<T>::GetNextLocation;

  public:
      explicit RectClipLines(const Rect<T>& rect)
          : RectClip<T>(rect) { };
      Paths<T> Execute(const Paths<T>& paths);
  };

} // Clipper2Lib namespace
#endif  // CLIPPER_RECTCLIP_H
