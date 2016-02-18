/* === S Y N F I G ========================================================= */
/*!	\file clamp.h
**	\brief Header file for implementation of the "Clamp" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2012-2013 Carlos López
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

#ifndef __SYNFIG_LAYER_SOLIDCOLOR_H
#define __SYNFIG_LAYER_SOLIDCOLOR_H

/* === H E A D E R S ======================================================= */

#include <synfig/layer.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig
{
namespace modules
{
namespace lyr_std
{

class Layer_Clamp : public Layer
{
	SYNFIG_LAYER_MODULE_EXT

private:
	//!Parameter: (bool)
	ValueBase param_invert_negative;
	//!Parameter: (bool)
	ValueBase param_clamp_ceiling;
	//!Parameter: (Real)
	ValueBase param_ceiling;
	//!Parameter: (Real)
	ValueBase param_floor;

	Color clamp_color(const Color &in)const;

public:

	Layer_Clamp();

	virtual bool set_param(const String & param, const ValueBase &value);

	virtual ValueBase get_param(const String & param)const;

	virtual Color get_color(Context context, const Point &pos)const;

	virtual Rect get_full_bounding_rect(Context context)const;

	virtual bool accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const;

	virtual Vocab get_param_vocab()const;
}; // END of class Layer_Clamp

}; // END of namespace lyr_std
}; // END of namespace modules
}; // END of namespace synfig

/* === E N D =============================================================== */

#endif
