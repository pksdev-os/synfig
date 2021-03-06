/* === S Y N F I G ========================================================= */
/*!	\file renderer_canvas.h
**	\brief Template Header
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**  ......... ... 2018 Ivan Mahonin
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_RENDERER_CANVAS_H
#define __SYNFIG_RENDERER_CANVAS_H

/* === H E A D E R S ======================================================= */

#include <climits>

#include <vector>
#include <map>

#include <glibmm/threads.h>

#include <synfig/time.h>
#include <synfig/rendering/task.h>

#include "workarearenderer.h"
#include "workarea.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

class Renderer_Canvas : public studio::WorkAreaRenderer
{
public:
	class FrameId {
	public:
		synfig::Time time;
		int width;
		int height;

		explicit FrameId(
			const synfig::Time &time = synfig::Time(),
			int width = 0,
			int height = 0
		):
			time(time), width(width), height(height) { }

		bool operator< (const FrameId &other) const {
			if (time < other.time) return true;
			if (other.time < time) return false;
			if (width < other.width) return true;
			if (other.width < width) return false;
			return height < other.height;
		}

		bool operator== (const FrameId &other) const
			{ return time == other.time && width == other.width && height == other.height; }

		bool operator!= (const FrameId &other) const
			{ return !(*this == other); }
	};

	class FrameDesc {
	public:
		FrameId id;
		synfig::ColorReal alpha;
		FrameDesc(): alpha() { }
		FrameDesc(
			const FrameId &id,
			synfig::ColorReal alpha ):
				id(id), alpha(alpha) { }
		FrameDesc(
			const synfig::Time &time,
			int width,
			int height,
			synfig::ColorReal alpha ):
				id(time, width, height), alpha(alpha) { }
	};

	class Tile: public etl::shared_object {
	public:
		typedef etl::handle<Tile> Handle;

		const long long refresh_id;
		const synfig::Time time;
		const synfig::RectInt rect;

		synfig::rendering::TaskEvent::Handle event;
		synfig::rendering::SurfaceResource::Handle surface;
		Cairo::RefPtr<Cairo::ImageSurface> cairo_surface;

		Tile(): refresh_id() { }
		Tile(int refresh_id, const synfig::Time &time, synfig::RectInt &rect):
			refresh_id(refresh_id), time(time), rect(rect) { }

		bool operator< (const Tile &other) const {
			if (refresh_id < other.refresh_id) return true;
			if (other.refresh_id < refresh_id) return false;
			return time < other.time;
		}
	};

	class TileLess {
	public:
		bool operator() (const Tile::Handle &a, const Tile::Handle &b)
			{ return a && b ? *a < *b : a < b; }
	};

	class TimeMeasure {
	public:
		long long time_us;
		long long cpu_time_us;
		TimeMeasure(): time_us(), cpu_time_us() { }
		static TimeMeasure now();
	};

	typedef std::vector<FrameDesc> FrameList;
	typedef std::vector<Tile::Handle> TileList;
	typedef std::multiset<Tile::Handle, TileLess> TileSet;
	typedef std::map<FrameId, TileSet> TileMap;

private:
	//! controls access to fields: tiles, onion_frames, refresh_id
	Glib::Threads::Mutex mutex;

	//! stored tiles may be actual/outdated and rendered/not-rendered
	TileMap tiles;

	//! all currently visible frames (onion skin feature allows to see more than one frame)
	FrameList onion_frames;

	//! increment of this field makes all tiles outdated
	long long refresh_id;

	//! require to call renderer after current rendering complete
	bool render_queued;

	TimeMeasure rendering_start_time;

	synfig::PixelFormat pixel_format;

	//! uses to normalize alpha value after blending of onion surfaces
	Cairo::RefPtr<Cairo::ImageSurface> alpha_src_surface;
	Cairo::RefPtr<Cairo::ImageSurface> alpha_dst_surface;
	Cairo::RefPtr<Cairo::Context> alpha_context;

	// don't try to pass arguments to callbacks by reference, it cannot be properly saved in signal
	// Renderer_Canvas is non-thread-safe sigc::trackable, so use static callback methods only in signals

	static void enqueue_rendering_task_callback(
		synfig::rendering::Renderer::Handle renderer,
		synfig::rendering::Task::Handle task,
		synfig::rendering::TaskEvent::Handle event );
	static void on_tile_finished_callback(bool success, Renderer_Canvas *obj, Tile::Handle tile);
	static void post_tile_finished_callback(etl::handle<Renderer_Canvas> obj);

	void on_tile_finished(bool success, const Tile::Handle &tile);
	void pre_tile_started();
	void post_tile_finished();

	void cancel_render(long long keep_refresh_id);

	void remove_old_tiles();

	Cairo::RefPtr<Cairo::ImageSurface> convert(
		const synfig::rendering::SurfaceResource::Handle &surface,
		int width, int height ) const;

public:
	Renderer_Canvas();
	~Renderer_Canvas();

	// functions to render canvas in background

	void inc_refresh_id();
	void enqueue_render(bool force = false);
	void wait_render();
	void cancel_render()
		{ cancel_render(LLONG_MAX); }

	// just paint already rendered tiles at window
	void render_vfunc(
		const Glib::RefPtr<Gdk::Window>& drawable,
		const Gdk::Rectangle& expose_area );
};

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
